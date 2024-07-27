#include <string_split.hpp>
// https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c

ps::list <ps::string> split(const ps::string &s, const ps::string &delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    ps::string token;
    ps::list <ps::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

ps::list <ps::string> split(const ps::string &s, const char delim) {
    ps::list <ps::string> result;
    std::stringstream ss (s.c_str());
    ps::string item;

    while (getline (ss, item, delim)) {
        result.push_back(item);
    }

    return result;
}

ps::string filterNonHex(const ps::string &text){
    ps::string output;
    for(const char &c : text) {
        if(isxdigit(c)) {
            output += c;
        }
    }
    output.shrink_to_fit();
    return output;
}