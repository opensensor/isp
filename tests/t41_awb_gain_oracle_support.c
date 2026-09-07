unsigned int oracle_words[2], oracle_bad_write, oracle_writes, oracle_triggers;
void oracle_write(unsigned int address, unsigned int value)
{
	++oracle_writes;
	if (address == 0x4004) oracle_words[0] = value;
	else if (address == 0x4008) oracle_words[1] = value;
	else if (address == 0x400c || address == 0x5004 || address == 0x500c) {
		if (value != oracle_words[0]) ++oracle_bad_write;
	} else if (address == 0x4010 || address == 0x5008 || address == 0x5010) {
		if (value != oracle_words[1]) ++oracle_bad_write;
	} else if (address != 0x4000 || value != 1) ++oracle_bad_write;
}
void oracle_trigger(unsigned int bank, unsigned int channel)
{
	++oracle_triggers;
	if (bank != 3 || channel != 0) ++oracle_bad_write;
}
