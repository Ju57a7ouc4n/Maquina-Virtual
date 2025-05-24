#include "operaciones.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NULO -1
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
    int dir,seg,celdas; // variables para memoria
    int mod=0; // comun para memoria o registro
    int contenido_registro;
    int indice_seg;
    int cod_reg;
    valB=recupera_valor_operando(vm,topB,B);
     
    switch(topA){
        // 01: registro
        case 0x01:
            mod = (unsigned int) (A & MASC_MODIFICADOR_REG) >> 2;
            masc = mascara(mod);
            if (mod == 0x02)
                corr = 1;
            posReg = (unsigned int)(A & MASC_CODIGO) >> 4; 
            valB = (unsigned int) valB << (corr*8);
            (*vm).REG[posReg] = ((*vm).REG[posReg] & (~masc)) | (valB & masc);
        break;  
        
        // 11: memoria
        case 0x03:
            dir = recupera_direccion_operando(A,vm);
            if (dir!=NULO){ //!!!!fallo de segmento si dir==-1 
                mod =  A & MASC_MODIFICADOR_MEM;
                switch(mod){
                    case 0x00:
                        celdas=4; //long
                    break;

                    case 0x01:
                        celdas=4; //long
                    break;
                    
                    case 0x02:
                        celdas=2; //word
                    break;

                    case 0x03:
                        celdas=1; //byte
                    break;
                }
                cod_reg = (unsigned int) (A & MASC_CODIGO) >> 4;
                contenido_registro=(*vm).REG[cod_reg];
                indice_seg= recupera_segmento(contenido_registro);  //!!!!necesito saber en que segmento estoy
                
                if (PermanezcoEnSegmento(vm,dir,indice_seg,celdas)){   //!!!!debo verificar que no me excedo del segmento
                    for (int i = celdas; i>0; i--){
                    vm->RAM[dir++] = valB >> ((i-1)*8);    
                    }
                }
                else 
                    vm->error = 3; //!!!!fallo de segmento si quiero leer/escribir fuera de los limites del segmento
            }
            else 
                vm->error = 3;
                //!!!! else aborta la ejecucion del programa
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

void SYS3(int dirMem, int cantCarac, TVM *vm) {
    char cadena[200] = {0};
    int tamCadena;
    int IndiceEDX = ((*vm).REG[EDX]>>16) & 0xFFFF;

    if (fgets(cadena, sizeof(cadena), stdin) == NULL) {
        return;
    }

    int len = strlen(cadena);
    if (len > 0 && cadena[len - 1] == '\n') {
        cadena[len - 1] = '\0';
        len--;
    }

    if (cantCarac == -1) {
        tamCadena = len;
    } else {
        tamCadena = (len < cantCarac) ? len : cantCarac;
    }

    if (dirMem + tamCadena < (*vm).SEG[IndiceEDX][0]+(*vm).SEG[IndiceEDX][1]) {
        strncpy((*vm).RAM + dirMem, cadena, tamCadena);
        (*vm).RAM[dirMem + tamCadena] = '\0';
    } else{
        // ERROR: cadena fuera de segmento
    }
}

void SYS4 (int dirMem,TVM *vm){
    int x,i=0;
    while((*vm).RAM[dirMem+i]!=0){
        printf("%c",(*vm).RAM[dirMem+i]);
        i++;
    }
}

void SYS7 (){
    system("cls");
}

void SYS (int operando,int topA, TVM *vm){
    int IndiceReg = ((*vm).REG[EDX] >> 16) & 0xFFFF;
    int dirMem = (((*vm).SEG[IndiceReg][0])) + ((*vm).REG[EDX]&0x0000FFFF);
    int celdas = (unsigned int)(*vm).REG[ECX] & MASC_RL;
    int tamanio = (unsigned int)((*vm).REG[ECX] & MASC_RH) >> 8;
    int formato = (unsigned int)(*vm).REG[EAX] & MASC_RL;
    int cantCarac = (unsigned int)(*vm).REG[ECX] & MASC_RX;
    switch (operando){
        case 0x01:
            SYS1(dirMem,celdas,tamanio,formato,vm);
        break;
        case 0x02:
            SYS2(dirMem,celdas,tamanio,formato,vm);
        break;
        case 0x03:
            SYS3(dirMem,cantCarac,vm);
        break;
        case 0x04:
            SYS4(dirMem,vm);
        break;
        case 0x07:
            SYS7();
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

void PUSH (int A, int topA, TVM *vm){
    int valA = recupera_valor_operando(vm,topA,A);
    
    int memSP = 0x00000060; //el PUSH es un MOV de de un registro, inmediato o memoria a una posicion de memoria
                            // el 6 es el valor que representa el SP y el 0 dice que son 4 bytes, los primeros 000000 dicen que no hay offset
    (*vm).REG[SP] -= 4;
    if ((*vm).REG[SP] > (*vm).REG[SS])
        MOV(memSP,3,valA,2,vm); // 
        else
        (*vm).error = 5; //stack overflow
}  

void POP (int A, int topA, TVM *vm){
    int B = 0x00000060; //el POP es un MOV de una posicion de memoria apuntado por SP a un registro o memoria
                        // el 6 es el valor que representa el SP y el 0 dice que son 4 bytes, los primeros 000000 dicen que no hay offset

    if (((*vm).REG[SP] & 0xFFFF) + 4 < ((*vm).SEG[((*vm).REG[SS]>>16) & 0xF][1])){
        MOV(A,topA,B,3,vm);
        (*vm).REG[SP] += 4;
    } else
        (*vm).error = 6; //stack underflow
}

void CALL (int A, int topA, TVM *vm){
    PUSH((*vm).REG[IP],2,vm); // CALL es similar a un PUSH IP y JMP
    JMP(A,topA,vm);
}

void RET (int A, int topA, TVM *vm){
    int B = 0x50;  // RET es similiar a un POP IP ==> MOV IP,[SP]
    POP(B,1,vm);
}