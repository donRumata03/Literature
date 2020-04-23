#pragma once

#include "basic_parser.h"
#include "pch.h"

#include "Library.h"
#include "sjson.hpp"
#include "word_top.h"
#include "model_error_function.h"

#include "suspicious_file_detector.h"

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
	word_top test_w(&l);
	test_w.add_words_with_info(test_str);
	// test_w.print_word_type_distribution();
	// test_w.print_first(100);
	
	// word_top t(readFile(war_and_piece_fname), l, true);
	// cout << endl << endl;
	// t.print_word_type_distribution();
	// t.print_first(100);
}

inline void json_test()
{
	JSON j = JSON::Load(readFile(R"(D:\Projects\Literature_downloading\cp1251_res\wiki_data.json)"));
	cout << j << endl;
}


inline void common_word_top_test()
{
	light_language lang(russian_filename);
	common_word_top all_top(&lang);
	Library lib(lang);
	
	all_top.add_file_data(lib.generate_all_artwork_paths(), true, 4);

	all_top.output_popular_words(cout, {size_t(-1), 100});
	// all_top.store_to_file(R"(D:\Literature_data\temp\common_word_top.txt)");
	all_top.store_to_file(R"(D:\Literature_data\temp\test_common_word_top.txt)");
	
	all_top.store_unknowns_to_file(R"(D:\Literature_data\output\unknowns.txt)", { 50 });
}


inline void load_word_top_test()
{
	light_language lang(russian_filename);
	common_word_top all_top(&lang);

	// all_top.add_file_data(load_literature_paths_from_file(), false, 12);
	all_top.load_from_file(R"(D:\Literature_data\temp\common_word_top.txt)");
	all_top.output_popular_words(cout, { size_t(-1), 100 });
	cout << "Words total: " << all_top.total_words << endl;
}


inline void test_literature_model_error_function(){
	// TODO!
}


inline void lib_test()
{
	Library lib;
	lib.load_metadata_from_json();
	lib.load_wiki_data();
	// lib.ensure_good_texts_loaded(false);
	
	for (size_t i = 0; i < 1; i++) {
		auto author = lib.generate_author(true, { 3, 1000000 });
		cout << author->name << " : " << author->descriptor << endl;
		cout << author->artworks[0].raw_content << endl;
	}

	
	cout << "________________________________________" << endl;

	cout << "different good author artworks: " << endl;
	auto [good1, good2] = lib.generate_different_authors_artworks(true);
	cout << good1->author_name << " : " <<  good1->name << "; " << good2->author_name << " : " << good2->name << endl;
	cout << "________________________________________" << endl;

	cout << "one good author artworks: " << endl;
	auto [good11, good12] = lib.generate_one_author_different_artworks(true);
	cout << good11->author_name << " : " << good11->name << "; " << good12->author_name << " : " << good12->name << endl;
	cout << "________________________________________" << endl;
}


inline void parse_data_test()
{
	Library lib;
	lib.load_metadata_from_json();

	vector<string> files = lib.generate_all_artwork_paths();

	
	cout << "Generated files" << endl;
	
	for(auto& filename : files)
	{
		try
		{
			auto parsed = parse_text(filename);
		} catch (exception& e) {
			cout << "Exception at " << filename << " : " << e.what() << endl;
		}
	}
}

inline void load_all_tops_test()
{
	load_all_names();

	Library lib;
	lib.load_metadata_from_json();
	lib.load_language();
	lib.multithread_load_tops();

	return;
	
	auto tolstoy = lib.get_author_by_name("Толстой Лев");

	for (auto & artwork: tolstoy->artworks) {
		cout << artwork.file_size << " " << artwork.parsed_content.size() << " " << artwork.raw_content.size() << " " << artwork.top.frequencies.size() << " " << artwork.unknown_words.size()
		<< endl;
	}

	while(true) {}
}

inline void range_test()
{
	vector<pair<size_t, size_t>> test_pairs = {
		{1, 12},
		{5, 12},
		{12, 12},
		{13, 12},
		{20, 12},
		{31, 12},
		{32, 12},
		{33, 12},
		{34, 12},
		{35, 12},
		{93, 12},
		{1000, 12}
	};


	for(size_t val = 0; val < 100; val++) 		cout << distribute_task_ranges(val, 12) << endl;

	cout << "_______________________" << endl;

	for (auto& p : test_pairs)
		cout << distribute_task_ranges(p.first, p.second) << endl;
}


inline void percent_plotter_test()
{
	cout << percent_plotter(1, size_t(10)) << endl;
}

inline void test_englishness()
{
	vector<string> eng_paths = {
		R"(D:\Literature_data\All_books\Силверберг Роберт\Venus Trap\text.txt)",
	};

	vector<string> russian_paths = {
		R"(D:\Literature_data\All_books\Толстой Лев\Война и мир Том 3 и 4\text.txt)",
		R"(D:\Literature_data\All_books\Толстой Лев\Война и мир Первый вариант романа\text.txt)",
		R"(D:\Literature_data\All_books\Толстой Лев\Война и мир Книга 1\text.txt)",
		R"(D:\Literature_data\All_books\Фролов Александр\Microsoft Visual C++ и MFC Программирование для Windows 95 и Windows NT Часть 2\text.txt)"
	};

	vector<string> poems = {
		R"(D:\Literature_data\All_books\Есенин Сергей\Полное собрание стихотворений\text.txt)"
	};
	
	vector<string> paths;
	for (auto& e : eng_paths) paths.push_back(e);
	for (auto& e : russian_paths) paths.push_back(e);
	for (auto& e : poems) paths.push_back(e);

	for (auto &path : paths)
	{
		cout << path << " : " << count_englishness(readFile(path)) << " " <<
			is_english(readFile(path), filtering_edges::true_all) << " " <<
			is_english(readFile(path), filtering_edges::optimal) << " " <<
			is_english(readFile(path), filtering_edges::false_all) << " " <<
									"\t" <<
			count_big_string_percent(split_lines(readFile(path)), 50) << " " <<
			is_poem(readFile(path), filtering_edges::true_all) << " " <<
			is_poem(readFile(path), filtering_edges::optimal) << " " <<
			is_poem(readFile(path), filtering_edges::false_all) << " "
		<< endl;
	}
	
}

inline void test_erasing()
{
	light_language lang(default_language_name);
	common_word_top top(&lang);

	top.load_from_file(R"(D:\Literature_data\temp\small_common_word_top.txt)");
	top.output_popular_words(cout, { size_t(-1), 100 });
	cout << "Words total: " << top.different_words() << endl;
	top.cut(10);
	cout << "Now words total: " << top.different_words() << endl;
	top.store_to_file(R"(D:\Literature_data\temp\very_small_common_word_top.txt)");
}


inline void launch_processing_engpoemness()
{
	Library lib;
	lib.process_good_authors_engpoemness({10, 100000});


	cout << "________________________________________" << endl;

	cout << "different good author artworks: " << endl;
	auto [good1, good2] = lib.generate_different_authors_artworks(true);
	cout << good1->author_name << " : " << good1->name << "; " << good2->author_name << " : " << good2->name << endl;
	cout << "________________________________________" << endl;

	cout << "one good author artworks: " << endl;
	auto [good11, good12] = lib.generate_one_author_different_artworks(true);
	cout << good11->author_name << " : " << good11->name << "; " << good12->author_name << " : " << good12->name << endl;
	cout << "________________________________________" << endl;
	
}

 
inline void test_weighted_average()
{
	vector<pair<double, double>> values = {
		{ -2.70878, 14.4381 },
		{ -3.35359, 14.2886 },
		{ -2.48595, 13.4949 },
		{ -2.48467, 13.488	},
		{ -3.28784, 11.9357 },
		{ -3.56952, 13.6164 }
	};
	cout << weighted_average(values) << endl;
}


inline void test_lib_serializing()
{
	Library lib;
	lib.ensure_metadata_is_loaded(); 
	lib.ensure_wiki_data_loaded();

	lib.store_metadata_to_json(R"(D:\Literature_data\temp\test_lib_ser.txt)");
}