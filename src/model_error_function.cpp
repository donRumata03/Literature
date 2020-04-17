#include "pch.h"
#include "model_error_function.h"


double literature_fitness_function(const std::function<double(pair<artwork_file*, artwork_file*>)>& func, const size_t amount)
{
	// Assume, that author data is already loaded!
	vector<double> one_author_dists, different_author_dists;

	// Generate artworks to analyze:
	random_device rd;
	mt19937 gen(rd());
	
	vector<pair<artwork_file*, artwork_file*>> generated_one_artwork_pairs;
	vector<pair<artwork_file*, artwork_file*>> generated_different_artwork_pairs;

	for (size_t author_index = 0; author_index < amount; author_index++)
	{
		generated_one_artwork_pairs.push_back({});
		generated_different_artwork_pairs.push_back({});
	}

	// Count author dists using given function:
	for(auto& p : generated_one_artwork_pairs)
	{
		one_author_dists.push_back(func(p));
	}

	for (auto& p : generated_different_artwork_pairs)
	{
		different_author_dists.push_back(func(p));
	}

	// TODO! Make weights proportional to length of text!!!!!!!!!
	
	// Make logs:
	auto one_author_logs = get_logariphmated(one_author_dists);
	auto different_author_logs = get_logariphmated(different_author_dists);

	// Clear dropouts:
	auto one_author_cleared = dropout_data_outliers(one_author_logs);
	auto different_authors_cleared = dropout_data_outliers(different_author_logs);

	// Count averages:
	double log_one_average = average(one_author_cleared);
	double log_different_average = average(one_author_cleared);

	// Exponentiate
	double one_average = exp(log_one_average);
	double different_average = exp(log_different_average);
	
	return different_average / one_average;
}
