/**
 * @file funexp.h
 * @brief Functions to transform an expression tree,
 * motivated by mathematical relations or properties.
 * @version 0.1
 * @date 2024-09-18
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef FUNEXP_H
#define FUNEXP_H

#include "exptree.h"
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

/**
 * @brief Compute the partial derivative expression w.r.t. the given variable.
 * @details computes \f$ \frac{ \partial f }{ \partial x } \f$ where f
 * is the differentiand and x the differentiation variable.
 *
 * @param[in] expr The expression to derive.
 * @param[in] var  The differentiation variable.
 * @return ExpTree* A newly heap-allocated, partial derivative expression
 * of the input.
 */
ExpTree *derivative(const ExpTree *expr, const char *var);

/**
 * @brief Compute the indefinite integral expression w.r.t. the given variable.
 * @details computes \f$ \int f(x) dx \f$ where f(x) is the integrand and x the
 * integration variable.
 *
 * @param[in] expr The expression to integrate; the integrand.
 * @param[in] var  The integration variable.
 * @return ExpTree* A newly heap-allocated, integral expression of the input.
 */
ExpTree *integral(const ExpTree *expr, const char *var);

/**
 * @brief Compute the definite integral expression w.r.t. the given variable
 * and domain.
 * @details computes \f$ \int_a^b f(x) dx \f$ where f(x) is the integrand and x
 * the integration variable.
 *
 * @param[in] expr   The expression to integrate; the integrand.
 * @param[in] var    The integration variable.
 * @param[in] lowerBound The lower bound of the integration domain.
 * @param[in] upperBound The upper bound of the integration domain.
 * @return ExpTree* A newly heap-allocated, definite integral expression
 * of the input.
 */
ExpTree *definiteIntegral(const ExpTree *expr, const char *var,
                          const ExpTree *lowerBound, const ExpTree *upperBound);

/**
 * @brief Verify if the expression is linear.
 *
 * @return true  iff. the expression is linear.
 * @return false else.
 */
bool isLinear(const ExpTree *expr);

/**
 * @brief Verify the exact equality of the given trees.
 * @details Both structure and content (data) must match exactly.
 *
 * Algebraic equivalence is not equality: (1 + x) =\\= (x + 1).
 *
 * Content refers to the data of nodes: (1 + y) =\\= (1 + x).
 *
 * @return true  iff. both trees match exactly.
 * @return false else.
 */
bool isEqual(const ExpTree *expr1, const ExpTree *expr2);

/**
 * @brief Compute the degree of the given monomial expression.
 * @details A monomial is a polynomial of only a single term.
 * The expression can only contain multiplication (*) unary negative (-)
 * and exponentiation (^) with non-negative powers.
 * @pre The expression must be of monomial form.
 *
 * @param[in] expr The monomial expression.
 * @return unsigned int The degree of the expression.
 */
unsigned int degreeMonomial(const ExpTree *expr);

/**
 * @brief A forward declaration of @ref substitute in @ref transformations.h.
 */
ExpTree *substitute(const ExpTree *source, const char *var,
                    const ExpTree *target);

#endif
