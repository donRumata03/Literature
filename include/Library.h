#pragma once
#include "pch.h"

#include <artwork_structs.h>
#include "suspicious_file_detector.h"

inline string default_language_name = R"(D:\Projects\Language\res\dictionary.txt)";
inline string default_metadata_json_name = R"(D:\Projects\Literature_parser\system\best_book_data.json)";
inline string default_literature_base_path = R"(D:\Literature_data\All_books)";
inline string default_wiki_data_name = R"(D:\Projects\Literature_downloading\cp1251_res\wiki_data.json)";


inline size_t default_min_artwork_byte_size = 100000; // 100 kBytes by default
inline size_t default_min_good_author_artwork_number = 10;


struct loading_requirements {
	size_t big_good_artworks_requirement = default_min_good_author_artwork_number; // Good <==> not english, not poem, big
	size_t min_n_artworks_byte_size = default_min_artwork_byte_size;
};



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
	bool lang_was_loaded_by_me = false;

private:
	mt19937 generator;
public:
	// Some OOP stuff:
	
	Library() { random_device rd; generator = mt19937(rd()); }
	explicit Library(light_language& _lang) : Library() { if (lang_was_loaded_by_me) delete lang; lang_was_loaded_by_me = false; lang = &_lang; }

	Library(const Library& lib) = delete;
	Library(Library&& lib) = delete;

	Library& operator =(const Library& other) = delete;
	Library& operator =(Library&& other) = delete;
	
	~Library() { if (lang_was_loaded_by_me) delete lang; }


	
	/*
	 *			Loading
	 *			 data
	 *			   !
	*/
	
	void load_language(const string& filename = default_language_name) { lang = new light_language(filename); lang_was_loaded_by_me = true; }
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
	void load_good_texts(bool include_parsing, const loading_requirements& req);

	bool ensure_all_texts_loaded(bool include_parsing);
	bool ensure_good_texts_loaded(bool include_parsing);

	void ensure_one_author_loaded(author_books* author);

	void load_tops(const loading_requirements& req = {0, 0});
	void init();
	
	// Deletions:
	void delete_author_big_raw_content(author_books* author);
	void delete_all_big_raw_content();

	
	/*
	 *			Generating 
	 *			  authors
	 */


	author_books* generate_author(bool demand_good = true, const loading_requirements& req = {}); // TODO! Make probability proportional to the length = number * size of artworks

	pair<author_books*, author_books*> generate_different_authors(bool demand_good = true, const loading_requirements& req = {});

	artwork_file* generate_author_artwork(author_books* author, const loading_requirements& req);
	artwork_file* generate_author_artwork(const string& author, const loading_requirements& req) { return get_author_by_name(author) ? generate_author_artwork(get_author_by_name(author), req) : nullptr ; };

	pair<artwork_file*, artwork_file*> generate_one_author_different_artworks(bool demand_good = true, const loading_requirements& req = {});
	pair< artwork_file*, artwork_file*> generate_different_authors_artworks(bool demand_good = true, const loading_requirements& req = {});

	vector<string> generate_all_artwork_paths();
	
	/*
	 *			Making
	 *			 tops
	*/
	
	void make_one_author_all_tops(author_books* author, const loading_requirements& req = {}, size_t cutting_number = 3);
	vector<double> make_author_freqs(author_books* author, const string& word);

	void all_cycle_make_author_tops(author_books* author, const loading_requirements& req); 

	void multithread_load_tops(bool demand_good = true, const loading_requirements& req = {}, size_t thread_number = std::thread::hardware_concurrency() - 2);
	void multithread_load_tops(const vector< author_books* >& authors, const loading_requirements& req = {}, size_t thread_number = std::thread::hardware_concurrency() - 2);
	
	/*
	 *			   Artwork
	 *			 Requirements
	*/

	bool author_satisfies_requirements(author_books* author, const loading_requirements& req);
	bool artwork_satisfies_requirements(artwork_file* artwork, const loading_requirements& req);

	vector<author_books*> get_good_authors(const loading_requirements& req = {});
	vector<artwork_file*> get_good_artworks(author_books* author, const loading_requirements& req = {});
	vector<artwork_file*> get_all_good_artworks(const loading_requirements& req = {});
	
	void process_good_authors_engpoemness(const loading_requirements& req = {});
};


constexpr auto all_common_word_top_path = R"(D:\Literature_data\temp\common_word_top.txt)";

#define load_library() \
	Library lib; \
	lib.ensure_metadata_is_loaded(); \
	lib.ensure_wiki_data_loaded(); \
	lib.load_language(); \
	common_word_top all_top; \
	all_top.load_from_file(all_common_word_top_path);

