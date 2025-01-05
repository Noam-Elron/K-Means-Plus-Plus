#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define epsilon 0.001


struct PointCoords {
  double val;
  struct PointCoords* next;
};

struct Point {
  struct PointCoords* head;
  struct PointCoords* last;
  struct Point* next;
};

struct PointsList {
  struct Point* head;
  struct Point* last;
 };

struct Centroid {
  struct Point* centroid;
  struct Point* centroid_next_pos;
  struct Centroid* next;
  int num_points;
};

struct CentroidsList{
  struct Centroid* head;
};

struct Lists {
  struct PointsList **points_list_address;
  struct CentroidsList **centroids_list_address;
};

struct Lists *lists = NULL;

/*
MEMORY MANAGEMENT, FREEING ALL INITIALIZED MEMORY
 */


void free_point(struct Point** ptr) {
  struct PointCoords *prev;
  struct PointCoords *curr;

  if (*ptr == NULL) {
    return;
  }

  prev = (*ptr)->head;

  while(prev != NULL) {
    curr = prev->next;
    free(prev);
    prev = curr;
  }

  free(*ptr);
  *ptr = NULL;
}

void free_pointslist(struct PointsList** list) {
  struct Point *prev;
  struct Point* curr;

  if (list == NULL) {
    return;
  }

  prev = (*list)->head;

  while(prev != NULL) {
    curr = prev->next;
    free_point(&prev);
    prev = curr;
  }
  free(*list);
  *list = NULL;
}

void free_centroid(struct Centroid **centroid_ptr_address) {
  struct Centroid *centroid_ptr;

  if (centroid_ptr_address == NULL) {
    return;
  }
  centroid_ptr = *centroid_ptr_address;
  if (centroid_ptr == NULL) {
    return;
  }


  if (centroid_ptr->centroid != NULL) {
    free_point(&(centroid_ptr->centroid));
  }

  if (centroid_ptr->centroid_next_pos != NULL) {
    free_point(&(centroid_ptr->centroid_next_pos));
  }
  free(*centroid_ptr_address);
  centroid_ptr = NULL;
  *centroid_ptr_address = NULL;
}

void free_centroidslist(struct CentroidsList **centroid_list_ptr_address) {
  struct Centroid *prev;
  struct Centroid* curr;
  struct CentroidsList *centroid_list_ptr;
  if (centroid_list_ptr_address == NULL) {
    return;
  }

  centroid_list_ptr = *centroid_list_ptr_address;
  if (centroid_list_ptr == NULL) {
    return;
  }


  prev = centroid_list_ptr->head;
  while(prev != NULL) {
    curr = prev->next;
    free_centroid(&prev);
    prev = curr;
  }
  free(*centroid_list_ptr_address);
  centroid_list_ptr = NULL;
  *centroid_list_ptr_address = NULL;
}

void free_all_memory() {
  if (lists == NULL) {
    return;
  }

  if (lists->points_list_address != NULL && *(lists->points_list_address) != NULL) {
    free_pointslist(lists->points_list_address);
  }
  if (lists->centroids_list_address != NULL ) {
    free_centroidslist(lists->centroids_list_address);
  }
  free(lists);
  lists = NULL;
}


/*
UTILITY FUNCTIONS FOR PRINTING AND DIAGNOSTIC
 */

void print_point(struct Point* ptr) {
  struct PointCoords* ptr_coords = ptr->head;

  while(ptr_coords != NULL) {
    if(ptr_coords == ptr->last) {
      printf("%.4f", ptr_coords->val);
    } else {
      printf("%.4f,", ptr_coords->val);
    }
    ptr_coords = ptr_coords->next;
  }
  printf("\n");
}

void print_points(const struct PointsList* list) {
  struct Point* ptr = list->head;

  while(ptr != NULL) {
    print_point(ptr);
    ptr = ptr->next;
  }
}

void print_centroids(struct CentroidsList* list) {
  struct Centroid* centroid_pointer = list->head;
  struct Point* point_pointer = NULL;

  while(centroid_pointer != NULL) {
    point_pointer = centroid_pointer->centroid;
    print_point(point_pointer);
    centroid_pointer = centroid_pointer->next;
  }
}


/*
  COORDINATE AND POINT FUNCTIONS
*/


void add_coord(struct Point* point, double val) {
  struct PointCoords* ptr = malloc(sizeof(struct PointCoords));

  if (ptr == NULL) {
    free_all_memory();
    printf("An Error has Occurred\n");
    exit(EXIT_FAILURE);
  }

  ptr->val = val;
  ptr->next = NULL;

  if (point->head == NULL) {
    point->head = ptr;
    point->last = ptr;
  }
  else {
    point->last->next = ptr;
    point->last = ptr;
  }
}

void add_point(struct PointsList* list) {
  struct Point* ptr = (struct Point*)malloc(sizeof(struct Point));

  if (ptr == NULL) {
    free_all_memory();
    printf("An Error has Occurred\n");
    exit(EXIT_FAILURE);
  }

  ptr->head = NULL;
  ptr->last = NULL;
  ptr->next = NULL;

  if (list->head == NULL) {
    list->head = ptr;
    list->last = ptr;
  }
  else {
    list->last->next = ptr;
    list->last = ptr;
  }
}

struct Point* deep_copy_point(struct Point* point) {
  struct Point* deep_copy = malloc(sizeof(struct Point));
  struct PointCoords* cur_coordinate;

  if (deep_copy == NULL) {
    free_all_memory();
    printf("An Error has Occurred\n");
    exit(EXIT_FAILURE);
  }

  deep_copy->head = NULL;
  deep_copy->last = NULL;
  deep_copy->next = NULL;
  cur_coordinate = point->head;

  while(cur_coordinate != NULL) {
    add_coord(deep_copy, cur_coordinate->val);
    cur_coordinate = cur_coordinate->next;
  }
  
  return deep_copy;
}

void point_addition(struct Point* point, struct Point* other) {
  /*
  Set point coordinates to be the sum of point and other's coordinates.
  Precondition: point is the centroid next-in-line
   */
  struct PointCoords* point_coord = point->head;
  struct PointCoords* other_coord = other->head;

  while (point_coord != NULL && other_coord != NULL) {
    point_coord->val = point_coord->val + other_coord->val;
    point_coord = point_coord->next;
    other_coord = other_coord->next;
  }
}

void point_division(struct Point* point, int divisor) {
  /*
  Set point coordinates to be the division of point's coordinates by divisior.
  Precondition: point is the centroid next-in-line
   */
  struct PointCoords* point_coord = point->head;

  while(point_coord != NULL) {
    point_coord->val = point_coord->val / divisor;
    point_coord = point_coord->next;
  }
}

double euclidean_distance(struct Point* point, struct Point* other) {
  double total = 0.0;
  struct PointCoords* point_coord = point->head;
  struct PointCoords* other_coord = other->head;

  while(point_coord != NULL) {
    total += pow(point_coord->val - other_coord->val, 2);
    point_coord = point_coord->next;
    other_coord = other_coord->next;
  }
  return sqrt(total);
}

/*
CENTROID FUNCTIONS
*/

struct CentroidsList* initialize_centroids(const struct PointsList* points_list, const int num_centroids) {
  struct Point* traversal_pointer = points_list->head;
  struct Point* curr_point;
  struct CentroidsList* centroid_list_ptr = malloc(sizeof(struct CentroidsList));
  struct Centroid* prev_centroid = malloc(sizeof(struct Centroid));
  struct Centroid* curr_centroid;
  int i;

  if (centroid_list_ptr == NULL || prev_centroid == NULL) {
    free_all_memory();
    printf("An Error has Occurred\n");
    exit(EXIT_FAILURE);
  }

  curr_point = deep_copy_point(traversal_pointer);
  prev_centroid->centroid = curr_point;
  prev_centroid->centroid_next_pos = NULL;
  prev_centroid->num_points = 0;

  centroid_list_ptr->head = prev_centroid;
  
  traversal_pointer = traversal_pointer->next;

  for (i = 1; i < num_centroids; i++) {
    curr_centroid = malloc(sizeof(struct Centroid));

    if (curr_centroid == NULL) {
      free_all_memory();
      printf("An Error has Occurred\n");
      exit(EXIT_FAILURE);
    }

    curr_point = deep_copy_point(traversal_pointer);
    prev_centroid->next = curr_centroid;
    prev_centroid->centroid_next_pos = NULL;
    curr_centroid->centroid = curr_point;
    curr_centroid->num_points = 0;
    prev_centroid = curr_centroid;
    traversal_pointer = traversal_pointer->next;
  }

  prev_centroid->centroid_next_pos = NULL;
  prev_centroid->next = NULL;
  return centroid_list_ptr;
}

void update_centroid(struct Centroid* centroid, struct Point* point) {
  if (centroid->centroid_next_pos == NULL) {
    centroid->centroid_next_pos = deep_copy_point(point);
  }
  else {
    point_addition(centroid->centroid_next_pos, point);
  }
  centroid->num_points++;
}


double finalize_next_centroid_pos(struct Centroid **centroid_ptr_address) {
  struct Point* prev_centroid_pos;
  struct Point* prev_centroid;
  struct Centroid *centroid_ptr;
  double distance;

  if (centroid_ptr_address == NULL) {
    return 0.0;
  }

  if ((*centroid_ptr_address)->num_points == 0) {
    return 0.0;
  }

  centroid_ptr = *centroid_ptr_address;
  prev_centroid_pos = deep_copy_point(centroid_ptr->centroid_next_pos);
  point_division(centroid_ptr->centroid_next_pos, centroid_ptr->num_points);

  prev_centroid = centroid_ptr->centroid;
  
  centroid_ptr->centroid = centroid_ptr->centroid_next_pos;  
  centroid_ptr->centroid_next_pos = NULL;
  centroid_ptr->num_points = 0;

  distance = euclidean_distance(centroid_ptr->centroid, prev_centroid);

  free_point(&prev_centroid_pos);
  free_point(&prev_centroid);
  return distance;
}

/*
CENTROID MANAGEMENT FUNCTIONS
*/

struct Centroid* closest_centroid(struct CentroidsList* centroids_list_ptr, struct Point* point_ptr) {
  struct Centroid* curr_centroid = centroids_list_ptr->head;
  double closest_dist = euclidean_distance(point_ptr, curr_centroid->centroid);
  struct Centroid* closest_centroid = NULL;

  while(curr_centroid != NULL) {
    double dist = euclidean_distance(point_ptr, curr_centroid->centroid);
    if (dist <= closest_dist) {
      closest_dist = dist;
      closest_centroid = curr_centroid;
    }
    curr_centroid = curr_centroid->next;
  }

   return closest_centroid;
}



/*
PARSE INPUT
 */
struct PointsList *parse() {
  struct PointsList* points_list_ptr = malloc(sizeof(struct PointsList));
  char line[1024];
  char* ptr;
  double value = 0;

  if (points_list_ptr == NULL) {
    free_all_memory();
    printf("An Error has Occurred\n");
    exit(EXIT_FAILURE);
  }

  points_list_ptr->head = NULL;
  points_list_ptr->last = NULL;

  while (fgets(line, sizeof(line), stdin)) {
    add_point(points_list_ptr);

    ptr = line;
    while (sscanf(ptr, "%lf", &value) == 1) {
        add_coord(points_list_ptr->last, value);
        while (*ptr != ',' && *ptr != '\n' && *ptr != '\0') {
          ptr++;
        }
        if (*ptr == ',' || *ptr == '\n') {
          ptr++;
        }
    }
  }
  return points_list_ptr;
}



int main(int argc, char* argv[]) {
  int iter;
  int K;
  char* iter_endpoint;
  char* K_endpoint;
  struct PointsList* points_list_ptr;
  int num_points;
  struct CentroidsList* centroid_list_ptr;
  struct Centroid* curr_centroid;
  struct Centroid* closest_cluster;
  struct Point* curr_point;
  int i;
  int j;
  int m;
  int converge;
  double delta;

  lists = malloc(sizeof(struct Lists));

  if (lists == NULL) {
    printf("An Error has Occurred\n");
    exit(EXIT_FAILURE);
  }

  lists->points_list_address = NULL;
  lists->centroids_list_address = NULL;

  if (argc == 2) {
    iter = 200;
    K = strtol(argv[1], &K_endpoint, 10);
  }
  else if (argc == 3) {
    K = strtol(argv[1], &K_endpoint ,10);
    iter = strtol(argv[2], &iter_endpoint, 10);
  }


  if ((*iter_endpoint) != '\0' || iter >= 1000 || iter <= 1) {
    printf("Invalid maximum iteration!\n");
    free_all_memory();
    exit(EXIT_FAILURE);
  }

  points_list_ptr = parse();
  lists->points_list_address = &points_list_ptr;

  num_points = 0;
  curr_point = points_list_ptr->head;
  while (curr_point != NULL) {
    num_points++;
    curr_point = curr_point->next;
  }

  if ((*K_endpoint) != '\0' ||K >= num_points || K <= 1) {
    printf("Invalid number of clusters!\n");
    free_all_memory();
    exit(EXIT_FAILURE);
  }

  /* Need to change initialize centroids to implementation required in the pdf */
  centroid_list_ptr = initialize_centroids(points_list_ptr, K);
  lists->centroids_list_address = &centroid_list_ptr;

  curr_centroid = NULL;
  converge = 1;
  closest_cluster = NULL;
  delta = 0.0;

  /* Perform K-Means iter times */
  for (i = 0; i < iter; i++) {
    /* Go over all points to assign the closest cluster*/
    curr_point = points_list_ptr->head;
    for (j = 0; j < num_points; j++) {
      closest_cluster = closest_centroid(centroid_list_ptr, curr_point);
      update_centroid(closest_cluster, curr_point);
      curr_point = curr_point->next;
    }
    converge = 1;
    /* Go over clusters to check for convergence */
    curr_centroid = centroid_list_ptr->head;
    for (m = 0; m < K; m++) {
      delta = finalize_next_centroid_pos(&curr_centroid);
      curr_centroid = curr_centroid->next;

      if (delta > epsilon) {
        converge = 0;
      }
    }

    if (converge) {
      break;
    }

  }
  print_centroids(centroid_list_ptr);

  free_all_memory();
  exit(EXIT_SUCCESS);
}

