unsigned int oracle_words[3][130], oracle_counts[3], oracle_bad_write;
void oracle_write(unsigned int address, unsigned int value)
{
	unsigned int bank;
	if (address < 0x50040 || address > 0x50084 ||
	    (address & 31) > 4 || (address & 3)) {
		++oracle_bad_write;
		return;
	}
	bank = (address - 0x50040) / 32;
	if (oracle_counts[bank] >= 130) { ++oracle_bad_write; return; }
	oracle_words[bank][oracle_counts[bank]++] = value;
}
long long oracle_signed_shift(long long value, unsigned int shift)
{ return value >> shift; }
