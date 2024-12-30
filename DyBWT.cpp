#include <divsufsort.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <sdsl/bit_vectors.hpp>
#include <valarray>
#include <algorithm>

class DyBWT{
public:
    // Data members from Python implementation
    std::set<char> alphabet;
    
    std::map<char, std::string> f_vectors;
    std::map<char, std::vector<int>> lf_mapping;
    std::map<char, int> counts;
    // std::vector<saidx_t> prim_idxs;
    int seq_total = 0;

    std::string build_bwt(std::string& text)
    {
        saidx_t p_idx;
        sauchar_t *bwt = (sauchar_t*)malloc((1+text.length()) * sizeof(sauchar_t));
        p_idx = divbwt((const sauchar_t*)text.c_str(), bwt, NULL, text.length());
        // prim_idxs.push_back(p_idx);
        bwt[text.length()] ='\0';
        std::string temp((char*)bwt), ret_str;
        free(bwt);
        ret_str.reserve(temp.length() + 2);
        ret_str.append(temp.substr(0, p_idx));
        ret_str.append("$");
        ret_str.append(temp.substr(p_idx, temp.length() - p_idx));
        return ret_str;
    }
    
    void build_vectors(std::string& bwt, 
        std::map<char, int>& chr_counts,
        std::set<char>& sigma,
        std::map<char, std::string>& f_vect,
        std::map<char, std::vector<int>>& lf_map
    ) 
    {
        // std::map<char, int> chr_counts;
        std::vector<int> mapping;
        for (char c : bwt) {
            if (!sigma.contains(c)) {
                chr_counts[c] = 0;
                mapping.push_back(chr_counts[c]);

                chr_counts[c] += 1;
            }
            else {

                mapping.push_back(chr_counts[c]);
                chr_counts[c] += 1;
            }
                sigma.insert(c);
            
        }

        int i = 0, j = 0;
        for (char c : sigma) {
            j = chr_counts[c];
            f_vect[c] = bwt.substr(i, j);
            lf_map[c].insert(lf_map[c].end(), mapping.begin()+ i, mapping.begin()+ i +j );
            i += j;


        }

        for (char c : sigma) {
            std::cout << c << " " << chr_counts[c] << std::endl;
        }
        // if (temp == bwt) {
        //     std::cout << "Strings are equal" << std::endl;
        // }


    }
// public:
    // Constructor
    DyBWT(std::string initial_string){
        std::string bwt = build_bwt(initial_string);
        build_vectors(bwt, counts, alphabet, f_vectors, lf_mapping);
        seq_total += 1;

    }

    // Add new sequence
    // TODO: fix bug that in here not allowing added seqs to be
    void add(std::string substring) {
        std::string bwt = build_bwt(substring);
        std::map<char, int> chr_counts;
        std::map<char, std::string> f_vect;
        std::map<char, std::vector<int>> lf_map;
        std::set<char> sigma;
        std::vector<int> mapping;

        // Build vectors directly
        sigma.clear();
        for (char c : bwt) {
            if (!sigma.contains(c)) {
                sigma.insert(c);
                chr_counts[c] = 0;
                mapping.push_back(chr_counts[c]);
                chr_counts[c] += 1;
            } else {
                mapping.push_back(chr_counts[c]);
                chr_counts[c] += 1;
            }
        }

        // Build f_vectors and lf_mapping
        int i = 0, count = 0;
        std::string temp = "";

        for (char c : sigma) {
            count = chr_counts[c];
            f_vect[c] = bwt.substr(i, count);
            lf_map[c].insert(lf_map[c].end(), 
                            mapping.begin() + i, 
                            mapping.begin() + i + count);
            i += count;
            // std::cout << "c: " << c << ":"<< counts[c] << " " << f_vect[c] << " " << lf_map[c].front() << " " << lf_map[c].back() << std::endl;
        }

        for (char c : sigma) {
            std::cout << c << " " << chr_counts[c] << std::endl;
        }


        // Merge alphabets
        // std::set<char> all_sigma;
        // all_sigma.merge(alphabet);
        // all_sigma.merge(sigma);

        for (char c : alphabet){
            sigma.insert(c);
            if (!alphabet.contains(c)){
                counts[c] = 0;
            }
        }


        // Update offsets and merge vectors
        for (char c : sigma) {
            if ( alphabet.contains(c) && sigma.contains(c)) {
                // Pre-allocate space
                size_t new_size = f_vectors[c].length() + f_vect[c].length();
                f_vectors[c].reserve(new_size);
                lf_mapping[c].reserve(lf_mapping[c].size() + lf_map[c].size());

                // Update LF mapping offsets
                for (size_t i = 0; i < f_vect[c].length(); i++) {
                    char chr = f_vect[c][i];
                    int offset = counts[chr];
                    lf_map[c][i] += offset;
                }

                // Merge vectors
                f_vectors[c].append(f_vect[c]);
                for (int i : lf_map[c]){
                    lf_mapping[c].push_back(i);
                }

            }
            else if (sigma.contains(c)) {
                continue;
                std::cout << "sigma contains: " << c << " " << f_vect[c].length() << " " << lf_map[c].size() << std::endl;
                size_t new_size = f_vect[c].length();
                f_vectors[c].reserve(new_size);
                lf_mapping[c].reserve(lf_map[c].size());

                // Update LF mapping offsets
                for (size_t i = 0; i < f_vect[c].length(); i++) {
                    char chr = f_vect[c][i];
                    // counts[chr] += ;
                    int offset = counts[chr];
                    lf_map[c][i] += offset;
                }

                // Merge vectors
                f_vectors[c].append(f_vect[c]);
                for (int i : lf_map[c]){
                    lf_mapping[c].push_back(i);
                }
                // lf_mapping[c].insert(lf_mapping[c].end(), 
                //                    lf_map[c].begin(), 
                //                    lf_map[c].end());
            }

        }

        // Update counts
        for (char c : sigma) {
            alphabet.insert(c);
        }

        for (char c : alphabet) {
            // if (chr_counts.contains(c)) {
            //     std::cout << "add PreCount:" << c << ":" << counts[c] << " " << chr_counts[c] << std::endl;
            counts[c] += chr_counts[c];
            //     std::cout << "add PostCount:" << c << ":" << counts[c] << std::endl;    
            // }
            // else {
            //     std::cout << "No change add PostCount:" << c << ":" << counts[c] << std::endl;    
            // }
        }


        seq_total++;
    }

    // Get BWT string
    std::string get_bwt() {
        std::string result;
        for (char c : alphabet) {
            result += f_vectors[c];
        }
        // Implementation similar to Python get_bwt()
        return result;
    }

    // Get original string from index
    std::string get_original(int index) {
        std::string sequence;
        // Implementation similar to Python get_original()
        return sequence;
    }

    // Get all original strings
    std::vector<std::string> get_originals() {
        std::vector<std::string> ret_lst;
        // Implementation similar to Python get_originals()
        return ret_lst;
    }

    // Search for pattern
    std::vector<int> search(std::string pattern) {
        std::vector<int> matches, positions;
        int offset = 0;
        std::reverse(pattern.begin(), pattern.end());
        bool found = false;
        int nxt_ind, cur_ind;
        char nxt_chr, cur_chr;

        if (pattern.length() == 0) {
            return positions;
        }
        for (char c : alphabet) {
            if (c < pattern[0]) {
                offset += f_vectors[c].length();
            }
        }

        if (pattern.length() == 1) {

            for (int i = offset; i < offset + f_vectors[pattern[0]].length(); i++) {
                positions.push_back(i);
            }
            return positions;
        }
        else {

            for (int i = 0; i < f_vectors[pattern[0]].length(); i++) {
                matches.push_back(i);
            }
            char start_char = pattern[0];
            // pattern = pattern.substr(1, pattern.length() - 1);
            // std::cout << "matches: " << matches.size() << std::endl;
            std::string seq, max_seq;
            int max_idx = 0, max_i = 0;
            for (int idx : matches) {
                seq = pattern[0];
                cur_chr = f_vectors[start_char][idx];
                nxt_ind = lf_mapping[start_char][idx];
                for (int i = 1; i < pattern.length(); i++) {
                    // seq += pattern[i];
                    nxt_chr = f_vectors[cur_chr][nxt_ind];
                    nxt_ind = lf_mapping[cur_chr][nxt_ind];
                    // std::cout << pattern[i] << " " << cur_chr << " " << nxt_chr << std::endl;
                    // if (i > max_idx) {
                    //     max_idx = idx;
                    //     max_i = i;
                    //     max_seq = seq+cur_chr;
                    // }
                    if (pattern[i] != cur_chr) {
                        // std::cout << "Pattern not found " << idx << " " << i << std::endl;
                        break;
                    }
                    seq += cur_chr;
                    cur_chr = nxt_chr;

                }
                // std::cout << seq << " " << cur_chr << std::endl;
                if (seq == pattern) {
                    // std::cout << idx + offset << "\n";
                    positions.push_back(idx + offset);
                }
            }
            // pattern = start_char + pattern;
            // max_seq = start_char + max_seq;
            // std::reverse(pattern.begin(), pattern.end());
            // std::reverse(max_seq.begin(), max_seq.end());
            // std::cout << "Max idx: " << max_i +1 << " " << max_idx  << std::endl;
            // std::cout << pattern << std::endl;
            // std::cout << max_seq  << std::endl;
            // return positions;

        }
        return positions;


    }

    // Length operator
    size_t length() const {
        size_t total = 0;
        // Sum up counts similar to Python __len__
        return total;
    }

std::string show() {
    std::string temp = "";
    int j = 0;

    for (char c : alphabet) {
        std::cout << c << " " << f_vectors[c].length() << " " << lf_mapping[c].size() << std::endl;
        temp += f_vectors[c]    ;

    }
 
    return temp;
}
    
};

// Main function with test cases
// int main() {
//     // Test Case 1: Basic Single Sequence
//     std::cout << "\n=== Test Case 1: Basic Single Sequence ===\n";
//     DyBWT bwt("banana");
//     std::cout << "Initial text: 'banana'\n";
//     std::cout << bwt.show() << std::endl;
//     std::cout << bwt.get_bwt() << std::endl;
    
//     // Test Case 2: Two Sequences
//     std::cout << "\n=== Test Case 2: Two Sequences ===\n";
//     bwt.add("ananab");
//     std::cout << bwt.show() << std::endl;
//     std::cout << bwt.get_bwt() << std::endl;

//     // Test Case 3: Three Sequences
//     std::cout << "\n=== Test Case 3: Three Sequences ===\n";
//     bwt.add("abracadabra");
//     std::cout << bwt.show() << std::endl;
//     std::cout << bwt.get_bwt() << std::endl;



//     return 0;
// }

