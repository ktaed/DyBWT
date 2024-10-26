class BWT:
    def __init__(self, input_string):
        self.input = input_string + '$'  # Add end-of-string marker
        self.bwt = self.transform()
        self.f_column = sorted(self.bwt)
    
    def transform(self):
        # Implement BWT transformation
        rotations = sorted(self.input[i:] + self.input[:i] for i in range(len(self.input)))
        return ''.join(rotation[-1] for rotation in rotations)
    
    def get_lf_mapping(self):
        # Implement LF mapping
        char_counts = {}
        lf_mapping = []
        
        for i, char in enumerate(self.bwt):
            if char not in char_counts:
                char_counts[char] = 0
            lf_mapping.append((char, char_counts[char]))
            char_counts[char] += 1
        
        return lf_mapping
    
    def display_bwt_and_lf(self):
        lf_mapping = self.get_lf_mapping()
        
        print("Burrows-Wheeler Transform:")
        print(self.bwt)
        print("\nLF Mapping:")
        print("Index | L (BWT) | F (First) | (char, count)")
        print("------|---------|-----------|---------------")
        for i, ((l_char, count), f_char) in enumerate(zip(lf_mapping, self.f_column)):
            print(f"{i:5d} | {l_char:7s} | {f_char:9s} | ({l_char}, {count})")
    
    def inverse(self):
        # Implement inverse BWT transformation
        n = len(self.bwt)
        
        # Create dictionary to store ranks
        rank = {char: 0 for char in set(self.bwt)}
        
        # Create list to store (char, rank) pairs
        char_rank_pairs = []
        
        # Populate char_rank_pairs and update ranks
        for char in self.bwt:
            char_rank_pairs.append((char, rank[char]))
            rank[char] += 1
        
        # Create the inverse permutation
        inverse_perm = {v: i for i, v in enumerate(char_rank_pairs)}
        
        # Reconstruct the original string
        result = []
        i = inverse_perm[('$', 0)]
        for _ in range(n - 1):
            result.append(self.f_column[i])
            i = inverse_perm[(self.f_column[i], char_rank_pairs[i][1])]
        
        return ''.join(result)[::-1]  # Reverse the result to get the original string

# Demonstration code
if __name__ == "__main__":
    print("Demonstrating Burrows-Wheeler Transform")
    print("---------------------------------------")
    
    # Example 1
    input_string1 = "banana"
    bwt1 = BWT(input_string1)
    print(f"Input string: {input_string1}")
    bwt1.display_bwt_and_lf()
    recovered_string1 = bwt1.inverse()
    print(f"Recovered string: {recovered_string1}")
    print()
    
    # Example 2
    input_string2 = "ACTGACG"
    bwt2 = BWT(input_string2)
    print(f"Input string: {input_string2}")
    bwt2.display_bwt_and_lf()
    recovered_string2 = bwt2.inverse()
    print(f"Recovered string: {recovered_string2}")
