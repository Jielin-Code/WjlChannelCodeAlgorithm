#ifndef _LIBWJLPARSEBMP_H
#define _LIBWJLPARSEBMP_H

#ifdef	__cplusplus
extern "C" {
#endif


int WJL_MemoryToFile(const char* szFilePath,const unsigned char* szData, const unsigned int *ulLen);

unsigned char* WJL_FileToMemory(const char* szFilePath,unsigned int* ulLen);

#ifdef	__cplusplus
}
#endif

#endif
