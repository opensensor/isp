#define ORACLE_WRITE_LIMIT 28
#include "t41_awb_stats_oracle_support.c"
void oracle_trigger(unsigned int bank,unsigned int channel)
{
	if(bank!=3 || channel!=0) { ++oracle_bad; return; }
	oracle_write(0x5000,1);
}
