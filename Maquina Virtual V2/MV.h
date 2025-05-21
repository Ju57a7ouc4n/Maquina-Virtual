#define REGISTROS 16
#define SEGMENTOS 6
#define ENTRADAS 2
#define SECCIONES 8
#define MEMORIA 16384
#define CANTCELDA 4

typedef struct TVM{
    int REG[REGISTROS]; //Registros del Procesador
    short int SEG[SECCIONES][ENTRADAS]; //Tabla de Segmentos 
    unsigned char *RAM; //Memoria Principal, char para que la celda ocupe un byte
    unsigned short error; //Boolean: 0 si esta todo bien, 1 instruccion invalida, 2 division por cero, 3 fallo de segmento, 4 error de memoria
}TVM;