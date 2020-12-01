// Auto-generated; do not edit.
// Params: deci_UWORD => base 10^4, FFT_ULIMB => base 10^5.
static FFT_ULIMB *raw_conv_to(deci_UWORD *wa, deci_UWORD *wa_end, FFT_ULIMB *out)
{
 while (wa != wa_end) {
  deci_UWORD x = *wa++;
  {
   deci_UWORD y = *wa++;
   deci_UWORD yq = y / 10u;
   deci_UWORD yr = y % 10u;
   *out++ = x + yr * FFT_ULIMB_C(10000);
   x = yq;
  }
  {
   deci_UWORD y = *wa++;
   deci_UWORD yq = y / 100u;
   deci_UWORD yr = y % 100u;
   *out++ = x + yr * FFT_ULIMB_C(1000);
   x = yq;
  }
  {
   deci_UWORD y = *wa++;
   deci_UWORD yq = y / 1000u;
   deci_UWORD yr = y % 1000u;
   *out++ = x + yr * FFT_ULIMB_C(100);
   x = yq;
  }
  {
   deci_UWORD y = *wa++;
   *out++ = x + y * FFT_ULIMB_C(10);
  }
 }
 return out;
}
static deci_UWORD *raw_conv_from(FFT_ULIMB *a, FFT_ULIMB *a_end, deci_UWORD *out)
{
 while (a != a_end) {
  FFT_ULIMB x = *a++;
  {
   *out++ = x % 10000u;
   x /= 10000u;
  }
  {
   FFT_ULIMB y = *a++;
   FFT_ULIMB yq = y / 1000u;
   deci_UWORD yr = y % 1000u;
   *out++ = x + yr * 10u;
   x = yq;
  }
  {
   FFT_ULIMB y = *a++;
   FFT_ULIMB yq = y / 100u;
   deci_UWORD yr = y % 100u;
   *out++ = x + yr * 100u;
   x = yq;
  }
  {
   FFT_ULIMB y = *a++;
   FFT_ULIMB yq = y / 10u;
   deci_UWORD yr = y % 10u;
   *out++ = x + yr * 1000u;
   x = yq;
  }
  {
   *out++ = x;
  }
 }
 return out;
}
