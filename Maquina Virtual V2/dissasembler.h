#include <stdio.h>

void imprimeMemoria(char car1, char car2, char car3);

void imprimeInmediato(char car1, char car2);

void imprimeOrdenUnOp(char orden);

void imprimeOrdenDosOp(char orden);

void imprimeRegistro(char car);

char* devuelveRegistro(unsigned char car);

char* devuelveRegistro2bytes(unsigned char car);

char* devuelveRegistroBajo(unsigned char car);

char* devuelveRegistroAlto(unsigned char car);

int memologitofisica(short int SEG[SECCIONES][ENTRADAS],unsigned int);

void llamadissasembler(TVM*);