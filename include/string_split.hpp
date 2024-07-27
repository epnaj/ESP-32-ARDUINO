#pragma once

#include <iostream>
#include <sstream>
#include <list>
#include <psram_custom_allocator.hpp>

/*
Split string using delimeter. Default is space.
*/
ps::list <ps::string> split(const ps::string &s, const ps::string &delimiter);

/*
Split string using delimeter. Default is space.
*/
ps::list <ps::string> split(const ps::string &s, char delim);

/*
Filter non hex values from given string.  
@attention 
Returns copy of string.
*/
ps::string filterNonHex(const ps::string &text);