#include "./filtered_string_view.h"
#include <compare>
#include <cstddef>
#include <functional>
#include <iostream>
#include <string>

std::function<bool(char)> fsv::filtered_string_view::default_predicate = [](const char){
    return true;
};
auto operator<=>(const fsv::filtered_string_view &lhs, const fsv::filtered_string_view &rhs) -> std::strong_ordering {
    return static_cast<std::string>(lhs) <=> static_cast<std::string>(rhs);
}
auto operator==(const fsv::filtered_string_view &lhs, const fsv::filtered_string_view &rhs) -> bool {
    return static_cast<std::string>(lhs) == static_cast<std::string>(rhs);
}
auto operator<<(std::ostream &os, const fsv::filtered_string_view &fsv) -> std::ostream&{
    os << static_cast<std::string>(fsv);
    return os;
}
auto compose(const fsv::filtered_string_view &fsv, const std::vector<fsv::filter> &filts) -> fsv::filtered_string_view{
    // Constructing new predicate
    fsv::filter new_filt = [filts](const char c)-> bool {
        std::vector<fsv::filter> f = filts;
        for (const fsv::filter &fil : f) {
            if (!fil(c)) {
                return false;
            }
        }
        return true;
    };
    fsv::filtered_string_view new_fsv = fsv::filtered_string_view(fsv.data(), new_filt);
    return new_fsv;
}
auto split(const fsv::filtered_string_view &fsv, const fsv::filtered_string_view &tok) -> std::vector<fsv::filtered_string_view> {
    std::vector<fsv::filtered_string_view> results = {};
    // Empty fsv checks
    if (fsv.size() == 0 || tok.size() == 0) {
        fsv::filtered_string_view new_fsv = fsv;
        results.push_back(new_fsv);
        return results;
    }
    int word_length = 0;
    const char* start = fsv.data();// Start of current substring
    const char* cur = fsv.data();
    std::function<bool(char)> p = fsv.predicate();
    // Iterate to first letter passing predicate function
    while (!p(*start)) {
        ++start;
        ++cur;
    }
    // Goes through the fsv to find tok
    for (int i = 0; i < static_cast<int>(fsv.size()); ++i) {
        ++word_length;
        // If first letter of tok found
        if (fsv[i] == *tok.data()) {
            bool accept = true;
            // Finds whether the rest of tok appears
            for (int j = 0; j < static_cast<int>(tok.size()); ++j) {
                // Bounds checking
                if (i+j >= static_cast<int>(fsv.size())) {
                    accept = false;
                    break;
                }
                if (fsv[i+j] != tok[j]) {
                    accept = false;
                }
            }
            if (accept) {
                // constructing new predicate for current substring
                fsv::filter new_filt = [word_length, p](const char c) mutable -> bool {
                    --word_length; // This took me like two whole days to figure out
                    std::vector<fsv::filter> f = {
                        p,
                        [word_length](const char) -> bool {
                            if (word_length > 0) {
                                return true;
                            }
                            return false;
                        }
                    };
                    for (const fsv::filter &fil : f) {
                        if (!fil(c)) {
                            return false;
                        }
                    }
                    return true;
                };
                fsv::filtered_string_view split = {start, new_filt};
                results.push_back(split);
                // Moving pointer past tok to beginning of next word
                for (int j = 0; j < static_cast<int>(tok.size()); ++j) {
                    ++cur;
                }
                // Moving pointer to next letter passing predicate
                while (!p(*cur)) {
                    ++cur;
                }

                start = cur;
                word_length = 0;
                --cur;// hacky way of preventing cur from being advanced one too many
            }
        }
        ++cur;// will not advance cur one too many now
    }
    fsv::filtered_string_view split = {start, p};
    results.push_back(split);
    return results;
}
auto substr(const fsv::filtered_string_view &fsv, int pos = 0, int count = 0) -> fsv::filtered_string_view {
    const char * start = fsv.data();
    std::function<bool(char)> pred = fsv.predicate();
    // Move start up to starting position of substring
    for (int i = 0; i < pos; ++i) {
        while (!pred(*start)) {
            ++start;
        }
        ++start;
    }
    int rcount = count <= 0 ? static_cast<int>(fsv.size()) - pos : count;
    // constructing new predicate
    fsv::filter new_filt = [rcount, pred](const char c) mutable -> bool {
        std::vector<fsv::filter> f = {
            pred,
            [rcount](const char) -> bool {
                if (rcount > 0) {
                    return true;
                }
                return false;
            }
        };
        for (const fsv::filter &fil : f) {
            if (!fil(c)) {
                return false;
            }
        }
        --rcount;
        return true;
    };
    fsv::filtered_string_view substr = {start, new_filt};
    return substr;
}