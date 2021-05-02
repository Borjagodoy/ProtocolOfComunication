/*
vamos a crear un protocolo en el cual se envie mediante broadcast una información
sin saber destino y mediante el type que sera 0x30 - 0x3a encontraremos la mac destino

*/
#include "linkLayer.h"
#include <iostream>


#ifndef _INTERACTIVEMENU_H
#define _INTERACTIVEMENU_H

const int MAX_GROUPS = 15;

int requestInterfaceToSelect(int numberOfDevices);

int requestGroup();

int requestHostMode();

int requestSendingMode();

int selectionProtocolType();

#endif
