#include <stdio.h>
#include <time.h>
#include <string.h>

#define SIZE 1048576
#define SAMPLING_SIZE 100 // max 255

void initArray(unsigned char* array, unsigned char value, unsigned int size) {
    unsigned int i;
    for(i = 0; i < size; i++) {
        array[i] = value;
    }
}

// TODO add median stat
void getStatistics(float* dts, float* min, float* max, float* avg) {
    unsigned char i;
    for(i = 0; i < SAMPLING_SIZE; i++) {
        *avg += dts[i];
        if(dts[i] < *min) {
            *min = dts[i];
        }
        if(*max < dts[i]) {
            *max = dts[i];
        }
    }
    *avg /= SAMPLING_SIZE;
}

void cVersion(float* dt) {
    time_t start, end;

    unsigned char threshold = 75;
    unsigned char buffer[SIZE];
    FILE *fp;
    fp = fopen("test.raw", "rb");
    FILE *foutput;
    foutput = fopen("testOut.raw", "wb");

    fread(buffer, sizeof(unsigned char), SIZE, fp);
    fclose(fp);

    start = clock();

    unsigned int index;
    for(index = 0; index < SIZE; index++) {
        buffer[index] = (buffer[index] < threshold) ? 0 : 255;
    }

    end = clock();
    *dt = (end - start)/(float)(CLOCKS_PER_SEC);

    fwrite(buffer, sizeof(unsigned char), SIZE, foutput);
    fclose(foutput);
}

void asmVersion() {
    time_t start, end;
    float dt;

    unsigned char threshold[16];
    initArray(threshold, 75, 16);
    unsigned char buffer[SIZE];

    FILE *fp;
    fp = fopen("test.raw", "rb");
    FILE *foutput;
    foutput = fopen("testOut.raw", "wb");

    fread(buffer, sizeof(unsigned char), SIZE, fp);
    fclose(fp);

    start = clock();

    asm("movdqu %0, %%xmm7\n"
        "movdqu %%xmm6, %%xmm1\n"
        "paddb %%xmm7, %%xmm6\n"
        "movdqu %%xmm7, %0"
        : "=r" (threshold)
        : "0" (threshold)
    );

    end = clock();
    dt = (end - start)/(float)(CLOCKS_PER_SEC);
    printf("Time: %f\n", dt);
    printf("[");
    unsigned char i;
    for(i = 0; i < 16; i++) {
        printf("%u", threshold[i]);
    }
    printf("]\n");

    fwrite(buffer, sizeof(unsigned char), SIZE, foutput);
    fclose(foutput);
}

int main() {
    /*
    float dts[SAMPLING_SIZE];
    float min;
    float max;
    float avg;

    unsigned char i;
    for(i = 0; i < SAMPLING_SIZE; i++) {
        cVersion(&dts[i]);
        // asmVersion();
    }

    getStatistics(dts, &min, &max, &avg);
    printf("min : %f\tmax : %f\tavg : %f\n", min, max, avg);
    */

    asmVersion();

    return 0;
}
