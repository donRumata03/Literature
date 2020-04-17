#include "pch.h"
#include "tests.h"
#include "threading_test.h"
#include "word_top_usage.h"

int main()
{
	system("chcp 1251");
	setlocale(LC_ALL, "Russian");
	
	lib_test();
	
	// language_test();
	// json_test();
	// test_word_top();

	// common_word_top_test();
	// load_word_top_test();

	// test_multi_text();

	// find_word_top_dist_coefficients();
	
	return 0;
}
