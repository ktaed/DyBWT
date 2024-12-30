#include "DyBWT.cpp"
#include <iostream>

int isNotACTGN(char c) {
    return  !std::isalnum(c);
    //!(c == 'A' || c == 'C' || c == 'T' || c == 'G' || c == 'N');
}

std::map<std::string, std::string> read_fasta(const std::string& filepath) {
    std::map<std::string, std::string> sequences;
    std::ifstream file(filepath);
    std::string line, header, sequence;
    std::set<char> alphabet, letters, diff;
    for (char c : "ACGTN$") {
        alphabet.insert(c);
    }
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filepath);
    }

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        if (line[0] == '>') {
            // If we have a previous sequence, save it
            if (!header.empty()) {
                sequences[header] = sequence;
            }
            // Start new sequence
            // line.replace(line.find(' '), 1, "_");
            header = line;  // Remove '>' character
            sequence.clear();
        } else {
            // Append to current sequence
            // letters.insert(line.begin(), line.end());
            // std::set_difference(letters.begin(), letters.end(), alphabet.begin(), alphabet.end(), std::inserter(diff, diff.begin()));
            
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
    try {
        std::string path = "/home/tsosie-schneider/Downloads/ncbi_dataset/data/GCA_000006155.2/GCA_000006155.2_ASM615v2_genomic.fna";
        // std::string path = "/home/tsosie-schneider/Downloads/ncbi_dataset/data/GCA_000008165.1/GCA_000008165.1_ASM816v1_genomic.fna";
        auto sequences = read_fasta(path);
        // DyBWT ref_bwt("");
        int i = 0;
        std::vector<std::string> seqs;
        for (const auto [header, seq] : sequences) {
            std::cout << "Header: " << header << " " << seq.length() << "\n";
            seqs.push_back(seq);
        }
        DyBWT ref_bwt(seqs[0]);
        ref_bwt.add(seqs[1]);
        ref_bwt.add(seqs[2]);
        std::vector<int> matches;
        for (const auto& [header, seq] : sequences) {
            // for (int i = 0; i < seq.length(); i++) {
            matches =ref_bwt.search(seq.substr(0, seq.length()-100));
            std::cout << "Matches for " << header << ": ";
            for (int j : matches) {
                std::cout << j << " ";
            }
            std::cout << std::endl;
        }
        // std::cout << ref_bwt.show() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    // DyBWT ref_bwt("banana");
    // std::cout << ref_bwt.get_bwt() << std::endl;
    // ref_bwt.add("ananab");
    // std::cout << ref_bwt.get_bwt() << std::endl;
    // ref_bwt.add("abracadabra");
    // // std::cout << ref_bwt.get_bwt() << std::endl;
    // // ref_bwt.add("arbadacarba");

    // std::cout << ref_bwt.show() << std::endl;
    // std::cout << "Done" << std::endl;
    // // std::cout << ref_bwt.show() << std::endl;
    // for (int i : ref_bwt.search("banana")) {
    //     std::cout << "banana: " << i << " ";
    // }
    // std::cout << std::endl;
    // for (int i : ref_bwt.search("ananab")) {
    //     std::cout << "ananab: " << i << " ";
    // }
    // std::cout << std::endl;

    // for (int i : ref_bwt.search("abracadabra")) {
    //     std::cout << "abracadabra: " << i << " ";
    // }
    // std::cout << std::endl;

    // for (int i : ref_bwt.search("arbadacarba")) {
    //     std::cout << "arbadacarba: " << i << " ";
    // }
    // std::cout << std::endl;

    // std::cout << ref_bwt.show() << std::endl;
    return 0;
}

