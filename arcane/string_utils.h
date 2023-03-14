#ifndef ARCANE_STRING_UTILS_H
#define ARCANE_STRING_UTILS_H

#include <ctype.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <algorithm>
#include <iterator>

namespace arcane {

inline std::vector<std::string> Split(const std::string& s, const std::string& sep) {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = 0;
    while ((end = s.find(sep, start)) != std::string::npos) {
        tokens.push_back(s.substr(start, end - start));
        start = end + sep.size();
    }
    if (start != std::string::npos) {
        tokens.push_back(s.substr(start));
    }
    return tokens;
}

inline std::vector<std::string> SplitLines(const std::string&s) {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = 0;
    while (end != std::string::npos) {
        size_t step = 0;
        if ((end = s.find("\n", start)) != std::string::npos) {
            step = 1;
        } else if ((end = s.find("\r", start)) != std::string::npos) {
            step = 1;
        } else if ((end = s.find("\v", start)) != std::string::npos) {
            step = 1;
        } else if ((end = s.find("\f", start)) != std::string::npos) {
            step = 1;
        } else if ((end = s.find(28, start)) != std::string::npos) {
            step = 1; // file separator
        } else if ((end = s.find(29, start)) != std::string::npos) {
            step = 1; // group separator
        } else if ((end = s.find(30, start)) != std::string::npos) {
            step = 1; // record separator
        } else if ((end = s.find("\r\n", start)) != std::string::npos) {
            step = 2;
        }
        if (end != std::string::npos) {
            tokens.push_back(s.substr(start, end - start));
        } else {
            tokens.push_back(s.substr(start));
        }
        start = end + step;
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

inline bool IsAlnum(const std::string& s) {
    if (s.empty()) {
        return false;
    }
    for (char c : s) {
        if (!isalnum(c)) {
            return false;
        }
    }
    return true;
}

inline bool IsAlpha(const std::string& s) {
    if (s.empty()) {
        return false;
    }
    for (char c : s) {
        if (!isalpha(c)) {
            return false;
        }
    }
    return true;
}

inline bool IsDigit(const std::string& s) {
    if (s.empty()) {
        return false; 
    } 
    for (char c : s) {
        if (!isdigit(c)) {
            return false;
        }
    }
    return true;
}

inline bool IsNumeric(const std::string& s) {
    if (s.empty()) {
        return false;
    }
    for (char c : s) {
        if (!isdigit(c)) {
            return false;
        }
    }
    return true;
}

inline bool IsSpace(const std::string& s) {
    if (s.empty()) {
        return false;
    }
    for (char c : s) {
        if (!isspace(c)) {
            return false;
        }
    }
    return true;
}

inline bool IsTitle(const std::string& s) {
    if (s.empty()) {
        return false;
    }
    bool is_prev_alnum = false;
    for (char c : s) {
        if (isalnum(c)) {
            if (!is_prev_alnum && !isupper(c)) {
                return false;
            }
            if (is_prev_alnum && isupper(c)) {
                return false;
            }
        } else {
            is_prev_alnum = false;
        }
    }
    return true;
}

inline bool IsUpper(const std::string& s) {
    if (s.empty()) {
        return false;
    }
    for (char c : s) {
        if (!isupper(c)) {
            return false;;
        }
    }
    return true;
}

inline bool IsLower(const std::string& s) {
    if (s.empty()) {
        return false;
    } 
    for (char c : s) {
        if (!islower(c)) {
            return false;
        }     
    }
    return true;
}

inline std::string Lower(const std::string& s) {
    std::string res;
    res.reserve(s.size());
    for (char c : s) {
        if (isdigit(c)) {
            res.push_back(tolower(c));
        } else {
            res.push_back(c);
        }
    }
    return res;
}

inline std::string Replace(
        const std::string& s, 
        const std::string& old,
        const std::string& target,
        size_t count = 0) {
    std::string res;
    res.reserve(s.size());
    size_t start = 0;
    size_t end = 0;
    size_t replace_count = 0;
    while (end != std::string::npos) {
        end = s.find(old, start);
        if (end != std::string::npos) {
            res.append(s.substr(start, end - start));
            res.append(target);
            start = end + old.size();
            ++replace_count;
            if (count > 0 && replace_count >= count) {
                res.append(s.substr(start));
                break;
            }
        } else {
            res.append(s.substr(start));
        }
    }
    return res;
}

inline std::string Expandtabs(const std::string& s, size_t tabsize = 8) {
    std::string res;
    res.reserve(s.size());
    size_t start = 0;
    size_t end = 0;
    while (end != std::string::npos) {
        end = s.find("\t", start);
        if (end != std::string::npos) {
            res.append(s.substr(start, end - start));
            size_t count = tabsize - end % tabsize;
            for (size_t i = 0; i < count; ++i) {
                res.push_back(' ');
            }
            start = end + 1;
        } else {
            res.append(s.substr(start));
        }
    }
    return res;
}

inline size_t Count(const std::string& s, const std::string& sub) {
    size_t count = 0;
    size_t start = 0;
    size_t end = 0;
    while ((end = s.find(sub, start)) != std::string::npos) {
        ++count;
    }
    return count;
}

inline std::string Center(const std::string& s, size_t width, char fill = ' ') {
    if (width <= s.size()) {
        return s;
    }
    std::string res;
    res.reserve(width);
    size_t pre_count = (width - s.size()) / 2;
    for (size_t i = 0; i < pre_count; ++i) {
        res.push_back(fill);
    }
    res.append(s);
    size_t tail_count = width - pre_count - s.size();
    for (size_t i = 0; i < tail_count; ++i) {
        res.push_back(fill);
    }
    return res;
}

inline std::string Ljust(const std::string& s, size_t width, char fill) {
    if (width <= s.size()) {
        return s;
    }
    std::string res;
    res.reserve(width);
    res.append(s);
    for (size_t i = 0; i < width - s.size(); ++i) {
        res.push_back(fill);
    }
    return res;
}

inline std::string Rjust(const std::string& s, size_t width, char fill) {
    if (width <= s.size()) {
        return s;
    }
    std::string res;
    res.reserve(width);
    for (size_t i = 0; i < width - s.size(); ++i) {
        res.push_back(fill);
    }
    res.append(s);
    return res;
}

inline std::vector<std::string> Partition(const std::string& s, const std::string& sep) {
    std::vector<std::string> res;
    res.reserve(3);
    size_t i = s.find(sep, 0);
    if (i != std::string::npos) {
        res.push_back(s.substr(0, i));
        res.push_back(sep);
        res.push_back(s.substr(i + sep.size()));
    } else {
        res.push_back("");
        res.push_back(sep);
        res.push_back("");
    }
    return res;
}

inline std::vector<std::string> Rpartition(const std::string& s, const std::string& sep) {
    std::vector<std::string> res;
    res.reserve(3);
    size_t i = s.rfind(sep, 0);
    if (i != std::string::npos) {
        res.push_back(s.substr(0, i));
        res.push_back(sep);
        res.push_back(s.substr(i + sep.size()));
    } else {
        res.push_back("");
        res.push_back(sep);
        res.push_back("");
    }
    return res;
}

inline bool Startswith(const std::string& s, const std::string& perfix) {
    return s.rfind(perfix, 0) == 0; 
}

inline bool Endswith(const std::string& s, const std::string& suffix) {
    if (suffix.size() > s.size()) {
        return false;
    }
    return s.find(suffix, 0) == (s.size() - suffix.size());
}

inline std::string& LstripFast(std::string& s, char c = 0) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [c](char ch) {
        if (c > 0) {
            return ch != c;
        } else {
            return !isspace(ch);
        }
    }));
    return s;
}

inline std::string Lstrip(const std::string& s, char c = 0) {
    std::string res = s;
    LstripFast(res, c);
    return res;
}

inline std::string& RstripFast(std::string& s, char c = 0) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [c](char ch) {
        if (c > 0) {
            return ch != c;
        } else {
            return !isspace(ch);
        }
    }).base(), s.end());
    return s;
}

inline std::string Rstrip(const std::string& s, char c = 0) {
    std::string res = s;
    RstripFast(res, c);
    return res;
}

inline std::string& StripFast(std::string&s , char c = 0) {
    LstripFast(s, c);
    RstripFast(s, c);
    return s;
}

inline std::string Strip(const std::string& s, char c = 0) {
    std::string res = s;
    LstripFast(res, c);
    RstripFast(res, c);
    return res;
}

inline std::string Swapcase(const std::string& s) {
    std::string res;
    res.reserve(s.size());
    for (char c : s) {
        char to = c;
        if (isalpha(c)) {
            if (islower(c)) {
                to = toupper(c); 
            } else {
                to = tolower(c);
            }
        }
        res.push_back(to);
    }
    return res;
}

inline std::string Title(const std::string& s) {
    std::string res;
    res.reserve(s.size());
    bool is_prev_alpha = false;
    for (char c : s) {
        char to = c;
        if (isalpha(c)) {
            if (!is_prev_alpha) {
                to = toupper(c);
                is_prev_alpha = true;
            }
        } else {
            is_prev_alpha = false;
        }
        res.push_back(to);
    }
    return res;
}

inline std::string Translate(
        const std::string& s,
        const std::unordered_map<char, std::string>& table) {
    std::string res;
    res.reserve(s.size());
    for (char c : s) {
        auto it = table.find(c);
        if (it != table.end()) {
            res.append(it->second);
        } else {
            res.push_back(c);
        }
    }
    return res;
}

inline std::string Upper(const std::string& s) {
    std::string res;
    res.reserve(s.size());
    for (char c : s) {
        res.push_back(toupper(c));
    }
    return res;
}

inline std::string Zfill(const std::string& s, size_t width) {
    if (width <= s.size() || !IsDigit(s)) {
        return s;
    }
    std::string res;
    res.reserve(width);
    size_t len = s.size() - width;
    if (std::stoll(s) < 0) {
        len -= 1;
        res.push_back('-');
    }
    for (size_t i = 0; i < len; ++i) {
        res.push_back('0'); 
    }
    res.append(s);
    return res;
}

} // namespace arcane

#endif

