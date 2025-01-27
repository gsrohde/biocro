# This tests some basic properties of system construction

context("Test error conditions during system validation and construction")

MAX_INDEX <- 100
VERBOSE <- FALSE

test_that("drivers must not be empty", {
    expect_error(
        run_biocro(drivers = data.frame()),
        regexp = "The drivers cannot be empty"
    )

    expect_error(
        validate_dynamical_system_inputs(
            drivers = data.frame(),
            verbose = VERBOSE
        ),
        regexp = "The drivers cannot be empty"
    )
})

test_that("certain run_biocro inputs must have the correct type", {
    expect_error(
        run_biocro(
            unlist(miscanthus_x_giganteus_initial_values),
            unlist(miscanthus_x_giganteus_parameters),
            unlist(get_growing_season_climate(weather2005)),
            miscanthus_x_giganteus_direct_modules,
            miscanthus_x_giganteus_differential_modules,
            unlist(miscanthus_x_giganteus_ode_solver['type'])
        ),
        regexp = paste0(
            "`initial_values` must be a list.\n",
            "  `parameters` must be a list.\n",
            "  `ode_solver` must be a list.\n",
            "  `drivers` must be a data frame.\n"
        )
    )
})

test_that("certain run_biocro inputs must not have empty elements", {
    expect_error(
        run_biocro(
            within(miscanthus_x_giganteus_initial_values, {bad_initial_value = numeric(0)}),
            within(miscanthus_x_giganteus_parameters, {bad_parameter = numeric(0)}),
            get_growing_season_climate(weather2005),
            within(miscanthus_x_giganteus_direct_modules, {bad_direct_module = character(0)}),
            within(miscanthus_x_giganteus_differential_modules, {bad_differential_module = character(0)}),
            within(miscanthus_x_giganteus_ode_solver, {bad_ode_solver_setting = numeric(0)})
        ),
        regexp = paste0(
            "The following `initial_values` members have lengths other than 1, but all members must have a length of exactly 1: bad_initial_value.\n",
            "  The following `parameters` members have lengths other than 1, but all members must have a length of exactly 1: bad_parameter.\n",
            "  The following `direct_module_names` members have lengths other than 1, but all members must have a length of exactly 1: bad_direct_module.\n",
            "  The following `differential_module_names` members have lengths other than 1, but all members must have a length of exactly 1: bad_differential_module.\n",
            "  The following `ode_solver` members have lengths other than 1, but all members must have a length of exactly 1: bad_ode_solver_setting.\n"
        )
    )
})

test_that("certain run_biocro inputs must not have elements with length > 1", {
    expect_error(
        run_biocro(
            within(miscanthus_x_giganteus_initial_values, {bad_initial_value = c(1,2)}),
            within(miscanthus_x_giganteus_parameters, {bad_parameter = c(1,2)}),
            get_growing_season_climate(weather2005),
            miscanthus_x_giganteus_direct_modules,
            miscanthus_x_giganteus_differential_modules,
            within(miscanthus_x_giganteus_ode_solver, {bad_ode_solver_setting = c(1,2)})
        ),
        regexp = paste0(
            "The following `initial_values` members have lengths other than 1, but all members must have a length of exactly 1: bad_initial_value.\n",
            "  The following `parameters` members have lengths other than 1, but all members must have a length of exactly 1: bad_parameter.\n",
            "  The following `ode_solver` members have lengths other than 1, but all members must have a length of exactly 1: bad_ode_solver_setting.\n"
        )
    )
})

test_that("certain run_biocro inputs must be strings", {
    expect_error(
        run_biocro(
            miscanthus_x_giganteus_initial_values,
            miscanthus_x_giganteus_parameters,
            get_growing_season_climate(weather2005),
            append(miscanthus_x_giganteus_direct_modules, 1.23),
            append(miscanthus_x_giganteus_differential_modules, 4.56),
            within(miscanthus_x_giganteus_ode_solver, {type = 7.89})
        ),
        regexp = paste0(
            "The following `direct_module_names` members are not strings, but all members must be strings: 1.23.\n",
            "  The following `differential_module_names` members are not strings, but all members must be strings: 4.56.\n",
            "  The following `ode_solver_type` members are not strings, but all members must be strings: 7.89.\n"
        )
    )
})

test_that("certain run_biocro inputs must be numeric", {
    expect_error(
        run_biocro(
            within(miscanthus_x_giganteus_initial_values, {bad_initial_value = "terrible"}),
            within(miscanthus_x_giganteus_parameters, {bad_parameter = "awful"}),
            within(get_growing_season_climate(weather2005), {bad_driver = "offensive"}),
            miscanthus_x_giganteus_direct_modules,
            miscanthus_x_giganteus_differential_modules,
            within(miscanthus_x_giganteus_ode_solver, {bad_ode_solver_setting = "heinous"})
        ),
        regexp = paste0(
            "The following `initial_values` members are not numeric or NA, but all members must be numeric or NA: bad_initial_value.\n",
            "  The following `parameters` members are not numeric or NA, but all members must be numeric or NA: bad_parameter.\n",
            "  The following `drivers` members are not numeric or NA, but all members must be numeric or NA: bad_driver.\n",
            "  The following `ode_solver_other_than_type` members are not numeric or NA, but all members must be numeric or NA: bad_ode_solver_setting.\n"
        )
    )
})

test_that("All modules must exist", {
    # Set up some bad inputs

    initial_values <- list(
        unused_initial_value = 0
    )

    parameters <- list(
        unused_parameter = 0
    )

    drivers <- data.frame(
        unused_varying_parameter=rep(0, MAX_INDEX)
    )

    direct_module_names <- c("module_that_does_not_exist")

    differential_module_names <- c()

    # This should throw an error

    expect_error(validate_dynamical_system_inputs(initial_values, parameters, drivers, direct_module_names, differential_module_names, verbose=VERBOSE))

})

test_that("Duplicated quantities produce an error during validation", {
    # Set up some bad inputs for a harmonic oscillator

    initial_values <- list(
        position = 1,   # defined for the first time
        velocity = 0
    )

    parameters <- list(
        mass = 1,
        spring_constant = 1,
        timestep = 1,
        position = 1    # defined for the second time
    )

    drivers <- data.frame(
        doy=rep(0, MAX_INDEX),
        hour=seq(from=0, by=1, length=MAX_INDEX)
    )

    direct_module_names <- c("harmonic_energy")

    differential_module_names <- c("harmonic_oscillator")

    # Validation should return FALSE

    expect_false(validate_dynamical_system_inputs(initial_values, parameters, drivers, direct_module_names, differential_module_names, verbose=VERBOSE))

})

test_that("Missing inputs produce an error during validation", {
    # Set up some bad inputs for a harmonic oscillator

    initial_values <- list(
        position = 1,
        velocity = 0
    )

    parameters <- list(
        mass = 1,
        #spring_constant = 1,   # not defined
        timestep = 1
    )

    drivers <- data.frame(
        doy=rep(0, MAX_INDEX),
        hour=seq(from=0, by=1, length=MAX_INDEX)
    )

    direct_module_names <- c("harmonic_energy")

    differential_module_names <- c("harmonic_oscillator")

    # Validation should return FALSE

    expect_false(validate_dynamical_system_inputs(initial_values, parameters, drivers, direct_module_names, differential_module_names, verbose=VERBOSE))

})

test_that("Differential modules only supply derivatives for quantities in the initial values", {
    # Set up some bad inputs for a harmonic oscillator

    initial_values <- list(
        position = 1
    )

    parameters <- list(
        mass = 1,
        spring_constant = 1,
        timestep = 1,
        velocity = 0    # should be in the initial values instead
    )

    drivers <- data.frame(
        doy=rep(0, MAX_INDEX),
        hour=seq(from=0, by=1, length=MAX_INDEX)
    )

    direct_module_names <- c("harmonic_energy")

    differential_module_names <- c("harmonic_oscillator")

    # Validation should return FALSE

    expect_false(validate_dynamical_system_inputs(initial_values, parameters, drivers, direct_module_names, differential_module_names, verbose=VERBOSE))

})

test_that("Direct modules are not required to be supplied in the correct order", {
    # Set up some bad inputs

    initial_values <- list(
        unused_initial_value = 0
    )

    parameters <- list(
        unused_parameter = 0
    )

    drivers <- data.frame(
        unused_varying_parameter=rep(0, MAX_INDEX)
    )

    direct_module_names <- c("Module_1", "Module_2")

    differential_module_names <- c()

    # This should be valid

    expect_true(validate_dynamical_system_inputs(initial_values, parameters, drivers, direct_module_names, differential_module_names, verbose=VERBOSE))

    # If we change the module order, it should still be valid

    direct_module_names <- c("Module_2", "Module_1")

    expect_true(validate_dynamical_system_inputs(initial_values, parameters, drivers, direct_module_names, differential_module_names, verbose=VERBOSE))

})
