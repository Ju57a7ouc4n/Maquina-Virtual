\\DATA 500
\\STACK 1000
TECTO1 EQU "PEPON"
TEXTO2 EQU "viva la pepa"
NULL EQU -1
MAIN: PUSH 2
PUSH 3
CALL POTENCIA
ADD SP,8
STOP
POTENCIA: PUSH BP;    para guardar el BP anterior
MOV BP,SP;    para poder usar el nuevo BP como "pivote"
PUSH EBX;    para resguardar el registro EBX
MOV EAX,1
MOV EBX,1
CICLO: CMP EBX,[BP+8];    if EBX > EXPONENTE-->FIN
JP FIN
MUL EAX,[BP+12]; EAX = EAX * BASE
ADD EBX,1
JMP CICLO
FIN: POP EBX;    recupera EBX
MOV SP,BP
POP BP;        recupera BP
RET;        continua la ejecucion desde donde se dejo