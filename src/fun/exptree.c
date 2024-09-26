#include "exptree.h"

ExpTree *newExpLeaf(const ExpType type, const char *const name) {
  /* Leaves are fully dependent on their data, so it must be specified. */
  assert(name != NULL);

  ExpTree *tree = (ExpTree *)malloc(sizeof(ExpTree));
  tree->data = strdup(name);
  tree->type = type;
  switch (type) {
  case EXP_NUM:
    tree->left = NULL;
    tree->right = NULL;
    break;
  case EXP_VAR:
    tree->left = NULL;
    tree->right = NULL;
    break;
  default:
    assert(false);
    return NULL;
  }
  return tree;
}

ExpTree *newExpOp(const ExpType type, ExpTree *left, ExpTree *right) {
  return newExpTree(type, NULL, left, right);
}

ExpTree *newExpTree(const ExpType type, char *name, ExpTree *left,
                    ExpTree *right) {
  ExpTree *tree = (ExpTree *)malloc(sizeof(ExpTree));
  tree->data = name;
  tree->type = type;
  switch (type) {
  /* binary operators */
  case EXP_ADD_OP:
  case EXP_SUB_OP:
  case EXP_MUL_OP:
  case EXP_DIV_OP:
  case EXP_EXP_OP:
    assert(left != NULL);
    tree->left = left;
    assert(right != NULL);
    tree->right = right;
    break;
  /* unary operators */
  case EXP_NEG:
  case EXP_FUN:
    assert(left != NULL);
    tree->left = left;
    assert(right == NULL);
    tree->right = right;
    break;
  default:
    assert(false);
    return NULL;
  }
  return tree;
}

void delExpTree(ExpTree *tree) {
  assert(tree != NULL);

  /* A simple depth-first search while freeing nodes post-order */
  if (tree->left != NULL)
    delExpTree(tree->left);
  if (tree->right != NULL)
    delExpTree(tree->right);
  /* Base case: delete the current node */
  if (tree->data != NULL)
    free(tree->data);
  free(tree);
}

ExpTree *cpyExpTree(const ExpTree *const src) {
  if (src == NULL) {
    return NULL;
  }

  ExpTree *copy = (ExpTree *)malloc(sizeof(ExpTree));
  if (copy == NULL) {
    fprintf(stderr, "Memory allocation error\n");
    exit(EXIT_FAILURE);
  }

  copy->type = src->type;
  copy->data = (src->data != NULL) ? strdup(src->data) : NULL;

  /* recursively copy the left tree node */
  copy->left = cpyExpTree(src->left);
  /* recursively copy the right tree node */
  copy->right = cpyExpTree(src->right);

  return copy;
}
