#include <cfenv>
#include <complex>
#include <limits>

// Match convert2.cpp's 64-element fast path. Keep the caller's known input
// out of the optimizer so the probe exercises the generated conversion loop.
template <typename T>
__attribute__((noinline, noclone))
static void convert(const std::complex<float>* in, T* out) {
  for (int i = 0; i < 64; ++i)
    out[i] = static_cast<T>(static_cast<int>(in[i].real()));
}

template <typename T>
static int check(const std::complex<float>* in) {
  T out[64];
  if (std::feclearexcept(FE_ALL_EXCEPT) != 0) return 2;
  convert(in, out);
  const int invalid = std::fetestexcept(FE_INVALID);
  for (int i = 0; i < 64; ++i)
    if (out[i] != i) return 2;
  return invalid ? 1 : 0;
}

int main() {
  std::complex<float> in[64];
  for (int i = 0; i < 64; ++i)
    in[i] = {static_cast<float>(i), std::numeric_limits<float>::quiet_NaN()};

  const int results[] = {check<unsigned char>(in), check<short>(in),
                         check<unsigned short>(in), check<int>(in)};
  for (int result : results)
    if (result != 0) return result;
  return 0;
}
