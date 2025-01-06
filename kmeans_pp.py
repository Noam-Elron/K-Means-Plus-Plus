import argparse
import os
import numpy as np
import pandas as pd

def kmeansplusplus():
    pass



def parse() -> argparse.Namespace:
    parser = argparse.ArgumentParser()
    parser.add_argument('K', type=str)
    parser.add_argument('iter', type=str, nargs='?', default="300")
    parser.add_argument('epsilon', type=str)
    parser.add_argument('file_name_1', type=str)
    parser.add_argument('file_name_2', type=str)
    return parser.parse_args()



if __name__ == "__main__":
    args: argparse.Namespace = parse()
    K, iterations, epsilon, file_name_1, file_name_2 = args.K, args.iter, args.epsilon, args.file_name_1, args.file_name_2
    
    try:
        K = int(K)
        iterations = int(iterations)
        filepath1 = os.path.join(os.getcwd(), file_name_1)
        filepath2 = os.path.join(os.getcwd(), file_name_2)

    except ValueError:
        print("An Error has Occurred")
