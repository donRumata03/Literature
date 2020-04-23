#include "pch.h"
#include "Library.h"

#include <mutex>


/*
 *							Loading stuff:
 */


void Library::load_metadata_from_json(const string& filename)
{
	auto j = Json::Load(readFile(filename));

	all_authors = j.size();
	all_authors_loaded = true;

	size_t has_file_counter = 0;
	size_t total_artwork_counter = 0;
	
	for (size_t author_index = 0; author_index < j.size(); author_index++) {
		Json author_json = j[author_index];

		string name = author_json["name"].ToString();
		author_names.push_back(name);
		
		author_books& author = data[name];

		author.name = name;
		author.path = author_json["path"].ToString();

		for (size_t artwork_index = 0; artwork_index < author_json["artworks"].size(); artwork_index++)
		{
			Json artwork_json = author_json["artworks"][artwork_index];
			auto& this_artwork = author.artworks.emplace_back();

			this_artwork.author_name = author.name;
			this_artwork.author_path = author.path;
			this_artwork.folder_path = artwork_json["folder_path"].ToString();

			this_artwork.name = artwork_json["name"].ToString();
			
			if (artwork_json.hasKey("file_name")) {
				this_artwork.file_path = artwork_json["file_name"].ToString();
				this_artwork.file_size = artwork_json["size"].ToInt();
				has_file_counter++;
			}

			if (artwork_json.hasKey("is_poem")) this_artwork.is_poem = artwork_json["is_poem"].ToBool();
			if (artwork_json.hasKey("is_english")) this_artwork.is_english = artwork_json["is_english"].ToBool();

			if (artwork_json.hasKey("top_path")) this_artwork.top_path = artwork_json["top_path"].ToString();
			if (artwork_json.hasKey("top_meta_path")) this_artwork.top_meta_path = artwork_json["top_meta_path"].ToString();
			if (artwork_json.hasKey("parsed_path")) this_artwork.parsed_path = artwork_json["parsed_path"].ToString();
			
			total_artwork_counter++;
		}
	}

	cout << "Has file percent: " << 100. * has_file_counter / total_artwork_counter << " %" << endl;
}

bool Library::ensure_metadata_is_loaded(const string& filename)
{
	bool temp_val = all_authors_loaded;
	if (!all_authors_loaded) load_metadata_from_json(filename);
	return temp_val;
}


inline void store_book_metadata_to_json(unordered_map<string, author_books>& data,
	const string& filename = R"(D:\Projects\Literature_parser\system\new_best_book_data.json)")
{
	auto j = sjson::Array();

	for (auto& author_p : data)
	{
		auto& author = author_p.second;
		Json author_json;

		author_json["name"] = author.name;
		author_json["path"] = author.path;

		author_json["artworks"] = sjson::Array();

		for (auto& artwork : author.artworks)
		{
			Json artwork_json;
			artwork_json["name"] = artwork.name;
			artwork_json["folder_path"] = artwork.folder_path;
			artwork_json["file_path"] = artwork.file_path;
			artwork_json["file_size"] = artwork.file_size;

			artwork_json["author_name"] = artwork.author_name;
			artwork_json["author_path"] = artwork.author_path;

			author_json["artworks"].append(artwork_json);
		}

		j.append(author_json);
	}


	ofstream out_file(filename);
	out_file << j.dump();
	out_file.close();
}


void Library::store_metadata_to_json(const string& filename)
{
	auto j = sjson::Array();

	for (auto& author_p : data)
	{
		auto& author = author_p.second;
		Json author_json;

		author_json["name"] = author.name;
		author_json["path"] = author.path;

		author_json["artworks"] = sjson::Array();

		for (auto& artwork : author.artworks)
		{
			Json artwork_json;
			artwork_json["name"] = artwork.name;
			artwork_json["folder_path"] = artwork.folder_path;
			artwork_json["file_path"] = artwork.file_path;
			artwork_json["file_size"] = artwork.file_size;

			artwork_json["author_name"] = artwork.author_name;
			artwork_json["author_path"] = artwork.author_path;
			
			artwork_json["is_english"] = artwork.is_english;
			artwork_json["is_poem"] = artwork.is_poem;

			if (!artwork.parsed_path.empty()) {
				artwork_json["parsed_path"] = artwork.parsed_path;
			}	

			if (!artwork.top_path.empty()) {
				artwork_json["top_path"] = artwork.top_path;
			}

			if (!artwork.top_meta_path.empty()) {
				artwork_json["top_meta_path"] = artwork.top_meta_path;
			}
			
			author_json["artworks"].append(artwork_json);
		}

		j.append(author_json);
	}


	ofstream out_file(filename);
	out_file << j;
	out_file.close();
}

void Library::scrape_metadata_from_folder(const string& base_path)
{
	all_authors_loaded = true;
	// tOdO or not ToDo
}



void Library::load_wiki_data(const string& filename)
{
	ensure_metadata_is_loaded();
	wiki_data_loaded = true;
	
	auto raw_wiki_data = Json::Load(readFile(filename));
	
	
	for (size_t a_index = 0; a_index < raw_wiki_data.size(); a_index++)
	{
		// Get name
		string this_name = raw_wiki_data[a_index]["name"].ToString();
		all_wiki_data[this_name] = raw_wiki_data[a_index];
		// Find object:
		auto author_obj = get_author_by_name(this_name);
		if (!author_obj) continue;
		// Add json to struct:
		author_obj->descriptor = raw_wiki_data[a_index]["wiki_data"];
		good_author_names.push_back(this_name);
		wiki_data[author_obj->name] = author_obj;
		good_authors++;
	}
	cout << good_authors << " / " << raw_wiki_data.size() << " ( " <<  100. * good_authors / raw_wiki_data.size() << "% ) good authors are loaded!" << endl;
}
	

bool Library::ensure_wiki_data_loaded(const string& filename)
{
	bool temp_val = wiki_data_loaded;
	if (!wiki_data_loaded) load_wiki_data(filename);
	return temp_val;
}



void Library::load_all_texts(const bool include_parsing)
{
	ensure_metadata_is_loaded();

	if (all_text_loading_state == loading_state::loaded_and_parsed) return;

	for (auto& author_p : data)
	{
		for (auto& artwork : author_p.second.artworks)
		{
			if (good_text_loading_state == loading_state::not_loaded) artwork.raw_content = readFile(artwork.file_path);
			auto& unknowns = artwork.unknown_words;
			if (include_parsing && all_text_loading_state == loading_state::loaded_not_parsed) artwork.parsed_content = lemmatize_words(artwork.raw_content, *lang, unknowns, true);
		}
	}

	all_text_loading_state = include_parsing ? loading_state::loaded_and_parsed : loading_state::loaded_not_parsed;
	good_text_loading_state = all_text_loading_state;
}

void Library::load_good_texts(const bool include_parsing, const loading_requirements& req)
{
	ensure_wiki_data_loaded();

	for (auto& author_p : wiki_data)
	{
		if (!author_satisfies_requirements(author_p.second, req)) {
			cout << "Author " << author_p.second->name << " is bad" << endl;
			continue;
		}
		cout << "Author " << author_p.second->name << " is good" << endl;

			for (auto& artwork : author_p.second->artworks)
		{
			if (artwork.file_size == 0) continue;
			if(artwork.raw_content.empty()) artwork.raw_content = readFile(artwork.file_path);
			auto& unknowns = artwork.unknown_words;
			if (include_parsing && artwork.parsed_content.empty()) artwork.parsed_content = lemmatize_words(artwork.raw_content, *lang, unknowns, true);
		}
	}
	
	good_text_loading_state = include_parsing ? loading_state::loaded_and_parsed : loading_state::loaded_not_parsed;
}

bool Library::ensure_all_texts_loaded(const bool include_parsing)
{
	auto state0 = all_text_loading_state;
	load_all_texts(include_parsing);
	return state0 == all_text_loading_state;
}

bool Library::ensure_good_texts_loaded(const bool include_parsing)
{
	auto state0 = good_text_loading_state;
	load_good_texts(include_parsing, {});
	return state0 == good_text_loading_state;
}

void Library::ensure_one_author_loaded(author_books* author) // TODO! FILTER ENGLISH AND POEMS!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
{
	ensure_metadata_is_loaded();
	for (auto& artwork : author->artworks)
	{
		if (artwork.raw_content.empty()) if (!artwork.file_path.empty()) artwork.raw_content = readFile(artwork.file_path);
	}
}

void Library::load_tops(const loading_requirements& req)
{
	for (auto author : get_good_authors(req))
	{
		for (auto artwork : get_good_artworks(author, req))
		{
			if (!artwork->top_path.empty()) {
				artwork->top.load_from_file(artwork->top_path);
			}
			if (!artwork->top_meta_path.empty()) {
				artwork->top.load_metadata_from_file(artwork->top_meta_path);
			}
		}
	}
}

void Library::delete_author_big_raw_content(author_books* author)
{
	for (auto& art : author->artworks)
	{
		art.raw_content.clear();
		art.parsed_content.clear();
		art.unknown_words.clear();
	}
}

void Library::delete_all_big_raw_content()
{
	for (auto& author_p : data)
	{
		delete_author_big_raw_content(&author_p.second);
	}
}


/*
 *						Generators:
 */



author_books* Library::generate_author(const bool demand_good, const loading_requirements& req)
{
	bool ok = false;
	author_books* possible_res = nullptr;
	while (!ok) {
		size_t sz = demand_good ? good_author_names.size() : author_names.size();
		
		size_t idx = generator() % sz;
		string name = demand_good ? good_author_names[idx] : author_names[idx];
		possible_res = demand_good ? wiki_data[name] : &data[name];
		ok = author_satisfies_requirements(possible_res, req);
	}
	return possible_res;
	/*
	else
	{
		bool ok = false;
		author_books* possible_res = nullptr;
		while (!ok) {
			idx = generator() % author_names.size(); //  : randint(0, author_names.size());
			string name = author_names[idx];
			possible_res = &data[name];
			ok = author_satisfies_requirements(possible_res, req);
		}
		return possible_res;
	}
	*/
}




bool Library::author_satisfies_requirements(author_books* author, const loading_requirements& req)
{
	size_t big_artworks_requirement = req.big_good_artworks_requirement;
	size_t min_n_artworks_byte_size = req.min_n_artworks_byte_size;

	size_t big_artwork_count = 0;
	for (auto& artwork : author->artworks) {
		if (artwork.is_english || artwork.is_poem) continue;
		if (artwork.file_size >= min_n_artworks_byte_size) big_artwork_count++;
	}
	
	return big_artwork_count >= big_artworks_requirement;
}

bool Library::artwork_satisfies_requirements(artwork_file* artwork, const loading_requirements& req)
{
	if (artwork->is_english || artwork->is_poem) return false;
	return artwork->file_size >= req.min_n_artworks_byte_size;
}


artwork_file* Library::generate_author_artwork(author_books* author, const loading_requirements& req)
{
	artwork_file* artwork;
	do {
		size_t idx = randint(0, author->artworks.size(), generator);
		artwork = &author->artworks[idx];
	} while (!artwork_satisfies_requirements(artwork, req));
	// TODO! Make proportiuonal to the size
	return artwork;
}

pair<artwork_file*, artwork_file*> Library::generate_one_author_different_artworks(const bool demand_good, const loading_requirements& req)
{
	auto author = generate_author(demand_good, req);
	artwork_file* first = generate_author_artwork(author, req);
	artwork_file* second = first;
	while (first == second) second = generate_author_artwork(author, req);

	return { first, second };
}

pair<author_books*, author_books*> Library::generate_different_authors(const bool demand_good, const loading_requirements& req)
{
	author_books* first = generate_author(demand_good, req);
	author_books* second = first;
	while (first == second) second = generate_author(demand_good, req);

	return {first, second};
}

pair<artwork_file*, artwork_file*> Library::generate_different_authors_artworks(const bool demand_good, const loading_requirements& req)
{
	auto different_authors = generate_different_authors(demand_good, req);
	return { generate_author_artwork(different_authors.first, req), generate_author_artwork(different_authors.second, req) };
}

vector<string> Library::generate_all_artwork_paths()
{
	ensure_metadata_is_loaded();
	
	vector<string> res;
	for (auto& author_p : data) {
		auto& author = author_p.second;
		for (auto& artwork : author.artworks)
		{
			res.push_back(artwork.file_path);
		}
	}
	return res;
}


/*
 *			Making
 *			 tops
*/


void Library::make_one_author_all_tops(author_books* author, const loading_requirements& req, const size_t cutting_number)
{
	// ensure_metadata_is_loaded();
	ensure_one_author_loaded(author);
	cout << "loaded texts!" << endl;
	if (!lang) {
		bool found_lang = false;
		for (auto& artwork : author->artworks) if (artwork.top.lang) { found_lang = true; lang = artwork.top.lang; break; }
		if (!found_lang) throw runtime_error("I don`t have language! And they - also...");
	}

	
	for (auto& artwork : get_good_artworks(author, req))
	{
		auto& top = artwork->top;

		if (top.total_words != 0) continue;
		
		if (!top.lang) top.lang = lang;

		if (artwork->parsed_content.empty())
		{
			auto& for_unknowns = artwork->unknown_words;
			artwork->parsed_content = lemmatize_words(artwork->raw_content, *lang, for_unknowns, true);
			cout << artwork->name << endl;
		}
		if (top.total_words == 0) top.add_words_with_info(artwork->parsed_content, artwork->unknown_words);
		top.cut(cutting_number);
	}
}

vector<double> Library::make_author_freqs(author_books* author, const string& word)
{
	make_one_author_all_tops(author);
	
	vector<double> res;
	for (auto& artwork : get_good_artworks(author))
	{
		cout << artwork->top.total_words << endl;
		if (artwork->top.total_words == 0) continue;
		res.push_back(artwork->top.get_percent_word_frequency(word));
	}

	return res; 
}

void Library::all_cycle_make_author_tops(author_books* author, const loading_requirements& req)
{
	make_one_author_all_tops(author, req);
	delete_author_big_raw_content(author);
}

void Library::multithread_load_tops(const bool demand_good, const loading_requirements& req, const size_t thread_number)
{
	if (demand_good)
		ensure_wiki_data_loaded();
	else ensure_metadata_is_loaded();

	vector<author_books*> authors_to_load;

	if(!demand_good)
		for (auto& author_p : data)
		{
			if (author_satisfies_requirements(&author_p.second, req)) authors_to_load.push_back(&author_p.second);
		}

	else
		for (auto& author_p : wiki_data)
		{
			if (author_satisfies_requirements(author_p.second, req)) authors_to_load.push_back(author_p.second);
		}
	

	size_t task_number = authors_to_load.size();

	cout << "All satisfying authors: " << task_number << " of " << (demand_good ? (good_authors) : (all_authors)) << endl;
	for (auto& author : authors_to_load) cout << author->name << endl;

	multithread_load_tops(authors_to_load, req, thread_number);
}

void Library::multithread_load_tops(const vector<author_books*>& authors, const loading_requirements& req, const size_t thread_number)
{

	size_t task_number = authors.size();
	auto ranges = distribute_task_ranges(task_number, thread_number);
	mutex print_mutex;

	vector<thread> threads;
	threads.reserve(thread_number);
	for (auto& thread_range : ranges) threads.emplace_back([&](const vector<author_books*>& all_tasks, const pair<size_t, size_t>& range)
		{
			{
				// lock_guard<mutex> lg(print_mutex);
				// cout << range << endl;
			}
			for (auto author : Slice(all_tasks, range.first, range.second)) all_cycle_make_author_tops(author, req);
		}, ref(authors), ref(thread_range));


	for (auto& the_thread : threads) the_thread.join();
}


vector<author_books*> Library::get_good_authors(const loading_requirements& req)
{
	vector<author_books*> res;
	for (auto& author_name : good_author_names) {
		auto author = wiki_data[author_name];
		if (author_satisfies_requirements(author, req)) res.push_back(author);
	}
	return res;
}

vector<artwork_file*> Library::get_good_artworks(author_books* author, const loading_requirements& req)
{
	vector<artwork_file*> res;
	for (auto& artwork : author->artworks)
	{
		if (artwork_satisfies_requirements(&artwork, req))
		{
			res.push_back(&artwork);
		}
	}
	return res;
}

vector<artwork_file*> Library::get_all_good_artworks(const loading_requirements& req)
{
	vector<artwork_file*> res;
	for (auto& good_author : get_good_authors(req)) {
		for (auto& good_artwork : get_good_artworks(good_author, req)) {
			res.push_back(good_artwork);
		}
	}
	return res;
}

void Library::process_good_authors_engpoemness(const loading_requirements& req)
{
	ensure_metadata_is_loaded();
	ensure_wiki_data_loaded();
	size_t counter = 0;
	auto good_artworks = get_all_good_artworks(req);
	for (auto& artwork : good_artworks)
	{
		bool was_loaded = !artwork->raw_content.empty();
		if (artwork->raw_content.empty() && artwork->file_size != 0 && !empty(artwork->file_path)) { artwork->raw_content = readFile(artwork->file_path); }
		artwork->is_english = is_english(artwork->raw_content, filtering_edges::true_all);
		artwork->is_poem = is_poem(artwork->raw_content, filtering_edges::true_all);
		if (!was_loaded) artwork->raw_content.clear();
		cout << artwork->author_name << " ; " << percent_plotter(counter++, good_artworks.size()) << endl;
	}
}
