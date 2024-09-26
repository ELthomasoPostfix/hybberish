/**
 * @file tm_arithmetic.h
 * @author Thomas Gueutal (thomas.gueutal@student.uantwerpen.be)
 * @brief A subset of order k Taylor model arithmetic.
 * @version 0.1
 * @date 2024-09-17
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef TAYLOR_MODEL_ARITHMETIC_H
#define TAYLOR_MODEL_ARITHMETIC_H

#include "utils.h"
#include "logging.h"
#include "variables.h"
#include "taylormodel.h"
#include "transformations.h"
#include "interval_arithmetic.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

/**
 * @brief Perform interval-valued expression evaluation via interval
 * arithmetic.
 * @details Interval evaluation consists of first substituting each variable
 * in the expression tree by the corresponding interval in the domains list.
 * Then evaluate the resulting interval expression via interval arithmetic.
 *
 * e.g. domains x &isin; [-1, 1], y &isin; [0, 2] and an expression
 * exp = y * (x + y). <br>
 * &rArr; eval(exp, domains)
 * = [0, 2] * ([-1, 1] + [0, 2])
 * = [0, 2] * [-1, 1.2]
 * = [-2, 2.4]
 * @pre Both \p tree and \p domains must **not** be NULL.
 *
 * @param[in] tree    The expression tree to evaluate via interval arithmetic.
 * @param[in] domains The mapping of expression variable to interval domain.
 * @return Interval The result of interval evaluation.
 */
Interval evaluateExpTree(const ExpTree *const tree,
                         const Domain *const domains);

/**
 * @brief Perform real-valued expression evaluation via real arithmetic.
 * @details Real evaluation consists of first substituting each variable
 * in the expression tree by the corresponding value in the valuation list.
 * Then evaluate the resulting expression via real arithmetic.
 *
 * e.g. valuations x = 1, y = 2 and an expression exp = y * (x + y). <br>
 * &rArr; eval(exp, valuations)
 * = 2 * (1 + 2) = 2 * 3 = 6
 * @pre Both \p tree and \p values must **not** be NULL.
 *
 * @param[in] tree   The expression tree to evaluate via real arithmetic.
 * @param[in] values The mapping of expression variable to exact valuation.
 * @return double The result of real evaluation.
 */
double evaluateExpTreeReal(const ExpTree *const tree,
                           const Valuation *const values);

/**
 * @brief Perform Taylor model valued expression evaluation via order k
 * Taylor model arithmetic.
 * @details Taylor model evaluation consists of first substituting each
 * variable in the expression tree by the corresponding model in the
 * Taylor model list. Then evaluate the resulting expression via
 * order k Taylor model arithmetic.
 *
 * This process happens analogously to the real-valued or interval-valued
 * expression evaluation cases.
 * @pre \p tree, \p list \p fun and \p variables must **not** be NULL.
 *
 * @param[in] tree      The expression tree to evaluate via TM arithmetic.
 * @param[in] list      The list of Taylor models to substitute the variables
 *                      in the expression tree by.
 * @param[in] fun       The ODE variable/function that the output Taylor model
 *                      adopts, regardless of which input models from \p list
 *                      are actually substituted into the expression.
 * @param[in] variables The mapping of expression variable to interval domain.
 *                      The domains of all variables are required for TM
 *                      arithmetic to be possible.
 * @param[in] k         The Taylor polynomial order to adhere to.
 * @return TaylorModel* The result of Taylor model evaluation.
 */
TaylorModel *evaluateExpTreeTM(const ExpTree *const tree,
                               const TaylorModel *const list,
                               const char *const fun,
                               const Domain *const variables,
                               const unsigned int k);

/**
 * @brief Binary TM addition, via order k TM arithmetic.
 * @details The operation is applied elementwise to the vector operands,
 * meaning: \f$ op(left, right)[i] = op(left[i], right[i]) \f$.
 * @pre Both operands must be lists of equal length.
 * @pre \p variables must **not** be NULL.
 *
 * @param[in] left      The left operand.
 * @param[in] right     The right operand.
 * @param[in] variables The mapping of expression variable to interval domain.
 * @param[in] k         The Taylor polynomial order to adhere to.
 * @return A newly heap-allocated Taylor model: \f$ left + right \f$.
 */
TaylorModel *addTM(const TaylorModel *const left,
                   const TaylorModel *const right,
                   const Domain *const variables, const unsigned int k);

/**
 * @brief Binary TM subtraction, via order k TM arithmetic.
 * @details The operation is applied elementwise to the vector operands,
 * meaning: \f$ op(left, right)[i] = op(left[i], right[i]) \f$.
 * @pre Both operands must be lists of equal length.
 * @pre \p variables must **not** be NULL.
 *
 * @param[in] left      The left operand, the minuend.
 * @param[in] right     The right operand, the subtrahend.
 * @param[in] variables The mapping of expression variable to interval domain.
 * @param[in] k         The Taylor polynomial order to adhere to.
 * @return A newly heap-allocated Taylor model: \f$ left - right\f$.
 */
TaylorModel *subTM(const TaylorModel *const left,
                   const TaylorModel *const right,
                   const Domain *const variables, const unsigned int k);

/**
 * @brief Binary TM multiplication, via order k TM arithmetic.
 * @details The operation is applied elementwise to the vector operands,
 * meaning: \f$ op(left, right)[i] = op(left[i], right[i]) \f$.
 * @pre Both operands must be lists of equal length.
 * @pre \p variables must **not** be NULL.
 *
 * @param[in] left      The left operand.
 * @param[in] right     The right operand.
 * @param[in] variables The mapping of expression variable to interval domain.
 * @param[in] k         The Taylor polynomial order to adhere to.
 * @return A newly heap-allocated Taylor model: \f$ left \times right \f$.
 */
TaylorModel *mulTM(const TaylorModel *const left,
                   const TaylorModel *const right,
                   const Domain *const variables, const unsigned int k);

/**
 * @brief Binary TM division, via order k TM arithmetic.
 * @details The operation is applied elementwise to the vector operands,
 * meaning: \f$ op(left, right)[i] = op(left[i], right[i]) \f$.
 * @pre Both operands must be lists of equal length.
 * @pre The denominator may **not** contain zero (0) in its enclosure:
 * for \f$ right = (p_r, I_r) \f$, it must hold that
 * \f$ 0 \notin Int((p_r, I_r)) = Int(p_r) + I_r \f$. Here \f$ p_r \f$
 * is the polynomial part, \f$ Int(p_r) \f$ is an interval enclosure
 * thereof and \f$ I_r \f$ is the remainder part of the denominator TM.
 * @pre \p variables must **not** be NULL.
 *
 * @param[in] left      The left operand; the dividend (numerator).
 * @param[in] right     The right operand; the divisor (denominator).
 * @param[in] variables The mapping of expression variable to interval domain.
 * @param[in] k         The Taylor polynomial order to adhere to.
 * @return A newly heap-allocated Taylor model: \f$ left \div right \f$.
 */
TaylorModel *divTM(const TaylorModel *const left,
                   const TaylorModel *const right,
                   const Domain *const variables, const unsigned int k);

/**
 * @brief Unary TM additive inverse (negation), via order k TM arithmetic.
 * @details The operation is applied elementwise to the vector operand,
 * meaning: \f$ op(list)[i] = op(list[i]) \f$.
 * @pre \p variables must **not** be NULL.
 *
 * @param[in] list      The operand.
 * @param[in] variables The mapping of expression variable to interval domain.
 * @param[in] k         The Taylor polynomial order to adhere to.
 * @return A newly heap-allocated Taylor model: \f$ - list \f$.
 */
TaylorModel *negTM(const TaylorModel *const list, const Domain *const variables,
                   const unsigned int k);

/**
 * @brief Binary TM exponentiation, via order k TM arithmetic.
 * @details The operation is applied elementwise to the vector operand,
 * meaning: \f$ op(left, right)[i] = op(left[i], right) \f$.
 * @pre \p variables must **not** be NULL.
 *
 * @param[in] left      The left operand; the base.
 * @param[in] right     The right operand; the exponent (power).
 * @param[in] variables The mapping of expression variable to interval domain.
 * @param[in] k         The Taylor polynomial order to adhere to.
 * @return A newly heap-allocated Taylor model: \f$ left ^ {right} \f$.
 */
TaylorModel *powTM(const TaylorModel *const left, const unsigned int right,
                   const Domain *const variables, const unsigned int k);

/**
 * @brief Definite TM integration, via order k TM arithmetic.
 * @details The operation is applied elementwise to the vector operand,
 * meaning: \f$ op(list)[i] = op(list[i]) \f$.
 * @pre \p variables must **not** be NULL.
 *
 * @param[in] list      The operand; the integrand.
 * @param[in] intDomain The bounds w.r.t. which to integrate, e.g. [a, b].
 * @param[in] intVar    The variable w.r.t. which to integrate, e.g. x.
 * @param[in] variables The mapping of expression variable to interval domain.
 * @param[in] k         The Taylor polynomial order to adhere to.
 * @return A newly heap-allocated Taylor model: \f$ \int_a^b ( list )dx \f$.
 */
TaylorModel *intTM(const TaylorModel *const list,
                   const Interval *const intDomain, const char *const intVar,
                   const Domain *const variables, const unsigned int k);

/**
 * @brief Truncate the given Taylor model to order k.
 * @details Truncating to order k implies:
 *   - retraining all terms of order <= k in the polynomial part
 *   - dropping all terms of order > k from the polynomial part
 *   - adding an enclosure of the dropped terms to the remainder interval,
 *     carrying onward the dropped terms as added an uncertainty or error.
 * @pre \p variables must **not** be NULL.
 * @post The truncated expression is also transformed to a normal form.
 *
 * @param[in] list      The list of Taylor models to each truncate to order k.
 * @param[in] variables The mapping of expression variable to interval domain.
 * @param[in] k         The truncation order; the Taylor polynomial order
 *                      to adhere to.
 * @return A newly heap-allocated Taylor model: \f$ Trunc_k(list) \f$.
 */
TaylorModel *truncateTM(const TaylorModel *const list,
                        const Domain *const variables, const unsigned int k);

#endif