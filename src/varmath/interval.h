/**
 * @file interval.h
 * @author Thomas Gueutal (thomas.gueutal@student.uantwerpen.be)
 * @brief An interface for constructing an interval.
 * @version 0.1
 * @date 2024-09-16
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef INTERVAL_H
#define INTERVAL_H

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

/**
 * @brief A class representing a closed interval I = [a, b], so that a <= b.
 *
 * @details Note, since intervals are really just convex sets of numbers,
 * interval arithmetic operations are set operations in disguise. So,
 * intuitively, unary interval arithmetic operations can generally be applied
 * elementwise, while binary interval arithmetic operations can generally be
 * applied pairwise to the input interval(s) to compute the output interval.
 *
 * Interval arithmetic is over-approximate. So the result of any interval
 * arithmetic operation can only expected to be a bound on the true result,
 * rather than the true result exactly.
 *
 * @invariant The **bounds restriction** a <= b is an invariant that is only
 * guaranteed at construction if the correct constructor method,
 * @ref newInterval, is used. All interval arithmetic operations also
 * safeguard this invariant.
 */
typedef struct Interval {
  double left;
  double right;
} Interval;

/**
 * @brief Construct a new interval that respects the bounds invariant.
 * @see Interval For the bounds invariant description.
 *
 * @param[in] left  The left/lower bound.
 * @param[in] right The right/upper bound.
 * @return Interval
 */
Interval newInterval(const double left, const double right);

#endif
