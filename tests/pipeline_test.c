#include "funexp.h"
#include "logging.h"
#include "odeparse.h"
#include "sysode.h"
#include "taylormodel.h"
#include "tmflowpipe.h"
#include "varparse.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  /* to avoid silly warnings about unused parameters */
  (void)argc;
  (void)argv;

  logLevelHybberish = LOG_COMP;

  /* Manually lay out the TM integration pipeline that the main script
    will follow. This allows testing how each of the different parts of the
    pipeline fit together, and verifying their output in a controlled
    environment. */
  {
    int code;                     // The output code for input parsing.
    const unsigned int order = 3; // The TM order to adhere to.
    const unsigned int k = 3;     // The TM truncation order to adhere to.

    /* Setup of the TM integration pipeline: call the parsing functionality
      to convert string inputs to data structures. */
    LOG_LINE(LOG_COMP, "=== Setup")

    char *odeStr = "x' = 1 + y; y' = x^2;";
    ODEList *odes;
    code = parseOdeString(odeStr, &odes);
    assert(code == 0);
    LOG_FMT(LOG_COMP, "%O", odes)

    char *varStr = "x in [-1, 1]; y in [-0.5, 0.5];";
    Domain *domains;
    code = parseVarString(varStr, &domains);
    assert(code == 0);
    LOG_FMT(LOG_COMP, "%D", domains)

    /* Actually run the TM flowpipe overapprox pipeline;
      the TM integration algorithm. */
    TaylorModel *tms = computeTaylorPolynomial(odes, order, k);
    LOG_FMT(LOG_COMP, "%Mp_%u%M = %T", order, tms);

    // TODO: Add safe remainder computation ...

    /* Clean up allocated memory. */
    delTaylorModel(tms);
    delDomain(domains);
    delOdeList(odes);
  }
}
