#include "variables.h"

Domain *newDomain(char *var, const Interval domain) {
  assert(var != NULL);

  Domain *list = (Domain *)malloc(sizeof(Domain));
  list->var = var;
  list->domain = domain;
  list->next = NULL;
  return list;
}

Domain *appDomainElem(Domain *tail, Domain *head) {
  assert(head != NULL);
  assert(head->next == NULL);

  head->next = tail;
  return head;
}

Domain *newDomainElem(Domain *tail, char *var, const Interval domain) {
  Domain *head = newDomain(var, domain);
  return appDomainElem(tail, head);
}

void delDomain(Domain *list) {
  if (list->next != NULL)
    delDomain(list->next);
  assert(list->var != NULL);
  free(list->var);
  free(list);
}

Valuation *newValuation(char *var, const double val) {
  assert(var != NULL);

  Valuation *list = (Valuation *)malloc(sizeof(Valuation));
  list->var = var;
  list->val = val;
  list->next = NULL;
  return list;
}

Valuation *appValuationElem(Valuation *tail, Valuation *head) {
  assert(head != NULL);
  assert(head->next == NULL);

  head->next = tail;
  return head;
}

Valuation *newValuationElem(Valuation *tail, char *var, const double val) {
  Valuation *head = newValuation(var, val);
  return appValuationElem(tail, head);
}

void delValuation(Valuation *list) {
  if (list->next != NULL)
    delValuation(list->next);
  assert(list->var != NULL);
  free(list->var);
  free(list);
}
