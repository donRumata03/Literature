#include "pch.h"
#include "Library.h"


/*
 *							Loading stuff:
 */


void Library::load_metadata_from_json(const string& filename)
{
	auto j = Json::Load(readFile(filename));

	all_authors = j.size();
	all_authors_loaded = true;

	
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
			
			if (artwork_json.hasKey("file_path")) {
				this_artwork.file_path = artwork_json["file_path"].ToString();
				this_artwork.file_size = artwork_json["file_size"].ToInt();
			}
		}
	}
}

bool Library::ensure_metadata_is_loaded(const string& filename)
{
	bool temp_val = all_authors_loaded;
	if (!all_authors_loaded) load_metadata_from_json(filename);
	return temp_val;
}

void Library::store_metadata_to_json(const string& filename)
{
	// TODO!
}

void Library::scrape_metadata_from_folder(const string& base_path)
{
	all_authors_loaded = true;
	// tOdO
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
		author_obj->descriptor = raw_wiki_data[a_index];
		good_author_names.push_back(this_name);
		wiki_data[author_obj->name] = author_obj;
	}
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

void Library::load_good_texts(const bool include_parsing)
{
	ensure_wiki_data_loaded();

	for (auto& author_p : wiki_data)
	{
		for (auto& artwork : author_p.second->artworks)
		{
			artwork.raw_content = readFile(artwork.file_path);
			auto& unknowns = artwork.unknown_words;
			if (include_parsing) artwork.parsed_content = lemmatize_words(artwork.raw_content, *lang, unknowns, true);
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
	load_good_texts(include_parsing);
	return state0 == good_text_loading_state;
}

void Library::ensure_one_author_loaded(author_books* author)
{
	ensure_metadata_is_loaded();
	for (auto& artwork : author->artworks)
	{
		if (artwork.raw_content.empty()) artwork.raw_content = readFile(artwork.file_path);
	}
}


/*
 *						Generators:
 */



author_books* Library::generate_author(const bool demand_good, const size_t min_artworks, const size_t min_big_artworks, const size_t min_n_artworks_byte_size)
{
	size_t idx;
	if (demand_good)
	{
		bool ok = false;
		author_books* possible_res = nullptr;
		while (!ok) {
			idx = generator() % good_author_names.size(); //  : randint(0, good_author_names.size());
			string name = good_author_names[idx];
			possible_res = wiki_data[name];
			ok = author_satisfies_requirements(possible_res, min_artworks, min_big_artworks, min_n_artworks_byte_size);
		}
		return possible_res;
	}
	else
	{
		bool ok = false;
		author_books* possible_res = nullptr;
		while (!ok) {
			idx = generator() % author_names.size(); //  : randint(0, author_names.size());
			string name = author_names[idx];
			possible_res = &data[name];
			ok = author_satisfies_requirements(possible_res, min_artworks, min_big_artworks, min_n_artworks_byte_size);
		}
		return possible_res;
	}
}




bool Library::author_satisfies_requirements(author_books* author, const size_t min_artworks, const size_t big_artworks_requirement, const size_t min_n_artworks_byte_size)
{
	if (author->artworks.size() < min_artworks) return false;

	size_t artwork_count = 0;
	for (auto& artwork : author->artworks) if (artwork.file_size >= min_n_artworks_byte_size) artwork_count++;
	return artwork_count >= big_artworks_requirement;
}


artwork_file* Library::generate_author_artwork(author_books* author)
{
	size_t idx = randint(0, author->artworks.size(), generator);
	return &author->artworks[idx];
}

pair<artwork_file*, artwork_file*> Library::generate_one_author_different_artworks(const bool demand_good)
{
	auto author = generate_author(demand_good);
	artwork_file* first = generate_author_artwork(author);
	artwork_file* second = first;
	while (first == second) second = generate_author_artwork(author);

	return { first, second };
}

pair<author_books*, author_books*> Library::generate_different_authors(const bool demand_good)
{
	author_books* first = generate_author(demand_good);
	author_books* second = first;
	while (first == second) second = generate_author(demand_good);

	return {first, second};
}

pair<artwork_file*, artwork_file*> Library::generate_different_authors_artworks(const bool demand_good)
{
	auto different_authors = generate_different_authors(demand_good);
	return { generate_author_artwork(different_authors.first), generate_author_artwork(different_authors.second) };
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

// Top making:
void Library::make_all_author_tops(author_books* author)
{
	// ensure_metadata_is_loaded();
	ensure_one_author_loaded(author);
	
	if (!lang) {
		bool found_lang = false;
		for (auto& artwork : author->artworks) if (artwork.top.lang) { found_lang = true; lang = artwork.top.lang; break; }
		if (!found_lang) throw runtime_error("I don`t have language! And they - also...");
	}

	
	for (auto& artwork : author->artworks)
	{
		auto& top = artwork.top;
		if (!top.lang) top.lang = lang;

		// const string& data;
		
		if (artwork.parsed_content.empty())
		{
			auto& for_unknowns = artwork.unknown_words;
			artwork.parsed_content = lemmatize_words(artwork.raw_content, *lang, for_unknowns, true);
		}
		top.add_words_with_info(artwork.parsed_content, artwork.unknown_words);
	}

}


