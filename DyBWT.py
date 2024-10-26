from BWT import BWT

class DyBWT(BWT):
    def __init__(self, initial_string):
        super().__init__(initial_string)
        # Initialize other necessary data structures
    
    def add(self, substring):
        # Implement insertion logic
        temp_bwt = BWT(substring)
        for sigma in temp_bwt.f_vectors.keys():
            for map in temp_bwt.lf_vectors[sigma]:
                self.lf_vectors[sigma].append((map[0], map[1]+len(self.f_vectors[sigma])))
            self.f_vectors[sigma] += temp_bwt.f_vectors[sigma]
        print(self)
    
    
    def get_bwt(self):
        # Return the current BWT
        pass

if __name__ == "__main__":
    bwt = DyBWT("banana")
    print(bwt)
    bwt.add("bana")

