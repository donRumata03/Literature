#include "pch.h"
#include "word_frequency_research.h"


vector<pair<string, double>> get_author_useful_words(Library& lib, const common_word_top& all_top, author_books* author, const loading_requirements& req, const size_t amount, const author_info_getter_params& params)
{
	auto stat = get_author_word_stat(lib, all_top, author, req, params);
	sort(stat.begin(), stat.end(), [](auto& p1, auto& p2) -> bool {return p1.second > p2.second; });

	return Slice(stat, 0, amount);
}

vector<pair<string, double>> get_author_word_stat(Library& lib, const common_word_top& all_top, author_books* author, const loading_requirements& req, const author_info_getter_params& params)
{
	vector<pair<string, double>> res;

	unordered_set<string> all_author_words;
	for (auto& artwork : lib.get_good_artworks(author, req))
	{
		for (auto & p : artwork->top.frequencies)
		{
			all_author_words.insert(p.first);
		}
	}

	cout << "Total different words: " << all_author_words.size() << endl;
	
	res.reserve(all_author_words.size());
	for (auto& this_word : all_author_words)
	{
		res.emplace_back(this_word, get_word_info(author, this_word, lib, all_top, req, params));
	}

	return res;
}

double get_word_freq(author_books* author, const string& string_word, Library& lib, const loading_requirements& req, const bool debug)
{
	// Count freqs:
	vector<double> freqs;
	vector<double> weights;
	auto good_artworks = lib.get_good_artworks(author, req);
	for (auto& artwork : good_artworks) {
		auto this_freq = artwork->top.get_percent_word_frequency(string_word);

		if (isnan(this_freq)) this_freq = 0;

		if (this_freq != 0) {
			weights.push_back(pow(log(artwork->file_size), 3));
			freqs.push_back(this_freq);
		}
	}

	
	vector<double> log_freqs = get_logariphmated(freqs);
	double mediana = weighted_average(log_freqs, weights);
	
	double n = 0.7;
	double n_sigma = sigma(log_freqs, true, mediana) * n;

	vector<double> clear_weights;
	vector<double> clear_values;
	
	for (size_t index = 0; index < log_freqs.size(); index++)
	{
		if (abs(mediana - log_freqs[index]) <= n_sigma)
		{
			clear_values.push_back(log_freqs[index]);
			clear_weights.push_back(weights[index]);
		}
	}
	double clear_average_log = weighted_average(clear_values, clear_weights);
	double clear_average = exp(clear_average_log);


	/////////////////////////////////////////
	if (debug) {
		auto simple_clear_values = dropout_data_outliers(log_freqs);
		auto raw_distr = count_number_density(log_freqs, 0.1, 1000, 1);
		auto w_clear_distr = count_number_density(clear_values, 0.1, 1000, 1);
		auto usual_clear_distr = count_number_density(simple_clear_values, 0.1, 1000, 1);
		double raw_average = average(log_freqs);
		double usual_clear_average = average(simple_clear_values);

		cout << "Mediana: " << mediana << endl;
		cout << "Simple mediana = " << average(log_freqs) << endl;
		
		cout << "All values: " << log_freqs.size() << endl;
		cout << "Simple dropped size: " << simple_clear_values.size() << endl;
		cout << "Weighted dropped size: " << clear_values.size() << endl;

		cout << "Usual average: " << raw_average << endl;
		cout << "Clear average: " << usual_clear_average << endl;
		cout << "Weighted clear average: " << clear_average_log << endl;

		cout << "All size: " << freqs.size() << " Size with: " << good_artworks.size() << endl;
		
		add_to_plot(raw_distr, { "Raw" });
		add_to_plot(w_clear_distr, { "Weighted clear" });
		add_to_plot(usual_clear_distr, { "Usual clear" });

		show_plot();
	}
	/////////////////////////////////////////
	
	
	if (freqs.size() <= double(good_artworks.size()) / 10) return 0;

	
	return clear_average;
}


double get_word_info(author_books* author, const string& string_word, Library& lib, const common_word_top& all_top, const loading_requirements& req, const author_info_getter_params& params)
{
	double term_freq = get_word_freq(author, string_word, lib, req);
	double all_freq = all_top.get_percent_word_frequency(string_word);

	// cout << string_word << " : {" << term_freq << ", " <<  all_freq << "}" << endl;

	if (all_top.get_abs_word_frequency(string_word) < params.all_freq_edge) return 0;

	if (!params.enable_logging)
		return pow(term_freq, params.term_pow) / pow(all_freq, params.all_pow);

	return sign_safe_pow(log(all_freq), params.all_pow) / sign_safe_pow(log(term_freq), params.term_pow);
}
