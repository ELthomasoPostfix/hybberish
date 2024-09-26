/**
 * @file exptree.h
 * @brief An interface for constructing and manipulating an expression tree.
 * @version 0.1
 * @date 2024-09-18
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef EXPTREE_H
#define EXPTREE_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

/**
 * @brief An enumeration of expression tree node types.
 */
typedef enum ExpType {
  EXP_NUM,    ///< A **leaf node** type: a number constant.
  EXP_VAR,    ///< A **leaf node** type: a variable.
  EXP_ADD_OP, ///< An **internal node** type: a binary add (a + b) operator.
  EXP_SUB_OP, ///< An **internal node** type: a binary sub (a - b) operator.
  EXP_MUL_OP, ///< An **internal node** type: a binary mul (a * b) operator.
  EXP_DIV_OP, ///< An **internal node** type: a binary div (a / b) operator.
  EXP_EXP_OP, ///< An **internal node** type: a binary exp (a ^ b) operator.
  EXP_NEG,    ///< An **internal node** type: a unary neg (- a) operator.
  EXP_FUN,    ///< An **internal node** type: an arbirary funtion.
} ExpType;

/**
 * @brief A binary expression tree node.
 * @details The node type has a large impact on the following aspects:
 *    - The NULL-ness or the valid values of the data member.
 *    - The NULL-ness or non-NULL-ness of the left and or right subtrees.
 *
 * Well-formedness of an expression tree node is only guaranteed if the
 * correct constructors are used, see @ref newExpLeaf, @ref newExpOp and
 * @ref newExpTree.
 */
typedef struct ExpTree {
  /// A field to store a node name or value, as a char array.
  char *data;
  /// The node type impacts requirements for the subtrees and data.
  ExpType type;
  struct ExpTree *left;  ///< The left child/subtree.
  struct ExpTree *right; ///< The right child/subtree.
} ExpTree;

/**
 * @brief The expression tree leaf node constructor.
 * @details All leaves must specify data. This function internally does
 * string duplication, which simplifies most calls to this function.
 * @pre \p name may **not** be NULL.
 * @post The ownership of the data input remains the caller's.
 *
 * @param[in] type The leaf's node type. Is restricted to leaf node types.
 * @param[in] name The leaf node data.
 * @return ExpTree* A newly heap-allocated leaf node.
 */
ExpTree *newExpLeaf(const ExpType type, const char *const name);

/**
 * @brief The expression tree internal (**operator**) node constructor.
 * @details Internal nodes are often operator nodes, meaning they do not
 * need to specify data. Their type is sufficiently informative.
 *
 * The term "operator" refers to mathematical operators, such as addition (+).
 * @pre All pointer arguments must be heap allocated or NULL. Though which
 * subtrees are allowed to be NULL is based on the operator's type.
 * @see ExpType
 * @post Transfers ownership of all heap-allocated arguments to the
 * newly created instance.
 * @post The node data is always NULL.
 *
 * @param[in] type  The node type to assign. Is restricted to internal types.
 * @param[in] left  The left subtree to assign.
 * @param[in] right The right subtree to assign.
 * @return ExpTree* A newly heap-allocated internal node.
 */
ExpTree *newExpOp(const ExpType type, ExpTree *left, ExpTree *right);

/**
 * @brief The expression tree internal (**general**) node constructor.
 * @details This constructor allows all node members to be specified.
 *
 * The term "general" implies that it is possible to construct operator as
 * well as other internal nodes using this constructor. Though, for operator
 * nodes, the @ref newExpOp constructor should be preferred instead.
 * @pre All pointer arguments must be heap allocated or NULL. Though which
 * subtrees are allowed to be NULL is based on the node's type.
 * @see ExpType
 * @post Transfers ownership of all heap-allocated arguments to the
 * newly created instance.
 *
 * @param[in] type  The node type to assign. Is restricted to internal types.
 * @param[in] name  The internal node data.
 * @param[in] left  The left subtree to assign.
 * @param[in] right The right subtree to assign.
 * @return ExpTree* A newly heap-allocated internal node.
 */
ExpTree *newExpTree(const ExpType type, char *name, ExpTree *left,
                    ExpTree *right);

/**
 * @brief Deallocate the given tree recursively.
 * @pre The given tree must not be NULL.
 */
void delExpTree(ExpTree *tree);

/**
 * @brief Make an exact, deep/recursive copy of the entire expression tree.
 * @details All node data is copied as well.
 *
 * @param[in] src The tree to copy.
 * @return ExpTree* A newly heap-allocated, exact copy.
 */
ExpTree *cpyExpTree(const ExpTree *const src);

#endif
