import argparse
import os
import numpy as np
import pandas as pd
import math
import mykmeanssp 
from typing import List, Union


def kmeansplusplus(K: int, dataframe: pd.DataFrame) -> List[List[float]]:
    """
    Method described in HW2 to initialize the first K centroids

    Parameters
    ----------
    K : int
        Number of centroids to be initialized
    dataframe : pd.DataFrame
        Dataframe of all points that we are given (two files were given and were inner joined and sorted (essentially just in each row of the first point coordinates the last
        n-1 points in the second input file in the corresponding row(they have same keys on the first column)))

    Returns
    -------
    List[List[float]]
        _description_
    """
    #print(dataframe)
    #print()
    #print()
    #print()
    np.random.seed(1234)
    centroids = []
    rows = dataframe.shape[0]
    initial_centroid_index = np.random.choice(list(range(rows)))
    #print(f"Initial Centroid Index: {initial_centroid_index}")
    initial_centroid = dataframe.iloc[initial_centroid_index].values.tolist()
    #print(f"Initial Centroid: {initial_centroid}")
    centroids.append(initial_centroid)
    dataframe.drop([initial_centroid_index], axis=0, inplace=True)
    
    while len(centroids) < K:
        total_dist = 0
        distances = []
        # Each time we remove a row so we need to update the number of rows.
        rows = rows - 1

        for i, row in enumerate(dataframe.values):
            #print(f"Calculating closest centroid to Row {i}: {row}")
            closest_centroid_distance = closest_cluster_distance(centroids, row)
            distances.append(closest_centroid_distance)
            total_dist += closest_centroid_distance
        
        probabilities = calc_probabilities(distances, total_dist)
        #print(f"len probablities: {len(probabilities)}, rows: {rows}")
        centroid_index = np.random.choice(list(range(rows)), p=probabilities) 
        centroid = dataframe.iloc[centroid_index].values.tolist()
        centroids.append(centroid)
        dataframe.drop([centroid_index], axis=0, inplace=True)
    
    return centroids

        

def calc_probabilities(distances_list, total_distances):
    """

    Parameters
    ----------
    distances_list : List[List[float]]
         List of distances of each point from its nearest centroid, index of point based on row in dataframe
    total_distances : float
        Sum of distances list
    
    Returns
    ----------
    List[float]
        List of probability of each point to get chosen as the next initial centroid.
    """
    probabilities = []

    for i in range(len(distances_list)):
        probabilities.append(distances_list[i]/total_distances)
    return probabilities

def read_files(filepath1: str, filepath2: str) -> pd.DataFrame:
    """

    Parameters
    ----------
    distances_list : _type_
        _description_
    total_distances : _type_
        _description_
        
    """
    df_file1: pd.DataFrame = pd.read_csv(filepath1, header=None)
    df_file2: pd.DataFrame = pd.read_csv(filepath2, header=None)
    inner_join_df: pd.DataFrame = pd.merge(df_file1, df_file2, how='inner', left_on=0, right_on=0)
    
    inner_join_df.sort_values(by=0, inplace=True)    
    return inner_join_df
    
def parse() -> argparse.Namespace:
    """

    Parameters
    ----------
    distances_list : _type_
        _description_
    total_distances : _type_
        _description_
        
    """
    parser = argparse.ArgumentParser()
    parser.add_argument('K', type=str)
    parser.add_argument('iter', type=str, nargs='?', default="300")
    parser.add_argument('epsilon', type=str)
    parser.add_argument('file_name_1', type=str)
    parser.add_argument('file_name_2', type=str)
    return parser.parse_args()

def euclidean_distance(point, other) -> float:
    """

    Parameters
    ----------
    distances_list : _type_
        _description_
    total_distances : _type_
        _description_
        
    """
    #print(f"Other: {other}")
    total = 0
    for i in range(len(point)):
        total += pow((point[i] - other[i]), 2)
    return math.sqrt(total)

def closest_cluster_distance(found_centroids, point) -> int:
    """

    Parameters
    ----------
    found_centroids : List[List[float]]
        List of all currently initialized centroids
    point : List[float]
        Point that we are currently checking the distance between it and its nearest current centroid

    Returns
    -------
    int
        Distance between point and the nearest centroid of those that have been discovered so far.
    """
    closest_dist = euclidean_distance(point, found_centroids[0])
    for i in range(1, len(found_centroids)):
        dist = euclidean_distance(point, found_centroids[i])
        if dist < closest_dist:
            closest_dist = dist
    return closest_dist



def main():
    args: argparse.Namespace = parse()
    K, iterations, epsilon, file_name_1, file_name_2 = args.K, args.iter, args.epsilon, args.file_name_1, args.file_name_2
    
    try:
        K = int(K)
        iterations = int(iterations)
        epsilon = float(epsilon)
        filepath1 = os.path.join(os.getcwd(), file_name_1)
        filepath2 = os.path.join(os.getcwd(), file_name_2)

    except ValueError:
        print("An Error has Occurred")
    
    if iterations >= 1000 or iterations <= 1 or type(iterations) != int:

        print("Invalid maximum iteration!")
        return

    points_dataframe = read_files(file_name_1, file_name_2)
    num_points = points_dataframe.shape[0]
    
    if K <= 1 or K >= num_points or type(K) != int:
        print("Invalid number of clusters!")
        return

    centroids = kmeansplusplus(K, points_dataframe)
    points = points_dataframe.astype(float).values.tolist()
    print(f"Initial centroids: {centroids}")
    final_centroids = mykmeanssp.fit(points, centroids, K, iterations, epsilon)
    
    print(f"Final centroids: {final_centroids}")
    return 


if __name__ == "__main__":
    main()




