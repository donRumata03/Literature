#pragma once

#include "literature_parser.h"
#include "pch.h"
#include "sjson.hpp"
#include "word_top.h"

using sjson::JSON;

constexpr bool enable_all_word_variations = true;

inline string russian_filename = R"(D:\Projects\Language\res\dictionary.txt)";
inline string test_filename = R"(D:\Projects\Language\res\little_test_data.txt)";


inline void language_test()
{
	light_language l(russian_filename);
	// light_language l(test_filename);
	string user_string;
	while (true) {

		cin >> user_string;
		if constexpr (!enable_all_word_variations) {
			word* w = l.get_main_form(user_string);
			if (!w) cout << "Try again!" << endl;
			else { w->print_data(); cout << endl; }
		}
		else {
			auto words = l.get_word_variations(user_string);
			if (!words) cout << "Can`t find this word :(\n";
			else {
				cout << "These " << (*words).size() << " words found:\n";
				for (auto& word : *words) {
					word->print_data();
					cout << "\n";
				}
			}
		}
		cout << "_________________________________________________\n";
	}
}

inline void test_word_top()
{
	string war_and_piece_fname = R"(D:\Literature_data\All_books\Толстой Лев\Война и мир Том 3 и 4\text.txt)";

	
	light_language l(russian_filename, true, true);

	string test_str = "Берн, Берлин, Йоханнесбург, Наташка ... - города Германия";
	word_top test_w(test_str, l, true);
	test_w.print_word_type_distribution();
	test_w.print_first(100);
	
	word_top t(readFile(war_and_piece_fname), l, true);
	cout << endl << endl;
	t.print_word_type_distribution();
	t.print_first(100);
}

inline void json_test()
{
	
	JSON j = JSON::Load(readFile(R"(D:\Projects\Literature_downloading\cp1251_res\wiki_data.json)"));
	cout << j << endl;
}


inline void common_word_top_test()
{
	light_language lang(russian_filename);
	common_word_top all_top(lang);

	all_top.add_file_data(load_literature_paths_from_file(), true, 4);

	all_top.output_popular_words(cout, {size_t(-1), 100});
	// all_top.store_to_file(R"(D:\Literature_data\temp\common_word_top.txt)");
	all_top.store_to_file(R"(D:\Literature_data\temp\test_common_word_top.txt)");
	
	all_top.store_unknowns_to_file(R"(D:\Literature_data\output\unknowns.txt)", { 50 });
}


inline void load_word_top_test()
{
	light_language lang(russian_filename);
	common_word_top all_top(lang);

	// all_top.add_file_data(load_literature_paths_from_file(), false, 12);
	all_top.load_from_file(R"(D:\Literature_data\temp\common_word_top.txt)");
	all_top.output_popular_words(cout, { size_t(-1), 100 });
	cout << "Words total: " << all_top.total_words << endl;
	all_top.store_unknowns_to_file(R"(D:\Literature_data\output\unknowns.txt)", { 50 });
}