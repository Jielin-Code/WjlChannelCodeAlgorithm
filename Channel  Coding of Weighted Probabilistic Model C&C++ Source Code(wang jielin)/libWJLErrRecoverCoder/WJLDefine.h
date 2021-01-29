#ifndef _WJLDEFINE_H
#define _WJLDEFINE_H

#include "stdlib.h"
#include "math.h"

typedef enum
{
	KEEPBACK_NULL	=	0,		// No symbol in front
	KEEPBACK_ZERO,				// symbol 0 in front
	KEEPBACK_ONE,				// symbol 1 in front
	KEEPBACK_ONEZERO			// symbol 1 and 0 in front
}KEEPBACK_SYMBOL;

// Weighted Probability Model Encoding Core
typedef struct
{
	unsigned int RC_SHIFT_BITS;
	unsigned int RC_MAX_RANGE;
	unsigned int RC_MIN_RANGE;

	unsigned char *in_buff;

	unsigned char *out_buff;
	unsigned int out_buff_size;

	unsigned int FLow;	
	unsigned int FRange;
	unsigned int FDigits;
	unsigned int FFollow;

	double	dzerochange;		// Probability of symbol 0
	double	donechange;			// Probability of symbol 1
}WJL_ERRRECOVERY_ENCODER;

// Weighted Probability Model Decoding Core
typedef struct
{
	unsigned int RC_SHIFT_BITS;
	unsigned int RC_MAX_RANGE;
	unsigned int RC_MIN_RANGE;

	unsigned char *out_buff; 
	unsigned int out_buff_size;

	unsigned char *in_buff;
	unsigned int in_buff_rest;
	unsigned int in_buff_pos;

	unsigned int FLow;
	unsigned int FRange;
	unsigned int Values;

	unsigned char mask;
	unsigned char outByte;

	double	dzerochange;
	double	donechange;

	KEEPBACK_SYMBOL keepBackSymbol;
}WJL_ERRRECOVERY_DECODER;

// Operation Buffer
typedef struct
{
	unsigned int Values;
	unsigned int FLow;
	unsigned int FRange;
	
	unsigned int out_buff_size;
	unsigned int in_buff_rest;
	unsigned char mask;

	unsigned int in_buff_pos;

	KEEPBACK_SYMBOL keepBackSymbol;
}TEMP_Values;

extern const unsigned char bitOfByteTable[256][8];
extern const unsigned int powoftwo[32];

#endif
