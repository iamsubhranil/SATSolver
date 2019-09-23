#pragma once

//#include "robin_hood.h"
#include <unordered_map>

template <typename K, typename V> using HashMap = std::unordered_map<K, V>;
