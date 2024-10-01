#include "logging.h"

void logh(FILE *where, const char *fmt, ...) {
  // Based on https://en.cppreference.com/w/c/variadic

  va_list args;
  va_start(args, fmt);
  const char *const null = "NULL";
  /* Keep track of when the format specifier string manually
    opens a math environment. This is useful to dynamically
    enclose varargs in math environments as needed. */
  bool notInMathEnv = true;

  while (*fmt != '\0') {
    char next = *(fmt + 1);

    // A '%' char could be the start of a custom format specifier.
    if (*fmt == '%' && next != '\0') {

      switch (next) {
      case 'E': {
        const ExpTree *exp = va_arg(args, ExpTree *);
        if (exp != NULL)
          LOG_DELIM(where, notInMathEnv, LOG_DMATH, printExpTree(exp, where);)
        else
          fprintf(where, "%s", null);
        ++fmt; // A format specifier matches two chars!
        break;
      }
      case 'O': {
        const ODEList *sys = va_arg(args, ODEList *);
        if (sys != NULL)
          LOG_DELIM(where, notInMathEnv, LOG_DMATH, fprintf(where, "(");
                    printOdeList(sys, where); fprintf(where, ")");)
        else
          fprintf(where, "%s", null);
        ++fmt; // A format specifier matches two chars!
        break;
      }
      case 'T': {
        const TaylorModel *tm = va_arg(args, TaylorModel *);
        if (tm != NULL)
          LOG_DELIM(where, notInMathEnv, LOG_DMATH, fprintf(where, "(");
                    printTaylorModel(tm, where); fprintf(where, ")");)
        else
          fprintf(where, "%s", null);
        ++fmt; // A format specifier matches two chars!
        break;
      }
      case 'I': {
        const Interval *interval = va_arg(args, Interval *);
        if (interval != NULL)
          LOG_DELIM(where, notInMathEnv, LOG_DMATH,
                    printInterval(interval, where);)
        else
          fprintf(where, "%s", null);
        ++fmt; // A format specifier matches two chars!
        break;
      }
      case 'D': {
        const Domain *domains = va_arg(args, Domain *);
        if (domains != NULL)
          LOG_DELIM(where, notInMathEnv, LOG_DMATH, fprintf(where, "(");
                    printDomain(domains, where); fprintf(where, ")");)
        else
          fprintf(where, "%s", null);
        ++fmt; // A format specifier matches two chars!
        break;
      }
      case 'u': {
        const unsigned int num = va_arg(args, unsigned int);
        LOG_DELIM(where, notInMathEnv, LOG_DMATH, fprintf(where, "%u", num);)
        ++fmt; // A format specifier matches two chars!
        break;
      }
      // Allow users to manually add math environment delimiters.
      // This gives great flexibility, but this can break the
      // rendering really easily if not carefully used!
      case 'M': {
        notInMathEnv = !notInMathEnv;
        fprintf(where, "%s", LOG_DMATH);
        ++fmt; // A format specifier matches two chars!
        break;
      }
      case 's': {
        const char *str = va_arg(args, char *);
        if (str != NULL)
          fprintf(where, "%s", str);
        else
          fprintf(where, "%s", null);
        ++fmt; // A format specifier matches two chars!
        break;
      }
      /* The next char does not match any known, custom format specified.
        So just print out the '%', it is a normal character. */
      default: {
        fprintf(where, "%c", *fmt);
        break;
      }
      }

      // All cases in the switch pass increment at the end of the loop!

      // Just log any other char.
    } else {
      fprintf(where, "%c", *fmt);
    }

    ++fmt;
  }

  /* There is an unclosed math environment in the format string! */
  if (!notInMathEnv)
    assert(false);

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
  fprintf(where, "(%s ,in, ", list->var);

  printInterval(&list->domain, where);
  fprintf(where, ")");

  if (list->next != NULL) {
    fprintf(where, ", ");
    printDomain(list->next, where);
  }
}

void printValuation(const Valuation *list, FILE *where) {
  assert(list->var != NULL);
  fprintf(where, "(%s ,=, %f)", list->var, list->val);

  if (list->next != NULL) {
    fprintf(where, ", ");
    printValuation(list->next, where);
  }
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

unsigned int parenthesisPrecedence(const ExpType type) {
  switch (type) {
  case EXP_ADD_OP:
  case EXP_SUB_OP:
    return 1;
  case EXP_MUL_OP:
  case EXP_DIV_OP:
    return 2;
  case EXP_NEG:
    return 3;
  /* EXP; must only be lower in precedence than leaves, to because enclosing
    anything except leaves with parentheses ensures correctness. */
  case EXP_EXP_OP:
    return 4;
  /* Leaves: leaves have highest precedence.
     Functions: in this context, functions can be treated as leaves. */
  case EXP_FUN:
  case EXP_NUM:
  case EXP_VAR:
    return 5;

  /* Unknown operator type. */
  default:
    assert(false);
    break;
  }
}

void printSubTree(const ExpTree *parent, const ExpTree *subtree, FILE *where,
                  const bool allowSamePrecedence) {
  assert(parent != NULL);
  assert(subtree != NULL);
  assert(where != NULL);

  const unsigned int parentPrec = parenthesisPrecedence(parent->type);
  const unsigned int childPrec = parenthesisPrecedence(subtree->type);

  /* If a subtree's node type has lower precedence than the parent's node type,
    then parentheses around the child are required to maintain the grouping of
    the subtree expression.
    The same can optionally be enforced when the parent and subtree have the
    same precedence. */
  if ((childPrec < parentPrec) ||
      ((childPrec == parentPrec) && allowSamePrecedence)) {
    fprintf(where, "(");
    printExpTree(subtree, where);
    fprintf(where, ")");
  } else
    printExpTree(subtree, where);
}

void printExpTree(const ExpTree *tree, FILE *where) {
  /* A simple depth-first search while printing in-order. */
  assert(tree != NULL);
  assert(where != NULL);

  switch (tree->type) {
  /* binary operators */
  case EXP_ADD_OP:
  case EXP_SUB_OP:
  case EXP_MUL_OP:
  case EXP_DIV_OP:
  case EXP_EXP_OP:
    assert(tree->left != NULL);
    assert(tree->right != NULL);

    /* If an operand of a division has the same precedence, so that parent
      and subtree have the same precedence, then group that operand to ensure
      correctness. */
    bool parentIsDiv = tree->type == EXP_DIV_OP;
    /* If the right operand of a subtraction has the same precedence, so that
      parent and subtree have the same precedence, then group that operand
      to ensure correctness. */
    bool parentIsSub = tree->type == EXP_SUB_OP;

    bool allowSamePrecL = parentIsDiv;
    bool allowSamePrecR = parentIsDiv || parentIsSub;
    printSubTree(tree, tree->left, where, allowSamePrecL);
    printBinOp(tree->type, where);
    printSubTree(tree, tree->right, where, allowSamePrecR);
    break;
  /* unary operators */
  case EXP_NEG:
    assert(tree->left != NULL);
    assert(tree->right == NULL);

    fprintf(where, "-");
    printSubTree(tree, tree->left, where, false);
    break;
  case EXP_FUN:
    assert(tree->data != NULL);
    assert(tree->left != NULL);
    assert(tree->right == NULL);

    /* Print parentheses here, the recursive call should never print any. */
    fprintf(where, "%s(", tree->data);
    printSubTree(tree, tree->left, where, false);
    fprintf(where, ")");
    break;
  /* base cases */
  case EXP_NUM:
  case EXP_VAR:
    assert(tree->data != NULL);
    assert(tree->left == NULL);
    assert(tree->right == NULL);
    fprintf(where, "%s", tree->data);
    break;
  default:
    assert(false);
  }
}

void printOdeList(const ODEList *list, FILE *where) {
  assert(list->fun != NULL);
  fprintf(where, "(%s' ,=, ", list->fun);
  assert(list->exp != NULL);

  printExpTree(list->exp, where);
  fprintf(where, ")");

  if (list->next != NULL) {
    fprintf(where, ", ");
    printOdeList(list->next, where);
  }
}

void printTaylorModel(const TaylorModel *const list, FILE *where) {
  assert(list->fun != NULL);
  fprintf(where, "(p(%s),=, ", list->fun);
  assert(list->exp != NULL);

  printExpTree(list->exp, where);
  fprintf(where, "\",\", ");
  printInterval(&list->remainder, where);
  fprintf(where, ")");

  if (list->next != NULL) {
    fprintf(where, ", ");
    printTaylorModel(list->next, where);
  }
}
