#include <stdio.h>
#include <string.h>
#include "valores_registros.h"
#include "mascaras.h"
#include "operaciones.h"
#define REGISTROS 16
#define SEGMENTOS 2
#define ENTRADAS 8
#define MEMORIA 16384
#define ICS 0
#define OP1 9
#define OP2 15

char* devuelveRegistro(unsigned char car);

char* devuelveRegistro2bytes(unsigned char car);

char* devuelveRegistroBajo(unsigned char car);

char* devuelveRegistroAlto(unsigned char car);

int memologitofisica(unsigned short SEG[ENTRADAS][SEGMENTOS],int);

int verificacabecera(char[]);

void cargaCS(char[MEMORIA], unsigned short[ENTRADAS][SEGMENTOS], const char*);   

int main(int argc, char *argv[]){      //2 operandos:  xxx1 0101
    TVM VMX;
    void (*op2op[OP2])(int, int, int, int, TVM*)={MOV, ADD, SUB, SWAP, MUL, DIV, CMP, SHL, SHR, AND, OR, XOR, LDL, LDH, RND};
    void (*op1op[OP1])(int, int, TVM*)={SYS, JMP, JZ, JP, JN, JNZ, JNP, JNN, NOT};
    cargaCS(VMX.RAM,&(VMX.SEG),argv[1]); //Carga el segmento de codigo en la memoria
    iniciaEjecucion(&VMX,argv[2], argc,op1op,op2op); //Inicia la ejecucion del programa
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

void cargaCS(char RAM[MEMORIA], unsigned short tabla[ENTRADAS][SEGMENTOS], const char *nombreArchivo){ //Carga el segmento de codigo en la memoria
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
                RAM[k]=lector;
                k++;
            }
            tabla[0][0]=ICS;
            tabla[0][1]=k-ICS;
            tabla[1][0]=tabla[0][0]+tabla[0][1];
        }
        else{
            printf("El archivo no es compatible. \n");
        }
    }
    fclose(fichero);
}

void imprimeOrdenDosOp(char orden){
    switch(orden){
        case 0x10: //MOV
            printf("MOV ");
            break;
        case 0x11: //ADD
            printf("ADD ");
            break;
        case 0x12: //SUB
            printf("SUB ");
            break;
        case 0x13: //SWAP
            printf("SWAP ");
            break; 
        case 0x14: //MUL
            printf("MUL ");
            break;
        case 0x15: //DIV
            printf("DIV ");
            break;
        case 0x16: //CMP
            printf("CMP ");
            break;
        case 0x17: //SHL
            printf("SHL ");
            break;
        case 0x18: //SHR
            printf("SHR ");
            break;
        case 0x19: //AND
            printf("AND ");
            break;
        case 0x1A: //OR
            printf("OR ");
            break;
        case 0x1B: //XOR
            printf("XOR ");
            break;
        case 0x1C: //LDL
            printf("LDL ");
            break;
        case 0x1D: //LDH
            printf("LDH ");
            break;
        case 0x1E: //RND
            printf("RND ");
            break;
    }
}

void imprimeOrdenUnOp(char orden){
    switch(orden){
        case 0x00: //SYS
            printf("SYS ");
            break;
        case 0x01: //JMP
            printf("JMP ");
            break;
        case 0x02: //JZ
            printf("JZ ");
            break;
        case 0x03: //JP
            printf("JP ");
            break;
        case 0x04: //JN
            printf("JN ");
            break;
        case 0x05: //JNZ
            printf("JNZ ");
            break;
        case 0x06: //JNP
            printf("JNP ");
            break;
        case 0x07: //JNN
            printf("JNN ");
            break;
        case 0x08: //NOT
            printf("NOT ");
            break;
    }
}

void imprimeInmediato(char car1, char car2){
    int i=0;
    i=(char)car1;
    i=i<<8;
    i|=(char)car2;
    printf("%d",i);
}

int armaInmediato(char car1, char car2){
    int i=0;
    i=(char)car1;
    i=i<<8;
    i|=(char)car2;
    return i;
}

void imprimeMemoria(char car1, char car2, char car3){
    int i=0;
    unsigned char aux;
    i=(char)car1;
    i=i<<8;
    i|=(char)car2;
    aux=((unsigned char)car3>>4);
    printf("[%d+%s]",i,devuelveRegistro(aux));
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

void imprimeRegistro(char car){
    char aux;
    aux=car & 0x0B;
    switch(aux){
        case 0x00:
            printf("%s",devuelveRegistro(car));
            break;
        case 0x0B:
            printf("%s",devuelveRegistro2bytes(car));
            break;
        case 0x08:
            printf("%s",devuelveRegistroBajo(car));
            break;
        case 0x04:
            printf("%s",devuelveRegistroAlto(car));
            break;
    }
}

char* devuelveRegistro(unsigned char car){
    switch(car){
        case 0x00:
            return "CS";
            break;
        case 0x01:  
            return "DS";
            break;
        case 0x05:
            return "IP";
            break;
        case 0x08:
            return "CC";
            break;
        case 0x10:
            return "EAX";
            break;
        case 0x11:
            return "EBX";
            break;
        case 0x12:
            return "ECX";
            break;
        case 0x13:
            return "EDX";
            break;
        case 0x14:
            return "EEX";
            break;
        case 0x15:
            return "EFX";
    }
}

char* devuelveRegistro2bytes(unsigned char car){
    switch(car){
        case 0x10:
            return "AX";
            break;
        case 0x11:
            return "BX";
            break;
        case 0x12:
            return "CX";
            break;
        case 0x13:
            return "DX";
            break;
        case 0x14:
            return "EX";
            break;
        case 0x15:
            return "FX";
    }
}

char* devuelveRegistroBajo(unsigned char car){
    switch(car){
        case 0x10:
            return "AL";
            break;
        case 0x11:
            return "BL";
            break;
        case 0x12:
            return "CL";
            break;
        case 0x13:
            return "DL";
            break;
        case 0x14:
            return "EL";
            break;
        case 0x15:
            return "FL";
    }
}

char* devuelveRegistroAlto(unsigned char car){
    switch(car){
        case 0x10:
            return "AH";
            break;
        case 0x11:
            return "BH";
            break;
        case 0x12:
            return "CH";
            break;
        case 0x13:
            return "DH";
            break;
        case 0x14:
            return "EH";
            break;
        case 0x15:
            return "FH";
    }
}

void iniciaEjecucion(TVM *VMX, char *parametro, int argc, void(*op1op[])(), void(*op2op[])() ){ //Funcion que inicia la ejecucion del programa
    int dirfisica,dirfisicaaux,topA,topB,A,B,assemb=0;
    char orden;
    (*VMX).REG[IP]=0x10;
    if(argc==3 && strcmp(parametro,"-d")==0){
        printf("Iniciando la ejecucion del programa...\n");
        dirfisica=memologitofisica((*VMX).SEG,(*VMX).REG[IP]);
        while((*VMX).RAM[dirfisica] != 0x0F){ //Mientras no sea un stop
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
        while((*VMX).RAM[dirfisica]!=0x0F){ //Mientras no sea un stop
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