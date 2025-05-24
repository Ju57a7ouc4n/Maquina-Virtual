#include "MV.h"
#include "dissasembler.h"
#include "mascaras.h"
#include "valores_registros.h"

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
        case 0x0B: //PUSH
            printf("PUSH ");
            break;
        case 0x0C: //POP
            printf("POP ");
            break;
        case 0x0D: //CALL
            printf("CALL ");
            break;
    }
}

void imprimeOrdenCeroOp(char orden){
    switch(orden){
        case 0x0F:
            printf("STOP ");
            break;
        case 0x0E:
            printf("RET ");
            break;
    }
}

void imprimeInmediato(char car1, char car2){
    int i=0;
    i=(unsigned char)car1 << 8;
    i|=(unsigned char)car2;
    printf("%X",i);
}

void imprimeMemoria(char car1, char car2, char car3){
    int i=0;
    int mod;
    unsigned char aux;
    i=(char)car1;
    i=i<<8;
    i|=(char)car2;
    aux=((unsigned char)car3>>4);
    mod=car3 & MASC_MODIFICADOR_MEM;
    if(mod==0x00 || mod==0x01){   // car1 car2 car3 ==> offsetMem + Reg ==> 
        printf("l");
        printf("[%s+%x]",devuelveRegistro(aux),i);
    }
    else{
        if(mod==0x02){
        printf("w");
        printf("[%s+%x]",devuelveRegistro(aux),i);
        }
        else{ //0x03
            printf("b");
            printf("[%s+%x]",devuelveRegistro(aux),i);
        }
    }
}

void imprimeRegistro(char car){
    unsigned char aux1,aux2;
    aux1=(unsigned char)(car & 0x0C)>>2; 
    aux2=((unsigned char)car>>4);
    switch(aux1){
        case 0x00:
            printf("%s",devuelveRegistro(aux2));
            break;
        case 0x01:
            printf("%s",devuelveRegistroBajo(aux2));
            break;
        case 0x02:
            printf("%s",devuelveRegistroAlto(aux2));
            break;
        case 0x03:
            printf("%s",devuelveRegistro2bytes(aux2));
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
        case 0x09:
            return "AC";
            break;
        case 0x0A:
            return "EAX";
            break;
        case 0x0B:
            return "EBX";
            break;
        case 0x0C:
            return "ECX";
            break;
        case 0x0D:
            return "EDX";
            break;
        case 0x0E:
            return "EEX";
            break;
        case 0x0F:
            return "EFX";
    }
}

char* devuelveRegistro2bytes(unsigned char car){
    switch(car){
        case 0x0A:
            return "AX";
            break;
        case 0x0B:
            return "BX";
            break;
        case 0x0C:
            return "CX";
            break;
        case 0x0D:
            return "DX";
            break;
        case 0x0E:
            return "EX";
            break;
        case 0x0F:
            return "FX";
    }
}

char* devuelveRegistroBajo(unsigned char car){
    switch(car){
        case 0x0A:
            return "AL";
            break;
        case 0x0B:
            return "BL";
            break;
        case 0x0C:
            return "CL";
            break;
        case 0x0D:
            return "DL";
            break;
        case 0x0E:
            return "EL";
            break;
        case 0x0F:
            return "FL";
    }
}

char* devuelveRegistroAlto(unsigned char car){
    switch(car){
        case 0x0A:
            return "AH";
            break;
        case 0x0B:
            return "BH";
            break;
        case 0x0C:
            return "CH";
            break;
        case 0x0D:
            return "DH";
            break;
        case 0x0E:
            return "EH";
            break;
        case 0x0F:
            return "FH";
    }
}

void imprime_tab (int x){
    for(int i=x ; i<7 ; i++){
        printf("\t");
    }
    printf("| ");
}
void llamadissasembler(TVM *VMX){
    int dirfisica=0,dirfisicaEP=0,topA=0,topB=0,orden=0,assemb=0,flag=0; 
    int A,B;
    int indiceCS = (unsigned int)(*VMX).REG[CS]>>16;
    dirfisica=memologitofisica((*VMX).SEG,(*VMX).REG[CS]);
    dirfisicaEP=memologitofisica((*VMX).SEG,(*VMX).REG[IP]);
 
    while((*VMX).error==0 && orden!=0x0F && dirfisica<((*VMX).SEG[indiceCS][0] + (*VMX).SEG[indiceCS][1])){ //Mientras no hay error y dentro de CS
            orden=(char)((*VMX).RAM[dirfisica] & MASC_COD_OPERACION); //Se obtiene la orden a ejecutar          
            topB=(((*VMX).RAM[dirfisica] & MASC_TIPO_OP_B) >> 6);
            topA=((*VMX).RAM[dirfisica] & MASC_TIPO_OP_A) >> 4;

            if(dirfisicaEP != dirfisica)
                printf(" ");
            else
                printf(">");
            
            printf("[%04X]\t",dirfisica);

            if((orden&0x10) == 0x10){ //Dos operandos
                (*VMX).REG[IP] = ((*VMX).REG[CS]) | (((*VMX).REG[IP] & 0x00007FFF) + topA + topB + 1); //Se actualiza el registro IP
                printf("%02X\t",(*VMX).RAM[dirfisica]);
                switch (topB){
                    case 0x01: //registro. 
                        assemb=1;
                        break;
                     case 0x02: //imnediato.
                        assemb=2;
                        break;
                     case 0x03: //memoria.
                         assemb=3;
                         break;
                 }
                switch (assemb){
                    case 1:
                       printf("%02X\t",(*VMX).RAM[dirfisica+1]);
                       break;
                    case 2:
                       printf("%02X\t%02X\t",(*VMX).RAM[dirfisica+1],(*VMX).RAM[dirfisica+2]);
                       break;
                    case 3:
                       printf("%02X\t%02X\t%02X\t",(*VMX).RAM[dirfisica+1],(*VMX).RAM[dirfisica+2],(*VMX).RAM[dirfisica+3]);
                       break;
               }
                if(topA==0x01){ //registro.
                    printf("%02X\t",(*VMX).RAM[dirfisica+topB+1]);                
                 }
                  else{  //memoria.
                     printf("%02X\t%02X\t%02X\t",(*VMX).RAM[dirfisica+topB+1],(*VMX).RAM[dirfisica+topB+2],(*VMX).RAM[dirfisica+topB+3]);
                 }
                imprime_tab(1+topA+topB);
                imprimeOrdenDosOp(orden);
                if(topA==0x01){ //registro.
                    imprimeRegistro((*VMX).RAM[dirfisica+topB+1]);
                    printf(",");                
                 }
                else{  //memoria.
                    imprimeMemoria((*VMX).RAM[dirfisica+topB+1],(*VMX).RAM[dirfisica+topB+2],(*VMX).RAM[dirfisica+topB+3]);
                    printf(",");
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
                }
            else{ 
                if(orden <= 0x08){ // un operando
                    (*VMX).REG[IP] = ((*VMX).REG[CS]) | (((*VMX).REG[IP] & 0x00007FFF)+ topB + 1); //Se actualiza el registro IP
                    printf("%02X\t",(*VMX).RAM[dirfisica]);
                    switch (topB){
                        case 0x01: //registro.
                            printf("%02X\t",(*VMX).RAM[dirfisica+1]);
                            break;
                        case 0x02: //imnediato.
                            printf("%02X\t%02X\t",(*VMX).RAM[dirfisica+1],(*VMX).RAM[dirfisica+2]);
                            break;
                        case 0x03: //memoria.
                            printf("%02X\t%02X\t%02X\t",(*VMX).RAM[dirfisica+1],(*VMX).RAM[dirfisica+2],(*VMX).RAM[dirfisica+3]);
                            break;
                    }
                    imprime_tab(1+topB);
                    imprimeOrdenUnOp(orden);
                    switch (topB){
                        case 0x01: //registro.
                            imprimeRegistro((*VMX).RAM[dirfisica+1]);
                            break;
                        case 0x02: //imnediato.
                            imprimeInmediato((*VMX).RAM[dirfisica+1],(*VMX).RAM[dirfisica+2]);
                            break;
                        case 0x03: //memoria.
                            imprimeMemoria((*VMX).RAM[dirfisica+1],(*VMX).RAM[dirfisica+2],(*VMX).RAM[dirfisica+3]);
                            break;
                    }
                    printf("\n");
                } else { // 0 operandos, en la primera parte solo es STOP
                    (*VMX).REG[IP] = ((*VMX).REG[CS]) | (((*VMX).REG[IP] & 0x00007FFF)+ 1);
                    printf("%02X ",(*VMX).RAM[dirfisica]);
                    imprime_tab(0);
                    imprimeOrdenCeroOp((*VMX).RAM[dirfisica]);
                    flag=1;
                    printf("\n");
                }
            }

            dirfisica=memologitofisica((*VMX).SEG,(*VMX).REG[IP]);
        }

        (*VMX).REG[IP] = ((*VMX).REG[IP] & 0x00010000); 
    }