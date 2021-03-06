/****************************************************************************/
/*    FILE:  CLI_RECV.H             Client Receive Packet Module            */
/****************************************************************************/

#ifndef _CLI_RECV_H_
#define _CLI_RECV_H_

T_void ClientReceivePlaceStartPacket(T_packetEitherShortOrLong *p_packet) ;

T_void ClientReceivePlayerLogoffPacket(T_packetEitherShortOrLong *p_packet) ;

T_void ClientReceiveMessagePacket(T_packetEitherShortOrLong *p_packet) ;

T_void ClientReceiveGotoPlacePacket(T_packetEitherShortOrLong *p_gotoPacket) ;

T_void ClientReceivedMemoryBlock
                   (T_void *p_data,
                    T_word32 size,
                    T_word32 extraData) ;

T_void ClientReceiveServerIDPacket(T_packetEitherShortOrLong *p_packet);

T_void ClientReceiveRequestEnterStatusPacket(
           T_packetEitherShortOrLong *p_packet);

T_void ClientReceiveLoadCharStatusPacket(
           T_packetEitherShortOrLong *p_packet) ;

T_void ClientReceiveCreateCharStatusPacket(
           T_packetEitherShortOrLong *p_packet) ;

T_void ClientReceiveDeleteCharStatusPacket(
           T_packetEitherShortOrLong *p_packet) ;

T_void ClientReceiveCheckPasswordStatusPacket(
           T_packetEitherShortOrLong *p_packet) ;

T_void ClientReceiveChangePasswordStatusPacket(
           T_packetEitherShortOrLong *p_packet) ;

T_void ClientReceiveRequestDataBlockPacket(
           T_packetEitherShortOrLong *p_packet) ;

T_void ClientReceiveRequestPiecewiseListPacket(
           T_packetEitherShortOrLong *p_packet) ;

T_void ClientReceiveSyncPacket(
           T_packetEitherShortOrLong *p_packet) ;

T_void ClientReceiveTownUIMessagePacket(
           T_packetEitherShortOrLong *p_packet) ;

T_void ClientReceivePlayerIDSelf(
           T_packetEitherShortOrLong *p_packet) ;

T_void ClientReceiveRequestPlayerIDPacket(
           T_packetEitherShortOrLong *p_packet) ;

T_void ClientReceiveGameRequestJoinPacket(
           T_packetEitherShortOrLong *p_packet) ;

T_void ClientReceiveGameRespondJoinPacket(
           T_packetEitherShortOrLong *p_packet) ;

T_void ClientReceiveGameStartPacket(
           T_packetEitherShortOrLong *p_packet) ;

#endif

/****************************************************************************/
/*    END OF FILE: CLI_RECV.H       Client Send Packet Module               */
/****************************************************************************/

