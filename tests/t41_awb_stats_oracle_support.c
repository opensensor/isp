#include <stddef.h>
unsigned int oracle_rgb[3], oracle_words[18][2], oracle_writes, oracle_reads, oracle_bad;
void *oracle_copy(void *dest, const void *src, unsigned int n)
{
	unsigned char *d=dest; const unsigned char *s=src;
	while(n--) *d++=*s++;
	return dest;
}
unsigned int oracle_read(unsigned int address)
{
	++oracle_reads;
	if(address<0x180ac || address>0x180b4 || (address&3)) { ++oracle_bad; return 0; }
	return oracle_rgb[(address-0x180ac)/4];
}
void oracle_write(unsigned int address,unsigned int value)
{
	if(oracle_writes>=18) { ++oracle_bad; return; }
	oracle_words[oracle_writes][0]=address;
	oracle_words[oracle_writes++][1]=value;
}
