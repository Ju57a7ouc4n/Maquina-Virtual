#include "dissasembler.h"

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

void imprimeMemoria(char car1, char car2, char car3){
    int i=0;
    unsigned char aux;
    i=(char)car1;
    i=i<<8;
    i|=(char)car2;
    aux=((unsigned char)car3>>4); // 1111 0000 >> 0000 1111
    printf("[%s+%d]",devuelveRegistro(aux),i);
}

void imprimeRegistro(char car){
    unsigned char aux1,aux2;
    aux1=(unsigned char)(car & 0x0C)>>2; // 0000 1100 >> 0000 0011
    aux2=((unsigned char)car>>4);
    switch(aux1){
        case 0x00:
            printf("%s",devuelveRegistro(aux2));
            break;
        case 0x01:
            printf("%s",devuelveRegistro2bytes(aux2));
            break;
        case 0x02:
            printf("%s",devuelveRegistroAlto(aux2));
            break;
        case 0x03:
            printf("%s",devuelveRegistroBajo(aux2));
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
