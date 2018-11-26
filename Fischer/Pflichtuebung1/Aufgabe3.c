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



float* Audiofilter(float* sample,unsigned int size) {


    //Reserviere Speicher für Neues Audiosignal
    float *signalNeu = malloc(sizeof(float) * size);

    //"Ringbuffer" für die Koeffizienten
    float* koef = malloc(sizeof(float)*4);

    //Setze Koeffizienten auf 0 um Fehler zu vermeiden
    for(int n = 0;n<4;n++)
        koef[n] = 0;


    for(int t = 0;t<(int)size;t++){

/* Test for GCC > 3.2.0 */
//Nested Functions sind im C Standart nicht erlaubt, deswegen (zur Sicherheit) die doppelte Implementierung
#if ((__GNUC__ > 3) || (__GNUC__ == 3 && (__GNUC_MINOR__ > 2)))

                //Implementierung moeglichst nah an der gegebenen Formel
                //via nested function

                //Anlernen des Audiofilters
                koef[t%4] -= 0.01f * sample[t - (t%4)] * (
                                                    ({
                                                        //Lokale Variable zu berechnung der Summenformel
                                                        float _sum = 0;
                                                        for (int j = 0; j < 4; j++)
                                                        {
                                                            _sum += sample[t - j] * koef[j];
                                                        }
                                                        _sum;
                                                    })
                                                    - sample[t]);
                //printf("%f\n",koef[(t%4)]);
#else
/* Implementierung, die auch mit MingGW Kompatibel sein soll*/
                float sum = 0;
                for (int j = 0; j < 4; j++)
                {
                    sum += sample[t - j] * koef[j];
                }

                koef[t%4] -= 0.01f * sample[t - (t%4)] * (sum - sample[t]);
                //printf("%f\n",koef[(t%4)]);
#endif


        if(t>=3) {
            //Erzeugung des Neuen Signals, in dem der Errechnete Fehler subtrahiert wird.
            signalNeu[t] = sample[t] - ((koef[t%4] * sample[t]) + (koef[(t+1)%1] * sample[t - 1]) + (koef[(t+2)%4] * sample[t - 2]) +
                                         (koef[(t+3)%4] * sample[t - 3]));
        }else {
            //H
            signalNeu[t] = sample[t];
        }


        printf("%f\n",signalNeu[t]);
    }
        return signalNeu;
}


int main() {
    unsigned int data_size;
    float* data = readDataChunk(&data_size);

    FILE *input;
    input = fopen("test.wav", "rb");
    if (input == NULL) {
        printf("Datei wurde nicht eingelesen\n");
        return -1;
    } else {
        printf("Datei erforlgreich eingelesen\n");
    }
    struct HEADER head;
    fread(&head, sizeof(head), 1, input);
    /*printf("%d\n",head.bits_per_sample);
        Anmerkung: Eigentlich müsste eine Differenzierung stattfinden, wie hoch die Audiodatei auflößt.
        Andernfalls ist das Programm (Bei z.B. 16 BIT PCM Datein) Fehlerhaft.
     */
    float* dataneu = Audiofilter(data,data_size);
    writePCM("Aufgabe3.wav",dataneu,data_size,head);


    fclose(input);

    return 0;
}