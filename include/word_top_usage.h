#pragma once
#include "pch.h"
#include "word_top.h"
#include "GA.h"
#include "Library.h"

// inline common_word_top unifying_word_top();

double estimate_word_top_coefficients(const vector<double>& pows /* double common_pow, double diff_pow */);
void find_word_top_dist_coefficients();

void cook_all_author_tops(author_books& author);

vector<double> generate_tolstoy_ya_freqs();