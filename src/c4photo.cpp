/*
 *  /src/c4photo.c by Fernando Ezequiel Miguez  Copyright (C) 2007-2010
 *
 *
 *  Part of this code (see function c4photoC) is based on the C4 photo of 
 *  WIMOVAC. WIMOVAC is copyright of Stephen P Long and Stephen Humphries.
 *  Documentation for WIMOVAC can be found at
 *  http://www.life.illinois.edu/plantbio/wimovac/ (checked 02-13-2010)
 *
 *  The ballBerry code is based on code provided by Joe Berry in an e-mail.
 *  The original function was written in Fortran. I translated it to C.
 * 
 */

#include <math.h>
#include "c4photo.h"

struct c4_str c4photoC(double Qp, double Tl, double RH, double vmax, double alpha, 
		       double kparm, double theta, double beta,
		       double Rd, double bb0, double bb1, double StomaWS, double Ca, int water_stress_approach, double upperT, double lowerT)
{

	const double AP = 101325; /*Atmospheric pressure According to wikipedia (Pa)*/
    const double P = AP / 1e3; /* kPa */
	const double Q10 = 2;  /* Q10 increase in a reaction by 10 C temp */

	double M;
	double Assim;

	double Gs;
	double OldAssim = 0.0, Tol = 0.1;

	double Csurface = (Ca * 1e-6) * AP;
	double InterCellularCO2 = Csurface * 0.4; /* Assign an initial guess. */
	double KQ10 =  pow(Q10, (Tl - 25.0) / 10.0);
	double kT = kparm * KQ10;

    // Collatz 1992. Appendix B. Equation set 5B.
    double Vtn = vmax * pow(2, (Tl - 25.0) / 10.0);
    double Vtd = (1 + exp(0.3 * (lowerT - Tl))) * (1 + exp(0.3 * (Tl - upperT)));
    double VT  = Vtn / Vtd;

	// Collatz 1992. Appendix B. Equation set 5B.
	double Rtn = Rd * pow(2, (Tl - 25) / 10);
	double Rtd =  1 + exp(1.3 * (Tl - 55));
	double RT = Rtn / Rtd; 

	// Collatz 1992. Appendix B. Equation 2B.
	double b0 = VT * alpha * Qp;
	double b1 = VT + alpha * Qp;
	double b2 = theta;

	/* Calculate the 2 roots */
	double M1 = (b1 + sqrt(b1 * b1 - (4 * b0 * b2))) / (2 * b2);
	double M2 = (b1 - sqrt(b1 * b1 - (4 * b0 * b2))) / (2 * b2);

	/* This piece of code selects the smallest root */
	if(M1 < M2)
		M = M1;
	else
		M = M2;

	int iterCounter = 0;
	while (iterCounter < 50) {

		double kT_IC_P = kT * (InterCellularCO2 / P * 1000);
		double a = M * kT_IC_P;
		double b = M + kT_IC_P;
		double c = beta;

		double a2 = (b - sqrt(b * b - (4 * a * c))) / (2 * c);

		Assim = a2 - RT;

		if (water_stress_approach == 0) Assim *= StomaWS; 

		/* milimole per meter square per second*/
		double csurfaceppm = Csurface * 10;

		/* Need to create the Ball-Berry function */
		Gs =  ballBerry(Assim, csurfaceppm, Tl, RH, bb0, bb1);
		if (water_stress_approach == 1) Gs *= StomaWS; 

		InterCellularCO2 = Csurface - (Assim * 1e-6 * 1.6 * AP) / (Gs * 0.001);

		if (InterCellularCO2 < 0)
			InterCellularCO2 = 1e-5;

		double diff = OldAssim - Assim;
		if (diff < 0) diff = -diff;

		if (diff < Tol) {
			break;
		} else {
			OldAssim = Assim;
		}

		++iterCounter;
	}

	double miC = (InterCellularCO2 / AP) * 1e6;

	if(Gs > 600)
	  Gs = 600;

	struct c4_str tmp;
    tmp.Assim = Assim;
    tmp.Gs = Gs;
    tmp.Ci = miC;
    tmp.GrossAssim=Assim + RT;
    return(tmp);
}


/* Ball Berry stomatal conductance function */
double ballBerry(double Amu, double Cappm, double Temp, double RelH, double beta0, double beta1)
{

	const double gbw = 1.2; /* According to Collatz et al. (1992) pg. 526*/
	const double ptotPa = 101325; /* Atmospheric pressure */

	double pwaPa, leafTk, Camf, assimn;
	double wa, pwi, wi, gswmol, Cs, acs;
	double aaa, bbb, ccc, ddd;
	double gsmol;
	double hs;

	leafTk = Temp + 273.15;
	pwi = fnpsvp(leafTk);
	pwaPa = RelH * pwi;
	Camf = Cappm * 1e-6;
	assimn = Amu * 1e-6;
  
	/* Calculate mole fraction (mixing ratio) of water vapor in */
	/* atmosphere from partial pressure of water in the atmosphere and*/
	/* the total air pressure */
	wa  = pwaPa / ptotPa;
	/* Get saturation vapor pressure for water in the leaf based on */
	/* current idea of the leaf temperature in Kelvin*/
	/* Already calculated above */
	/* Calculate mole fraction of water vapor in leaf interior */
	wi  = pwi / ptotPa;

	if(assimn < 0.0){
		/* Set stomatal conductance to the minimum value, beta0*/
		gswmol = beta0;
		/* Calculate leaf surface relative humidity, hs, (as fraction)*/
		/* for when C assimilation rate is <= 0*/
		/* hs = (beta0 + (wa/wi)*gbw)/(beta0 + gbw); ! unused here??*/
	}
	else{
		/* leaf surface CO2, mole fraction */
		Cs  = Camf - (1.4/gbw)*assimn;
		if(Cs < 0.0)
			Cs = 1;
		/* Constrain the ratio assimn/cs to be > 1.e-6*/
		acs = assimn/Cs;

		if(acs < 1e-6) 	acs = 1e-6;		

		/* Calculate leaf surface relative humidity, hs, from quadratic */
		/* equation: aaa*hs^2 + bbb*hs + ccc = 0 */
		/*  aaa= beta1 * assimn / cs */
		aaa = beta1 * acs;
		/*      bbb= beta0 + gbw - (beta1 * assimn / cs)*/
		bbb = beta0 + gbw - (beta1 * acs);
		ccc = -(wa / wi) * gbw - beta0;

		/* Solve the quadratic equation for leaf surface relative humidity */
		/* (as fraction) */
		ddd = bbb * bbb - 4*aaa*ccc;

		hs  = (-bbb + sqrt(ddd)) / (2* aaa);

		/* Ball-Berry equation (Collatz'91, eqn 1) */
		gswmol = beta1 * hs * acs + beta0;
	}
	gsmol = gswmol * 1000; /* converting to mmol */

	if(gsmol <= 0) gsmol = 1e-5;

	return(gsmol);
}


double fnpsvp(double Tkelvin){
	/* water boiling point = 373.16 oK*/
/* This is the Arden Buck Equation 
http://en.wikipedia.org/wiki/Arden_Buck_equation
 */
	double u, v;
	double tmp, esat;

	tmp = Tkelvin - 273.15;
	u = (18.678 - tmp/234.5)*tmp;
	v = 257.14 + tmp;
	esat = 6.1121 * exp(u/v);
	esat /= 10;

	return(esat);
}

