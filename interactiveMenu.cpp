#include "interactiveMenu.h"


int requestInterfaceToSelect(int numberOfDevices){
    int selection;
    bool isNumber = true;
    printf ("Selecione interfaz:");
    isNumber = scanf("%d", &selection);
    while(selection >= numberOfDevices ){
        if( isNumber == 0 ) { 
            for( int c = getchar(); c != EOF && c != ' ' && c != '\n' ; c = getchar());
        }
        printf ("ha selecionado una interfaz no disponible,");
        printf ("Selecione una interfaz validad:");
        isNumber = scanf("%d", &selection);
        printf("%d", isNumber);
    }
    return selection;
}
int requestGroup(){
    bool isNumber = true;
    int groupNumber = 0;
    printf("Introduzca el numero de grupo:");
    isNumber = scanf("%d", &groupNumber);
    while(groupNumber == 0 || groupNumber >= MAX_GROUPS ){
        if( isNumber == 0 ) { 
            for( int c = getchar(); c != EOF && c != ' ' && c != '\n' ; c = getchar());
        }
        printf ("ha selecionado una grupo no disponible, tiene que ser un valor entre 1 y 15");
        printf ("Selecione una grupo valido:");
        isNumber = scanf("%d", &groupNumber);
        printf("%d", isNumber);
    }
    return groupNumber;
}
int requestHostMode(){
    bool isNumber = true;
    int hostType = 0;
    printf("Selecione el modo de la estación:\n");
    printf("[1] Modo Maestra\n");
    printf("[2] Modo Esclavo\n");
    isNumber = scanf("%d", &hostType);
    while(hostType == 0 || hostType >= 3){
        if( isNumber == 0 ) { 
            for( int c = getchar(); c != EOF && c != ' ' && c != '\n' ; c = getchar());
        }
        printf ("ha selecionado un modo no disponible, 1 para modo Maestra ò 2 para modo esclavo\n");
        isNumber = scanf("%d", &hostType);
        printf("%d", isNumber);
    }
    return hostType;
}
int requestSendingMode(){
    int key;
        key=getch();
        if(key == 27){
            key=getch();
            if(key ==79 && key != EOF) {
                key=getch();
                if(key == 80)
                    return 1;
                if(key == 81)
                    return 2;
                if(key == 82)
                    return 3;
            } else {
                return 0;
            }

    }
    return 10;
}
int selectionProtocolType(){
    bool isNumber = true;
    int option = 0;
    printf("Protocolo paro y espera. Para salir pulse ESC: \n");
    printf("Selecione el tipo de operación:\n");
    printf("[1] Operación de seleción\n");
    printf("[2] Operación de sondeo\n");
    printf("[3] salir\n");
    isNumber = scanf("%d", &option);
    printf("%d", option);
    while(option == 0 || option > 3){
        if( isNumber == 0 ) { 
            for( int c = getchar(); c != EOF && c != ' ' && c != '\n' ; c = getchar());
        }
        printf ("ha selecionado un modo no disponible, 1 para modo Seleción, 2 para modo Sondeo o 3 para salir\n");
        isNumber = scanf("%d", &option);
        printf("%d", isNumber);
    }
    fflush(stdin);
    return option;
}