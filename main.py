from DyBWT import DyBWT
from BWT import BWT

def main():
    print("BWT and Dynamic BWT Implementation")
    
    # Example usage of BWT
    bwt = BWT("example_string")
    transformed = bwt.transform()
    print(f"BWT transformed: {transformed}")
    
    # Example usage of DyBWT
    dybwt = DyBWT("initial_string")
    dybwt.insert("new_substring", 5)
    result = dybwt.get_bwt()
    print(f"DyBWT result: {result}")

if __name__ == "__main__":
    main()
