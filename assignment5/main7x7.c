#include <stdio.h>
#include <time.h>
#include <string.h>
#include <float.h>
#include <limits.h>

#define SIZE 1048576
#define LOOP 65536
#define SAMPLING_SIZE 300
#define PIXELS 1024
#define SCALE 7

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

void cVersion(float* dt) {
    // Start clock
    time_t start, end;

    // Alloc and init variables
    unsigned char* buffer = malloc(sizeof(unsigned char) * SIZE);
    unsigned char* bufferOut = malloc(sizeof(unsigned char) * SIZE);

    // Open files
    FILE *fp;
    fp = fopen("test.raw", "rb");

    fread(buffer, sizeof(unsigned char), SIZE, fp);
    fclose(fp);


    start = clock();

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
                value = getMaxValue(value, buffer[lineIndex + (index*PIXELS)+i]);
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
                value = getMinValue(value, buffer[lineIndex + (index*PIXELS)+i]);
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

    // End clock
    end = clock();
    *dt = (end - start)/(float)(CLOCKS_PER_SEC);

    // Save file
    FILE *foutput;
    foutput = fopen("testOutC.raw", "wb");
    fwrite(bufferOut, sizeof(unsigned char), SIZE, foutput);

    free(buffer);
    free(bufferOut);
}

void asmVersion(float* dt) {
    time_t start, end;

    unsigned char* buffer = malloc(sizeof(unsigned char) * SIZE);
    unsigned char* bufferOut = malloc(sizeof(unsigned char) * SIZE);
    start = clock();

    FILE *fp;
    fp = fopen("test.raw", "rb");
    FILE *foutput;
    foutput = fopen("testOutASM.raw", "wb");

    fread(buffer, sizeof(unsigned char), SIZE, fp);
    fclose(fp);

    __asm__("mov $0, %%eax;\n"               // constante utilisée pour la comparaison
            "mov $104243, %%esi;\n"               // Counter = 101.8*1024 = 104243.2
            "mov %0, %%edx;\n"                  // Address destination
            "mov %[buf], %%ebx;\n"              // Adresse des donn�es
                "loop: mov %%ebx, %%ecx;\n"       // Save ebx dans ecx
                    "movdqu (%%ebx), %%xmm0;\n" // xmm0: line 1
                    "add $1024, %%ecx;\n"       // Décalage
                    "movdqu (%%ecx), %%xmm1;\n" // xmm1: line 2
                    "add $1024, %%ecx;\n"       // Décalage
                    "movdqu (%%ecx), %%xmm2;\n" // xmm2: line 3
                    "add $1024, %%ecx;\n"       // Décalage
                    "movdqu (%%ecx), %%xmm3;\n" // xmm2: line 4
                    "add $1024, %%ecx;\n"       // Décalage
                    "movdqu (%%ecx), %%xmm4;\n" // xmm2: line 5
                    "add $1024, %%ecx;\n"       // Décalage
                    "movdqu (%%ecx), %%xmm5;\n" // xmm2: line 6
                    "add $1024, %%ecx;\n"       // Décalage
                    "movdqu (%%ecx), %%xmm6;\n" // xmm2: line 7

                    // Calcul min
                    "movdqu %%xmm0, %%xmm7;\n"  // Move
                    "pminub %%xmm1, %%xmm7;\n"  // Min line 1 & 2
                    "pminub %%xmm2, %%xmm7;\n"  // Min line 1 & 3
                    "pminub %%xmm3, %%xmm7;\n"  // Min line 1 & 4
                    "pminub %%xmm4, %%xmm7;\n"  // Min line 1 & 5
                    "pminub %%xmm5, %%xmm7;\n"  // Min line 1 & 5
                    "pminub %%xmm6, %%xmm7;\n"  // Min line 1 & 5

                    // Calcul max
                    "pmaxub %%xmm1, %%xmm0;\n"  // Max line 1 & 2
                    "pmaxub %%xmm2, %%xmm0;\n"  // Max line 1 & 3
                    "pmaxub %%xmm3, %%xmm0;\n"  // Max line 1 & 4
                    "pmaxub %%xmm4, %%xmm0;\n"  // Max line 1 & 5
                    "pmaxub %%xmm5, %%xmm0;\n"  // Max line 1 & 6
                    "pmaxub %%xmm6, %%xmm0;\n"  // Max line 1 & 7

                    // Minimum
                    "movdqu %%xmm7, %%xmm1;\n"  // Copie les minimums
                    "psrldq $1, %%xmm1;\n"      // Shift Min
                    "movdqu %%xmm1, %%xmm2;\n"  // Save
                    "pminub %%xmm1, %%xmm7;\n"  // Min

                    "psrldq $1, %%xmm2;\n"      // Shift Min
                    "movdqu %%xmm2, %%xmm3;\n"  // Save
                    "pminub %%xmm2, %%xmm7;\n"  // Min

                    "psrldq $1, %%xmm3;\n"      // Shift Min
                    "movdqu %%xmm3, %%xmm4;\n"  // Save
                    "pminub %%xmm3, %%xmm7;\n"  // Min

                    "psrldq $1, %%xmm4;\n"      // Shift Min
                    "movdqu %%xmm4, %%xmm5;\n"  // Save
                    "pminub %%xmm4, %%xmm7;\n"  // Min

                    "psrldq $1, %%xmm5;\n"      // Shift Min
                    "movdqu %%xmm5, %%xmm6;\n"  // Save
                    "pminub %%xmm5, %%xmm7;\n"  // Min

                    "psrldq $1, %%xmm6;\n"      // Shift Min
                    "pminub %%xmm6, %%xmm7;\n"  // Min

                    // Maximum
                    "movdqu %%xmm0, %%xmm1;\n"  // Copie les maximum
                    "psrldq $1, %%xmm1;\n"      // Shift Max
                    "movdqu %%xmm1, %%xmm2;\n"
                    "pmaxub %%xmm1, %%xmm0;\n"  // Max

                    "psrldq $1, %%xmm2;\n"      // Shift Max
                    "movdqu %%xmm2, %%xmm3;\n"
                    "pmaxub %%xmm2, %%xmm0;\n"  // Max

                    "psrldq $1, %%xmm3;\n"      // Shift Max
                    "movdqu %%xmm3, %%xmm4;\n"
                    "pmaxub %%xmm3, %%xmm0;\n"  // Max

                    "psrldq $1, %%xmm4;\n"      // Shift Max
                    "movdqu %%xmm4, %%xmm5;\n"
                    "pmaxub %%xmm4, %%xmm0;\n"  // Max

                    "psrldq $1, %%xmm5;\n"      // Shift Max
                    "movdqu %%xmm5, %%xmm6;\n"
                    "pmaxub %%xmm5, %%xmm0;\n"  // Max

                    "psrldq $1, %%xmm6;\n"      // Shift Max
                    "pmaxub %%xmm6, %%xmm0;\n"  // Max

                    // Result
                    "psubb %%xmm7, %%xmm0;\n"   // Difference
                    // Move result
                    "movdqu %%xmm0, (%%edx);\n" // Result TODO change ebx
                    // Add destination source
                    "add $10, %%ebx;\n"         // D�calage source
                    "add $10, %%edx;\n"         // D�calage destination
                    "sub $1, %%esi;\n"          // Decrementation
                    "cmp %%eax, %%esi;\n"
                "jnz loop;\n"
            : "=m" (bufferOut)      // Output
            : [buf]"r" (buffer)  // Input
            : "eax", "ebx", "esi", "edx", "xmm0", "xmm1", "xmm2"
            );

    end = clock();
    *dt = (end - start)/(float)(CLOCKS_PER_SEC);

    fwrite(bufferOut, sizeof(unsigned char), SIZE, foutput);

    free(buffer);
    free(bufferOut);

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
    min = FLT_MAX;
    max = 0.0;
    avg = 0.0;

    for(i = 0; i < SAMPLING_SIZE; i++) {
        asmVersion(&dts[i]);
    }
    getStatistics(dts, &min, &max, &avg);
    printf("ASM Result: min : %f\tmax : %f\tavg : %f\n", min, max, avg);

    return 0;
}
