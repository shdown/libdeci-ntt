// Auto-generated; do not edit.
// Params: deci_UWORD => base 10^9, FFT_ULIMB => base 10^14.
static FFT_ULIMB *raw_conv_to(deci_UWORD *wa, deci_UWORD *wa_end, FFT_ULIMB *out)
{
 while (wa != wa_end) {
  deci_UWORD x = *wa++;
  {
   deci_UWORD y = *wa++;
   deci_UWORD yq = y / 100000u;
   deci_UWORD yr = y % 100000u;
   *out++ = x + yr * FFT_ULIMB_C(1000000000);
   x = yq;
  }
  {
   deci_UWORD y = *wa++;
   deci_UWORD z = *wa++;
   deci_UWORD zq = z / 10u;
   deci_UWORD zr = z % 10u;
   *out++ = x + y * FFT_ULIMB_C(10000) + zr * FFT_ULIMB_C(10000000000000);
   x = zq;
  }
  {
   deci_UWORD y = *wa++;
   deci_UWORD yq = y / 1000000u;
   deci_UWORD yr = y % 1000000u;
   *out++ = x + yr * FFT_ULIMB_C(100000000);
   x = yq;
  }
  {
   deci_UWORD y = *wa++;
   deci_UWORD z = *wa++;
   deci_UWORD zq = z / 100u;
   deci_UWORD zr = z % 100u;
   *out++ = x + y * FFT_ULIMB_C(1000) + zr * FFT_ULIMB_C(1000000000000);
   x = zq;
  }
  {
   deci_UWORD y = *wa++;
   deci_UWORD yq = y / 10000000u;
   deci_UWORD yr = y % 10000000u;
   *out++ = x + yr * FFT_ULIMB_C(10000000);
   x = yq;
  }
  {
   deci_UWORD y = *wa++;
   deci_UWORD z = *wa++;
   deci_UWORD zq = z / 1000u;
   deci_UWORD zr = z % 1000u;
   *out++ = x + y * FFT_ULIMB_C(100) + zr * FFT_ULIMB_C(100000000000);
   x = zq;
  }
  {
   deci_UWORD y = *wa++;
   deci_UWORD yq = y / 100000000u;
   deci_UWORD yr = y % 100000000u;
   *out++ = x + yr * FFT_ULIMB_C(1000000);
   x = yq;
  }
  {
   deci_UWORD y = *wa++;
   deci_UWORD z = *wa++;
   deci_UWORD zq = z / 10000u;
   deci_UWORD zr = z % 10000u;
   *out++ = x + y * FFT_ULIMB_C(10) + zr * FFT_ULIMB_C(10000000000);
   x = zq;
  }
  {
   deci_UWORD y = *wa++;
   *out++ = x + y * FFT_ULIMB_C(100000);
  }
 }
 return out;
}
static deci_UWORD *raw_conv_from(FFT_ULIMB *a, FFT_ULIMB *a_end, deci_UWORD *out)
{
 while (a != a_end) {
  FFT_ULIMB x = *a++;
  {
   *out++ = x % 1000000000u;
   x /= 1000000000u;
  }
  {
   FFT_ULIMB y = *a++;
   FFT_ULIMB yq = y / 10000u;
   deci_UWORD yr = y % 10000u;
   *out++ = x + yr * 100000u;
   x = yq;
  }
  {
   *out++ = x % 1000000000u;
   x /= 1000000000u;
  }
  {
   FFT_ULIMB y = *a++;
   FFT_ULIMB yq = y / 100000000u;
   deci_UWORD yr = y % 100000000u;
   *out++ = x + yr * 10u;
   x = yq;
  }
  {
   FFT_ULIMB y = *a++;
   FFT_ULIMB yq = y / 1000u;
   deci_UWORD yr = y % 1000u;
   *out++ = x + yr * 1000000u;
   x = yq;
  }
  {
   *out++ = x % 1000000000u;
   x /= 1000000000u;
  }
  {
   FFT_ULIMB y = *a++;
   FFT_ULIMB yq = y / 10000000u;
   deci_UWORD yr = y % 10000000u;
   *out++ = x + yr * 100u;
   x = yq;
  }
  {
   FFT_ULIMB y = *a++;
   FFT_ULIMB yq = y / 100u;
   deci_UWORD yr = y % 100u;
   *out++ = x + yr * 10000000u;
   x = yq;
  }
  {
   *out++ = x % 1000000000u;
   x /= 1000000000u;
  }
  {
   FFT_ULIMB y = *a++;
   FFT_ULIMB yq = y / 1000000u;
   deci_UWORD yr = y % 1000000u;
   *out++ = x + yr * 1000u;
   x = yq;
  }
  {
   FFT_ULIMB y = *a++;
   FFT_ULIMB yq = y / 10u;
   deci_UWORD yr = y % 10u;
   *out++ = x + yr * 100000000u;
   x = yq;
  }
  {
   *out++ = x % 1000000000u;
   x /= 1000000000u;
  }
  {
   FFT_ULIMB y = *a++;
   FFT_ULIMB yq = y / 100000u;
   deci_UWORD yr = y % 100000u;
   *out++ = x + yr * 10000u;
   x = yq;
  }
  {
   *out++ = x;
  }
 }
 return out;
}
