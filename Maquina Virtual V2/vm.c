#include "valores_registros.h"
#include "mascaras.h"
#include "segmentos.h"
#include "operaciones.h"
#include "dissasembler.h"
#include <stdio.h>
#include <string.h>
#include <string.h>
#define OP1 9
#define OP2 15
#define NULO -1

int memologitofisica(unsigned short SEG[ENTRADAS][SEGMENTOS],unsigned int);

int verificacabecera(char[]);

int armaInmediato(char car1, char car2);

int armaMemoria(char car1, char car2, char car3);

void cargaParametro(TVM*, char*, int*, int*, int*);

void cargaPS(TVM*,char**,int,int*);

void cargaCS(TVM*,char*,int*,int);   

void iniciaEjecucion(TVM*, char **, int, void(*op1op[])(), void(*op2op[])());

int main(int argc, char *argv[]){ 
    TVM VMX;
    VMX.error=0;
    int compatible=1,k,i,j=0,TPS=0;
    void (*op2op[OP2])(int, int, int, int, TVM*)={MOV, ADD, SUB, SWAP, MUL, DIV, CMP, SHL, SHR, AND, OR, XOR, LDL, LDH, RND};
    void (*op1op[OP1])(int, int, TVM*)={SYS, JMP, JZ, JP, JN, JNZ, JNP, JNN, NOT};
    cargaPS(&VMX,argv,argc,&TPS); //Carga el segmento de datos en la memoria
    if (argc > 1){
        cargaCS(&VMX,argv[1],&compatible,TPS); //Carga el segmento de codigo en la memoria
        (compatible)?iniciaEjecucion(&VMX,argv,argc,op1op,op2op):printf("El archivo no es compatible. \n"); //Inicia la ejecucion del programa
    }
}

void cargaPS(TVM *VMX,char *argv[],int argc,int *TPS){ //Carga el segmento de datos en la memoria
    int i=0,vp[100],cantp=0,k=0,sig=0;
    while(i<argc && argv[i]!="-p")
        i++;
    if(argv[i]=="-p"){
        while(i<argc){
            i++;
            cargaParametro(VMX,argv[i],vp,&cantp,&sig); //cantp: cantidad de punteros
        }
        while(k<cantp){ //sig: proxima posicion en la memoria donde se escribe el puntero del parametro K
            (*VMX).RAM[sig]=(vp[k] & MASC_BYTE1)>>24;
            (*VMX).RAM[sig+1]=(vp[k] & MASC_BYTE2)>>16;
            (*VMX).RAM[sig+2]=(vp[k] & MASC_BYTE3)>>8;
            (*VMX).RAM[sig+3]=(vp[k] & MASC_BYTE4);
            sig+=4;
            k++;
        }
    }
    *TPS=sig;
}

void cargaParametro(TVM *VMX, char *argv, int *vp, int *cantp, int *sig){
    int tamanio= strlen(argv);
    int i=0;
    vp[*cantp]=(*sig); //Se guarda la posicion del puntero
    (*cantp)++;
    while(i<tamanio){
        (*VMX).RAM[(*sig)]=argv[i];
        (*sig)++;
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

void iniciaRegistros(TVM *VMX, int TSS, int EP){
    (*VMX).REG[CS]=((*VMX).SEG[codCS][0]<<16);
    (*VMX).REG[DS]=((*VMX).SEG[codDS][0]<<16);
    (*VMX).REG[ES]=((*VMX).SEG[codES][0]<<16);
    (*VMX).REG[SS]=((*VMX).SEG[codSS][0]<<16);
    (*VMX).REG[KS]=((*VMX).SEG[codKS][0]<<16);
    (*VMX).REG[SP]=(*VMX).REG[SS]+TSS;
    (*VMX).REG[IP]=(*VMX).SEG[codCS][0]<<16;
    (*VMX).REG[IP]=+EP;
}

int sufMemo(int TPS, int TCS, int TDS, int TKS, int TES, int TSS){ //Funcion que verifica si hay suficiente memoria
    if (TPS+TCS+TDS+TKS+TES+TSS>MEMORIA){
        return 0; //No hay suficiente memoria
    }
    else{
        return 1; //Hay suficiente memoria
    }
}

void armaTabla(TVM *VMX, int TPS, int TCS, int TDS, int TKS, int TES, int TSS, int EP){ //Arma la tabla de segmentos
    int TamAcum=TPS;
    if(sufMemo(TPS,TCS,TDS,TKS,TES,TSS)==1){
        if(TKS==0){
            (*VMX).SEG[codKS][0]=NULO; //Base del Constant Segment
            (*VMX).SEG[codKS][1]=NULO; //Tamanio del Constant Segment
        }
        else{
            (*VMX).SEG[codKS][0]=TPS; //Base del Constant Segment
            (*VMX).SEG[codKS][1]=TKS; //Tamanio del Constant Segment
            TamAcum+=TKS;
        }
        (*VMX).SEG[codCS][0]=TamAcum; //Base del code segment
        (*VMX).SEG[codCS][1]=TCS; //Tamanio del segmento de codigo
        TamAcum+=TCS;
        (*VMX).SEG[codDS][0]=TamAcum; //Segmento de datos
        (*VMX).SEG[codDS][1]=TDS; //Tamanio del segmento de datos
        TamAcum+=TDS;
        if(TES==0){
            (*VMX).SEG[codES][0]=NULO; //Base del Extra Segment
            (*VMX).SEG[codES][1]=NULO; //Tamanio del Extra Segment    
        }
        else{
            (*VMX).SEG[codES][0]=TamAcum; //Base del Extra Segment
            (*VMX).SEG[codES][1]=TES; //Tamanio del Extra Segment
            TamAcum+=TES;
        }
        (*VMX).SEG[codSS][0]=TamAcum; //Base de la Pila
        (*VMX).SEG[codSS][1]=TSS; //Tamanio de la Pila
        iniciaRegistros(VMX,TSS,EP); //Inicia los registros
    }
    else{
        (*VMX).error=4; //Error: No hay suficiente memoria
    }
}

/*void armaTablaJOUI(TVM *VMX, int VecTamSegmentos[],int TamTotal){
    int i,memAcum;
    if(sufMemo(VecTamSegmentos)){
        memAcum=0;
        for (i=0,i<SEGMENTOS,i++){
            if (VecTamSegmentos[i]==0){
                (*VMX).SEG[i][0]=NULL;
                (*VMX).SEG[i][1]=NULL
            }
            else{
                (*VMX).SEG[i][0]=memAcum;
                (*VMX).SEG[i][1]=VecTamSegmentos[i];
                memAcum+=VecTamSegmentos[i];
            }
         iniciaRegistros(VMX); //Inicia los registros
        }    
    }
    else
        (*VMX).error=4; //Error: No hay suficiente memoria
}
*/

void cargaCS(TVM *VMX,char *nombreArchivo,int *compatible, int TPS){ //Carga el segmento de codigo en la memoria
    FILE *fichero;
    unsigned char lector;
    char vec[5];
    int i=0,j=0,TCS=0,TDS=0,TES=0,TSS=0,TKS=0,OfssetEP=0, dirBaseCS=0;
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
            if(lector==1){
                fread(&lector,sizeof(unsigned char),1,fichero); //Tamanio del codigo
                TCS|=lector;
                TCS=TCS<<8;
                fread(&lector,sizeof(unsigned char),1,fichero);
                TCS|=lector;
            }
            else{
                if(lector==2){
                    fread(&lector,sizeof(unsigned char),1,fichero);
                    TCS|=lector;
                    TCS=TCS<<8;
                    fread(&lector,sizeof(unsigned char),1,fichero);
                    TCS|=lector;
                    fread(&lector,sizeof(unsigned char),1,fichero);
                    TDS|=lector;
                    TDS=TDS<<8;
                    fread(&lector,sizeof(unsigned char),1,fichero);
                    TDS|=lector;
                    fread(&lector,sizeof(unsigned char),1,fichero);
                    TES|=lector;
                    TES=TES<<8;
                    fread(&lector,sizeof(unsigned char),1,fichero);
                    TES|=lector;
                    fread(&lector,sizeof(unsigned char),1,fichero);
                    TSS|=lector;
                    TSS=TSS<<8;
                    fread(&lector,sizeof(unsigned char),1,fichero);
                    TSS|=lector;
                    fread(&lector,sizeof(unsigned char),1,fichero);
                    TKS|=lector;
                    TKS=TKS<<8;
                    fread(&lector,sizeof(unsigned char),1,fichero);
                    TKS|=lector;
                    fread(&lector,sizeof(unsigned char),1,fichero);
                    OfssetEP|=lector;
                    OfssetEP=OfssetEP<<8;
                    fread(&lector,sizeof(unsigned char),1,fichero);
                    OfssetEP|=lector;
                }
                else{
                    *compatible=0;
                }
            }
            if(*compatible){
                armaTabla(VMX,TPS,TCS,TDS,TKS,TES,TSS,OfssetEP); //Arma la tabla de segmentos
                while(i<TCS){
                    fread(&lector,sizeof(unsigned char),1,fichero);
                    dirBaseCS=memologitofisica((*VMX).SEG,(*VMX).REG[CS]);
                    (*VMX).RAM[dirBaseCS+i]=lector;
                    printf("[%x]: %x\n",dirBaseCS+i,lector);
                    i++;
                }
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
    if(strcmp(argv[2],"-d")==0)
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
        case 4: printf("Error: No hay suficiente memoria \n");
                break;
    }
}




