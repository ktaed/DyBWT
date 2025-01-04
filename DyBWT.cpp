#include <divsufsort.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <sdsl/bit_vectors.hpp>
#include <valarray>
#include <algorithm>
#include <sstream>

class DyBWT{
private:
    // Stores the alphabet (unique characters) found in all sequences
    std::set<char> alphabet;    

    // Maps each character to its corresponding column in the BWT matrix
    // Key: character, Value: string of characters in that column
    std::map<char, std::vector<char>> f_vectors;

    // Maps each character to its Last-to-First (LF) mapping values
    // Key: character, Value: vector of indices for LF mapping
    std::map<char, std::vector<int>> lf_mapping;

    // Stores the count of each character in the sequences
    // Key: character, Value: number of occurrences
    std::map<char, int> counts;

    // Total number of sequences added to the structure
    int seq_total = 0;


    /**
     * Builds the Burrows-Wheeler Transform (BWT) of the input text
     * 
     * @param text The input string to transform
     * @return The BWT string with '$' sentinel character inserted
     * 
     * Steps:
     * 1. Allocates memory for BWT computation
     * 2. Uses divsufsort's divbwt to compute BWT
     * 3. Converts result to string and inserts '$' at primary index
     * 4. Returns final BWT string with sentinel character
     * 
     * Note: Uses libdivsufsort for efficient suffix array construction
     */
    std::string build_bwt(std::string& text)
    {
        saidx_t p_idx;
        sauchar_t *bwt = (sauchar_t*)malloc((1+text.length()) * sizeof(sauchar_t));
        p_idx = divbwt((const sauchar_t*)text.c_str(), bwt, NULL, text.length());
        bwt[text.length()] ='\0';
        std::string temp((char*)bwt), ret_str;
        free(bwt);
        ret_str.reserve(temp.length() + 2);
        ret_str.append(temp.substr(0, p_idx));
        ret_str.append("$");
        ret_str.append(temp.substr(p_idx, temp.length() - p_idx));
        return ret_str;
    }
    
    /**
     * Builds the split vector representation of a BWT string
     * 
     * @param bwt          The BWT string to process
     * @param chr_counts   Map to store character frequencies
     * @param sigma        Set to store alphabet of unique characters
     * @param f_vect       Map to store F-vectors (first column of BWT matrix)
     * @param lf_map       Map to store LF-mapping values for each character
     * 
     * Function:
     * 1. Splits BWT string into character-specific vectors
     * 2. Computes character frequencies
     * 3. Builds LF-mapping for each character position
     * 4. Constructs F-vectors for BWT matrix columns
     * 
     * Note: Core component for dynamic BWT operations
     */
    void build_vectors(std::string& bwt, 
        std::map<char, int>& chr_counts,
        std::set<char>& sigma,
        std::map<char, std::vector<char>>& f_vect,
        std::map<char, std::vector<int>>& lf_map
    ) 
    {
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
            std::copy(bwt.begin() + i, bwt.begin() + i + j, std::back_inserter(f_vect[c]));
            std::copy(mapping.begin() + i, mapping.begin() + i + j, std::back_inserter(lf_map[c]));
            // lf_map[c].insert(lf_map[c].end(), mapping.begin()+ i, mapping.begin()+ i +j );
            i += j;
        }
    }
    public:
    // Constructor
    DyBWT(std::string initial_string){
        std::string bwt = build_bwt(initial_string);
        build_vectors(bwt, counts, alphabet, f_vectors, lf_mapping);
        seq_total++;

    }

    /**
     * Adds a new sequence to the dynamic BWT structure
     * 
     * @param substring The new sequence to add to the existing BWT
     * 
     * Process:
     * 1. Builds BWT of new sequence
     * 2. Creates split vector representation of new sequence
     * 3. Updates alphabet with new characters
     * 4. Merges vectors:
     *    - Pre-allocates space for efficiency
     *    - Updates LF-mapping offsets based on existing counts
     *    - Appends new F-vectors and LF-mappings
     * 5. Updates character counts
     * 
     * Note: Critical for maintaining dynamic BWT structure when adding sequences.
     * Handles merging of data structures while preserving BWT properties.
     */
    void add(std::string substring) {
        std::string bwt = build_bwt(substring);
        std::map<char, int> chr_counts;
        std::map<char, std::vector<char>> f_vect;
        std::map<char, std::vector<int>> lf_map;
        std::set<char> sigma;
        std::vector<int> mapping;

        build_vectors(bwt, chr_counts, sigma, f_vect, lf_map);

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
                size_t new_size = f_vectors[c].size() + f_vect[c].size();
                f_vectors[c].reserve(new_size);
                lf_mapping[c].reserve(lf_mapping[c].size() + lf_map[c].size());

                // Update LF mapping offsets
                for (size_t i = 0; i < f_vect[c].size(); i++) {
                    char chr = f_vect[c][i];
                    int offset = counts[chr];
                    lf_map[c][i] += offset;
                }

                // Merge vectors
                std::copy(f_vect[c].begin(), f_vect[c].end(), std::back_inserter(f_vectors[c]));
                std::copy(lf_map[c].begin(), lf_map[c].end(), std::back_inserter(lf_mapping[c]));

            }   
            else if (sigma.contains(c)) {
                size_t new_size = f_vect[c].size();
                f_vectors[c].reserve(new_size);
                lf_mapping[c].reserve(lf_map[c].size());

                // Update LF mapping offsets
                for (size_t i = 0; i < f_vect[c].size(); i++) {
                    char chr = f_vect[c][i];
                    int offset = counts[chr];
                    lf_map[c][i] += offset;
                }

                // Merge vectors
                std::copy(f_vect[c].begin(), f_vect[c].end(), std::back_inserter(f_vectors[c]));
                std::copy(lf_map[c].begin(), lf_map[c].end(), std::back_inserter(lf_mapping[c]));
            }

        }

        for (char c : sigma) {
            alphabet.insert(c);
        }

        for (char c : alphabet) {
            counts[c] += chr_counts[c];
        }
        seq_total++;
    }

    // Get BWT string
    std::string get_bwt() {
        
        std::string result;

        for (char c : alphabet) {
            result += std::string(f_vectors[c].begin(), f_vectors[c].end());
        }
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
                offset += f_vectors[c].size();
            }
        }

        if (pattern.length() == 1) {

            for (int i = offset; i < offset + f_vectors[pattern[0]].size(); i++) {
                positions.push_back(i);
            }
            return positions;
        } else if (pattern.length() == 2) {
            for (int i = 0; i < f_vectors[pattern[0]].size(); i++) {
                matches.push_back(i);
            }
            for (int i : matches) {
                if (f_vectors[pattern[0]][i] == pattern[1]) {
                    positions.push_back(i + offset);
                }
            }
            return positions;
        }
        else {

            for (int i = 0; i < f_vectors[pattern[0]].size(); i++) {
                matches.push_back(i);
            }
            char start_char = pattern[0];
            std::string seq, max_seq;
            int max_idx = 0, max_i = 0;
            for (int idx : matches) {
                seq = pattern[0];
                cur_chr = f_vectors[start_char][idx];
                nxt_ind = lf_mapping[start_char][idx];
                for (int i = 1; i < pattern.length(); i++) {
                    if (cur_chr != pattern[i]) {
                        break;
                    }
                    seq += cur_chr;
                    nxt_chr = f_vectors[cur_chr][nxt_ind];
                    nxt_ind = lf_mapping[cur_chr][nxt_ind];
                    cur_chr = nxt_chr;
                }
                if (seq == pattern) {
                    positions.push_back(idx + offset);
                }
            }
        }
        return positions;


    }

    int max_suf_depth(std::string pattern) {
        int max_i = 0;
        std::vector<int> matches, positions;
        int offset = 0;
        std::reverse(pattern.begin(), pattern.end());
        bool found = false;
        int nxt_ind, cur_ind;
        char nxt_chr, cur_chr;

        if (pattern.length() == 0) {
            return -1;
        }
        for (char c : alphabet) {
            if (c < pattern[0]) {
                offset += f_vectors[c].size();
            }
        }

        if (pattern.length() == 1) {
            for (int i = offset; i < offset + f_vectors[pattern[0]].size(); i++) {
                positions.push_back(i);
            }
            if (positions.size() > 0) {
                return 1;
            }
            return -1;
        } else if (pattern.length() == 2) {
            for (int i = 0; i < f_vectors[pattern[0]].size(); i++) {
                matches.push_back(i);
            }
            for (int i : matches) {
                if (f_vectors[pattern[0]][i] == pattern[1]) {
                    positions.push_back(i + offset);
                }
            }
            if (positions.size() > 0) {
                return 2;
            }
            return -1;
        }
        else {

            for (int i = 0; i < f_vectors[pattern[0]].size(); i++) {
                matches.push_back(i);
            }
            if (matches.size() == 0) {
                return -1;
            }
            char start_char = pattern[0];
            std::string seq, max_seq;
            
            int i;
            for (int idx : matches) {
                seq = pattern[0];
                cur_chr = f_vectors[start_char][idx];
                nxt_ind = lf_mapping[start_char][idx];
                for (i = 1; i < pattern.length(); i++) {
                    if (cur_chr != pattern[i]) {
                        break;
                    }
                    seq += cur_chr;
                    nxt_chr = f_vectors[cur_chr][nxt_ind];
                    nxt_ind = lf_mapping[cur_chr][nxt_ind];
                    cur_chr = nxt_chr;
                }
                if (i > max_i) {
                    max_i = i;
                }
            }
        }


        return max_i;
    }
    // Length operator
    size_t length() const {
        size_t total = 0;
        for (auto& [c, f_vect] : f_vectors) {
            total += f_vect.size();
        }
        return total;
    }

    /**
     * Displays the current state and properties of the Dynamic BWT
     * @return String containing the analysis
     */
    std::string show_properties() {
        std::stringstream ss;
        size_t total_length = 0;
        
        ss << "\n=== Dynamic BWT Properties ===\n";
        ss << "Number of sequences: " << seq_total << "\n\n";
        
        // Alphabet and character statistics
        ss << "Alphabet size: " << alphabet.size() << "\n";
        ss << "Character frequencies:\n";
        for (char c : alphabet) {
            // Check if character exists in all maps before accessing
            if (f_vectors.count(c) && counts.count(c) && lf_mapping.count(c)) {
                total_length += f_vectors.at(c).size();
                ss << "'" << c << "': " << counts.at(c) 
                   << " (F-vector length: " << f_vectors.at(c).size() 
                   << ", LF-mapping size: " << lf_mapping.at(c).size() << ")\n";
            } else {
                ss << "'" << c << "': WARNING - Missing from one or more maps"
                   << " (f_vectors: " << (f_vectors.count(c) ? "yes" : "no")
                   << ", counts: " << (counts.count(c) ? "yes" : "no")
                   << ", lf_mapping: " << (lf_mapping.count(c) ? "yes" : "no")
                   << ")\n";
            }
        }
        
        // Overall statistics
        ss << "\nTotal length: " << total_length << "\n";
        ss << "Average sequence length: " << (total_length / seq_total) << "\n";
        
        // First few characters of concatenated BWT
        ss << "\nBWT preview (first 50 chars): ";
        std::string bwt_preview;
        for (char c : alphabet) {
            bwt_preview += std::string(f_vectors.at(c).begin(), f_vectors.at(c).end());
            if (bwt_preview.length() >= 50) break;
        }
        if (bwt_preview.length() < 50) {
            ss << bwt_preview << "...\n";
        }
        else {
            ss << bwt_preview.substr(0, 50) << "...\n";
        }
        
        return ss.str();
    }

    std::string show() {
        std::string temp = "";
        int j = 0;

        for (char c : alphabet) {
            std::cout << c << " " << f_vectors[c].size() << " " << lf_mapping[c].size() << std::endl;
            temp += std::string(f_vectors[c].begin(), f_vectors[c].end());

        }
     
        return temp;
    }
    
};

