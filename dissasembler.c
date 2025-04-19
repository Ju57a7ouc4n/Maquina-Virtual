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
    aux=((unsigned char)car3>>4);
    printf("[%d+%s]",i,devuelveRegistro(aux));
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
