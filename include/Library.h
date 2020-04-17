#pragma once
#include "pch.h"

#include <artwork_structs.h>


inline string default_language_name = R"(D:\Projects\Language\res\dictionary.txt)";
inline string default_metadata_json_name = R"(D:\Projects\Literature_parser\system\best_book_data.json)";
inline string default_literature_base_path = R"(D:\Literature_data\All_books)";
inline string default_wiki_data_name = R"(D:\Projects\Literature_downloading\cp1251_res\wiki_data.json)";



struct Library
{
	enum class loading_state
	{
		not_loaded,
		loaded_not_parsed,
		loaded_and_parsed
	};
	
	unordered_map<string, author_books> data;
	vector<string> author_names;
	size_t all_authors = 0;
	bool all_authors_loaded = false;
	loading_state all_text_loading_state = loading_state::not_loaded;

	unordered_map < string, Json > all_wiki_data;		// All the data from Wikipedia
	unordered_map < string, author_books* > wiki_data; // Only loaded authors
	vector<string> good_author_names;				  // Only loaded names
	size_t good_authors = 0;						 // Number of loaded authors in wikipedia
	size_t all_good_authors = 0;					// Nuumber all authors in Wikipedia
	bool wiki_data_loaded = false;
	loading_state good_text_loading_state = loading_state::not_loaded;
	
	light_language* lang = nullptr;

private:
	mt19937 generator;
	bool author_satisfies_requirements(author_books* author, size_t min_artworks, size_t big_artworks_requirement = 2, size_t min_n_artworks_byte_size = 100000); // 100 kBytes by default
public:
	// Some OOP stuff:
	
	Library() { random_device rd; generator = mt19937(rd()); }
	explicit Library(light_language& _lang) : Library() { lang = &_lang; }

	Library(const Library& lib) = delete;
	Library(Library&& lib) = delete;

	Library& operator =(const Library& other) = delete;
	Library& operator =(Library&& other) = delete;
	
	~Library() { delete lang; }


	
	/*
	 *			Loading
	 *			 data
	 *			   !
	*/
	
	void load_language(const string& filename = default_language_name) { lang = new light_language(filename); }
	void assign_lang(light_language* _lang) { lang = _lang; }
	
	void load_metadata_from_json(const string& filename = default_metadata_json_name);
	bool ensure_metadata_is_loaded(const string& filename = default_metadata_json_name);
	void store_metadata_to_json(const string& filename = default_metadata_json_name);

	void scrape_metadata_from_folder(const string& base_path = default_literature_base_path);

	[[nodiscard]] bool author_is_loaded(const string& name) const { return data.find(name) != data.end(); }
	[[nodiscard]] bool author_is_good(const string& name) const { return in(wiki_data, name); }

	author_books* get_author_by_name(const string& name) { return (author_is_loaded(name)) ? (&data.find(name)->second) : (nullptr); }
	
	void load_wiki_data(const string& filename = default_wiki_data_name);
	bool ensure_wiki_data_loaded(const string& filename = default_wiki_data_name);

	void load_all_texts(bool include_parsing);
	void load_good_texts(bool include_parsing);

	bool ensure_all_texts_loaded(bool include_parsing);
	bool ensure_good_texts_loaded(bool include_parsing);


	void ensure_one_author_loaded(author_books* author);
	/*
	 *			Generating 
	 *			  authors
	 */

	author_books* generate_author(bool demand_good = true, 
		size_t min_artworks = 3, size_t min_big_artworks = 2, size_t min_n_artworks_byte_size = 100000); // TODO! Make probability proportional to the number of artworks

	pair<author_books*, author_books*> generate_different_authors(bool demand_good = true);

	artwork_file* generate_author_artwork(author_books* author);
	artwork_file* generate_author_artwork(const string& author) { return get_author_by_name(author) ? generate_author_artwork(get_author_by_name(author)) : nullptr ; };

	pair<artwork_file*, artwork_file*> generate_one_author_different_artworks(bool demand_good = true);
	pair< artwork_file*, artwork_file*> generate_different_authors_artworks(bool demand_good = true);

	vector<string> generate_all_artwork_paths();
	
	// Top making:
	void make_all_author_tops(author_books* author);
};


