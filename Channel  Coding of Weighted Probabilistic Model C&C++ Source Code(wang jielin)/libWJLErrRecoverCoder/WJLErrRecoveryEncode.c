#include "WJLErrRecoveryEncode.h"

//--------------------------------------------------------------------------------------------------------------------
void Encode_Init(WJL_ERRRECOVERY_ENCODER *erEncoder,const unsigned char *inbuff,const unsigned int *inbufflen)
{
	erEncoder->RC_SHIFT_BITS = 23;
	erEncoder->RC_MIN_RANGE = 1<<erEncoder->RC_SHIFT_BITS;
	erEncoder->RC_MAX_RANGE = 1<<31;	

	erEncoder->donechange = 1.0;
	// According to my paper, the probability of symbol 0 is 1/3 or 1/3.999
	erEncoder->dzerochange = 1/3.0;

	erEncoder->in_buff = (unsigned char*)inbuff;

	erEncoder->out_buff_size = 0;
	erEncoder->out_buff = (unsigned char*)malloc(*inbufflen*(-log(erEncoder->dzerochange)/log(2.0))+100);

	erEncoder->FLow = erEncoder->RC_MAX_RANGE;
	erEncoder->FRange = erEncoder->RC_MAX_RANGE;
	erEncoder->FDigits = 0;
	erEncoder->FFollow = 0;
}

void Encode_UpdateRange(WJL_ERRRECOVERY_ENCODER *erEncoder,const unsigned char *symbol)
{
	unsigned int i,High;
	
	if (1 == *symbol)
	{
		erEncoder->FLow += (unsigned int)(erEncoder->FRange*erEncoder->dzerochange);
		return;
	}

	erEncoder->FRange *= erEncoder->dzerochange;

	if(erEncoder->FRange<=erEncoder->RC_MIN_RANGE)
	{
		High = erEncoder->FLow + erEncoder->FRange-1;
		if(erEncoder->FFollow!=0)
		{
			if (High <= erEncoder->RC_MAX_RANGE) 
			{
				erEncoder->out_buff[erEncoder->out_buff_size++] = erEncoder->FDigits;
				for (i = 1; i <= erEncoder->FFollow - 1; i++)
				{
					erEncoder->out_buff[erEncoder->out_buff_size++] = 0xFF;
				}
				erEncoder->FFollow = 0;
				erEncoder->FLow += erEncoder->RC_MAX_RANGE;
			} 
			else if (erEncoder->FLow >= erEncoder->RC_MAX_RANGE) 
			{
				erEncoder->out_buff[erEncoder->out_buff_size++] = erEncoder->FDigits + 1;
				for (i = 1; i <= erEncoder->FFollow - 1; i++)
				{
					erEncoder->out_buff[erEncoder->out_buff_size++] = 0x00;
				}									
				erEncoder->FFollow = 0;
			} 
			else
			{				
				erEncoder->FFollow++;				
				erEncoder->FLow = (erEncoder->FLow << 8) & (erEncoder->RC_MAX_RANGE - 1);  // 扩展区间RC_MAX_RANGE - 1得到的是1个0后面31个1，进行与运算就使得FLow的最高位被抹去
				erEncoder->FRange <<= 8;
				return;
			}
		}

		if (((erEncoder->FLow ^ High) & (0xFF << erEncoder->RC_SHIFT_BITS)) == 0) 
		{
			erEncoder->out_buff[erEncoder->out_buff_size++] = erEncoder->FLow>>erEncoder->RC_SHIFT_BITS;
		}
		else
		{
			erEncoder->FLow -= erEncoder->RC_MAX_RANGE;
			erEncoder->FDigits = erEncoder->FLow >> erEncoder->RC_SHIFT_BITS;
			erEncoder->FFollow = 1;
		}
		erEncoder->FLow = (erEncoder->FLow << 8) & (erEncoder->RC_MAX_RANGE - 1);
		erEncoder->FRange <<= 8;
	}
}

//--------------------------------------------------------------------------------------------------------------------
void Encode_Agent(WJL_ERRRECOVERY_ENCODER *erEncoder,unsigned char *ucInBuffer,const unsigned int *unInbufferLen)
{
	unsigned int i,j;
	unsigned char symbol;

	for (i=0;i<*unInbufferLen;++i)
	{
		for (j=0;j<8;j++)
		{
			//Pretreatment :0->101,1->01
			symbol = bitOfByteTable[ucInBuffer[i]][j];
			if (0x00 == symbol)
			{
				symbol = 0x01;
				Encode_UpdateRange(erEncoder,&symbol);
			}
			symbol = 0x00;
			Encode_UpdateRange(erEncoder,&symbol);
			symbol = 0x01;
			Encode_UpdateRange(erEncoder,&symbol);
		}
	}
}

void Encode_End(WJL_ERRRECOVERY_ENCODER *erEncoder)
{
	unsigned int n = 0;
	if (erEncoder->FFollow != 0) 
	{
		if (erEncoder->FLow < erEncoder->RC_MAX_RANGE) 
		{
			erEncoder->out_buff[erEncoder->out_buff_size++] = erEncoder->FDigits;
			for (n = 1; n <= erEncoder->FFollow - 1; n++)
			{
				erEncoder->out_buff[erEncoder->out_buff_size++] = 0xFF;
			}
		} 
		else 
		{
			erEncoder->out_buff[erEncoder->out_buff_size++] = erEncoder->FDigits + 1;
			for (n = 1; n <= erEncoder->FFollow - 1; n++)
			{
				erEncoder->out_buff[erEncoder->out_buff_size++] = 0x00;
			}
		}
	}
	erEncoder->FLow = erEncoder->FLow << 1;
	n = sizeof(unsigned int)*8;
	do{
		n -= 8;
		erEncoder->out_buff[erEncoder->out_buff_size++] = erEncoder->FLow >> n;
	} while ( n > 0 );
}
