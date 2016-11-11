#ifndef __SFR_HELPER__
#define __SFR_HELPER__

#define _V(SFR, N)     ((SFR >> N) & 1)                // Bit value
#define _SB(SFR, N)     SFR |= (1 << N)                 // Set N-th bit
#define _CB(SFR, N)     SFR &=~(1 << N)                 // Celear N-th bit
#define _NB(SFR, N, X)  SFR ^= (-X ^ SFR) & (1 << N)    // Value of N-t bit = X X->[0,1]

typedef unsigned char UCHAR;
typedef UCHAR uint8_t;
typedef unsigned int uint16_t;
#endif /* __SFR_HELPER__ */
