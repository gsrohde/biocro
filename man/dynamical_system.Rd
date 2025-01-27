\name{dynamical_system}

\alias{dynamical_system}
\alias{validate_dynamical_system_inputs}

\title{Validating dynamical system inputs}

\description{
  Utility function for checking inputs to \code{run_biocro} without running it
}

\usage{
validate_dynamical_system_inputs(
    initial_values = list(),
    parameters = list(),
    drivers,
    direct_module_names = list(),
    differential_module_names = list(),
    verbose = TRUE
)
}

\arguments{
  \code{validate_dynamical_system_inputs} accepts the same input arguments as
  \code{\link{run_biocro}} with the exceptions of \code{ode_solver} (which is
  not required to check the validity of a dynamical system).
}

\details{
  \code{validate_system_inputs} checks a set of parameters, drivers, modules,
  and initial values to see if they can properly define a dynamical system and
  can therefore be used as inputs to \code{\link{run_biocro}}. Although the
  \code{\link{run_biocro}} function performs the same validity checks, the
  \code{validate_system_inputs} includes additional information, such as a list
  of parameters whose values are not used as inputs by any modules, since in
  principle these parameters could be removed for clarity.
}

\value{
  A boolean indicating whether or not the inputs are valid
}

\seealso{\code{\link{run_biocro}}}

\examples{
# Example 1: missing a parameter and an initial value
validate_dynamical_system_inputs(
  within(soybean_initial_values, rm(Leaf)),         # remove the initial `Leaf` value
  within(soybean_parameters, rm(leaf_reflectance)), # remove `leaf_reflectance`
  soybean_weather2002,
  soybean_direct_modules,
  soybean_differential_modules
)

# Example 2: a valid set of input arguments
validate_dynamical_system_inputs(
  soybean_initial_values,
  soybean_parameters,
  soybean_weather2002,
  soybean_direct_modules,
  soybean_differential_modules
)
}
