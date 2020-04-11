#include "pch.h"
#include "word_top.h"
#include "word_util.h"
#include "literature_parser.h"

using namespace std;

void word_top::cook_top(const vector<string>& words)
{
	for (auto& word : words)
	{
		max_word_length = max(max_word_length, word.size());
		++data[word];
	}


	for (const auto& it : data)
	{
		data_sorted_by_num.emplace_back(it);
	}
	sort(data_sorted_by_num.begin(), data_sorted_by_num.end(), [](auto a, auto b) { return a.second > b.second; });

}

word_top::word_top(const string& s)
{
	auto raw_words = parse_data(s);
	vector<string> words(raw_words.size());
	copy_if(raw_words.begin(), raw_words.end(), words.begin(), [](auto& s) {return (s != "-") && (!is_english(s)); });

	cook_top(words);
}


word_top::word_top(const string& s, light_language& _lang, bool debug_words)
{
	this->lang = &_lang;

	auto raw_words = parse_data(s);
	vector<string> words(raw_words.size());
	copy_if(raw_words.begin(), raw_words.end(), words.begin(), [](auto& s) {return (s != "-") && (!is_english(s)); });

	vector<string> real_words;
	size_t lemmatizated_counter = 0, non_semantic_counter = 0;

	// Debugging stuff
	string debug_filename = R"(D:\Projects\Language\res\output\unknown_words.txt)";
	vector<string> unknown_words;
	
	for (auto& this_word_string : words)
	{
		vector<word*>* response = lang->get_word_variations(this_word_string);
		if (!response) {
			if (this_word_string.size() > 3) {
				real_words.push_back(this_word_string);
				if (debug_words) unknown_words.push_back(this_word_string);
			}
			continue;
		}
		word& this_word = *response->operator [](0);

		word_objects.push_back(&this_word);

		if (any_of(response->begin(), response->end(), [](word* w) {return !(w->has_semantic); })) {
			non_semantic_counter++;
			continue;
		}
		vector<word>& word_block = *(this_word.parent_block);
		word main_form = word_block[0];
		// word* main_form = lang->get_main_form(this_word_string);
		// if (!main_form) {
			// real_words.push_back(this_word_string);
			// continue;
		// }
		lemmatizated_counter++;
		
		real_words.push_back(main_form.data);
	}

	cout << "Percent of non_semantic_words: " << 100. * non_semantic_counter / real_words.size() << endl;
	cout << "Words lemmatizated: " << lemmatizated_counter << " of " << real_words.size() << "( " << 100. * lemmatizated_counter / real_words.size() <<  "% )" << endl;


	// Deubg unknown words:
	if (debug_words) {
		ofstream dfile(debug_filename);
		for (auto& uword : unknown_words)
		{
			dfile << uword << endl;
		}
		dfile.close();
	}
	
	cook_top(real_words);
	

	
	// Init word type distribution:
	for (auto& this_word : word_objects)
	{
		++word_type_distribution[this_word->word_type];
	}
}


void word_top::print_first(const int n)
{
	cout << "Max word length: " << max_word_length << endl;

	cout << endl << "Stat. first " << n << endl;
	for (int i = 0; i < min(n, int(data_sorted_by_num.size())); i++)
	{
		cout << data_sorted_by_num[i].first << " : " << data_sorted_by_num[i].second << endl;
	}
	cout << endl;
}

void word_top::print_if_more_than(const size_t num)
{
	cout << endl << "Stat; more than :" << num << endl;
	for (const auto& i : data_sorted_by_num)
	{
		if (i.second > num)
		{
			cout << i.first << ": " << i.second << endl;
		}
	}
	cout << endl;
}

void word_top::print_word_type_distribution()
{
	for (auto& p : word_type_distribution)
	{
		auto word_type = p.first;
		auto amount = p.second;
		print_word_type(word_type);
		cout << " : " << amount << endl;
	}
}


double word_top::meaning_dist(word_top& t1, word_top& t2, word_top& common_top,
                              double common_pow, double diff_pow)
{
	unordered_set<string> words;
	for (const auto& p : t1.data)
	{
		words.insert(p.first);
	}
	for (const auto& p : t2.data)
	{
		words.insert(p.first);
	}

	double difference = 0;

	for (const auto& word : words)
	{
		auto fnd1 = t1.data.find(word);
		uint64_t times_in_1 = (fnd1 == t1.data.end()) ? (0) : fnd1->second;

		auto fnd2 = t2.data.find(word);
		uint64_t times_in_2 = (fnd2 == t2.data.end()) ? (0) : fnd2->second;
		
		size_t common_times = common_top.data[word];
		
		double percent_in_1 = double(times_in_1) / t1.data.size();
		double percent_in_2 = double(times_in_2) / t2.data.size();


		
		difference += square(percent_in_1 - percent_in_2);
	}
	return difference;
}


double word_top::grammar_dist(word_top& t1, word_top& t2)
{
	// Append difference caused by different used word types:
	double word_type_difference = 0;

	set<word_types> word_type_buff;
	int sigma1 = 0, sigma2 = 0;
	for (auto& s : t1.word_type_distribution)
	{
		word_type_buff.insert(s.first);
		sigma1 += s.second;
	}
	for (auto& s : t2.word_type_distribution)
	{
		word_type_buff.insert(s.first);
		sigma2 += s.second;
	}


	for (word_types this_type : word_type_buff)
	{
		int val1 = (t1.word_type_distribution.find(this_type) != t1.word_type_distribution.end())
			           ? (t1.word_type_distribution[this_type])
			           : (0);
		int val2 = (t2.word_type_distribution.find(this_type) != t2.word_type_distribution.end())
			           ? (t2.word_type_distribution[this_type])
			           : (0);

		double percent1 = double(val1) / sigma1, percent2 = double(val2) / sigma2;

		word_type_difference += square(percent1 - percent2);
	}

	double word_type_err = word_type_difference;

	return word_type_err;
}
