#include "comunicationProtocol.h"

const unsigned char ENQ[2] = {0, 5};
const unsigned char ACK[2] = {0, 6}; 
const unsigned char EOT[2] = {0, 4}; 
const unsigned char NACK[2] = {2, 1}; 

unsigned char control[5];

bool isConfirmation(const unsigned char *trama){
    const unsigned char* payload = trama + 14;
    return payload[1] == 0 && payload[2] == 6 && payload[0] == 0 && payload[3] == 6;
}
bool isEOT(const unsigned char *trama){
    const unsigned char* payload = trama + 14;
    return payload[1] == 0 && payload[2] == 4;
}
unsigned char* buildENQTrame(int option, unsigned char& controlTrame){
    char optionProtocol;
    if(option == 1){
        optionProtocol = 'T';
    } else {
        optionProtocol = 'R';
    }
    controlTrame[0] = optionProtocol;
    controlTrame[1] = ENQ[0];
    controlTrame[2] = ENQ[1];
    controlTrame[3] = '0';
}
unsigned char* buildSTXTrame(int option, unsigned char& controlTrame, unsigned char data[256]){
    char optionProtocol;
    int i = 0;
    int bce = 0;
    if(option == 1){
        optionProtocol = 'T';
    } else {
        optionProtocol = 'R';
    }
    controlTrame[0] = optionProtocol;
    controlTrame[1] = '0';
    controlTrame[2] = '2';
    controlTrame[3] = '0';
    controlTrame[4] = sizeof(data);
    while(data[i] != NULL) {
         controlTrame[i + 5] = data[i];
         if(i == 0){
             bce = int(data[i]);
         }  else{
            bce = bce ^ int(data[i]);
         }
        i++;
    }
    controlTrame[i + 5] = bce;
   
}
unsigned char* buildACKHeader(unsigned char dir, unsigned char enumeration, unsigned char &controlTrame[]){
    controlTrame[0] = dir;
    controlTrame[1] = ACK[0];
    controlTrame[2] = ACK[1];
    controlTrame[3] = enumeration;
}  
bool establishmentPhaseMaster(int option, interface_t iface, unsigned char *MACdst, unsigned char type[2]){
    printf("empezando fase de establecimiento");
    bool established = false;
    unsigned char controlTrame[4] ={ 0x00,0x00,0x80, 0xC0 } ;
    
    buildENQTrame(option, &controlTrame);

    sendCharacter(iface, MACdst, type, controlTrame);
    printf("Enviada trama ENQ");
    apacket_t received = ReceiveFrame(&iface);
    while(received.packet == NULL){
        received = ReceiveFrame(&iface);
    }

    if(isConfirmation(received.packet)){
        transferPhase();
    } else {
        establishmentPhaseMaster(option, iface, MACdst, type);
    }
    return true;
}
bool establishmentPhaseSlave(interface_t iface, unsigned char *MACdst, unsigned char type[2]){
    const unsigned char* payload=NULL;
    unsigned char controlTrame;

    apacket_t received = ReceiveFrame(&iface);
    while(received.packet == NULL){
        received = ReceiveFrame(&iface);
    }
    payload = received.packet+14;
    buildACKHeader(payload[0], payload[3], &controlTrame);    
    sendCharacter(iface, MACdst, type, &controlTrame);
    transferPhase();
    return true;
}
void transferPhase(int option, unsigned char& controlTrame){
    printf("empezando fase de transferencia");
    if(option == 0){
        ReadAndSendProtocolFile(option, controlTrame);
    } else {
        ReceivingInformation();
    }
}
void closePhase(int option){
    
}
void stopAndWaitProtocol(bool isMaster, interface_t iface, unsigned char *MACdst, unsigned char type[2]){
    bool estabilished = false;
    if(isMaster){
        int option = selectionProtocolType();
        estabilished = establishmentPhaseMaster(option, iface, MACdst, type);
    } else {
        printf("Protocolo paro y espera. Para salir pulse ESC. \n Estás en Modo esclavo");
        estabilished = establishmentPhaseSlave(iface, MACdst, type);

    }
}

void ReadAndSendProtocolFile(int option, unsigned char& controlTrame) {
    fstream ficheroEntrada;
    unsigned char trama[256];
    int i = 0;
    ficheroEntrada.open ("EProtoc.txt", ios::in);
    if (ficheroEntrada.is_open()) {
        while (! ficheroEntrada.eof() ) {
            ficheroEntrada >> trama;
            buildSTXTrame(option, controlTrame, trama);
            sendCharacter(iface, MACdst, type, controlTrame);
            //wait response from client.
            i++;
        }
        ficheroEntrada.close();
    } else cout << "Fichero inexistente" << endl;
}
void receivingInformation(interface_t iface, unsigned char *MACdst, unsigned char type[2]){
    const unsigned char* payload=NULL;
    unsigned char controlTrame;

    apacket_t received = ReceiveFrame(&iface);
    while(received.packet == NULL){
        received = ReceiveFrame(&iface);
    }
    payload = received.packet+14;
    buildACKHeader(payload[0], payload[3], &controlTrame);    
    sendCharacter(iface, MACdst, type, &controlTrame);
    if(!isEOT()){
        receivingInformation(iface, MACdst, type)
    }
}