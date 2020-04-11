#pragma once

#include "pch.h"

#include "../src/tests.h"
#include "lemmatizator.h"

inline void watch_word_data(const vector<string>& values, light_language& lang, const pair<size_t, size_t>& range)
{
	/*
	for (size_t index  = range.first; index < range.second; index++)
	{
		auto query_res = lang.get_word_variations(values[index]);
		if (query_res->empty()) continue;
		word* first_word = query_res->operator[](0);
	}
	*/
	vector<string> unknown_words;

	// auto some_stuff = parse_data(join(" ", Slice(values, range.first, range.second)));
	auto some_other_stuff = lemmatize_words(join(" ", Slice(values, range.first, range.second)), lang, unknown_words, true);
}

inline double multithread_test(const vector<string>& data, light_language& lang, const size_t thread_number, const size_t times)
{
	double summa = 0;

	for(size_t count = 0; count < times; count++)
	{
		Timer this_time;

		auto ranges = distribute_task_ranges(data.size(), thread_number);

		vector<thread> threads;
		for (size_t thread_index = 0; thread_index < thread_number; thread_index++)
		{
			threads.emplace_back(watch_word_data, ref(data), ref(lang), ref(ranges[thread_index]));
		}
		for (auto& the_thread : threads) the_thread.join();
		
		summa += this_time.get_time();
	}

	return summa / times;
}

inline double single_thread_test(const vector<string>& data, light_language& lang, const size_t times)
{
	double summa = 0;

	for (size_t count = 0; count < times; count++)
	{
		Timer this_time;

		watch_word_data(data, lang, {0, data.size()});		

		summa += this_time.get_time();
	}
	return summa / times;
}

inline void test_multi_text()
{

	size_t thread_number = 12;
	size_t copy_times = 20;
	size_t copy_step = 2;
	
	string filename = R"(D:\Literature_data\All_books\Толстой Лев\Война и мир Том 3 и 4\text.txt)";
	vector<string> file_data = split_words(readFile(filename));

	
	
	light_language lang(russian_filename);

	pms one_threads, multi_threads;

	for(size_t this_times = copy_step; this_times <= copy_times; this_times += copy_step)
	{
		vector<string> this_data;
		this_data.reserve(this_times * file_data.size() + 1000);
		
		for (size_t time = 0; time < this_times; time++)
		{
			for (auto& str : file_data)
			{
				this_data.push_back(str);
			}
		}

		cout << "This times: " << this_times << endl;
		double one_thread_performance = single_thread_test(this_data, lang, 2);
		cout << "Single_thread: " << one_thread_performance << " ms" << endl;

		double multi_thread_performance = multithread_test(this_data, lang, thread_number, 5);
		cout << "Multithread: " << multi_thread_performance << " ms" << endl;


		one_threads.push_back({ this_times, this_times / one_thread_performance });
		multi_threads.push_back({ this_times,  this_times / multi_thread_performance });
	}
	add_to_plot(one_threads);
	add_to_plot(multi_threads);

	show_plot();
}
