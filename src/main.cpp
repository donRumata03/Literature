#include "pch.h"
#include "tests.h"
#include "threading_test.h"

#include "word_top_usage.h"
#include "Vocab_research.h"
#include "word_frequency_research.h"

int main()
{
	system("chcp 1251");
	setlocale(LC_ALL, "Russian");

	// launch_processing_author_vocab();

	test_lib_serializing();
	
	// analyze_word_freqs();
	
	//launch_best_word_getting();
	// test_weighted_average();
	
	// research_word_frequency();

	// parse_data_test();

	// load_all_tops_test();

	// test_erasing();
	
	// test_englishness();
	
	// lib_test();

	// launch_processing_engpoemness();
	
	// language_test();
	// json_test();
	// test_word_top();

	// common_word_top_test();
	// load_word_top_test();

	// test_multi_text();

	// find_word_top_dist_coefficients();
	
	return 0;
}
