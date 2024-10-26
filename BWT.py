class BWT:
    def __init__(self, input_string):
        self.input = input_string + '$'  # Add end-of-string marker
    
    def transform(self):
        # Implement BWT transformation
        rotations = sorted(self.input[i:] + self.input[:i] for i in range(len(self.input)))
        return ''.join(rotation[-1] for rotation in rotations)
    
    def get_lf_mapping(self, bwt_string):
        # Implement LF mapping
        char_counts = {}
        lf_mapping = []
        
        for i, char in enumerate(bwt_string):
            if char not in char_counts:
                char_counts[char] = 0
            lf_mapping.append((char, char_counts[char]))
            char_counts[char] += 1
        
        return lf_mapping
    
    def display_bwt_and_lf(self):
        bwt = self.transform()
        lf_mapping = self.get_lf_mapping(bwt)
        
        # Get the F column (first column of sorted rotations)
        f_column = sorted(bwt)
        
        print("Burrows-Wheeler Transform:")
        print(bwt)
        print("\nLF Mapping:")
        print("Index | L (BWT) | F (First) | (char, count)")
        print("------|---------|-----------|---------------")
        for i, ((l_char, count), f_char) in enumerate(zip(lf_mapping, f_column)):
            print(f"{i:5d} | {l_char:7s} | {f_char:9s} | ({l_char}, {count})")
    
    def inverse(self, bwt_string):
        # Implement inverse BWT transformation
        pass

# Demonstration code
if __name__ == "__main__":
    print("Demonstrating Burrows-Wheeler Transform")
    print("---------------------------------------")
    
    # Example 1
    input_string1 = "banana"
    bwt1 = BWT(input_string1)
    print(f"Input string: {input_string1}")
    bwt1.display_bwt_and_lf()
    print()
    
    # Example 2
    input_string2 = "ACTGACG"
    bwt2 = BWT(input_string2)
    print(f"Input string: {input_string2}")
    bwt2.display_bwt_and_lf()
