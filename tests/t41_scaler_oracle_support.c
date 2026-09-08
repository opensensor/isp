/* Test-only non-PIC dependencies: no kernel, ISP, sensor or MMIO access. */
unsigned int oracle_addresses[668], oracle_values[668], oracle_count;
unsigned int oracle_bad_call, oracle_allocated;
static int scratch[257];

void *oracle_alloc(unsigned int size)
{
	if (oracle_allocated || size > sizeof(scratch)) {
		++oracle_bad_call;
		return (void *)0;
	}
	oracle_allocated = 1;
	return scratch;
}

void oracle_free(void *p)
{
	if (p != scratch || !oracle_allocated)
		++oracle_bad_call;
	oracle_allocated = 0;
}

int oracle_write(unsigned int address, unsigned int value)
{
	if (oracle_count >= 668) {
		++oracle_bad_call;
		return -1;
	}
	oracle_addresses[oracle_count] = address;
	oracle_values[oracle_count++] = value;
	return 0;
}

int oracle_unexpected(void)
{
	++oracle_bad_call;
	return -1;
}
