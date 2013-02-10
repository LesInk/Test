/****************************************************************************/
/*    FILE:  CREATURE.H                                                      */
/****************************************************************************/

#ifndef _CREATURE_H_
#define _CREATURE_H_

T_void CreaturesLoad(T_word32 mapNumber) ;

T_void CreaturesUnload(T_void) ;

T_void CreatureTakeDamage(
           T_3dObject *p_obj,
           T_word32 damage,
           T_byte8 type) ;

#ifdef SERVER_ONLY
T_3dObject *CreatureLookForPlayer(T_3dObject *p_obj) ;
T_void CreaturePlayerGone(T_player player) ;
#else
T_void CreaturePlayerGone(T_word16 playerId) ;
#endif

T_void CreaturesUpdate(T_void) ;

T_void CreatureAttachToObject(T_3dObject *p_obj) ;

#endif

/****************************************************************************/
/*    END OF FILE:  CREATURE.H                                               */
/****************************************************************************/
