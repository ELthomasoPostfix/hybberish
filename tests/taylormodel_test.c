#include "funexp.h"
#include "logging.h"
#include "tm_arithmetic.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool approx(float actual, float expected, float epsilon) {
  assert(epsilon >= 0);
  return fabs(actual - expected) < epsilon;
}

/* Test if actual = expected up to precision epsilon. and print out the results.
 */
void testReal(double actual, double expected, float epsilon) {
  assert(epsilon >= 0);

  /* printing and comparison */
  bool compare = actual == expected;
  printf("Expect: %f\n", expected);
  printf("Actual:    %f\n", actual);
  printf("Compare = %i\n\n", compare);
  fflush(stdout); // Flush stdout
  assert(compare);
}

/* Test if actual = [a, b] = [expectedLeft, expectedRight]
  up to precision epsilon. and print out the results. */
void testInterval(Interval *actual, float expectedLeft, float expectedRight,
                  float epsilon) {
  assert(actual != NULL);
  assert(epsilon >= 0);

  /* printing and comparison */
  bool compare = approx(actual->left, expectedLeft, epsilon) &&
                 approx(actual->right, expectedRight, epsilon);

  printf("Expect: [%f, %f]\n", expectedLeft, expectedRight);
  printf("Actual:    [%f, %f]\n", actual->left, actual->right);
  printf("Compare = %i\n\n", compare);
  fflush(stdout); // Flush stdout
  assert(compare);
}

bool isEqualTM(TaylorModel *actual, TaylorModel *expected, float epsilon) {
  /* Base case: no mismatches found at all */
  if (actual == NULL && expected == NULL)
    return true;
  /* Length mismatch */
  if ((actual == NULL) != (expected == NULL))
    return false;
  if (strcmp(actual->fun, expected->fun) != 0)
    return false;
  if (!(approx(actual->remainder.left, expected->remainder.left, epsilon) &&
        approx(actual->remainder.right, expected->remainder.right, epsilon)))
    return false;
  if (!isEqual(actual->exp, expected->exp))
    return false;

  return isEqualTM(actual->next, expected->next, epsilon);
}

void testTaylorModel(TaylorModel *actual, TaylorModel *expected,
                     float epsilon) {
  bool compare = isEqualTM(actual, expected, epsilon);

  printf("actual:    ");
  if (actual == NULL)
    printf("NULL");
  else
    printTaylorModel(actual, stdout);
  printf("\nexpect: ");
  if (expected == NULL)
    printf("NULL");
  else
    printTaylorModel(expected, stdout);
  printf("\nequal:  %i", compare);
  printf("\n\n");
  fflush(stdout);

  assert(compare);
}

int main(int argc, char *argv[]) {
  /* to avoid silly warnings about unused parameters */
  (void)argc;
  (void)argv;

  /* Setup: construct common components for use in many tests. */
  const float epsilon = 0.0001;

  ExpTree *x = newExpLeaf(EXP_VAR, "x");
  ExpTree *y = newExpLeaf(EXP_VAR, "y");
  ExpTree *z = newExpLeaf(EXP_VAR, "z");
  ExpTree *zero = newExpLeaf(EXP_NUM, "0");
  ExpTree *one = newExpLeaf(EXP_NUM, "1");
  ExpTree *two = newExpLeaf(EXP_NUM, "2");

  const Interval I11 = newInterval(-0.1, 0.1);
  const Interval I12 = newInterval(-0.1, 0.1);
  const Interval I21 = newInterval(-0.2, 0.2);
  const Interval I22 = newInterval(-0.2, 0.2);

  /* (x + y) */
  ExpTree *expX1 = newExpOp(EXP_ADD_OP, cpyExpTree(x), cpyExpTree(y));
  /* (1 - (y^2)) */
  ExpTree *expY1 =
      newExpOp(EXP_SUB_OP, cpyExpTree(one),
               newExpOp(EXP_EXP_OP, cpyExpTree(y), cpyExpTree(two)));
  /* x */
  ExpTree *expX2 = cpyExpTree(x);
  /* (x - z) */
  ExpTree *expY2 = newExpOp(EXP_SUB_OP, cpyExpTree(x), cpyExpTree(z));

  /* Construct common-use Taylor models */
  TaylorModel *tm1 = newTMElem(NULL, strdup(y->data), expY1, I12);
  tm1 = newTMElem(tm1, strdup(x->data), expX1, I11);
  TaylorModel *tm2 = newTMElem(NULL, strdup(y->data), expY2, I22);
  tm2 = newTMElem(tm2, strdup(x->data), expX2, I21);

  /* Define variable valuation */
  Domain *domz = newDomainElem(NULL, strdup("z"), newInterval(-2, 1));
  Domain *domy = newDomainElem(domz, strdup("y"), newInterval(3, 4));
  Domain *domx = newDomainElem(domy, strdup("x"), newInterval(1, 2));
  Domain *domains = domx;

  /* Test constructor and destructor. */
  {
    printf("\n=== Constructor and destructor ===\n");
    fflush(stdout);

    char *fun1 = "x";
    ExpTree *exp1 = newExpLeaf(EXP_VAR, fun1);
    Interval remainder1 = newInterval(1, 2);
    TaylorModel *newTM1 = newTaylorModel(strdup(fun1), exp1, remainder1);
    assert(strcmp(fun1, newTM1->fun) == 0);
    assert(isEqual(exp1, newTM1->exp));
    assert(eqInterval(&remainder1, &newTM1->remainder, epsilon));
    assert(newTM1->next == NULL);

    char *fun2 = "y";
    ExpTree *exp2 = newExpLeaf(EXP_VAR, fun2);
    Interval remainder2 = newInterval(2, 3);
    TaylorModel *newTM2 = newTMElem(newTM1, strdup(fun2), exp2, remainder2);
    assert(strcmp(fun2, newTM2->fun) == 0);
    assert(isEqual(exp2, newTM2->exp));
    assert(eqInterval(&remainder2, &newTM2->remainder, epsilon));
    assert(newTM2->next != NULL);
    assert(strcmp(fun1, newTM2->next->fun) == 0);
    assert(isEqual(exp1, newTM2->next->exp));
    assert(eqInterval(&remainder1, &newTM2->next->remainder, epsilon));
    assert(newTM2->next->next == NULL);

    newTM2->next = NULL;
    newTM2 = appTMElem(newTM1, newTM2);
    assert(strcmp(fun2, newTM2->fun) == 0);
    assert(isEqual(exp2, newTM2->exp));
    assert(eqInterval(&remainder2, &newTM2->remainder, epsilon));
    assert(newTM2->next != NULL);
    assert(strcmp(fun1, newTM2->next->fun) == 0);
    assert(isEqual(exp1, newTM2->next->exp));
    assert(eqInterval(&remainder1, &newTM2->next->remainder, epsilon));
    assert(newTM2->next->next == NULL);

    /* Clean */
    delTaylorModel(newTM2);
  }

  /* Test TM copying */
  {
    printf("\n=== Copying ===\n");
    fflush(stdout);

    /* Construct test values */
    char *fun1 = "x";
    ExpTree *exp1 = newExpLeaf(EXP_VAR, fun1);
    Interval remainder1 = newInterval(1, 2);
    TaylorModel *newTM1 = newTaylorModel(strdup(fun1), exp1, remainder1);

    char *fun2 = "y";
    ExpTree *exp2 = newExpLeaf(EXP_VAR, fun2);
    Interval remainder2 = newInterval(2, 3);
    TaylorModel *newTM2 = newTMElem(newTM1, strdup(fun2), exp2, remainder2);

    /* Testing: the resulting instance should be an exact copy of the input. */
    TaylorModel *copied;

    copied = cpyTaylorModel(newTM2);
    testTaylorModel(copied, newTM2, epsilon);
    delTaylorModel(copied);

    copied = cpyTaylorModelHead(newTM2);
    newTM2->next = NULL;
    testTaylorModel(copied, newTM2, epsilon);
    delTaylorModel(copied);

    /* Clean */
    delTaylorModel(newTM1);
    delTaylorModel(newTM2);
  }

  /* Test expression tree interval evaluation. */
  {
    printf("\n=== Interval evaluation ===\n");
    fflush(stdout);

    /* -(((x + y) / x) - (z^2 + 1)) */
    ExpTree *xPy = newExpOp(EXP_ADD_OP, cpyExpTree(x), cpyExpTree(y));
    ExpTree *div = newExpOp(EXP_DIV_OP, xPy, cpyExpTree(x));
    ExpTree *z2 = newExpOp(EXP_EXP_OP, cpyExpTree(z), cpyExpTree(two));
    ExpTree *z2P1 = newExpOp(EXP_ADD_OP, z2, cpyExpTree(one));
    ExpTree *sub = newExpOp(EXP_SUB_OP, div, z2P1);
    ExpTree *exp = newExpOp(EXP_NEG, sub, NULL);

    /* NOTE: There may be multiple implementations of
      the Interval pow/exponentiation function! The
      computation below assumes a naive algorithm,
      where exponentiation is unrolled into multiple
      multiplications.

      Fill in  -(((x + y) / x) - (z^2 + 1))  by choosing
      x in [1, 2], y in [3, 4] and z in [-2, 1].

      -((([1.0, 2.0] + [3.0, 4.0]) / [1.0, 2.0]) -
        ([-2.0, 1.0]^2 + [1.0, 1.0]))
      = -(([4.0, 6.0] / [1.0, 2.0]) - ([-2.0, 4.0] + [1.0, 1.0]))
      = -([2.0, 6.0] - [-1.0, 5.0])
      = -[-3.0, 7.0]
      =  [-7.0, 3.0]
    */
    Interval res = evaluateExpTree(exp, domains);
    testInterval(&res, -7.0, 3.0, 0.001);

    /* Clean */
    delExpTree(exp);
  }

  /* Test expression tree real evaluation. */
  {
    printf("\n=== Real evaluation ===\n");
    fflush(stdout);

    /* -(((x + y) / x) - (z^2 + 1)) */
    ExpTree *xPy = newExpOp(EXP_ADD_OP, cpyExpTree(x), cpyExpTree(y));
    ExpTree *div = newExpOp(EXP_DIV_OP, xPy, cpyExpTree(x));
    ExpTree *z2 = newExpOp(EXP_EXP_OP, cpyExpTree(z), cpyExpTree(two));
    ExpTree *z2P1 = newExpOp(EXP_ADD_OP, z2, cpyExpTree(one));
    ExpTree *sub = newExpOp(EXP_SUB_OP, div, z2P1);
    ExpTree *exp = newExpOp(EXP_NEG, sub, NULL);

    double xReal = 2.;
    double yReal = 3.;
    double zReal = 4.;
    Valuation *xVal = newValuation(strdup("x"), xReal);
    Valuation *yVal = newValuation(strdup("y"), yReal);
    Valuation *zVal = newValuation(strdup("z"), zReal);
    Valuation *values;
    values = appValuationElem(zVal, yVal);
    values = appValuationElem(values, xVal);

    /*
      Fill in  -(((x + y) / x) - (z^2 + 1))  by choosing
      x = 2, y = 3 and z = 4.

      -(((x + y) / x) - (z^2 + 1))
      = -(((2. + 3.) / 2.) - (4.^2 + 1))
      = -((5. / 2.) - (16. + 1))
      = -(2.5 - 17.)
      = -(-14.5)
      = 14.5
    */

    double res = evaluateExpTreeReal(exp, values);
    testReal(res, 14.5, 0.001);

    /* Clean */
    delExpTree(exp);
    delValuation(values);
  }

  /* Test order k Taylor model arithmetic. Simply construct some
    taylor models and then apply different TM operators. Then
    verify the results. */
  printf("\n=== Taylor model arithmetic ===\n");
  fflush(stdout);

  {
    const unsigned int tmOrder = 1;

    printf("### Taylor model binary ADD (+); TM order k = %i ###\n", tmOrder);
    fflush(stdout);

    /* Test two cases, where we express the expected results as follows:
          "TM arithmetic result" => "truncated TM arithmetic result"

      1) Terms do NOT get truncated for the first TM element
          (x + y) + x  =>  (x + y) + x
      2) Terms DO get truncated for the second TM element:
          (1 - y^2) + (x - z)
       =>  (1 - 0) + (x - z)
       =>  1 + (x - z)
    */
    {
      ExpTree *addx =
          newExpOp(EXP_ADD_OP, cpyExpTree(tm1->exp), cpyExpTree(tm2->exp));
      ExpTree *addy =
          newExpOp(EXP_ADD_OP, cpyExpTree(one), cpyExpTree(tm2->next->exp));

      /* No terms were truncated in the first TM element. */
      Interval remx = addInterval(&I11, &I21);
      /* Il = I1 + I2 + Int(pe)  where Int(pe) is the interval
        enclosure of the truncated terms. */
      Interval ypow2 = pow2Interval(&domy->domain, 2);
      Interval neg = negInterval(&ypow2);
      Interval remy = addInterval(&I11, &I22);
      remy = addInterval(&remy, &neg);

      /* Compose the Taylor model expected as output. */
      TaylorModel *expected =
          newTMElem(NULL, strdup(tm1->next->fun), addy, remy);
      expected = newTMElem(expected, strdup(tm1->fun), addx, remx);

      /* Compute and test results. */
      TaylorModel *binop = addTM(tm1, tm2, domains, tmOrder);
      testTaylorModel(binop, expected, epsilon);

      /* Clean */
      delTaylorModel(binop);
      delTaylorModel(expected);
    }
  }

  {
    const unsigned int tmOrder = 1;

    printf("### Taylor model binary SUB (-); TM order k = %i ###\n", tmOrder);
    fflush(stdout);

    /* Test two cases, where we express the expected results as follows:
          "TM arithmetic result" => "truncated TM arithmetic result"

      1) Terms do NOT get truncated for the first TM element:
          (x + y) - x  =>  (x + y) - x
      2) Terms DO get truncated for the second TM element:
          (1 - y^2) - (x - z)
       => (1 - 0) - (x - z)
       => 1 - (x - z)
    */
    {
      ExpTree *subx =
          newExpOp(EXP_SUB_OP, cpyExpTree(tm1->exp), cpyExpTree(tm2->exp));
      ExpTree *suby =
          newExpOp(EXP_SUB_OP, cpyExpTree(one), cpyExpTree(tm2->next->exp));

      /* No terms were truncated in the first TM element. */
      Interval remx = subInterval(&I11, &I21);
      /* Il = I1 - I2 + Int(pe)  where Int(pe) is the interval
        enclosure of the truncated terms. */
      Interval ypow2 = pow2Interval(&domy->domain, 2);
      Interval neg = negInterval(&ypow2);
      Interval remy = subInterval(&I11, &I22);
      remy = addInterval(&remy, &neg);

      /* Compose the Taylor model expected as output. */
      TaylorModel *expected =
          newTMElem(NULL, strdup(tm1->next->fun), suby, remy);
      expected = newTMElem(expected, strdup(tm1->fun), subx, remx);

      /* Compute and test results. */
      TaylorModel *binop = subTM(tm1, tm2, domains, tmOrder);
      testTaylorModel(binop, expected, epsilon);

      /* Clean */
      delTaylorModel(binop);
      delTaylorModel(expected);
    }
  }

  {
    const unsigned int tmOrder = 2;

    printf("### Taylor model binary MUL (*); TM order k = %i ###\n", tmOrder);
    fflush(stdout);

    /* Test two cases, where we express the expected results as follows:
          "TM arithmetic result" => "truncated TM arithmetic result"

      1) Terms do NOT get truncated for the first TM element
          (x * x) + (x * y)  =>  (x * x) + (x * y)
      2) Terms DO get truncated for the second TM element:
          (((1 * x) - (1 * z)) - ((y^2 * x) - (y^2 * z)))
      =>  (((1 * x) - (1 * z)) - (0 - 0))
      =>  (x - z)
    */
    {
      ExpTree *xTx = newExpOp(EXP_MUL_OP, cpyExpTree(x), cpyExpTree(x));
      ExpTree *xTy = newExpOp(EXP_MUL_OP, cpyExpTree(x), cpyExpTree(y));
      ExpTree *mulx = newExpOp(EXP_ADD_OP, xTx, xTy);

      ExpTree *muly = newExpOp(EXP_SUB_OP, cpyExpTree(x), cpyExpTree(z));

      Interval ypow2 = pow2Interval(&domy->domain, 2);
      Interval intOne = newInterval(1, 1);
      Interval encp11 = addInterval(&domx->domain, &domy->domain);
      Interval encp12 = subInterval(&intOne, &ypow2);
      Interval encp21 = domx->domain;
      Interval encp22 = subInterval(&domx->domain, &domz->domain);

      /* No terms were truncated in the first TM element. */
      Interval termx1 = mulInterval(&I11, &I21);
      Interval termx2 = mulInterval(&encp11, &I21);
      Interval termx3 = mulInterval(&encp21, &I11);
      Interval remx = addInterval(&termx1, &termx2);
      remx = addInterval(&remx, &termx3);
      /* Il = I1*I2 + Int(p1)*I2 + Int(p2)*I1 + Int(pe)
        where Int(pe) is the interval enclosure of the truncated terms,
        and similar for Int(p1) and Int(p2). */
      Interval termy1 = mulInterval(&I12, &I22);
      Interval termy2 = mulInterval(&encp12, &I22);
      Interval termy3 = mulInterval(&encp22, &I12);
      /* truncated expression:
        -((y^2 * x) - (y^2 * z))
      = (-(y^2 * x) + (y^2 * z)) */
      Interval truncEnc1 = mulInterval(&ypow2, &domx->domain);
      truncEnc1 = negInterval(&truncEnc1);
      Interval truncEnc2 = mulInterval(&ypow2, &domz->domain);
      Interval remy = addInterval(&termy1, &termy2);
      remy = addInterval(&remy, &termy3);
      remy = addInterval(&remy, &truncEnc1);
      remy = addInterval(&remy, &truncEnc2);

      /* Compose the Taylor model expected as output. */
      TaylorModel *expected =
          newTMElem(NULL, strdup(tm1->next->fun), muly, remy);
      expected = newTMElem(expected, strdup(tm1->fun), mulx, remx);

      /* Compute and test results. */
      TaylorModel *binop = mulTM(tm1, tm2, domains, tmOrder);
      testTaylorModel(binop, expected, epsilon);

      /* Clean */
      delTaylorModel(binop);
      delTaylorModel(expected);
    }
  }

  {
    const unsigned int tmOrder = 3;

    printf("### Taylor model binary EXP (^); TM order k = %i ###\n", tmOrder);
    fflush(stdout);

    /* Test two cases, where we express the expected results as follows:
          "TM arithmetic result" => "truncated TM arithmetic result"

      1) Terms do NOT get truncated for the first TM element
          (x + 1)^3
      =>  (x + 1)*(x + 1)*(x + 1)     // intermediate result
      =>  (x + 1)*(x*x + x + x + 1)   // intermediate result
      =>  x*x*x + x*x + x*x + x + x*x + x + x + 1
      2) Terms DO get truncated for the second TM element:
          (x + y^2)^3
      =>  (x + y^2)*(x + y^2)*(x + y^2)                  // intermediate result
      =>  (x + y^2)*(x*x + x*y^2 + y^2*x + y^2*y^2)      // intermediate result
      =>  (x + y^2)*(x*x + x*y^2 + y^2*x + 0)      // intermediate trunc!
      =>  x*x*x   + x*x*y^2   + x*y^2*x   +        // intermediate result
          y^2*x*x + y^2*x*y^2 + y^2*y^2*x
      =>  x*x*x + 0 + 0 + 0 + 0 + 0
    */
    {
      ExpTree *y2 = newExpOp(EXP_EXP_OP, cpyExpTree(y), cpyExpTree(two));
      ExpTree *xP1 = newExpOp(EXP_ADD_OP, cpyExpTree(x), cpyExpTree(one));
      ExpTree *xPy = newExpOp(EXP_ADD_OP, cpyExpTree(x), y2);

      TaylorModel *tmpow = newTMElem(NULL, strdup(y->data), xPy, I12);
      tmpow = newTMElem(tmpow, strdup(x->data), xP1, I11);

      /* ((((x * (x * x)) + (x * x)) + ((x * x) + x)) +
        (((x * x) + x) + (x + 1))) */
      ExpTree *xTxTx =
          newExpOp(EXP_MUL_OP, cpyExpTree(x),
                   newExpOp(EXP_MUL_OP, cpyExpTree(x), cpyExpTree(x)));
      ExpTree *xTxTo = newExpOp(EXP_MUL_OP, cpyExpTree(x), cpyExpTree(x));
      ExpTree *xToTo = cpyExpTree(x);
      ExpTree *oToTo = cpyExpTree(one);
      ExpTree *oToTx = cpyExpTree(x);
      ExpTree *oTxTx = newExpOp(EXP_MUL_OP, cpyExpTree(x), cpyExpTree(x));
      ExpTree *xToTx = newExpOp(EXP_MUL_OP, cpyExpTree(x), cpyExpTree(x));
      ExpTree *oTxTo = cpyExpTree(x);
      ExpTree *add1x = newExpOp(EXP_ADD_OP, xTxTx, xTxTo);
      ExpTree *add2x = newExpOp(EXP_ADD_OP, xToTx, xToTo);
      ExpTree *add3x = newExpOp(EXP_ADD_OP, add1x, add2x);
      ExpTree *add4x = newExpOp(EXP_ADD_OP, oTxTx, oTxTo);
      ExpTree *add5x = newExpOp(EXP_ADD_OP, oToTx, oToTo);
      ExpTree *add6x = newExpOp(EXP_ADD_OP, add4x, add5x);
      ExpTree *mulx = newExpOp(EXP_ADD_OP, add3x, add6x);

      /* (x * (x * x)) */
      ExpTree *muly = cpyExpTree(xTxTx);

      Interval remx = newInterval(-2.791000, 2.791000);
      Interval remy = newInterval(930.139000, 5921.741000);

      /* Compose the Taylor model expected as output. */
      TaylorModel *expected =
          newTMElem(NULL, strdup(tm1->next->fun), muly, remy);
      expected = newTMElem(expected, strdup(tm1->fun), mulx, remx);

      /* Compute and test results. */
      TaylorModel *binop = powTM(tmpow, 3, domains, tmOrder);
      testTaylorModel(binop, expected, epsilon);

      /* Clean */
      delTaylorModel(tmpow);
      delTaylorModel(binop);
      delTaylorModel(expected);
    }
  }

  {
    const unsigned int tmOrder = 1;

    printf("### Taylor model binary DIV (/); TM order k = %i ###\n", tmOrder);
    fflush(stdout);

    /* Test two cases, where we express the expected results as follows:
          "TM arithmetic result" => "truncated TM arithmetic result"

      1) Terms do NOT get truncated for the first TM element
          x / x  =>  x / x
      2) Terms DO get truncated for the second TM element:
          (x / y^2)  =>  (x / y^2) + 0
    */
    {

      ExpTree *divx = newExpOp(EXP_DIV_OP, cpyExpTree(x), cpyExpTree(x));

      ExpTree *y2 = newExpOp(EXP_EXP_OP, cpyExpTree(y), cpyExpTree(two));
      ExpTree *x3 =
          newExpOp(EXP_EXP_OP, cpyExpTree(x), newExpLeaf(EXP_NUM, "3"));
      ExpTree *xDy2 = newExpOp(EXP_DIV_OP, cpyExpTree(x), y2);
      ExpTree *divy = newExpOp(EXP_ADD_OP, xDy2, x3);

      /* Compose the Taylor model expected as output. */
      TaylorModel *expected =
          newTMElem(NULL, strdup(tm2->next->fun), divy, newInterval(0, 0));
      expected =
          newTMElem(expected, strdup(tm1->next->fun), divx, newInterval(0, 0));

      /* Compose new input Taylor models, since TM division has
        some restrictions. */
      ExpTree *tm1ExpX = cpyExpTree(x);
      ExpTree *tm1ExpY = cpyExpTree(x);
      ExpTree *tm2ExpX = cpyExpTree(x);
      ExpTree *tm2ExpY = newExpOp(EXP_EXP_OP, cpyExpTree(y), cpyExpTree(two));

      TaylorModel *tm1Div =
          newTMElem(NULL, strdup("y"), tm1ExpY, newInterval(2, 2));
      tm1Div = newTMElem(tm1Div, strdup("x"), tm1ExpX, newInterval(1, 1));

      TaylorModel *tm2Div =
          newTMElem(NULL, strdup("y"), tm2ExpY, newInterval(2, 2));
      tm2Div = newTMElem(tm2Div, strdup("x"), tm2ExpX, newInterval(1, 1));

      /* Compose new variable domains to simplify the example. */
      Domain *domyDiv = newDomainElem(NULL, strdup("y"), newInterval(2, 4));
      Domain *domxDiv = newDomainElem(domyDiv, strdup("x"), newInterval(2, 4));
      Domain *domainsDiv = domxDiv;

      // TODO: Uncomment test code, when TM DIV is ready to be tested.
      //       Do not forget to add logging to it!

      /* Compute and test results. */
      // TaylorModel *binop = divTM(tm1Div, tm2Div, domainsDiv, tmOrder);
      // testTaylorModel(binop, expected, epsilon);
      // delTaylorModel(binop);

      /* Clean */
      delDomain(domainsDiv);
      delTaylorModel(expected);
      delTaylorModel(tm1Div);
      delTaylorModel(tm2Div);
    }
  }

  /* Test expression tree Taylor model evaluation. */
  {
    printf("\n=== Taylor model evaluation ===\n");
    fflush(stdout);

    const unsigned int tmOrder = 2;
    char *fun = "y";

    /* Construct Taylor models to evaluate with. */
    TaylorModel *tmz = newTaylorModel(strdup(z->data), newExpLeaf(EXP_VAR, "x"),
                                      newInterval(-0.3, 0.3));
    TaylorModel *tmy = newTMElem(tmz, strdup(y->data), newExpLeaf(EXP_VAR, "z"),
                                 newInterval(-0.2, 0.2));
    TaylorModel *tmx = newTMElem(tmy, strdup(x->data), newExpLeaf(EXP_VAR, "y"),
                                 newInterval(-0.1, 0.1));
    TaylorModel *tms = tmx;

    /* -(((x + y) / (x * x)) - (z^2 + 1)) */
    ExpTree *xPy;
    ExpTree *xTx;
    ExpTree *div;
    ExpTree *z2P1;
    ExpTree *sub;
    ExpTree *exp;
    // xPy = newExpOp(EXP_ADD_OP, cpyExpTree(x), cpyExpTree(y));
    // xTx = newExpOp(EXP_MUL_OP, cpyExpTree(x), cpyExpTree(x));
    // div = newExpOp(EXP_DIV_OP, xPy, xTx);
    // ExpTree *z2 = newExpOp(EXP_EXP_OP, cpyExpTree(z), cpyExpTree(two));
    // z2P1 = newExpOp(EXP_ADD_OP, z2, cpyExpTree(one));
    // sub = newExpOp(EXP_SUB_OP, div, z2P1);
    // exp = newExpOp(EXP_NEG, sub, NULL);

    // TODO: \/\/\/ Use the previous, more complete expression once eval/TM
    //              arithmetic is implemented more fully!!!!
    /* -((((x * z) + y) * x) - (z + 1)) */
    xTx = newExpOp(EXP_MUL_OP, cpyExpTree(x), cpyExpTree(z));
    xPy = newExpOp(EXP_ADD_OP, xTx, cpyExpTree(y));
    div = newExpOp(EXP_MUL_OP, xPy, cpyExpTree(x));
    z2P1 = newExpOp(EXP_ADD_OP, cpyExpTree(z), cpyExpTree(one));
    sub = newExpOp(EXP_SUB_OP, div, z2P1);
    exp = newExpOp(EXP_NEG, sub, NULL);
    // TODO: /\/\/\ Use the previous, more complete expression once eval/TM
    //              arithmetic is implemented more fully!!!!

    /* NOTE: There may be multiple implementations of
      the Interval pow/exponentiation function!!!

      ORDER k = 2
      x in [ 1, 2]
      y in [ 3, 4]
      z in [-2, 1]
      TM x = ( y, [-0.1, 0.1] )
      TM y = ( z, [-0.2, 0.2] )
      TM z = ( x, [-0.3, 0.3] )

      ==> Each variable in the expression below is substituted
       by the TM corresponding to that variable. i.e. substitute
       every variable 'x' by TM x.

    The following expression will be overapproximated.
    Note that each Ei simply facilitates a step-by-step solving of the
    expression, no actual algebraic manipulations are performed.
      -((((x * z) + y) * x) - (z + 1))
    = -(((E1 + y) * x) - E2)
    = -((E3 * x) - E2)
    = -(E4 - E2)
    = -E5
    = E6

    E1 = (y, [-0.1, 0.1]) * (x, [-0.3, 0.3])
       = (y * x - pe, Int(pe) + Int(p1)*I2 + Int(p2)*I1 + I1*I2)
       = (y * x, [0, 0] + [3, 4]*[-0.3, 0.3] +
                 [1, 2]*[-0.1, 0.1] + [-0.1, 0.1]*[-0.3, 0.3])
       = (y * x, [0, 0] + [-1.2, 1.2] +
                 [-0.2, 0.2] + [-0.03, 0.03])
       = (y * x, [-1.43, 1.43])
    E3 = (y * x, [-1.43, 1.43]) + (z, [-0.2, 0.2])
       = ((y * x) + z, [-1.63, 1.63])
    E4 = ((y * x) + z, [-1.63, 1.63]) * (y, [-0.1, 0.1])
       = (((y * x) + z) * y - pe, Int(pe) + Int(p1)*I2 + Int(p2)*I1 + I1*I2)
       = ((((y * x) * y) + (y * z)) - pe,
          Int(pe) + Int(p1)*I2 + Int(p2)*I1 + I1*I2)
       = (y * z - pe, Int(pe) + Int(p1)*I2 + Int(p2)*I1 + I1*I2)
       = (y * z, Int(pe) + Int(p1)*I2 + Int(p2)*I1 + I1*I2)
       = (y * z, [9, 32] + [1, 9]*[-0.1, 0.1] +
                 [3, 4]*[-1.63, 1.63] + [-1.63, 1.63]*[-0.1, 0.1])
       = (y * z, [9, 32] + [-0.9, 0.9]        +
                 [-6.52, 6.52] + [-0.163, 0.163])
       = (y * z, [9, 32] + [-0.9, 0.9]        +
                 [-6.52, 6.52] + [-0.163, 0.163])
       = (y * z, [1.417, 39.583])
    E2 = (x, [-0.3, 0.3]) + (1, [0, 0])
       = (x + 1, [-0.3, 0.3])
    E5 = (y * z, [1.417, 39.583]) - (x + 1, [-0.3, 0.3])
       = ((y * z) - (x + 1), [1.417, 39.583] - [-0.3, 0.3])
       = ((y * z) - (x + 1), [1.117, 39.883])
    E6 = (-((y * z) - (x + 1)), -[1.117, 39.883])
       = (-((y * z) - (x + 1)), [-39.883, -1.117])
    */
    ExpTree *yTz = newExpOp(EXP_MUL_OP, cpyExpTree(y), cpyExpTree(z));
    ExpTree *subLeft = yTz;
    ExpTree *subRight = newExpOp(EXP_ADD_OP, cpyExpTree(x), cpyExpTree(one));
    ExpTree *subFull = newExpOp(EXP_SUB_OP, subLeft, subRight);
    ExpTree *neg = newExpOp(EXP_NEG, subFull, NULL);
    Interval remainder = newInterval(-39.883, -1.117);
    TaylorModel *expected = newTaylorModel(strdup(fun), neg, remainder);

    TaylorModel *res = evaluateExpTreeTM(exp, tms, fun, domains, tmOrder);
    testTaylorModel(res, expected, epsilon);

    /* Clean */
    delExpTree(exp);
    delTaylorModel(res);
    delTaylorModel(tms);
    delTaylorModel(expected);
  }

  /* Clean */
  delExpTree(x);
  delExpTree(y);
  delExpTree(z);
  delExpTree(zero);
  delExpTree(one);
  delExpTree(two);
  delTaylorModel(tm1);
  delTaylorModel(tm2);
  delDomain(domains);
}
