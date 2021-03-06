#ifndef COMUNICATIONPROTOCOL_H
#define COMUNICATIONPROTOCOL_H

#include "linkLayer.h"
#include "interactiveMenu.h"
#include "comunication.h"
#include <iostream>
#include <stdio.h>
#include <fstream>

unsigned char* buildACKHeader(unsigned char direction, unsigned char enumerations);

void stopAndWaitProtocol(bool isMaster,interface_t iface, unsigned char *MACdst, unsigned char type[2]);

void establishmentPhaseMster(int option, interface_t iface, unsigned char *MACdst, unsigned char type[2]);

void establishmentPhaseSlave(interface_t iface, unsigned char *MACdst, unsigned char type[2]);

void transferPhase(interface_t iface, char mode,  unsigned char *MACdst, unsigned char type[2]);

void closePhase();

#endif
