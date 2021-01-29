#ifndef _WJLERRRECOVERYDECODE_H
#define _WJLERRRECOVERYDECODE_H

#include "WJLDefine.h"

void ErrRecovery_Init(WJL_ERRRECOVERY_DECODER *erDecoder,unsigned char *inbuff,const unsigned int *unInfbuffLen);
unsigned char ErrRecovery_Agent(WJL_ERRRECOVERY_DECODER *erDecoder,const unsigned int *unInfbuffLen,unsigned  int *unPosErr);
void Decode_Init(WJL_ERRRECOVERY_DECODER *erDecoder,const unsigned int *unOutbufferLen);
void Decode_Agent(WJL_ERRRECOVERY_DECODER *erDecoder,const unsigned int *unOutbufferLen);

#endif
