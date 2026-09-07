/* Userspace-only non-PIC leaf dependencies for the reference compute code. */
void *oracle_copy(void *to, const void *from, unsigned int size)
{
	unsigned char *d = to;
	const unsigned char *s = from;
	unsigned int i;
	for (i = 0; i < size; ++i) d[i] = s[i];
	return to;
}
void *oracle_fill(void *to, int value, unsigned int size)
{
	unsigned char *d = to;
	unsigned int i;
	for (i = 0; i < size; ++i) d[i] = value;
	return to;
}
unsigned long long oracle_shift(unsigned long long value, unsigned int shift)
{
	return value >> shift;
}
