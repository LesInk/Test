/****************************************************************************/
/*    FILE:  PACKET.C                                                       */
/****************************************************************************/

#include "standard.h"

/* Option to create the file RECEIVE.DAT to record all received bytes */
/* from the packet driver. */
//#define PACKET_CREATE_RECEIVE_FILE

/* Keep track of the number of the current packet.  Although this is mainly */
/* for debugging, we can use it to make sure packets are sent correctly.   */
/* If nothing else, we'll make sure everything is in order. */
static T_word32 G_packetID = 1 ;

static T_word16 IPacketComputeChecksum(T_packetEitherShortOrLong *p_packet) ;

/****************************************************************************/
/*  Routine:  PacketSendShort                                               */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    PacketSendShort is called to small (about 16 byte) packets out the    */
/*  currently active communications port.                                   */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_packetShort *shortPacket  -- Packet with data entry filled.  The    */
/*                                   rest of the fields will be filled out. */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    T_sword16                   -- 0 if packet sent, -1 if not sent       */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  12/13/94  Created                                                */
/*    LES  01/04/95  Modified so it returns a 0 if sent, -1 if not sent.    */
/*                                                                          */
/****************************************************************************/

T_sword16 PacketSendShort(T_packetShort *p_shortPacket)
{
    T_sword16 code = -1 ;

    DebugRoutine("PacketSendShort") ;
    DebugCheck(p_shortPacket != NULL) ;

    /* Store the header information in the packet. */
    p_shortPacket->header.prefix = PACKET_PREFIX ;

    /* Make this a short packet. */
    p_shortPacket->header.packetLength = SHORT_PACKET_LENGTH ;

    /* Put the id for this packet in the packet. */
    p_shortPacket->header.id = G_packetID++ ;

    /* Compute the checksum for this packet. */
    p_shortPacket->header.checksum =
        IPacketComputeChecksum((T_packetEitherShortOrLong *)p_shortPacket) ;

    /* Actually send the data out the port. */
    DirectTalkSendData((T_byte8 *)p_shortPacket, sizeof(T_packetShort)) ;
    /* Note that the packet was sent. */
    code = 0 ;

    DebugEnd() ;

    return code ;
}

/****************************************************************************/
/*  Routine:  PacketSendLong                                                */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    PacketSendLong sends a long packet of 80 characters out the           */
/*  currently active communications port.                                   */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_packetLong *p_longPacket  -- Packet with data entry filled.  The    */
/*                                   rest of the fields will be filled out. */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    T_sword16                   -- 0 if packet sent, -1 if not sent       */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  12/16/94  Created                                                */
/*    LES  01/04/95  Modified so it returns a 0 if sent, -1 if not sent.    */
/*                                                                          */
/****************************************************************************/

T_sword16 PacketSendLong(T_packetLong *p_longPacket)
{
    T_sword16 code = -1 ;

    DebugRoutine("PacketSendLong") ;
    DebugCheck(p_longPacket != NULL) ;

    /* Store the header information in the packet. */
    p_longPacket->header.prefix = PACKET_PREFIX ;

    /* Make this a long packet. */
    p_longPacket->header.packetLength = LONG_PACKET_LENGTH ;

    /* Put the id for this packet in the packet. */
    p_longPacket->header.id = G_packetID++ ;

    /* Compute the checksum for this packet. */
    p_longPacket->header.checksum =
        IPacketComputeChecksum((T_packetEitherShortOrLong *)p_longPacket) ;

    /* See if there is room to send the packet. */
    /* Actually send the data out the port. */
    DirectTalkSendData((T_byte8 *)p_longPacket, sizeof(T_packetLong)) ;

    /* Note that the packet was sent. */
    code = 0 ;

    DebugEnd() ;

    return code ;
}

/****************************************************************************/
/*  Routine:  PacketSendAnyLength                                           */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    PacketSendAnyLength sends a packet of any size up to a long packet    */
/*  size out the active communications port.                                */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_packetEitherShortOrLong *p_packet -- packet to send.                */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    T_sword16                   -- 0 if packet sent, -1 if not sent       */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  12/28/96  Created                                                */
/*                                                                          */
/****************************************************************************/

T_sword16 PacketSendAnyLength(T_packetEitherShortOrLong *p_anyPacket)
{
    T_sword16 code = -1 ;

    DebugRoutine("PacketSendAnyLength") ;
    DebugCheck(p_anyPacket != NULL) ;
    DebugCheck(p_anyPacket->header.packetLength <= LONG_PACKET_LENGTH) ;

    /* Store the header information in the packet. */
    p_anyPacket->header.prefix = PACKET_PREFIX ;

    /* Make this a long packet. */
//    p_anyPacket->header.packetLength = LONG_PACKET_LENGTH ;

    /* Put the id for this packet in the packet. */
    p_anyPacket->header.id = G_packetID++ ;

    /* Compute the checksum for this packet. */
    p_anyPacket->header.checksum = IPacketComputeChecksum(p_anyPacket) ;

    /* See if there is room to send the packet. */
    /* Actually send the data out the port. */
    DirectTalkSendData(
        (T_byte8 *)p_anyPacket,
        (T_word16)(sizeof(T_packetHeader) + p_anyPacket->header.packetLength)) ;

    /* Note that the packet was sent. */
    code = 0 ;

    DebugEnd() ;

    return code ;
}

/****************************************************************************/
/*  Routine:  PacketSend                                                    */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    PacketSend sends either a short or a long packet out the current      */
/*  communications port.                                                    */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_packetEitherShortOrLong *p_packet -- packet to send.                */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    PacketSendShort                                                       */
/*    PacketSendLong                                                        */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  01/23/95  Created                                                */
/*                                                                          */
/****************************************************************************/

T_sword16 PacketSend(T_packetEitherShortOrLong *p_packet)
{
    T_sword16 status ;

    DebugRoutine("PacketSend") ;

    switch(p_packet->header.packetLength)  {
        case SHORT_PACKET_LENGTH:
            status = PacketSendShort((T_packetShort *)p_packet) ;
            break ;
        case LONG_PACKET_LENGTH:
            status = PacketSendLong((T_packetLong *)p_packet) ;
            break ;
        default:
            DebugCheck(p_packet->header.packetLength <= LONG_PACKET_LENGTH) ;
            status = PacketSendAnyLength(p_packet) ;
            break ;
    }

    DebugEnd() ;

    return status ;
}

/****************************************************************************/
/*  Routine:  PacketSetId                                                   */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    PacketSetId sets the packet Id.  THIS SHOULD ONLY BE CALLED FROM      */
/*  WITHIN CMDQUEUE.                                                        */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_packetEitherShortOrLong * -- Pointer to packet whose ID must be     */
/*                   p_packet        changed.                               */
/*    T_word32 packetID           -- New ID to assign to the packet.        */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    AMT  07/15/95  Created                                                */
/*                                                                          */
/****************************************************************************/
T_void PacketSetId (T_packetEitherShortOrLong *p_packet, T_word32 packetID)
{
    p_packet->header.id = packetID;
}


/****************************************************************************/
/*  Routine:  IPacketComputeChecksum             * INTERNAL *               */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    IPacketComputeChecksum calculates a 16 bit checksum for the either    */
/*  short or long packet passed and returns that value.                     */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_packetEitherShortOrLong *packet -- Packet to compute checksum       */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    T_word16                    -- Calculated checksum                    */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    Nothing.                                                              */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  12/16/94  Created                                                */
/*                                                                          */
/****************************************************************************/

static T_word16 IPacketComputeChecksum(T_packetEitherShortOrLong *packet)
{
    T_word16 checksum ;
    T_word16 i ;

    DebugRoutine("IPacketComputeChecksum") ;
    DebugCheck(packet != NULL) ;

    /* Start out the checksum to equal the id of the block. */
    checksum = packet->header.id ;

    /* Add in the packet type. */
    checksum += packet->header.packetLength ;

    /* Loop the length of the data. */
    for (i=0; i<packet->header.packetLength; i++)  {
        /* If i is odd, then add.  Otherwise, do an exclusive-or to mix */
        /* up the bits. */
        if (i&1)
            checksum += packet->data[i] ;
        else
            checksum ^= packet->data[i] ;
    }

    DebugEnd() ;

    return checksum ;
}

/****************************************************************************/
/*  Routine:  PacketGet                                                     */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    PacketGet is the routine called to retrieve a packet from the         */
/*  currently active communications port.  If no packet is found, a -1      */
/*  is returned.  If a packet is found, a 0 is returned.                    */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_packetLong *packet        -- Packet location to receive data.       */
/*                                   Note that you must have room allocated */
/*                                   for a long packet in case either a     */
/*                                   long or a short packet is received.    */
/*                                   You will want to check the packet type */
/*                                   if you do receive data.                */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    T_sword16                   -- Resultant flag.  A -1 means no         */
/*                                   packet was received.  A 0 means a      */
/*                                   packet was found.                      */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    CommReadData                                                          */
/*    CommReadByte                                                          */
/*    CommGetReadBufferLength                                               */
/*    CommScanByte                                                          */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  07/13/96  Created                                                */
/*                                                                          */
/****************************************************************************/

static T_packetLong newPacket ;
static E_Boolean newPacketFilled ;

T_sword16 PacketGet(T_packetLong *p_packet)
{
    T_sword16 status ;

    DebugRoutine("PacketGet") ;

    newPacketFilled = FALSE ;

    DirectTalkPollData() ;

    if (newPacketFilled)  {
        status = 0 ;
        *p_packet = newPacket ;
    } else  {
        status = -1 ;
    }

    DebugEnd() ;

    return status ;
}

#ifdef PACKET_CREATE_RECEIVE_FILE
static FILE *G_fpRecv ;

T_void PacketCloseReceiveFile(T_void)
{
    fclose(G_fpRecv) ;
}
#endif

T_void PacketReceiveData(T_void *p_data, T_byte8 size)
{
    T_word16 i ;

    DebugRoutine("ConnectReceiveData") ;
    DebugCheck(p_data != NULL) ;

#ifdef PACKET_CREATE_RECEIVE_FILE
    if (G_fpRecv == NULL)  {
        G_fpRecv = fopen("receive.dat", "wb") ;
        atexit(PacketCloseReceiveFile) ;
    }

    fprintf(G_fpRecv, "CONNECT: Received %d data\n", size) ;
    for (i=0; i<size; i++)
        fprintf(G_fpRecv, "<%02X>", ((T_byte8 *)p_data)[i]) ;
    fprintf(G_fpRecv, "\n") ;
#endif

    memcpy(&newPacket, p_data, size) ;
    newPacketFilled = TRUE ;

    DebugEnd() ;
}

/****************************************************************************/
/*    END OF FILE:  PACKET.C                                                */
/****************************************************************************/
