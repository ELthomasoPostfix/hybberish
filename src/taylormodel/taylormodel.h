/**
 * @file taylormodel.h
 * @author Thomas Gueutal (thomas.gueutal@student.uantwerpen.be)
 * @brief An interface for constructing and manipulating Taylor models.
 * @details This file encapsulates functions to construct Taylor model
 * objects and manipulate their contents.
 * @version 0.1
 * @date 2024-09-17
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef TAYLOR_MODEL_H
#define TAYLOR_MODEL_H

#include "exptree.h"
#include "interval.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief A vector of Taylor Models, as a linked list.
 * @details The models get derived from a system of ODEs. So, each model
 * corresponds to a single variable solved for within the system of ODEs.
 *
 * e.g. given as system of ODEs <br>
 *    ODE = \f$(x' = f_x(x, y, t), y' = f_y(x, y, t))\f$ <br>
 * the corresponding Taylor model linked list is of the form <br>
 *    TM = \f$("x", p_x(x, y, t), I_x) \rightarrow
 *            ("y", p_y(x, y, t), I_y)\f$. <br>
 * Here fx represents a vector field component, px a Taylor polynomial and Ix a
 * remainder interval, all of which correspond to the ODE variable/function x'.
 * Consequently, we call \f$("x", px(x, y, t), Ix)\f$ <br> the Taylor model
 * of x'.
 *
 * @invariant Both the member @ref TaylorModel.fun and the member
 * TaylorModel.exp may never be NULL. This is only guaranteed at construction if
 * the correct constructor method, @ref newTaylorModel, is used.
 */
typedef struct TaylorModel {
  /// @brief The name of the ODE variable this vector component corresponds to.
  char *fun;
  /// @brief The polynomial part of the Taylor mode.
  ExpTree *exp;
  /// @brief The remainder interval part of the Taylor model.
  Interval remainder;
  /// @brief The next component of the TaylorModel vector.
  struct TaylorModel *next;
} TaylorModel;

/**
 * @brief Create a new, single element list.
 * @pre Both \p fun and \p exp may **not** be NULL and must be heap-allocated.
 * @post Transfers ownership of all heap-allocated arguments to the
 * newly created TaylorModel instance.
 *
 * @param[in] fun       The ODE variable to which the Taylor model corresponds.
 * @param[in] exp       The polynomial part of the Taylor model.
 * @param[in] remainder The remainder part of the Taylor model.
 * @return TaylorModel* A heap-allocated Taylor model instance.
 */
TaylorModel *newTaylorModel(char *const fun, ExpTree *const exp,
                            const Interval remainder);

/**
 * @brief Attach the second element as the head of the first list.
 * @pre The \p head argument must be a single element list (not NULL).
 * @pre The \p tail argument must be NULL or heap-allocated.
 * @post Transfers ownership of \p tail to \p head. Ownership of
 * \p head is still the caller's.
 *
 * @param[in] tail The list to prepend a new head to.
 * @param[in] head The head to prepend to \p tail.
 * @return TaylorModel* The \p head pointer, with the tail attached.
 */
TaylorModel *appTMElem(TaylorModel *const tail, TaylorModel *head);

/**
 * @brief Allocate a new TaylorModel element and assign the passed member
 * values.
 * @details The newly created element will be prepended to the passed tail
 * list if it exists. Else, the new element will have an empty tail, making
 * the result a single element list. e.g. given <br>
 *     \p tail = \f$("y", p_y(x, y, t), I_y)\f$,
 * \p fun = "x", \p exp = \f$p_x(x, y, t)\f$ and \p remainder = \f$ I_x \f$,
 * the result is
 *     \f$("x", p_x(x, y, t), I_x) \rightarrow
 *        ("y", p_y(x, y, t), I_y)\f$. <br>
 *
 * @param[in] tail      The tail to prepend the newly created Taylor model
 *                      element to.
 * @param[in] fun       The ODE variable to which the new Taylor model element
 *                      corresponds.
 * @param[in] exp       The polynomial part of the new Taylor model element.
 * @param[in] remainder The remainder part of the new Taylor model element.
 * @return TaylorModel* A heap-allocated, new Taylor model element with the
 * given tail.
 */
TaylorModel *newTMElem(TaylorModel *const tail, char *const fun,
                       ExpTree *const exp, const Interval remainder);

/**
 * @brief Create a copy of the entire, given list.
 *
 * @param[in] list The list to copy.
 * @return TaylorModel* A heap-allocated copy of the list.
 */
TaylorModel *cpyTaylorModel(const TaylorModel *const list);

/**
 * @brief Create a copy of only the head of the given list. Ignore the tail.
 * @pre The given list may **not** be NULL.
 *
 * @return TaylorModel* A heap-allocated copy of the head of the list.
 * The result will always be a single element list.
 */
TaylorModel *cpyTaylorModelHead(const TaylorModel *const list);

/**
 * @brief Reverse the linked list in-place.
 *
 * @return TaylorModel* The pointer to the new head of the list.
 */
TaylorModel *reverseTaylorModel(TaylorModel *const list);

/**
 * @brief Deallocate the given list.
 * @pre The given list must not be NULL.
 */
void delTaylorModel(TaylorModel *list);

#endif
