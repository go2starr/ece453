/****************************************************************************
 *
 ****************************************************************************/
#include <stdint.h>

/****************************************************************************
 *
 ****************************************************************************/
static const uint32_t k[64] =
{
   0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
   0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
   0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
   0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
   0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
   0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
   0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
   0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
   0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
   0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
   0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

/****************************************************************************
 *
 ****************************************************************************/
static uint32_t byteSwap32(uint32_t value)
{
   value = ((value >> 24) & 0x000000ff) |
           ((value >>  8) & 0x0000ff00) |
           ((value <<  8) & 0x00ff0000) |
           ((value << 24) & 0xff000000);

   return value;
}

/****************************************************************************
 *
 ****************************************************************************/
static uint32_t ror(uint32_t value, uint32_t amount)
{
   return (value >> amount) | (value << (32 - amount));
}

/****************************************************************************
 *
 ****************************************************************************/
static void sha256chunk(uint32_t sha256[8], const uint32_t chunk[16])
{
   uint32_t a = sha256[0];
   uint32_t b = sha256[1];
   uint32_t c = sha256[2];
   uint32_t d = sha256[3];
   uint32_t e = sha256[4];
   uint32_t f = sha256[5];
   uint32_t g = sha256[6];
   uint32_t h = sha256[7];
   uint32_t w[64];
   uint32_t s0;
   uint32_t s1;
   uint32_t i;

   for (i = 0; i < 16; i++)
      w[i] = byteSwap32(chunk[i]);

   for (i = 16; i < 64; i++)
   {
      s0 = ror(w[i - 15], 7) ^ ror(w[i - 15], 18) ^ (w[i - 15] >> 3);
      s1 = ror(w[i - 2], 17) ^ ror(w[i - 2], 19) ^ (w[i - 2] >> 10);
      w[i] = w[i - 16] + s0 + w[i - 7] + s1;
   }

   for (i = 0; i < 64; i++)
   {
      uint32_t ch;
      uint32_t maj;
      uint32_t t1;
      uint32_t t2;

      s0 = ror(a, 2) ^ ror(a, 13) ^ ror(a, 22);
      maj = (a & b) ^ (a & c) ^ (b & c);
      t2 = s0 + maj;
      s1 = ror(e, 6) ^ ror(e, 11) ^ ror(e, 25);
      ch = (e & f) ^ (~e & g);
      t1 = h + s1 + ch + k[i] + w[i];

      h = g;
      g = f;
      f = e;
      e = d + t1;
      d = c;
      c = b;
      b = a;
      a = t1 + t2;
   }

   sha256[0] += a;
   sha256[1] += b;
   sha256[2] += c;
   sha256[3] += d;
   sha256[4] += e;
   sha256[5] += f;
   sha256[6] += g;
   sha256[7] += h;
}

/****************************************************************************
 *
 ****************************************************************************/
void sha256sum(uint32_t sha256[8], void* msg, uint32_t size)
{
   uint8_t* msg8 = (uint8_t*) msg;
   uint32_t remainder = size % 64;
   uint8_t padding[64];
   uint32_t i;

   size -= remainder;

   sha256[0] = 0x6a09e667;
   sha256[1] = 0xbb67ae85;
   sha256[2] = 0x3c6ef372;
   sha256[3] = 0xa54ff53a;
   sha256[4] = 0x510e527f;
   sha256[5] = 0x9b05688c;
   sha256[6] = 0x1f83d9ab;
   sha256[7] = 0x5be0cd19;

   for (i = 0; i < size; i += 64)
      sha256chunk(sha256, (uint32_t*) &msg8[i]);

   for (i = 0; i < remainder; i++)
      padding[i] = msg8[size + i];

   padding[remainder] = 0x80;

   if (remainder >= (64 - 8))
   {
      for (i = remainder + 1; i < 64; i++)
         padding[i] = 0x00;

      sha256chunk(sha256, (uint32_t*) padding);

      for (i = 0; i < 60; i++)
         padding[i] = 0x00;
   }
   else
   {
      for (i = remainder + 1; i < 60; i++)
         padding[i] = 0x00;
   }

   *(uint32_t*) &padding[60] = byteSwap32(8 * (size + remainder));

   sha256chunk(sha256, (uint32_t*) padding);
}
