class BWT:
    def __init__(self, input_string):
        self.input = input_string + '$'  # Add end-of-string marker
        self.f_vectors, self.lf_vectors = self._build_bwt()
        self.counts = {x:len(self.lf_vectors[x]) for x in self.lf_vectors.keys()}
    def __str__(self):
        ret_str = f"{'L':<5} {'F':<5} {'Map':<5}\n"
        for sigma in sorted(self.f_vectors.keys()):
            for i, char in enumerate(self.f_vectors[sigma]):
                try:
                    ret_str += f"{char:5s} {sigma:5s} {self.lf_vectors[sigma][i]}\n"
                except:
                    ret_str += f"{char:5s} {sigma:5s} {self.lf_vectors[sigma][0]}\n"
                    # print("Fucked up", self.lf_vectors[sigma])
        return ret_str

    def _build_bwt(self):
        lf_vectors = {}
        f_vectors = {}
        char_counts = {}
        rotations = sorted(self.input[i:] + self.input[:i] for i in range(len(self.input)))
        for rotation in rotations:
            if rotation[0] not in f_vectors:
                f_vectors[rotation[0]] = rotation[-1]
                lf_vectors[rotation[0]] = []
            else:
                f_vectors[rotation[0]] += rotation[-1]
            if rotation[-1] not in char_counts:
                char_counts[rotation[-1]] = 0
            lf_vectors[rotation[0]].append((rotation[-1], char_counts[rotation[-1]]))
            char_counts[rotation[-1]] += 1
        return f_vectors, lf_vectors


# Demonstration code
if __name__ == "__main__":
    print("Demonstrating Burrows-Wheeler Transform")
    print("---------------------------------------")
    
    # Example 1
    input_string1 = "banana"
    bwt1 = BWT(input_string1)
    print(bwt1)
    print()
    
    # Example 2
    input_string2 = "ACTGACG"
    bwt2 = BWT(input_string2)
    print(bwt2)
    print()

