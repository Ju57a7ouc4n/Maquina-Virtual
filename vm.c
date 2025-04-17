#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TVM.h"
#include "valores_registros.h"
#include "mascaras.h"
#define REGISTROS 16
#define SEGMENTOS 2
#define ENTRADAS 8
#define MEMORIA 16384
#define ICS 0

int memologitofisica(unsigned short tabla[ENTRADAS][SEGMENTOS],int);

int verificacabecera(char[]);

void cargaCS(char[MEMORIA], unsigned short[ENTRADAS][SEGMENTOS], const char*);   

void iniciaEjecucion(); 

void muestraCS(TVM vm );




















int main(int argc, char *argv[]){
    TVM vmx;
    vmx.R = REGISTROS;
    vmx.error = 0;
    cargaCS(vmx.RAM,vmx.SEG,argv[1]); //Carga el segmento de codigo en la memoria
    muestraCS(vmx);
    scanf("");
 //   iniciaEjecucion(vmx.RAM,vmx.SEG, argv[2], argc,vmx.REG); //Inicia la ejecucion del programa
    return 0;
}

int memologitofisica(unsigned short tabla[ENTRADAS][SEGMENTOS], int dirlogica){ //Funcion que convierte una direccion logica a fisica
    return (tabla[0][0]+dirlogica);
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

void cargaCS(char RAM[], unsigned short tabla[ENTRADAS][SEGMENTOS], const char *nombreArchivo){ //Carga el segmento de codigo en la memoria
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

void muestraCS(TVM vm){
    for (int i=0; i<vm.SEG[0][1];i++){
        printf("%d %c",i,vm.RAM[i]);
    }


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
            print:("SHR ");
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

char* devuelveRegistro(char car){
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

char* devuelveRegistro2bytes(char car){
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

char* devuelveRegistroBajo(char car){
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

char* devuelveRegistroAlto(char car){
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

void iniciaEjecucion(char RAM[MEMORIA], unsigned short tabla[ENTRADAS][SEGMENTOS], char *parametro, int argc, int registros[REGISTROS]){ //Funcion que inicia la ejecucion del programa
    int dirfisica;
    char orden;
    registros[IP]=0;
    if(argc==2 && strcmp(parametro,"-d")==0){
        printf("Iniciando la ejecucion del programa...\n");
        while(RAM[memologitofisica(tabla,registros[IP])] != (char)0x0F){
            dirfisica=memologitofisica(tabla,registros[IP]);
            printf("[%5.d] ",dirfisica);
            orden=(RAM[dirfisica]|MASC_COD_OPERACION); //Se obtiene la orden a ejecutar
            if(RAM[dirfisica]&MASC_CANT_OP==MASC_CANT_OP){ //Orden con dos operandos
                imprimeOrdenDosOp(RAM[dirfisica&MASC_COD_OPERACION]); //Imprime la orden
            }
            else{
                if(RAM[dirfisica]&MASC_UN_OP==MASC_UN_OP){ //Orden con un operando
                    imprimeOrdenUnOp(RAM[dirfisica&MASC_COD_OPERACION]); //Imprime la orden
                    if(RAM[dirfisica]&MASC_TIPO_OP_B==MASC_TIPO_OP_B) { //Es un operando de memoria
                        printf("[");
                        imprimeInmediato(RAM[dirfisica+1],RAM[dirfisica+2]);
                        printf("+%s] \n",devuelveRegistro(RAM[dirfisica+3]));
                        /*
                        INVOCACION A LA FUNCION
                        */
                    } else {
                        if(RAM[dirfisica]&MASC_TIPO_OP_B==0x80){ //Es un operando inmediato
                            printf("%d \n",(int)RAM[dirfisica]);
                            imprimeInmediato(RAM[dirfisica+1],RAM[dirfisica+2]);
                            printf(" \n");
                            /*
                            INVOCACION A LA FUNCION
                            */
                        }
                        else{ //Es un operando de registro
                            if(RAM[dirfisica+1]&0x0C==0x00){
                                printf("%s \n",devuelveRegistro(RAM[dirfisica+1]));
                                /*
                                INVOCACION A LA FUNCION
                                */
                            }
                            else{
                                if(RAM[dirfisica+1]&0x0C==0x0C){
                                    printf("%s \n",devuelveRegistro2bytes(RAM[dirfisica+1]));
                                    /*
                                    INVOCACION A LA FUNCION
                                    */
                                }
                                else{
                                    if(RAM[dirfisica+1]&0x0C==0x04){
                                        printf("%s \n",devuelveRegistroBajo(RAM[dirfisica+1]));
                                        /*
                                        INVOCACION A LA FUNCION
                                        */
                                    }
                                    else{
                                        if(RAM[dirfisica+1]&0x0C==0x08){
                                            printf("%s \n",devuelveRegistroAlto(RAM[dirfisica+1]));
                                            /*
                                            INVOCACION A LA FUNCION
                                            */
                                        }
                                    }
                                }
                            }
                       }
                    }
                }
                else{ //Es un stop
                    printf("STOP /n");
                }
            }
        }
    }
}




