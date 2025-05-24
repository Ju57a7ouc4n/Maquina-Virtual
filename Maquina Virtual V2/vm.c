#include "valores_registros.h"
#include "mascaras.h"
#include "operaciones.h"
#include "dissasembler.h"
#include <stdio.h>
#include <string.h>
#include <string.h>
#include <stdlib.h>
#define OP1 15
#define OP2 15
#define NULO -1

int memologitofisica(short int tabla[SECCIONES][ENTRADAS], unsigned int dirlogica);

int verificacabecera(char[]);

int armaInmediato(char car1, char car2);

int armaMemoria(char car1, char car2, char car3);

void cargaPS(TVM*,char**,int,int*,int*,int*);

void cargaCS(TVM*,char*,int*,int,size_t);   

void iniciaEjecucion(TVM*, char **, int, void(*op1op[])(), void(*op2op[])(),int,size_t);

void iniciaRAM(int, char*[], TVM*,size_t*);

int detectaArch(char *argv[]);

void cargaVMI(TVM*,char*,int*,size_t*);

int verificaVMI(char vec[6]);

void sys_breakpoint(TVM*, size_t, char *[],int*);

void Inicializacion(TVM*,char*,int*,int,size_t);

void IniciaPila(TVM*,int,int);

int main(int argc, char *argv[]){ 
    TVM VMX;
    size_t tamanioRAM = 16384;
    VMX.error=0;
    int compatible=1,k,i,j=0,TPS=0,tarch=0,breakdown=0,PPP,cantp;
    void (*op2op[OP2])(int, int, int, int, TVM*)={MOV, ADD, SUB, SWAP, MUL, DIV, CMP, SHL, SHR, AND, OR, XOR, LDL, LDH, RND};
    void (*op1op[OP1])(int, int, TVM*)={SYS, JMP, JZ, JP, JN, JNZ, JNP, JNN, NOT,NULL,NULL,PUSH,POP,CALL,RET};
    tarch=detectaArch(argv);
    if (tarch==1 || tarch==2){
        iniciaRAM(argc,argv,&VMX,&tamanioRAM); //Inicia la memoria RAM
        cargaPS(&VMX,argv,argc,&TPS,&PPP,&cantp); //Carga el segmento de datos en la memoria
        Inicializacion(&VMX,argv[1],&compatible,TPS,tamanioRAM); //Carga el segmento de codigo en la memoria desde un .vmx
        IniciaPila(&VMX,PPP,cantp);
        if(tarch==2)
            breakdown=1;
        else
            breakdown=0;
    }
    else{
            if (tarch==3)
                cargaVMI(&VMX,argv[1],&compatible,&tamanioRAM); //Carga el segmento de datos en la memoria desde un .vmi
            else
                printf("Error: No se ha indicado un archivo .vmx o .vmi\n");
    }
    (compatible)?iniciaEjecucion(&VMX,argv,argc,op1op,op2op,breakdown,tamanioRAM):printf("El archivo no es compatible. \n"); //Inicia la ejecucion del programa
    free(VMX.RAM); //Libera la memoria RAM
}
//LA VEDETTE DE LA MAQUINA VIRTUAL
int memologitofisica(short int tabla[SECCIONES][ENTRADAS], unsigned int dirlogica){ //Funcion que convierte una direccion logica a fisica
    unsigned int IPH=0, offset=0;
    if (dirlogica != NULO){
        offset|=(dirlogica & 0x0000FFFF); //Se obtiene el offset
        IPH=(unsigned int)((dirlogica  & 0xFFFF0000) >> 16); //Se obtiene la base 
        return (int)(tabla[IPH][0]+offset);
    } 
    else
        return NULO;
}

int detectaArch(char *argv[]){ //Funcion que detecta si el archivo es un .vmx, un .vmi o ambos
    int i;
    if(strstr(argv[1],".vmx")!=NULL){
        if(strstr(argv[2],".vmi")==NULL)
            i = 1; //Solo .vmx
        else
            i = 2; //Ambos
    }
    else{
        if(strstr(argv[1],".vmi")!=NULL)
            i = 3; //Solo .vmi
        else
            i = 0; //Ninguno
    }
    return i;
}

void sys_breakpoint(TVM *VMX, size_t tamanioRAM, char *argv[],int *otro_breakpoint){
    char accion;
    generaVMI(*VMX,tamanioRAM,argv[2]);
    
    do{
        scanf(" %c,",&accion);
    } while(accion != 'g' && accion != 'q' && accion != '\n');
    
    switch(accion){
        case 'g': //go
            (*otro_breakpoint) = 0; //para continuar la ejecucion hasta el siguiente breakpoint o finalizar
        break;

        case 'q'://quit                  
            (*VMX).REG[IP] = NULO;  //para abortar la ejecucion
        break;

        case '\n': //sig instruccion + breakpoint
            (*otro_breakpoint) = 1; //no cambia
        break;
    }
}

void cargaVMI(TVM *VMX,char *nombreArchivo,int *compatible,size_t *tamanioRAM){
    FILE *fichero;
    short unsigned int m=0;
    int aux=0,dirfisica,base,offset;
    unsigned char lector;
    char vec[6];
    fichero=fopen(nombreArchivo,"rb");
    if(fichero!=NULL){
        for(int i=0;i<6;i++){
            fread(&lector,sizeof(unsigned char),1,fichero);
            vec[i]=lector;
        }
        if (verificaVMI(vec)==1){
            fread(&lector,sizeof(unsigned char),1,fichero);
            aux=lector;
            aux=aux<<8;
            fread(&lector,sizeof(unsigned char),1,fichero);
            aux|=lector;
            *tamanioRAM=aux*1024;
            VMX->RAM = (unsigned char*)malloc(*tamanioRAM * sizeof(unsigned char));
            for(int j=0;j<16;j++){ //Este ciclo arma los registros
                fread(&lector,sizeof(unsigned char),1,fichero);
                (*VMX).REG[j]|(lector<<24);
                fread(&lector,sizeof(unsigned char),1,fichero);
                (*VMX).REG[j]|(lector<<16);
                fread(&lector,sizeof(unsigned char),1,fichero);
                (*VMX).REG[j]|(lector<<8);
                fread(&lector,sizeof(unsigned char),1,fichero);
                (*VMX).REG[j]|lector;
            }
            for(int k=0;k<8;k++){ //Este ciclo arma la bendita tabla de segmentos
                fread(&lector,sizeof(unsigned char),1,fichero);
                base|lector<<8;
                fread(&lector,sizeof(unsigned char),1,fichero);
                base|lector;
                fread(&lector,sizeof(unsigned char),1,fichero);
                offset|lector<<8;
                fread(&lector,sizeof(unsigned char),1,fichero);
                offset|lector;
                (*VMX).SEG[k][0]=base;
                (*VMX).SEG[k][1]=offset;
            }
            m=0;
            dirfisica=memologitofisica((*VMX).SEG,(*VMX).REG[CS]);
            while(fread(&lector,sizeof(unsigned char),1,fichero)==1){
                (*VMX).RAM[dirfisica]=lector;
                m+=1;
                dirfisica=memologitofisica((*VMX).SEG, m);
            }
            *compatible=1;
        }
        else{
            *compatible=0;
            printf("Error: Archivo .vmi no compatible.\n");
        }
    }
}

int verificaVMI(char vec[6]){ //Funcion que verifica la cabecera del archivo vmx
    int i=0;
    char cabecera[6]={'V','M','I','2','5','1'};
    while (i<6 && vec[i]==cabecera[i]){
        i++;
    }   
    if (i==6){
        return 1; //Cabecera correcta
    }
    else{
        return 0; //Cabecera incorrecta
    }
}

void iniciaRAM(int argc, char *argv[], TVM *VMX,size_t *tamanioRAM){ //Inicia la memoria RAM
    int i = 0;

    // Buscar el argumento m=M
    while(i < argc) {
        if (strncmp(argv[i], "m=", 2) == 0) { // Verifica si es m=M
            *tamanioRAM = (size_t)atoi(argv[i] + 2);
        }
        i++;
    }
    (*VMX).RAM = (unsigned char*)malloc((*tamanioRAM) * sizeof(unsigned char));
}

void cargaParametro(TVM *VMX, char *argv, int *vp, int *cantp, int *sig){
    int tamanio= strlen(argv);
    int i=0;
    vp[*cantp]=(*sig); //Se guarda la posicion del puntero
    (*cantp)++;
    while(i<tamanio){
        (*VMX).RAM[(*sig)] = (unsigned char)argv[i];
        (*sig)++;
        i++;
    }
    (*VMX).RAM[(*sig)] = 0x00; //Agrega terminator
    (*sig)++;
}

void cargaPS(TVM *VMX,char *argv[],int argc,int *TPS,int *PPP,int *cantp){ //Carga el segmento de datos en la memoria
    int i=0,vp[100],k=0,sig=0;  //vp: vector de punteros 
    *PPP = NULO;   //PPP: puntero al primero parametro
    *cantp = 0;//cantp: cantidad de parametros
    while(i < argc && strcmp(argv[i], "-p") != 0)
        i++;
    if(i < argc && strcmp(argv[i], "-p") == 0){
        while(++i < argc){  
            cargaParametro(VMX,argv[i],vp,cantp,&sig);
        }
        (*PPP) |= sig & 0xFFFF; 
        while(k<(*cantp)){ //sig: proxima posicion en la memoria donde se escribe el puntero del parametro K
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

void IniciaPila(TVM *VMX,int PPP,int cantp){
    if((*VMX).REG[SS] != NULO){ //si existe el stack segment
        PUSH(PPP,2,VMX);   // puntero a primer puntero de parametros 
        PUSH(cantp,2,VMX); // cantidad de parametros
        PUSH(NULO,2,VMX);  // RET = NULO
    }
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
    (*VMX).REG[SP]=(*VMX).REG[SS] + TSS;
    (*VMX).REG[IP]=(*VMX).REG[CS] + EP; 
} 


int sufMemo(int VecTamSeg[],size_t tamanioRAM){ //Funcion que verifica si hay suficiente memoria
    int memAcum=0;
    for(int i=0;i<SEGMENTOS;i++)
        memAcum+=VecTamSeg[i];
    return (memAcum<tamanioRAM);
}

int CuentaSegmentos(int VecTamSeg[]){
    int cont=0;
    for(int i=0;i<SEGMENTOS;i++)
        cont+=(VecTamSeg[i]!=0);
    return cont;
}
void AjustaVecTam(int VecTamSegmentos[],int TPS){
    int TKS=VecTamSegmentos[4];
    for(int i=3;i>=0;i--)
        VecTamSegmentos[i+2]=VecTamSegmentos[i]; // [0 0 47 16337 0 0 ]
    VecTamSegmentos[0]=TPS;
    VecTamSegmentos[1]=TKS;
}

void armaTabla(TVM *VMX, int VecTamSeg[],int EP,size_t tamanioRAM){ //Arma la tabla de segmentos{
    int fila,cantSegmentosExistentes,segActual,indiceREG,memAcum; 

    //VecTamSegmentos = {TPS,TKS,TCS,TDS,TEX,TSS} siendo T el tamanio de cada segmento

    if(sufMemo(VecTamSeg,tamanioRAM)){ //Verifica si hay suficiente memoria
        segActual=0;
        memAcum=0;
        fila=0;
        
        for(int i=0;i<SECCIONES;i++) //Inicializa la tabla de segmentos
            for(int j=0;j<ENTRADAS;j++)
                (*VMX).SEG[i][j]=0;
       
        for (segActual=0 ; segActual < SEGMENTOS ; segActual++){
    
            if (segActual==1) //Se obtiene el codigo de registro correspondiente segun su posicion en el vector de tamanios
                indiceREG=4; 
            else
                indiceREG=segActual-2;
                                                  
            if (VecTamSeg[segActual]!=0){                     
                (*VMX).SEG[fila][0]=memAcum;
                (*VMX).SEG[fila][1]=VecTamSeg[segActual];
                memAcum += VecTamSeg[segActual];
                if(segActual!=0) //ya que el param segment no tiene registro para inicializar
                    (*VMX).REG[indiceREG] = (unsigned int)fila << 16; //se indica la entrada a la tabla de segmentos para cada registro de segmento
                fila++;
            }
            else {
                if (segActual!=0) //misma razon que antes
                    (*VMX).REG[indiceREG] = NULO;
            }
            
         iniciaRegistros(VMX,VecTamSeg[5],EP); //Inicia los registros IP y SP. Para ello se envia el tamanio del stack segment y el entry point
        
        }    
    }
        else
        (*VMX).error=4; //Error: No hay suficiente memoria
}


void Inicializacion(TVM *VMX,char *nombreArchivo,int *compatible, int TPS,size_t tamanioRAM){ //Carga el segmento de codigo en la memoria
    FILE *fichero;
    unsigned char lector;
    char VecCabecera[5];
    int VecTamSeg[6] = {0}; //5 ya que el param no esta contemplado en el vector de tamanios
   int i=0,j=0,OffsetEP=0, dirBaseCS=0, dirBaseKS=0, version=0;
    fichero=fopen(nombreArchivo,"rb");
    if (fichero==NULL){
        printf("Error al abrir el archivo\n");
    }
    else{
        while (j<5 && fread(&lector,sizeof(unsigned char),1,fichero)==1){
            VecCabecera[j]=lector;
            j++;
        }
        if(verificacabecera(VecCabecera)==1){
            fread(&lector,sizeof(unsigned char),1,fichero); //Version
            if(lector==1){
                version=1;
                fread(&lector,sizeof(unsigned char),1,fichero); //Tamanio del codigo
                VecTamSeg[CS] |=lector;
                VecTamSeg[CS]<<8;
                fread(&lector,sizeof(unsigned char),1,fichero);
                VecTamSeg[CS] |=lector;
                VecTamSeg[DS] = tamanioRAM - VecTamSeg[CS] - 1;
            }
            else{
                if(lector==2){
                    version=2;
                    for(int n=0;n<5;n++){
                        fread(&lector,sizeof(unsigned char),1,fichero);
                        VecTamSeg[n]|=lector;
                        VecTamSeg[n]=VecTamSeg[n]<<8;
                        fread(&lector,sizeof(unsigned char),1,fichero);
                        VecTamSeg[n]|=lector;
                    }
                    fread(&lector,sizeof(unsigned char),1,fichero);
                    OffsetEP |= lector;
                    OffsetEP = OffsetEP <<8;
                    fread(&lector,sizeof(unsigned char),1,fichero);
                    OffsetEP |= lector;
                }
                else{
                    *compatible=0;
                }
            }
            if((*compatible)){
                AjustaVecTam(VecTamSeg,TPS);//Tamanio del param segment
                armaTabla(VMX,VecTamSeg,OffsetEP,tamanioRAM); //Arma la tabla de segmentos
                //CARGA CS
                dirBaseCS=memologitofisica((*VMX).SEG,(*VMX).REG[CS]);
                while(i<VecTamSeg[CS+2]){
                    fread(&lector,sizeof(unsigned char),1,fichero);
                    (*VMX).RAM[dirBaseCS+i]=lector;
                    i++;
                }
                //CARGA KS 
                if ((*VMX).REG[KS] != NULO){
                    dirBaseKS = memologitofisica((*VMX).SEG,(*VMX).REG[KS]);
                    i = 0;
                    while(i<VecTamSeg[1]){
                        fread(&lector,sizeof(unsigned char),1,fichero);   //CREO Q ESTE PINGO DEBERIA ANDAR
                        (*VMX).RAM[dirBaseKS+i]=lector;
                        i++;
                    }
                }
            }
        fclose(fichero);
    }
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

void iniciaEjecucion(TVM *VMX, char *argv[], int argc, void(*op1op[])(), void(*op2op[])(), int breakdown,size_t tamanioRAM){ //Funcion que inicia la ejecucion del programa
    int dirfisica,dirfisicaTCS,topA,topB,A,B,assemb=0,i=0,inmediato;
    char orden;
    int indiceCS = (unsigned int)(*VMX).REG[CS]>>16;
    int otro_breakpoint=0;

    while(i<argc && strcmp(argv[i],"-d")!=0)
       i++;
    if(i<argc && strcmp(argv[i],"-d")==0)
        llamadissasembler(VMX); //Desensambla el programa
    dirfisica=memologitofisica((*VMX).SEG,(*VMX).REG[IP]);
    printf("Iniciando la ejecucion del programa...\n");
    dirfisica=memologitofisica((*VMX).SEG,(*VMX).REG[IP]);
    //       MIENTRAS:  NO ES STOP     Y  DIR FISICA ES VALIDA  Y NO HAY ERROR         Y DIR FISICA DENTRO DEL CODE SEGMENT  
    while((*VMX).RAM[dirfisica]!=0x0F && dirfisica!=NULO && (*VMX).error==0 && dirfisica<((*VMX).SEG[indiceCS][0] + (*VMX).SEG[indiceCS][1])){ //Mientras no sea un stop, no sea un ret, no haya error y esté dentro del CS
        orden=((*VMX).RAM[dirfisica] & MASC_COD_OPERACION); //Se obtiene la orden a ejecutar   
        if(!(orden>=0x00 && orden<=0x08) && !(orden>=0x0B && orden<=0x0E) && !(orden>=0x10 && orden<=0x1E)){ ///PREGUNTAR SI LA ORDEN ES INVALIDA: cuando el codigo de operacion de la instruccion a ejecutar no existe 
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
                        inmediato = armaInmediato((*VMX).RAM[dirfisica+1],(*VMX).RAM[dirfisica+2]);
                        if((orden & 0x0F)==0x00 && inmediato==0x0F && breakdown)
                            otro_breakpoint = 1;
                        else{
                            if (((orden & 0x0F)!=0x00) || ((orden & 0x0F)==0x00 && inmediato!=0x0F))
                                op1op[(orden & 0x0F)](inmediato,topB,VMX);
                        }
                        break;
                    case 0x03: //memoria.
                        op1op[(orden &0x0F)](armaMemoria((*VMX).RAM[dirfisica+1],(*VMX).RAM[dirfisica+2],(*VMX).RAM[dirfisica+3]),topB,VMX);
                        break;
                }
            }
            if(otro_breakpoint){
                sys_breakpoint(VMX,tamanioRAM,argv,&otro_breakpoint);
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
        case 5: printf("Error: Stack Overflow \n");
                break;
        case 6: printf("Error: Stack Underflow \n");
                break;
    }
    generaVMI(*VMX,tamanioRAM,argv[1]);
}




