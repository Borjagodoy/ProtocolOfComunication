#include "comunicationProtocol.h"
#include "linkLayer.h"
#include "interactiveMenu.h"
#include <iostream>
#include <stdio.h>
#include <fstream>

#ifndef _COMUNICATION_H
#define _COMUNICATION_H

void interactiveMode(interface_t iface, unsigned char *MACdst, unsigned char type[2]);

void startComunication(interface_t iface, unsigned char *MACdst, unsigned char type[2], int hostType);

void recivedChar(interface_t iface);

void sendCharacter(interface_t iface,  unsigned char MACdst[6], unsigned char type[2], unsigned char* payload);

void sendFile(interface_t iface, unsigned char MACdst[6], unsigned char type[2]);


#endif
