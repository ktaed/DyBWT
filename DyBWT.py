from BWT import BWT

class DyBWT(BWT):
    def __init__(self, initial_string):
        super().__init__(initial_string)
        # Initialize other necessary data structures
    
    def add(self, substring):
        temp_bwt = BWT(substring)
        all_keys = set(self.lf_mapping.keys()) | set(temp_bwt.lf_mapping.keys())
        
        for key in sorted(all_keys):
            if key in self.lf_mapping and key in temp_bwt.lf_mapping:
                for i in range(len(temp_bwt.lf_mapping[key])):
                    cur_char = temp_bwt.f_vectors[key][i]
                    try:
                        offset = self.counts[cur_char]
                    except:
                        offset = 0
                    temp_bwt.lf_mapping[key][i] += offset
            elif key in temp_bwt.lf_mapping:
                for i in range(len(temp_bwt.lf_mapping[key])):
                    cur_char = temp_bwt.f_vectors[key][i]
                    try:
                        offset = self.counts[cur_char]
                    except:
                        offset = 0
                    temp_bwt.lf_mapping[key][i] += offset

            if key in self.f_vectors and key in temp_bwt.f_vectors:
                self.f_vectors[key] += temp_bwt.f_vectors[key]
                self.lf_mapping[key] += temp_bwt.lf_mapping[key]
            elif key in temp_bwt.f_vectors:
                self.f_vectors[key] = temp_bwt.f_vectors[key]
                self.lf_mapping[key] = temp_bwt.lf_mapping[key]
        for key in all_keys:
            if key not in self.counts:
                self.counts[key] = temp_bwt.counts[key]
            elif key in self.counts and key in temp_bwt.counts:
                self.counts[key] += temp_bwt.counts[key]

    def get_bwt(self):
        """Returns the current BWT string"""
        result = []
        for key in sorted(self.f_vectors.keys()):
            result.append(self.f_vectors[key])
        return ''.join(result)
    
    def get_original(self, index):
        """
        Reconstructs original string starting from given index
        Args:
            index: Starting position in the BWT
        Returns:
            Original string or empty string if invalid
        """
        # Edge case: Empty BWT
        if not self.f_vectors:
            return "Empty BWT. Try adding a sequence first."
            
        # Edge case: Invalid index
        least_char = min(self.f_vectors.keys())
        if index < 0 or index >= len(self.f_vectors[least_char]):
            return "Out of starting index range. Try using search() instead."
            
        sequence = ""
        current_char = self.f_vectors[least_char][index]
        next_index = self.lf_mapping[least_char][index]
        
        try:
            while True:
                sequence += current_char           
                next_char = self.f_vectors[current_char][next_index]
                next_index = self.lf_mapping[current_char][next_index]
                current_char = next_char
                if next_char == '$':
                    break
            return sequence[::-1]
        except (KeyError, IndexError):
            # Edge case: Any unexpected access errors
            return ""
            
        return sequence[::-1]
    def get_originals(self):
        ret_lst = []
        least_char = min(self.f_vectors.keys())
        for i in range(len(self.f_vectors[least_char])):
            ret_lst.append(self.get_original(i))
        return ret_lst
        
    def search(self, pattern):
        """
        Finds all occurrences of a pattern in the BWT
        Args:
            pattern: String to search for
        Returns:
            List of indices where pattern occurs
        """
        # Edge cases
        if not pattern or not self.f_vectors:
            return []
            
        # Start with last character of pattern
        current_char = pattern[-1]
        pattern = pattern[::-1]
        if current_char not in self.f_vectors:
            return []
        least_char = min(self.f_vectors.keys())
        offset = self.counts[least_char]
        # offset = 0  
        for i in sorted(self.f_vectors.keys())[1:]:
            if i < pattern[-1]:
                offset += self.counts[i]
            # else:
            #     break   
        # Initialize range to all positions of last character
        matches = list(range(len(self.f_vectors[current_char])))
        if len(pattern) == 1:
            return [x+offset for x in matches] 
        new_matches = []
        for i in matches:
            sequence = pattern[0]
            current_char = self.f_vectors[pattern[0]][i]
            next_index = self.lf_mapping[pattern[0]][i]

            for j in range(1, len(pattern)):
                sequence += current_char
                next_char = self.f_vectors[current_char][next_index]
                next_index = self.lf_mapping[current_char][next_index]
                current_char = next_char
                if sequence[-1] != pattern[j]:
                    break
            if sequence == pattern:
                offset = 0
                for j in sorted(self.f_vectors.keys()):
                    if j < pattern[-1]:
                        offset += self.counts[j]

                new_matches.append(i + offset)

        return new_matches

    def __len__(self):
        """Returns the total length of the transformed text"""
        return sum(self.counts.values)
    
    # def __str__(self):
    #     """String representation showing BWT and original text"""
    #     bwt = self.get_bwt()
    #     original = self.get_originals()
    #     return f"BWT: {bwt}\nOriginal: {original}"
    

if __name__ == "__main__":
    # Test case 1: Basic Single Sequence
    print("\n=== Test Case 1: Basic Single Sequence ===")
    bwt = DyBWT("banana")
    print("Initial text: 'banana'")
    print(bwt)
    print(f"Search 'ana': {bwt.search('ana')}")
    print(f"Search 'na': {bwt.search('na')}")
    print(f"Search 'ban': {bwt.search('ban')}")
    print(f"Search 'a': {bwt.search('a')}")

    # Test case 2: Two Sequences
    print("\n=== Test Case 2: Two Sequences ===")
    bwt.add("ananab")
    print("After adding 'ananab':")
    print(bwt)
    print(f"Search 'ana': {bwt.search('ana')}")  # Should find in both sequences
    print(f"Search 'ban': {bwt.search('ban')}")  # Should find in first sequence
    print(f"Search 'nab': {bwt.search('nab')}")  # Should find in second sequence
    print(f"Search 'a': {bwt.search('a')}")      # Should find multiple occurrences

    # Test case 3: Multiple Sequences
    print("\n=== Test Case 3: Multiple Sequences ===")
    bwt = DyBWT("hello")
    print("\nInitial: 'hello'")
    print(bwt)
    print(f"Search 'll': {bwt.search('ll')}")
    print(f"Search 'lo': {bwt.search('lo')}")
    
    bwt.add("world")
    print("\nAfter adding 'world':")
    print(bwt)
    print(f"Search 'l': {bwt.search('l')}")    # Should find in both sequences
    print(f"Search 'o': {bwt.search('o')}")    # Should find in both sequences
    print(f"Search 'ld': {bwt.search('ld')}")  # Should find in second sequence
    
    bwt.add("happy")
    print("\nAfter adding 'happy':")
    print(bwt)
    print(f"Search 'py': {bwt.search('py')}")
    print(f"Search 'p': {bwt.search('p')}")
    print(f"Search 'y': {bwt.search('y')}")

    # Test case 4: Edge Cases
    print("\n=== Test Case 4: Edge Cases ===")
    bwt = DyBWT("test")
    print(bwt)
    print(f"Empty pattern: {bwt.search('')}")
    print(f"Pattern not in text: {bwt.search('xyz')}")
    print(f"Pattern longer than text: {bwt.search('testing')}")
    print(f"Single char not in text: {bwt.search('k')}")

    # Test case 5: Special Characters
    print("\n=== Test Case 5: Special Characters ===")
    bwt = DyBWT("test.txt")
    bwt.add("data.csv")
    print(bwt)
    print(f"Search '.': {bwt.search('.')}")      # Should find in both sequences
    print(f"Search 'txt': {bwt.search('txt')}")  # Should find in first sequence
    print(f"Search 'csv': {bwt.search('csv')}")  # Should find in second sequence
    print(f"Search 't': {bwt.search('t')}")      # Should find multiple occurrences

    # Test case 6: Overlapping Patterns
    print("\n=== Test Case 6: Overlapping Patterns ===")
    bwt = DyBWT("mississippi")
    bwt.add("mississippi")
    print(bwt)
    print("Searching in 'mississippi' (twice):")
    print(f"Search 'issi': {bwt.search('issi')}")  # Should find overlapping matches
    print(f"Search 'ssi': {bwt.search('ssi')}")
    print(f"Search 'si': {bwt.search('si')}")
    print(f"Search 'i': {bwt.search('i')}")        # Should find all occurrences

    # Print BWT state for verification
    print("\n=== Final BWT State ===")
    print(f"BWT string: {bwt.get_bwt()}")
    print(f"All sequences: {bwt.get_originals()}")
    print(bwt)
