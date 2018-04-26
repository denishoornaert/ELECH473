#include <stdio.h>
#include <time.h>
#include <string.h>

#define SIZE 1048576
#define LOOP 65536
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
/*
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
}*/

void asmVersion() {
    time_t start, end;
    float dt;

    unsigned char threshold[16];
    initArray(threshold, 75-128, 16);
    unsigned char counterC2[16];
    initArray(counterC2, 128, 16);
    unsigned char buffer[SIZE];
    unsigned char bufferOut[SIZE];

    FILE *fp;
    fp = fopen("test.raw", "rb");
    FILE *foutput;
    foutput = fopen("testOut.raw", "wb");

    fread(buffer, sizeof(unsigned char), SIZE, fp);
    fclose(fp);

    start = clock();

    __asm__("mov $1022, %%esi;\n"              // Counter
            "mov %0, %%edx;\n"              // Address destination
            "add $1025, %%edx;\n"           // Shift
            "mov %[buf], %%ebx;\n"          // Mov param
            "loop: mov %%ecx, %%ebx;\n"
            "movdqu (%%ebx), %%xmm0;\n"
            "add $1024, %%ecx;\n"
            "movdqu (%%ecx), %%xmm1;\n"
            "add $1024, %%ecx;\n"
            "movdqu (%%ecx), %%xmm2;\n"
            "movdqu %%xmm0, %%xmm3;\n"
            "movdqu %%xmm1, %%xmm4;\n"
            "movdqu %%xmm2, %%xmm5;\n"
            "pmaxub %%xmm1, %%xmm0;\n"         // Max
            "pmaxub %%xmm2, %%xmm0;\n"
            "movdqu %%xmm1, %%xmm0;\n"         // Copy Max
            "movdqu %%xmm2, %%xmm0;\n"
            "psrldq $1, %%xmm1;\n"             // Shift Max
            "psrldq $2, %%xmm2;\n"
            "pmaxub %%xmm2, %%xmm1;\n"          // Colon max
            "pmaxub %%xmm1, %%xmm0;\n"
            "pminub %%xmm4, %%xmm3;\n"         // Min
            "pminub %%xmm5, %%xmm3;\n"
            "movdqu %%xmm4, %%xmm3;\n"         // Copy Min
            "movdqu %%xmm5, %%xmm3;\n"
            "psrldq $1, %%xmm4;\n"             // Shift Min
            "psrldq $2, %%xmm5;\n"
            "pmaxub %%xmm5, %%xmm4;\n"         // Colon Min
            "pmaxub %%xmm4, %%xmm3;\n"
            "psubb %%xmm3, %%xmm0;\n"          // Difference
            "movdqu %%xmm0, (%%edx);\n"        // Result TODO change ebx
            // Add destination source
            "add $14, %%ebx;\n"
            "add $1, %%edx;\n"
            "sub $1, %%esi;\n"
            "mov $0, %%eax;\n"
            "cmp %%eax, %%esi;\n"
            "jnz loop;\n"
            : "=m" (bufferOut)      // Output
            : [buf]"r" (buffer)  // Input
            );

    /*__asm__("mov $65536, %%edx;\n"
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
        );*/


    end = clock();
    dt = (end - start)/(float)(CLOCKS_PER_SEC);
    printf("Time: %f\n", dt);
    printf("[");
    unsigned char i;
    for(i = 0; i < 16; i++) {
        printf("%u, ", threshold[i]);
    }
    printf("]\n");
    for(i = 0; i < 16; i++) {
        printf("%u, ", buffer[i]);
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

