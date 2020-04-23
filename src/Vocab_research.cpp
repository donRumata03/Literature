#include "pch.h"

#include "Vocab_research.h"

vector<pair<double, double>> make_author_vocab(author_books* author)
{
	common_word_top author_top;
	for (auto& artwork : author->artworks) author_top.merge(artwork.top);
	cout << "Got merged top!" << endl;
	
	size_t all_diff_words = author_top.different_words();
	auto max_freq = author_top.most_frequent_word().second;
	cout << "All different words: " << all_diff_words << "; Max frequency: " << max_freq << endl;

	vector<pair<double, double>> res = { /*{ 0., double(all_diff_words) }*/ };
	for(size_t cutting_number = 1; cutting_number < max_freq; cutting_number++)
	{
		author_top.cut(cutting_number);
		if (author_top.different_words() == 0) break;
		if (!res.empty() && author_top.different_words() == res.back().second) continue;
		res.emplace_back(double(cutting_number), double(author_top.different_words()));
	}
	
	return res;
}

vector<pair<double, double>> make_log_author_vocab(author_books* author, const bool log_x, const bool log_y)
{
	cout << "Got result!" << endl;
	auto not_logged = make_author_vocab(author);
	if (!log_x && !log_y) return not_logged;

	vector<pair<double, double>> answer;
	for (auto& p : not_logged)
	{
		if (log_x) p.first = log(p.first);
		if (log_y) p.second = log(p.second);

		if (p.first != p.first || p.second != p.second)
		{
			cout << "Number error!" << endl;
			continue;
		}
		answer.emplace_back(p);
	}
	return answer;
}

void reduce_line(vector<pair<double, double>>& graph)
{
	pair<double, double> first(graph.front()), last(graph.back());

	double k = (first.second - last.second) / (first.first - last.first);
	double b = first.second - first.first * k;

	for (auto& pnt : graph)
	{
		pnt.second -= pnt.first * k + b;
	}
}
