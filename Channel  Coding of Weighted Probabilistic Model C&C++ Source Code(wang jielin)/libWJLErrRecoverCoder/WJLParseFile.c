#include "libWJLParseFile.h"
#include "malloc.h"
#include "stdio.h"

// ---------------------------------------------------------------------------------------------
int WJL_MemoryToFile( const char* szFilePath,const unsigned char* szData, const unsigned int *ulLen )
{
	FILE* pOut;

	if (NULL == szFilePath || NULL == szData || NULL == ulLen)
	{
		return -1;		// 参数错误
	}

	pOut = fopen(szFilePath,"wb");
	if (NULL == pOut)
	{
		return 1;		// 文件打开失败
	}

	fwrite(szData,*ulLen,1,pOut);

	fclose(pOut);
	return 0;
}

// ---------------------------------------------------------------------------------------------
unsigned char* WJL_FileToMemory( const char* szFilePath,unsigned int* ulLen )
{
	FILE* pOut;
	unsigned char *szBuffer;

	if (NULL == szFilePath || NULL == ulLen)
	{
		return NULL;		// 参数错误
	}

	pOut = NULL;
	szBuffer = NULL;
	*ulLen = 0;

	pOut = fopen(szFilePath,"rb");
	if (NULL == pOut)
	{
		return NULL;		// 文件打开失败
	}

	fseek (pOut,0,SEEK_END);// 将文件指针移动文件结尾
	*ulLen = ftell(pOut);	// 求出当前文件指针距离文件开始的字节数

	szBuffer = (unsigned char*)malloc(*ulLen);
	if (NULL == szBuffer)
	{
		fclose(pOut);
		return NULL;
	}

	fseek (pOut,0,SEEK_SET);
	fread(szBuffer,*ulLen,1,pOut);

	fclose(pOut);
	return szBuffer;
}
