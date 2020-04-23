#include "pch.h"
#include "word_top.h"
#include "word_util.h"
// #include "literature_parser.h"

using namespace std;

/*

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

*/

/*
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

*/

void word_top::from_serialization(const string& data_path, const string& metadata_path)
{
	load_metadata_from_file(metadata_path);
	load_from_file(data_path);
}

void word_top::add_to_distribution(const vector<string>& parsed_words)
{
	for (auto& str_word : parsed_words)
	{
		max_word_length = max(max_word_length, str_word.size());

		// Try to find information from language:
		auto word_data = lang->get_word_variations(str_word);
		if (word_data)
		{
			++word_type_distribution[word_data->operator[](0)->word_type];
		}
	}
}


void word_top::add_words_with_info(const string& text)
{
	vector<string> for_unknowns;
	vector<string> parsed_words;
	try {
		parsed_words = lemmatize_words(text, *lang, for_unknowns, true);
	}
	catch (exception& e)
	{
		cout << e.what() << endl;
		return;
	}

	add_words_with_info(parsed_words, for_unknowns);
}

void word_top::add_words_with_info(const vector<string>& parsed_words, const vector<string>& unknowns)
{
	for (auto& w : unknowns)
	{
		++unknown_words[w];
	}

	add_data(parsed_words);
	add_to_distribution(parsed_words);
}

void word_top::load_metadata_from_file(const string& filename)
{
	vector<string> readed_data = split_lines(readFile(filename));
	for (auto& str_data : Slice(readed_data, 0, noslice)) {
		size_t this_freq = 0;
		stringstream ss;
		ss << split(str_data)[1];
		ss >> this_freq;
		
		pair<word_types, size_t> distr_data = { string_to_word_type_converter[split(str_data)[0]], this_freq };
		word_type_distribution.insert(distr_data);
	}
}
// Distance functions:

double word_top::meaning_dist(word_top& t1, word_top& t2, common_word_top& common_top,
                              const double common_pow, const double diff_pow)
{
	unordered_set<string> words;
	for (const auto& p : t1.frequencies) words.insert(p.first);
	for (const auto& p : t2.frequencies) words.insert(p.first);

	double difference = 0;

	for (const auto& word : words)
	{
		double log_common_percent = log(common_top.get_percent_word_frequency(word));
		
		double log_percent_1 = log(t1.get_percent_word_frequency(word));
		double log_percent_2 = log(t2.get_percent_word_frequency(word));

		double freq_diff = log_percent_1 - log_percent_2;
		
		double tf = sgn(freq_diff) * pow(abs(freq_diff), diff_pow);
		double df = sgn(log_common_percent) * pow(abs(log_common_percent), common_pow);
		
		difference += tf / df;
	}
	return difference;
}


double word_top::grammar_dist(word_top& t1, word_top& t2)
{
	// Append difference caused by different used word types:
	double word_type_difference = 0;

	set<word_types> word_type_buff;
	double sigma1 = 0, sigma2 = 0;
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
		size_t val1 = (t1.word_type_distribution.find(this_type) != t1.word_type_distribution.end())
			           ? (t1.word_type_distribution[this_type])
			           : (size_t(0));
		size_t val2 = (t2.word_type_distribution.find(this_type) != t2.word_type_distribution.end())
			           ? (t2.word_type_distribution[this_type])
			           : (size_t(0));

		double percent1 = double(val1) / sigma1, percent2 = double(val2) / sigma2;

		word_type_difference += square(percent1 - percent2);
	}

	double word_type_err = word_type_difference;

	return word_type_err;
}
