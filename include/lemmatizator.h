#pragma once

#include "pch.h"

#include <basic_parser.h>
#include "word_util.h"

inline vector<string> lemmatize_words(const string& text, light_language& lang, vector<string>& unknown_words, const bool debug_words = false)
{
	auto raw_words = parse_data(text);
	// cout << "Parsed data!" << endl;
	vector<string> words(raw_words.size());
	copy_if(raw_words.begin(), raw_words.end(), words.begin(), [](const string& s) {return (s != "-") && (!is_english(s)); });

	vector<string> real_words;
	real_words.reserve(words.size());
	size_t lemmatizated_counter = 0, non_semantic_counter = 0;

	
	for (auto& this_word_string : words)
	{
		vector<word*>* response = lang.get_word_variations(this_word_string);
		if (!response) {
			if (this_word_string.size() > 3) {
				real_words.push_back(this_word_string);
				if (debug_words) unknown_words.push_back(this_word_string);
			}
			continue;
		}
		word& this_word = *response->operator [](0);

		if (any_of(response->begin(), response->end(), [](word* w) {return !(w->has_semantic); })) {
			non_semantic_counter++;
			continue;
		}
		vector<word>& word_block = *(this_word.parent_block);
		word main_form = word_block[0];

		lemmatizated_counter++;

		real_words.push_back(main_form.data);
	}
	return real_words;
}


inline vector<string> lemmatize_words(const string& text, light_language& lang)
{
	vector<string> temp;
	return lemmatize_words(text, lang, temp, false);
}


