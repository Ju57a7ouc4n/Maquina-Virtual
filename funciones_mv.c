#include <stdio.h>
#include "TVM.h"
#include "mascaras.h"

//  codReg: IP, EAX EBX
// inicioSeg:  

int recupera_direccion_registro(int contenido_registro,TVM *vm){
    int inicioSeg = (contenido_registro >> 16) & MASC_SEGMENTO;
    int offsetReg = contenido_registro & MASC_SEGMENTO;
    return vm->SEG[inicioSeg][0] + offsetReg;
}

int recupera_direccion_operando(int operando,TVM *vm){
    int offset = (operando & MASC_OFFSET) >> 8;
    int cod = (operando & MASC_CODIGO) >> 4;
    return  recupera_direccion_registro(vm->REG[cod],vm) + offset;
} 

int mascara(int modificador){
    int masc;
    switch(modificador){
        case 0:
             masc = MASC_ERX;
        break;

        case 1:
            masc = MASC_RL;
        break;

        case 2:
            masc= MASC_RH;
        break;

        case 3:
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

        case 1: //operando de registro
            mod = (operando & MASC_MODIFICADOR) >> 2; //obtiene el modificador del registro
            cod = (operando & MASC_CODIGO) >> 4; //obtiene el codigo de registro
            mascara_de_registro = mascara(mod);
            valor = vm->REG[cod];
            valor = valor & mascara_de_registro;
            if (mod == 2)
                valor = valor >> 8;
        break;
        
        case 2: //operando inmediato
            valor=operando;
        break;
        
        case 3: //operando de memoria
            valor = 0;
            mod = (operando & MASC_MODIFICADOR) >> 2;
            if (mod == 0) 
                celdas = CANTCELDA;
            else 
                if (mod == 3)
                    celdas = 2;
                else
                    celdas = 1;
            direccion=recupera_direccion_operando(operando,vm);
            for (int i = celdas ; i > 0 ; i--) {
                valor |= vm->RAM[direccion++] << 8*i; 
            }
        break;
    }

    return valor;
} 

int jump_valido (TVM vm, int salto){
    int inicioCS = (vm.SEG[0][0] >> 16) & MASC_SEGMENTO;
    int finCS = (vm.SEG[0][0] & MASC_SEGMENTO) + inicioCS;
    return (salto < finCS) && (salto >= inicioCS);
}

void jump (TVM *vm,int salto){
    if (!jump_valido(*vm,salto))
        vm->error = 3; // Fallo de segmento
}

void entrada(int *x,int formato)
{
    char c;
    char aux[100];

    switch(formato)
    {
        case 1:
            scanf("%d",x);
            break;
        case 2:
            scanf("%c",&c);
            *x = (int)c;
            //while (getchar() != '\n');
            break;
        case 4:
            scanf("%o",x);
            break;
        case 8:
            scanf("%x",x);
            break;
        case 16:
            scanf("%s",aux);
            
    }
}