#include "valores_registros.h"
#include "mascaras.h"
#include "operaciones.h"
#include "dissasembler.h"
#include <stdio.h>
#include <string.h>
#include <string.h>
#define REGISTROS 16
#define MEMORIA 16384
#define ICS 0
#define OP1 9
#define OP2 15
#define SEGMENTOS 2
#define ENTRADAS 8

int memologitofisica(unsigned short SEG[ENTRADAS][SEGMENTOS],unsigned int);

int verificacabecera(char[]);

int armaInmediato(char car1, char car2);

int armaMemoria(char car1, char car2, char car3);

void cargaCS(TVM*,char*,int*);   

void iniciaEjecucion(TVM*, char **, int, void(*op1op[])(), void(*op2op[])());

int main(int argc, char *argv[]){ 
    TVM VMX;
    VMX.error=0;
    int compatible=1,k,i,j=0;
    void (*op2op[OP2])(int, int, int, int, TVM*)={MOV, ADD, SUB, SWAP, MUL, DIV, CMP, SHL, SHR, AND, OR, XOR, LDL, LDH, RND};
    void (*op1op[OP1])(int, int, TVM*)={SYS, JMP, JZ, JP, JN, JNZ, JNP, JNN, NOT};
    if (argc > 1){
        cargaCS(&VMX,argv[1],&compatible); //Carga el segmento de codigo en la memoria
        (compatible)?iniciaEjecucion(&VMX,argv,argc,op1op,op2op):printf("El archivo no es compatible. \n"); //Inicia la ejecucion del programa
    }
}

int memologitofisica(unsigned short tabla[ENTRADAS][SEGMENTOS], unsigned int dirlogica){ //Funcion que convierte una direccion logica a fisica
    unsigned int IPH=0, offset=0;
    offset|=(dirlogica & 0x0000FFFF); //Se obtiene el offset
    IPH=(unsigned int)((dirlogica  & 0xFFFF0000) >> 16); //Se obtiene la base 
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

void iniciaRegistros(TVM *VMX,int posCS,int posDS){
    (*VMX).REG[CS] = posCS << 16; 
    (*VMX).REG[IP] = (*VMX).REG[CS];
    (*VMX).REG[DS] = posDS << 16;   
}

void armaTabla(TVM *VMX, int tamanioCS,int posCS,int posDS){ //Arma la tabla de segmentos
    (*VMX).SEG[posCS][0]=ICS; //Segmento de codigo BASE
    (*VMX).SEG[posCS][1]=tamanioCS; //Tamanio del segmento de codigo
    (*VMX).SEG[posDS][0]=(*VMX).SEG[0][0]+(*VMX).SEG[0][1]; //Segmento de datos
    (*VMX).SEG[posDS][1]=MEMORIA-(*VMX).SEG[0][1]; //Tamanio del segmento de datos
    iniciaRegistros(VMX,posCS,posDS); //Inicia los registros
}

void cargaCS(TVM *VMX,char *nombreArchivo,int *compatible){ //Carga el segmento de codigo en la memoria
    FILE *fichero;
    unsigned char lector;
    char vec[5];
    int i=0,j=0,TCS=0, posCS = 0, posDS = 1; 
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
            armaTabla(VMX,TCS,posCS,posDS); //Arma la tabla de segmentos
            
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
    i = (char)car1 << 8;
    i|= (unsigned char)car2;
    i = (i<<16)>>16;  // expande signo si es negativo
    return i;
}

int armaMemoria(char car1, char car2, char car3){  
    unsigned int i = 0;
    i |= ((unsigned char)car1) << 16;
    i |= ((unsigned char)car2) << 8;
    i |= ((unsigned char)car3);
    return i;
}

void iniciaEjecucion(TVM *VMX, char *argv[], int argc, void(*op1op[])(), void(*op2op[])() ){ //Funcion que inicia la ejecucion del programa
    int dirfisica,dirfisicaTCS,topA,topB,A,B,assemb=0;
    char orden;
    int indiceCS = (unsigned int)(*VMX).REG[CS]>>16;
    if(argc==3 && strcmp(argv[2],"-d")==0){
        llamadissasembler(VMX); //Desensambla el programa
    dirfisica=memologitofisica((*VMX).SEG,(*VMX).REG[IP]);
    printf("Iniciando la ejecucion del programa...\n");
    dirfisica=memologitofisica((*VMX).SEG,(*VMX).REG[IP]);
    while((*VMX).RAM[dirfisica]!=0x0F && (*VMX).error==0 && dirfisica<((*VMX).SEG[indiceCS][0] + (*VMX).SEG[indiceCS][1])){ //Mientras no sea un stop, no hay error y esta dentro del CS
        orden=((*VMX).RAM[dirfisica] & MASC_COD_OPERACION); //Se obtiene la orden a ejecutar   
        if(!(orden>=0x00 && orden<=0x08) && !(orden>=0x10 && orden<=0x1E)){ ///PREGUNTAR SI LA ORDEN ES INVALIDA: cuando el codigo de operacion de la instruccion a ejecutar no existe 
            (*VMX).error= 1;
        }
        else{      
            topB=(((*VMX).RAM[dirfisica] & MASC_TIPO_OP_B) >> 6);
            topA=((*VMX).RAM[dirfisica] & MASC_TIPO_OP_A) >> 4;
            if((orden&0x10) == 0x10){ //Dos operandos
                (*VMX).REG[IP] = ((*VMX).REG[CS]) | (((*VMX).REG[IP] & 0x00007FFF) + topA + topB + 1); //Se actualiza el registro IP
                switch (topB){
                    case 0x01: //registro.
                        B=0;
                        B |= (*VMX).RAM[dirfisica+1];
                        break;
                    case 0x02: //imnediato.
                        B=0;
                        B = armaInmediato((*VMX).RAM[dirfisica+1],(*VMX).RAM[dirfisica+2]);
                        break;
                    case 0x03: //memoria.
                        B=0;
                        B = armaMemoria((*VMX).RAM[dirfisica+1],(*VMX).RAM[dirfisica+2],(*VMX).RAM[dirfisica+3]);
                        break;
                }
                if(topA==0x01){ //registro.       
                    A=0;         
                    A = (int)(*VMX).RAM[dirfisica+topB+1];
                }
                else{  //memoria.
                    A=0;
                    A = armaMemoria((*VMX).RAM[dirfisica+topB+1],(*VMX).RAM[dirfisica+topB+2],(*VMX).RAM[dirfisica+topB+3]);
                }
                op2op[(orden & 0x0F)](A,topA,B,topB,VMX);
            }
            else{ //Un operando
                (*VMX).REG[IP] = ((*VMX).REG[CS]) | (((*VMX).REG[IP] & 0x00007FFF) + topB + 1); //Se actualiza el registro IP
                switch (topB){
                    case 0x01: //registro.
                        op1op[(orden & 0x0F)]((int)(*VMX).RAM[dirfisica+1],topB,VMX);
                        break;
                    case 0x02: //imnediato.
                        op1op[(orden & 0x0F)](armaInmediato((*VMX).RAM[dirfisica+1],(*VMX).RAM[dirfisica+2]),topB,VMX);
                        break;
                    case 0x03: //memoria.
                        op1op[(orden &0x0F)](armaMemoria((*VMX).RAM[dirfisica+1],(*VMX).RAM[dirfisica+2],(*VMX).RAM[dirfisica+3]),topB,VMX);
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
        case 3: printf("Error: Fallo de segmento \n");
                break;
    }
    }
}




