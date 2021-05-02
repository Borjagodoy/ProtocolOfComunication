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

bool establishmentPhaseMster(int option, interface_t iface, unsigned char *MACdst, unsigned char type[2]);

bool establishmentPhaseSlave(interface_t iface, unsigned char *MACdst, unsigned char type[2]);

void transferPhase();

void closePhase();

#endif
