#ifndef MULTILAYER_C4_CANOPY_H
#define MULTILAYER_C4_CANOPY_H

#include "../state_map.h"
#include "multilayer_canopy_photosynthesis.h"
#include "multilayer_canopy_properties.h"
#include "c4_leaf_photosynthesis.h"

/**
 * @class ten_layer_c4_canopy
 *
 * @brief Represents a ten layer canopy where leaf-level photosynthesis is
 * calculated using the Collatz et al. model for C4 photosynthesis; see the
 * `c4_leaf_photosynthesis` class for more information about this model.
 *
 * More specifically, this is a child class of
 * `multilayer_canopy_photosynthesis` where:
 *
 *  - The canopy module is set to the `ten_layer_canopy_properties` module
 *
 *  - The leaf module is set to the `c4_leaf_photosynthesis` module
 *
 *  - The number of layers is set to 10
 *
 * Instances of this class can be created using the module factory, unlike the
 * parent class `multilayer_canopy_photosynthesis`.
 */
class ten_layer_c4_canopy : public multilayer_canopy_photosynthesis<ten_layer_canopy_properties, c4_leaf_photosynthesis>
{
   public:
    ten_layer_c4_canopy(
        state_map const* input_parameters,
        state_map* output_parameters)
        : multilayer_canopy_photosynthesis<ten_layer_canopy_properties, c4_leaf_photosynthesis>(
              "ten_layer_c4_canopy",
              ten_layer_c4_canopy::nlayers,
              input_parameters,
              output_parameters)
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();

   private:
    // Number of layers
    int static const nlayers;

    // Main operation
    void do_operation() const;
};

#endif
