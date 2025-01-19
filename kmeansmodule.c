#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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
    printf("1 :( \n");
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
    printf("2 :( \n");
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

void add_centroid(struct CentroidsList* list, struct Point* point) {
  struct Centroid* centroid = (struct Centroid*)malloc(sizeof(struct Centroid));

  if (centroid == NULL) {
    free_all_memory();
    printf("3 :( \n");
    printf("An Error has Occurred\n");
    exit(EXIT_FAILURE);
  }

  centroid->centroid = point;
  centroid->centroid_next_pos = NULL;
  centroid->num_points = 0;

  if (list->head == NULL) {
    list->head = centroid;
  }
  else {
    centroid->next = list->head;
    list->head = centroid;
  }
}

struct Point* deep_copy_point(struct Point* point) {
  struct Point* deep_copy = malloc(sizeof(struct Point));
  struct PointCoords* cur_coordinate;

  if (deep_copy == NULL) {
    free_all_memory();
    printf("4 :( \n");
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
  struct Centroid* curr_centroid_ptr;
  int i;

  if (centroid_list_ptr == NULL || prev_centroid == NULL) {
    free_all_memory();
    printf("5 :( \n");
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
    curr_centroid_ptr = malloc(sizeof(struct Centroid));

    if (curr_centroid_ptr == NULL) {
      free_all_memory();
      printf("6 :( \n");
      printf("An Error has Occurred\n");
      exit(EXIT_FAILURE);
    }

    curr_point = deep_copy_point(traversal_pointer);
    prev_centroid->next = curr_centroid_ptr;
    prev_centroid->centroid_next_pos = NULL;
    curr_centroid_ptr->centroid = curr_point;
    curr_centroid_ptr->num_points = 0;
    prev_centroid = curr_centroid_ptr;
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
  struct Centroid* curr_centroid_ptr = centroids_list_ptr->head;
  double closest_dist = euclidean_distance(point_ptr, curr_centroid_ptr->centroid);
  struct Centroid* closest_centroid = NULL;

  while(curr_centroid_ptr != NULL) {
    double dist = euclidean_distance(point_ptr, curr_centroid_ptr->centroid);
    if (dist <= closest_dist) {
      closest_dist = dist;
      closest_centroid = curr_centroid_ptr;
    }
    curr_centroid_ptr = curr_centroid_ptr->next;
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
    printf("7 :( \n");
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







struct PointsList* unpack_points_list(PyObject *points_list_py_ptr) {
  /* Assumes valid Python List Object is passed to function, meaning error checks for type should be external */
  Py_ssize_t num_points;
  Py_ssize_t num_coords;
  Py_ssize_t i;
  Py_ssize_t j;
  PyObject *py_point;
  PyObject *py_coord;
  double coord;
  struct PointsList* points_list_ptr = malloc(sizeof(struct PointsList));

  if (points_list_ptr == NULL) {
    printf("8 :( \n");
    printf("An Error has Occurred\n");
    exit(EXIT_FAILURE);
  }

  points_list_ptr->head = NULL;
  points_list_ptr->last = NULL;

  num_points = PyList_Size(points_list_py_ptr);

  for (i = 0; i < num_points; i++) {
    add_point(points_list_ptr);
    py_point = PyList_GetItem(points_list_py_ptr, i);
    if (!PyList_Check(py_point)) {
      printf("9 :( \n");
      printf("An Error has Occurred\n");
      exit(EXIT_FAILURE);
    }
    num_coords = PyList_Size(py_point);
    for (j = 0; j < num_coords; j++) {
      py_coord = PyList_GetItem(py_point, j);
      if (Py_IS_TYPE(py_coord, &PyFloat_Type) == 0) {
        printf("10:( \n");
        printf("An Error has Occurred\n");
        exit(EXIT_FAILURE);
      }
      coord = PyFloat_AsDouble(py_coord);
      add_coord(points_list_ptr->last, coord);
    }
  }
  return points_list_ptr;
}

struct CentroidsList* unpack_centroids_list(PyObject *centroids_list_py_ptr) {
  /* Assumes valid Python List Object is passed to function, meaning error checks for type should be external */
  Py_ssize_t num_centroids;
  Py_ssize_t i;
  struct Point *traversal_ptr;
  struct CentroidsList *centroids_list_ptr = malloc(sizeof(struct CentroidsList));
  struct PointsList *temp_list_ptr = malloc(sizeof(struct PointsList));

  if (centroids_list_ptr == NULL) {
    printf("11 :( \n");
    printf("An Error has Occurred\n");
    exit(EXIT_FAILURE);
  }

  if (temp_list_ptr == NULL) {
    printf("12 :( \n");
    printf("An Error has Occurred\n");
    exit(EXIT_FAILURE);
  } 
  
  centroids_list_ptr->head = NULL;
  temp_list_ptr = unpack_points_list(centroids_list_py_ptr);
  traversal_ptr = temp_list_ptr->head;
  num_centroids = PyList_Size(centroids_list_py_ptr);
  
  for(i = 0; i < num_centroids; i++) {
    add_centroid(centroids_list_ptr, traversal_ptr);
    traversal_ptr = traversal_ptr->next;
  }

  return centroids_list_ptr;
}


PyObject* convert_centroids_pyobject(struct CentroidsList* centroid_list_ptr, int K) {
  int i;
  int appended;
  struct Centroid* curr_centroid_ptr = centroid_list_ptr->head;
  struct PointCoords* curr_coord_ptr;
  PyObject* centroids_list_py_ptr = PyList_New(0);
  PyObject* temp_list_py;
  PyObject* temp_coord_py;
  
  if (centroids_list_py_ptr == NULL) {
    printf("13 :( \n");
    printf("An Error has Occurred\n");
    exit(EXIT_FAILURE);
  }
  
  for (i = 0; i < K; i++) {
    /*printf("Iteration number %d in convert_centroids_pyobject\n", i);*/
    temp_list_py = PyList_New(0);
    
    if (temp_list_py == NULL) {
      printf("14 :( \n");
      printf("An Error has Occurred\n");
      exit(EXIT_FAILURE);
    }
    
    curr_coord_ptr = curr_centroid_ptr->centroid->head;
    while(curr_coord_ptr != NULL) {
      /*printf("Adding coordinate: %f to temp_list_py\n", curr_coord_ptr->val);*/
      temp_coord_py = PyFloat_FromDouble(curr_coord_ptr->val);
      if (temp_coord_py == NULL) {
        printf("15.1 :( \n");
        printf("An Error has Occurred\n");
        exit(EXIT_FAILURE);
      }
      appended = PyList_Append(temp_list_py, temp_coord_py);
      
      if (appended == -1) {
        printf("15.2 :( \n");
        printf("An Error has Occurred\n");
        exit(EXIT_FAILURE);
      }
      
      curr_coord_ptr = curr_coord_ptr->next;
    }

    curr_centroid_ptr = curr_centroid_ptr->next;
    appended = PyList_Append(centroids_list_py_ptr, temp_list_py);
    if (appended == -1) {
      printf("16 :( \n");
      printf("An Error has Occurred\n");
      exit(EXIT_FAILURE);
    }
  }

  return centroids_list_py_ptr;
}



void kmeans(struct Lists *lists, int K, int iter, double epsilon, Py_ssize_t num_points) {
  struct Centroid* curr_centroid_ptr;
  struct Centroid* closest_cluster;
  struct Point* curr_point;
  int i;
  int j;
  int m;
  int converge;
  double delta;
  struct PointsList *points_list_ptr = *(lists->points_list_address);
  struct CentroidsList *centroid_list_ptr = *(lists->centroids_list_address);
  curr_point = points_list_ptr->head;
  

  /* Need to change initialize centroids to implementation required in the pdf */

  curr_centroid_ptr = NULL;
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
    curr_centroid_ptr = centroid_list_ptr->head;
    for (m = 0; m < K; m++) {
      delta = finalize_next_centroid_pos(&curr_centroid_ptr);
      curr_centroid_ptr = curr_centroid_ptr->next;

      if (delta > epsilon) {
        converge = 0;
      }
    }

    if (converge) {
      break;
    }

  }
}




static PyObject* k_means_plus_plus_c_wrapper(PyObject *self, PyObject *args) {
  /* Wrapper takes in Points, Initial Centroids, K, Iter */
  PyObject* points;
  PyObject* initial_centroids;
  int K;
  int iter;
  double epsilon;
  Py_ssize_t num_points;
  PyObject* final_centroids;

  if (!PyArg_ParseTuple(args, "OOiid", &points, &initial_centroids, &K, &iter, &epsilon)) {
    printf("17 :( \n");
    printf("An Error has Occurred\n");
    exit(EXIT_FAILURE);
  }

  if (!PyList_Check(points) || !PyList_Check(initial_centroids)) {
    printf("18 :( \n");
    printf("An Error has Occurred\n");
    exit(EXIT_FAILURE);
  }

  struct PointsList* points_list_ptr = unpack_points_list(points);
  struct CentroidsList* centroids_list_ptr = unpack_centroids_list(initial_centroids);
  num_points = PyList_Size(points);


  print_centroids(centroids_list_ptr);
  

  lists = malloc(sizeof(struct Lists));

  if (lists == NULL) {
    printf("19 :( \n");
    printf("An Error has Occurred\n");
    exit(EXIT_FAILURE);
  }

  lists->points_list_address = &points_list_ptr;
  lists->centroids_list_address = &centroids_list_ptr;

  kmeans(lists, K, iter, epsilon, num_points);

  final_centroids = convert_centroids_pyobject(centroids_list_ptr, K);
  free_all_memory();

  return final_centroids;
}


static PyMethodDef KMeansPPMethods[] = {
  {
    "fit", 
    (PyCFunction) k_means_plus_plus_c_wrapper,
    METH_VARARGS,
    "K-Means Plus Plus C Wrapper"
  },
  {NULL, NULL, 0, NULL}
};
  

static struct PyModuleDef KMeansPPModule = {
  PyModuleDef_HEAD_INIT,
  "mykmeanssp",
  "Python interface for the kmeans++ C module",
  -1,
  KMeansPPMethods
};

PyMODINIT_FUNC PyInit_mykmeanssp(void) {
    PyObject *module;
    module = PyModule_Create(&KMeansPPModule);
    if (!module) {
        return NULL;
    }
    return module;
}