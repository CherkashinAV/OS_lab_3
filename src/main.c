#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
#define DEFAULT_THREADS_NUMBER 2
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))


int median(const int n, int values[n]){
	for(int i = 0; i < n; i++){
        for(int j = i; j < n; j++){
            if(values[i] > values[j]){
               int temp = values[i];
               values[i] = values[j];
               values[j] = temp;
            }
        }
    }
    return values[n / 2];
}

void median_filter(const int height, const int width, int **a, int **b, int area, int lower_row, int upper_row){
	int radius = (area - 1) / 2;
	for (int y = lower_row; y < upper_row; y++){
		int top = MAX(y - radius, 0);
		int bottom = MIN(y + radius, height - 1);
		for (int x = 0; x < width; x++){
			int left = MAX(x - radius, 0);
			int right = MIN(x + radius, width - 1);
			int m[(bottom - top + 1) * (right - left + 1)];
			int k = 0;
			for (int v = top; v <= bottom; v++){
				for (int u = left; u <= right; u++){
					m[k] = a[v][u];
					k++;
				}
			}
			b[y][x] = median((bottom - top + 1) * (right - left + 1), m);
		}
	}
}

typedef struct{
	int height;
	int width;
	int **first_matrix;
	int **second_matrix;
	int area; 
	int lower_row;
	int upper_row;
}thread_args;

void *lineresation(void *args){
	thread_args *arg = args;
	median_filter(arg->height, arg->width, arg->first_matrix, arg->second_matrix, arg->area, arg->lower_row, arg->upper_row);
	return NULL;
}

void matrix_print(const int n, const int m, int **mat){
	for (int i = 0; i < n; i++){
		for (int j = 0; j < m; j++){
			printf("%d ", mat[i][j]);
		}
		printf("\n");
	}
}

bool is_number(char *num){
	for (int i = 0; i < strlen(num); i++){
		if (num[i] < '0' || num[i] > '9')
			return false; 
	}
	return true;
}

int main(int argc, char* argv[]){
	int n, m;
	int count_of_threads = DEFAULT_THREADS_NUMBER;
	if (argc == 2){
		char *num = argv[1];
		if (! is_number(num)){
			perror("Something wrong with your argument!");
			exit(-1);
		}
		count_of_threads = atoi(num);
	}
	else if (argc < 2)
		printf("You didn't specify the number of threads when you started the program.\nProgram will run with default number of threads %d\n", DEFAULT_THREADS_NUMBER);
	else if (argc > 2){
		perror("Something wrong with count of your arguments!\n");
		exit(-1);
	}
	
	printf("Input matrix size: ");
	scanf("%d %d", &n, &m);
	if (count_of_threads > n)
		count_of_threads = n;
	int **mat1 = (int**)malloc(n * sizeof(int*));
	int **mat2 = (int**)malloc(n * sizeof(int*));
	for (int i = 0; i < n; i++){
		mat1[i] = (int*)malloc(m * sizeof(int));
		mat2[i] = (int*)malloc(m * sizeof(int));
	}
	int window_size;
	int k;
	printf("Input window size (from 3 and other odd numers()): ");
	scanf("%d", &window_size);
	printf("Input count of median filters to matrix: ");
	scanf("%d", &k);
	int n_preceesed_rows = n / count_of_threads;
	for (int i = 0; i < n; i++){
		for (int j = 0; j < m; j++){
			scanf("%d", &mat1[i][j]);
		}
	}
	pthread_t threads[count_of_threads];
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	thread_args p_args[count_of_threads];
	for (int i  = 0; i < k; i++){
		for(int j = 0; j < count_of_threads; j++){
			p_args[j].height = n;
			p_args[j].width = m;
			if (i % 2 == 0){
				p_args[j].first_matrix = mat1;
				p_args[j].second_matrix = mat2;
			}
			else{
				p_args[j].first_matrix = mat2;
				p_args[j].second_matrix = mat1;
			}
			p_args[j].area = window_size;
			p_args[j].lower_row = j * n_preceesed_rows;
			p_args[j].upper_row = (j + 1) * n_preceesed_rows;
			if (j == count_of_threads - 1)
				p_args[j].upper_row = n;
			printf("Thread %d is working with [%d,%d) rows\n", j + 1, p_args[j].lower_row, p_args[j].upper_row);
		}
		for (int i = 0; i < count_of_threads; i++){
			int res = pthread_create(&threads[i], &attr, lineresation, &p_args[i]);
			if (res != 0){
				perror("Error with thread creation!");
				exit(-2);
			}
		}
		for (int i = 0; i < count_of_threads; i++){
			int res = pthread_join(threads[i], NULL);
			if (res != 0){
				perror("Error with thread joining!");
				exit(-2);
			}
		}
		printf("-------------------------------------------------\n");
	}

	if (k % 2 == 1)
		matrix_print(n, m, mat2);
	else 
		matrix_print(n, m, mat1);
}
