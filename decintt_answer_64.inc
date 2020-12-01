// Auto-generated; do not edit.

void decintt_fft_recover_answer(
        FFT_ULIMB *out, size_t nout, FFT_ULIMB *a1, FFT_ULIMB *a2)
{
 FFT_DOUBLE_ULIMB carry = 0;
 for (size_t i = 0; i < nout; ++i) {
  carry += crt2(a1[i], a2[i]);
  FFT_DOUBLE_ULIMB q = (big_mulh(carry, UINT64_C(6018080969204141205), UINT64_C(12980742146337069071))) >> 46;
  out[i] = carry - q * UINT64_C(100000000000000);
  carry = q;
 }
}
