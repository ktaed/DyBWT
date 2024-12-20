from DyBWT import DyBWT
from BWT import BWT
from random import shuffle
def main():
    print("BWT and Dynamic BWT Implementation")
        
    # Example usage of DyBWT
    sequences = ["abra","cada","brac","dabra"]
    shuffle(sequences)
    dybwt = DyBWT(sequences[0])
    print(dybwt.get_originals())
    for i in sequences[1:]:
        dybwt.add(i)
        print(dybwt.get_originals())


if __name__ == "__main__":
    main()
