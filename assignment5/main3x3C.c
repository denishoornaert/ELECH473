#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#define SIZE 1048576
#define LOOP 65536
#define SAMPLING_SIZE 100 // max 255
#define PIXELS 1024
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))


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

void shift(unsigned char* array, int n) {
    for (unsigned char* i = array; i < array+PIXELS-n; i++) {
        *i = *(i+n);
    }
    for (unsigned char* i = array+PIXELS-n; i < array+PIXELS; i++)
        *i = 0;
}

void cVersion() {
    // Start clock
    time_t start, end;
    float dt;
    
    // Alloc and init variables
    unsigned char* buffer = malloc(sizeof(unsigned char) * SIZE);
    unsigned char* bufferOut = malloc(sizeof(unsigned char) * SIZE);
    start = clock();
    
    // Open files
    FILE *fp;
    fp = fopen("test.raw", "rb");
    FILE *foutput;
    foutput = fopen("testOut.raw", "wb");
    
    fread(buffer, sizeof(unsigned char), SIZE, fp);
    fclose(fp);
    
    // PRINT INIT VALUES
    
    // ASM equivalent
    for (int loop = 0; loop < PIXELS-2; loop++) {
        // Current const
        int current = loop*PIXELS;
        // load line 1 & 2 & 3 and max value
        unsigned char* line1 = malloc(sizeof(unsigned char) * PIXELS);
        unsigned char* line2 = malloc(sizeof(unsigned char) * PIXELS);
        unsigned char* line3 = malloc(sizeof(unsigned char) * PIXELS);
        for (int i = 0; i < PIXELS; i++) {
            int index;
            if (buffer[current+i] >= buffer[current+i+PIXELS] && buffer[current+i] >= buffer[current+i+2*PIXELS])
                index = current+i;
            else if (buffer[current+i+PIXELS] >= buffer[current+i] && buffer[current+i+PIXELS] >= buffer[current+i+2*PIXELS])
                index = current+i+PIXELS;
            else index = current+i+2*PIXELS;
            line1[i] = buffer[index];
            line2[i] = buffer[index];
            line3[i] = buffer[index];
        }
        // Shift value
        shift(line2,1);
        shift(line3,2);
        // FINAL max value in line1
        for (int i = 0; i < PIXELS-2; i++) {
            if (line2[i] >= line1[i] && line2[i] >= line3[i]) bufferOut[i+current] = line2[i];
            else if (line3[i] >= line1[i] && line3[i] >= line2[i]) bufferOut[i+current] = line3[i];
            else bufferOut[i+current] = line1[i];
        }
        
        // load line 1 & 2 & 3 and max value
        for (int i = 0; i < PIXELS; i++) {
            int index;
            if (buffer[current+i] <= buffer[current+i+PIXELS] && buffer[current+i] <= buffer[current+i+2*PIXELS])
                index = i+current;
            else if (buffer[current+i+PIXELS] <= buffer[current+i] && buffer[current+i+PIXELS] <= buffer[current+i+2*PIXELS])
                index = i+PIXELS+current;
            else index = i+2*PIXELS+current;
            line1[i] = buffer[index];
            line2[i] = buffer[index];
            line3[i] = buffer[index];
        }
        // Shift value
        shift(line2,1);
        shift(line3,2);
        // FINAL min/max value
        for (int i = 0; i < PIXELS-2; i++) {
            if (line2[i] <= line1[i] && line2[i] <= line3[i]) bufferOut[i+current] -= line2[i];
            else if (line3[i] <= line1[i] && line3[i] <= line2[i]) bufferOut[i+current] -= line3[i];
            else bufferOut[i+current] -= line1[i];
        }
    }
    printf("[");
    for (int i = 0; i < PIXELS*PIXELS; i++) {
        printf("%u, ", bufferOut[i]);
    }
    printf("]");
    
    // PRINT OUTPUT VALUES
    
    // End clock
    end = clock();
    dt = (end - start)/(float)(CLOCKS_PER_SEC);
    
    
    // Save file
    fwrite(bufferOut, sizeof(unsigned char), SIZE, foutput);
    
    free(buffer);
    free(bufferOut);
}

void asmVersion() {
    /*
    __asm__("mov $0, %%eax;\n"               // constante utilissée pour la comparaison
            "mov $74752, %%esi;\n"               // Counter = 73*1024 =
            "mov %0, %%edx;\n"                  // Address destination
            "mov %[buf], %%ebx;\n"              // Adresse des donnÈes
            
        "loop: mov %%ebx, %%ecx;\n"       // Save ebx dans ecx
            "movdqu (%%ebx), %%xmm0;\n" // xmm0: line 1 (max)
            "movdqu (%%ebx), %%xmm3;\n" // xmm3: line 1 (min)
            "add $1024, %%ecx;\n"       // DÈcalage
            "movdqu (%%ecx), %%xmm1;\n" // xmm1: line 2 (max)
            "movdqu (%%ecx), %%xmm4;\n" // xmm4: line 2 (min)
            "add $1024, %%ecx;\n"       // DÈcalage
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
            "add $14, %%ebx;\n"         // DÈcalage source
            "add $14, %%edx;\n"         // DÈcalage destination
            "sub $1, %%esi;\n"          // Decrementation
            "cmp %%eax, %%esi;\n"
        "jnz loop;\n"
        //    : "=m" (bufferOut)      // Output
        //    : [buf]"r" (buffer)  // Input
        //    : "eax", "ebx", "esi", "edx", "xmm0", "xmm1", "xmm2"
        //    );
     */
}

int main() {
    
    cVersion();
    
    return 0;
}


