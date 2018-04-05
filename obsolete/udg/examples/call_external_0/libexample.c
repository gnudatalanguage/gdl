#include <math.h>
double c_nextafter(int argc, void* argv[]) {
  return nextafter(*(double*)argv[0], *(double*)argv[1]);
}

