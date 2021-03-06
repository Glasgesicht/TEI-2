#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include "wave.h"
#include <math.h>
#include <string.h>


#ifndef M_PI
#define M_PI 3.1415926535897
#endif //M_PI

#define __AUDIODATA__ "test.wav"

float* readDataChunk(unsigned int* data_size) {
    FILE *input2;
    input2 = fopen(__AUDIODATA__, "rb");
    struct CHUNKHEADER chunkhead;
    char findData[4];
    int position = 0;


    for (int j = 12;; j = (j + chunkhead.chunk_size + 8)) {

        fseek(input2, j, 0);
        fread(&chunkhead, sizeof(chunkhead), 1, input2);

        for (int i = 0; i < 4; i++) {
            findData[i] = chunkhead.chunk_id[i];
        }

        if (findData[0] == 'd' && findData[1] == 'a' && findData[2] == 't' && findData[3] == 'a') {
            position = j;
//            printf("\nPosition gefunden: %i\n", position);
            break;
        }
        if (feof(input2)) {
//            printf("Datei ENDE\n");
            break;
        }
    }

    float *pdata;
    pdata = (float*) malloc(chunkhead.chunk_size);
    fseek(input2, position+8, 0);
    fread(pdata, chunkhead.chunk_size, 1, input2);
//    printf("Hier:: %d\n",chunkhead.chunk_size);
    *data_size = chunkhead.chunk_size/(sizeof(float));
//    printf("Array Length: %f\n", *data_size);
    return pdata;
}


float* sinusSignal(unsigned int N, unsigned int f, float a, unsigned int r) {
    float* x= malloc(N* sizeof(float));
    for (unsigned int i = 0; i < N; i++) {
        x[i] = (float)(sin(f*2*M_PI*((float)i/r))*(float)a);
        printf("%f\n",x[i]);
    }
    return x;
}


#define bufferlength 20
#define bufferkoeffizient ((float)1/(float)20)


float* Audiofilter(float* signal,unsigned int size){

    float* signalneu = malloc(sizeof(float)*size);
    float* ringbuffer = malloc(sizeof(float)*bufferlength);

    float summe_ringbuffer;

    for(int n = 0;n<bufferlength;n++) {
        ringbuffer[n] = 0;
    }

    for(unsigned int j =0; j < size;j++){

        summe_ringbuffer = 0;
        ringbuffer[j%20] = signal[j]*bufferkoeffizient;

        for(int t =0;t<bufferlength;t++) {
            summe_ringbuffer+=ringbuffer[t];
        }

            signalneu[j] = summe_ringbuffer;

            printf("%f\n",signalneu[j]);
        }

        return signalneu;
    }


int main() {
    unsigned int data_size;
    float* data = readDataChunk(&data_size);

    FILE *input;
    float file_size = 0;

    input = fopen(__AUDIODATA__, "rb");
    if (input == NULL) {
        printf("Datei wurde nicht eingelesen\n");
        return -1;
    } else {
        printf("Datei erforlgreich eingelesen\n");
    }
    struct HEADER head;
    fread(&head, sizeof(head), 1, input);

    float* dataneu = Audiofilter(data,data_size);
    writePCM("Aufgabe1.wav",dataneu,data_size,head);

    fclose(input);

    return 0;
}
