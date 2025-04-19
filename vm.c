#include "valores_registros.h"
#include "mascaras.h"
#include "operaciones.h"
#include "dissasembler.h"
#include <stdio.h>
#include <string.h>
#include <string.h>
#define REGISTROS 16
#define SEGMENTOS 2
#define ENTRADAS 8
#define MEMORIA 16384
#define ICS 0
#define OP1 9
#define OP2 15

int memologitofisica(unsigned short SEG[ENTRADAS][SEGMENTOS],int);

int verificacabecera(char[]);

int armaInmediato(char car1, char car2);

int armaMemoria(char car1, char car2, char car3);

void cargaCS(TVM*,char*);   

void iniciaEjecucion(TVM*, char **, int, void(*op1op[])(), void(*op2op[])());

int main(int argc, char *argv[]){      //2 operandos:  xxx1 0101
    TVM VMX;
    void (*op2op[OP2])(int, int, int, int, TVM*)={MOV, ADD, SUB, SWAP, MUL, DIV, CMP, SHL, SHR, AND, OR, XOR, LDL, LDH, RND};
    void (*op1op[OP1])(int, int, TVM*)={SYS, JMP, JZ, JP, JN, JNZ, JNP, JNN, NOT};
    if (argc > 1){
        cargaCS(&VMX,argv[1]); //Carga el segmento de codigo en la memoria
        iniciaEjecucion(&VMX,argv,argc,op1op,op2op); //Inicia la ejecucion del programa
    }
    return 0;
}

int memologitofisica(unsigned short tabla[ENTRADAS][SEGMENTOS], int dirlogica){ //Funcion que convierte una direccion logica a fisica
    int IPH, offset;
    IPH= dirlogica >> 16; //Se obtiene la base
    offset= (dirlogica<<16)>>16; //Se obtiene el offset
    return (tabla[IPH][0]+offset);
}

int verificacabecera(char vec[7]){ //Funcion que verifica la cabecera del archivo vmx
    int i=0;
    char cabecera[7]={'V','M','X','2','5','1','\0'};
    while (i<7 && vec[i]==cabecera[i]){
        i++;
    }
    if (i==7){
        return 1; //Cabecera correcta
    }
    else{
        return 0; //Cabecera incorrecta
    }
}

void cargaCS(TVM *VMX,char *nombreArchivo){ //Carga el segmento de codigo en la memoria
    FILE *fichero;
    unsigned char lector;
    char vec[7];
    int i=0,j=0,k=0;
    fichero=fopen(nombreArchivo,"rb");
    if (fichero==NULL){
        printf("Error al abrir el archivo\n");
    }
    else{
        while (j<7 && fread(&lector,sizeof(unsigned char),1,fichero)==1){
            vec[j]=lector;
            j++;
        }
        vec[j]='\0';
        if(verificacabecera(vec)==1){
            k=ICS;
            i=0;
            fread(&lector,sizeof(unsigned char),1,fichero);
            i|=(unsigned char)lector;
            i=i<<8;
            fread(&lector,sizeof(unsigned char),1,fichero);
            i|=(unsigned char)lector;
            i+=ICS;
            while(k<i){
                fread(&lector,sizeof(unsigned char),1,fichero);
                (*VMX).RAM[k]=lector;
                k++;
            }
            (*VMX).SEG[0][0]=ICS;
            (*VMX).SEG[0][1]=k-ICS;
            (*VMX).SEG[1][0]=(*VMX).SEG[0][0]+(*VMX).SEG[0][1];
        }
        else{
            printf("El archivo no es compatible. \n");
        }
        fclose(fichero);
    }
    
}

int armaInmediato(char car1, char car2){
    int i=0;
    i=(char)car1;
    i=i<<8;
    i|=(char)car2;
    return i;
}

int armaMemoria(char car1, char car2, char car3){
    int i=0;
    i=(char)car1;
    i=i<<16;
    i|=(char)car2;
    i=i<<8;
    i|=(char)car3;
    return i;
}

void iniciaEjecucion(TVM *VMX, char *argv[], int argc, void(*op1op[])(), void(*op2op[])() ){ //Funcion que inicia la ejecucion del programa
    int dirfisica,dirfisicaaux,topA,topB,A,B,assemb=0;
    char orden;
    (*VMX).REG[IP]=0x10;
    if(argc==3 && strcmp(argv[2],"-d")==0){ 
        printf("Iniciando la ejecucion del programa...\n");
        dirfisica=memologitofisica((*VMX).SEG,(*VMX).REG[IP]);
        while((*VMX).RAM[dirfisica] != 0x0F && (*VMX).error==0){ //Mientras no sea un stop y no hay error
            orden=((*VMX).RAM[dirfisica] & MASC_COD_OPERACION); //Se obtiene la orden a ejecutar 
            //PREGUNTAR SI LA ORDEN ES INVALIDA: cuando el codigo de operacion de la instruccion a ejecutar no existe         
            topB=(((*VMX).RAM[dirfisica] & MASC_TIPO_OP_B) >> 6) & 0xFF;
            topA=((*VMX).RAM[dirfisica] & MASC_TIPO_OP_A) >> 4;
            if(orden&0x10 == 0x10){ //Dos operandos
                printf("[%d] \t",dirfisica);
                imprimeOrdenDosOp(orden);
                switch (topB){
                    case 1: //registro.
                        B = (int)(*VMX).RAM[dirfisica+1];
                        assemb=1;
                        break;
                    case 2: //imnediato.
                        B = armaInmediato((*VMX).RAM[dirfisica+1],(*VMX).RAM[dirfisica+2]);
                        assemb=2;
                        break;
                    case 3: //memoria.
                        B = armaMemoria((*VMX).RAM[dirfisica+1],(*VMX).RAM[dirfisica+2],(*VMX).RAM[dirfisica+3]);
                        assemb=3;
                        break;
                }
                if(topA==1){ //registro.
                    imprimeRegistro((*VMX).RAM[dirfisica+topB+1]);
                    printf(",");                
                    A = (int)(*VMX).RAM[dirfisica+topB+1];
                }
                else{  //memoria.
                    imprimeMemoria((*VMX).RAM[dirfisica+topB+1],(*VMX).RAM[dirfisica+topB+2],(*VMX).RAM[dirfisica+topB+3]);
                    printf(",");
                    A = armaMemoria((*VMX).RAM[dirfisica+topB+1],(*VMX).RAM[dirfisica+topB+2],(*VMX).RAM[dirfisica+topB+3]);
                }
                switch (assemb){
                    case 1:
                        imprimeRegistro((*VMX).RAM[dirfisica+1]);
                        break;
                    case 2:
                        imprimeInmediato((*VMX).RAM[dirfisica+1],(*VMX).RAM[dirfisica+2]);
                        break;
                    case 3:
                        imprimeMemoria((*VMX).RAM[dirfisica+1],(*VMX).RAM[dirfisica+2],(*VMX).RAM[dirfisica+3]);
                        break;
                }
                printf("\n");
                op2op[orden](A,topA,B,topB,VMX);
            }
            else{ //Un operando
                switch (topB){
                    case 1: //registro.
                        imprimeOrdenUnOp(orden);
                        imprimeRegistro((*VMX).RAM[dirfisica+1]);
                        op1op[orden]((int)(*VMX).RAM[dirfisica+1],topB,VMX);
                        printf("\n");
                        break;
                    case 2: //imnediato.
                        imprimeOrdenUnOp(orden);
                        imprimeInmediato((*VMX).RAM[dirfisica+1],(*VMX).RAM[dirfisica+2]);
                        op1op[orden](armaInmediato((*VMX).RAM[dirfisica+1],(*VMX).RAM[dirfisica+2]),topB,VMX);
                        printf("\n");
                        break;
                    case 3: //memoria.
                        imprimeOrdenUnOp(orden);
                        imprimeMemoria((*VMX).RAM[dirfisica+1],(*VMX).RAM[dirfisica+2],(*VMX).RAM[dirfisica+3]);
                        op1op[orden](armaMemoria((*VMX).RAM[dirfisica+1],(*VMX).RAM[dirfisica+2],(*VMX).RAM[dirfisica+3]),topB,VMX);
                        printf("\n");
                        break;
                }
            }
        }
        printf("STOP \n");
    }
    else{
        printf("Iniciando la ejecucion del programa...\n");
        dirfisica=memologitofisica((*VMX).SEG,(*VMX).REG[IP]);
        while((*VMX).RAM[dirfisica]!=0x0F && (*VMX).error==0){ //Mientras no sea un stop y no hay error
            orden=((*VMX).RAM[dirfisica] & MASC_COD_OPERACION); //Se obtiene la orden a ejecutar         
            topB=(((*VMX).RAM[dirfisica] & MASC_TIPO_OP_B) >> 6) & 0xFF;
            topA=((*VMX).RAM[dirfisica] & MASC_TIPO_OP_A) >> 4;
            if(orden&0x10 == 0x10){ //Dos operandos
                switch (topB){
                    case 1: //registro.
                        B = (int)(*VMX).RAM[dirfisica+1];
                        break;
                    case 2: //imnediato.
                        B = armaInmediato((*VMX).RAM[dirfisica+1],(*VMX).RAM[dirfisica+2]);
                        break;
                    case 3: //memoria.
                        B = armaMemoria((*VMX).RAM[dirfisica+1],(*VMX).RAM[dirfisica+2],(*VMX).RAM[dirfisica+3]);
                        break;
                }
                if(topA==1){ //registro.                
                    A = (int)(*VMX).RAM[dirfisica+topB+1];
                }
                else{  //memoria.
                    A = armaMemoria((*VMX).RAM[dirfisica+topB+1],(*VMX).RAM[dirfisica+topB+2],(*VMX).RAM[dirfisica+topB+3]);
                }
                
                op2op[orden](A,topA,B,topB,VMX);
                
            }
            else{ //Un operando
                switch (topB){
                    case 1: //registro.
                        op1op[orden]((int)(*VMX).RAM[dirfisica+1],topB,VMX);
                        break;
                    case 2: //imnediato.
                        op1op[orden](armaInmediato((*VMX).RAM[dirfisica+1],(*VMX).RAM[dirfisica+2]),topB,VMX);
                        break;
                    case 3: //memoria.
                        op1op[orden](armaMemoria((*VMX).RAM[dirfisica+1],(*VMX).RAM[dirfisica+2],(*VMX).RAM[dirfisica+3]),topB,VMX);
                        break;
                }
            }
        }
        printf("Fin de la Ejecucion del programa...\n");
    }
}