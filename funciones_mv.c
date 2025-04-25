#include <stdio.h>
#include <string.h>
#include "MV.h"
#include "mascaras.h"
#include <ctype.h>

//  codReg: IP, EAX EBX
// inicioSeg:  

int recupera_direccion_registro(int contenido_registro,TVM *vm){
    int inicioSeg = (unsigned int)(contenido_registro & 0xFFFF0000) >> 16;
    int offsetReg = (unsigned int)contenido_registro & MASC_SEGMENTO;
    return ((*vm).SEG[inicioSeg][0] + offsetReg);
}

int recupera_direccion_operando(int operando,TVM *vm){ // 00000000 00000000 00000000 00000000
    int offset = (unsigned int)(operando & MASC_OFFSET) >> 8 ;
    int cod = (unsigned int)(operando & 0x000000F0 )>> 4;
    return  (recupera_direccion_registro((*vm).REG[cod],vm) + offset);
} 

int mascara(int modificador){
    int masc;
    switch(modificador){
        case 0x00:
             masc = MASC_ERX;
        break;

        case 0x01:
            masc = MASC_RL;
        break;

        case 0x02:
            masc= MASC_RH;
        break;

        case 0x03:
            masc = MASC_RX;
        break;
    }
    return masc;
}



int recupera_valor_operando(TVM *vm, int top, int operando){
    int valor; //comunes
    int mod,masc,cod,mascara_de_registro; //para registros
    int direccion,celdas; //para memoria
    switch (top){

        case 0x01: //operando de registro
            mod = (unsigned int)(operando & MASC_MODIFICADOR) >> 2; //obtiene el modificador del registro
            cod = (unsigned int)(operando & MASC_CODIGO) >> 4 ; //obtiene el codigo de registro
            mascara_de_registro = mascara(mod);
            valor = (*vm).REG[cod];
            valor = valor & mascara_de_registro;
            if (mod == 2)
                valor = (unsigned int)valor >> 8;
        break;
        
        case 0x02: //operando inmediato
            valor=operando;
        break;
        
        case 0x03: //operando de memoria
            valor = 0;
            mod = (unsigned int)(operando & MASC_MODIFICADOR) >> 2 ;
            if (mod == 0) 
                celdas = CANTCELDA;
            else 
                if (mod == 3)
                    celdas = 2;
                else
                    celdas = 1;
            direccion=recupera_direccion_operando(operando,vm);
            for (int i = 0; i < celdas; i++) {
                valor <<= 8;
                valor |= (vm->RAM[direccion + i] & 0xFF);
            }
        break;
    }

    return valor;
} 

int jump_valido (TVM vm, int salto){
    int inicioCS = vm.SEG[0][0];
    int finCS = vm.SEG[0][1] + inicioCS;
    return (salto < finCS) && (salto > inicioCS);
}

void jump (TVM *vm,int salto){
    if (!jump_valido(*vm,salto))
        vm->error = 3; // Fallo de segmento
}

void entrada(int *x,int formato)
{
    char c,aux[33];
    int i=0;

    switch(formato)
    {
        case 0x01: //decimal.
            scanf("%d",x);
            break;
        case 0x02: //caracter.
            scanf("%c",&c); 
            *x = (int)c;
            while (getchar() != '\n');
            break;
        case 0x04: //octal.
            scanf("%o",x);
            break;
        case 0x08: //hexadecimal.
            scanf("%x",x);
            break;
        case 0x10: //binario.
            scanf("%s",aux);
            *x = 0;
            while(aux[i]!='\0'){
                if(aux[i]=='1') 
                    *x |= 1;
                i++; 
                *x << 1;
            }  
            break; 
    }
}

void salida (int x,int formato,int tamanio){
    char c,aux[32];
    int i=31;

    if ((formato & 0x10) == 0x10){ //binario.
        printf("0b");
        for(int k=31 ; k>=0 ; k--){
            aux[k] = (x >> k) & 0x01;
        }
        while(aux[i] != 1)
            i--;
        for(i ; i>=0 ; i--){
            if(aux[i] == 1)
                printf("%c",'1');
            else
                printf("%c",'0');
        }
        printf(" "); 
    }
    if ((formato & 0x08) == 0x08){ //hexadecimal.
        printf("0x%x",x);
    }
    if ((formato & 0x04) == 0x04){ //octal.
        printf("0o%o",x);
    }
    if ((formato & 0x02) == 0x02){ //caracter.
        for (int i=tamanio ; i>=0 ; i--){
            int e= x >> 8*i;
            if(isprint(e))
                c = x >> 8*i;
            else
                c = '.';
            printf("%c",c);
        }
        printf(" ");
    }
    if ((formato & 0x01) == 0x01){ //decimal.
        printf("%d",x);
    }
    printf("\n");
}