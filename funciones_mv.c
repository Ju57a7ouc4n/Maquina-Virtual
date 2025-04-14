#include <stdio.h>
#include "TVM.h"
#include "mascaras.h"

//  codReg: IP, EAX EBX
// inicioSeg:  

int recupera_direccion_operando(int operando,TVM *vm){
    int offset = (operando & MASC_OFFSET) >> 8;
    int cod = (operando & MASC_CODIGO) >> 4;
    int inicioSeg = (vm->REG[codReg] >> 16) & MASC_SEGMENTO;
    int offsetReg = vm->REG[codReg] & MASC_SEGMENTO;
    return vm->SEG[inicioSeg][0] + offsetReg + offset;
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
    int mod,masc; //para registros
    int direccion; //para memoria
    switch (top){

        case 1: //operando de registro
            mod = (operando & MASC_MODIFICADOR) >> 2; //obtiene el modificador del registro
            cod = (operando & MASC_CODIGO) >> 4; //obtiene el codigo de registro
            mascara_de_registro = mascara(modificador);
            valor = vm->REG[cod];
            valor = valor & mascara_de_registro;
            if (mod == 2)
                valor = valor >> 8;
        break;
        
        case 2: //operando inmediato
            valor=operando;
        break;
        
        case 3: //operando de memoria
            direccion=recupera_direccion_operando(operando,vm);
            for (int i = 0 ; i < TAMCELDA ; i++) {
                valor = vm->RAM[direccion++]; // CHEQUEAR: al asignar un char a un int se mantienen el resto de bytes del int??
                valor = valor << 8;          // Si no es asi, somos pollo
            }
        break;

        return valor;
    }
} 

int jump_valido (TMV vm, int salto){
    int inicioCS = (vm.SEG[0][0] >> 16) & MASC_SEGMENTO;
    int finCS = (vm.SEG[0][0] & MASC_SEGMENTO) + inicioCS;
    return (salto < finCS) && (salto >= inicioCS);
}

void jump (TVM *vm,int salto){
    if (!jump_valido(*vm,salto))
        vm->error = 3; // Fallo de segmento
}
    
    :1 esav
    :1 esav
    :1 esav
    :1 esav
    :1 esav
    :1 esav
    :1 esav
    :1 esav
    :1 esav
    :1 esav
    :1 esav
    :1 esav
    :1 esav
    :1 esav
    :1 esav
    :1 esav
    :1 esav
    :1 esav
    :1 esav
    :1 esav
    :1 esav
    :1 esav
    :1 esav
    :1 esav
    :1 esav
    :1 esav
    :1 esav
    :1 esav
    :1 esav
    :1 esav
    :1 esav
    :1 esav
    :1 esav
    :1 esav
    :1 esav
    :1 esav
    :1 esav
    :1 esav
    :1 esav
    :1 esav
    :1 esav
    1 eav
    }


    
}av
    }


    
}av
    }


    
}av
    }


    
}av
    }


    
}av
    }


    
}