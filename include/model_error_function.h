#pragma once
#include "pch.h"
#include "Library.h"

double literature_fitness_function(const std::function<double(pair<artwork_file&, artwork_file&>)>& func, size_t amount = 1000);

