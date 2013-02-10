#include "standard.h"

T_word32 hit_count = 0;

void main()
{
    T_COMPort port = 0;
    T_COMPort port2 = 0;
    T_word16 value ;

    DebugRoutine("main") ;
    COMIO_Initialize() ;
    port = COMIO_Open(0x2F8, 3) ;  /* Com port 2 */
    COMIO_SetTypePort(port, COM_PORT_TYPE_8250) ;
    COMIO_SetBaudRate(port, 2400) ;
    COMIO_SetControl(port, COM_BIT_LENGTH_8, COM_STOP_BIT_1, COM_PARITY_NONE) ;
/*
    port2 = COMIO_Open(0x3f8, 0) ;
    COMIO_SetTypePort(port2, COM_PORT_TYPE_8250) ;
    COMIO_SetBaudRate(port2, 1200) ;
    COMIO_SetControl(port2, COM_BIT_LENGTH_8, COM_STOP_BIT_1, COM_PARITY_NONE) ;
*/

        printf("Carrier on port 1: %s\n",
            (COMIO_CheckCarrier(port) == TRUE)?"yes":"no") ;
    /* Turn the ports on. */
    COMIO_EnablePort(port) ;
        printf("Carrier on port 1: %s\n",
            (COMIO_CheckCarrier(port) == TRUE)?"yes":"no") ;
/*
    COMIO_EnablePort(port2) ;
*/
    printf("port 1 has received %d bytes.\n", COMIO_GetReceiveCount(port)) ;
/*
    printf("port 2 has received %d bytes.\n", COMIO_GetReceiveCount(port2)) ;
*/
    printf("port 1 has sending %d bytes.\n", COMIO_GetSendCount(port)) ;
/*
    printf("port 2 has sending %d bytes.\n", COMIO_GetSendCount(port2)) ;
*/
/*
    printf("Carrier on port 2: %s\n",
        (COMIO_CheckCarrier(port2) == TRUE)?"yes":"no") ;
*/
    while (!kbhit())  {
/*
        printf("%08X hits\n", hit_count) ;
*/
        value = COMIO_ReceiveByte(port) ;
        if (value != 0x9000)
            printf("%02X  ", value) ;
        fflush(stdout) ;

//        COMIO_SendByte(port, '*') ;
        if (value != 0x9000)
            if (COMIO_SendByte(port, value) == COM_ERROR_FULL_BUFFER)
                puts("Buffer full!") ;

/*
        outp(0x2f9, 0x0F) ;
*/
/*
        COMIO_ForceSendByte(port, 'b') ;
*/
    }
/*
    COMIO_DisablePort(port2) ;
*/
    COMIO_DisablePort(port) ;
/*
    COMIO_Close(port2) ;
*/
    COMIO_Close(port) ;

    printf("%08X hits\n", hit_count) ;
    DebugEnd() ;
}
