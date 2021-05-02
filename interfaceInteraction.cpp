#include "interfaceInteraction.h"

char* getNameListOfInterface(int selectionNumber, pcap_if_t *listofInterfaces){
    bool found = false;
    int i = 0;
    while(listofInterfaces->next != NULL && !found){
        if(i == selectionNumber){
            found = true;
            return listofInterfaces->name;
        }else{
            listofInterfaces = listofInterfaces->next;
            i++;
        }
    }
return NULL;
}

void selectInterface(char *name, interface_t *ifaceSelected){
    setDeviceName(ifaceSelected, name);
    printf("Interfaz elegida:  %s \n", ifaceSelected->deviceName);
}
