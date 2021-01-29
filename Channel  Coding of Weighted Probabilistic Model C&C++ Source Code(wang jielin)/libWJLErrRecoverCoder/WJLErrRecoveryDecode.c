#include "WJLErrRecoveryDecode.h"
#include "string.h"

void ErrRecovery_Init(WJL_ERRRECOVERY_DECODER *erDecoder,unsigned char *inbuff,const unsigned int *unInfbuffLen)
{
	int n = sizeof(unsigned int)*8;

	erDecoder->RC_SHIFT_BITS = 23;
	erDecoder->RC_MIN_RANGE = 1<<erDecoder->RC_SHIFT_BITS;
	erDecoder->RC_MAX_RANGE = 1<<31;	
	erDecoder->FLow = 0;
	erDecoder->FRange = erDecoder->RC_MAX_RANGE;

	erDecoder->donechange = 1.0;
	// According to my paper, the probability of symbol 0 is 1/3 or 1/3.999
	erDecoder->dzerochange = 1/3.0;

	erDecoder->out_buff = NULL;
	erDecoder->out_buff_size = 0;

	erDecoder->in_buff = inbuff;
	erDecoder->in_buff_rest = *unInfbuffLen;
	erDecoder->in_buff_pos = 0;

	do{
		n -= 8;
		erDecoder->Values = (erDecoder->Values << 8) | erDecoder->in_buff[erDecoder->in_buff_pos++];
		erDecoder->in_buff_rest--;
	} while(n>0);

	erDecoder->mask = 0x01;
	erDecoder->outByte = 0x00;

	erDecoder->keepBackSymbol = KEEPBACK_NULL;
}

// Compare the interval superscripts of symbol 0 with V values
unsigned char Decode_GetSymbol(WJL_ERRRECOVERY_DECODER *erDecoder,unsigned char *ucAppearErr,unsigned char isCheckErr)
{
	unsigned char symbol;
	unsigned int H0 = erDecoder->FLow,values = erDecoder->Values >> 1;

	if (values < erDecoder->FLow)
	{
		values += erDecoder->RC_MAX_RANGE;
	}

	H0 += (unsigned int)(erDecoder->FRange*erDecoder->dzerochange);
	symbol = values<H0?0:1;

	if (KEEPBACK_NULL == erDecoder->keepBackSymbol)
	{
		erDecoder->keepBackSymbol = (0x00==symbol?KEEPBACK_ZERO:KEEPBACK_ONE);
	}
	else if (KEEPBACK_ZERO == erDecoder->keepBackSymbol)
	{
		if (0x00 == symbol)
		{// Left boundary 00: error
			*ucAppearErr = 0x01;	
		}
		else
		{// Processing of 01: Output 1
			erDecoder->mask <<= 1;
			if (0x00 == isCheckErr)
			{
				erDecoder->outByte <<= 1;
				erDecoder->outByte |= 0x01;
			}			
			if (0x00 == erDecoder->mask)
			{
				if (0x00 == isCheckErr)
				{
					erDecoder->out_buff[erDecoder->out_buff_size] = erDecoder->outByte;
					erDecoder->outByte = 0x00;
				}				
				erDecoder->out_buff_size++;				
				erDecoder->mask = 0x01;
			}

			erDecoder->keepBackSymbol = KEEPBACK_NULL;
		}
	}
	else if (KEEPBACK_ONE == erDecoder->keepBackSymbol)
	{
		if (0x00 == symbol)
		{// Treatment of 10: retention 10
			erDecoder->keepBackSymbol = KEEPBACK_ONEZERO;
		}
		else
		{// Left bound appears 11: Error
			*ucAppearErr = 0x01;
		}
	}
	else
	{
		if (0x00 == symbol)
		{// Left boundary 100: error
			*ucAppearErr = 0x01;
		}
		else
		{// Treatment of 101: output 0
			erDecoder->mask <<= 1;
			if (0x00 == isCheckErr)
			{
				erDecoder->outByte <<= 1;
			}			
			if (0x00 == erDecoder->mask)
			{
				if (0x00 == isCheckErr)
				{
					erDecoder->out_buff[erDecoder->out_buff_size] = erDecoder->outByte;
					erDecoder->outByte = 0x00;
				}
				erDecoder->out_buff_size++;
				erDecoder->mask = 0x01;
			}

			erDecoder->keepBackSymbol = KEEPBACK_NULL;
		}
	}

	return symbol;
}

unsigned char Decode_UpdateRange(WJL_ERRRECOVERY_DECODER *erDecoder,const unsigned char *symbol)
{
	if (1 == *symbol)
	{
		erDecoder->FLow += (unsigned int)(erDecoder->FRange*erDecoder->dzerochange);
		return 0x01;
	}

	erDecoder->FRange *= erDecoder->dzerochange;

	if (erDecoder->FRange <= erDecoder->RC_MIN_RANGE) 
	{
		erDecoder->FLow = (erDecoder->FLow << 8) & (erDecoder->RC_MAX_RANGE - 1);
		erDecoder->FRange <<= 8;
		if (erDecoder->in_buff_rest > 0)
		{
			erDecoder->Values = (erDecoder->Values << 8) | erDecoder->in_buff[erDecoder->in_buff_pos++];
			erDecoder->in_buff_rest --;
			return 0x00;
		}
		
		return 0x02;	// The input buffer is exhausted
	}

	return 0x01;		// Detection of current v values not completed
}

// Error checking for the current v value, return 0 x00 means the v value is correct, return 0 x01 is found, return 0 means input buffer is exhausted
unsigned char Detect_Core(WJL_ERRRECOVERY_DECODER *erDecoder)
{
	unsigned char symbol,ucAppearErr=0x00,ucRes=0x00;

	// Check v values for bit transmission errors
	while (1)
	{
		symbol = Decode_GetSymbol(erDecoder,&ucAppearErr,1);
		if (ucAppearErr)
		{// Check not pass, end decode
			return 0x01;
		}

		ucRes = Decode_UpdateRange(erDecoder,&symbol);
		if (0x01 == ucRes)
		{// Continuous detection when the current v value is not detected
			continue;
		}

		// The v value passes the check and the v value is updated, then the next v value is, and if the input buffer the input buffer is exhausted
		return ucRes;
	}
}

// Pre-decode a certain number of V values, return 0 x00 means through ucPreCount pre-decode, return 0 x01 for not passing the pre-decode, return 0 for the end but not sure if there is a problem
unsigned char Detect_Agent(WJL_ERRRECOVERY_DECODER *erDecoder,unsigned char ucPreCount,short bitPos)
{
	TEMP_Values tValues;
	unsigned char ucCount = 0x00,ucRes=0x00;
	do
	{
		ucRes = Detect_Core(erDecoder);
		if (ucRes)
		{
			return ucRes;
		}		
		// No errors found, continue to pre-decode
		if (ucCount == bitPos/8)
		{
			tValues.Values = erDecoder->Values;
			tValues.FLow = erDecoder->FLow;
			tValues.FRange = erDecoder->FRange;
			tValues.out_buff_size = erDecoder->out_buff_size;
			tValues.mask = erDecoder->mask;
			tValues.in_buff_rest = erDecoder->in_buff_rest;
			tValues.in_buff_pos = erDecoder->in_buff_pos;
			tValues.keepBackSymbol = erDecoder->keepBackSymbol;
		}
		ucCount ++;
	} while (ucCount < ucPreCount);

	if (ucCount == ucPreCount)
	{// Each V value of the updated backup
		erDecoder->Values = tValues.Values;
		erDecoder->FLow = tValues.FLow;
		erDecoder->FRange = tValues.FRange;
		erDecoder->out_buff_size = tValues.out_buff_size;
		erDecoder->mask = tValues.mask;
		erDecoder->in_buff_rest = tValues.in_buff_rest;
		erDecoder->in_buff_pos = tValues.in_buff_pos;
		erDecoder->keepBackSymbol = tValues.keepBackSymbol;
	}
	return 0x00;
}

void Rollback_Core(WJL_ERRRECOVERY_DECODER *erDecoder,TEMP_Values *tempValues,unsigned char ucBackBits)
{
	unsigned char ucDex = 13 - ucBackBits/8;

	erDecoder->Values = tempValues[ucDex].Values;
	erDecoder->FLow = tempValues[ucDex].FLow;
	erDecoder->FRange = tempValues[ucDex].FRange;
	erDecoder->keepBackSymbol = tempValues[ucDex].keepBackSymbol;
	erDecoder->mask = tempValues[ucDex].mask;
	erDecoder->in_buff_rest = tempValues[ucDex].in_buff_rest;
	erDecoder->out_buff_size = tempValues[ucDex].out_buff_size;
	erDecoder->in_buff_pos = tempValues[ucDex].in_buff_pos;
}

// Back the V value and the corresponding parameters to the corresponding item of the backup as required
void Rollback_Agent(WJL_ERRRECOVERY_DECODER *erDecoder,TEMP_Values *tempValues,short bitPos)
{
	short k;
	for (k=0;k<=13;++k)
	{
		if ((k==0&&tempValues[k].in_buff_pos>=4)
		  ||(tempValues[k].in_buff_pos==4))
		{
			if (bitPos<104-(k-1)*8)
			{
				Rollback_Core(erDecoder,tempValues,bitPos/8*8);
			}
			else
			{
				Rollback_Core(erDecoder,tempValues,96-(k-1)*8);
			}
			return;
		}
	}
}

// Bits that flip pos positions
void Reverse_Core(unsigned int *values,unsigned char pos)
{
	(((*values>>(pos-1))&0x01)==0x00)?(*values += powoftwo[pos-1]):(*values -= powoftwo[pos-1]);
}

// The bit of bitPos%8 or 7-(- bitPos-1)%8 of the flip unBytePos position
void Reverse_Agent(WJL_ERRRECOVERY_DECODER *erDecoder,TEMP_Values *tempValues,short bitPos)
{
	unsigned int unBytePos;
	unsigned char symbol;
	short k;

	if (bitPos < 0)
	{
		unBytePos = tempValues[13].in_buff_pos+(-bitPos-1)/8;
		symbol = bitOfByteTable[erDecoder->in_buff[unBytePos]][(-bitPos-1)%8];
		symbol==0x00?(erDecoder->in_buff[unBytePos] += powoftwo[7-(-bitPos-1)%8]):(erDecoder->in_buff[unBytePos] -= powoftwo[7-(-bitPos-1)%8]);
		return;
	}

	unBytePos = tempValues[13].in_buff_pos-1-bitPos/8;
	symbol = bitOfByteTable[erDecoder->in_buff[unBytePos]][7-bitPos%8];
	symbol==0x00?(erDecoder->in_buff[unBytePos] += powoftwo[bitPos%8]):(erDecoder->in_buff[unBytePos] -= powoftwo[bitPos%8]);
	for (k=0;k<=13;++k)
	{
		if ((k==0&&tempValues[k].in_buff_pos>=4)
		  ||(tempValues[k].in_buff_pos==4))
		{
			if (bitPos<112-k*8)
			{
				Reverse_Core(&(tempValues[13-bitPos/8].Values),bitPos%8+1);
			}
			else if (bitPos<120-k*8)
			{
				Reverse_Core(&(tempValues[k].Values),bitPos%8+9);
			}
			else if (bitPos<128-k*8)
			{
				Reverse_Core(&(tempValues[k].Values),bitPos%8+17);
			}
			else
			{
				Reverse_Core(&(tempValues[k].Values),bitPos%8+25);
			}
			return;
		}
	}
}

// Returns 0 x00 for successful correction ,0 x01 for failure ,0 for uncertainty
unsigned char ErrRecovery_Core_OneErr(WJL_ERRRECOVERY_DECODER *erDecoder,TEMP_Values *tempValues)
{
	unsigned char ucPreCount,ucRes=0x00;
	short i;

	for (i=0;i<112;++i)
	{// Bits that flip i positions
		Reverse_Agent(erDecoder,tempValues,i);
		// Back the V value and the corresponding parameters to the corresponding item of the backup as required
		Rollback_Agent(erDecoder,tempValues,i);
		// Number V value ucPreCount precode
		ucPreCount=erDecoder->in_buff_rest>=16?16:erDecoder->in_buff_rest+1;
		ucRes = Detect_Agent(erDecoder,ucPreCount,i);
		if (0x01 == ucRes)
		{// Bits that restore i positions
			Reverse_Agent(erDecoder,tempValues,i);
			continue;
		}
		return ucRes;
	}

	return 0x01;
}

//Correct only two bit errors, Returns x00 for successful correction and x01 for failure to correct
unsigned char ErrRecovery_Core_TwoErr(WJL_ERRRECOVERY_DECODER *erDecoder,TEMP_Values *tempValues)
{
	unsigned char ucPreCount,ucRes=0x00;
	short i,j;	

	for (i=0;i<112;++i)
	{// Bits that flip i positions
		Reverse_Agent(erDecoder,tempValues,i);
		for (j=i-1;j>=-104+i/8*8;--j)
		{// Bits that flip j positions
			Reverse_Agent(erDecoder,tempValues,j);
			// Back the V value and the corresponding parameters to the corresponding item of the backup as required
			Rollback_Agent(erDecoder,tempValues,i);
			// Number V value ucPreCount precode
			ucPreCount=erDecoder->in_buff_rest>=16?16:erDecoder->in_buff_rest+1;
			ucRes = Detect_Agent(erDecoder,ucPreCount,i);
			if (0x01 == ucRes)
			{// Bits that restore j positions
				Reverse_Agent(erDecoder,tempValues,j);
				continue;
			}
			return ucRes;
		}
		// Bits that restore i positions
		Reverse_Agent(erDecoder,tempValues,i);
	}

	return 0x01;
}

unsigned char ErrRecovery_Core_ThreeErr(WJL_ERRRECOVERY_DECODER *erDecoder,TEMP_Values *tempValues)
{
	unsigned char ucPreCount,ucRes=0x00;
	short i,j,l;

	for (i=0;i<112;++i)
	{
		Reverse_Agent(erDecoder,tempValues,i);
		for (j=i-1;j>=-103+i/8*8;--j)
		{
			Reverse_Agent(erDecoder,tempValues,j);
			for (l=j-1;l>=-104+i/8*8;--l)
			{
				Reverse_Agent(erDecoder,tempValues,l);
				Rollback_Agent(erDecoder,tempValues,i);
				ucPreCount=erDecoder->in_buff_rest>=16?16:erDecoder->in_buff_rest+1;
				ucRes = Detect_Agent(erDecoder,ucPreCount,i);
				if (0x01 == ucRes)
				{
					Reverse_Agent(erDecoder,tempValues,l);
					continue;
				}
				return ucRes;
			}
			Reverse_Agent(erDecoder,tempValues,j);
		}
		Reverse_Agent(erDecoder,tempValues,i);
	}

	return 0x01;
}

unsigned char ErrRecovery_Core_FourErr(WJL_ERRRECOVERY_DECODER *erDecoder,TEMP_Values *tempValues)
{
	unsigned char ucPreCount,ucRes=0x00;
	short i,j,l,m;

	for (i=0;i<112;++i)
	{
		Reverse_Agent(erDecoder,tempValues,i);
		for (j=i-1;j>=-102+i/8*8;--j)
		{
			Reverse_Agent(erDecoder,tempValues,j);
			for (l=j-1;l>=-103+i/8*8;--l)
			{
				Reverse_Agent(erDecoder,tempValues,l);
				for (m=l-1;m>=-104+i/8*8;--m)
				{
					Reverse_Agent(erDecoder,tempValues,m);
					Rollback_Agent(erDecoder,tempValues,i);
					ucPreCount=erDecoder->in_buff_rest>=16?16:erDecoder->in_buff_rest+1;
					ucRes = Detect_Agent(erDecoder,ucPreCount,i);
					if (0x01 == ucRes)
					{
						Reverse_Agent(erDecoder,tempValues,m);
						continue;
					}
					return ucRes;
				}
				Reverse_Agent(erDecoder,tempValues,l);
			}
			Reverse_Agent(erDecoder,tempValues,j);
		}
		Reverse_Agent(erDecoder,tempValues,i);
	}

	return 0x01;
}

unsigned char ErrRecovery_Core_FiveErr(WJL_ERRRECOVERY_DECODER *erDecoder,TEMP_Values *tempValues)
{
	unsigned char ucPreCount,ucRes=0x00;
	short i,j,l,m,n;

	for (i=0;i<112;++i)
	{
		Reverse_Agent(erDecoder,tempValues,i);
		for (j=i-1;j>=-101+i/8*8;--j)
		{
			Reverse_Agent(erDecoder,tempValues,j);
			for (l=j-1;l>=-102+i/8*8;--l)
			{
				Reverse_Agent(erDecoder,tempValues,l);
				for (m=l-1;m>=-103+i/8*8;--m)
				{
					Reverse_Agent(erDecoder,tempValues,m);
					for (n=m-1;n>=-104+i/8*8;--n)
					{
						Reverse_Agent(erDecoder,tempValues,n);
						Rollback_Agent(erDecoder,tempValues,i);
						ucPreCount=erDecoder->in_buff_rest>=16?16:erDecoder->in_buff_rest+1;
						ucRes = Detect_Agent(erDecoder,ucPreCount,i);
						if (0x01 == ucRes)
						{
							Reverse_Agent(erDecoder,tempValues,n);
							continue;
						}
						return ucRes;
					}
					Reverse_Agent(erDecoder,tempValues,m);
				}
				Reverse_Agent(erDecoder,tempValues,l);
			}
			Reverse_Agent(erDecoder,tempValues,j);
		}
		Reverse_Agent(erDecoder,tempValues,i);
	}

	return 0x01;
}

unsigned char ErrRecovery_Core_SixErr(WJL_ERRRECOVERY_DECODER *erDecoder,TEMP_Values *tempValues)
{
	unsigned char ucPreCount,ucRes=0x00;
	short i,j,l,m,n,g;

	for (i=0;i<112;++i)
	{
		Reverse_Agent(erDecoder,tempValues,i);
		for (j=i-1;j>=-100+i/8*8;--j)
		{
			Reverse_Agent(erDecoder,tempValues,j);
			for (l=j-1;l>=-101+i/8*8;--l)
			{
				Reverse_Agent(erDecoder,tempValues,l);
				for (m=l-1;m>=-102+i/8*8;--m)
				{
					Reverse_Agent(erDecoder,tempValues,m);
					for (n=m-1;n>=-103+i/8*8;--n)
					{
						Reverse_Agent(erDecoder,tempValues,n);
						for (g=n-1;g>=-104+i/8*8;--g)
						{
							Reverse_Agent(erDecoder,tempValues,g);
							Rollback_Agent(erDecoder,tempValues,i);
							ucPreCount=erDecoder->in_buff_rest>=16?16:erDecoder->in_buff_rest+1;
							ucRes = Detect_Agent(erDecoder,ucPreCount,i);
							if (0x01 == ucRes)
							{
								Reverse_Agent(erDecoder,tempValues,g);
								continue;
							}
							return ucRes;
						}					
						Reverse_Agent(erDecoder,tempValues,n);
					}
					Reverse_Agent(erDecoder,tempValues,m);
				}
				Reverse_Agent(erDecoder,tempValues,l);
			}
			Reverse_Agent(erDecoder,tempValues,j);
		}
		Reverse_Agent(erDecoder,tempValues,i);
	}

	return 0x01;
}

unsigned char ErrRecovery_Core_SevenErr(WJL_ERRRECOVERY_DECODER *erDecoder,TEMP_Values *tempValues)
{
	unsigned char ucPreCount,ucRes=0x00;
	short i,j,l,m,n,g,h;

	for (i=0;i<112;++i)
	{
		Reverse_Agent(erDecoder,tempValues,i);
		for (j=i-1;j>=-99+i/8*8;--j)
		{
			Reverse_Agent(erDecoder,tempValues,j);
			for (l=j-1;l>=-100+i/8*8;--l)
			{
				Reverse_Agent(erDecoder,tempValues,l);
				for (m=l-1;m>=-101+i/8*8;--m)
				{
					Reverse_Agent(erDecoder,tempValues,m);
					for (n=m-1;n>=-102+i/8*8;--n)
					{
						Reverse_Agent(erDecoder,tempValues,n);
						for (g=n-1;g>=-103+i/8*8;--g)
						{
							Reverse_Agent(erDecoder,tempValues,g);
							for (h=g-1;h>=-104+i/8*8;--h)
							{
								Reverse_Agent(erDecoder,tempValues,h);
								Rollback_Agent(erDecoder,tempValues,i);
								ucPreCount=erDecoder->in_buff_rest>=16?16:erDecoder->in_buff_rest+1;
								ucRes = Detect_Agent(erDecoder,ucPreCount,i);
								if (0x01 == ucRes)
								{
									Reverse_Agent(erDecoder,tempValues,h);
									continue;
								}
								return ucRes;
							}
							Reverse_Agent(erDecoder,tempValues,g);
						}
						Reverse_Agent(erDecoder,tempValues,n);
					}
					Reverse_Agent(erDecoder,tempValues,m);
				}
				Reverse_Agent(erDecoder,tempValues,l);
			}
			Reverse_Agent(erDecoder,tempValues,j);
		}
		Reverse_Agent(erDecoder,tempValues,i);
	}

	return 0x01;
}

unsigned char ErrRecovery_Core_EightErr(WJL_ERRRECOVERY_DECODER *erDecoder,TEMP_Values *tempValues)
{
	unsigned char ucPreCount,ucRes=0x00;
	short i,j,l,m,n,g,h,o;

	for (i=0;i<112;++i)
	{
		Reverse_Agent(erDecoder,tempValues,i);
		for (j=i-1;j>=-99+i/8*8;--j)
		{
			Reverse_Agent(erDecoder,tempValues,j);
			for (l=j-1;l>=-100+i/8*8;--l)
			{
				Reverse_Agent(erDecoder,tempValues,l);
				for (m=l-1;m>=-101+i/8*8;--m)
				{
					Reverse_Agent(erDecoder,tempValues,m);
					for (n=m-1;n>=-102+i/8*8;--n)
					{
						Reverse_Agent(erDecoder,tempValues,n);
						for (g=n-1;g>=-103+i/8*8;--g)
						{
							Reverse_Agent(erDecoder,tempValues,g);
							for (h=g-1;h>=-104+i/8*8;--h)
							{
								Reverse_Agent(erDecoder,tempValues,h);
								for (o=h-1;o>=-105+i/8*8;--o)
								{
									Reverse_Agent(erDecoder,tempValues,o);
									Rollback_Agent(erDecoder,tempValues,i);
									ucPreCount=erDecoder->in_buff_rest>=16?16:erDecoder->in_buff_rest+1;
									ucRes = Detect_Agent(erDecoder,ucPreCount,i);
									if (0x01 == ucRes)
									{
										Reverse_Agent(erDecoder,tempValues,o);
										continue;
									}
									return ucRes;
								}
								Reverse_Agent(erDecoder,tempValues,h);
							}
							Reverse_Agent(erDecoder,tempValues,g);
						}
						Reverse_Agent(erDecoder,tempValues,n);
					}
					Reverse_Agent(erDecoder,tempValues,m);
				}
				Reverse_Agent(erDecoder,tempValues,l);
			}
			Reverse_Agent(erDecoder,tempValues,j);
		}
		Reverse_Agent(erDecoder,tempValues,i);
	}

	return 0x01;
}

unsigned char ErrRecovery_Core_NineErr(WJL_ERRRECOVERY_DECODER *erDecoder,TEMP_Values *tempValues)
{
	unsigned char ucPreCount,ucRes=0x00;
	short i,j,l,m,n,g,h,o,p;

	for (i=0;i<112;++i)
	{
		Reverse_Agent(erDecoder,tempValues,i);
		for (j=i-1;j>=-99+i/8*8;--j)
		{
			Reverse_Agent(erDecoder,tempValues,j);
			for (l=j-1;l>=-100+i/8*8;--l)
			{
				Reverse_Agent(erDecoder,tempValues,l);
				for (m=l-1;m>=-101+i/8*8;--m)
				{
					Reverse_Agent(erDecoder,tempValues,m);
					for (n=m-1;n>=-102+i/8*8;--n)
					{
						Reverse_Agent(erDecoder,tempValues,n);
						for (g=n-1;g>=-103+i/8*8;--g)
						{
							Reverse_Agent(erDecoder,tempValues,g);
							for (h=g-1;h>=-104+i/8*8;--h)
							{
								Reverse_Agent(erDecoder,tempValues,h);
								for (o=h-1;o>=-105+i/8*8;--o)
								{
									Reverse_Agent(erDecoder,tempValues,o);
									for (p=o-1;p>=-106+i/8*8;--p)
									{
										Reverse_Agent(erDecoder,tempValues,p);
										Rollback_Agent(erDecoder,tempValues,i);
										ucPreCount=erDecoder->in_buff_rest>=16?16:erDecoder->in_buff_rest+1;
										ucRes = Detect_Agent(erDecoder,ucPreCount,i);
										if (0x01 == ucRes)
										{
											Reverse_Agent(erDecoder,tempValues,p);
											continue;
										}
										return ucRes;
									}
									Reverse_Agent(erDecoder,tempValues,o);
								}
								Reverse_Agent(erDecoder,tempValues,h);
							}
							Reverse_Agent(erDecoder,tempValues,g);
						}
						Reverse_Agent(erDecoder,tempValues,n);
					}
					Reverse_Agent(erDecoder,tempValues,m);
				}
				Reverse_Agent(erDecoder,tempValues,l);
			}
			Reverse_Agent(erDecoder,tempValues,j);
		}
		Reverse_Agent(erDecoder,tempValues,i);
	}

	return 0x01;
}

unsigned char ErrRecovery_Core_TenErr(WJL_ERRRECOVERY_DECODER *erDecoder,TEMP_Values *tempValues)
{
	unsigned char ucPreCount,ucRes=0x00;
	short i,j,l,m,n,g,h,o,p,q;

	for (i=0;i<112;++i)
	{
		Reverse_Agent(erDecoder,tempValues,i);
		for (j=i-1;j>=-99+i/8*8;--j)
		{
			Reverse_Agent(erDecoder,tempValues,j);
			for (l=j-1;l>=-100+i/8*8;--l)
			{
				Reverse_Agent(erDecoder,tempValues,l);
				for (m=l-1;m>=-101+i/8*8;--m)
				{
					Reverse_Agent(erDecoder,tempValues,m);
					for (n=m-1;n>=-102+i/8*8;--n)
					{
						Reverse_Agent(erDecoder,tempValues,n);
						for (g=n-1;g>=-103+i/8*8;--g)
						{
							Reverse_Agent(erDecoder,tempValues,g);
							for (h=g-1;h>=-104+i/8*8;--h)
							{
								Reverse_Agent(erDecoder,tempValues,h);
								for (o=h-1;o>=-105+i/8*8;--o)
								{
									Reverse_Agent(erDecoder,tempValues,o);
									for (p=o-1;p>=-106+i/8*8;--p)
									{
										Reverse_Agent(erDecoder,tempValues,p);
										for (q=p-1;q>=-107+i/8*8;--q)
										{
											Reverse_Agent(erDecoder,tempValues,q);
											Rollback_Agent(erDecoder,tempValues,i);
											ucPreCount=erDecoder->in_buff_rest>=16?16:erDecoder->in_buff_rest+1;
											ucRes = Detect_Agent(erDecoder,ucPreCount,i);
											if (0x01 == ucRes)
											{
												Reverse_Agent(erDecoder,tempValues,q);
												continue;
											}
											return ucRes;
										}
										Reverse_Agent(erDecoder,tempValues,p);
									}
									Reverse_Agent(erDecoder,tempValues,o);
								}
								Reverse_Agent(erDecoder,tempValues,h);
							}
							Reverse_Agent(erDecoder,tempValues,g);
						}
						Reverse_Agent(erDecoder,tempValues,n);
					}
					Reverse_Agent(erDecoder,tempValues,m);
				}
				Reverse_Agent(erDecoder,tempValues,l);
			}
			Reverse_Agent(erDecoder,tempValues,j);
		}
		Reverse_Agent(erDecoder,tempValues,i);
	}

	return 0x01;
}

unsigned char ErrRecovery_Core_ElevenErr(WJL_ERRRECOVERY_DECODER *erDecoder,TEMP_Values *tempValues)
{
	unsigned char ucPreCount,ucRes=0x00;
	short i,j,l,m,n,g,h,o,p,q,r;

	for (i=0;i<112;++i)
	{
		Reverse_Agent(erDecoder,tempValues,i);
		for (j=i-1;j>=-99+i/8*8;--j)
		{
			Reverse_Agent(erDecoder,tempValues,j);
			for (l=j-1;l>=-100+i/8*8;--l)
			{
				Reverse_Agent(erDecoder,tempValues,l);
				for (m=l-1;m>=-101+i/8*8;--m)
				{
					Reverse_Agent(erDecoder,tempValues,m);
					for (n=m-1;n>=-102+i/8*8;--n)
					{
						Reverse_Agent(erDecoder,tempValues,n);
						for (g=n-1;g>=-103+i/8*8;--g)
						{
							Reverse_Agent(erDecoder,tempValues,g);
							for (h=g-1;h>=-104+i/8*8;--h)
							{
								Reverse_Agent(erDecoder,tempValues,h);
								for (o=h-1;o>=-105+i/8*8;--o)
								{
									Reverse_Agent(erDecoder,tempValues,o);
									for (p=o-1;p>=-106+i/8*8;--p)
									{
										Reverse_Agent(erDecoder,tempValues,p);
										for (q=p-1;q>=-107+i/8*8;--q)
										{
											Reverse_Agent(erDecoder,tempValues,q);
											for (r=q-1;r>=-108+i/8*8;--r)
											{
												Reverse_Agent(erDecoder,tempValues,r);
												Rollback_Agent(erDecoder,tempValues,i);
												ucPreCount=erDecoder->in_buff_rest>=16?16:erDecoder->in_buff_rest+1;
												ucRes = Detect_Agent(erDecoder,ucPreCount,i);
												if (0x01 == ucRes)
												{
													Reverse_Agent(erDecoder,tempValues,r);
													continue;
												}
												return ucRes;
											}
											Reverse_Agent(erDecoder,tempValues,q);
										}
										Reverse_Agent(erDecoder,tempValues,p);
									}
									Reverse_Agent(erDecoder,tempValues,o);
								}
								Reverse_Agent(erDecoder,tempValues,h);
							}
							Reverse_Agent(erDecoder,tempValues,g);
						}
						Reverse_Agent(erDecoder,tempValues,n);
					}
					Reverse_Agent(erDecoder,tempValues,m);
				}
				Reverse_Agent(erDecoder,tempValues,l);
			}
			Reverse_Agent(erDecoder,tempValues,j);
		}
		Reverse_Agent(erDecoder,tempValues,i);
	}

	return 0x01;
}

unsigned char ErrRecovery_Core_TwelveErr(WJL_ERRRECOVERY_DECODER *erDecoder,TEMP_Values *tempValues)
{
	unsigned char ucPreCount,ucRes=0x00;
	short i,j,l,m,n,g,h,o,p,q,r,s;

	for (i=0;i<112;++i)
	{
		Reverse_Agent(erDecoder,tempValues,i);
		for (j=i-1;j>=-99+i/8*8;--j)
		{
			Reverse_Agent(erDecoder,tempValues,j);
			for (l=j-1;l>=-100+i/8*8;--l)
			{
				Reverse_Agent(erDecoder,tempValues,l);
				for (m=l-1;m>=-101+i/8*8;--m)
				{
					Reverse_Agent(erDecoder,tempValues,m);
					for (n=m-1;n>=-102+i/8*8;--n)
					{
						Reverse_Agent(erDecoder,tempValues,n);
						for (g=n-1;g>=-103+i/8*8;--g)
						{
							Reverse_Agent(erDecoder,tempValues,g);
							for (h=g-1;h>=-104+i/8*8;--h)
							{
								Reverse_Agent(erDecoder,tempValues,h);
								for (o=h-1;o>=-105+i/8*8;--o)
								{
									Reverse_Agent(erDecoder,tempValues,o);
									for (p=o-1;p>=-106+i/8*8;--p)
									{
										Reverse_Agent(erDecoder,tempValues,p);
										for (q=p-1;q>=-107+i/8*8;--q)
										{
											Reverse_Agent(erDecoder,tempValues,q);
											for (r=q-1;r>=-108+i/8*8;--r)
											{
												Reverse_Agent(erDecoder,tempValues,r);
												for (s=r-1;s>=-109+i/8*8;--s)
												{
													Reverse_Agent(erDecoder,tempValues,s);
													Rollback_Agent(erDecoder,tempValues,i);
													ucPreCount=erDecoder->in_buff_rest>=16?16:erDecoder->in_buff_rest+1;
													ucRes = Detect_Agent(erDecoder,ucPreCount,i);
													if (0x01 == ucRes)
													{
														Reverse_Agent(erDecoder,tempValues,s);
														continue;
													}
													return ucRes;
												}
												Reverse_Agent(erDecoder,tempValues,r);
											}
											Reverse_Agent(erDecoder,tempValues,q);
										}
										Reverse_Agent(erDecoder,tempValues,p);
									}
									Reverse_Agent(erDecoder,tempValues,o);
								}
								Reverse_Agent(erDecoder,tempValues,h);
							}
							Reverse_Agent(erDecoder,tempValues,g);
						}
						Reverse_Agent(erDecoder,tempValues,n);
					}
					Reverse_Agent(erDecoder,tempValues,m);
				}
				Reverse_Agent(erDecoder,tempValues,l);
			}
			Reverse_Agent(erDecoder,tempValues,j);
		}
		Reverse_Agent(erDecoder,tempValues,i);
	}

	return 0x01;
}
/*************************************************
(1) For e=12 in this paper, only 12 error correction functions are written
(2) here we can add more error correction functions. From the above function, a large 
number of nested loops are very inefficient, but this problem can be optimized and 
solved. The purpose of this procedure is to verify the correctness of the theory 
without considering the efficiency problem.
*************************************************/
void Init_TempValues(WJL_ERRRECOVERY_DECODER *erDecoder,TEMP_Values *tempValues)
{
	TEMP_Values temp;
	temp.Values = 0;
	temp.FLow = 0;
	temp.FRange = 0;
	temp.in_buff_rest = 0;
	temp.out_buff_size = 0;
	temp.mask = 0;
	temp.in_buff_pos = 0;
	temp.keepBackSymbol = KEEPBACK_NULL;

	memcpy(&(tempValues[0]),&temp,sizeof(TEMP_Values));
	memcpy(&(tempValues[1]),&temp,sizeof(TEMP_Values));
	memcpy(&(tempValues[2]),&temp,sizeof(TEMP_Values));
	memcpy(&(tempValues[3]),&temp,sizeof(TEMP_Values));
	memcpy(&(tempValues[4]),&temp,sizeof(TEMP_Values));
	memcpy(&(tempValues[5]),&temp,sizeof(TEMP_Values));
	memcpy(&(tempValues[6]),&temp,sizeof(TEMP_Values));
	memcpy(&(tempValues[7]),&temp,sizeof(TEMP_Values));
	memcpy(&(tempValues[8]),&temp,sizeof(TEMP_Values));
	memcpy(&(tempValues[9]),&temp,sizeof(TEMP_Values));
	memcpy(&(tempValues[10]),&temp,sizeof(TEMP_Values));
	memcpy(&(tempValues[11]),&temp,sizeof(TEMP_Values));
	memcpy(&(tempValues[12]),&temp,sizeof(TEMP_Values));

	tempValues[13].Values = erDecoder->Values;
	tempValues[13].FLow = erDecoder->FLow;
	tempValues[13].FRange = erDecoder->FRange;
	tempValues[13].in_buff_rest = erDecoder->in_buff_rest;
	tempValues[13].out_buff_size = erDecoder->out_buff_size;
	tempValues[13].mask = erDecoder->mask;
	tempValues[13].in_buff_pos = erDecoder->in_buff_pos;
	tempValues[13].keepBackSymbol = erDecoder->keepBackSymbol;
}

// Backup 14 v values for backtracking when current v values can not be corrected
// 14 values are used to test the theoretical limit, according to which only 12 values are required
void Update_TempValues(WJL_ERRRECOVERY_DECODER *erDecoder,TEMP_Values *tempValues)
{
	memcpy(&(tempValues[0]),&(tempValues[1]),sizeof(TEMP_Values));
	memcpy(&(tempValues[1]),&(tempValues[2]),sizeof(TEMP_Values));
	memcpy(&(tempValues[2]),&(tempValues[3]),sizeof(TEMP_Values));
	memcpy(&(tempValues[3]),&(tempValues[4]),sizeof(TEMP_Values));
	memcpy(&(tempValues[4]),&(tempValues[5]),sizeof(TEMP_Values));
	memcpy(&(tempValues[5]),&(tempValues[6]),sizeof(TEMP_Values));
	memcpy(&(tempValues[6]),&(tempValues[7]),sizeof(TEMP_Values));
	memcpy(&(tempValues[7]),&(tempValues[8]),sizeof(TEMP_Values));
	memcpy(&(tempValues[8]),&(tempValues[9]),sizeof(TEMP_Values));
	memcpy(&(tempValues[9]),&(tempValues[10]),sizeof(TEMP_Values));
	memcpy(&(tempValues[10]),&(tempValues[11]),sizeof(TEMP_Values));
	memcpy(&(tempValues[11]),&(tempValues[12]),sizeof(TEMP_Values));
	memcpy(&(tempValues[12]),&(tempValues[13]),sizeof(TEMP_Values));	

	tempValues[13].Values = erDecoder->Values;
	tempValues[13].FLow = erDecoder->FLow;
	tempValues[13].FRange = erDecoder->FRange;
	tempValues[13].keepBackSymbol = erDecoder->keepBackSymbol;
	tempValues[13].mask = erDecoder->mask;
	tempValues[13].in_buff_rest = erDecoder->in_buff_rest;
	tempValues[13].out_buff_size = erDecoder->out_buff_size;
	tempValues[13].in_buff_pos = erDecoder->in_buff_pos;
}

unsigned char ErrRecovery_Agent(WJL_ERRRECOVERY_DECODER *erDecoder,const unsigned int *unInfbuffLen,unsigned int *unPosErr)
{
	unsigned char ucRes = 0x00;
	TEMP_Values tempValues[14];
	Init_TempValues(erDecoder,tempValues);

	do
	{// Check if the current V value is wrong (if checked V the value will be updated 8 bits to the right and then each V value of the backup will be updated)
		ucRes = Detect_Core(erDecoder);
		if (0x00 == ucRes)
		{// Each V value of the backup is updated without error
			Update_TempValues(erDecoder,tempValues);
			continue;
		}

		if (0x02 == ucRes)
		{// Input buffer resolved
			return 0x00;
		}

		ucRes = ErrRecovery_Core_OneErr(erDecoder,tempValues);
		if (0x01 == ucRes)
		{
			ucRes = ErrRecovery_Core_TwoErr(erDecoder,tempValues);
			if (0x01 == ucRes)
			{
				ucRes = ErrRecovery_Core_ThreeErr(erDecoder,tempValues);
				if (0x01 == ucRes)
				{
					ucRes = ErrRecovery_Core_FourErr(erDecoder,tempValues);
					if (0x01 == ucRes)
					{
						ucRes = ErrRecovery_Core_FiveErr(erDecoder,tempValues);
						if (0x01 == ucRes)
						{
							ucRes = ErrRecovery_Core_SixErr(erDecoder,tempValues);
							if (0x01 == ucRes)
							{
								ucRes = ErrRecovery_Core_SevenErr(erDecoder,tempValues);
								if (0x01 == ucRes)
								{
									ucRes = ErrRecovery_Core_EightErr(erDecoder,tempValues);
									if (0x01 == ucRes)
									{
										ucRes = ErrRecovery_Core_NineErr(erDecoder,tempValues);
										if (0x01 == ucRes)
										{
											ucRes = ErrRecovery_Core_TenErr(erDecoder,tempValues);
											if (0x01 == ucRes)
											{
												ucRes = ErrRecovery_Core_ElevenErr(erDecoder,tempValues);
												if (0x01 == ucRes)
												{
													ucRes = ErrRecovery_Core_TwelveErr(erDecoder,tempValues);
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	} while (!ucRes);

	if (0x01 == ucRes)
	{// Give up and exit the error correction process
		(tempValues[13].in_buff_rest + 13<=*unInfbuffLen)?(*unPosErr=*unInfbuffLen-tempValues[13].in_buff_rest-13):(*unPosErr = 0);
		return 0x01;
	}
	return 0x00;
}

void Decode_Init(WJL_ERRRECOVERY_DECODER *erDecoder,const unsigned int *unOutbufferLen)
{
	unsigned int n = 32;

	erDecoder->out_buff = (unsigned char*)malloc(*unOutbufferLen);
	erDecoder->out_buff_size = 0;

	erDecoder->in_buff_rest = erDecoder->in_buff_pos;
	erDecoder->in_buff_pos = 0;

	erDecoder->FLow = 0;
	erDecoder->FRange = erDecoder->RC_MAX_RANGE;

	do{
		n -= 8;
		erDecoder->Values = (erDecoder->Values<<8) | erDecoder->in_buff[erDecoder->in_buff_pos++];
		erDecoder->in_buff_rest--;
	} while(n>0);
	erDecoder->mask = 0x01;
	erDecoder->outByte = 0x00;

	erDecoder->keepBackSymbol = KEEPBACK_NULL;
}

void Decode_Agent(WJL_ERRRECOVERY_DECODER *erDecoder,const unsigned int *unOutbufferLen)
{
	unsigned char symbol,ucAppearErr=0x00;

	do
	{
		symbol = Decode_GetSymbol(erDecoder,&ucAppearErr,0);
		Decode_UpdateRange(erDecoder,&symbol);
	} while (erDecoder->out_buff_size < *unOutbufferLen);
}
