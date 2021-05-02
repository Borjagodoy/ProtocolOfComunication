//============================================================================
// ----------- PRACTICAS DE FUNDAMENTOS DE REDES DE COMUNICACIONES -----------
// ---------------------------- CURSO 2020/21 --------------------------------
// -----------------------------PRACTICAFRC.CPP ---------------------------------
// ------------Autor--- BORJA GODOY GAGO--------------------------------------
//============================================================================

//*Tenemos que mandar a broadcast una trama con el tipo, accedemos al tipo con +6  Dst-> 0-5 src-> 6-13 trama 14*  La MAC tiene que ser unsigned char* o unsigned char[6]
//Si usamos unsigned char* Reservamos recuros con -calloc o malloc y luego usamos memcopy(O, dst, tamaño) esto se ejecutara cuando el tipo coincida con el grupo + 0x01
//
#include <stdio.h>
#include <iostream>
#include "linkLayer.h"
#include "comunication.h"
#include "interfaceInteraction.h"
#include "interactiveMenu.h"

using namespace std;

void printMAC( unsigned char MACaddr[6], bool isSrc){
    if(isSrc){
        printf("La MAC orgin es: ");
    } else {
        printf("La MAC destino es: ");

    }
    for(int i=0; i < 6; i++){ 
        cout << hex << (int) MACaddr[i];
        if( i != 5){
            printf(":");
        }
    }
    printf("\n");
}

int printListOfDevices(pcap_if_t *a){
    int i;
    printf ("Tenemos estos elementos: \n");
    for(i=0; a->next != NULL; i++){
        printf ("[%d] %s \n", i, a->name);
        a = a->next;
    }
    return i;
} 


int main()
{
 interface_t iface;
 pcap_if_t *avail_ifaces=NULL;
 int selection=0;
 char *selectedInterfaceName;
 int numberOfDevices;
 bool isSelectedItem;
 char car = 'A';
 int group;
 int hostType;
 unsigned char mac_src[6]={0x00, 0x00, 0x00, 0x00,0x00, 0x00};
 unsigned char mac_dst[6]={0x00, 0x01, 0x02, 0x03,0x04, 0x05};
 unsigned char type[2];
 

 printf("\n----------------------------\n");
 printf("--------- Practica - FRC ------\n");
 printf("-------------------------------\n");
 //Show all the adapters 
 avail_ifaces=GetAvailAdapters();
 numberOfDevices = printListOfDevices(avail_ifaces);
 //request to the user his selected adapter
 selection = requestInterfaceToSelect(numberOfDevices);

 printf("\n---Getting info of selected Adapter---\n");
 //getting information about the interface selected by the user
 selectedInterfaceName = getNameListOfInterface(selection, avail_ifaces);
 selectInterface(selectedInterfaceName, &iface);
    
 //Getting MAC and show by terminal the MAC number of device selected.
 GetMACAdapter(&iface);
 printMAC(iface.MACaddr, true);

 group = requestGroup();
 
 int openAdapter = OpenAdapter(&iface);
 printf("Puerto abierto correctament \n");
 
 hostType = requestHostMode();
 type[0] = 48 + group;
 
 startComunication(iface, mac_dst, type, hostType);

 CloseAdapter(&iface);

 return 0;
}
