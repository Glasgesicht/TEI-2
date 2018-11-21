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
    float averageValue = 0;

    FILE *input;
    float file_size = 0;


    averageValue = average(data, data_size);
    printf("Average value: %f\n", averageValue); // Ergebnis: 0.049511


    input = fopen("test.wav", "rb");
    if (input == NULL) {
        printf("Datei wurde nicht eingelesen\n");
        return -1;
    } else {
        printf("Datei erforlgreich eingelesen\n");
    }
    struct HEADER head;
    fread(&head, sizeof(head), 1, input);
    printf("samplerate: %d\n", head.sample_rate);

 /*   float* sinus1600 = sinusSignal(data_size, 1600, 1, head.sample_rate);
    float* sinus7600 = sinusSignal(data_size, 7600, 1, head.sample_rate);

    float* ueberlagert = ueberlagern(data,data_size,sinus1600,data_size); */

   // float* sinus440 = sinusSignal(data_size, 440, 1, head.sample_rate);
  /*  writePCM("sinus1600Hz.wav", sinus1600, data_size, head);
    writePCM("sinus7600Hz.wav", sinus7600, data_size, head);
    writePCM("Ueberlagert.wav", ueberlagert, data_size, head);*/


    float* dataneu = Audiofilter(data,data_size);
  writePCM("AudioNeu.wav",dataneu,data_size,head);



    //Die Datei "sinus7600Hz.wav" klingt tiefer, da die bitrate mit 8000Hz zu niedrig gewählt ist.
    //Sie muss immer dem Syntax Bitrate > Frequenz*2 entsprechen !

    fclose(input);

    return 0;
}