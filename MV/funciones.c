

#include "funciones.h"

uint32_t get(TVM *MV,uint32_t OP){
    uint32_t byteCabecera = (OP & 0xFF000000);
}
void set(TVM *MV,uint32_t * OP1,uint32_t OP2);


void MOV(TVM * MV){
    set(MV,MV->registros[4],get(MV,MV->registros[5]));
}