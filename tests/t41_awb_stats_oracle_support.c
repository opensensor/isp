#include <stddef.h>
unsigned long long oracle_left_shift(unsigned long long value,unsigned int shift) { return value<<shift; }
unsigned long long oracle_shift(unsigned long long value,unsigned int shift) { return value>>shift; }
unsigned int oracle_div64(unsigned long long *value,unsigned int divisor)
{
	unsigned int remainder=*value%divisor;
	*value/=divisor;
	return remainder;
}
unsigned int oracle_rgb[3], oracle_words[18][2], oracle_writes, oracle_reads, oracle_bad;
void *oracle_copy(void *dest, const void *src, unsigned int n)
{
	unsigned char *d=dest; const unsigned char *s=src;
	while(n--) *d++=*s++;
	return dest;
}
void *oracle_fill(void *dest,int value,unsigned int n)
{
	unsigned char *d=dest;
	while(n--) *d++=value;
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
