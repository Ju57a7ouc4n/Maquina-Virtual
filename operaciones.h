#include "TVM.h"


// topA: tipo operando A.
// topB: tipo operando B.
// A es la direccion fisica del dato
// B depende de topB 


void MOV (int A, int opA, int B, int opB, TVM *vm);

void ADD (int A, int opA, int B, int opB, TVM *vm);

void SUB (int A, int opA, int B, int opB, TVM *vm);

void SWAP (int A, int opA, int B, int opB, TVM *vm);

void MUL (int A, int opA, int B, int opB, TVM *vm);

void DIV (int A, int opA, int B, int opB, TVM *vm);

void CMP (int A, int opA, int B, int opB, TVM *vm);

void SHL (int A, int opA, int B, int opB, TVM *vm);

void SHR (int A, int opA, int B, int opB, TVM *vm);

void AND (int A, int opA, int B, int opB, TVM *vm);

void OR (int A, int opA, int B, int opB, TVM *vm);

void XOR (int A, int opA, int B, int opB, TVM *vm);

void LDL (int A, int opA, int B, int opB, TVM *vm);

void LDH (int A, int opA, int B, int opB, TVM *vm);

void RND (int A, int opA, int B, int opB, TVM *vm);

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