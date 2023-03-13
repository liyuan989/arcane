#ifndef ARCANE_STRING_UTILS_H
#define ARCANE_STRING_UTILS_H

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

namespace arcane {

inline std::vector<std::string> Split(const std::string& str, char sep) {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = 0;
    while ((end = str.find(sep, start)) != std::string::npos) {
        tokens.push_back(str.substr(start, end - start));
        start = end + 1;
    }
    if (start != std::string::npos) {
        tokens.push_back(str.substr(start));
    }
    return tokens;
}

template <typename Container>
std::string Join(const Container& c, const std::string& sep) {
    std::ostringstream ss;
    std::copy(c.begin(), 
              c.end(), 
              std::ostream_iterator<typename Container::value_type>(ss, sep.c_str()));
    return ss.str();
}

} // namespace arcane

#endif

