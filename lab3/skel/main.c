#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#define NO_THREADS 4
#define SECOND_MICROS		1000000.f


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
	struct timeval start1, end1, start2, end2, start3, end3;


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

        #pragma omp barrier

        #pragma omp master
        {
            gettimeofday(&start1, NULL);
        }
        
        #pragma omp for reduction(+:sum1) private(i)
        for (i = 0; i < size; i++) {
            sum1 += numbers[i];
        }

        #pragma omp master
        {
            gettimeofday(&end1, NULL);
            t1 = (end1.tv_sec - start1.tv_sec) * SECOND_MICROS + end1.tv_usec - start1.tv_usec;
            time1 = t1 / SECOND_MICROS;
        }

        #pragma omp master
        {
            gettimeofday(&start2, NULL);
        }

        #pragma omp for private(i)
        for (i = 0; i < size; i++) {
            #pragma omp atomic
            sum2 += numbers[i];
        }

        #pragma omp master
        {
            gettimeofday(&end2, NULL);
            t2 = (end2.tv_sec - start2.tv_sec) * SECOND_MICROS + end2.tv_usec - start2.tv_usec;
            time2 = t2 / SECOND_MICROS;
        }

        #pragma omp master
        {
            gettimeofday(&start3, NULL);
        }

        #pragma omp for private(i)
        for (i = 0; i < size; i++) {
            #pragma omp critical
            sum3 += numbers[i];
        }

        #pragma omp master
        {
            gettimeofday(&end3, NULL);
            t2 = (end3.tv_sec - start3.tv_sec) * SECOND_MICROS + end3.tv_usec - start3.tv_usec;
            time3 = t2 / SECOND_MICROS;
        }

        #pragma omp barrier

        #pragma omp sections
        {
            #pragma omp section
            {
                write_file("reduction.txt", "atomic", time1, sum1);
            }

            #pragma omp section
            {
                write_file("atomic.txt", "atomic", time2, sum2);
            }

            #pragma omp section
            {
                write_file("critical.txt", "critical", time3, sum3);
            }
        }
    }

    // fiti atenti la partea de citire din fisier
    

    // de paralelizat in 3 moduri - atomic, critical si reduction, de masurat timpii de executi

    // TODO: de scris timpii de executie in 3 fisiere folosind sections (fiecare scriere intr-un section)


    return 0;
}