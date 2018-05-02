#include <stdio.h>
#include <time.h>
#include <string.h>
#include <float.h>
#include <pthread.h>

#define SIZE 1048576
#define LOOP 65536
#define SAMPLING_SIZE 1
#define NB_THR 2

#define FILE_INPUT "test.raw"
#define FILE_OUTPUT_C "testOutC.raw"
#define FILE_OUTPUT_ASM "testOutASM.raw"

unsigned char threshold = 75;

void initArray(unsigned char* array, unsigned char value, unsigned int size) {
    unsigned int i;
    for(i = 0; i < size; i++) {
        array[i] = value;
    }
}

void getStatistics(float* dts, float* min, float* max, float* avg) {
    unsigned int i;
    for(i = 0; i < SAMPLING_SIZE; i++) {
        *avg += dts[i];
        if(dts[i] < *min) {
            *min = dts[i];
        }
        if(*max < dts[i]) {
            *max = dts[i];
        }
    }
    *avg /= (float) SAMPLING_SIZE;
}

void* thresholdFunc(void* buffer) {
    unsigned char* array = (unsigned char*) buffer;
    unsigned int index;
    for(index = 0; index < SIZE/NB_THR; index++) {
        array[index] = (array[index] < threshold) ? 0 : 255;
    }
    
    pthread_exit(NULL);
}

void cVersion(float* dt) {
    time_t start, end;

    unsigned char buffer[SIZE];

    FILE *fp;
    fp = fopen(FILE_INPUT, "rb");
    FILE *foutput;
    foutput = fopen(FILE_OUTPUT_C, "wb");

    fread(buffer, sizeof(unsigned char), SIZE, fp);
    fclose(fp);

    start = clock();
    
    pthread_t tid[NB_THR];
    
    unsigned int addr;
    unsigned int index = 0;
    for (addr = 0; addr < SIZE; addr += (SIZE/NB_THR)) {
        pthread_create(&tid[index], NULL, thresholdFunc, (void*)&buffer[addr]);
        index++;
    }
    
    for (index = 0; index < NB_THR; index++) {
    	pthread_join(tid[index], NULL);
    }
    
    end = clock();
    *dt = (end - start)/(float)(CLOCKS_PER_SEC);

    fwrite(buffer, sizeof(unsigned char), SIZE, foutput);
    fclose(foutput);
}

int main() {
    float dts[SAMPLING_SIZE];
    float min = FLT_MAX;
    float max = 0.0;
    float avg = 0.0;
    unsigned int i;

    for(i = 0; i < SAMPLING_SIZE; i++) {
        cVersion(&dts[i]);
    }
    getStatistics(dts, &min, &max, &avg);
    printf("C Result: min : %f\tmax : %f\tavg : %f\n", min, max, avg);
    
    return 0;
}
