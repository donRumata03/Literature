#pragma once

#include "word_top.h"

struct author_books;

struct artwork_file
{
	string raw_content;
	vector<string> parsed_content;
	vector<string> unknown_words;
	word_top top;

	string name;
	string folder_path;
	string file_path;
	size_t file_size = 0;
	string author_name;
	string author_path;

	string top_path;
	string top_meta_path;
	
	string parsed_path;

	bool is_poem = false;
	bool is_english = false;
};

struct author_books
{
	string name;
	string path;
	Json descriptor;
	vector<artwork_file> artworks;
	size_t total_size = 0;
};


