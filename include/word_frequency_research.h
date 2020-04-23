#pragma once

#include "Library.h"


struct author_info_getter_params {
	size_t all_freq_edge = 10000; // if less than this amount of words: we consider the frequency = 0
	double term_pow = 1;
	double all_pow = 1.3;

	bool enable_logging = true;
};


vector < pair < string, double > > get_author_useful_words(Library& lib, const common_word_top& all_top, author_books* author, const loading_requirements& req, size_t amount, const author_info_getter_params& params = {});
vector<pair<string, double>> get_author_word_stat(Library& lib, const common_word_top& all_top, author_books* author, const loading_requirements& req, const author_info_getter_params& params = {});
double get_word_info(author_books* author, const string& string_word, Library& lib, const common_word_top& all_top, const loading_requirements& req, const author_info_getter_params& params = {});
double get_word_freq(author_books* author, const string& string_word, Library& lib, const loading_requirements& req, bool debug = false);



inline void research_word_frequency()
{
	Library lib;
	lib.load_wiki_data();

	author_books* tolstoy = lib.get_author_by_name("Толстой Лев");

	light_language lang(default_language_name);
	lib.assign_lang(&lang);
	
	// lib.make_one_author_all_tops(tolstoy);
	vector<double> freqs = lib.make_author_freqs(tolstoy, "я");

	cout << freqs << endl;
}


inline void analyze_word_freqs()
{
	vector<double> freqs = { 0.00744335, 0.0224605, 0.0283016, 0.0895253, 0.0289822, 0.0333488, 0.0310337, 0.0149778, 0.0191567, 0.0282338, 0.0250386, 0.0293131, 0.022409, 0.0284211, 0.0219352, 0.0126779, 0.0190366, 0.0242686, 0.0134934, 0.0126778, 0.0123861, 0.0169515, 0.0159202, 0.0486911, 0.0236529, 0.0657365, 0.0385009, 0.0409639, 0.0335935, 0.00662626, 0.0843668, 0.0333394, 0.101912, 0.0131329, 0.025704, 0.0348127, 0.0158956, 0.0145985, 0.0393773, 0.0131078, 0.0177624, 0.0103189, 0.0158116, 0.0913957, 0.0531937, 0.0187077, 0.0274159, 0.0150518, 0.0519773, 0.0568727, 0.0232165, 0.0132455, 0.0111386, 0.0192364, 0.0157935, 0.00986128, 0.00873463, 0.0274147, 0.0683209, 0.0699896, 0.0698569, 0.0334223, 0.00658686, 0.0103604, 0.0222118, 0.0196345, 0.0192502, 0.011707, 0.0268758, 0.0224482, 0.0226367, 0.0189107, 0.0259771, 0.0560337, 0.0310609, 0.0229287, 0.00679315, 0.0138747, 0.0114865, 0.0116311, 0.00805341, 0.0118531, 0.0216432, 0.0173681, 0.124605, 0.0137842, 0.023672, 0.0327826, 0.027481, 0.065355, 0.0256277, 0.0603332, 0.0353827, 0.024159, 0.0118919, 0.0363835, 0.0178642, 0.0853081, 0.0589781, 0.0295969, 0.0502012, 0.0459019, 0.0298397, 0.0320926, 0.0230337, 0.00679324, 0.0411219, 0.0207948, 0.0240629, 0.0135553, 0.0126789, 0.043605, 0.0280805, 0.0281257, 0.0154518, 0.0183619, 0.0226279, 0.010034, 0.0046757, 0.00762313, 0.0344373, 0.00896321, 0.0816981 };
	vector<double> log_freqs = get_logariphmated(freqs);

	vector<double> dropped_logs = dropout_data_outliers(log_freqs);

	double usual_average_log = average(log_freqs);
	double clear_average_log = average(dropped_logs);

	double usual_average = exp(usual_average_log);
	double clear_average = exp(clear_average_log);
	
	cout << "Total data: " << log_freqs.size() << endl;
	cout << "Cleared data: " << dropped_logs.size() << endl;
	
	cout << "\nUsusal average: " << usual_average << endl;
	cout << "Clear average: " << clear_average << endl;

	common_word_top all_top;
	all_top.load_from_file(R"(D:\Literature_data\temp\very_small_common_word_top.txt)");
	cout << "Common value = " << all_top.get_percent_word_frequency("я") << endl;
	
	auto density = count_number_density(log_freqs, 0.1, 5000, 1);
	auto clear_density = count_number_density(dropped_logs, 0.1, 5000, 1);

	add_to_plot(density, { "Raw density" });
	add_to_plot(clear_density, { "Density" });
	show_plot();
}

inline void test_weights()
{
	// Preparation:
	
}



inline void launch_best_word_getting(const bool test = true)
{
	vector<string> author_names_to_check = {
		"Толстой Лев",
		"Достоевский Федор",
		"Азимов Айзек"
	};

	string author_name_to_test = "Стругацкий Аркадий";
	// string author_name_to_test = "Достоевский Федор";

	loading_requirements req = {3, 100000};
	
	// Loading:
	Library lib;
	lib.ensure_metadata_is_loaded();
	lib.ensure_wiki_data_loaded();

	auto author_ptr = lib.get_author_by_name(author_name_to_test);

	// Check good works:
	cout << "Total artworks: " << author_ptr->artworks.size() << endl;
	cout << "Good artworks: " << lib.get_good_artworks(author_ptr).size() << endl;

	common_word_top all_top;
	all_top.load_from_file(R"(D:\Literature_data\temp\common_word_top.txt)");

	lib.load_language();

	lib.make_one_author_all_tops(author_ptr, req);

	cout << "Prepared!" << endl;


	if (test) {
		vector<string> dostoyevsky_words_to_test = {
			"я",
			"зловещий",
			"обманщик",
			"преступление",
			"дьявол",
			"страх",
			"зло",
			"ненависть"
		};

		vector<string> tolstoy_words_to_test = {
			"князь",
			"сказать",
			"герой",
			"толпа"
		};


		for (auto& word_to_test : dostoyevsky_words_to_test) {
			cout << "Word " << word_to_test << ": " << endl;

			auto all_freq = all_top.get_percent_word_frequency(word_to_test);
			cout << "Global percent: " << all_freq << endl;
			auto this_freq = get_word_freq(author_ptr, word_to_test, lib, req);
			cout << "This author percent: " << this_freq << endl;
			cout << "Rating: " << this_freq / pow(all_freq, 1.3) << endl;
			cout << "______________________________________________________________\n";
		}

		auto make_test = [&](const auto& params)
		{
			vector<string> words = {};
			for (auto& p : get_author_useful_words(lib, all_top, author_ptr, req, 100, params))
			{
				string this_word = p.first;
				words.push_back(this_word);

				double this_all_freq = all_top.get_percent_word_frequency(this_word);
				double this_word_this_author_freq = get_word_freq(author_ptr, this_word, lib, req);

				cout << "Word: \t" << this_word << ".\t\t Rating: " << p.second << ". \t\tAll freq log: " << log(this_all_freq) << ".\t\t This author freq:" << log(this_word_this_author_freq) << endl;
			}

			for (auto& w : words)
			{
				cout << w << endl;
			}
		};

		auto usual_params_strong = author_info_getter_params{ 50000, 2, 1.3, false };
		auto usual_params = author_info_getter_params{ 10000, 1, 1.3, false };


		auto log_params = author_info_getter_params{ 10000, 1, 1.3, true };
		auto log_specific = author_info_getter_params{ 7500, 2, 1.5, true };

		auto log_test_1 = author_info_getter_params{ 7500, 2, 2, true };
		auto log_test_2 = author_info_getter_params{ 7500, 10, 10, true };
		
		

		cout << "_________________________________________\n";
		make_test(usual_params_strong);
		cout << "_________________________________________\n";
		make_test(usual_params);
		cout << "_________________________________________\n";
		make_test(log_params);
		cout << "_________________________________________\n";

		make_test(log_specific);
		cout << "_________________________________________\n";
	}
	else
	{
		
	}
}


