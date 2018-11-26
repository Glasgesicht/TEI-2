
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "..\FT.h"
#include "..\wave.h"

#define COEFF_ARR_SIZE 4
// double COEFF_ARR[COEFF_ARR_SIZE] = { 0.2, 0.2, 0.2, 0.2 };

    /// TASK 3

float Filter(FILE* fileIn, wavheader header, double *coefficients, float *data)
{
    float sum = 0;
    int j,t = 0;
    int i = 0;

    for(t=0;t<(header.fmt_chunk_header.chunk_size/4);t++){
        for(j=0;j<COEFF_ARR_SIZE;j++){
         sum += (data[t-j]*coefficients[j]);
        }
    }
    // sum = sum-data[t];
    coefficients[i] = coefficients[i]-((0.01*data[t-1])*sum);

    print_that_thing(header,sum,coefficients);

    return sum;
}

void print(wavheader header,float sum,double *coefficients)
{
    int i = 0;
    for(i = 0; i < COEFF_ARR_SIZE;i++)
    {
        printf("\n%.5f",coefficients[i]);
    }

    printf("\nSum: %.5f",sum);

}
