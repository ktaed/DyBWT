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
            return ""
            
        # Edge case: Invalid index
        least_char = min(self.f_vectors.keys())
        if index < 0 or index >= len(self.f_vectors[least_char]):
            return ""
            
        sequence = ""
        current_char = self.f_vectors[least_char][index]
        next_index = self.lf_mapping[least_char][index]
        print(least_char, current_char, next_index)
        # Edge case: Prevent infinite loops
        seen_positions = set()
        
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
        #TODO
        pass

    
    def __len__(self):
        """Returns the total length of the transformed text"""
        return sum(self.counts.values)
    
    # def __str__(self):
    #     """String representation showing BWT and original text"""
    #     bwt = self.get_bwt()
    #     original = self.get_original()
    #     return f"BWT: {bwt}\nOriginal: {original}"
    

if __name__ == "__main__":
    # Initialize with first sequence
    bwt = DyBWT("hello")
    print("\nAfter initializing with 'hello':")
    print(f"Original (0): {bwt.get_original(0)}")
    
    # Add and test second sequence
    bwt.add("world")
    print("\nAfter adding 'world':")
    print(f"Original (0): {bwt.get_original(0)}")
    print(f"Original (1): {bwt.get_original(1)}")
    
    # Add and test third sequence
    bwt.add("happy")
    print("\nAfter adding 'happy':")
    print(f"Original (0): {bwt.get_original(0)}")
    print(f"Original (1): {bwt.get_original(1)}")
    print(f"Original (2): {bwt.get_original(2)}")
    
    # Add and test fourth sequence
    bwt.add("smile")
    print("\nAfter adding 'smile':")
    print(f"Original (0): {bwt.get_original(0)}")
    print(f"Original (1): {bwt.get_original(1)}")
    print(f"Original (2): {bwt.get_original(2)}")
    print(f"Original (3): {bwt.get_original(3)}")
    
    # Add and test fifth sequence
    bwt.add("dance")
    print("\nAfter adding 'dance':")
    print(f"Original (0): {bwt.get_original(0)}")
    print(f"Original (1): {bwt.get_original(1)}")
    print(f"Original (2): {bwt.get_original(2)}")
    print(f"Original (3): {bwt.get_original(3)}")
    print(f"Original (4): {bwt.get_original(4)}")
    
    # Print final BWT string
    print("\nFinal BWT string:")
    print(bwt.get_bwt())
    
    # Print all sequences at once
    print("\nAll sequences:")
    print(bwt.get_originals())

    # print(BWT("banana"))
    # print(BWT("ananab"))
    # print(BWT("cadabra"))
