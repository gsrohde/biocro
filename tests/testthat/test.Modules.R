context("Run test cases for all modules")

source("module_testing_helper_functions.R") # for cases_from_csv and test_module

# Define a helping function that tests a module unless it should be skipped
run_test <- function(module_name) {
    modules_to_skip <- c(
        # no module tests need to be skipped right now
    )

    if (module_name %in% modules_to_skip) {
        print(paste('Test of', module_name, 'needs fixing.'))
    } else {
        test_module(module_name, cases_from_csv(module_name))
    }
}

# Check test cases for all modules
lapply(
    get_all_modules(),
    run_test
)

# Make sure the `evaluate_module` function is properly reporting missing inputs
test_that("all module inputs must be defined when calling `evaluate_module`", {
    expect_error(
        evaluate_module('thermal_time_linear', list()),
        regexp = paste0(
             "The `thermal_time_linear` module requires `sowing_time` as an input quantity\n",
             "  The `thermal_time_linear` module requires `tbase` as an input quantity\n",
             "  The `thermal_time_linear` module requires `temp` as an input quantity\n",
             "  The `thermal_time_linear` module requires `time` as an input quantity\n"
        )
    )
})
