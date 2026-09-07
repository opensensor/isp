/* All reference register writes terminate in this private array. */
unsigned int oracle_words[8];
unsigned int oracle_bad_write;
void oracle_write(unsigned int address, unsigned int value)
{
	if (address >= 0xb004 && address <= 0xb020 && !(address & 3))
		oracle_words[(address - 0xb004) / 4] = value;
	else if (address != 0xb000 || value != 1)
		++oracle_bad_write;
}
long long oracle_signed_shift(long long value, unsigned int shift)
{
	return value >> shift;
}
