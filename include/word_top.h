#pragma once

#include "pch.h"
#include "common_word_top.h"



struct word_top : common_word_top {
	vector<word*> word_objects;

	pms word_length_distr;
	luint max_word_length = 0;
	map<word_types, size_t> word_type_distribution;

	void from_serialization(const string& data_path, const string& metadata_path);
	
	void add_to_distribution(const vector<string>& parsed_words);

	word_top() = default;
	explicit word_top(light_language* _lang) : common_word_top(_lang) {}

	void add_words_with_info(const string& text);
	void add_words_with_info(const vector<string>& parsed_words, const vector<string>& unknowns = {});

	void load_metadata_from_file(const string& filename);
	
	// Dists:
	static double meaning_dist(word_top& t1, word_top &t2, common_word_top& common_top,
		double common_pow, double diff_pow);

	static double grammar_dist(word_top& t1, word_top &t2);
};

