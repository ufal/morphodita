// This file is part of MorphoDiTa <http://github.com/ufal/morphodita/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ragel_tokenizer.h"

namespace ufal {
namespace morphodita {





static const char _ragel_url_email_cond_offsets[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 14, 14, 14, 14, 
	14, 14, 14, 14, 14, 14, 14, 14, 
	14, 14, 14, 14, 14, 14, 14, 14, 
	14, 14, 14, 14, 14, 14, 14, 14, 
	14, 14, 14, 14, 14, 14, 14, 14, 
	14, 14, 14, 14, 14, 14, 14, 14, 
	14, 14, 14, 28, 28, 28, 28, 28, 
	28, 28, 28, 28, 28, 28, 28, 28, 
	28
};

static const char _ragel_url_email_cond_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 14, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 14, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0
};

static const short _ragel_url_email_cond_keys[] = {
	48u, 57u, 65u, 90u, 97u, 122u, 129u, 129u, 
	131u, 131u, 135u, 135u, 142u, 142u, 143u, 143u, 
	151u, 151u, 155u, 155u, 156u, 156u, 157u, 157u, 
	158u, 158u, 159u, 159u, 48u, 57u, 65u, 90u, 
	97u, 122u, 129u, 129u, 131u, 131u, 135u, 135u, 
	142u, 142u, 143u, 143u, 151u, 151u, 155u, 155u, 
	156u, 156u, 157u, 157u, 158u, 158u, 159u, 159u, 
	0
};

static const char _ragel_url_email_cond_spaces[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0
};

static const short _ragel_url_email_key_offsets[] = {
	0, 0, 15, 29, 41, 54, 63, 71, 
	78, 86, 92, 100, 131, 140, 148, 157, 
	165, 174, 182, 190, 201, 211, 219, 227, 
	238, 248, 256, 264, 275, 285, 301, 316, 
	332, 346, 362, 379, 395, 412, 428, 445, 
	461, 477, 496, 514, 530, 546, 565, 583, 
	599, 615, 634, 652, 668, 684, 700, 711, 
	712, 727, 737, 768, 771, 781, 791, 801, 
	814, 826, 843, 856, 870, 887, 904, 921, 
	941
};

static const short _ragel_url_email_trans_keys[] = {
	33u, 48u, 49u, 50u, 95u, 36u, 37u, 39u, 
	46u, 51u, 57u, 65u, 90u, 97u, 122u, 33u, 
	58u, 64u, 95u, 36u, 37u, 39u, 46u, 48u, 
	57u, 65u, 90u, 97u, 122u, 33u, 95u, 36u, 
	37u, 39u, 46u, 48u, 57u, 65u, 90u, 97u, 
	122u, 33u, 64u, 95u, 36u, 37u, 39u, 46u, 
	48u, 57u, 65u, 90u, 97u, 122u, 48u, 49u, 
	50u, 51u, 57u, 65u, 90u, 97u, 122u, 45u, 
	46u, 48u, 57u, 65u, 90u, 97u, 122u, 45u, 
	48u, 57u, 65u, 90u, 97u, 122u, 45u, 46u, 
	48u, 57u, 65u, 90u, 97u, 122u, 48u, 57u, 
	65u, 90u, 97u, 122u, 45u, 46u, 48u, 57u, 
	65u, 90u, 97u, 122u, 33u, 39u, 41u, 61u, 
	95u, 641u, 643u, 647u, 663u, 36u, 47u, 58u, 
	59u, 63u, 64u, 304u, 313u, 321u, 346u, 353u, 
	378u, 560u, 569u, 577u, 602u, 609u, 634u, 654u, 
	655u, 667u, 671u, 48u, 49u, 50u, 51u, 57u, 
	65u, 90u, 97u, 122u, 45u, 46u, 48u, 57u, 
	65u, 90u, 97u, 122u, 48u, 49u, 50u, 51u, 
	57u, 65u, 90u, 97u, 122u, 45u, 46u, 48u, 
	57u, 65u, 90u, 97u, 122u, 48u, 49u, 50u, 
	51u, 57u, 65u, 90u, 97u, 122u, 45u, 46u, 
	48u, 57u, 65u, 90u, 97u, 122u, 45u, 46u, 
	48u, 57u, 65u, 90u, 97u, 122u, 45u, 46u, 
	53u, 48u, 52u, 54u, 57u, 65u, 90u, 97u, 
	122u, 45u, 46u, 48u, 53u, 54u, 57u, 65u, 
	90u, 97u, 122u, 45u, 46u, 48u, 57u, 65u, 
	90u, 97u, 122u, 45u, 46u, 48u, 57u, 65u, 
	90u, 97u, 122u, 45u, 46u, 53u, 48u, 52u, 
	54u, 57u, 65u, 90u, 97u, 122u, 45u, 46u, 
	48u, 53u, 54u, 57u, 65u, 90u, 97u, 122u, 
	45u, 46u, 48u, 57u, 65u, 90u, 97u, 122u, 
	45u, 46u, 48u, 57u, 65u, 90u, 97u, 122u, 
	45u, 46u, 53u, 48u, 52u, 54u, 57u, 65u, 
	90u, 97u, 122u, 45u, 46u, 48u, 53u, 54u, 
	57u, 65u, 90u, 97u, 122u, 33u, 45u, 46u, 
	58u, 64u, 95u, 36u, 37u, 39u, 44u, 48u, 
	57u, 65u, 90u, 97u, 122u, 33u, 45u, 58u, 
	64u, 95u, 36u, 37u, 39u, 46u, 48u, 57u, 
	65u, 90u, 97u, 122u, 33u, 45u, 46u, 58u, 
	64u, 95u, 36u, 37u, 39u, 44u, 48u, 57u, 
	65u, 90u, 97u, 122u, 33u, 58u, 64u, 95u, 
	36u, 37u, 39u, 46u, 48u, 57u, 65u, 90u, 
	97u, 122u, 33u, 45u, 46u, 58u, 64u, 95u, 
	36u, 37u, 39u, 44u, 48u, 57u, 65u, 90u, 
	97u, 122u, 33u, 48u, 49u, 50u, 58u, 64u, 
	95u, 36u, 37u, 39u, 46u, 51u, 57u, 65u, 
	90u, 97u, 122u, 33u, 45u, 46u, 58u, 64u, 
	95u, 36u, 37u, 39u, 44u, 48u, 57u, 65u, 
	90u, 97u, 122u, 33u, 48u, 49u, 50u, 58u, 
	64u, 95u, 36u, 37u, 39u, 46u, 51u, 57u, 
	65u, 90u, 97u, 122u, 33u, 45u, 46u, 58u, 
	64u, 95u, 36u, 37u, 39u, 44u, 48u, 57u, 
	65u, 90u, 97u, 122u, 33u, 48u, 49u, 50u, 
	58u, 64u, 95u, 36u, 37u, 39u, 46u, 51u, 
	57u, 65u, 90u, 97u, 122u, 33u, 45u, 46u, 
	58u, 64u, 95u, 36u, 37u, 39u, 44u, 48u, 
	57u, 65u, 90u, 97u, 122u, 33u, 45u, 46u, 
	58u, 64u, 95u, 36u, 37u, 39u, 44u, 48u, 
	57u, 65u, 90u, 97u, 122u, 33u, 45u, 46u, 
	53u, 58u, 64u, 95u, 36u, 37u, 39u, 44u, 
	48u, 52u, 54u, 57u, 65u, 90u, 97u, 122u, 
	33u, 45u, 46u, 58u, 64u, 95u, 36u, 37u, 
	39u, 44u, 48u, 53u, 54u, 57u, 65u, 90u, 
	97u, 122u, 33u, 45u, 46u, 58u, 64u, 95u, 
	36u, 37u, 39u, 44u, 48u, 57u, 65u, 90u, 
	97u, 122u, 33u, 45u, 46u, 58u, 64u, 95u, 
	36u, 37u, 39u, 44u, 48u, 57u, 65u, 90u, 
	97u, 122u, 33u, 45u, 46u, 53u, 58u, 64u, 
	95u, 36u, 37u, 39u, 44u, 48u, 52u, 54u, 
	57u, 65u, 90u, 97u, 122u, 33u, 45u, 46u, 
	58u, 64u, 95u, 36u, 37u, 39u, 44u, 48u, 
	53u, 54u, 57u, 65u, 90u, 97u, 122u, 33u, 
	45u, 46u, 58u, 64u, 95u, 36u, 37u, 39u, 
	44u, 48u, 57u, 65u, 90u, 97u, 122u, 33u, 
	45u, 46u, 58u, 64u, 95u, 36u, 37u, 39u, 
	44u, 48u, 57u, 65u, 90u, 97u, 122u, 33u, 
	45u, 46u, 53u, 58u, 64u, 95u, 36u, 37u, 
	39u, 44u, 48u, 52u, 54u, 57u, 65u, 90u, 
	97u, 122u, 33u, 45u, 46u, 58u, 64u, 95u, 
	36u, 37u, 39u, 44u, 48u, 53u, 54u, 57u, 
	65u, 90u, 97u, 122u, 33u, 45u, 46u, 58u, 
	64u, 95u, 36u, 37u, 39u, 44u, 48u, 57u, 
	65u, 90u, 97u, 122u, 33u, 45u, 46u, 58u, 
	64u, 95u, 36u, 37u, 39u, 44u, 48u, 57u, 
	65u, 90u, 97u, 122u, 33u, 45u, 46u, 58u, 
	64u, 95u, 36u, 37u, 39u, 44u, 48u, 57u, 
	65u, 90u, 97u, 122u, 33u, 47u, 95u, 36u, 
	37u, 39u, 57u, 65u, 90u, 97u, 122u, 47u, 
	33u, 48u, 49u, 50u, 95u, 36u, 37u, 39u, 
	46u, 51u, 57u, 65u, 90u, 97u, 122u, 45u, 
	46u, 47u, 58u, 48u, 57u, 65u, 90u, 97u, 
	122u, 33u, 39u, 41u, 61u, 95u, 641u, 643u, 
	647u, 663u, 36u, 47u, 58u, 59u, 63u, 64u, 
	304u, 313u, 321u, 346u, 353u, 378u, 560u, 569u, 
	577u, 602u, 609u, 634u, 654u, 655u, 667u, 671u, 
	47u, 48u, 57u, 45u, 46u, 47u, 58u, 48u, 
	57u, 65u, 90u, 97u, 122u, 45u, 46u, 47u, 
	58u, 48u, 57u, 65u, 90u, 97u, 122u, 45u, 
	46u, 47u, 58u, 48u, 57u, 65u, 90u, 97u, 
	122u, 45u, 46u, 47u, 53u, 58u, 48u, 52u, 
	54u, 57u, 65u, 90u, 97u, 122u, 45u, 46u, 
	47u, 58u, 48u, 53u, 54u, 57u, 65u, 90u, 
	97u, 122u, 33u, 45u, 46u, 47u, 58u, 64u, 
	95u, 36u, 37u, 39u, 44u, 48u, 57u, 65u, 
	90u, 97u, 122u, 33u, 47u, 95u, 36u, 37u, 
	39u, 46u, 48u, 57u, 65u, 90u, 97u, 122u, 
	33u, 47u, 64u, 95u, 36u, 37u, 39u, 46u, 
	48u, 57u, 65u, 90u, 97u, 122u, 33u, 45u, 
	46u, 47u, 58u, 64u, 95u, 36u, 37u, 39u, 
	44u, 48u, 57u, 65u, 90u, 97u, 122u, 33u, 
	45u, 46u, 47u, 58u, 64u, 95u, 36u, 37u, 
	39u, 44u, 48u, 57u, 65u, 90u, 97u, 122u, 
	33u, 45u, 46u, 47u, 58u, 64u, 95u, 36u, 
	37u, 39u, 44u, 48u, 57u, 65u, 90u, 97u, 
	122u, 33u, 45u, 46u, 47u, 53u, 58u, 64u, 
	95u, 36u, 37u, 39u, 44u, 48u, 52u, 54u, 
	57u, 65u, 90u, 97u, 122u, 33u, 45u, 46u, 
	47u, 58u, 64u, 95u, 36u, 37u, 39u, 44u, 
	48u, 53u, 54u, 57u, 65u, 90u, 97u, 122u, 
	0
};

static const char _ragel_url_email_single_lengths[] = {
	0, 5, 4, 2, 3, 3, 2, 1, 
	2, 0, 2, 9, 3, 2, 3, 2, 
	3, 2, 2, 3, 2, 2, 2, 3, 
	2, 2, 2, 3, 2, 6, 5, 6, 
	4, 6, 7, 6, 7, 6, 7, 6, 
	6, 7, 6, 6, 6, 7, 6, 6, 
	6, 7, 6, 6, 6, 6, 3, 1, 
	5, 4, 9, 1, 4, 4, 4, 5, 
	4, 7, 3, 4, 7, 7, 7, 8, 
	7
};

static const char _ragel_url_email_range_lengths[] = {
	0, 5, 5, 5, 5, 3, 3, 3, 
	3, 3, 3, 11, 3, 3, 3, 3, 
	3, 3, 3, 4, 4, 3, 3, 4, 
	4, 3, 3, 4, 4, 5, 5, 5, 
	5, 5, 5, 5, 5, 5, 5, 5, 
	5, 6, 6, 5, 5, 6, 6, 5, 
	5, 6, 6, 5, 5, 5, 4, 0, 
	5, 3, 11, 1, 3, 3, 3, 4, 
	4, 5, 5, 5, 5, 5, 5, 6, 
	6
};

static const short _ragel_url_email_index_offsets[] = {
	0, 0, 11, 21, 29, 38, 45, 51, 
	56, 62, 66, 72, 93, 100, 106, 113, 
	119, 126, 132, 138, 146, 153, 159, 165, 
	173, 180, 186, 192, 200, 207, 219, 230, 
	242, 252, 264, 277, 289, 302, 314, 327, 
	339, 351, 365, 378, 390, 402, 416, 429, 
	441, 453, 467, 480, 492, 504, 516, 524, 
	526, 537, 545, 566, 569, 577, 585, 593, 
	603, 612, 625, 634, 644, 657, 670, 683, 
	698
};

static const char _ragel_url_email_indicies[] = {
	0, 2, 3, 4, 0, 0, 0, 5, 
	6, 6, 1, 0, 7, 8, 0, 0, 
	0, 0, 0, 0, 1, 9, 9, 9, 
	9, 9, 9, 9, 1, 9, 8, 9, 
	9, 9, 9, 9, 9, 1, 10, 11, 
	12, 13, 14, 14, 1, 15, 16, 14, 
	14, 14, 1, 15, 14, 14, 14, 1, 
	15, 17, 14, 14, 14, 1, 14, 18, 
	18, 1, 15, 17, 14, 19, 19, 1, 
	20, 21, 21, 20, 20, 21, 21, 21, 
	21, 20, 20, 20, 21, 21, 21, 21, 
	21, 21, 21, 21, 1, 22, 23, 24, 
	25, 18, 18, 1, 15, 26, 14, 14, 
	14, 1, 27, 28, 29, 30, 18, 18, 
	1, 15, 31, 14, 14, 14, 1, 32, 
	33, 34, 35, 18, 18, 1, 15, 31, 
	30, 14, 14, 1, 15, 31, 27, 14, 
	14, 1, 15, 31, 36, 30, 27, 14, 
	14, 1, 15, 31, 27, 14, 14, 14, 
	1, 15, 26, 25, 14, 14, 1, 15, 
	26, 22, 14, 14, 1, 15, 26, 37, 
	25, 22, 14, 14, 1, 15, 26, 22, 
	14, 14, 14, 1, 15, 16, 13, 14, 
	14, 1, 15, 16, 10, 14, 14, 1, 
	15, 16, 38, 13, 10, 14, 14, 1, 
	15, 16, 10, 14, 14, 14, 1, 0, 
	39, 40, 7, 8, 0, 0, 0, 41, 
	41, 41, 1, 0, 39, 7, 8, 0, 
	0, 0, 41, 41, 41, 1, 0, 39, 
	42, 7, 8, 0, 0, 0, 41, 41, 
	41, 1, 0, 7, 8, 0, 0, 0, 
	41, 43, 43, 1, 0, 39, 42, 7, 
	8, 0, 0, 0, 41, 44, 44, 1, 
	0, 45, 46, 47, 7, 8, 0, 0, 
	0, 48, 43, 43, 1, 0, 39, 49, 
	7, 8, 0, 0, 0, 41, 41, 41, 
	1, 0, 50, 51, 52, 7, 8, 0, 
	0, 0, 53, 43, 43, 1, 0, 39, 
	54, 7, 8, 0, 0, 0, 41, 41, 
	41, 1, 0, 55, 56, 57, 7, 8, 
	0, 0, 0, 58, 43, 43, 1, 0, 
	39, 54, 7, 8, 0, 0, 0, 53, 
	41, 41, 1, 0, 39, 54, 7, 8, 
	0, 0, 0, 50, 41, 41, 1, 0, 
	39, 54, 59, 7, 8, 0, 0, 0, 
	53, 50, 41, 41, 1, 0, 39, 54, 
	7, 8, 0, 0, 0, 50, 41, 41, 
	41, 1, 0, 39, 49, 7, 8, 0, 
	0, 0, 48, 41, 41, 1, 0, 39, 
	49, 7, 8, 0, 0, 0, 45, 41, 
	41, 1, 0, 39, 49, 60, 7, 8, 
	0, 0, 0, 48, 45, 41, 41, 1, 
	0, 39, 49, 7, 8, 0, 0, 0, 
	45, 41, 41, 41, 1, 0, 39, 40, 
	7, 8, 0, 0, 0, 5, 41, 41, 
	1, 0, 39, 40, 7, 8, 0, 0, 
	0, 2, 41, 41, 1, 0, 39, 40, 
	61, 7, 8, 0, 0, 0, 5, 2, 
	41, 41, 1, 0, 39, 40, 7, 8, 
	0, 0, 0, 2, 41, 41, 41, 1, 
	0, 39, 42, 7, 8, 0, 0, 0, 
	41, 62, 62, 1, 0, 39, 42, 7, 
	8, 0, 0, 0, 41, 63, 63, 1, 
	0, 39, 42, 64, 8, 0, 0, 0, 
	41, 63, 63, 1, 9, 65, 9, 9, 
	9, 9, 9, 1, 66, 1, 0, 2, 
	3, 4, 0, 0, 0, 5, 41, 41, 
	1, 15, 17, 21, 67, 14, 19, 19, 
	1, 20, 21, 21, 20, 20, 21, 21, 
	21, 21, 20, 20, 20, 21, 21, 21, 
	21, 21, 21, 21, 21, 1, 21, 67, 
	1, 15, 17, 21, 67, 14, 14, 14, 
	1, 15, 17, 21, 67, 35, 14, 14, 
	1, 15, 17, 21, 67, 32, 14, 14, 
	1, 15, 17, 21, 68, 67, 35, 32, 
	14, 14, 1, 15, 17, 21, 67, 32, 
	14, 14, 14, 1, 0, 39, 42, 21, 
	69, 8, 0, 0, 0, 41, 44, 44, 
	1, 9, 21, 9, 9, 9, 70, 9, 
	9, 1, 9, 21, 8, 9, 9, 9, 
	70, 9, 9, 1, 0, 39, 42, 21, 
	69, 8, 0, 0, 0, 41, 41, 41, 
	1, 0, 39, 42, 21, 69, 8, 0, 
	0, 0, 58, 41, 41, 1, 0, 39, 
	42, 21, 69, 8, 0, 0, 0, 55, 
	41, 41, 1, 0, 39, 42, 21, 71, 
	69, 8, 0, 0, 0, 58, 55, 41, 
	41, 1, 0, 39, 42, 21, 69, 8, 
	0, 0, 0, 55, 41, 41, 41, 1, 
	0
};

static const char _ragel_url_email_trans_targs[] = {
	2, 0, 29, 47, 49, 48, 51, 3, 
	5, 4, 6, 25, 27, 26, 8, 7, 
	12, 9, 10, 57, 11, 58, 13, 21, 
	23, 22, 14, 15, 17, 19, 18, 16, 
	60, 61, 63, 62, 20, 24, 28, 30, 
	34, 31, 32, 33, 65, 35, 43, 45, 
	44, 36, 37, 39, 41, 40, 38, 68, 
	69, 71, 70, 42, 46, 50, 52, 53, 
	54, 55, 56, 59, 64, 66, 67, 72
};

static const char _ragel_url_email_trans_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 1, 0, 1, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	1, 1, 1, 1, 0, 0, 0, 0, 
	0, 0, 0, 0, 1, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 1, 
	1, 1, 1, 0, 0, 0, 0, 0, 
	0, 0, 0, 1, 1, 1, 1, 1
};

static const int ragel_url_email_start = 1;





vector<uint8_t> ragel_tokenizer::ragel_map;
atomic_flag ragel_tokenizer::ragel_map_flag = ATOMIC_FLAG_INIT;

ragel_tokenizer::ragel_tokenizer(unsigned url_email_tokenizer) : unicode_tokenizer(url_email_tokenizer) {
  while (ragel_map_flag.test_and_set()) {}
  if (ragel_map.empty()) {
    for (uint8_t ascii = 0; ascii < 128; ascii++)
      ragel_map.push_back(ascii);

    ragel_map_add(U'…', 160);
    ragel_map_add(U'’', 161);
    ragel_map_add(U'‘', 162);
    ragel_map_add(U'‐', 163);
  }
  ragel_map_flag.clear();
}

void ragel_tokenizer::ragel_map_add(char32_t chr, uint8_t mapping) {
  if (chr >= ragel_map.size())
    ragel_map.resize(chr + 1, 128);
  ragel_map[chr] = mapping;
}

bool ragel_tokenizer::ragel_url_email(unsigned version, const vector<char_info>& chars, size_t& current, vector<token_range>& tokens) {
  int cs;

  size_t start = current, end = current;
  
	{
	cs = ragel_url_email_start;
	}

	{
	int _klen;
	const short *_keys;
	int _trans;
	short _widec;

	if ( ( current) == ( (chars.size() - 1)) )
		goto _test_eof;
	if ( cs == 0 )
		goto _out;
_resume:
	_widec = ( ragel_char(chars[current]));
	_klen = _ragel_url_email_cond_lengths[cs];
	_keys = _ragel_url_email_cond_keys + (_ragel_url_email_cond_offsets[cs]*2);
	if ( _klen > 0 ) {
		const short *_lower = _keys;
		const short *_mid;
		const short *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( _widec < _mid[0] )
				_upper = _mid - 2;
			else if ( _widec > _mid[1] )
				_lower = _mid + 2;
			else {
				switch ( _ragel_url_email_cond_spaces[_ragel_url_email_cond_offsets[cs] + ((_mid - _keys)>>1)] ) {
	case 0: {
		_widec = (short)(256u + (( ragel_char(chars[current])) - 0u));
		if ( 
 version >= 2  ) _widec += 256;
		break;
	}
				}
				break;
			}
		}
	}

	_keys = _ragel_url_email_trans_keys + _ragel_url_email_key_offsets[cs];
	_trans = _ragel_url_email_index_offsets[cs];

	_klen = _ragel_url_email_single_lengths[cs];
	if ( _klen > 0 ) {
		const short *_lower = _keys;
		const short *_mid;
		const short *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( _widec < *_mid )
				_upper = _mid - 1;
			else if ( _widec > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (unsigned int)(_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _ragel_url_email_range_lengths[cs];
	if ( _klen > 0 ) {
		const short *_lower = _keys;
		const short *_mid;
		const short *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( _widec < _mid[0] )
				_upper = _mid - 2;
			else if ( _widec > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += (unsigned int)((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	_trans = _ragel_url_email_indicies[_trans];
	cs = _ragel_url_email_trans_targs[_trans];

	if ( _ragel_url_email_trans_actions[_trans] == 0 )
		goto _again;

	switch ( _ragel_url_email_trans_actions[_trans] ) {
	case 1:
	{ end = current + 1; }
	break;
	}

_again:
	if ( cs == 0 )
		goto _out;
	if ( ++( current) != ( (chars.size() - 1)) )
		goto _resume;
	_test_eof: {}
	_out: {}
	}



  if (end > start) {
    tokens.emplace_back(start, end - start);
    current = end;
    return true;
  } else {
    current = start;
    return false;
  }
}

} // namespace morphodita
} // namespace ufal
