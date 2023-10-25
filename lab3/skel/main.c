#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NO_THREADS 4

// scriere in fisier
void write_file(char *filename, char *type, double time, long sum) {
    FILE *file = fopen(filename, "w+");
    fprintf(file, "Type: %s\nTime: %lf\nSum: %ld", type, time, sum);
    fclose(file);
}

int main() {
    FILE *file = fopen("input.txt", "r");
    int size, i;
    int *numbers;

    long sum1 = 0, sum2 = 0, sum3 = 0;
    double t1, t2;
    double time1, time2, time3;
    omp_set_num_threads(NO_THREADS);

    // de paralelizat toata bucata de mai jos
    #pragma omp parallel
    {
        #pragma omp single
        {
            fscanf(file, "%d", &size);
            numbers = malloc(size * sizeof(int));

            for (int i = 0; i < size; i++) {
                fscanf(file, "%d", &numbers[i]);
            }
            fclose(file);
        }

        #pragma omp for reduction(+:sum1) private(i)
        for (i = 0; i < size; i++) {
            sum1 += numbers[i];
        }

        #pragma omp for shared(sum2) private(i)
        for (i = 0; i < size; i++) {
            #pragma omp atomic
            sum2 += numbers[i];
        }

        #pragma omp for shared(sum3) private(i)
        for (i = 0; i < size; i++) {
            #pragma omp critical
            sum2 += numbers[i];
        }
    }

    // fiti atenti la partea de citire din fisier
    

    // de paralelizat in 3 moduri - atomic, critical si reduction, de masurat timpii de executi
    printf("Sum1: %ld\n", sum1);
    printf("Sum2: %ld\n", sum2);
    printf("Sum3: %ld\n", sum3);

    // TODO: de scris timpii de executie in 3 fisiere folosind sections (fiecare scriere intr-un section)


    return 0;
}