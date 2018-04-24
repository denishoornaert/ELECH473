/*
Authors :
    - Denis Hoornaert (000413326)
    - Nathan Liccardo (000408278)
    - Remy Detobel (000408013)

After having implemented the algorithm in C and in ASM (using SIMD), we launched
the program four times in order to get an insight of the performance difference.
The first and second launches of the program use both 10,000 samples but where
respectively compiled using the debug and the release mode.

--------------------------------------------------------------------------------
Samples : 10,000

Debug
C   -> min : 0.000000  max : 0.016000  avg : 0.002899
ASM -> min : 0.000000  max : 0.016000  avg : 0.000097
    => Factor of 29

Release
C   -> min : 0.000000  max : 0.016000  avg : 0.000963
ASM -> min : 0.000000  max : 0.016000  avg : 0.000091
    => Factor of 10
--------------------------------------------------------------------------------

We can easily see that, no matter the compilation mode used, the average time
required by the ASM (SIMD based) algorithm is far inferior to the one required
by the C version.

The difference between the 'Debug' and the 'Release' modes could be explain by
the fact that, in 'Release', mode some optimization are performed. In addition,
the 'Debug' mode could also add some information or initialization. For example,
some 'Debug' modes initialize automatically variables to zero. On the other
hand, the 'Release' mode optimizeq the code and do not save unnecessary information.

Therefore, we draw as a conclusion that if an algorithm can benefit from a
parallelization optimization (like in our case) then spending time on
implementing an SIMD version is a good idea as the compiler are not mature
enough to use those kings of accelerations/optimization properly.
*/

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <float.h>

#define SIZE 1048576
#define LOOP 65536
#define SAMPLING_SIZE 1000

#define FILE_INPUT "test.raw"
#define FILE_OUTPUT_C "testOutC.raw"
#define FILE_OUTPUT_ASM "testOutASM.raw"

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

void cVersion(float* dt) {
    time_t start, end;

    unsigned char threshold = 75;
    unsigned char buffer[SIZE];

    FILE *fp;
    fp = fopen(FILE_INPUT, "rb");
    FILE *foutput;
    foutput = fopen(FILE_OUTPUT_C, "wb");

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
    fp = fopen(FILE_INPUT, "rb");
    FILE *foutput;
    foutput = fopen(FILE_OUTPUT_ASM, "wb");

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
            : [thr]"r" (threshold), [buf]"r" (buffer), [cc2]"r" (counterC2)
            : "xmm0", "eax", "ebx", "ecx"
        );


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
