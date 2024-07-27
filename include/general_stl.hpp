#pragma once

#include <vector>
#include <list>
#include <string>
#include <map>

namespace general {
    template <typename T, class Allocator>
    using vector = std::vector <T, Allocator>;
    template <typename T, class Allocator>
    using list = std::list <T, Allocator>;
    template <typename Key, typename Val, class Allocator>
    using map = std::map <Key, Val, std::less<Key>, Allocator>;
    template <class Allocator>
    using string = std::basic_string <char, std::char_traits <char>, Allocator>;
}