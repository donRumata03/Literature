#include "pch.h"
#include "tests.h"
#include "threading_test.h"


int main()
{
	system("chcp 1251");
	setlocale(LC_ALL, "Russian");

	
	// language_test();
	// json_test();
	// test_word_top();

	// common_word_top_test();
	load_word_top_test();

	// test_multi_text();
	
	return 0;
}
