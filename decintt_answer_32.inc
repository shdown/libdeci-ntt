// Auto-generated; do not edit.

void decintt_fft_recover_answer(
        FFT_ULIMB *out, size_t nout, FFT_ULIMB *a1, FFT_ULIMB *a2)
{
 FFT_DOUBLE_ULIMB carry = 0;
 for (size_t i = 0; i < nout; ++i) {
  carry += crt2(a1[i], a2[i]);
  FFT_DOUBLE_ULIMB q = (big_mulh((carry) >> 5, UINT32_C(1907652675), UINT32_C(175921860))) >> 7;
  out[i] = carry - q * UINT32_C(100000);
  carry = q;
 }
}
