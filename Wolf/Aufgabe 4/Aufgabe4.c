#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include "wave.h"
#include <math.h>
#include <string.h>

#define maximum(a,b) \
     (a > b ? a : b)


#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795028841971
#endif //M_PI

float* readDataChunk(unsigned int* data_size) {
    FILE *input;
    input = fopen("test.wav", "rb");
    struct CHUNKHEADER chunkheader;
    char findData[4];
    int position = 0;


    for (int j = 12;; j = (j + chunkheader.chunk_size + 8)) {

        fseek(input, j, 0);
        fread(&chunkheader, sizeof(chunkheader), 1, input);

        for (int i = 0; i < 4; i++) {
            findData[i] = chunkheader.chunk_id[i];
        }

        if (findData[0] == 'd' && findData[1] == 'a' && findData[2] == 't' && findData[3] == 'a') {
            position = j;
            break;
        }
        if (feof(input)) {
            break;
        }
    }

    float *pdata;
    pdata = (float*) malloc(chunkheader.chunk_size);
    fseek(input, position+8, 0);
    fread(pdata, chunkheader.chunk_size, 1, input);
    *data_size = chunkheader.chunk_size/(sizeof(float));
    return pdata;
}

//Aufgabe 1:
//Mittlerer Betrag der Daten: 0.049511
float average(float* data, unsigned int size) {
    float sum = 0;
    for (unsigned int i = 0; i < size; i++) {
        sum += (float)fabs(data[i]);
    }
    return (sum/size);
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

int main() {
    unsigned int data_size;
    float* data = readDataChunk(&data_size);
    float averageValue = 0;

    FILE *input;
    float file_size = 0;

    averageValue = average(data, data_size);
    printf("Mittlerer Betrag der Daten: %f\n", averageValue); // Ergebnis: 0.049511


    input = fopen("test.wav", "rb");
    if (input == NULL) {
        printf("Datei wurde nicht eingelesen\n");
        return -1;
    } else {
        // printf("Datei erforlgreich eingelesen\n");
    }
    struct HEADER head;
    fread(&head, sizeof(head), 1, input);

    float* sinus1600 = sinusSignal(data_size, 1600, 1, head.sample_rate);
    float* sinus7600 = sinusSignal(data_size, 7600, 1, head.sample_rate);

    float* ueberlagert = ueberlagern(data,data_size,sinus1600,data_size);

   // float* sinus440 = sinusSignal(data_size, 440, 1, head.sample_rate);
    writePCM("sinus1600Hz.wav", sinus1600, data_size, head);
    writePCM("sinus7600Hz.wav", sinus7600, data_size, head);
    writePCM("Ueberlagert.wav", ueberlagert, data_size, head);

   /* zu Aufgabe 2c:
    * Die Datei "sinus7600Hz.wav" klingt tiefer, da die bitrate mit 8000Hz zu niedrig gewählt ist.
    * Sie muss immer dem Syntax Bitrate > Frequenz*2 entsprechen !
    */

    fclose(input);

    return 0;
}