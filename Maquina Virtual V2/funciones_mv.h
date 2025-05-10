#include <stdio.h>
#include "mascaras.h"
#include "valores_registros.h"
#include "MV.h"

int recupera_direccion_registro(int contenido_registro, TVM *vm);

int recupera_direccion_operando(int operando, TVM *vm);

int mascara(int modificador);

int recupera_valor_operando(TVM *vm, int top, int operando);

int jump_valido(TVM vm, int salto);

void jump(TVM *vm, int salto);

void entrada(int *x, int formato);

void salida(int x, int formato, int tamanio);