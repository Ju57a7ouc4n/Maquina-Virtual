#include <stdio.h>
#define REGISTROS 16
#define SEGMENTOS 6
#define ENTRADAS 2
#define MEMORIA_MAXIMA 16384

typedef struct{
    unsigned int REG[REGISTROS],R=REGISTROS; //Registros del Procesador
    unsigned short SEG[ENTRADAS][SEGMENTOS]; //Tabla de Segmentos 
    char RAM[MEMORIA]; //Memoria Principal, char para que la celda ocupe un byte
    unsigned short error = 0; //tipos de errores en errores.h
}TVM;