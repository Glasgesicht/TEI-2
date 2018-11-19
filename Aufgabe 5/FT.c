#define _CRT_SECURE_NO_WARNINGS

#include "FT.h"

#include <malloc.h>
#include <math.h>

#include <stdlib.h>  
#include <stdio.h>

#include <string.h>

// --- Discrete Fourier Transform ------------------------
spektrum* DFT(float* x, size_t N)
{
  size_t n,k;

  /*Aus didaktischen Gründen N Koeffizienten reservieren/berechnen (statt (N+2)/2)*/
  spektrum* sp = (spektrum *) malloc(sizeof(spektrum));
  sp->real=(float*)malloc(sizeof(float)*N);
  sp->imag=(float*)malloc(sizeof(float)*N);
  sp->ampl=(float*)malloc(sizeof(float)*N);

  for (k=0; k < N ; k++) /* eigentlich reicht bis (N+2)/2, siehe oben*/
  {
    sp->real[k]=sp->imag[k]=0.0;
    for (n=0; n<N; n++)
    {
      sp->real[k] += x[n] * (float)cos((k*2.0*M_PI*n)/N);
      sp->imag[k] += x[n] * (float)sin((k*2.0*M_PI*n)/N);
    }
    sp->ampl[k] = (float) sqrt(sp->real[k]*sp->real[k] + sp->imag[k]*sp->imag[k]);
  }

  return sp;
}

// --- inverse Discrete Fourier Transform ------------------------
float* invDFT(spektrum* sp, size_t N)
{
  size_t n,k;
  float* x = (float*)malloc(sizeof(float)*N);
  float norm=2.f/(float) N;

  for (n=0; n<N; n++)
  {
    x[n]=sp->real[0]/(float) N;
    for (k=1; k<(N+1)/2; k++)
    {
      x[n] += norm*(sp->real[k] * (float)cos(k*2.0*M_PI*n/N) + sp->imag[k] * (float)sin(k*2.0*M_PI*n/N));
    }
    x[n] += (1.f-N%2)*(1.f-2.f*(n%2))*sp->real[N/2]/(float)N;
  }
  return x;
}

void writeArray(char* name, float *X, int N, float f0)
{
  char fn[100];
  char* c;
  int n;
  FILE *fileOut;

  strcat(strcpy(fn, name),".txt");
  c=strstr(fn, ">");
  if(c!=NULL) *c='-';
  
  fileOut = fopen(fn,"wt");
  for(n=0; n < N; ++n)
    fprintf(fileOut, "%d,%f,%f\n",n,X[n],(f0*n)/N);

  fclose(fileOut);
}

void plotfile(char* name, int maxN)
{
  char* c;
  char command[100];
  
  sprintf(command, "start python plotsig.py %s.txt %d", name, maxN);
  
  c=strstr(command, ">");
  if(c!=NULL) *c='-';
  system(command);
}

float* sinusSignal(unsigned int N, unsigned int f, float a, unsigned int r) {
    float* x= malloc(N* sizeof(float));
    for (unsigned int i = 0; i < N; i++) {
        x[i] = (float)(sin(f*2*M_PI*((float)i/r))*(float)a);
        // printf("%f\n",x[i]); @Debug
    }
    return x;
}

float* ueberlagern(float* data_a,unsigned int size_a, float* data_b, unsigned int size_b){

    float* final = malloc(maximum(size_a,size_b)*sizeof(float));
    for (unsigned int i = 0; i < size_a; i++) {
        final[i] = data_a[i];
    }
    for (unsigned int i = 0; i < size_b; i++) {
        final[i] = (final[i]+data_b[i])/2;
    }

    return final;
}

int main() 
{
    float* data1;
    float* data2;
    float* dataFinal;
    size_t size = 50;

    data1 = sinusSignal(50, 1, 1, size);
    data2 = sinusSignal(50, 5, 2, size);
    dataFinal = ueberlagern(data1, data2, size);

	plot(dataFinal, size);

    spektrum* sp;
    sp = DFT(dataFinal, size);
    plot(sp->ampl, size);

    return 0;
}
