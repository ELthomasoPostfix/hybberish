/**
 * @file logging.h
 * @author Thomas Gueutal (thomas.gueutal@student.uantwerpen.be)
 * @brief The hybberish logging header. Defines public macros to write
 * to the log stream, in addition to print functionality for multiple
 * structs.
 * @version 0.1
 * @date 2024-09-23
 *
 * @copyright Copyright (c) 2024
 *
 */

/// @brief The set of macros that facilitate optional logging.
#ifndef LOG_MACROS_HYBBERISH_H
#define LOG_MACROS_HYBBERISH_H

#include "exptree.h"
#include "interval.h"
#include "sysode.h"
#include "taylormodel.h"
#include "variables.h"
#include <stdarg.h>
#include <stdio.h>

/**
 * @brief The level of logging that the application should perform at runtime.
 */
typedef enum LogLevel {
  LOG_NONE, ///< No logging; nothing gets logged.
  LOG_COMP, ///< Compact logging; only final results get logged.
  LOG_VERB, ///< Verbose logging; intermediate results get logged.
} LogLevel;

/// @brief The globally accessible logging level of the hybberish tool.
extern LogLevel logLevelHybberish;

/* These are "private" macros, and should normally not be used.
 */
/// @brief The stream the logger writes to.
#define LOG_STREAM stderr
/// @brief The math environment delimiter.
#define LOG_DMATH "\\`"
/// @brief Print delimeters around the given code fragment if toggle is true.
#define LOG_DELIM(where, toggle, delim, code)                                  \
  if (toggle) {                                                                \
    fprintf(where, delim);                                                     \
    code fprintf(where, delim);                                                \
  } else {                                                                     \
    code                                                                       \
  }

/* These are "public" macros, and should be used for logging.
 */
/// @brief Print to the log using a format specifier. Allows Hybberish structs.
#define LOG_FMT(level, fmt, ...)                                               \
  if (level == logLevelHybberish) {                                            \
    fprintf(LOG_STREAM, "LOG: ");                                              \
    logh(LOG_STREAM, fmt, __VA_ARGS__);                                        \
    fprintf(LOG_STREAM, "\n");                                                 \
    fflush(LOG_STREAM);                                                        \
  }
/// @brief Log a Taylor model as a separate log line.
#define LOG_TM(level, tm) LOG_FMT(level, "%T", tm)
/// @brief Print a char* message to the log stream with prefix and a newline.
#define LOG_LINE(level, msg) LOG_FMT(level, "%s", msg)

/**
 * @brief Print a string representation of an interval to a stream.
 *
 * @param[in]     source The interval to print.
 * @param[in,out] where  The destination stream.
 */
void printInterval(const Interval *const source, FILE *where);

/**
 * @brief Print a representation of the given list to the specified stream.
 * @pre Neither the given \p list nor stream ( \p where ) may both be NULL.
 *
 * @param[in] list  The list to print.
 * @param[in] where The stream (destination) to print to.
 */
void printDomain(const Domain *list, FILE *where);

/**
 * @brief Print a representation of the given list to the specified stream.
 * @pre Neither the given \p list nor stream ( \p where ) may both be NULL.
 *
 * @param[in] list  The list to print.
 * @param[in] where The stream (destination) to print to.
 */
void printValuation(const Valuation *list, FILE *where);

/**
 * @brief Determine an operator precedence value for use in grouping subtrees
 * using parentheses in algebra expressions.
 * @details In most cases, an expression of node type B should be enclosed by
 * parentheses if it is of strictly lower precedence than its parent of node
 * type A.
 *
 * Intuitively, in a * b + c the multiplication "steals" the "b" operand from
 * the addition, so we require parentheses to instead enforce a * (b + c).
 * This is because the multiplication has higher operator precedence than the
 * addition.
 *
 * In some cases, nodes with the same precedence value also require grouping,
 * either for correctness or clarity.
 * For example: a - (b - c) != a - b - c, so grouping ensures correctness.
 * Or: a / b / c != a / (b / c), because a / b / c is usually implicitly
 * grouped left-to-right as ((a / b) / c), so grouping ensures correctness.
 * Or: a / b / c == (a / b) / c, so parentheses simply make the expression
 * more explicit for added clarity.
 *
 * @param type The type to determine the precedence value/ranking for.
 * @return unsigned int The precedence value.
 */
unsigned int parenthesisPrecedence(const ExpType type);

/**
 * @brief Print a representation of the given subtree, based on a known parent.
 * @details Make use of operator precedence rules defined by
 * @ref parenthesisPrecedence to decide if the subtree's expression should be
 * grouped using parentheses or not. For example, given a multiplication parent
 * node and an addition subtree node, the addition needs to always be grouped.
 * Observe: a * (b + c) != a * b + c. Here the parent is a * (b + c), while the
 * subtree is (b + c).
 *
 * Some operators may need to group same-precedence operands.
 * Same-precedence grouping must be explicitly allowed using
 * \p allowSamePrecedence.
 *
 * @param parent              The parent, based on which to decide grouping.
 * @param subtree             The tree to print.
 * @param where               The stream (destination) to print to.
 * @param allowSamePrecedence If false, then only strictly lower (<) precedence
 *                            results in grouping.
 *                            If true, then lower or equal precedence results
 *                            in grouping.
 */
void printSubTree(const ExpTree *parent, const ExpTree *subtree, FILE *where,
                  const bool allowSamePrecedence);

/**
 * @brief Print a representation of the given tree to the specified stream.
 * @pre Neither the given \p tree nor stream ( \p where ) may be NULL.
 *
 * @param[in] tree  The tree to print.
 * @param[in] where The stream (destination) to print to.
 */
void printExpTree(const ExpTree *tree, FILE *where);

/**
 * @brief Print a representation of the given list to the specified stream.
 * @pre Neither the given \p list nor stream ( \p where ) may be NULL.
 *
 * @param[in] list  The list to print.
 * @param[in] where The stream (destination) to print to.
 */
void printOdeList(const ODEList *list, FILE *where);

/**
 * @brief Print a representation of the given list to the specified stream.
 * @pre Neither the given \p list nor stream ( \p where ) may be NULL.
 *
 * @param[in] list  The list to print.
 * @param[in] where The stream (destination) to print to.
 */
void printTaylorModel(const TaylorModel *const list, FILE *where);

/**
 * @brief Log variadic Hyberrish structs and strings.
 * @details The format specifier list may contain any character.
 * Certain special sequences are used to specify Hyberrish structs or
 * strings. All other characters of the format string are simply logged:
 *
 * Specifier Substring |  Parameter Type  | Is Pointer Type?
 * :-----------------: | :--------------- | :--------------:
 *        \%E          | ExpTree *        |       YES
 *        \%O          | ODEList *        |       YES
 *        \%T          | TaylorModel *    |       YES
 *        \%I          | Interval *       |       YES
 *        \%D          | Domain *         |       YES
 *        \%s          | char *           |       YES
 *        \%u          | unsigned int     |       NO
 *
 * Many variadic parameters are required to be a pointer type.
 * NULL pointers are simply logged as the string "NULL".
 * See the details for which specifier should or should not be a pointer type.
 *
 * Certain parameter-less format specifiers are also allowed, i.e. you should
 * never pass a parameter for such a specifier. They are used to print known
 * constants to the log. The following table specifies them.
 *
 * | Specifier Substring |  Description
 * | :-----------------: | :-----------
 * |        \%M          |  The math environment delimiter. Manually specify
 * |                     |  a math environment in the format string.
 *
 * For example, "%I + %I" can be used to log a string resembling a sum of
 * two intervals. The following python-esque pseudocode gives an example.
 * Note, care should be taken to pass pointer types where required, the
 * pseudocode does *not* consider this!
 *
 *     >>> I1 = [1, 2]
 *     >>> I2 = [-2, -1]
 *     >>> lineNr = 3
 *     >>> logh(stdout, "LINE %u: sum = %I + %I", lineNr, I1, I2)
 *     "LINE 3: sum = [1, 2] + [-2, -1]"
 *
 * @param[in, out] where The destination stream to log to.
 * @param[in]      fmt   The format specifier string. See the details section.
 * @param[in]      ...   The variadic parameters to log. All parameters must be
 *                       pointer types.
 */
void logh(FILE *where, const char *fmt, ...);

#endif
