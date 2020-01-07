#ifndef PARTITIONING_COEFFICIENT_LOGISTIC_H
#define PARTITIONING_COEFFICIENT_LOGISTIC_H

#include <cmath>
#include  <iostream>
#include "../modules.h"

class partitioning_coefficient_logistic : public SteadyModule {
	public:
		partitioning_coefficient_logistic(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			SteadyModule("partitioning_coefficient_logistic"),
			// Get pointers to input parameters
//			TTemr_ip(get_ip(input_parameters, "TTemr")),
//            TTveg_ip(get_ip(input_parameters, "TTveg")),
//            TTrep_ip(get_ip(input_parameters, "TTrep")),
            DVI_ip(get_ip(input_parameters,"DVI")),
            alphaRoot_ip(get_ip(input_parameters, "alphaRoot")),
            alphaStem_ip(get_ip(input_parameters, "alphaStem")),
            alphaLeaf_ip(get_ip(input_parameters, "alphaLeaf")),
            betaRoot_ip(get_ip(input_parameters, "betaRoot")),
            betaStem_ip(get_ip(input_parameters, "betaStem")),
            betaLeaf_ip(get_ip(input_parameters, "betaLeaf")),
//            TTc_ip(get_ip(input_parameters, "TTc")),
            kRhizome_emr_ip(get_ip(input_parameters, "kRhizome_emr")),
    
			// Get pointers to output parameters
			kStem_op(get_op(output_parameters, "kStem")),
			kLeaf_op(get_op(output_parameters, "kLeaf")),
			kRoot_op(get_op(output_parameters, "kRoot")),
			kRhizome_op(get_op(output_parameters, "kRhizome")),
			kGrain_op(get_op(output_parameters, "kGrain"))
    {}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
//		const double* TTemr_ip;
//		const double* TTveg_ip;
//		const double* TTrep_ip;
        const double* DVI_ip;
		const double* alphaRoot_ip;
        const double* alphaStem_ip;
		const double* alphaLeaf_ip;
		const double* betaRoot_ip;
		const double* betaStem_ip;
		const double* betaLeaf_ip;
//		const double* TTc_ip;
        const double* kRhizome_emr_ip;
		
		// Pointers to output parameters
		double* kStem_op;
		double* kLeaf_op;
		double* kRoot_op;
		double* kRhizome_op;
		double* kGrain_op;
//        double* DVI_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> partitioning_coefficient_logistic::get_inputs() {
	return {
		"DVI", "alphaRoot", "alphaStem", "alphaLeaf",
		"betaRoot", "betaStem", "betaLeaf", "kRhizome_emr"
	};
}

std::vector<std::string> partitioning_coefficient_logistic::get_outputs() {
	return {
		"kStem", "kLeaf", "kRoot", "kRhizome", "kGrain"
	};
}

void partitioning_coefficient_logistic::do_operation() const {
//	double TTc = *TTc_ip; // Thermal time accumulated, degrees C * day
//	double TTemr = *TTemr_ip; // Thermal time until emergence, degrees C * day
//	double TTveg = *TTveg_ip; // Thermal time of vegetative stage, degrees C * day
//	double TTrep = *TTrep_ip; // Thermal time of reproductive stage, degrees C * day
    const double DVI = *DVI_ip; // dimensionless; Development Index
	double kStem, kLeaf, kRoot, kRhizome, kGrain, kDenom;
    
//    // Determine the DVI (development index) based on the accumulated thermal time
//    // -1<=DVI<0 is before crop emergence, 0<=DVI<1 is during the vegetative stage, and 1<=DVI<2 is the reproductive stage
//    // DVI=2 is considered the end of the growing season
//    // from Osborne et al., 2015 JULES-crop doi:10.5194/gmd-8-1139-2015
//    if (0.0 <= TTc && TTc < TTemr) {
//        DVI = -1.0 + TTc / TTemr; //dimensionless
//    }
//    else if (TTemr <= TTc && TTc < (TTveg + TTemr)) {
//        DVI = (TTc - TTemr) / TTveg; //dimensionless
//    }
//    else if ((TTveg + TTemr) <= TTc) {
//        DVI = 1.0 + (TTc - TTemr - TTveg) / TTrep; //dimensionless
//    }
//    else {
//        DVI = 5.0;
// //        throw std::out_of_range(std::string("TTc not in range, thrown by partitioning_coefficient_logistic module.\n TTc="));
//    }
    
    // Determine partitioning coefficients using multinomial logistic equations from Osborne et al., 2015 JULES-crop doi:10.5194/gmd-8-1139-2015
    kDenom = exp(*alphaRoot_ip + *betaRoot_ip * DVI) + exp(*alphaLeaf_ip + *betaLeaf_ip * DVI) + exp(*alphaStem_ip + *betaStem_ip * DVI) + 1.0; // denomiator term for kRoot, kStem, kLeaf, and kGrain, dimensionless
    kRoot = exp(*alphaRoot_ip + *betaRoot_ip * DVI) / kDenom; // dimensionless
    kStem = exp(*alphaStem_ip + *betaStem_ip * DVI) / kDenom; // dimensionless
    kLeaf = exp(*alphaLeaf_ip + *betaLeaf_ip * DVI) / kDenom; // dimensionless
    kGrain = 1.0 / kDenom; // dimensionless
    
    // Give option for rhizome to contribute to growth during the emergence stage, kRhizome_emr is an input parameter and should be non-positive
    // To ignore rhizome, set kRhizome_emr to 0 in input parameter file, and adjust initial leaf, stem, and root biomasses accordingly
    if (DVI<0) {
        kRhizome = *kRhizome_emr_ip; //dimensionless
    }
    else kRhizome = 0.0; // dimensionless
    
    
	// Update the output parameters
	update(kStem_op, kStem); //dimensionless
	update(kLeaf_op, kLeaf); //dimensionless
	update(kRoot_op, kRoot); //dimensionless
	update(kRhizome_op, kRhizome); //dimensionless
	update(kGrain_op, kGrain); //dimensionless
}

#endif
