#ifndef SE_SOLVER_H
#define SE_SOLVER_H

#include <memory>  // for std::shared_ptr
#include <vector>
#include "se_solver_helper_functions.h"
#include "simultaneous_equations.h"

/**
 * @class se_observer_null
 * 
 * @brief An observer to be used when the user doesn't specify one. This observer doesn't do anything.
 */
struct se_observer_null {
    se_observer_null() {}
    void operator()(std::vector<double> const& /*uq*/, bool /*adjustment_made*/) {}
};

/**
 * @class se_solver
 * 
 * @brief An abstract class for a generic simultaneous equation solver. Its `solve` member function provides
 * a uniform interface for all derived solvers, and its constructor defines inputs that are common to all
 * solvers.
 */
class se_solver
{
   public:
    se_solver(
        std::string solver_name,
        double rel_error_tol,
        double abs_error_tol,
        int max_it) : solver_name(solver_name),
                      rel_error_tolerance(rel_error_tol),
                      abs_error_tolerance(abs_error_tol),
                      max_iterations(max_it) {}

    // Make the destructor a pure virtual function to indicate that this class should be abstract
    virtual ~se_solver() = 0;
    
    template <typename observer_type>
    bool solve(
        std::shared_ptr<simultaneous_equations> const& se,
        std::vector<double> const& initial_guess_for_root,
        std::vector<double> const& lower_bounds,
        std::vector<double> const& upper_bounds,
        std::vector<double>& final_value_for_root,
        observer_type observer);
    
    bool solve(
        std::shared_ptr<simultaneous_equations> const& se,
        std::vector<double> const& initial_guess_for_root,
        std::vector<double> const& lower_bounds,
        std::vector<double> const& upper_bounds,
        std::vector<double>& final_value_for_root)
        {
            return solve(se, initial_guess_for_root, lower_bounds, upper_bounds, final_value_for_root, se_observer_null());
        }

    std::string generate_info_report() const;
    std::string generate_solve_report() const;

   private:
    const std::string solver_name;

    const double rel_error_tolerance;
    const double abs_error_tolerance;
    const int max_iterations;

    bool solve_method_has_been_called = false;
    bool converged_abs;
    bool converged_rel;
    int num_iterations;
    int num_adjustments;

    virtual std::vector<double> get_next_guess(
        std::shared_ptr<simultaneous_equations> const& se,
        std::vector<double> const& input_guess) = 0;

    virtual std::vector<double> adjust_bad_guess(
        std::vector<double> const& bad_guess,
        std::vector<double> const& lower_bounds,
        std::vector<double> const& upper_bounds);
};

// A destructor must be defined, and since the default is overwritten when defining it as pure virtual, add an inline one in the header
inline se_solver::~se_solver()
{
}

/**
 * @brief Tries to find a root, starting at the initial guess and staying within the bounds.
 * 
 * @param[in] se
 * 
 * @param[in] initial_guess_for_root
 * 
 * @param[in] lower_bounds
 * 
 * @param[in] upper_bounds
 * 
 * @param[out] final_value_for_root
 * 
 * @param[in] observer an object with an operator()(std::vector<double> const& uq, bool adjustment_made)
 *                     method. This method will be called at each step of the solution process.
 * 
 * @return indicates whether or not the procedure was successful in finding a root.
 * 
 * The solver proceeds by steps, in which a new guess for the root is determined from the previous guess
 * using the `get_next_guess` method, which must be implemented by derived classes. When a new guess is
 * found, it is first checked to see if it lies within the acceptable bounds. If it lies outside the bounds,
 * it is adjusted with the `adjust_bad_guess` method, which has a default behavior but can be overridden by
 * derived classes. If the new guess doesn't need an adjustment, it is compared to the previous guess to
 * check for convergence using either an absolute or relative threshold. The process ends when convergence
 * occurs or when the number of iterations exceeds a threshold value.
 */
template <typename observer_type>
bool se_solver::solve(
    std::shared_ptr<simultaneous_equations> const& se,
    std::vector<double> const& initial_guess_for_root,
    std::vector<double> const& lower_bounds,
    std::vector<double> const& upper_bounds,
    std::vector<double>& final_value_for_root,
    observer_type observer)
{
    // Reset all the status indicators
    solve_method_has_been_called = true;
    converged_abs = false;
    converged_rel = false;
    num_iterations = 0;
    num_adjustments = 0;
    
    // Pass the first guess to the observer
    observer(initial_guess_for_root, false);

    // Initialize temporary variables for the loop
    std::vector<double> previous_guess = initial_guess_for_root;
    std::vector<double> next_guess = initial_guess_for_root;
    bool need_to_make_adjustment;

    // Define a lambda to simplify error checking
    auto errors_occurred = [](std::vector<bool> error_vector) -> bool {
        int num_errors = std::accumulate(error_vector.begin(), error_vector.end(), 0);
        return num_errors > 0;
    };

    // Define a lambda to simplify convergence checking
    auto check_convergence = [&errors_occurred](std::vector<bool> convergence_error_vector, bool* convergence_achieved) -> void {
        if (!errors_occurred(convergence_error_vector)) {
            *convergence_achieved = true;
        }
    };

    // Run the loop
    do {
        ++num_iterations;

        next_guess = get_next_guess(se, previous_guess);

        // Check to see if the new guess is acceptable.
        // If it is, check to see if we have reached convergence.
        need_to_make_adjustment = errors_occurred(is_outside_bounds(next_guess, lower_bounds, upper_bounds));
        
        if (need_to_make_adjustment) {
            ++num_adjustments;
            next_guess = adjust_bad_guess(next_guess, lower_bounds, upper_bounds);
        } else {
            std::map<std::vector<bool>, bool*> convergence_checks = {
                {has_not_converged_abs(previous_guess, next_guess, abs_error_tolerance),    &converged_abs},
                {has_not_converged_rel(previous_guess, next_guess, rel_error_tolerance),    &converged_rel}
            };

            for (auto const& x : convergence_checks) {
                check_convergence(x.first, x.second);
            }
        }
        
        observer(next_guess, need_to_make_adjustment);
        
        previous_guess = next_guess;

    } while (num_iterations < max_iterations && !converged_abs && !converged_rel);

    final_value_for_root = next_guess;

    return converged_abs || converged_rel;
}

#endif