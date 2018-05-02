// Pas fini !!!
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <float.h>
#include <limits.h>
#include <pthread.h>

#define SIZE 1048576
#define LOOP 65536
#define SAMPLING_SIZE 300
#define PIXELS 1024
#define SCALE 3
#define NB_THR 2

#define FILE_INPUT "test.raw"
#define FILE_OUTPUT_C "testOutC.raw"
#define FILE_OUTPUT_ASM "testOutASM.raw"


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
    *avg /= (float)SAMPLING_SIZE;
}

int getMaxValue(unsigned char one, unsigned char two) {
    return one >= two ? one : two;
}

int getMinValue(unsigned char one, unsigned char two) {
    return one <= two ? one : two;
}

void* thresholdFunc(void* buffer) {
    unsigned char* array = (unsigned char*) buffer;
    unsigned int index;
    
    // ASM equivalent
    unsigned char* line = malloc(sizeof(unsigned char) * PIXELS);
    unsigned int lineIndex;
    for(lineIndex = 0; lineIndex < (PIXELS*PIXELS)-SCALE; lineIndex += PIXELS) {
        
        // MAX VALUE
        unsigned int i;
        for(i = 0; i < PIXELS; ++i) {
            int value = INT_MIN;
            unsigned int index;
            for(index = 0; index < SCALE; ++index) {
                value = getMaxValue(value, array[[lineIndex + (index*PIXELS)+i]);
            }
            line[i] = value;
        }

        // FINAL max value in line
        for(i = 0; i < PIXELS-SCALE; ++i) {
            int value = INT_MIN;
            unsigned int index;
            for(index = 0; index < SCALE && i+index < PIXELS; ++index) {
                value = getMaxValue(value, line[i+index]);
            }
            bufferOut[lineIndex+i] = value;
        }

        // MIN VALUE
        for(i = 0; i < PIXELS; ++i) {
            int value = INT_MAX;
            unsigned int index;
            for(index = 0; index < SCALE; ++index) {
                value = getMinValue(value, array[[lineIndex + (index*PIXELS)+i]);
            }
            line[i] = value;
        }
        // Shift value and final value
        for(i = 0; i < PIXELS-SCALE; ++i) {
            int value = INT_MAX;
            unsigned int index;
            for(index = 0; index < SCALE && i+index < PIXELS; ++index) {
                value = getMinValue(value, line[i+index]);
            }
            bufferOut[lineIndex+i] -= value;
        }
    }
    free(line);
    
    pthread_exit(NULL);
}

void cVersion(float* dt) {
    // Start clock
    time_t start, end;

    // Alloc and init variables
    unsigned char* buffer = malloc(sizeof(unsigned char) * SIZE);
    unsigned char* bufferOut = malloc(sizeof(unsigned char) * SIZE);

    // Open files
    FILE *fp;
    fp = fopen(FILE_INPUT, "rb");

    fread(buffer, sizeof(unsigned char), SIZE, fp);
    fclose(fp);


    start = clock();
    
    unsigned char threadIndex = 0;
    unsigned int total = (PIXELS*PIXELS)-SCALE;
    unsigned int addr;
    for(addr = 0; addr < total/NB_THR; addr += total/NB_THR) {
        pthread_create(&tid[index], NULL, thresholdFunc, (void*)&buffer[addr]);
        threadIndex++;
    }
    

    // End clock
    end = clock();
    *dt = (end - start)/(float)(CLOCKS_PER_SEC);

    // Save file
    FILE *foutput;
    foutput = fopen(FILE_OUTPUT_C, "wb");
    fwrite(bufferOut, sizeof(unsigned char), SIZE, foutput);

    free(buffer);
    free(bufferOut);
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
