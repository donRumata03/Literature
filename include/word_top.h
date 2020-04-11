#pragma once

#include "pch.h"
#include "common_word_top.h"



class word_top {
	unordered_map<string, int> data;
	pms word_length_distr;
	luint max_word_length = 0;
	vector<pair<string, uint64_t>> data_sorted_by_num;
	light_language* lang = nullptr;
	map<word_types, int> word_type_distribution;

	vector<word*> word_objects;
	void cook_top(const vector<string>& words);
public:
	word_top() = delete;
	explicit word_top(const string& s);
	word_top(const string& s, light_language& lang, bool debug_words = false);

	// Printings:
	void print_first(int n);
	void print_if_more_than(uint64_t num);
	void print_word_type_distribution();
	
	// Dists:
	static double meaning_dist(word_top& t1, word_top &t2, word_top& common_top,
		double common_pow, double diff_pow);
	static double grammar_dist(word_top& t1, word_top &t2);
};

