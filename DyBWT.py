from BWT import BWT

class DyBWT(BWT):
    def __init__(self, initial_string):
        super().__init__(initial_string)
        # Initialize other necessary data structures
    
    def add(self, substring):
        # Implement insertion logic 

        #TODO fix the offet issue
        temp_bwt = BWT(substring)
        f_len = { x: len(self.f_vectors[x]) if x in self.f_vectors else 0 for x in set(self.f_vectors.keys()).union(set(temp_bwt.f_vectors.keys()))}

        for sigma in temp_bwt.lf_vectors.keys():
            for c, i in temp_bwt.lf_vectors[sigma]:
                try:
                    self.lf_vectors[sigma].append((c, i+f_len[c]))
                except:
                    self.lf_vectors[sigma] = [(c, i)]
        for sigma in temp_bwt.f_vectors.keys():
            try:    
                self.f_vectors[sigma] += temp_bwt.f_vectors[sigma]
            except:
                self.f_vectors[sigma] = temp_bwt.f_vectors[sigma]


    def get_bwt(self):
        """Returns the current BWT string"""
        result = []
        for key in sorted(self.f_vectors.keys()):
            result.append(self.f_vectors[key])
        return ''.join(result)
    
    def get_original(self,index=-1):
        """Reconstructs and returns the original text"""
        if not self.f_vectors:
            return ""
        least_char = min(self.f_vectors.keys())
        sequences = []                          

        if index >=0 and index < len(self.f_vectors[least_char]):
            sequence = ""
            last_char = least_char
            current_char = self.f_vectors[least_char][index]
            while current_char != '$':
                sequence += current_char
                next_index = self.lf_vectors[last_char][index][1]
                last_char = current_char
                current_char = self.f_vectors[last_char][next_index]
                index = next_index

            sequences.append(sequence[::-1])
        elif index == -1:
            indices = range( 0, len(self.f_vectors[least_char]))
            # print(indices)
            for index in indices:
                sequence = ""
                last_char = least_char
                current_char = self.f_vectors[least_char][index]
                while current_char != '$':
                    sequence += current_char
                    next_index = self.lf_vectors[last_char][index][1]
                    last_char = current_char
                    current_char = self.f_vectors[last_char][next_index]
                    index = next_index
                    # print(index,sequence, next_index, current_char, last_char)
                sequences.append(sequence[::-1])

        
        return '\t'.join(sequences)
    
    def search(self, pattern):
        """Returns the number of occurrences of the pattern"""
        if not pattern:
            return 0
            
        current_char = pattern[-1]
        if current_char not in self.f_vectors:
            return 0
            
        # Find initial range
        matches = len(self.f_vectors[current_char])
        
        # Iterate through pattern backwards
        for char in reversed(pattern[:-1]):
            if char not in self.f_vectors:
                return 0
            # Update matches using LF-mapping
            new_matches = 0
            for mapping in self.lf_vectors[char]:
                if mapping[1] <= matches:
                    new_matches += 1
            matches = new_matches
            
        return matches
    
    def __len__(self):
        """Returns the total length of the transformed text"""
        return sum(len(v) for v in self.f_vectors.values())
    
    def __str__(self):
        """String representation showing BWT and original text"""
        bwt = self.get_bwt()
        original = self.get_original()
        return f"BWT: {bwt}\nOriginal: {original}"
    

if __name__ == "__main__":
    # Test case 1: Basic operations and search
    print("\n=== Test Case 1: Basic Operations and Search ===")
    print("Regular BWT of 'banana':")
    
    bwt = DyBWT("banana")
    print("\nInitial DyBWT of 'banana':")
    print(f"BWT string: {bwt.get_bwt()}")
    print(f"Original (first sequence): {bwt.get_original(0)}")
    print(f"Original (all sequences): {bwt.get_original(-1)}")
    print(f"Search 'ana': {bwt.search('ana')} occurrences")
    print(f"Search 'na': {bwt.search('na')} occurrences")
    print(f"Search 'ban': {bwt.search('ban')} occurrences")
    
    print("\nRegular BWT of 'ananab':")
    
    bwt.add("ananab")
    print("\nAfter adding 'ananab':")
    print(f"BWT string: {bwt.get_bwt()}")
    print(f"Original (sequence 0): {bwt.get_original(0)}")
    print(f"Original (sequence 1): {bwt.get_original(1)}")
    print(f"Original (all sequences): {bwt.get_original(-1)}")
    print(f"Search 'ana': {bwt.search('ana')} occurrences")
    print(f"Search 'nab': {bwt.search('nab')} occurrences")
    print(f"Search 'ba': {bwt.search('ba')} occurrences")
    print(f"Search 'a': {bwt.search('a')} occurrences")
    print("\nFull BWT representation:")
    print(bwt)

    # Test case 2: Edge case searches
    print("\n=== Test Case 2: Edge Case Searches ===")
    print(f"Search empty string: {bwt.search('')} occurrences")
    print(f"Search non-existent pattern 'xyz': {bwt.search('xyz')} occurrences")
    print(f"Search pattern longer than text 'bananananana': {bwt.search('bananananana')} occurrences")
