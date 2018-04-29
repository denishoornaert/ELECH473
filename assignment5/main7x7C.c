#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#define SIZE 1048576
#define LOOP 65536
#define SAMPLING_SIZE 100 // max 255
#define PIXELS 1024

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

int getMaxValue(unsigned char one, unsigned char two, unsigned char three, unsigned char four, unsigned char five, unsigned char six, unsigned char seven) {
    if (one >= two && one >= three && one >= four && one >= five && one >= six && one >= seven) return one;
    if (two >= one && two >= three && two >= four && two >= five && two >= six && two >= seven) return two;
    if (three >= one && three >= two && three >= four && three >= five && three >= six && three >= seven) return three;
    if (four >= one && four >= two && four >= three && four >= five && four >= six && four >= seven) return four;
    if (five >= one && five >= two && five >= three && five >= four && five >= six && five >= seven) return four;
    if (six >= one && six >= two && six >= three && six >= four && six >= five && six >= seven) return four;
    return seven;
}

int getMinValue(unsigned char one, unsigned char two, unsigned char three, unsigned char four, unsigned char five, unsigned char six, unsigned char seven) {
    if (one <= two && one <= three && one <= four && one <= five && one <= six && one <= seven) return one;
    if (two <= one && two <= three && two <= four && two <= five && two <= six && two <= seven) return two;
    if (three <= one && three <= two && three <= four && three <= five && three <= six && three <= seven) return three;
    if (four <= one && four <= two && four <= three && four <= five && four <= six && four <= seven) return four;
    if (five <= one && five <= two && five <= three && five <= four && five <= six && five <= seven) return four;
    if (six <= one && six <= two && six <= three && six <= four && six <= five && six <= seven) return four;
    return seven;
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
    unsigned char* line1A = malloc(sizeof(unsigned char) * PIXELS);
    unsigned char* line2A = malloc(sizeof(unsigned char) * PIXELS);
    unsigned char* line3A = malloc(sizeof(unsigned char) * PIXELS);
    unsigned char* line4A = malloc(sizeof(unsigned char) * PIXELS);
    unsigned char* line5A = malloc(sizeof(unsigned char) * PIXELS);
    unsigned char* line6A = malloc(sizeof(unsigned char) * PIXELS);
    unsigned char* line7A = malloc(sizeof(unsigned char) * PIXELS);
    for (int line1 = 0; line1 < (PIXELS*PIXELS)-6; line1 += PIXELS) {
        int line2 = line1+PIXELS;
        int line3 = line2+PIXELS;
        int line4 = line3+PIXELS;
        int line5 = line4+PIXELS;
        int line6 = line5+PIXELS;
        int line7 = line6+PIXELS;
        
        // MAX VALUE
        for (int i = 0; i < PIXELS; i++) {
            int value = getMaxValue(buffer[line1+i], buffer[line2+i], buffer[line3+i], buffer[line4+i], buffer[line5+i], buffer[line6+i], buffer[line7+i]);
            line1A[i] = value;
            line2A[i] = value;
            line3A[i] = value;
            line4A[i] = value;
            line5A[i] = value;
            line6A[i] = value;
            line7A[i] = value;
        }
        // Shift value
        shift(line2A,1);
        shift(line3A,2);
        shift(line4A,3);
        shift(line5A,4);
        shift(line6A,5);
        shift(line7A,6);
        
        // FINAL max value in line1
        for (int i = 0; i < PIXELS-2; i++) {
            bufferOut[line1+i] = getMaxValue(line1A[i], line2A[i], line3A[i], line4A[i], line5A[i], line6A[i], line7A[i]);
        }
        
        // MIN VALUE
        // load line 1 & 2 & 3 and max value
        for (int i = 0; i < PIXELS; i++) {
            int value = getMinValue(buffer[line1+i], buffer[line2+i], buffer[line3+i], buffer[line4+i], buffer[line5+i], buffer[line6+i], buffer[line7+i]);
            line1A[i] = value;
            line2A[i] = value;
            line3A[i] = value;
            line4A[i] = value;
            line5A[i] = value;
            line6A[i] = value;
            line7A[i] = value;
        }
        // Shift value and final value
        shift(line2A,1);
        shift(line3A,2);
        for (int i = 0; i < PIXELS-2; i++) {
            bufferOut[line1+i] -= getMinValue(line1A[i], line2A[i], line3A[i], line4A[i], line5A[i], line6A[i], line7A[i]);
        }
    }
    free(line1A);
    free(line2A);
    free(line3A);
    free(line4A);
    free(line5A);
    free(line6A);
    free(line7A);
    
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

int main() {
    
    cVersion();
    
    return 0;
}


