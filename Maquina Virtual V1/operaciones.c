#include "operaciones.h"
#include <stdio.h>
#include <stdlib.h>

//Funcion que modica los bits NZ del registro CC 
void NZ (int valor, TVM *vm){
    vm->REG[CC] = 0;
    
    if (valor < 0)
        vm->REG[CC] |= MASC_CC_NEGATIVO;
    
    if (valor == 0)
        vm->REG[CC] |= MASC_CC_CERO;

}

//FUNCIONES DE DOS PARAMETROS 

void MOV (int A, int topA, int B, int topB, TVM *vm) {
    int valB =0; 
    int posReg,masc,corr=0; // variables para registros
    int dir,celdas; // variables para memoria
    int modReg=0; // comun para memoria o registro
    valB=recupera_valor_operando(vm,topB,B);
    modReg = (A & 0x0000000C) >> 2; 
    switch(topA){

        // 01: registro
        case 0x01:
            masc = mascara(modReg);
            if (modReg == 0x02)
                corr = 1;
            posReg = (unsigned int)(A & MASC_CODIGO) >> 4; 
            valB = (unsigned int) valB << (corr*8);
            (*vm).REG[posReg] = ((*vm).REG[posReg] & (~masc)) | (valB & masc);
        break;  
        
        // 11: memoria
        case 0x03:
            dir = recupera_direccion_operando(A,vm);
            
            if (dir < 0 || dir >= MEMORIA) {  // Fallo de segmento. direccion fuera de memoria
                vm->error = 3;
                return;
            }
            
            //if (modReg == 0) // dependiendo el modificador del registro es la cantidad de celdas a modificar
            //    celdas = CANTCELDA;
            //else 
            //    if (modReg == 3)
            //        celdas = 2;
            //    else
            //        celdas = 1;

            celdas = CANTCELDA;
            if (dir + celdas > MEMORIA) { // Fallo de segmento. la direccion que quiero modificar fuera de memoria
                vm->error = 3;
                return;
            }

            for (int i = celdas; i>0; i--){
                vm->RAM[dir++] = valB >> ((i-1)*8); 
            } 
            break;                            
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
    int resta = valA - valB;
    MOV(A,topA,resta,2,vm);
    NZ(resta,vm);
}

void SWAP (int A, int topA, int B, int topB, TVM *vm){
    int valA = recupera_valor_operando(vm, topA, A);
    int valB = recupera_valor_operando(vm, topB, B);
    MOV(A, topA, valB, 2, vm); 
    MOV(B, topB, valA, 2, vm); 
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
        int opAC = (AC << 4) | (0 << 2); // Modificador 0
        MOV(opAC,1,resto,2,vm);
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
    int carga = (valA & 0xFFFF0000) | (valB & 0x0000FFFF);
    MOV(A,topA,carga,2,vm);
}

void LDH (int A, int topA, int B, int topB, TVM *vm){
    int valB = recupera_valor_operando(vm,topB,B);
    int valA = recupera_valor_operando(vm,topA,A);
    int carga = (valA & 0x0000FFFF) | ((valB & 0x0000FFFF) << 16);
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
        printf("[%04X]: ", dir);
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
        printf("[%04X]: ",dir);
        for(int k=tamanio-1 ; k>=0 ; k--){
            x |= (vm->RAM[dir++] << (8*k)) & (0xFF << (8*k));
        }
        salida(x,formato,tamanio);
    }
}

void SYS (int operando,int topA, TVM *vm){
    int dirMem = (((*vm).SEG[1][0])) + ((*vm).REG[EDX]&0x0000FFFF);
    //int dirMem = recupera_direccion_registro((*vm).REG[EDX],vm);
    int celdas = (unsigned int)(*vm).REG[ECX] & MASC_RL;
    int tamanio = (unsigned int)((*vm).REG[ECX] & MASC_RH) >> 8;
    int formato = (unsigned int)(*vm).REG[EAX] & MASC_RL;
    switch (operando){
        case 0x01:
            SYS1(dirMem,celdas,tamanio,formato,vm);
            break;
        case 0x02:
            SYS2(dirMem,celdas,tamanio,formato,vm);
            break;
    }
}

void JMP (int A, int topA, TVM *vm){
    int valA = recupera_valor_operando(vm,topA,A);
    jump(vm,valA);
    if(!vm->error) {
        vm->REG[IP] &= MASC_BORRA_OFFSET;
        vm->REG[IP] |= valA;
    }
}

void JZ (int A, int topA, TVM *vm){
    if(vm->REG[CC] & MASC_CC_CERO){
        JMP(A,topA,vm);
    }
}

void JP (int A, int topA, TVM *vm){
    if((!(vm->REG[CC] & MASC_CC_CERO)) && (!(vm->REG[CC] & MASC_CC_NEGATIVO)))
    {
        JMP(A,topA,vm);
    }
}

void JN (int A, int topA, TVM *vm){
    if(vm->REG[CC] & MASC_CC_NEGATIVO)
    {
        JMP(A,topA,vm);
    } 
}

void JNZ (int A, int topA, TVM *vm){
    if(!(vm->REG[CC] & MASC_CC_CERO))
    {
        JMP(A,topA,vm);
    } 
}

void JNP (int A, int topA, TVM *vm){
    if(((vm->REG[CC] & MASC_CC_CERO) || (vm->REG[CC] & MASC_CC_NEGATIVO)))
    {
        JMP(A,topA,vm);
    } 
}

void JNN (int A, int topA, TVM *vm){
    if((!(vm->REG[CC] & MASC_CC_NEGATIVO)))
    {
        JMP(A,topA,vm);
    } 
}

void NOT (int A, int topA, TVM *vm){
    int valA = recupera_valor_operando(vm,topA,A);
    int not = ~valA;
    MOV(A,topA,not,2,vm);
    NZ(not,vm);
}