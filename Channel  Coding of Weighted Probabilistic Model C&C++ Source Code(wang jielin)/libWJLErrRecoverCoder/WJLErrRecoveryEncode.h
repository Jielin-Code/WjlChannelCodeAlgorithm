#ifndef _WJLERRRECOVERYENCODE_H
#define _WJLERRRECOVERYENCODE_H

#include "WJLDefine.h"

void Encode_Init(WJL_ERRRECOVERY_ENCODER *erEncoder,const unsigned char *inbuff,const unsigned int *inbufflen);
void Encode_Agent(WJL_ERRRECOVERY_ENCODER *erEncoder,unsigned char *ucInBuffer,const unsigned int *unInbufferLen);
void Encode_End(WJL_ERRRECOVERY_ENCODER *erEncoder);

#endif