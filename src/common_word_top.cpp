#include "pch.h"

#include "common_word_top.h"
#include "lemmatizator.h"


void common_word_top::add_data(const string& text)
{
	vector<string> for_unknowns;
	vector<string> parsed_words;
	try {
		// cout << "Started lemmatization!" << endl;
		parsed_words = lemmatize_words(text, *lang, for_unknowns, true);
	} catch(exception& e)
	{
		// cout << "Error occured!" << endl;
		cout << e.what() << endl;
		return;
	}

	for (auto& w : for_unknowns)
	{
		++unknown_words[w];
	}
	
	add_data(parsed_words);	
}

void common_word_top::add_data(const vector<string>& parsed_words)
{
	for (auto& w : parsed_words)
	{
		++frequencies[w];
	}

	total_words += parsed_words.size();
}

void thread_word_top_making_function(const vector<string>& paths, const pair<size_t, size_t>& range, common_word_top& res)
{

	for (size_t path_index = range.first; path_index < range.second; path_index++) {
		// cout << this_thread::get_id() << " : " <<  &res << endl;
		// cout << paths[path_index] << endl;
		res.add_data(readFile(paths.at(path_index)));
	}
}

void common_word_top::add_file_data(const vector<string>& paths, const bool allow_threading, const size_t thread_number)
{
	if (allow_threading) {
		vector<thread> threads;
		vector<common_word_top> tops;

		ensure_names_loaded();
		
		auto ranges = distribute_task_ranges(paths.size(), thread_number);

		
		for (size_t this_thread_index = 0; this_thread_index < thread_number; this_thread_index++) {
			auto& this_top = tops.emplace_back(lang);
			threads.emplace_back(thread_word_top_making_function, ref(paths), ref(ranges[this_thread_index]), ref(this_top));
		}

		for (auto& this_thread : threads) this_thread.join();

		for (auto& top : tops)
		{
			merge(top);
		}
	}

	else {
		size_t target_number = 0;

		for (auto& path : paths) {
			add_data(readFile(path));
			cout << path << " " << 100. * target_number / paths.size() << " %" << endl;
			target_number++;
		}
	}
}

void common_word_top::cut(const size_t min_freq)
{
	for (auto it = frequencies.cbegin(); it != frequencies.cend();)
	{
		if (it->second < min_freq)
		{
			frequencies.erase(it++);
			total_words--;
		}
		else
		{
			++it;
		}
	}
}

vector<pair<string, size_t>> common_word_top::get_sorted_words() const
{
	return get_sorted_elements(unknown_words);
}


vector<pair<string, size_t>> common_word_top::get_sorted_unknown_words() const
{
	return get_sorted_elements(frequencies);
}


void common_word_top::store_unknowns_to_file(const string& filename, const word_top_output_descriptor& descriptor) const
{
	ofstream out_file(filename);

	output_map(unknown_words, out_file, descriptor);

	out_file.close();
}

void common_word_top::output_popular_words(ostream& out, const word_top_output_descriptor& descriptor) const
{
	output_map(frequencies, out, descriptor);
}


void common_word_top::store_to_file(const string& filename) const
{
	ofstream out_file(filename);
	auto sorted = get_sorted_elements(frequencies);

	for (auto& p : sorted)
	{
		out_file << p.first << " " << p.second << endl;
	}

	out_file.close();
}

void common_word_top::load_from_file(const string& filename)
{
	auto lines = split_lines(readFile(filename));

	for(auto & line : lines)
	{
		auto line_els = split(line);
		if (line_els.empty()) continue;

		size_t sz = 0;
		size_t this_freq = stoll(line_els[line_els.size() - 1], &sz, 0);

		frequencies[line_els[0]] += this_freq;
		total_words += this_freq;
	}
}

pair<string, size_t> common_word_top::most_frequent_word()
{
	pair<string, size_t> best_pair;
	for (auto p : frequencies)
	{
		if (p.second > best_pair.second) best_pair = p;
	}

	return best_pair;
}


size_t common_word_top::get_abs_word_frequency(const string& w) const
{
	auto fnd = frequencies.find(w);

	if (fnd != frequencies.end())
	{
		return fnd->second;
	}
	return 0;
}

void common_word_top::merge(const common_word_top& other)
{
	total_words += other.total_words;

	for (auto& p : other.frequencies)
	{
		// TODO: Optimize!
		auto fnd = frequencies.find(p.first);
		if (fnd == frequencies.end()) frequencies[p.first] = p.second;
		else frequencies[p.first] += p.second;
	}
}

void common_word_top::output_map(const unordered_map<string, size_t>& target_map, ostream& out, const word_top_output_descriptor& descriptor) const
{
	auto sorted = get_sorted_elements(target_map);

	vector<pair<string, size_t>> res;

	size_t word_index = 0;
	for (auto& p : sorted)
	{
		if (descriptor.low_word_freq_limit != size_t(-1)) {
			if (p.second < descriptor.low_word_freq_limit) break;
		}
		if (descriptor.word_number != size_t(-1)) {
			if (word_index > descriptor.word_number) continue;
		}

		res.emplace_back(p);
		word_index++;
	}

	for (auto& p : res)
	{
		out << p.first << " " << p.second << endl;
	}
}
