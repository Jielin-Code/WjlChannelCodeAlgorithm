#ifndef _LIBWJLERRRECOVERYCODER_H
#define _LIBWJLERRRECOVERYCODER_H

#ifdef	__cplusplus
extern "C" {
#endif

unsigned char *WJL_ErrRecovery_Encode_Buff(unsigned char *ucInBuffer,const unsigned int *unInbufferLen,unsigned int *unOutbufferLen);

unsigned char *WJL_ErrRecovery_Decode_Buff(unsigned char *ucInBuffer,const unsigned int *unInbufferLen,unsigned int *unOutbufferLen,unsigned int *unPosErr);

#ifdef	__cplusplus
}
#endif

#endif
