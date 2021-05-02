#include "comunicationProtocol.h"

using namespace std;

const unsigned char ENQ[2] = {'0', '5'};
const unsigned char ACK[2] = {'0', '6'}; 
const unsigned char EOT[2] = {'0', '4'}; 
const unsigned char NACK[2] = {'2', '1'}; 

unsigned char control[5];

bool isConfirmation(const unsigned char *trama, char sequence){
    const unsigned char* payload = trama + 14;
    return (payload[1] == ACK[0] && payload[2] == ACK[1] && payload[3] == sequence);
}
bool isEOT(const unsigned char *trama){
    const unsigned char* payload = trama + 14;
    return payload[1] == EOT[0] && payload[2] == EOT[1];
}

unsigned char* buildSTXTrame(int option, unsigned char *controlTrame, unsigned char data[256]){
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
unsigned char* buildACKHeader(unsigned char dir, unsigned char enumeration, unsigned char *controlTrame){
    controlTrame[0] = dir;
    controlTrame[1] = ACK[0];
    controlTrame[2] = ACK[1];
    controlTrame[3] = enumeration;
}  
bool establishmentPhaseMaster(int option, interface_t iface, unsigned char *MACdst, unsigned char type[2]){
    bool established = false;
    unsigned char controlTrame[4];
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

    sendCharacter(iface, MACdst, type, controlTrame);
    printf("\n\nE  %c  ENQ  0\n", optionProtocol);
    apacket_t received = ReceiveFrame(&iface);
    while(received.packet == NULL){
        received = ReceiveFrame(&iface);
    }

    if(isConfirmation(received.packet, '0')){
       printf("R  %c  ACK  0\n\n", optionProtocol);
       transferPhase(iface, optionProtocol, MACdst, type);
    } else {
        establishmentPhaseMaster(option, iface, MACdst, type);
    }
    return true;
}
bool establishmentPhaseSlave(interface_t iface, unsigned char *MACdst, unsigned char type[2]){
    const unsigned char* payload=NULL;
    unsigned char controlTrame[4];
    char optionProtocol;
    char sequence = '0';
    bool eot = false;
    int lengthData = 0;
    int i;
    int bce;

    apacket_t received = ReceiveFrame(&iface);
    while(received.packet == NULL){
        received = ReceiveFrame(&iface);
    }
    payload = received.packet+14;
    optionProtocol = payload[0];
    controlTrame[0] = optionProtocol;
    controlTrame[1] = ACK[0];
    controlTrame[2] = ACK[1];
    controlTrame[3] = payload[3];
    printf("\n\nR  %c  ENQ  0\n", optionProtocol);
    sendCharacter(iface, MACdst, type, controlTrame);
    printf("E  %c  ACK  0\n\n", optionProtocol);
    //Fase de transferencia;
    ofstream fichero("RProtoc.txt");
    while(!eot) {
        apacket_t received = ReceiveFrame(&iface);
        while(received.packet == NULL){
            received = ReceiveFrame(&iface);
        }
        eot = isEOT(received.packet);
        payload = received.packet+14;
        optionProtocol = payload[0];
        sequence = payload[3];
        lengthData = payload[4];
        i = 0;
        while(i < lengthData && payload[5 + i] != '0'){
            fichero << payload[5 + i];
            i++;
        }
        fichero << endl; 
        bce = payload[5 + i - 1];
        controlTrame[0] = optionProtocol;
        controlTrame[1] = ACK[0];
        controlTrame[2] = ACK[1];
        controlTrame[3] = sequence;
        if(!eot){
            printf("R  %c  STX  %c  %d\n", optionProtocol, sequence, (int)bce);
        } else{
            printf("\n\nR  %c  EOT  0\n", optionProtocol);
        }
        sendCharacter(iface, MACdst, type, controlTrame);
        printf("E  %c  ACK  %c\n", optionProtocol, sequence);
    }
        fichero.close();
        printf("\n\n comunicación finalizada");
    return true;
}
int sendTranspherTrame(char mode, char sequence, char data[254],  unsigned char *MACdst, unsigned char type[2], interface_t iface){
    unsigned char trama[270];
    int i = 0;
    int bce = 0;
    for(int i=0; i<270; i++){
        trama[i] = '/000';
    }
    trama[0] = mode;
    trama[1] = '0';
    trama[2] = '2';
    //sequence
    trama[3] = sequence;
    trama[4] = sizeof(data);
    while(data[i] != NULL) {
         trama[i + 5] = data[i];
         if(i == 0){
             bce = int(data[i]);
         }  else{
            bce = bce ^ int(data[i]);
         }
        i++;
    }
    trama[5 + i] = bce;
    sendCharacter(iface, MACdst, type, trama);
    return bce;
}
void transferPhase(interface_t iface, char mode,  unsigned char *MACdst, unsigned char type[2]){
    fstream ficheroEntrada;
    char sequence = '0';
    char data[254];
    bool success = true;
    apacket_t received;
    ficheroEntrada.open ("EProtoc.txt", ios::in);
        if (ficheroEntrada.is_open()) {
        while (! ficheroEntrada.eof() && success) {
            ficheroEntrada >> data;
            int bce =sendTranspherTrame(mode, sequence, data, MACdst, type, iface);
            printf("E  %c  STX  %c  %d\n", mode, sequence, bce);
             received.packet = NULL;
             received = ReceiveFrame(&iface);
             while(received.packet == NULL){
                received = ReceiveFrame(&iface);
            }

            if(isConfirmation(received.packet, sequence)){
                printf("R  %c  ACK  %c\n", mode, sequence);
                if(sequence == '0') {
                    sequence = '1';
                } else {
                    sequence = '0';
                }
                success = true;
            } else {
                success = false;
            }
        }
            ficheroEntrada.close();
            unsigned char closeTrame[4];
            closeTrame[0] = mode;
            closeTrame[1] = EOT[0];
            closeTrame[2] = EOT[1];
            closeTrame[3] = '0';
            sendCharacter(iface, MACdst, type, closeTrame);
            printf("\n\nE  %c  EOT  0\n", mode);
    } else cout << "Fichero inexistente" << endl;
    apacket_t receivedConfirmationClosed = ReceiveFrame(&iface);
    while(receivedConfirmationClosed.packet == NULL){
        receivedConfirmationClosed = ReceiveFrame(&iface);
    }

    if(isConfirmation(receivedConfirmationClosed.packet, '0')){
        printf("R  %c  ACK  0\n\n", mode);
        printf("Comunicación finalizada\n", mode);
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
