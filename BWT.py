class BWT:
    """
    Burrows-Wheeler Transform implementation.
    Transforms input string into BWT representation using F-vectors and LF-mapping.
    """
    def __init__(self, input_string):
        # Append end-of-string marker '$' to input string
        self.input = input_string + '$'
        # Build the BWT structures: F-vectors (first column) and LF-mapping vectors
        self.f_vectors, self.lf_mapping, self.counts = self._build_bwt()

    def __str__(self):
        """
        Returns a string representation of the BWT showing:
        L: Last column characters
        F: First column characters
        Map: LF-mapping values for each character
        """
        ret_str = f"{'L':<5}\t{'F':<5}\t{'Map':<5}\n"
        ret_str += "-" * 25 + "\n"
        
        # Iterate through sorted characters in first column
        for sigma in sorted(self.f_vectors.keys()):
            # Get corresponding last column characters
            last_chars = self.f_vectors[sigma]
            # Get LF-mapping values
            mappings = self.lf_mapping[sigma]
            
            # Print each character's mapping
            for i in range(len(last_chars)):
                last_char = last_chars[i]
                mapping = mappings[i]
                ret_str += f"{last_char:<5}\t{sigma:<5}\t{mapping}\n"
        
        # Add summary information
        ret_str += "-" * 25 + "\n"
        ret_str += f"Total length: {sum(len(v) for v in self.f_vectors.values())}\n"
        ret_str += f"Unique chars: {len(self.f_vectors)}\n"
        ret_str += f"Char counts: {self.counts}"
        
        return ret_str

    def _build_bwt(self):
        """
        Builds the Burrows-Wheeler Transform structures.
        
        Returns:
            tuple: (f_vectors, lf_vectors)
                - f_vectors: Dictionary mapping first column chars to their corresponding last column chars
                - lf_vectors: Dictionary storing LF-mapping information for each character
        """
        # Initialize data structures
        map_vectors = {}  # Stores LF-mapping information
        f_vectors = {}   # Stores first-to-last column mapping
        char_counts = {} # Keeps track of character occurrences

        # Generate all rotations of input string and sort them
        rotations = sorted(self.input[i:] + self.input[:i] for i in range(len(self.input)))

        # Process each rotation to build BWT structures
        for rotation in rotations:
            # Handle first occurrence of a character in first column
            if rotation[0] not in f_vectors:
                f_vectors[rotation[0]] = rotation[-1]
                map_vectors[rotation[0]] = []
            else:
                # Append to existing character mapping
                f_vectors[rotation[0]] += rotation[-1]

            # Initialize count for new characters in last column
            if rotation[-1] not in char_counts:
                char_counts[rotation[-1]] = 0

            # Build LF-mapping: (character, occurrence_count)
            map_vectors[rotation[0]].append(char_counts[rotation[-1]])
            char_counts[rotation[-1]] += 1

        return f_vectors, map_vectors, char_counts


# Demonstration code
if __name__ == "__main__":
    print("Demonstrating Burrows-Wheeler Transform")
    print("---------------------------------------")
    
    # Example 1: Basic string transformation
    print("\nExample 1: 'banana'")
    print("-----------------")
    input_string1 = "banana"
    bwt1 = BWT(input_string1)
    print(f"Input string: {input_string1}")
    print("\nBWT Structure:")
    print(bwt1)
    print(f"F-vectors: {bwt1.f_vectors}")
    print(f"LF-mapping: {bwt1.lf_mapping}")
    print(f"Character counts: {bwt1.counts}")
    
    # Example 2: DNA sequence
    print("\nExample 2: DNA Sequence 'ACTGACG'")
    print("-----------------------------")
    input_string2 = "ACTGACG"
    bwt2 = BWT(input_string2)
    print(f"Input string: {input_string2}")
    print("\nBWT Structure:")
    print(bwt2)
    print(f"F-vectors: {bwt2.f_vectors}")
    print(f"LF-mapping: {bwt2.lf_mapping}")
    print(f"Character counts: {bwt2.counts}")
    
    # Example 3: Special characters
    print("\nExample 3: Special Characters 'Hello$World'")
    print("------------------------------------")
    input_string3 = "Hello$World"
    bwt3 = BWT(input_string3)
    print(f"Input string: {input_string3}")
    print("\nBWT Structure:")
    print(bwt3)
    print(f"F-vectors: {bwt3.f_vectors}")
    print(f"Character counts: {bwt3.counts}")
    
    # Example 4: Repeated characters
    print("\nExample 4: Repeated Characters 'aaaaaa'")
    print("----------------------------------")
    input_string4 = "aaaaaa"
    bwt4 = BWT(input_string4)
    print(f"Input string: {input_string4}")
    print("\nBWT Structure:")
    print(bwt4)
    print(f"F-vectors: {bwt4.f_vectors}")
    print(f"Character counts: {bwt4.counts}")
    
    # Example 5: Empty string
    print("\nExample 5: Empty String")
    print("-------------------")
    input_string5 = ""
    bwt5 = BWT(input_string5)
    print(f"Input string: '{input_string5}'")
    print("\nBWT Structure:")
    print(bwt5)
    print(f"F-vectors: {bwt5.f_vectors}")
    print(f"Character counts: {bwt5.counts}")
