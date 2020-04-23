#pragma once

#include "pch.h"

enum class filtering_edges
{
	false_all, // Makes function give result false for many of the inputs
	optimal,
	true_all // Makes function give result true for many of the inputs
};

constexpr double usual_poem_threshold = 0.72;
constexpr double all_poem_threshold = 1.0;
constexpr double all_prose_poem_threshold = 0.2258;

constexpr double usual_english_percent_threshold = 0.5; //  = english_words / (english_words + russian_words)
constexpr double all_russian_percent_threshold = 0.9; //  = english_words / (english_words + russian_words)
constexpr double all_english_percent_threshold = 0.1; //  = english_words / (english_words + russian_words)

constexpr size_t string_length_count_threshold = 50;

// Helpers:

double count_big_string_percent(const vector<string>& strings, double threshold);
double count_englishness(const string& text);

inline double get_poem_threshold(const filtering_edges& edge)
{
	switch (edge)
	{
	case filtering_edges::false_all:
		return all_prose_poem_threshold;
	case filtering_edges::optimal:
		return usual_poem_threshold;
	case filtering_edges::true_all:
		return all_poem_threshold;
	}
	return usual_poem_threshold;
}

inline double get_english_threshold(const filtering_edges& edge)
{
	switch (edge)
	{
	case filtering_edges::false_all:
		return all_russian_percent_threshold;
	case filtering_edges::optimal:
		return usual_english_percent_threshold;
	case filtering_edges::true_all:
		return all_english_percent_threshold;
	}
	return usual_english_percent_threshold;
}

inline bool is_english(const string& text, const filtering_edges& edge)
{
	return count_englishness(text) > get_english_threshold(edge);
}


inline bool is_poem(const string& text, const filtering_edges& edge)
{
	return count_big_string_percent(split_lines(text), string_length_count_threshold) < get_poem_threshold(edge);
}

