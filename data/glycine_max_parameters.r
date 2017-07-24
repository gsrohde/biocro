# Do the calculations inside an empty list so that temporary variables are not created in .Global.
glycine_max_parameters = with(list(), {
    datalines =
    "symbol value
     timestep      1
     lat           40
     seneLeaf       950
     seneStem       1500
     seneRoot       5500
     seneRhizome    5500
     Tfrosthigh    5   
     Tfrostlow     0   
     leafdeathrate 5
     iSp                 2.5      
     SpD                0        
     nlayers            10       
     kd                 0.37     
     mrc1             0.02
     mrc2             0.03
     heightf       3        
     growth_respiration_fraction 0
     vmax1   111.2 
     alpha1  32.5
     jmax   213.2 
     Rd     1.1 
     Catm   370
     O2     210 
     b0     0.048
     b1     5   
     theta  0.7 
     water_stress_approach     1   
     tp1       400   
     tp2       600
     tp3       800
     tp4       1000
     tp5       1250
     tp6       1400  
     kStem1    0.3
     kLeaf1    0.25
     kRoot1    0.45
     kRhizome1 0
     kGrain1   0
     kStem2    0.05
     kLeaf2    0.65
     kRoot2    0.29
     kRhizome2 0.00 
     kGrain2   0.01
     kStem3    0.35
     kLeaf3    0.4
     kRoot3    0.1
     kRhizome3 0.0 
     kGrain3   0.15
     kStem4    0.3
     kLeaf4    0.24
     kRoot4    0.23
     kRhizome4 0.0
     kGrain4   0.23
     kStem5    0.16
     kLeaf5    0.01
     kRoot5    0.5
     kRhizome5 0
     kGrain5   0.33
     kStem6    0.0   
     kLeaf6    0.0
     kRoot6    0.0  
     kRhizome6 0.0  
     kGrain6   1
     rate_constant_leaf 0.1
     rate_constant_stem 1
     rate_constant_root 1
     rate_constant_rhizome 1
     rate_constant_grain 1
     KmLeaf     10  
     KmStem     10 
     KmRoot     10 
     KmRhizome  10 
     KmGrain    10  
     resistance_leaf_to_stem 9
     resistance_stem_to_grain 9
     resistance_stem_to_root 9
     resistance_stem_to_rhizome 9
     tbase     10     
     FieldC     -1       
     WiltP      -1       
     phi1       0.01     
     phi2       10       
     soilDepth  1        
     soilType   6        
     soilLayers 1        
     soilDepths1 0
     soilDepths2 1
     wsFun      0        
     scsf       1        
     transpRes  5e+06    
     leafPotTh  -800     
     hydrDist   0        
     rfl        0.2      
     rsec       0.2      
     rsdf       0.44   
     LeafN_0   2   
     kln      0.5 
     vmaxb1  0   
     alphab1 0   
     kpLN     0.2
     lnb0     -5  
     lnb1     18  
     lnfun    0
     electrons_per_carboxylation    4.5
     electrons_per_oxygenation 5.25"
    
    data_frame = read.table(textConnection(datalines), header=TRUE)
    values = as.list(data_frame$value)
    names(values) = data_frame$symbol
    values
})

