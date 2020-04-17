#include "pch.h"
#include "word_top_usage.h"
#include "model_error_function.h"
#include "tests.h"


double estimate_word_top_coefficients(const vector<double>& pows)
{
	assert(pows.size() == 1);
	double common_pow = pows[0];
	double diff_pow = pows[1];

	auto est_lambda = [](pair<artwork_file*, artwork_file*> p) -> double
	{
		// return word_top::meaning_dist(p.first->top, p.second->top, unifying_word_top);
		return {};
	};
	
	return {}; // literature_fitness_function();
}

void find_word_top_dist_coefficients()
{
	// Load all literature word top:
	light_language lang(russian_filename);
	common_word_top unifing_word_top(&lang);
	
	// Perform GA:
	GA_params params;
	params.epoch_num = 10;
	params.population_size = 100;

	params.hazing_percent = 0.7;

	params.allow_multithreading = true;

	vector<pair<double, double>> point_ranges = { {0, 5}, {0, 3} }; // common_pow, diff_pow
	
	auto[best_fitness, best_genome] = GA::ga_optimize(estimate_word_top_coefficients, point_ranges, params, 
		[](const double percent, const double best_fitness_now, const vector<double>& best_chromosome)
	{
			cout << "GA percent: " << percent << " ; best fitness: " << best_fitness_now << " ; best common pow: " << best_chromosome[0] << " ; best diff pow: " << best_chromosome[1] << endl;
	} );

	cout << "__________________________________________________________\n";
	cout << "Solution found! Fitness: " << best_fitness <<  " Optimal Coefficients: " << best_genome << endl;
}

void cook_all_author_tops(author_books& author, light_language& lang)
{
	for (auto& artwork : author.artworks)
	{
		if(artwork.top.total_words == 0)
		{
			if (artwork.parsed_content.empty()) artwork.top;
			artwork.top.add_words_with_info(artwork.raw_content);
		}
	}
}

vector<double> generate_tolstoy_ya_freqs()
{
	Library lib;
	light_language lang(default_language_name);
	lib.assign_lang(&lang);
	
	auto tolstoy_ptr = lib.get_author_by_name("Толстой Лев");
	/*

	for (auto& artwork : tolstoy_ptr->artworks)
	{
		artwork.top
	}
	*/
	lib.make_all_author_tops(tolstoy_ptr);

	return {}; // TODO!
}

