#include "operaciones.h"
#include "mascaras.h"
#include <stdlib.h>
#define TAMCELDA 4

//FUNCIONES DE DOS PARAMETROS 

void MOV (int A, int topA, int B, int topB, TVM *vm) {
    int valB = recupera_valor_operando(&vm,topB,B); 
    int posReg, dir;
    
    switch(topA){

        // 01: registro
        case 1:
             posReg = A >> 4;
             vm->REG[posReg] = valB;
        break;   
        
        // 11: memoria
        case 3:
            dir = recupera_direccion_operando(A,&vm);
            for (int i = TAMCELDA; i>0; i--){               
                vm->RAM[dir++] = valB >> ((i-1)*8);  //se descartan los bits mas significativos
            }                             
        }
}

void ADD (int A, int topA, int B, int topB, TVM *vm){
    int valB = recupera_valor_operando(vm,topB,B); 
    int valA = recupera_valor_operando(vm,topA,A);
    int suma = valB + valA;
    MOV(A,topA,suma,topB,vm);
}

void SUB (int A, int topA, int B, int topB, TVM *vm){
    int valB = recupera_valor_operando(vm,topB,B); 
    int valA = recupera_valor_operando(vm,topA,A);
    int resta = valB - valA;
    MOV(A,topA,resta,topB,vm);
}

void SWAP (int A, int topA, int B, int topB, TVM *vm){
    int aux = recupera_valor_operando(vm,topA,A);
    MOV(A,topB,B,topB,vm);
    MOV(B,topB,aux,topA,vm); // CHEQUEAR
}

void MUL (int A, int topA, int B, int topB, TVM *vm);

void DIV (int A, int topA, int B, int topB, TVM *vm);

void CMP (int A, int topA, int B, int topB, TVM *vm);

void SHL (int A, int topA, int B, int topB, TVM *vm);

void SHR (int A, int opA, int B, int opB, TVM *vm);

void AND (int A, int opA, int B, int opB, TVM *vm);

void OR (int A, int opA, int B, int opB, TVM *vm);

void XOR (int A, int opA, int B, int opB, TVM *vm);

void LDL (int A, int opA, int B, int opB, TVM *vm);

void LDH (int A, int opA, int B, int opB, TVM *vm);

void RND (int A, int opA, int B, int opB, TVM *vm);

//FUNCIONES DE UN SOLO PARAMETRO 

void SYS (int A, int opA, TVM *vm);

void JMP (int A, int opA, TVM *vm);

void JZ (int A, int opA, TVM *vm);

void JP (int A, int opA, TVM *vm);

void JN (int A, int opA, TVM *vm);

void JNZ (int A, int opA, TVM *vm);

void JNP (int A, int opA, TVM *vm);

void JNN (int A, int opA, TVM *vm);

void NOT (int A, int opA, TVM *vm);

void STOP ();



