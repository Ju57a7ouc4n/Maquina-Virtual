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

int memologitofisica(unsigned short SEG[ENTRADAS][SEGMENTOS],unsigned int);

int verificacabecera(char[]);

int armaInmediato(char car1, char car2);

int armaMemoria(char car1, char car2, char car3);

void cargaCS(TVM*,char*,int*);   

void iniciaEjecucion(TVM*, char **, int, void(*op1op[])(), void(*op2op[])());

int main(int argc, char *argv[]){      //2 operandos:  xxx1 0101
    TVM VMX;
    VMX.error=0;
    int compatible=1,k;
    void (*op2op[OP2])(int, int, int, int, TVM*)={MOV, ADD, SUB, SWAP, MUL, DIV, CMP, SHL, SHR, AND, OR, XOR, LDL, LDH, RND};
    void (*op1op[OP1])(int, int, TVM*)={SYS, JMP, JZ, JP, JN, JNZ, JNP, JNN, NOT};
    if (argc > 1){
        cargaCS(&VMX,argv[1],&compatible); //Carga el segmento de codigo en la memoria
        (compatible)?iniciaEjecucion(&VMX,argv,argc,op1op,op2op):printf("El archivo no es compatible. \n"); //Inicia la ejecucion del programa
    }
    return 0;
}

int memologitofisica(unsigned short tabla[ENTRADAS][SEGMENTOS], unsigned int dirlogica){ //Funcion que convierte una direccion logica a fisica
    int IPH=0, offset=0;
    IPH=((dirlogica&0xFFFF0000) >> 16); //Se obtiene la base
    offset|=dirlogica; //Se obtiene el offset
    printf("%d \n",dirlogica);
    return (tabla[IPH][0]+offset);
}

int verificacabecera(char vec[5]){ //Funcion que verifica la cabecera del archivo vmx
    int i=0;
    char cabecera[5]={'V','M','X','2','5'};
    while (i<5 && vec[i]==cabecera[i]){
        i++;
    }
    if (i==5){
        return 1; //Cabecera correcta
    }
    else{
        return 0; //Cabecera incorrecta
    }
}

void cargaCS(TVM *VMX,char *nombreArchivo,int *compatible){ //Carga el segmento de codigo en la memoria
    FILE *fichero;
    unsigned char lector;
    char vec[7];
    int i=0,j=0,TCS=0;
    fichero=fopen(nombreArchivo,"rb");
    if (fichero==NULL){
        printf("Error al abrir el archivo\n");
    }
    else{
        while (j<5 && fread(&lector,sizeof(unsigned char),1,fichero)==1){
            vec[j]=lector;
            j++;
        }
        if(verificacabecera(vec)==1){
            fread(&lector,sizeof(unsigned char),1,fichero); //Version
            
            fread(&lector,sizeof(unsigned char),1,fichero); //Tamanio del codigo
            TCS|=lector;
            TCS=TCS<<8;
            fread(&lector,sizeof(unsigned char),1,fichero);
            TCS|=lector;
            (*VMX).SEG[0][1]=TCS;
            i=0;
            (*VMX).SEG[0][0]=ICS;
            (*VMX).SEG[1][0]=(*VMX).SEG[0][0]+(*VMX).SEG[0][1];
            (*VMX).SEG[1][1]=MEMORIA-(*VMX).SEG[0][1]; //Tamanio del segmento de datos
            *compatible=1;
            
            while(i<VMX->SEG[0][1]){
                fread(&lector,sizeof(unsigned char),1,fichero);
                (*VMX).RAM[ICS+i]=lector;
                i++;
            }
        }
        else{
            *compatible=0;
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
    int dirfisica,dirfisicaTCS,topA,topB,A,B,assemb=0;
    char orden;
    (*VMX).REG[CS] = 0x00;
    (*VMX).REG[IP] = 0x00;  // SEG[0][1]
    (*VMX).REG[DS] = 0x00+13;        //  REG[DS] = 00000000 00000001 00000000 00000000
    if(argc==3 && strcmp(argv[2],"-d")==0){
        printf("%u \n",(*VMX).REG[IP]);
        dirfisica=memologitofisica((*VMX).SEG,(*VMX).REG[IP]);
        while((*VMX).error==0 && (*VMX).RAM[dirfisica] != 0x0F){ //Mientras no sea un stop y no hay error
            orden=((*VMX).RAM[dirfisica] & MASC_COD_OPERACION); //Se obtiene la orden a ejecutar 
            if(!(orden>=0x00 && orden<=0x08) && !(orden>=0x0F && orden<=0x1E)){ ///PREGUNTA SI LA ORDEN ES INVALIDA: cuando el codigo de operacion de la instruccion a ejecutar no existe 
                (*VMX).error= 1;
            }
            else{          
                topB=(((*VMX).RAM[dirfisica] & MASC_TIPO_OP_B) >> 6) & 0xFF;
                topA=((*VMX).RAM[dirfisica] & MASC_TIPO_OP_A) >> 4;
                if(orden&0x10 == 0x10){ //Dos operandos
                    (*VMX).REG[IP]+=topA+topB+1; //Se actualiza el registro IP
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
                    (*VMX).REG[IP]+=topB+1; //Se actualiza el registro IP
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
                dirfisica=memologitofisica((*VMX).SEG,(*VMX).REG[IP]);
            }
        }
        switch ((*VMX).error){
            case 0: printf("STOP \n");
                    break;
            case 1: printf("Error: Instruccion no valida \n");
                    break;
            case 2: printf("Error: Division por cero \n");
                    break;
            case 3: printf("Error: Segmento no valido \n");
                    break;
        }
    }
    else{
        printf("Iniciando la ejecucion del programa...\n");
        dirfisica=memologitofisica((*VMX).SEG,(*VMX).REG[IP]);
        while((*VMX).RAM[dirfisica]!=0x0F && (*VMX).error==0){ //Mientras no sea un stop y no hay error
            orden=((*VMX).RAM[dirfisica] & MASC_COD_OPERACION); //Se obtiene la orden a ejecutar   
            if(!(orden>=0x00 && orden<0x08) && !(orden>=0x10 && orden<=0x1E)){ ///PREGUNTAR SI LA ORDEN ES INVALIDA: cuando el codigo de operacion de la instruccion a ejecutar no existe 
                (*VMX).error= 1;
            }
            else{      
                topB=(((*VMX).RAM[dirfisica] & MASC_TIPO_OP_B) >> 6) & 0xFF;
                topA=((*VMX).RAM[dirfisica] & MASC_TIPO_OP_A) >> 4;
                if(orden&0x10 == 0x10){ //Dos operandos
                    (*VMX).REG[IP]+=topA+topB+1; //Se actualiza el registro IP
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
                    (*VMX).REG[IP]+=topB+1; //Se actualiza el registro IP
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
            dirfisica=memologitofisica((*VMX).SEG,(*VMX).REG[IP]);
            }
        }
        switch ((*VMX).error){
            case 0: printf("Fin de la Ejecucion del programa...\n");
                    break;
            case 1: printf("Error: Instruccion no valida \n");
                    break;
            case 2: printf("Error: Division por cero \n");
                    break;
            case 3: printf("Error: Segmento no valido \n");
                    break;
        }
    }
}