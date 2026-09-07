unsigned int oracle_addresses[128], oracle_values[128], oracle_count, oracle_bad_write;
char oracle_message[256];
char oracle_rodata[16384] __attribute__((aligned(65536)));
void oracle_write(unsigned int address, unsigned int value)
{
	if (oracle_count >= 128) { ++oracle_bad_write; return; }
	oracle_addresses[oracle_count] = address;
	oracle_values[oracle_count++] = value;
}
int oracle_unexpected(void) { ++oracle_bad_write; return -1; }
