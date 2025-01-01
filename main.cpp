#include "DyBWT.cpp"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

int isNotACTGN(char c) {
    return  !std::isalnum(c);
}

std::map<std::string, std::string> read_fasta(const std::string& filepath) {
    std::map<std::string, std::string> sequences;
    std::ifstream file(filepath);
    std::string line, header, sequence;
    std::set<char> alphabet;
    for (char c : "ACGTN$") {
        alphabet.insert(c);
    }
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filepath);
    }

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        if (line[0] == '>') {
            if (!header.empty()) {
                sequences[header] = sequence;
            }
            header = line;  // Remove '>' character
            sequence.clear();
        } else {
            std::replace_if(line.begin(), line.end(), isNotACTGN, 'N');

            sequence += line;
        }
    }
    
    // Save the last sequence
    if (!header.empty()) {
        sequences[header] = sequence;
    }
    
    file.close();
    return sequences;
}


int main() {
    std::cout << "\n=== Processing FASTA Files in Current Directory ===\n\n";
    
    // Get all .fasta files in current directory
    std::vector<std::string> fasta_files;
    for (const auto& entry : fs::directory_iterator(".")) {
        if (entry.path().extension() == ".fasta" || entry.path().extension() == ".fna") {
            fasta_files.push_back(entry.path().string());
        }
    }

    if (fasta_files.empty()) {
        std::cout << "No FASTA files found in current directory\n";
        return 1;
    }

    std::cout << "Found " << fasta_files.size() << " FASTA files\n\n";

    try {
        // Process first file to initialize DyBWT
        std::cout << "Processing initial file: " << fasta_files[0] << "\n";
        auto initial_sequences = read_fasta(fasta_files[0]);
        if (initial_sequences.empty()) {
            throw std::runtime_error("No sequences found in initial file");
        }

        // Initialize with first sequence
        auto first_seq = initial_sequences.begin();
        DyBWT dybwt(first_seq->second);
        std::cout << "Initialized with sequence: " << first_seq->first << "\n";
        std::cout << dybwt.show_properties() << "\n";

        // Add remaining sequences from first file
        for (auto it = std::next(initial_sequences.begin()); it != initial_sequences.end(); ++it) {
            std::cout << "Adding sequence: " << it->first << "\n";
            dybwt.add(it->second);
        }

        // Process remaining files
        for (size_t i = 1; i < fasta_files.size(); ++i) {
            std::cout << "\nProcessing file: " << fasta_files[i] << "\n";
            auto sequences = read_fasta(fasta_files[i]);
            
            for (const auto& [header, seq] : sequences) {
                std::cout << "Adding sequence: " << header << "\n";
                dybwt.add(seq);
            }
        }

        // Final BWT properties
        std::cout << "\n=== Final BWT Properties ===\n";
        std::cout << dybwt.show_properties() << "\n";

        // Test searching for original sequences
        std::cout << "\n=== Testing Original Sequence Searches ===\n";
        for (const auto& file : fasta_files) {
            std::cout << "\nTesting sequences from: " << file << "\n";
            auto sequences = read_fasta(file);
            
            for (const auto& [header, seq] : sequences) {
                std::cout << "\nSearching for sequence: " << header << "\n";
                std::cout << "Sequence length: " << seq.length() << "\n";
                
                // Test full sequence
                auto matches = dybwt.search(seq);
                std::cout << "Full sequence matches: " << matches.size() << " at positions: ";
                for (size_t i = 0; i < std::min(matches.size(), size_t(5)); ++i) {
                    std::cout << matches[i] << " ";
                }
                if (matches.size() > 5) std::cout << "...";
                std::cout << "\n";
                
                // Test prefix (first 1000 bp)
                size_t prefix_len = std::min(seq.length(), size_t(1000));
                matches = dybwt.search(seq.substr(0, prefix_len));
                std::cout << "Prefix (" << prefix_len << "bp) matches: " << matches.size() << " at positions: ";
                for (size_t i = 0; i < std::min(matches.size(), size_t(5)); ++i) {
                    std::cout << matches[i] << " ";
                }
                if (matches.size() > 5) std::cout << "...";
                std::cout << "\n";
                
                // Test suffix (last 1000 bp)
                size_t suffix_start = (seq.length() > 1000) ? seq.length() - 1000 : 0;
                matches = dybwt.search(seq.substr(suffix_start));
                std::cout << "Suffix (" << seq.length() - suffix_start << "bp) matches: " << matches.size() << " at positions: ";
                for (size_t i = 0; i < std::min(matches.size(), size_t(5)); ++i) {
                    std::cout << matches[i] << " ";
                }
                if (matches.size() > 5) std::cout << "...";
                std::cout << "\n";
            }
        }

        // Original pattern testing remains
        std::cout << "\n=== Testing Standard Pattern Searches ===\n";
        std::vector<std::string> test_patterns = {
            "ATGC", "GCTA", "TGCA", "CATG",
            "ATGCATGC", "GCATGCAT"
        };

        for (const auto& pattern : test_patterns) {
            auto matches = dybwt.search(pattern);
            std::cout << "Pattern '" << pattern << "': " 
                      << matches.size() << " matches at positions: ";
            for (size_t i = 0; i < std::min(matches.size(), size_t(10)); ++i) {
                std::cout << matches[i] << " ";
            }
            if (matches.size() > 10) std::cout << "...";
            std::cout << "\n";
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

