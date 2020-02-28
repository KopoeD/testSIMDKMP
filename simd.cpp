#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include <string>
#include <iostream>
#include <emmintrin.h>
#include <pmmintrin.h>
#include <bitset>
#include <Bits.h>
#include <intrin.h>
//#include "bitss.h"


void print128_num(__m128i var)
{
	uint8_t *val = (uint8_t*)&var;
	printf("Numerical: %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i \n",
		val[0], val[1], val[2], val[3], val[4], val[5],
		val[6], val[7], val[7], val[8], val[9], val[10],
		val[11], val[12], val[12], val[13], val[14], val[15]);
}


// Function to implement KMP algorithm
int seek_substring_KMP(char s[], char p[], char rp[], char rt[])
{
	int i, j, N, M;
	N = strlen(s);
	M = strlen(p);

	// Динамический массив длины М
	int *d = (int*)malloc(M * sizeof(int));
	int *op = (int*)malloc(M * sizeof(int));
	// Вычисление префикс-функции
	d[0] = 0;
	op[0] = 0;
	for (i = 1, j = 0; i < M; i++)
	{
		while (j > 0 && p[j] != p[i])
			j = op[j - 1];
		if (p[j] == p[i])
			j++;
		op[i] = j;
	}
	//simd
	for (i = 1, j = 0; i < M;) {
		if (j == 0) {
			auto sseVal = _mm_set1_epi8(p[j]);
			auto sseArr = _mm_loadu_si128((__m128i *) &p[i]);
			//uint8_t *val = (uint8_t*)&(_mm_cmpeq_epi8(sseVal, sseArr));
			auto ss = _mm_movemask_epi8(_mm_cmpeq_epi8(sseVal, sseArr));
			//std::cout << std::bitset<16>(ss) << std::endl;
			//std::cout << std::bitset<16>(~ss) << std::endl;
			unsigned long index;
			unsigned char isNonzero = _BitScanForward(&index, ss);
			//std::cout << index << std::endl;
			if (isNonzero)
			{
				for (int k = i; k <= index + i; k++) {
					d[k] = 0;
				}
				int num = index + i;
				d[num] = 1;
				j++;
				i = num + 1;

			}
			else
			{
				i = i + 16;
			}
		}
		else {
			bool change = false;
			while ((j > 0) && !change) {
				auto sseVal = _mm_set1_epi8(p[i]);
				auto sseArr = _mm_loadu_si128((__m128i *) &rp[M - j - 1]);
				auto mask = _mm_movemask_epi8(_mm_cmpeq_epi8(sseVal, sseArr));
				unsigned long index;
				unsigned char isNonzero = _BitScanForward(&index, mask);
				if (isNonzero)
				{
					if (index < j) {
						j++;
						d[i] = j;
						change = true;
					}
					else {
						j = 0;
					}

				}
				else
				{
					j = j - 16;
				}
			}
			if (!change) { j = 0; d[i] = j; }
			i++;
		}
		/*for (int m = 0; m < M; m++) {
			std::cout << d[m] << " ";

		}
		std::cout << std::endl;*/
	}

	for (int m = 0; m < M; m++) {
		std::cout << d[m] << " " << op[m] << std::endl;
	}
	std::cout << std::endl;
	// Поиск

	/*for (i = 0, j = 0; i < N;) {
		 auto sseFind= _mm_loadu_si128((__m128i *) &p[j]);
		 auto sseText = _mm_loadu_si128((__m128i *) &s[i]);
		 auto mask = ~(_mm_movemask_epi8(_mm_cmpeq_epi8(sseFind, sseText)));
		 unsigned long index;
		 unsigned char isNonzero = _BitScanForward(&index, mask);
		 if (isNonzero)
		 {
			 if ()
			 int num = index + i;
			 
			 d[num] = 1;
			 j++;
			 i = num + 1;

		 }
		 else
		 {
			 i = i + 16;
		 }
	}
*/



	for (i = 0, j = 0; i < N; i++)
	{
		while (j > 0 && p[j] != s[i])
			j = d[j - 1];
		if (p[j] == s[i])
			j++;
		if (j == M)
		{
			free(d);
			return i - j + 1;
		}
	}
	free(d);
	return -1;
}


int main(void)
{
	char* text = "ABCABAABCABAC";
	char* pattern = "AAB";
	int lenght = strlen(pattern);

	char* rp = (char*)malloc((lenght + 1) * sizeof(char));
	rp[lenght] = '\0';
	int i;
	for (i = 0; i < lenght; i++) {
		rp[i] = pattern[lenght - 1 - i];
	}

	int lenght1 = strlen(text);
	char* rt = (char*)malloc((lenght1 + 1) * sizeof(char));
	rt[lenght1] = '\0';
	for (i = 0; i < lenght1; i++) {
		rt[i] = text[lenght1 - 1 - i];
	}
	//AABAABCABDDDDAABAD
	//CABACAABDSOSAAJSNCASNSANSLPSASKMSNNCANBSSDCAHJJCANNKALMCAB
	int a = seek_substring_KMP(text, pattern, rp, rt);
	printf("%d", a);
	_getch();
	return 0;
}