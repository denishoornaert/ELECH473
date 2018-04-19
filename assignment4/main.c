#include <stdio.h>
#include <time.h>
#include <string.h>

#define SIZE 1048576
#define LOOP 65536
#define SAMPLING_SIZE 1000

void initArray(unsigned char* array, unsigned char value, unsigned int size) {
    unsigned int i;
    for(i = 0; i < size; i++) {
        array[i] = value;
    }
}

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

void asmVersion(float* dt) {
    time_t start, end;

    unsigned char threshold[16];
    initArray(threshold, 75-128, 16);
    unsigned char counterC2[16];
    initArray(counterC2, 128, 16);
    unsigned char buffer[SIZE];

    FILE *fp;
    fp = fopen("test.raw", "rb");
    FILE *foutput;
    foutput = fopen("testOut.raw", "wb");

    fread(buffer, sizeof(unsigned char), SIZE, fp);
    fclose(fp);

    start = clock();

    __asm__("mov $65536, %%edx;\n"
            "mov %[thr], %%eax;\n"
            "mov %[buf], %%ebx;\n"
            "mov %[cc2], %%ecx;\n"
            "movdqu (%%ecx), %%xmm2;\n"
            "movdqu (%%eax), %%xmm0;\n"
            "loop: movdqu (%%ebx), %%xmm1;\n"
            "psubb %%xmm2, %%xmm1;\n"
            "pcmpgtb %%xmm0, %%xmm1;\n"
            "movdqu %%xmm1, (%%ebx);\n"
            "add $16, %%ebx;\n"
            "sub $1, %%edx;\n"
            "mov $0, %%esi;\n"
            "cmp %%edx, %%esi;\n"
            "jnz loop;\n"
            : "=m" (buffer) // Output
            : [thr]"r" (threshold), [buf]"r" (buffer), [cc2]"r" (counterC2)  // Input
            : "xmm0", "eax", "ebx", "ecx"
        );


    end = clock();
    *dt = (end - start)/(float)(CLOCKS_PER_SEC);

    fwrite(buffer, sizeof(unsigned char), SIZE, foutput);
    fclose(foutput);
}

int main() {
    float dts[SAMPLING_SIZE];
    float min;
    float max;
    float avg;
    unsigned int i;

    for(i = 0; i < SAMPLING_SIZE; i++) {
        cVersion(&dts[i]);
    }
    getStatistics(dts, &min, &max, &avg);
    printf("min : %f\tmax : %f\tavg : %f\n", min, max, avg);

    for(i = 0; i < SAMPLING_SIZE; i++) {
        asmVersion(&dts[i]);
    }
    getStatistics(dts, &min, &max, &avg);
    printf("min : %f\tmax : %f\tavg : %f\n", min, max, avg);

    return 0;
}
