//BAJO DIOS DEL CIELO Y DIJO: 
//"se aplicaran siempre primero las mascaras y luego los corrimientos precedidos por un casteo"

#define MASC_COD_OPERACION 0x1F //0001 1111
#define MASC_TIPO_OP_A 0x30     //0011 0000
#define MASC_TIPO_OP_B 0xC0     //1100 0000
#define MASC_CANT_OP 0x10       //0001 0000
#define MASC_UN_OP 0x20       //0010 0000

//mascaras para operandos de registro
#define MASC_MODIFICADOR_REG 0x0C     //0000 1100
#define MASC_MODIFICADOR_MEM 0x03 //0000 0011
#define MASC_CODIGO 0xF0          //1111 0000    

#define MASC_ERX 0xFFFFFFFF //registro completo 
#define MASC_RL 0x000000FF //registro x low
#define MASC_RH 0x0000FF00 //registro x high
#define MASC_RX 0x0000FFFF //registro x 

//mascaras para operandos de memoria
#define MASC_OFFSET 0xFFFFFFFF //extrae el desplazamiento del operando
#define MASC_SEGMENTO 0xFFFF //para obtener el codigo de segmento 
#define MASC_CC_NEGATIVO 0x80000000 //modifica registro CC si el ultimo resultado fue negativo
#define MASC_CC_CERO 0x40000000 //modifica registro CC si el ultimo resultado fue cero
#define MASC_BORRA_OFFSET 0xFFFF0000

//mascaras para parametros
#define MASC_BYTE1 0xFF000000
#define MASC_BYTE2 0x00FF0000
#define MASC_BYTE3 0x0000FF00
#define MASC_BYTE4 0x000000FF
