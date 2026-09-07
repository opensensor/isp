unsigned int oracle_words[29], oracle_bad_write;
void oracle_write(unsigned int address, unsigned int value)
{
	if (address >= 0x11000 && address <= 0x11070 && !(address & 3))
		oracle_words[(address-0x11000)/4] = value;
	else ++oracle_bad_write;
}
int oracle_unexpected(void) { ++oracle_bad_write; return -1; }
long long oracle_signed_shift(long long value, unsigned int shift) { return value >> shift; }
