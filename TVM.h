#include <stdio.h>
#define REGISTROS 16
#define SEGMENTOS 2
#define ENTRADAS 8
#define MEMORIA 16384

typedef struct{
    unsigned int REG[REGISTROS],R=REGISTROS; //Registros del Procesador
    unsigned short SEG[ENTRADAS][SEGMENTOS]; //Tabla de Segmentos 
    char RAM[MEMORIA]; //Memoria Principal, char para que la celda ocupe un byte
    unsigned short error = 0; //Boolean: 0 si esta todo bien, 1 instruccion invalida, 2 division por cero, 3 fallo de segmento
}TVM;