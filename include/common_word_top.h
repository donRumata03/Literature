#pragma once

#include "pch.h"
#include "lemmatizator.h"

struct word_top_output_descriptor
{
	size_t low_word_freq_limit = size_t(-1);
	size_t word_number = size_t(-1);
};

struct common_word_top
{
	light_language* lang = nullptr;
	unordered_map<string, size_t> frequencies;
	unordered_map<string, size_t> unknown_words;
	size_t total_words = 0;


	common_word_top() = default;
	explicit common_word_top(light_language* _lang) : lang(_lang) {}
	void assign_lang(light_language* _lang) { lang = _lang; }
	
	void add_data(const string& text);
	void add_data(const vector<string>& parsed_words);
	void add_file_data(const vector<string>& paths, bool allow_threading = false, size_t thread_number = 12);

	void cut(size_t min_freq = 3);
	[[nodiscard]] size_t different_words() const { return frequencies.size(); }

	[[nodiscard]] vector<pair<string, size_t>> get_sorted_words() const;
	[[nodiscard]] vector<pair<string, size_t>> get_sorted_unknown_words() const;

	void store_unknowns_to_file(const string& filename, const word_top_output_descriptor& descriptor) const;
	void output_popular_words(ostream& out, const word_top_output_descriptor& descriptor) const;

	void store_to_file(const string& filename) const;
	void load_from_file(const string& filename);

	pair<string, size_t> most_frequent_word();
	
	[[nodiscard]] size_t get_abs_word_frequency(const string& w) const;
	[[nodiscard]] double get_percent_word_frequency(const string& w) const { return double(get_abs_word_frequency(w)) / total_words; }

	void merge(const common_word_top& other);
private:
	void output_map(const unordered_map<string, size_t>& target_map, ostream& out,
	                const word_top_output_descriptor& descriptor) const;
};
