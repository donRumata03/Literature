#include "pch.h"

#include "suspicious_file_detector.h"
#include "word_util.h"


// Poems
double count_big_string_percent(const vector<string>& strings, const double threshold)
{
	double res = 0;
	size_t str_size = 0;
	for (auto& line : strings)
	{
		if (line.size() >= threshold) {
			double this_appendix = log(line.size() - 9 * threshold / 10);
			res += this_appendix;
		}
		if (line.size() >= 5) str_size++;
	}
	return res / str_size;
}



// Englishness
inline double count_some_word_percent(const vector<string>& text, bool (*func) (const string&))
{
	size_t all_word_counter = 0, good_word_counter = 0;

	for (const auto& w : text)
	{
		if (w.size() <= 2)
		{
			// continue;
		}
		if (func(w)) good_word_counter++;
		all_word_counter++;
	}

	return 1. * good_word_counter / all_word_counter;
}

double count_english_word_percent(const vector<string>& text)
{
	return count_some_word_percent(text, is_english);
}

double count_russian_word_percent(const vector<string>& text)
{
	return count_some_word_percent(text, is_russian);
}



double count_englishness(const string& text)
{
	auto splitted = split_words(text);

	double englishs = count_english_word_percent(splitted);
	double russians = count_russian_word_percent(splitted);
	return englishs / (russians + englishs);
}

