/*
*/
#include "linkLayer.h"


#ifndef _INTERFACEINTERACTION_H
#define _INTERFACEINTERACTION_H
 

char* getNameListOfInterface(int selectionNumber, pcap_if_t *listofInterfaces);

void selectInterface(char *name, interface_t *ifaceSelected);

#endif
