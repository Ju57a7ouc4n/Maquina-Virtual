#include <stdio.h>
#include <string.h>
#include "MV.h"
#include "mascaras.h"
#include "valores_registros.h"
#include <ctype.h>
#define NULO -1 

int recupera_segmento(int contenido_registro){
    return (unsigned int)(contenido_registro & 0xFFFF0000) >> 16;
}

int PermanezcoEnSegmento(TVM *vm,int dir,int indice_seg,int desplazamiento){
    int dir_inicio_seg;
    int dir_fin_seg;
    
    dir_inicio_seg=(*vm).SEG[indice_seg][0];
    dir_fin_seg=dir_inicio_seg+(*vm).SEG[indice_seg][1];
    
    return ((dir>=dir_inicio_seg) && ((dir + desplazamiento) <= dir_fin_seg));
}

void recupera_direccion_registro(int contenido_registro,TVM *vm,int *indice_seg,int *dir){
    *indice_seg = recupera_segmento(contenido_registro);
    int offsetReg = (unsigned int)contenido_registro & MASC_SEGMENTO;
    *dir = ((*vm).SEG[*indice_seg][0] + offsetReg);
}

int recupera_direccion_operando(int operando,TVM *vm){
    int indice_seg;
    int dir;
    int offset =(operando << 8) >> 16;
    int cod = (unsigned int)(operando & 0x000000F0 )>> 4;
    recupera_direccion_registro((*vm).REG[cod],vm,&indice_seg,&dir);   
    dir += offset;
    if (PermanezcoEnSegmento(vm,dir,indice_seg,0)) //verifica que el calculo de la direccion no se encuentre fuera de los limites del segmento
        return  dir;
    else
        return NULO;
} 

int mascara(int modificador){
    int masc;
    switch(modificador){
        case 0x00:
             masc = MASC_ERX;
        break;

        case 0x01:
            masc = MASC_RL;
        break;

        case 0x02:
            masc= MASC_RH;
        break;

        case 0x03:
            masc = MASC_RX;
        break;
    }
    return masc;
}



int recupera_valor_operando(TVM *vm, int top, int operando){
    int valor=0; //comunes
    int mod,masc,cod,mascara_de_registro; //para registros
    int dir,celdas; //para memoria
    int contenido_registro;
    int indice_seg;
    int cod_reg;
    switch (top){

        case 0x01: //operando de registro
            mod = (unsigned int)(operando & MASC_MODIFICADOR_REG) >> 2; //obtiene el modificador del registro
            cod = (unsigned int)(operando & MASC_CODIGO) >> 4 ; //obtiene el codigo de registro
            mascara_de_registro = mascara(mod);
            valor = (*vm).REG[cod];
            valor = valor & mascara_de_registro;
            if (mod == 3)
                valor = (valor << 16) >> 16;
            if (mod == 2)
                valor = valor >> 8;
            if (mod == 1 || mod ==2)
                valor = (valor << 24) >> 24;
        break;
        
        case 0x02: //operando inmediato
            valor = operando;
        break;
        
        case 0x03: //operando de memoria
            dir=recupera_direccion_operando(operando,vm);
            if(dir!=NULO){
                mod = operando & MASC_MODIFICADOR_MEM;
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

                cod_reg = (unsigned int) (operando & MASC_CODIGO) >> 4;
                contenido_registro=(*vm).REG[cod_reg];
                indice_seg= recupera_segmento(contenido_registro);  //!!!!necesito saber en que segmento estoy

                if(PermanezcoEnSegmento(vm,dir,indice_seg,celdas)){//!!!! debo verificar que no me excedo del segmento)
                    for (int i = 0; i < celdas; i++) {
                        valor <<= 8;
                        valor |= (vm->RAM[dir + i] & 0xFF);
                    }
                }
                else
                    vm->error = 3; //!!!!fallo de segmento si quiero leer/escribir fuera de los limites del segmento
            }
            else
                vm->error=3;
            break;
    }
    return valor;      
} 

int jump_valido (TVM vm, int salto){
    int IndiceCS = (vm.REG[CS] >> 16) & 0xFFFF;
    int finCS = vm.SEG[IndiceCS][1];
    return (salto < finCS);
}

void jump (TVM *vm,int salto){
    if (!jump_valido(*vm,salto))
        vm->error = 3; // Fallo de segmento 
}

void entrada(int *x,int formato)
{
    char c,aux[33];
    int i=0;

    switch(formato)
    {
        case 0x01: //decimal.
            scanf("%d",x);
            break;
        case 0x02: //caracter.
            scanf("%c",&c); 
            *x = (int)c;
            break;
        case 0x04: //octal.
            scanf("%o",x);
            break;
        case 0x08: //hexadecimal.
            scanf("%x",x);
            break;
        case 0x10: //binario.
            scanf("%s",aux);
            *x = 0;
            while(aux[i]!='\0'){
                *x <<= 1;
                if(aux[i] == '1') 
                    *x |= 1;
                i++;
            }  
            break; 
    }
    while (getchar() != '\n');
}

void salida (int x,int formato,int tamanio){
    char c,aux[32];
    int i=31;

    if ((formato & 0x10) == 0x10){ //binario.
        printf("0b");
        for(int k=31 ; k>=0 ; k--){
            aux[k] = (x >> k) & 0x01;
        }
        while(aux[i] != 1)
            i--;
        for(i ; i>=0 ; i--){
            if(aux[i] == 1)
                printf("%c",'1');
            else
                printf("%c",'0');
        }
        printf(" "); 
    }
    if ((formato & 0x08) == 0x08){ //hexadecimal.
        printf("0x%x ",x);
    }
    if ((formato & 0x04) == 0x04){ //octal.
        printf("0o%o ",x);
    }
    if ((formato & 0x02) == 0x02){ //caracter.
        for (int i = tamanio - 1; i >= 0; i--) {
            char c = (x >> (i * 8)) & 0xFF;
            if (isprint(c))
                putchar(c);
            else
                putchar('.');
        }
        printf(" ");
    }
    if ((formato & 0x01) == 0x01){ //decimal.
        printf("%d ",x);
    }
    printf("\n");
}

void generaVMI(TVM vm,size_t tamanioRAM,char *nombreArchivo){
    FILE *fichero;
    int i=0,aux=0;
    unsigned char grabador,alto=0,bajo=0;
    char auxc[256]; // Buffer local seguro
    strncpy(auxc, nombreArchivo, sizeof(auxc) - 1);
    auxc[sizeof(auxc) - 1] = '\0'; // Asegura terminación nula

    char *punto = strrchr(auxc, '.'); // Busca la última aparición de '.'
    if (punto != NULL) {
        strcpy(punto, ".vmi"); // Reemplaza la extensión
    } else {
        strcat(auxc, ".vmi"); // Si no hay extensión, la agrega
    }
    fichero=fopen(auxc,"wb");
    if(fichero!=NULL){
        grabador='V';
        fwrite(&grabador,sizeof(unsigned char),1,fichero);
        grabador='M';
        fwrite(&grabador,sizeof(unsigned char),1,fichero);
        grabador='I';
        fwrite(&grabador,sizeof(unsigned char),1,fichero);
        grabador='2';
        fwrite(&grabador,sizeof(unsigned char),1,fichero);
        grabador='5';
        fwrite(&grabador,sizeof(unsigned char),1,fichero);
        grabador=1;
        fwrite(&grabador,sizeof(unsigned char),1,fichero);
        aux=tamanioRAM/1024; 
        alto=(aux&0xFF00)>>8;
        bajo=aux&0x00FF;
        grabador=alto; // CHE EL CARGA CS TIENE UNA VARIABLE DE CONTROL QUE SE LLAMA H CREO QUE LEE, GRABA Y SUMA
        fwrite(&grabador,sizeof(unsigned char),1,fichero);
        grabador=bajo;
        fwrite(&grabador,sizeof(unsigned char),1,fichero);
        for(int j=0;j<16;j++){ //Este ciclo arma los registros
                grabador=((vm).REG[j]&0xFF000000)>>24;
                fwrite(&grabador,sizeof(unsigned char),1,fichero);
                grabador=((vm).REG[j]&0x00FF0000)>>16;
                fwrite(&grabador,sizeof(unsigned char),1,fichero);
                grabador=((vm).REG[j]&0x0000FF00)>>8;
                fwrite(&grabador,sizeof(unsigned char),1,fichero);
                grabador=((vm).REG[j]&0x000000FF);
                fwrite(&grabador,sizeof(unsigned char),1,fichero);
        }
        for(int k=0;k<8;k++){ //Este ciclo arma la bendita tabla de segmentos
                grabador=((vm).SEG[k][0]&0xFF00)>>8;
                fwrite(&grabador,sizeof(unsigned char),1,fichero);
                grabador=((vm).SEG[k][0]&0x00FF);
                fwrite(&grabador,sizeof(unsigned char),1,fichero);
                grabador=((vm).SEG[k][1]&0xFF00)>>8;
                fwrite(&grabador,sizeof(unsigned char),1,fichero);
                grabador=((vm).SEG[k][1]&0x00FF);
                fwrite(&grabador,sizeof(unsigned char),1,fichero);
        }
        for(i=0;i<tamanioRAM;i++){
            grabador=(unsigned char)(vm).RAM[i];
            fwrite(&grabador,sizeof(unsigned char),1,fichero);
        }
    }
}