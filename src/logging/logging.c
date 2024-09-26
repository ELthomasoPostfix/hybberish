#include "logging.h"

void logh(FILE *where, const char *fmt, ...) {
  // Based on https://en.cppreference.com/w/c/variadic

  va_list args;
  va_start(args, fmt);
  const char *const null = "NULL";

  while (*fmt != '\0') {
    char next = *(fmt + 1);

    // A '%' char could be the start of a custom format specifier.
    if (*fmt == '%' && next != '\0') {
      switch (next) {
      case 'E': {
        const ExpTree *exp = va_arg(args, ExpTree *);
        if (exp != NULL)
          printExpTree(exp, where);
        else
          fprintf(where, "%s", null);
        ++fmt; // A format specifier matches two chars!
        break;
      }
      case 'T': {
        const TaylorModel *tm = va_arg(args, TaylorModel *);
        if (tm != NULL)
          printTaylorModel(tm, where);
        else
          fprintf(where, "%s", null);
        ++fmt; // A format specifier matches two chars!
        break;
      }
      case 'I': {
        const Interval *interval = va_arg(args, Interval *);
        if (interval != NULL)
          printInterval(interval, where);
        else
          fprintf(where, "%s", null);
        ++fmt; // A format specifier matches two chars!
        break;
      }
      case 'D': {
        const Domain *domains = va_arg(args, Domain *);
        if (domains != NULL)
          printDomain(domains, where);
        else
          fprintf(where, "%s", null);
        ++fmt; // A format specifier matches two chars!
        break;
      }
      case 's': {
        const char *str = va_arg(args, char *);
        str = str != NULL ? str : null;
        fprintf(where, "%s", str);
        ++fmt; // A format specifier matches two chars!
        break;
      }
      case 'u': {
        const unsigned int num = va_arg(args, unsigned int);
        fprintf(where, "%u", num);
        ++fmt; // A format specifier matches two chars!
        break;
      }

      /* The next char does not match any known, custom format specified.
        So just print out the '%', it is a normal character. */
      default:
        fprintf(where, "%c", *fmt);
        break;
      }
      // All cases in the switch pass increment at the end of the loop!

      // Just log any other char.
    } else {
      fprintf(where, "%c", *fmt);
    }

    ++fmt;
  }

  va_end(args);
}

void printInterval(const Interval *const source, FILE *where) {
  assert(source != NULL);
  assert(where != NULL);
  fprintf(where, "[");
  fprintf(where, "%f", source->left);
  fprintf(where, ", ");
  fprintf(where, "%f", source->right);
  fprintf(where, "]");
}

void printDomain(const Domain *list, FILE *where) {
  assert(list->var != NULL);
  fprintf(where, "%s in ", list->var);

  printInterval(&list->domain, where);

  fprintf(where, "; ");
  if (list->next != NULL)
    printDomain(list->next, where);
}

void printValuation(const Valuation *list, FILE *where) {
  assert(list->var != NULL);
  fprintf(where, "%s = %f", list->var, list->val);

  fprintf(where, "; ");
  if (list->next != NULL)
    printValuation(list->next, where);
}

static void printBinOp(ExpType type, FILE *where) {
  switch (type) {
  case EXP_ADD_OP:
    fprintf(where, " + ");
    break;
  case EXP_SUB_OP:
    fprintf(where, " - ");
    break;
  case EXP_MUL_OP:
    fprintf(where, " * ");
    break;
  case EXP_DIV_OP:
    fprintf(where, " / ");
    break;
  case EXP_EXP_OP:
    fprintf(where, "^");
    break;
  default:
    assert(false);
  }
}

void printExpTree(const ExpTree *tree, FILE *where) {
  /* A simple depth-first search while printing in-order */
  assert(tree != NULL);
  assert(where != NULL);

  switch (tree->type) {
  /* binary operators */
  case EXP_ADD_OP:
  case EXP_SUB_OP:
  case EXP_MUL_OP:
  case EXP_DIV_OP:
  case EXP_EXP_OP:
    fprintf(where, "(");
    assert(tree->left != NULL);
    printExpTree(tree->left, where);
    printBinOp(tree->type, where);
    assert(tree->right != NULL);
    printExpTree(tree->right, where);
    fprintf(where, ")");
    break;
  /* unary operators */
  case EXP_NEG:
    fprintf(where, "-");
    assert(tree->left != NULL);
    printExpTree(tree->left, where);
    assert(tree->right == NULL);
    break;
  case EXP_FUN:
    assert(tree->data != NULL);
    fprintf(where, "%s(", tree->data);
    assert(tree->left != NULL);
    printExpTree(tree->left, where);
    fprintf(where, ")");
    assert(tree->right == NULL);
    break;
  /* base cases */
  case EXP_NUM:
  case EXP_VAR:
    assert(tree->data != NULL);
    fprintf(where, "%s", tree->data);
    assert(tree->left == NULL);
    assert(tree->right == NULL);
    break;
  default:
    assert(false);
  }
}

void printOdeList(ODEList *list, FILE *where) {
  assert(list->fun != NULL);
  fprintf(where, "%s' = ", list->fun);
  assert(list->exp != NULL);
  printExpTree(list->exp, where);
  fprintf(where, "; ");
  if (list->next != NULL)
    printOdeList(list->next, where);
}

void printTaylorModel(const TaylorModel *const list, FILE *where) {
  assert(list->fun != NULL);
  fprintf(where, "(p(%s) = ", list->fun);
  assert(list->exp != NULL);

  printExpTree(list->exp, where);
  fprintf(where, ", ");
  printInterval(&list->remainder, where);
  fprintf(where, ")");

  fprintf(where, "; ");
  if (list->next != NULL)
    printTaylorModel(list->next, where);
}
