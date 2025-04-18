#include "box.h"
//#include "utilities.h"

#define MARGIN				sConfigBox.BOX.margin	

s_Box boxBuffer[MAX_BOX_BUFFER];

void initialize_boxes(){
	indexBox = 0;
	
	sConfigBox.HCSR.prom_quant = 3; //cargar de la EEPROM el ultimo valor
	sConfigBox.HCSR.prom_iter = 0;
	sConfigBox.HCSR.auxHeight = 0;
	sConfigBox.BOX.margin = 29;
	sConfigBox.BOX.hA = 348;
	sConfigBox.BOX.hB = 464;
	sConfigBox.BOX.hC = 580;
	indexBox = 0;
	boxFlag.byte = 0;
	
	for(iterator = 0; iterator < MAX_BOX_BUFFER; iterator++){
		boxBuffer[iterator].type = NO_TYPE; 
	}
}

void addBox(uint16_t altura){
	if(altura >= (sConfigBox.BOX.hA -MARGIN) && altura <= (sConfigBox.BOX.hA +MARGIN)){
		boxBuffer[indexBox].type = TYPE_A;
	}else if(altura >= (sConfigBox.BOX.hB -MARGIN) && altura <= (sConfigBox.BOX.hB +MARGIN)){
		boxBuffer[indexBox].type = TYPE_B;
	}else if(altura >= (sConfigBox.BOX.hC -MARGIN) && altura <= (sConfigBox.BOX.hC +MARGIN)){
		boxBuffer[indexBox].type = TYPE_C;
	}else{
		boxBuffer[indexBox].type = DISCARD;
	}
	indicepepe  = indexBox;
	indexBox++;
	indexBox &= 15;
}

eType getBoxType(eType ind){
	return boxBuffer[ind].type;
}

void setBoxType(uint8_t ind, eType type){
	boxBuffer[ind].type = type;
}