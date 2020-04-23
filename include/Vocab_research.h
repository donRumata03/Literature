#pragma once
#define throw

#include "Library.h"

vector < pair < double, double > > make_author_vocab(author_books* author);
vector < pair < double, double > > make_log_author_vocab(author_books* author, bool log_x = true, bool log_y = true);
void reduce_line(vector<pair<double, double>>& graph);

using pvec = vector < pair < double, double > >;

inline void launch_processing_author_vocab()
{
	vector<string> author_names = {
		"Толстой Лев",
		"Достоевский Федор",
		"Азимов Айзек",
		"Стругацкий Аркадий",
		"Тургенев Иван",
		"Карамзин Николай",
		"Гоголь Николай",
		"Аббасзаде Гусейн"
	};

	
	load_library();

	vector<author_books*> authors;
	for (auto& author_name : author_names) {
		auto tolstoy = lib.get_author_by_name(author_name);
		assert(tolstoy);
		authors.push_back(tolstoy);
	}
	for (auto& tolstoy : authors)
		lib.make_one_author_all_tops(tolstoy, {}, 0);

	loading_requirements req = {3, 100000};
	
	cout << "Loading tops..." << endl;
	// lib.multithread_load_tops(authors);
	cout << "Loaded tops!" << endl;

	for (auto& author_name : author_names) {
		auto tolstoy = lib.get_author_by_name(author_name);	
		auto vocab = make_log_author_vocab(tolstoy);
		reduce_line(vocab);
		exponentate_y(vocab);
		
		pms points;
		for (auto & p : vocab) points.push_back({p.first, p.second});
		pms new_points = smooth(points, 0.05, 1000, 1);
		pvec res_vocab;
		for (auto& p : new_points) res_vocab.push_back({ p.x, p.y });


		cout << author_name << " : " << count_graph_area(res_vocab) << endl;
		add_to_plot(res_vocab, { author_name });
	}

	show_plot();
}
