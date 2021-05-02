#include "comunication.h"

using namespace std;

void sendCharacter(interface_t iface,  unsigned char MACdst[6], unsigned char type[2], unsigned char* payload){
    unsigned char *frame = BuildFrame(iface.MACaddr, MACdst, type, payload);
    SendFrame(&iface, frame, sizeof(frame));
}

void recivedChar(interface_t iface){
    apacket_t received = ReceiveFrame(&iface);
    int i=0;
    if(received.packet != NULL){
        const unsigned char* character= NULL;
        character=received.packet+14;
        while(character[i] != NULL) {
            printf("%c", character[i]);
            i++;
        }
    }
}
unsigned char* initMacdst(unsigned char *MACdst){
    unsigned char MACdstAux[6]= {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    if(MACdst == NULL) {
        return MACdstAux;
    } else {
        return MACdst;
    }
}

void sendDiscoverTrace(interface_t iface, unsigned char *MACsrc, unsigned char *MACdst, unsigned char type[2]){
   unsigned char *frame = BuildHeader(MACsrc, MACdst, type);
   SendFrame(&iface, frame, sizeof(frame));
   printf("traza de descubrimiento lanzada %s", frame);
}
bool isCorrectType(const unsigned char* type, unsigned char myType[2]){
    unsigned char typeAux[2] = {type[0], type[1]};
    return ((typeAux[0] == type[0]) && (typeAux[1] == 0x01));
}
void interactiveMode(interface_t iface, unsigned char *MACdst, unsigned char type[2]){
    char character;
    unsigned char payload[256];
    printf("seleciona un character:");
    getch();
    while((int)character != 27) {
        if(kbhit()){
            character = getch();
            payload[0] = character;
            sendCharacter(iface, MACdst, type, payload);
        }
        recivedChar(iface);
    }
}
void slavecomunication(interface_t iface, unsigned char type[2]){
    int mode;
    bool exit = false;
    printf("Esperando que se una la estación maestra \n");
    apacket_t received = ReceiveFrame(&iface);
    while(received.packet==NULL){
        received = ReceiveFrame(&iface);
    }
    if(received.packet != NULL &&  isCorrectType(received.packet + 12, type)){
        printf("Estación maestra encontrada. La mac es:  \n");
        unsigned char mac_dst[6]= {received.packet[6], received.packet[7], received.packet[8], received.packet[9], received.packet[10], received.packet[11]};
        sendDiscoverTrace(iface, iface.MACaddr, mac_dst, type);
        for(int i=0; i < 6; i++){ 
            cout << hex << (int) mac_dst[i];
            if( i != 5){
                printf(":");
            }
        }
        received.packet = NULL;
        printf("\n");
        int key;
        printf("Selecione de modo:\n");
        printf("[F1] Envio de carácteres interactivo\n");
        printf("[F3] Protocolo paro y espera\n");
        printf("[ESC] Salir \n");
        while(!exit){
            recivedChar(iface);
            mode = requestSendingMode();
            if(mode == 1){
                interactiveMode(iface, mac_dst, type);
            } else if(mode == 2){
                sendFile(iface, mac_dst, type);
            } else if(mode == 3) {
                printf("Protocolo paro y espera");
                stopAndWaitProtocol(false, iface, mac_dst, type);
            } else if(mode == 0){
                printf("Adios!\n");
                exit = true;
            }   
        }

    }else {
        printf("ha habido un error en la comunicación");
    }


}
void sendFile(interface_t iface, unsigned char MACdst[6], unsigned char type[2]){
    fstream ficheroEntrada;
    unsigned char trama[256];

    ficheroEntrada.open ("Fenvio.txt", ios::in);
    if (ficheroEntrada.is_open()) {
        while (! ficheroEntrada.eof() ) {
            ficheroEntrada >> trama;
            sendCharacter(iface,  MACdst, type, trama);
            cout << trama << " ";
        }
            ficheroEntrada.close();
    } else cout << "Fichero inexistente" << endl;
}
void masterComunication(interface_t iface, unsigned char *MACdst, unsigned char type[2]){
    unsigned char *MACdstAux;
    MACdstAux = initMacdst(MACdst);
    type[1]= 0x01;
    int mode;
    bool exit = false;
    printf("Esperando que se na la estación esclava");
    sendDiscoverTrace(iface, iface.MACaddr, MACdst, type);
    apacket_t received = ReceiveFrame(&iface);
    while(received.packet==NULL){
        received = ReceiveFrame(&iface);
    }
    printf("Estación encontrada. La MAC es: ");
    unsigned char mac_dst[6]= {received.packet[6], received.packet[7], received.packet[8], received.packet[9], received.packet[10], received.packet[11]};
    for(int i=0; i < 6; i++){ 
        cout << hex << (int) mac_dst[i];
        if( i != 5){
            printf(":");
        }
    }
    printf("\n");
    printf("Selecione de modo:\n");
    printf("[F1] Envio de carácteres interactivo\n");
    printf("[F2] Envio de un fichero \n");
    printf("[F3] Protocolo Paro y espera \n");
    printf("[ESC] Salir \n");
    while(!exit){
        recivedChar(iface);
        mode = requestSendingMode();
        if(mode == 1){
            interactiveMode(iface, mac_dst, type);
        } else if(mode == 2){
            sendFile(iface, mac_dst, type);
        } else if(mode == 3) {
                printf("Protocolo paro y espera");
                stopAndWaitProtocol(true, iface, mac_dst, type);
        } else if(mode == 0){
            printf("Adios!\n");
            exit = true;
        }
    }

    
}
void startComunication(interface_t iface, unsigned char *MACdst, unsigned char type[2], int hostType){
    printf("Comienzo %d", hostType);
    if(hostType == 1){
        printf("Comienzo de la comunicación como maestro");
        masterComunication(iface, MACdst, type);
    } else {
        printf("Comienzo de la comunicación como esclavo");
        slavecomunication(iface, type);
    }
}