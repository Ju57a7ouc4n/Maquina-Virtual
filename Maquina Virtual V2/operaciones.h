#include "funciones_mv.h"
#include "mascaras.h"
#include "valores_registros.h"
#include <stdio.h>

void MOV (int , int, int , int, TVM*);

void ADD (int , int, int , int, TVM*);

void SUB (int , int, int , int, TVM*);

void SWAP (int , int, int , int, TVM*);

void MUL (int , int, int , int, TVM*);

void DIV (int , int, int , int, TVM*);

void CMP (int , int, int , int, TVM*);

void SHL (int , int, int , int, TVM*);

void SHR (int , int, int , int, TVM*);

void AND (int , int, int , int, TVM*);

void OR (int , int, int , int, TVM*);

void XOR (int , int, int , int, TVM*);

void LDL (int , int, int , int, TVM*);

void LDH (int , int, int , int, TVM*);

void RND (int , int, int , int, TVM*);

void SYS (int , int, TVM*);

void JMP (int , int, TVM*);

void JZ (int , int, TVM*);

void JP (int , int, TVM*);

void JN (int , int, TVM*);

void JNZ (int , int, TVM*);

void JNP (int , int, TVM*);

void JNN (int , int, TVM*);

void NOT (int , int, TVM*);

void PUSH (int, int, TVM*);

void POP (int, int, TVM*);

void CALL (int, int, TVM*);

void RET (int, int, TVM*);