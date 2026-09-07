unsigned int oracle_addresses[4096],oracle_values[4096],oracle_count,oracle_bad_write;
void oracle_write(unsigned int address,unsigned int value)
{
	if(oracle_count>=4096) { ++oracle_bad_write; return; }
	oracle_addresses[oracle_count]=address; oracle_values[oracle_count++]=value;
}
int oracle_unexpected(void) { ++oracle_bad_write; return -1; }
int oracle_noop(void) { return 0; }
void *oracle_copy(void *to,const void *from,unsigned int bytes)
{ unsigned char *d=to; const unsigned char *s=from; while(bytes--) *d++=*s++; return to; }
void *oracle_fill(void *to,int value,unsigned int bytes)
{ unsigned char *d=to; while(bytes--) *d++=value; return to; }
unsigned long long oracle_shift(unsigned long long value,unsigned int bits) { return value>>bits; }
long long oracle_signed_shift(long long value,unsigned int bits) { return value>>bits; }
