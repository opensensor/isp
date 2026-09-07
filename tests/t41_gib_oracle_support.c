unsigned int oracle_words[3], oracle_bad_write;
void oracle_write(unsigned int address, unsigned int value)
{
	if (address == 0x803c) oracle_words[0] = value;
	else if (address == 0x8000) oracle_words[1] = value;
	else if (address == 0x8004) oracle_words[2] = value;
	else if (address != 0x8040 || value != 1) ++oracle_bad_write;
}
long long oracle_signed_shift(long long value, unsigned int shift) { return value >> shift; }
