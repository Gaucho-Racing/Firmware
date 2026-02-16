#include <stdint.h>

#include "ConvenienceMacros.h"

int getValue(int a)
{
	return a;
}

int main(void)
{
	int a = 5, b = 10;
	if (MIN(a, b) != 5 || MAX(a, b) != 10) {
		return 1;
	}

	int c = -3, d = 2;
	if (MIN(c, d) != -3 || MAX(c, d) != 2) {
		return 2;
	}

	int e = 0, f = 0;
	if (MIN(e, f) != 0 || MAX(e, f) != 0) {
		return 3;
	}

	int g = 5, h = 5;
	if (MIN(++g, h) != 5 || MAX(--g, h) != 5) {
		return 4;
	}

	int i = 3, j = 7;
	if (MIN(i++, j) != 3 || MAX(i, ++j) != 8) {
		return 5;
	}

	int k = 5, l = 5;
	if (MIN(k, l++) != 5 || MAX(k, l) != 6) {
		return 6;
	}

	int m = 5, n = 10;
	if (MIN_WITH_TYPES(m, n) != 5 || MAX_WITH_TYPES(m, n) != 10) {
		return 7;
	}

	int o = -3, p = 2;
	if (MIN_WITH_TYPES(o, p) != -3 || MAX_WITH_TYPES(o, p) != 2) {
		return 8;
	}

	int q = 0, r = 0;
	if (MIN_WITH_TYPES(q, r) != 0 || MAX_WITH_TYPES(q, r) != 0) {
		return 9;
	}

	int s = 5, t = 5;
	if (MIN_WITH_TYPES(++s, t) != 5 || MAX_WITH_TYPES(--s, t) != 5) {
		return 10;
	}

	int u = 3, v = 7;
	if (MIN_WITH_TYPES(u++, v) != 3 || MAX_WITH_TYPES(u, ++v) != 8) {
		return 11;
	}

	int w = 5, x = 5;
	if (MIN_WITH_TYPES(w, x++) != 5 || MAX_WITH_TYPES(w, x) != 6) {
		return 12;
	}

	double y = 3.14, z = 2.71;
	if (MIN_WITH_TYPES(y, z) != 2.71 || MAX_WITH_TYPES(y, z) != 3.14) {
		return 13;
	}

	float aa = 1.5f, bb = 2.5f;
	if (MIN_WITH_TYPES(aa, bb) != 1.5f || MAX_WITH_TYPES(aa, bb) != 2.5f) {
		return 14;
	}

	double cc = -1.0, dd = 0.0;
	if (MIN_WITH_TYPES(cc, dd) != -1.0 || MAX_WITH_TYPES(cc, dd) != 0.0) {
		return 15;
	}

	double ee = 0.0, ff = 0.0;
	if (MIN_WITH_TYPES(ee, ff) != 0.0 || MAX_WITH_TYPES(ee, ff) != 0.0) {
		return 16;
	}

	double gg = 1.0, hh = 1.0;
	if (MIN_WITH_TYPES(++gg, hh) != 1.0 || MAX_WITH_TYPES(--gg, hh) != 1.0) {
		return 17;
	}

	uint8_t ii = 255, jj = 0;
	if (MIN_WITH_TYPES(ii, jj) != 0 || MAX_WITH_TYPES(ii, jj) != 255) {
		return 18;
	}

	uint16_t kk = 65535, ll = 255;
	if (MIN_WITH_TYPES(kk, ll) != 255 || MAX_WITH_TYPES(kk, ll) != 65535) {
		return 19;
	}

	if (MIN_WITH_TYPES(getValue(5), getValue(10)) != 5 || MAX_WITH_TYPES(getValue(5), getValue(10)) != 10) {
		return 20;
	}

	if (MIN_WITH_TYPES(getValue(-3), getValue(2)) != -3 || MAX_WITH_TYPES(getValue(-3), getValue(2)) != 2) {
		return 21;
	}

	if (MIN_WITH_TYPES(getValue(0), getValue(0)) != 0 || MAX_WITH_TYPES(getValue(0), getValue(0)) != 0) {
		return 22;
	}

	if (MIN_WITH_TYPES(getValue(5), getValue(5)) != 5 || MAX_WITH_TYPES(getValue(5), getValue(5)) != 5) {
		return 23;
	}

	if (MIN_WITH_TYPES(getValue(3), getValue(7)) != 3 || MAX_WITH_TYPES(getValue(3), getValue(7)) != 7) {
		return 24;
	}

	if (MIN_WITH_TYPES(getValue(5), getValue(5)) != 5 || MAX_WITH_TYPES(getValue(5), getValue(5)) != 5) {
		return 25;
	}

	if (MIN_WITH_TYPES(MIN_WITH_TYPES(5, 10), MIN_WITH_TYPES(3, 7)) != 3 || MAX_WITH_TYPES(MAX_WITH_TYPES(5, 10), MAX_WITH_TYPES(3, 7)) != 10) {
		return 26;
	}

	if (MIN_WITH_TYPES(MIN_WITH_TYPES(-3, 2), MIN_WITH_TYPES(0, 0)) != -3 || MAX_WITH_TYPES(MAX_WITH_TYPES(-3, 2), MAX_WITH_TYPES(0, 0)) != 2) {
		return 27;
	}

	if (MIN_WITH_TYPES(MIN_WITH_TYPES(0, 0), MIN_WITH_TYPES(0, 0)) != 0 || MAX_WITH_TYPES(MAX_WITH_TYPES(0, 0), MAX_WITH_TYPES(0, 0)) != 0) {
		return 28;
	}

	int *ptr1 = (int *)0x1000;
	int *ptr2 = (int *)0x2000;
	if (MIN_WITH_TYPES(ptr1, ptr2) != ptr1 || MAX_WITH_TYPES(ptr1, ptr2) != ptr2) {
		return 29;
	}

	char char1 = 'A', char2 = 'B';
	if (MIN_WITH_TYPES(char1, char2) != 'A' || MAX_WITH_TYPES(char1, char2) != 'B') {
		return 30;
	}

	return 0;
}
