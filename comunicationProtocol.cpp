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
    sendCharacter(iface, MACdst, type, trama, 256);
    return bce;
}

void transferPhaseSend(interface_t iface, char mode,  unsigned char *MACdst, unsigned char type[2]){
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
            printf("E  %c  STX  %c  %d  %d\n", mode, sequence, bce, bce);
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
    } else cout << "Fichero inexistente" << endl;
    bool confirmation = false;
    sequence = '1';
    while(!confirmation){
        if(sequence == '1'){
            sequence = '0';
        } else {
            sequence = '1';
        }
        unsigned char closeTrame[4];
        closeTrame[0] = mode;
        closeTrame[1] = EOT[0];
        closeTrame[2] = EOT[1];
        closeTrame[3] = sequence;
        printf("\n\nE  %c  EOT  %c\n", mode, sequence);
        sendCharacter(iface, MACdst, type, closeTrame, 256);
        apacket_t receivedConfirmationClosed = ReceiveFrame(&iface);
        while(receivedConfirmationClosed.packet == NULL){
            receivedConfirmationClosed = ReceiveFrame(&iface);
        }
        const unsigned char* payload = receivedConfirmationClosed.packet+14;
        sequence = payload[3];
        if(isConfirmation(receivedConfirmationClosed.packet, '0')){
            printf("R  %c  ACK  0\n\n", mode);
            printf("Comunicación finalizada\n", mode);
            confirmation = true;
        } else {
            printf("R  T  NACK  %c\n", sequence);
        }
    }

 

}
void closeSondeo(interface_t iface, unsigned char *MACdst, unsigned char type[2], char sequence){
    unsigned char controlTrame[4];


    int close = 0;
    while(close != 1){
      close = requestCloseOperation();
      if(close == 2){
        controlTrame[0] = 'T';
        controlTrame[1] = NACK[0];
        controlTrame[2] = NACK[1];
        controlTrame[3] = sequence;
        sendCharacter(iface, MACdst, type, controlTrame, 256);
        printf("E  T  NACK  %c\n", sequence);
         apacket_t received = ReceiveFrame(&iface);
        while(received.packet == NULL){
            received = ReceiveFrame(&iface);
        }
        const unsigned char* payload=NULL;
        payload = received.packet + 14;
        sequence = payload[3];
        printf("R  T  EOT  %c\n", sequence);
      } else {
        controlTrame[0] = 'T';
        controlTrame[1] = ACK[0];
        controlTrame[2] = ACK[1];
        controlTrame[3] = sequence;
        sendCharacter(iface, MACdst, type, controlTrame, 256);
        printf("E  T  ACK  %c\n", sequence);
      }
    }
    }
void transferToReceive(interface_t iface, unsigned char *MACdst, unsigned char type[2], int kind) {
    const unsigned char* payload=NULL;
    unsigned char controlTrame[4];
    char optionProtocol;
    char sequence = '0';
    bool eot = false;
    int lengthData = 0;
    int i;
    int bce;
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
            sendCharacter(iface, MACdst, type, controlTrame, 256);
            printf("E  %c  ACK  %c\n", optionProtocol, sequence);
        } else{
            printf("\n\nR  %c  EOT  0\n", optionProtocol);
            if(kind == 0){
                closeSondeo(iface, MACdst, type, sequence);
            } else {
                sendCharacter(iface, MACdst, type, controlTrame, 256);
                printf("E  %c  ACK  %c\n", optionProtocol, sequence);
            }
        }
    }
        fichero.close();
}

void establishmentPhaseMaster(int option, interface_t iface, unsigned char *MACdst, unsigned char type[2]){
    bool established = false;
    unsigned char controlTrame[4];
    char optionProtocol;

    if(option == 1){
        optionProtocol = 'R';
    } else {
        optionProtocol = 'T';
    }
    controlTrame[0] = optionProtocol;
    controlTrame[1] = ENQ[0];
    controlTrame[2] = ENQ[1];
    controlTrame[3] = '0';

    sendCharacter(iface, MACdst, type, controlTrame, 256);
    printf("\n\nE  %c  ENQ  0\n", optionProtocol);
    apacket_t received = ReceiveFrame(&iface);
    while(received.packet == NULL){
        received = ReceiveFrame(&iface);
    }

    if(isConfirmation(received.packet, '0')){
       printf("R  %c  ACK  0\n\n", optionProtocol);
        if(optionProtocol == 'R'){
            transferPhaseSend(iface, optionProtocol, MACdst, type);
        } else {
            transferToReceive(iface, MACdst, type, 0);
        }
    } else {
        establishmentPhaseMaster(option, iface, MACdst, type);
    }
}
void establishmentPhaseSlave(interface_t iface, unsigned char *MACdst, unsigned char type[2]){
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
    sendCharacter(iface, MACdst, type, controlTrame, 256);
    printf("E  %c  ACK  0\n\n", optionProtocol);
    if(optionProtocol == 'R'){
        transferToReceive(iface, MACdst, type, 1);
    } else {
        transferPhaseSend(iface, optionProtocol, MACdst, type);
    }
    printf("\n\n comunicación finalizada");
}

void closePhase(int option){

}
void stopAndWaitProtocol(bool isMaster, interface_t iface, unsigned char *MACdst, unsigned char type[2]){
    if(isMaster){
        int option = selectionProtocolType();
        establishmentPhaseMaster(option, iface, MACdst, type);
    } else {
        printf("Protocolo paro y espera. Para salir pulse ESC. \n Estás en Modo esclavo");
        establishmentPhaseSlave(iface, MACdst, type);

    }
}
