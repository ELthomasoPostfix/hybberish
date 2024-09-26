#include "taylormodel.h"

TaylorModel *newTaylorModel(char *const fun, ExpTree *const exp,
                            const Interval remainder) {
  assert(fun != NULL);
  assert(exp != NULL);

  TaylorModel *list = (TaylorModel *)malloc(sizeof(TaylorModel));
  list->fun = fun;
  list->exp = exp;
  list->remainder = remainder;
  list->next = NULL;
  return list;
}

TaylorModel *appTMElem(TaylorModel *const tail, TaylorModel *head) {
  assert(head != NULL);
  assert(head->next == NULL);

  head->next = tail;
  return head;
}

TaylorModel *newTMElem(TaylorModel *const tail, char *const fun,
                       ExpTree *const exp, const Interval remainder) {
  TaylorModel *head = newTaylorModel(fun, exp, remainder);
  return appTMElem(tail, head);
}

void delTaylorModel(TaylorModel *list) {
  if (list->next != NULL)
    delTaylorModel(list->next);
  assert(list->fun != NULL);
  free(list->fun);
  assert(list->exp != NULL);
  delExpTree(list->exp);
  free(list);
}

TaylorModel *cpyTaylorModel(const TaylorModel *const list) {
  /* Base case: The tail/next of the last element is NULL. */
  if (list == NULL)
    return NULL;

  /* Recursive case: The tail of the new element is everything built until now.
   */
  return appTMElem(cpyTaylorModel(list->next), cpyTaylorModelHead(list));
}

TaylorModel *cpyTaylorModelHead(const TaylorModel *const list) {
  char *fun = strdup(list->fun);
  ExpTree *exp = cpyExpTree(list->exp);
  return newTaylorModel(fun, exp, list->remainder);
}

TaylorModel *reverseTaylorModel(TaylorModel *const list) {
  assert(list != NULL);

  /* Base case: push up the last element as the new head. */
  if (list->next == NULL)
    return list;

  /* Recursive case: Reverse the remaining tail and retrieve new head. */
  TaylorModel *lastElem = reverseTaylorModel(list->next);

  /* Only upon backtracking update this element's tail relation,
    else the recursion would be called on incorrect or on NULL ptrs. */
  assert(list->next != NULL);
  list->next->next = list; // reverse link: A->B  =>  A<-B
  list->next = NULL;       // sever own next: A->B  =>  A->NULL

  return lastElem;
}
