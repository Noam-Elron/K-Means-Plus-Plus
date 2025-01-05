import os
import sys
from typing import List
import math
import argparse

epsilon = 0.001

class Point:
    def __init__(self, point=None):
        if point != None:
            self.vals = point
        else:
            self.vals = []
        
    def push(self, val):
        self.vals.append(val)

    def euclidean_distance(self, other) -> float:
        total = 0
        for i in range(len(self.vals)):
            total += pow((self.vals[i] - other.vals[i]), 2)
        return math.sqrt(total)

    def __add__(self, other):
        new_point = Point()
        for i in range(len(self.vals)):
            x_i = self.vals[i] + other.vals[i]
            new_point.push(x_i)
        return new_point

    def __truediv__(self, num):
        assert type(num) == int

        new_point = Point()
        for i in range(len(self.vals)):
            x_i = self.vals[i] / num
            new_point.push(x_i)
        return new_point
    
    def __repr__(self):
        return f"Point({self.vals})"
    
    def __str__(self):
        formatted_floats = list(map(lambda x: f'{x:.4f}', self.vals))
        result = ', '.join(formatted_floats)

        return result


class Cluster:
    def __init__(self, centroid):
        self.centroid: Point = centroid        
        self.total_points: Point = Point([0 for i in range(len(self.centroid.vals))]) 
        self.num_points: int = 0

    def update_centroid(self, point):
        self.total_points += point
        self.num_points += 1

    def finalize_next_centroid_pos(self) -> float:
        if self.num_points == 0:
            return 0
        prev_centroid = self.centroid
        self.centroid = self.total_points / self.num_points
        self.num_points = 0
        self.total_points = Point([0 for i in range(len(self.centroid.vals))])
        return prev_centroid.euclidean_distance(self.centroid)
    
    def __str__(self):
        return str(self.centroid)


class ClusterManager:
    def __init__(self):
        self.clusters: List[Cluster] = []

    def closest_cluster(self, point: Point) -> int:
        closest_id = 0
        closest_dist = point.euclidean_distance(self.clusters[0].centroid)
        for i in range(1, len(self.clusters)):
            dist = point.euclidean_distance(self.clusters[i].centroid)
            if dist < closest_dist:
                closest_id = i
                closest_dist = dist
        return closest_id

    def update_next_centroid_position(self, centroid_id, point):
        self.clusters[centroid_id].update_centroid(point)

    def push(self, cluster: Cluster) -> None:
        self.clusters.append(cluster)

    def __iter__(self):
        return iter(self.clusters)


def main(K, iter, input_data):
    if iter >= 1000 or iter <= 1 or type(iter) != int:
        print("Invalid maximum iteration!")
        return
    
    points: List[Point] = []
    with open(input_data, "r", encoding="utf-8") as input:
        for line in input:
            line = line.strip()
            line = line.split(",")
            line = list(map(float, line))
            point = Point(line)
            points.append(point)

            
    if K <= 1 or K >= len(points) or type(K) != int:
        print("Invalid number of clusters!")
        return
    
    manager = ClusterManager()

    for i in range(K):
        cluster = Cluster(points[i])
        manager.push(cluster)

    for i in range(iter):
        for point in points:
            nearest_cluster = manager.closest_cluster(point)
            manager.update_next_centroid_position(nearest_cluster, point)
        
        converge = True
        for cluster in manager: 
            delta = cluster.finalize_next_centroid_pos()
            if delta > epsilon:
                converge = False
        
        if converge:
            break
    
    for cluster in manager:
        print(cluster)
    
    return

def parse() -> argparse.Namespace:
    parser = argparse.ArgumentParser()
    parser.add_argument('K', type=str)
    parser.add_argument('iter', type=str, nargs='?', default="200")
    parser.add_argument('input_file', type=str)
    return parser.parse_args()

if __name__ == "__main__":
    args: argparse.Namespace = parse()
    K, iterations, filepath = args.K, args.iter, args.input_file
    
    try:
        K = int(K)
        iterations = int(iterations)
        filepath = os.path.join(os.getcwd(), filepath)
        main(K, iterations, filepath)
    except ValueError:
        print("An Error has Occurred")


