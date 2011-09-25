/*
 * Copyright (c) 2011 Paul LaMendola
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

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

int readnum(char *in, int inlen, char *out, int outlen, char term) {
	int i;
	int maxlen;

	maxlen = (inlen < outlen) ? inlen : outlen;
	if(maxlen < 1) {
		out[0] = '\0';
		return(0);
	}

	for(i = 0; i < maxlen; i++) {
		if(in[i] == term) {
			out[i] = '\0';
			return(i);
		} else if(in[i] < '0' || in[i] > '9')
			break;
		out[i] = in[i];
	}

	out[i] = '\0';
	return(i - 1);
}
