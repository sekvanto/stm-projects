#include <stdio.h>
#include <math.h>

#define MIN 512
#define MAX 1536
#define SAMPLES_PER_CYCLE 100 // cycle = 2π

int main()
{
    printf("int16_t a441[] = {\n");

    int counter = 0;
    float step = (2*M_PI - 0) / SAMPLES_PER_CYCLE;
    for (float i = 0; i <= (2*M_PI); i += step) {
        float y = sin(i); // -1...1
        y *= 512; // -512...512
        y += 1024; // 512...1536
        printf("\t%d", (int) y);
        if (i + step > (2*M_PI)) {
            printf("\n");
        } else {
            printf(",\n");
        }
        counter++;
    }

    printf("};\n");
}