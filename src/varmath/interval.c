#include "interval.h"

Interval newInterval(const double left, const double right) {
  assert(left <= right);
  return (Interval){left, right};
}
