#include <stdio.h>
#include <time.h>
#include <string.h>

#define SIZE 1048576

void cVersion() {
    time_t start, end;
    float dt;

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
    dt = (end - start)/(float)(CLOCKS_PER_SEC);
    printf("Time: %f", dt);

    fwrite(buffer, sizeof(unsigned char), SIZE, foutput);
    fclose(foutput);
}

void asmVersion() {
    time_t start, end;
    float dt;

    unsigned char valTreshold = 75;
    unsigned char threshold[16];
    memcpy(threshold, &valTreshold, 16);
    unsigned char buffer[SIZE];

    FILE *fp;
    fp = fopen("test.raw", "rb");
    FILE *foutput;
    foutput = fopen("testOut.raw", "wb");

    fread(buffer, sizeof(unsigned char), SIZE, fp);
    fclose(fp);

    start = clock();


    // asm code motherfucker
    /*__asm__ {
        "mov %[in], %%esi\n"
        "mov $75, %%eax\n"
    "loop:\n"
        "mov (%%esi), %%ebx\n"
        "cmp %%esi, %%eax\n" // TODO compare
        "jb zero\n"
        "\n" // write 255
    "zero:\n"
        "\n" // write 0
        "add %%esi, $1\n"
        "\n" // Compare with %[l]
        "\n" // Jump to loop if != 0
        :"=m"(buffer) // Outputs
        :[in]"m"(buffer), [1]"m" (SIZE): // inputs
        "esi", "ecx", "eax", "ebx"
    }*/

    end = clock();
    dt = (end - start)/(float)(CLOCKS_PER_SEC);
    printf("Time: %f", dt);

    fwrite(buffer, sizeof(unsigned char), SIZE, foutput);
    fclose(foutput);
}

int main() {
    cVersion();
    asmVersion();
    return 0;
}
