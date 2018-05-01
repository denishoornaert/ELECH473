#include <stdio.h>
#include <time.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <windows.h>

#define SIZE 1048576
#define LOOP 65536
#define SAMPLING_SIZE 300
#define PIXELS 1024
#define SCALE 3

double PCFreq = 0.0;
__int64 CounterStart = 0;

void getStatistics(double* dts, double* min, double* max, double* avg) {
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
    *avg /= (double)SAMPLING_SIZE;
}

void StartCounter() {
    LARGE_INTEGER li;
    if(!QueryPerformanceFrequency(&li)) {
        printf("QueryPerformanceFrequency failed!\n");
    }
    PCFreq = ((double)li.QuadPart)/1000.0;
    QueryPerformanceCounter(&li);
    CounterStart = li.QuadPart;
}

double GetCounter() {
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return ((double)li.QuadPart-CounterStart)/PCFreq;
}

int getMaxValue(unsigned char one, unsigned char two) {
    return one >= two ? one : two;
}

int getMinValue(unsigned char one, unsigned char two) {
    return one <= two ? one : two;
}

void cVersion(double* dt) {

    // Alloc and init variables
    unsigned char* buffer = malloc(sizeof(unsigned char) * SIZE);
    unsigned char* bufferOut = malloc(sizeof(unsigned char) * SIZE);

    // Open files
    FILE *fp;
    fp = fopen("test.raw", "rb");

    fread(buffer, sizeof(unsigned char), SIZE, fp);
    fclose(fp);


    StartCounter();

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
    *dt = GetCounter();

    // Save file
    FILE *foutput;
    foutput = fopen("testOutC.raw", "wb");
    fwrite(bufferOut, sizeof(unsigned char), SIZE, foutput);

    free(buffer);
    free(bufferOut);
}

void asmVersion(double* dt) {
    unsigned char* buffer = malloc(sizeof(unsigned char) * SIZE);
    unsigned char* bufferOut = malloc(sizeof(unsigned char) * SIZE);

    FILE *fp;
    fp = fopen("test.raw", "rb");
    FILE *foutput;
    foutput = fopen("testOutASM.raw", "wb");

    fread(buffer, sizeof(unsigned char), SIZE, fp);
    fclose(fp);

    StartCounter();

    __asm__("mov $0, %%eax;\n"               // constante utilissé pour la comparaison
            "mov $74752, %%esi;\n"               // Counter = 73*1024 =
            "mov %0, %%edx;\n"                  // Address destination
            // "add $1, %%edx;\n"                  // Décalage
            "mov %[buf], %%ebx;\n"              // Adresse des données
                "loop: mov %%ebx, %%ecx;\n"       // Save ebx dans ecx
                    "movdqu (%%ebx), %%xmm0;\n" // xmm0: line 1 (max)
                    "movdqu (%%ebx), %%xmm3;\n" // xmm3: line 1 (min)
                    "add $1024, %%ecx;\n"       // Décalage
                    "movdqu (%%ecx), %%xmm1;\n" // xmm1: line 2 (max)
                    "movdqu (%%ecx), %%xmm4;\n" // xmm4: line 2 (min)
                    "add $1024, %%ecx;\n"       // Décalage
                    "movdqu (%%ecx), %%xmm2;\n" // xmm2: line 3 (max)
                    "movdqu (%%ecx), %%xmm5;\n" // xmm5: line 3 (min)
                    // Calcul max
                    "pmaxub %%xmm1, %%xmm0;\n"  // Max line 1 & 2
                    "pmaxub %%xmm2, %%xmm0;\n"  // Max line 1 & 3
                    "movdqu %%xmm0, %%xmm1;\n"  // Copie les maximum
                    "movdqu %%xmm0, %%xmm2;\n"
                    "psrldq $1, %%xmm1;\n"      // Shift Max
                    "psrldq $2, %%xmm2;\n"
                    "pmaxub %%xmm1, %%xmm0;\n"  // Colon max
                    "pmaxub %%xmm2, %%xmm0;\n"
                    // Calcul min
                    "pminub %%xmm4, %%xmm3;\n"  // Max line 1 & 2
                    "pminub %%xmm5, %%xmm3;\n"  // Max line 1 & 3
                    "movdqu %%xmm3, %%xmm4;\n"  // Copie les maximum
                    "movdqu %%xmm3, %%xmm5;\n"
                    "psrldq $1, %%xmm4;\n"      // Shift Max
                    "psrldq $2, %%xmm5;\n"
                    "pminub %%xmm4, %%xmm3;\n"  // Colon min
                    "pminub %%xmm5, %%xmm3;\n"
                    // Result
                    "psubb %%xmm3, %%xmm0;\n"   // Difference
                    // Move result
                    "movdqu %%xmm0, (%%edx);\n" // Result TODO change ebx
                    // Add destination source
                    "add $14, %%ebx;\n"         // Décalage source
                    "add $14, %%edx;\n"         // Décalage destination
                    "sub $1, %%esi;\n"          // Decrementation
                    "cmp %%eax, %%esi;\n"
                "jnz loop;\n"
            : "=m" (bufferOut)      // Output
            : [buf]"r" (buffer)  // Input
            : "eax", "ebx", "esi", "edx", "xmm0", "xmm1", "xmm2"
            );

    *dt = GetCounter();

    fwrite(bufferOut, sizeof(unsigned char), SIZE, foutput);

    free(buffer);
    free(bufferOut);

    fclose(foutput);
}

int main() {
    double dts[SAMPLING_SIZE];
    double min = FLT_MAX;
    double max = 0.0;
    double avg = 0.0;
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
