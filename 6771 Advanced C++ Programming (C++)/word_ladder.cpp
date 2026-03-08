#include "word_ladder.h"
#include <unordered_set>
#include <algorithm>
#include <fstream>
#include <map>
#include <string>
#include <queue>
#include <vector>

auto word_ladder::read_lexicon(const std::string &path) -> std::unordered_set<std::string> {
	std::ifstream ifs(path.c_str(), std::ifstream::in);
	std::unordered_set<std::string> res_lexicon;
	while (ifs.good()) {
		std::string line;
		std::getline(ifs, line);
		res_lexicon.insert(line);
	}
	ifs.close();
	return res_lexicon;
}


auto word_ladder::generate(
	const std::string &from,
	const std::string &to,
	const std::unordered_set<std::string> &lexicon
) -> std::vector<std::vector<std::string>> {
	// Initialize queue
	std::queue<std::vector<std::string>> q;
	std::vector<std::string> start {from};
	q.push(start);
	// Initialize seen words
	std::unordered_set<std::string> seen_words_current;
	seen_words_current.emplace(from);
	// Filters lexicon down to words that match the length of from/to
	std::unordered_set<std::string> lexicon_filtered (8192);
	auto first = lexicon.begin();
	auto last = lexicon.end();
	while (first!=last) {
		if (first->size() == from.size()) {
			lexicon_filtered.emplace(*first);
		}
		++first;
  	}
	lexicon_filtered.erase(from);

	std::vector<std::string> alphabet = {
		"a", "b", "c", "d", "e", "f", "g", 
		"h", "i", "j", "k", "l", "m", "n", 
		"o", "p", "q", "r", "s", "t", "u", 
		"v", "w", "x", "y", "z"
	};

	std::vector<std::vector<std::string>> ladders_return_vector;
	unsigned long smallest_ladder_size = 127145; // english.txt only contains 127142 words
	unsigned long current_ladder_size = 1;

	std::unordered_map<std::string, std::vector<std::string>> seen_words_adjacent;
	
	// Loops until queue is empty or all shortest ladders are found
	while (!q.empty()) {

		// Optimisation -> if current ladder has more rungs than the smallest ladder, automatically quits
		// as there will only be longer ladders thereafter
		if (smallest_ladder_size < q.front().size()) {
			break;
		}
		// Updates seen words every new 'generation' of ladders
		if (q.front().size() != current_ladder_size) {
			for (const auto& word : seen_words_current) {
				lexicon_filtered.erase(word);
			}
			seen_words_current.clear();
			current_ladder_size = q.front().size();
		}

		// If word has been seen, goes through all adjacent pairs, creates new ladders and adds them to the queue
		// Otherwise, starts finding adjacent words
		auto it = seen_words_adjacent.find(q.front().back());
		if (it != seen_words_adjacent.end()) {
			
			for (const auto& rung_new : it->second) {
				if (rung_new != to) {
					q.emplace(q.front().begin(), q.front().end());
					q.back().emplace_back(rung_new);
					seen_words_current.emplace(rung_new);
				}
				else {
					ladders_return_vector.emplace_back(q.front().begin(), q.front().end());
					ladders_return_vector.back().emplace_back(rung_new);
					smallest_ladder_size = q.front().size();
				}
			}
		}
		else {
			std::vector<std::string> vec;
			seen_words_adjacent.emplace(q.front().back(), vec);
			// Starting from the first letter of the current rung, replaces that letter with a letter from the alphabet. Does this
			// for each letter in the word and for every letter in the alphabet.
			// If the new word exists in lexicon, creates a new ladder with the appended new rung/word and then 
			// adds it to the end of the queue.
			for (unsigned long i = 0; i < q.front().back().length(); i++) {

				std::string rung_new;
				for (const auto& s : alphabet) {

					rung_new = q.front().back();
					rung_new.replace(i, 1, s);

					if (lexicon_filtered.count(rung_new) == 1) {

						seen_words_adjacent.at(q.front().back()).emplace_back(rung_new);

						if (rung_new != to) {
							q.emplace(q.front().begin(), q.front().end());
							q.back().emplace_back(rung_new);
							seen_words_current.emplace(rung_new);
						}
						else {
							ladders_return_vector.emplace_back(q.front().begin(), q.front().end());
							ladders_return_vector.back().emplace_back(rung_new);
							smallest_ladder_size = q.front().size();
						}
					}
				}
			}
			
		}

		q.pop();
	}
	
	std::sort(ladders_return_vector.begin(), ladders_return_vector.end());
	return ladders_return_vector;
}