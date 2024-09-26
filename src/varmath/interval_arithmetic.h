/**
 * @file interval_arithmetic.h
 * @author Thomas Gueutal (thomas.gueutal@student.uantwerpen.be)
 * @brief A basic implementation of interval arithmetic and a few interval
 * properties.
 * @version 0.1
 * @date 2024-09-16
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef INTERVAL_ARITHMETIC_H
#define INTERVAL_ARITHMETIC_H

#include "interval.h"
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

/**
 * @brief Binary interval addition.
 * @details Interval addition **is** commutative: I1 + I2 = I2 + I1.
 *
 * @param[in] left  The left operand.
 * @param[in] right The right operand.
 * @return \p left + \p right = [a, b] + [c, d] = [a + c, b + d]
 */
Interval addInterval(const Interval *const left, const Interval *const right);

/**
 * @brief Binary interval subtraction.
 * @details Interval subtraction is generally **not** commutative:
 * I1 - I2 != I2 - I1.
 *
 * @param[in] left  The left operand; the minuend.
 * @param[in] right The right operand; the subtrahend.
 * @return \p left - \p right = [a, b] - [c, d] = [a - d, b - c]
 */
Interval subInterval(const Interval *const left, const Interval *const right);

/**
 * @brief Binary interval multiplication.
 * @details Interval multiplication **is** commutative: I1 * I2 = I2 * I1.
 *
 * @param[in] left  The left operand.
 * @param[in] right The right operand.
 * @return \p left * \p right = [a, b] * [c, d] = [ min{ac, ad, bc, bd}, max{ac,
 * ad, bc, bd} ]
 */
Interval mulInterval(const Interval *const left, const Interval *const right);

/**
 * @brief Binary interval division.
 * @details Interval division is generally **not** commutative:
 * I1 / I2 != I2 / I1.
 * @pre The \p right operand must **not** contain zero (0).
 *
 * @param[in] left  The left operand; the dividend (numerator).
 * @param[in] right The right operand; the divisor (denominator).
 * @return \p left / \p right = [a, b] / [c, d] = [a, b] * [1/d, 1/c]
 */
Interval divInterval(const Interval *const left, const Interval *const right);

/**
 * @brief Unary interval additive inverse (negation).
 *
 * @param[in] source The operand.
 * @return - \p source = -[a, b] = [-b, -a]
 */
Interval negInterval(const Interval *const source);

/**
 * @brief Unary interval the square root (sqrt).
 * @pre The interval's lower bound must be greater than or equal to 0,
 * meaning the interval does **not** contain any negative values.
 *
 * @param[in] source The operand; the radicand.
 * @return sqrt( \p source ) = sqrt([a, b]) = [sqrt(a), sqrt(b)]
 */
Interval sqrtInterval(const Interval *const source);

/**
 * @brief Binary interval exponentiation using a ***smart*** algorithm.
 * @details The smart algorithm takes into account that the exponentiation can
 * be applied elementwise to the interval (set), allowing for a tighter
 * bounding of the exponentiation result.
 *
 * @param[in] source  The left operand; the base.
 * @param[in] exponent The right operand; the exponent (power).
 * @return \p source ^ \p exponent = [a, b]^n
 */
Interval powInterval(const Interval *const source, const unsigned int exponent);

/**
 * @brief Binary interval exponentiation using a ***naive*** algorithm.
 * @details The naive algorithm simply unrolls the exponentiation into a
 * sequence of multiplications, resulting in a less accurate bounding of
 * the exponentiation result than the smart algorithm of @ref powInterval.
 *
 * @param[in] source  The left operand; the base.
 * @param[in] exponent The right operand; the exponent (power).
 * @return \p source ^ \p exponent = [a, b]^n
 */
Interval pow2Interval(const Interval *const source,
                      const unsigned int exponent);

/**
 * @brief Binary interval equality checking.
 * @details Interval equality **is** commutative: (I1 = I2) = (I2 = I1).
 *
 * @param[in] left    The left operand.
 * @param[in] right   The right operand.
 * @param[in] epsilon An epsilon, to use as an equality threshold. So, two
 *                    bounds of two intervals are equal iff. they differ at
 *                    most \p epsilon.
 * @return true  if \p left is equal to \p right.
 * @return false else.
 */
bool eqInterval(const Interval *const left, const Interval *const right,
                const double epsilon);

/**
 * @brief Binary interval subset or equal checking.
 * @details The subseteq operation is generally **not** commutative:
 * (I1 subseteq I2) != (I2 subseteq I1)
 *
 * @param[in] left  The left operand.
 * @param[in] right The right operand.
 * @return true  if \p left is a subset of or equal to \p right.
 * @return false else.
 */
bool subeqInterval(const Interval *const left, const Interval *const right);

/**
 * @brief Binary interval membership checking.
 *
 * @param[in] elem   The element to check membership in \p source for.
 * @param[in] source The set to check membership of \p elem within.
 * @return true  if \p elem is contained inside \p source.
 * @return false else.
 */
bool elemInterval(const double elem, const Interval *const source);

/**
 * @brief Compute the width of an interval.
 *
 * @param[in] source The interval, [a, b].
 * @return Width([a, b]) = b - a
 */
double intervalWidth(const Interval *const source);

/**
 * @brief Compute the midpoint of an interval.
 *
 * @param[in] source The interval, [a, b].
 * @return Mid([a, b]) = (a + b) / 2
 */
double intervalMidpoint(const Interval *const source);

/**
 * @brief Compute the magnitude of an interval.
 *
 * @param[in] source The interval, [a, b].
 * @return Mag([a, b]) = max{|a|, |b|}
 */
double intervalMagnitude(const Interval *const source);

/**
 * @brief Check if the interval is degenerate.
 * @pre The epsilon **must** be >= 0.
 *
 * @param[in] source  The interval, [a, b].
 * @param[in] epsilon An epsilon, to use as an equality threshold. So, the two
 *                bounds of the interval are equal iff. they differ at most
 *                \p epsilon.
 * @return true  if |b - a| < \p epsilon
 * @return false else.
 */
bool intervalIsDegenerate(const Interval *const source, const double epsilon);

#endif