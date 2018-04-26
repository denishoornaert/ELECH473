#include <stdio.h>
#include <time.h>
#include <string.h>

#define SIZE 1048576
#define LOOP 65536
#define SAMPLING_SIZE 100 // max 255


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
    *avg /= (float)SAMPLING_SIZE;
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

    unsigned char* buffer = malloc(sizeof(unsigned char) * SIZE);
    unsigned char* bufferOut = malloc(sizeof(unsigned char) * SIZE);
    start = clock();

    FILE *fp;
    fp = fopen("test.raw", "rb");
    FILE *foutput;
    foutput = fopen("testOut.raw", "wb");

    fread(buffer, sizeof(unsigned char), SIZE, fp);
    fclose(fp);

    unsigned char i;
    for(i = 0; i < 16; i++) {
        printf("%u, ", buffer[i]);
    }
    printf("]\n");

    __asm__("mov $0, %%eax;\n"               // constante utiliss� pour la comparaison
            "mov $74752, %%esi;\n"               // Counter = 73*1024 =
            "mov %0, %%edx;\n"                  // Address destination
            // "add $1, %%edx;\n"                  // D�calage
            "mov %[buf], %%ebx;\n"              // Adresse des donn�es
                "loop: mov %%ebx, %%ecx;\n"       // Save ebx dans ecx
                    "movdqu (%%ebx), %%xmm0;\n" // xmm0: line 1 (max)
                    "movdqu (%%ebx), %%xmm3;\n" // xmm3: line 1 (min)
                    "add $1024, %%ecx;\n"       // D�calage
                    "movdqu (%%ecx), %%xmm1;\n" // xmm1: line 2 (max)
                    "movdqu (%%ecx), %%xmm4;\n" // xmm4: line 2 (min)
                    "add $1024, %%ecx;\n"       // D�calage
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
                    "add $14, %%ebx;\n"         // D�calage source
                    "add $14, %%edx;\n"         // D�calage destination
                    "sub $1, %%esi;\n"          // Decrementation
                    "cmp %%eax, %%esi;\n"
                "jnz loop;\n"
            : "=m" (bufferOut)      // Output
            : [buf]"r" (buffer)  // Input
            : "eax", "ebx", "esi", "edx", "xmm0", "xmm1", "xmm2"
            );

/*
    __asm__("mov $0, %%eax;\n"               // constante utiliss� pour la comparaison
            "mov $1, %%esi;\n"               // Counter = 8 * (nbrLine-2) = 8176
            "mov %0, %%edx;\n"                  // Address destination
            // "add $1, %%edx;\n"                  // D�calage
            "mov %[buf], %%ebx;\n"              // Adresse des donn�es
                "loop: mov %%ebx, %%ecx;\n"       // Save ebx dans ecx
                    "movdqu (%%ebx), %%xmm0;\n" // line 1
                    "add $1024, %%ecx;\n"       // D�calage
                    "movdqu (%%ecx), %%xmm1;\n" // line 2
                    "add $1024, %%ecx;\n"       // D�calage
                    "movdqu (%%ecx), %%xmm2;\n" // line 3
                    "pmaxub %%xmm1, %%xmm0;\n"  // Max line 1 & 2
                    "pmaxub %%xmm2, %%xmm0;\n"  // Max line 1 & 3
                    "movdqu %%xmm0, %%xmm1;\n"  // Copie les maximum
                    "movdqu %%xmm0, %%xmm2;\n"
                    "psrldq $1, %%xmm1;\n"      // Shift Max
                    "psrldq $2, %%xmm2;\n"
                    "pmaxub %%xmm1, %%xmm0;\n"  // Colon max
                    "pmaxub %%xmm2, %%xmm0;\n"
                    "movdqu %%xmm0, (%%edx);\n" // Result TODO change ebx
                    // Add destination source
                    "add $14, %%ebx;\n"         // D�calage source
                    "add $14, %%edx;\n"         // D�calage destination
                    "sub $1, %%esi;\n"          // Decrementation
                    "cmp %%eax, %%esi;\n"
                "jnz loop;\n"
            : "=m" (bufferOut)      // Output
            : [buf]"r" (buffer)  // Input
            : "eax", "ebx", "esi", "edx", "xmm0"
            );
*/

    /*
    __asm__("mov $1022, %%esi;\n"               // Counter
            "mov %0, %%edx;\n"                  // Address destination
            // "add $1, %%edx;\n"                  // D�calage
            "mov %[buf], %%ebx;\n"              // Adresse des donn�es
                "loop:\n"
                    "mov %%ecx, %%ebx;\n"       // Save ebx dans ecx
                    "movdqu (%%ebx), %%xmm0;\n" // line 1
                    "add $1024, %%ecx;\n"       // D�calage
                    "movdqu (%%ecx), %%xmm1;\n" // line 2
                    "add $1024, %%ecx;\n"       // D�calage
                    "movdqu (%%ecx), %%xmm2;\n" // line 3
                    "movdqu %%xmm0, %%xmm3;\n"  // Copie line1 pour le min
                    "movdqu %%xmm1, %%xmm4;\n"  // Copie line2 pour le min
                    "movdqu %%xmm2, %%xmm5;\n"  // Copie line3 pour le min
                    "pmaxub %%xmm1, %%xmm0;\n"  // Max line 1 & 2
                    "pmaxub %%xmm2, %%xmm0;\n"  // Max line 1 & 3
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
            "add $14, %%ebx;\n"             // Ligne suivante
            "add $1, %%edx;\n"
            "sub $1, %%esi;\n"
            "mov $0, %%eax;\n"
            "cmp %%eax, %%esi;\n"
            "jnz loop;\n"
            : "=m" (bufferOut)      // Output
            : [buf]"r" (buffer)  // Input
            );
            */

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
    for(i = 0; i < 16; i++) {
        printf("%u, ", bufferOut[i]);
    }
    printf("]\n");

    fwrite(bufferOut, sizeof(unsigned char), SIZE, foutput);

    free(buffer);
    free(bufferOut);

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

