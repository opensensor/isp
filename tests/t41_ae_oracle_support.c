/* Non-PIC userspace leaves. No device, kernel or production dependencies. */
unsigned long long oracle_left_shift(unsigned long long value, unsigned int n)
{
	return value << n;
}
unsigned int oracle_div64(unsigned long long *value, unsigned int divisor)
{
	unsigned int remainder = *value % divisor;
	*value /= divisor;
	return remainder;
}
void oracle_noop(void) { }
