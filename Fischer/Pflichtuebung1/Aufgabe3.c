#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include "wave.h"
#include <math.h>
#include <string.h>


#ifndef M_PI
#define M_PI 3.1415926535897
#endif //M_PI


float* readDataChunk(unsigned int* data_size) {
    FILE *input2;
    input2 = fopen("test.wav", "rb");
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


float* Audiofilter(float* signal,unsigned int size) {

    float *sample = malloc(sizeof(float) * size);
    float koef[4];
    float y = 0;

    for(unsigned int t = 0;t<size;t++){
        if(t>2){
            y = (koef[0] *signal[t])+(koef[1]*signal[t-1])+(koef[2]*signal[t-2])+(koef[3]*signal[t-3]);
            sample[t] = signal[t]-y;
        }else{
            sample[t]=signal[t];
        }

        for (int i = 0; i < 4; i++)
        {
            float sum = 0;
            for (int j = 0; j < 4; j++)
            {
                sum += signal[t - j] * koef[j];
            }

            if (t - i < 0)
                koef[i] = 0;
            else
                koef[i] += - 0.01f * signal[t-i] * (sum - signal[t]);
        }

    }

//Non-Funktionabel, zu stolz es zu loeschen
/*  for(unsigned int t = 0;t < size ; t++ ) {
        for (int i = 0; i < 4; i++) {
            y = koeff[i] - 0.01f * signal[t - i] *
                                   ({
                                       float sum;
                                       sum = 0;
                                       for (int j = 0; j < 4; j++) {
                                           sum += signal[t - j] * koeff[t];
                                       }
                                       sum = sum - signal[t];
                                       sum;
                                   });
        }
        sample[t] = signal[t] - y;
    } */
        return sample;
}


int main() {
    unsigned int data_size;
    float* data = readDataChunk(&data_size);

    FILE *input;
    float file_size = 0;
    input = fopen("test.wav", "rb");
    if (input == NULL) {
        printf("Datei wurde nicht eingelesen\n");
        return -1;
    } else {
        printf("Datei erforlgreich eingelesen\n");
    }
    struct HEADER head;
    fread(&head, sizeof(head), 1, input);

    float* dataneu = Audiofilter(data,data_size);
    writePCM("AudioNeu.wav",dataneu,data_size,head);


    fclose(input);

    return 0;
}