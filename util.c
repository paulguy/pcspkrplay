int test_bit(void *data, int length, int bit) {
	int boffset;
	boffset = bit / 8;
	bit %= 8;

	if(boffset > length) {
		return -1;
	}

	if(((char *)data)[boffset] & bit) {
		return 1;
	}

	return 0;
}

void chartolower(char *str, int length) {
	int i;

	for(i = 0; i < length; i++) {
		if(str[i] > 65 && str[i] < 90) {
			str[i] += 32;
		}
	}
}
