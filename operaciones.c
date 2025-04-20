#include "operaciones.h"
#include <stdio.h>
#include <stdlib.h>

//Funcion que modica los bits NZ del registro CC 
void NZ (int valor, TVM *vm){
    vm->REG[CC] &= 0x3FFFFFFF;
    
    if (valor < 0)
        vm->REG[CC] |= MASC_CC_NEGATIVO;
    
    if (valor == 0)
    vm->REG[CC] |= MASC_CC_CERO;
}

//FUNCIONES DE DOS PARAMETROS 

void MOV (int A, int topA, int B, int topB, TVM *vm) {
    int valB = recupera_valor_operando(vm,topB,B);
    int posReg,masc,corr=0; // variables para registros
    int dir,celdas; // variables para memoria
    int modReg; // comun para memoria o registro
    modReg = (A & MASC_MODIFICADOR)>>2;
    switch(topA){

        // 01: registro
        case 0x01:
            masc = mascara(modReg);
            if (modReg == 0x02)
                corr = 1;
            posReg = (unsigned int)(A & 0xF0) >> 4;
            valB = valB << (corr*8);
            vm->REG[posReg] = (vm->REG[posReg] & (~masc)) | (valB & masc);
        break;   
        
        // 11: memoria
        case 0x03:
            dir = recupera_direccion_operando(A,vm);
            if (modReg == 0) // dependiendo el modificador del registro es la cantidad de celdas a modificar
                celdas = CANTCELDA;
            else 
                if (modReg == 3)
                    celdas = 2;
                else
                    celdas = 1;
            for (int i = celdas; i>0; i--){
                vm->RAM[dir++] = valB >> ((i-1)*8);  //se descartan los bits mas significativos
            }                             
        }
}

void ADD (int A, int topA, int B, int topB, TVM *vm){
    int valB = recupera_valor_operando(vm,topB,B); 
    int valA = recupera_valor_operando(vm,topA,A);
    int suma = valB + valA;
    MOV(A,topA,suma,2,vm);
    NZ(suma,vm);
}

void SUB (int A, int topA, int B, int topB, TVM *vm){
    int valB = recupera_valor_operando(vm,topB,B); 
    int valA = recupera_valor_operando(vm,topA,A);
    int resta = valB - valA;
    MOV(A,topA,resta,2,vm);
    NZ(resta,vm);
}

void SWAP (int A, int topA, int B, int topB, TVM *vm){
    int aux = recupera_valor_operando(vm,topA,A);
    MOV(A,topA,B,topB,vm);
    MOV(B,topB,aux,2,vm); //CHEQUEAR
}

void MUL (int A, int topA, int B, int topB, TVM *vm){
    int valB = recupera_valor_operando(vm,topB,B); 
    int valA = recupera_valor_operando(vm,topA,A);
    int multiplicacion = valA * valB;
    MOV (A,topA,multiplicacion,2,vm);
    NZ(multiplicacion,vm);
}

void DIV (int A, int topA, int B, int topB, TVM *vm){
    int valB = recupera_valor_operando(vm,topB,B); 
    int valA = recupera_valor_operando(vm,topA,A);
    if (valB) {
        int division = valA / valB;
        int resto = valA % valB;
        MOV (A,topA,division,2,vm);
        MOV(AC,1,resto,2,vm);
        NZ(division,vm);
    } else
        vm->error = 2; //Division por cero
}

void CMP (int A, int topA, int B, int topB, TVM *vm){
    int valB = recupera_valor_operando(vm,topB,B); 
    int valA = recupera_valor_operando(vm,topA,A);
    int resta = valA - valB;
    NZ(resta,vm);
}

void SHL (int A, int topA, int B, int topB, TVM *vm){
    int valB = recupera_valor_operando(vm,topB,B);
    int valA = recupera_valor_operando(vm,topA,A);
    int desplazamiento = valA << valB;
    MOV(A,topA,desplazamiento,2,vm);
    NZ(desplazamiento,vm);
}

void SHR (int A, int topA, int B, int topB, TVM *vm){
    int valB = recupera_valor_operando(vm,topB,B);
    int valA = recupera_valor_operando(vm,topA,A);
    int desplazamiento = valA >> valB;
    MOV(A,topA,desplazamiento,2,vm);
    NZ(desplazamiento,vm);
}

void AND (int A, int topA, int B, int topB, TVM *vm){
    int valB = recupera_valor_operando(vm,topB,B);
    int valA = recupera_valor_operando(vm,topA,A);
    int and = valA & valB;
    MOV(A,topA,and,2,vm);
    NZ(and,vm);
}

void OR (int A, int topA, int B, int topB, TVM *vm){
    int valB = recupera_valor_operando(vm,topB,B);
    int valA = recupera_valor_operando(vm,topA,A);
    int or = valA | valB;
    MOV(A,topA,or,2,vm);
    NZ(or,vm);
}

void XOR (int A, int topA, int B, int topB, TVM *vm){
    int valB = recupera_valor_operando(vm,topB,B);
    int valA = recupera_valor_operando(vm,topA,A);
    int xor = valA ^ valB;
    MOV(A,topA,xor,2,vm);
    NZ(xor,vm);
}

void LDL (int A, int topA, int B, int topB, TVM *vm){
    int valB = recupera_valor_operando(vm,topB,B);
    int valA = recupera_valor_operando(vm,topA,A);
    int carga = (valB & 0xFFFF) | (valA & 0xFFFF);
    MOV(A,topA,carga,2,vm);
}

void LDH (int A, int topA, int B, int topB, TVM *vm){
    int valB = recupera_valor_operando(vm,topB,B);
    int valA = recupera_valor_operando(vm,topA,A);
    int carga = (valB & 0xFFFF) | (valA & 0xFFFF0000);
    MOV(A,topA,carga,2,vm);
}

void RND (int A, int topA, int B, int topB, TVM *vm){
    int valB = recupera_valor_operando(vm,topB,B);
    int random = rand() % (valB + 1); //rand() genera un numero aleatorio y al hacerle % (valB + 1) se establece el rango entre 0 y valB.
    MOV(A,topA,random,2,vm);
}

//FUNCIONES DE UN SOLO PARAMETRO 

void SYS1 (int dir,int celdas,int tamanio,int formato, TVM *vm){
    int x;

    for (int i=0 ; i<celdas ; i++){
        printf("[%04x]: ", dir);
        entrada(&x,formato);
        for (int k=tamanio-1 ; k>=0 ; k--){
            vm->RAM[dir++] = (x & (0xFF<<8*k)) >> (8*k);
        }
    }
}

void SYS2 (int dir,int celdas,int tamanio,int formato, TVM *vm){
    int x;

    for(int i=0 ; i<celdas ; i++){
        x = 0;
        printf("[%04x]: ",dir);
        for(int k=tamanio-1 ; k>=0 ; k--){
            x |= (vm->RAM[dir++] << (8*k)) & (0xFF << (8*k)); 
        }
        salida(x,formato,tamanio);
    }
}

void SYS (int operando,int topA, TVM *vm){ // TODOS los operandos de entrada son inmediatos??
    int dirMem = recupera_direccion_operando(vm->REG[EDX],vm);
    int celdas = vm->REG[ECX] & MASC_RL;    
    int tamanio = (vm->REG[ECX] >> 8) & MASC_RH;
    int formato = vm->REG[EAX] & MASC_RL;

    switch (operando){
        case 1:
            SYS1(dirMem,celdas,tamanio,formato,vm);
            break;
        case 2:
            SYS2(dirMem,celdas,tamanio,formato,vm);
            break;
    }
}

void JMP (int A, int topA, TVM *vm){
    int valA = recupera_valor_operando(vm,topA,A);
    jump(vm,valA);
    if(!vm->error) {
        vm->REG[IP] &= MASC_BORRA_OFFSET;
        vm->REG[IP] = valA;
    }
}

void JZ (int A, int topA, TVM *vm){
    int valA = recupera_valor_operando(vm,topA,A);
    jump(vm,valA);
    if((!vm->error) && (vm->REG[CC] & MASC_CC_CERO)){
        vm->REG[IP] &= MASC_BORRA_OFFSET;
        vm->REG[IP] = valA;
    }
}

void JP (int A, int topA, TVM *vm){
    int valA = recupera_valor_operando(vm,topA,A);
    jump(vm,valA);
    if((!vm->error) && (!(vm->REG[CC] & MASC_CC_CERO)) && (!(vm->REG[CC] & MASC_CC_NEGATIVO)))
    {
        vm->REG[IP] &= MASC_BORRA_OFFSET;
        vm->REG[IP] = valA;
    }
}

void JN (int A, int topA, TVM *vm){
    int valA = recupera_valor_operando(vm,topA,A);
    jump(vm,valA);
    if(!(vm->error) && (vm->REG[CC] & MASC_CC_NEGATIVO))
    {
        vm->REG[IP] &= MASC_BORRA_OFFSET;
        vm->REG[IP] = valA;
    } 
}

void JNZ (int A, int topA, TVM *vm){
    int valA = recupera_valor_operando(vm,topA,A);
    jump(vm,valA);
    if((!vm->error) && (!(vm->REG[CC] & MASC_CC_CERO)))
    {
        vm->REG[IP] &= MASC_BORRA_OFFSET;
        vm->REG[IP] = valA;
    } 
}

void JNP (int A, int topA, TVM *vm){
    int valA = recupera_valor_operando(vm,topA,A);
    jump(vm,valA);
    if((!vm->error) && ((vm->REG[CC] & MASC_CC_CERO) || (vm->REG[CC] & MASC_CC_NEGATIVO)))
    {
        vm->REG[IP] &= MASC_BORRA_OFFSET;
        vm->REG[IP] = valA;
    } 
}

void JNN (int A, int topA, TVM *vm){
    int valA = recupera_valor_operando(vm,topA,A);
    jump(vm,valA);
    if((!vm->error) && (!(vm->REG[CC] & MASC_CC_NEGATIVO)))
    {
        vm->REG[IP] &= MASC_BORRA_OFFSET;
        vm->REG[IP] = valA;
    } 
}

void NOT (int A, int topA, TVM *vm){
    int valA = recupera_valor_operando(vm,topA,A);
    int not = ~valA;
    MOV(A,topA,not,2,vm);
    NZ(not,vm);
}