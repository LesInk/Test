/****************************************************************************/
/*    FILE:  CREATURE.C                                                     */
/****************************************************************************/

#include "standard.h"

T_void CreatureUpdateFireball (T_3dObject *p_obj);

T_void CreatureUpdateElf(
           T_3dObject *p_obj,
           T_word32 delta,
           T_word32 newTime) ;

T_void CreatureUpdateFlyer(
           T_3dObject *p_obj,
           T_word32 delta,
           T_word32 newTime) ;

T_void CreatureUpdateGriffon(
           T_3dObject *p_obj,
           T_word32 delta,
           T_word32 newTime) ;

T_void CreatureUpdateBanshee(
           T_3dObject *p_obj,
           T_word32 delta,
           T_word32 newTime) ;

T_void CreatureUpdateWalker(
           T_3dObject *p_obj,
           T_word32 delta,
           T_word32 newTime) ;

T_void CreatureUpdateLich(
           T_3dObject *p_obj,
           T_word32 delta,
           T_word32 newTime) ;

T_void CreatureUpdateDruid(
           T_3dObject *p_obj,
           T_word32 delta,
           T_word32 newTime) ;

T_void CreatureUpdateHydra(
           T_3dObject *p_obj,
           T_word32 delta,
           T_word32 newTime) ;

T_void CreatureUpdateRandomWalker(
           T_3dObject *p_obj,
           T_word32 delta,
           T_word32 newTime) ;

T_void CreatureUpdateDragon(
           T_3dObject *p_obj,
           T_word32 delta,
           T_word32 newTime) ;

T_void CreatureUpdateApe(
           T_3dObject *p_obj,
           T_word32 delta,
           T_word32 newTime) ;

T_void CreatureUpdateBronzeBarbarian(
           T_3dObject *p_obj,
           T_word32 delta,
           T_word32 newTime) ;

T_void CreatureUpdateBlackBarbarian(
           T_3dObject *p_obj,
           T_word32 delta,
           T_word32 newTime) ;

T_void CreatureUpdateWhiteBarbarian(
           T_3dObject *p_obj,
           T_word32 delta,
           T_word32 newTime) ;

#ifdef SERVER_ONLY

#define MAX_CREATURES 100
#define CREATURE_BAD NULL

/* Extra data for a creature that is stored in an object's */
/* extra data slot. */
typedef struct T_creatureStruct {

    T_word16 state ;
    T_sword32 stateData ;
//    T_word16 target ;
    T_3dObject *p_target ;
    T_word16 stepping ;
    E_Boolean goForward ;
    E_Boolean needTurn ;
    T_word16 turnWay ;
    T_sword32 stepAway ;
    T_word16 playerId ;
    T_word32 nextScream;
    T_word32 nextAttack ;
} T_creature ;

/* Note if we have initialized the creatuers module. */
static E_Boolean G_creaturesInit = FALSE ;

/* Note when the creatures were last updated. */
static T_word32 G_lastCreatureUpdate = 0 ;
static T_word32 G_newCreatureUpdate = 0 ;
static T_word32 G_deltaTime = 0 ;

/* Internal prototypes: */
T_3dObject *CreatureLookForPlayer(T_3dObject *p_obj) ;

T_void ICreatureDestroy(T_creature *p_creature) ;

T_void CreatureUpdateSplat(T_3dObject *p_obj) ;

T_void ICreatureShootFireball(T_3dObject *p_creatureObj, T_3dObject *target) ;

E_Boolean ICreatureUpdate(T_3dObject *p_obj, T_word32 time) ;

E_Boolean ICreaturePlayerGone(T_3dObject *p_obj, T_word32 data) ;

/****************************************************************************/
/*  Routine:  CreaturesLoad                                                 */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreaturesLoad loads the creature information file and sets up all     */
/*  the creature memory.                                                    */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_word16 mapNumber          -- Map to load creatures from             */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
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
/*    LES  02/23/95  Created                                                */
/*    LES  05/30/95  Made creatures always impassable.                      */
/*    LES  07/10/95  Uses object system now.                                */
/*                                                                          */
/****************************************************************************/

T_void CreaturesLoad(T_word32 mapNumber)
{
    FILE *fp ;
    char filename[20] ;
    T_creature *p_creature ;
    T_word16 i ;
    T_sword32 valueA, valueB, valueC ;
    T_3dObject *p_obj ;
    T_word16 numberCreatures ;
    T_file file;

    DebugRoutine("CreaturesLoad") ;
    DebugCheck(G_creaturesInit == FALSE) ;

    /* Find the file name of the creature file. */
    sprintf(filename, "l%ld.cre", mapNumber) ;

    /* Open up the file. */
    fp = fopen(filename, "r") ;


    DebugCheck(fp != NULL) ;

    /* How many creatures is it telling us that we have. */
    fscanf(fp, "%d", &valueA) ;
    numberCreatures = valueA ;
numberCreatures = 0 ;

    /* Loop and create each of the creatures. */
    for (i=0; i<numberCreatures; i++)  {
        /* Each creature requires memory. */
        p_creature = MemAlloc(sizeof(T_creature)) ;

        /* If we don't get the memory.  Stop here and say this */
        /* is the last creature. */
        DebugCheck(p_creature != NULL) ;
        if (p_creature == NULL)  {
            numberCreatures = i ;
            break ;
        }

        /* Clear out the creature data before we start. */
        memset(p_creature, 0, sizeof(T_creature)) ;

        /* Read in some more information. */
        fscanf(fp, "%d %d %d",
            &valueA,
            &valueB,
            &valueC) ;

        /* Use the object number to get more information about the */
        /* creature. */
        p_obj = ObjectFind((T_word16)valueA) ;
        if (p_obj != NULL)  {
            ObjectSetExtraData(p_obj, p_creature) ;
            ObjectSetHealth(p_obj, valueC) ;

            /* Make the creature impassable. */
//            ObjectMakeImpassable(p_obj) ;

            /* Start the creature in the initial state. */
            p_creature->state = 0 ;

            /* No packets have been sent about this creature. */
            ObjectSetNumPackets(p_obj, 0) ;
        } else {
            DebugCheck(FALSE) ;
        }
    }

    fclose(fp) ;

    /* Start the creature updates from this time point. */
    G_lastCreatureUpdate = TickerGet() ;

    G_creaturesInit = TRUE ;

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  CreatureAttachToObject                                        */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreatureAttachToObject does the job of making an object into a        */
/*  creature by allocating a creature structure and binding the two         */
/*  together.                                                               */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_3dObject *p_obj           -- Object to turn into a creature         */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    MemAlloc                                                              */
/*    memset                                                                */
/*    ObjectSetExtraData                                                    */
/*    ObjectSetHealth                                                       */
/*    ObjectMakeImpassible                                                  */
/*    ObjectSetNumPackets                                                   */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  02/12/96  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void CreatureAttachToObject(T_3dObject *p_obj)
{
    T_creature *p_creature ;

    DebugRoutine("CreatureAttachToObject") ;
    DebugCheck(p_obj != NULL) ;

    if (p_obj)  {
        /* Each creature requires memory. */
        p_creature = MemAlloc(sizeof(T_creature)) ;

        /* If we don't get the memory.  Stop here and say this */
        /* is the last creature. */
        DebugCheck(p_creature != NULL) ;
        if (p_creature != NULL)  {
            /* Clear out the creature data before we start. */
            memset(p_creature, 0, sizeof(T_creature)) ;

            /* Make the object use this creature data as its extra */
            /* data. */
            ObjectSetExtraData(p_obj, p_creature) ;
            ObjectSetHealth(p_obj, ObjTypeGetHealth(p_obj->p_objType)) ;

            /* Make the creature impassable. */
//            ObjectMakeImpassable(p_obj) ;

            /* Start the creature in the initial state. */
            p_creature->state = 0 ;

            /* No packets have been sent about this creature. */
            ObjectSetNumPackets(p_obj, 0) ;
        }
    }

    DebugEnd() ;
}


/****************************************************************************/
/*  Routine:  CreaturesUnload                                               */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreaturesUnload removes any creature data that is in memory.          */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
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
/*    LES  02/23/95  Created                                                */
/*    LES  07/10/95  Uses object system now.                                */
/*                                                                          */
/****************************************************************************/

T_void CreaturesUnload(T_void)
{
    T_creature *p_next ;
    T_word16 i ;

    DebugRoutine("CreaturesUnload") ;
    DebugCheck(G_creaturesInit == TRUE) ;

    G_creaturesInit = FALSE ;

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  CreaturesUpdate                                               */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreaturesUpdate moves all the creatures in play.                      */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    ObjectCreate                                                          */
/*    PlayerGetX                                                            */
/*    PlayerGetY                                                            */
/*    ObjectDeclareStatic                                                   */
/*    TickerGet                                                             */
/*    ObjectDestroy                                                          */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  02/23/95  Created                                                */
/*    LES  03/01/95  Improved for movement and attacking.                   */
/*    LES  04/16/95  Broke apart logic to allow multiple types of creatures.*/
/*    LES  05/30/95  Only updates creatures that have health now.           */
/*    LES  07/10/95  Uses object system now.                                */
/*                                                                          */
/****************************************************************************/

T_void CreaturesUpdate(T_void)
{
    T_creature *p_creature ;
    static T_word32 lastTime = 0 ;
    T_word32 newTime ;
    T_word32 delta ;

    DebugRoutine("CreaturesUpdate") ;

    /* Get the update time, and calculate the delta time since */
    /* the last update.  Unfortunately, the callback to ObjectsDoToAll */
    /* doesn't take data. */
    G_newCreatureUpdate = TickerGet() ;
    G_deltaTime = G_newCreatureUpdate - G_lastCreatureUpdate ;

    ObjectsDoToAll(ICreatureUpdate, G_newCreatureUpdate) ;

    /* Note the time we updated. */
    G_lastCreatureUpdate = G_newCreatureUpdate ;

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  ICreatureUpdate                                               */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    ICreatureUpdate updates the movement of one creature as called by     */
/*  ObjectsDoToAll (via CreaturesUpdate).  It uses the G_newCreatureUpdata  */
/*  and G_deltaTime to determine update times.                              */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_3dObject *p_obj           -- object/creature to update              */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    ObjectGetScript                                                       */
/*    CreatureUpdateWalker                                                  */
/*    CreatureUpdateFlyer                                                   */
/*    CreatureUpdateRandom                                                  */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  07/10/95  Derived from old CreatureUpdate                        */
/*    AMT  07/18/95  Modified so it passes over weapons, who use the script */
/*                   field for their own devious purposes.                  */
/*                                                                          */
/****************************************************************************/

E_Boolean ICreatureUpdate(T_3dObject *p_obj, T_word32 time)
{
    T_word32 script ;

    DebugRoutine("ICreatureUpdate") ;

    /* Get the script attached to this object. */
    if (p_obj->p_objType != NULL)  {
        DebugCheck(p_obj->p_objType != NULL) ;
        script = ObjectGetScript(p_obj) ;

        /* If there IS a script, and it's not a weapon, do that script. */
        if ((!(ObjectGetAttributes (p_obj) & OBJECT_ATTR_WEAPON)) &&
	    (script != 0))  {
#if 1
             /* !!! Actual script handling is called here.  However, */
             /* since we are hard-coding monster movement for now, */
             /* we MUST use a switch statement. */
             switch(script)  {
                 case 1:    /* Random walker (stefan) */
                     CreatureUpdateRandomWalker(
                         p_obj,
                         G_deltaTime,
                         time) ;
                     break ;
                 case 2:    /* attacker (biff) */
                     CreatureUpdateWalker(
                         p_obj,
                         G_deltaTime,
                         time) ;
                     break ;
                 case 3:    /* flyer (shadow) */
                     CreatureUpdateFlyer(
                         p_obj,
                         G_deltaTime,
                         time) ;
                     break ;
                 case 23:   /* griffon */
                     CreatureUpdateGriffon(
                         p_obj,
                         G_deltaTime,
                         time) ;
                     break ;
                 case 12:   /* banshee */
                     CreatureUpdateBanshee(
                         p_obj,
                         G_deltaTime,
                         time) ;
                     break ;
                 case 4:    /* straight & boom (fireball) */
                     CreatureUpdateFireball (p_obj);
                     break ;
                 case 5:    /* Dragon */
                     CreatureUpdateDragon(p_obj, G_deltaTime, time);
                     break ;
                 case 6:    /* attacker (ape) */
                     CreatureUpdateApe(
                         p_obj,
                         G_deltaTime,
                         time) ;
                     break ;
                 case 7:    /* straight & boom (fireball) */
                     CreatureUpdateFireball (p_obj);
                     break ;
                 case 18:
                     CreatureUpdateElf(
                         p_obj,
                         G_deltaTime,
                         time) ;
                     break ;
                 case 19:
                     CreatureUpdateDruid(
                         p_obj,
                         G_deltaTime,
                         time) ;
                     break ;
                 case 20:
                     CreatureUpdateLich(
                         p_obj,
                         G_deltaTime,
                         time) ;
                     break ;
                 case 22:   /* hydra */
                     CreatureUpdateHydra(
                         p_obj,
                         G_deltaTime,
                         time) ;
                     break ;
                 case 26:
                     CreatureUpdateBronzeBarbarian(
                         p_obj,
                         G_deltaTime,
                         time) ;
                     break ;
                 case 27:
                     CreatureUpdateBlackBarbarian(
                         p_obj,
                         G_deltaTime,
                         time) ;
                     break ;
                 case 28:
                     CreatureUpdateWhiteBarbarian(
                         p_obj,
                         G_deltaTime,
                         time) ;
                 default:
#if 1
                     CreatureUpdateWalker(
                         p_obj,
                         G_deltaTime,
                         time) ;
                     break ;
#else
#ifndef NDEBUG
                     printf("bad script %d\n", script) ;
#endif
                     /* Bad script, stop here. */
                     DebugCheck(FALSE) ;
                     break ;
#endif
             }
#else
             /* NOw that we have determined that we have a creature */
             /* with a script ... call the script with a time update. */
             if (script == 4)
                 CreatureUpdateFireball(p_obj) ;
             else
                 ScriptEvent(
                     ObjectGetScriptHandle(p_obj),
                     SCRIPT_EVENT_TIME_UPDATE,
                     SCRIPT_DATA_TYPE_NONE,
                     NULL,
                     SCRIPT_DATA_TYPE_NONE,
                     NULL,
                     SCRIPT_DATA_TYPE_NONE,
                     NULL) ;

#endif
        }
    }

    DebugEnd() ;

    return FALSE ;
}

/****************************************************************************/
/*  Routine:  CreatureLookForPlayer                                         */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreatureLookForPlayer determines if there is player in the direction  */
/*  that a creature is facing.  If multiple players are seen, it takes the  */
/*  closest one.                                                            */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_creature *p_creature      -- Creature that is looking               */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    T_sword16                   -- Number of player, or -1.               */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    PlayersGetFirstPlayer                                                 */
/*    PlayersGetNextPlayer                                                  */
/*    MathArcTangent                                                        */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  03/01/95  Created                                                */
/*                                                                          */
/****************************************************************************/

T_3dObject *CreatureLookForPlayer(T_3dObject *p_obj)
{
    T_player player ;
    T_word32 distance ;
    T_word32 bestDistance = 0xFFFFFFFF;
    T_player closest = NULL ;
    T_sword16 playerX, playerY ;
    T_sword32 x, y ;
    T_sword32 dx, dy ;
    T_word16 angle ;
    T_3dObject *p_playerObj ;
    T_3dObject *p_found ;

    DebugRoutine("CreatureLookForPlayer") ;
    DebugCheck(p_obj != NULL) ;

    x = ObjectGetX16(p_obj) ;
    y = ObjectGetY16(p_obj) ;

    /* Go through each player looking for the those in the view */
    /* and then find the closest of those. */
    player = PlayersGetFirstPlayer() ;
    while (player)  {
        p_playerObj = PlayersGetPlayerObject(player) ;

        /* See where player is located. */
        playerX = ObjectGetX16(p_playerObj) ;
        playerY = ObjectGetY16(p_playerObj) ;

        dx = ((T_sword32)playerX) - x ;
        dy = ((T_sword32)playerY) - y ;

        /* What angle is that? */
        angle = MathArcTangent((T_sword16)dx, (T_sword16)dy) ;

        /* Is that angle in the 90 degrees that the creature sees? */
        if (((T_word16)((ObjectGetAngle(p_obj) - angle)+INT_ANGLE_45))
                             < ((T_word16)INT_ANGLE_90))  {
            /* Yes, the player is in view. */
            /* How far is this one. */
            distance = (dx*dx) + (dy*dy) ;

            /* Is it closer than the last player we found. */
            if (distance < bestDistance)  {
                /* Yes.  Make this the new choice target. */
                bestDistance = distance ;
                closest = player ;
            }
        }

        player = PlayersGetNextPlayer(player) ;
    }

    if (closest == NULL)
        p_found = NULL ;
    else
        p_found = PlayersGetPlayerObject(closest) ;

    DebugEnd() ;

    return p_found ;
}

/****************************************************************************/
/*  Routine:  CreaturePlayerGone                                            */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreaturePlayerGone is called when a player has left the game (either  */
/*  by choice or dies).  This routine goes through all the creatures and    */
/*  uses this information to get the creatures to change targets if their   */
/*  target was the given player.                                            */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_creature *p_creature      -- Creature that is being sent            */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    T_sword16                   -- Number of player, or -1.               */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    Nothing                                                               */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  03/01/95  Created                                                */
/*    LES  07/10/95  Changed to make use of the object system.              */
/*                                                                          */
/****************************************************************************/

T_void CreaturePlayerGone(T_player player)
{
    DebugRoutine("CreaturePlayerGone") ;

    ObjectsDoToAll(
        ICreaturePlayerGone, 
        (T_word32)
            (PlayersGetPlayerObject(player))) ;

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  ICreaturePlayerGone                * INTERNAL *               */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    ICreaturePlayerGone is called when a player has left the game (either */
/*  by choice or dies).  This routine goes through all the creatures and    */
/*  uses this information to get the creatures to change targets if their   */
/*  target was the given player.                                            */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_3dObject *p_obj           -- Creature/object to check target of     */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    E_Boolean                   -- always FALSE                           */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    Nothing                                                               */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  07/10/95  Created                                                */
/*                                                                          */
/****************************************************************************/

E_Boolean ICreaturePlayerGone(T_3dObject *p_obj, T_word32 data)
{
    T_creature *p_creature ;
    T_3dObject *p_playerObj ;

    DebugRoutine("ICreaturePlayerGone") ;

    p_playerObj = (T_3dObject *)data ;

    /* Does this object have an intelligence? */
    if (ObjectGetScript(p_obj) != 0)  {
        /* Yes.  Get rid of the targeted item. */
        p_creature = (T_creature *)ObjectGetExtraData(p_obj) ;
        if (p_creature != NULL)  {
            if (p_creature->p_target == p_playerObj)
                p_creature->state = 0 ;
        }
    }

    DebugEnd() ;

    return FALSE ;
}

/****************************************************************************/
/*  Routine:  ICreatureDestroy                                              */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    ICreatureDestroy removes a creature from the creature list and frees  */
/*  the memory used by the creature.                                        */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_creature *p_creature      -- Creature that was hit                  */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    Nothing.                                                              */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    MemFree                                                               */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  03/08/95  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void ICreatureDestroy(T_creature *p_creature)
{
    T_creature *p_prev ;

    DebugRoutine("ICreatureDestroy") ;


    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  CreatureUpdateRandomWalker                                    */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreatureUpdateRandomWalker is used to move creatures that walk        */
/*  randomly and DONT attack.                                               */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_creature *p_creature      -- creature to update                     */
/*                                                                          */
/*    T_word32 delta              -- Amount to move/update creature         */
/*                                                                          */
/*    T_word32 newTime            -- Current time to update                 */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    MathArcTangent                                                        */
/*    CalculateDistance                                                     */
/*    ICreatureSendCreatureAttack                                           */
/*    View3dSetExceptObject                                                 */
/*    View3dMoveTo                                                          */
/*    CreatureLookForPlayer                                                 */
/*    View3dGetFloorAndCeiling                                              */
/*    MapGetFloorWalkingHeight                                              */
/*    ICreatureSendMoveCreaturePacket                                       */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  04/28/95  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void CreatureUpdateRandomWalker(
           T_3dObject *p_obj,
           T_word32 delta,
           T_word32 newTime)
{
    T_sword16 playerX, playerY ;
    T_word16 angle ;
    T_sword16 diffangle ;
    T_sword16 absdiffangle ;
    T_word16 distance ;
    T_3dObject *p_newTarget ;
    T_word16 turnangle ;
    T_creature *p_creature ;

    DebugRoutine("CreatureUpdateRandomWalker") ;
    DebugCheck(p_obj != NULL) ;

    ObjectSetMaxVelocity(p_obj, 8) ;
    p_creature = (T_creature *)ObjectGetExtraData(p_obj) ;

    if (p_creature != NULL)  {

        /** Is it time for a noise? **/
        if (TickerGet () > p_creature->nextScream)
        {
           if (p_creature->nextScream == 0)
           {
              /** First time.  Don't sound off, just set the timer. **/
           }
           else
           {
              /** Time for a holler. **/
              if (p_creature->state < 2)
                  ServerAreaSoundGlobal (
                        (T_sword16)(ObjectGetX16 (p_obj)),
                        (T_sword16)(ObjectGetY16 (p_obj)),
                        1000,
                        50);
           }
           /** Set the timer for the next scream. **/
           p_creature->nextScream = TickerGet() + 700 + (rand () % 700);
        }

        /* First, see if the creature was blocked last time. */
        if (ObjectWasBlocked(p_obj))  {
            /* We are hitting something, let's turn randomly */
            /* 45 degrees to one side. */
            if (p_creature->goForward != TRUE)
                p_creature->turnWay = TickerGet() & 1 ;

            if (p_creature->turnWay)  {
                ObjectAddAngle(p_obj, INT_ANGLE_45) ;
            } else {
                ObjectAddAngle(p_obj, -INT_ANGLE_45) ;
            }

            p_creature->goForward = TRUE ;
            p_creature->stepAway = 70 ;

            /* Make sure future access say we were not blocked. */
            ObjectClearBlockedFlag(p_obj) ;
        }

        /* Check to see if we still have a target. */
        if (p_creature->p_target == NULL)
            /* Nope, go into the mode to get one. */
            p_creature->state = 0 ;

        switch(p_creature->state)  {
            case 0:   /* Initial state -- looking for a target. */
                ObjectSetStance(p_obj, STANCE_STAND) ;
                if ((p_creature->p_target =
                        CreatureLookForPlayer(p_obj)) != NULL)  {
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;
                    /* !!! Creature sound that he sees you! */
                }
                break ;
            case 1:   /* Moving towards opponent state. */
                /* Make us animate as walking. */
                ObjectSetStance(p_obj, STANCE_WALK) ;

                /* Locate where the target is. */
/*
                ServerGetPlayerLocation(
                    p_creature->target,
                    &playerX,
                    &playerY) ;
*/

                /* Get the angle to this target. */
/*
                angle = MathArcTangent(
                            playerX - ObjectGetX16(p_obj),
                            playerY - ObjectGetY16(p_obj)) ;
*/
                angle = ObjectGetAngle(p_obj) ;

                /* What is the difference to the angle I'm currently */
                /* facing? */
                diffangle = ObjectGetAngle(p_obj) - angle ;
                if (diffangle < 0)
                    absdiffangle = -diffangle ;
                else
                    absdiffangle = diffangle ;

                if (p_creature->goForward == TRUE)  {
                    absdiffangle = 0 ;
                    p_creature->stepAway -= delta ;
                    if (p_creature->stepAway < 0)
                        p_creature->goForward = FALSE ;
                }

                /* Is that a big angle or a small one? */
                if (absdiffangle >= INT_ANGLE_45)  {
                    /* Yeah, a big angle.  Just turn on our heels. */
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, (delta << 8)) ;
                    else
                        ObjectAddAngle(p_obj, -((T_sword16)(delta << 8))) ;
                } else {
                    /* Not a big angle, we might as well go ahead */
                    /* and try moving a bit. */

                    /* Turn as needed. */
                    turnangle = (delta<<6) ;
                    if (turnangle > absdiffangle)
                        turnangle = absdiffangle ;
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, turnangle) ;
                    else
                        ObjectAddAngle(p_obj, -turnangle) ;

                    /* How close are we? */
                    distance = CalculateEstimateDistance(
                                   (T_sword16)(ObjectGetX16(p_obj)),
                                   (T_sword16)(ObjectGetY16(p_obj)),
                                   playerX,
                                   playerY) ;

    
    

                    /* Are we close enough to attack? */
                    if (distance < 70)  {
                        /* !!! Attack !!! */
                        ObjectSetStance(p_obj, STANCE_ATTACK) ;
                        ObjectSetAngle(p_obj, angle) ;

                        /* Set the time for pausing on the attack. */
                        p_creature->state = 2 ;
                        p_creature->stateData = 27 ;
                    } else {
                        /* Too far, let's move closer. */
                        ObjectAccelFlat(p_obj, delta, ObjectGetAngle(p_obj)) ;
                        p_creature->stepping += delta ;

                        /* See if we are tired of moving and want */
                        /* a new opponent. */
                        if (p_creature->stateData <= newTime)  {
                            /* Look around for opponents. */
                            p_newTarget = CreatureLookForPlayer(p_obj) ;
                            if (p_newTarget != NULL)
                                p_creature->p_target = p_newTarget ;
                            p_creature->stateData = newTime+210 ;
                        }
                    }
                }
                break ;
            case 2:   /* Pause after attacking or getting hurt. */
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 10)
                    /* Yes, continue going after target. */
                    p_creature->state = 3 ;
                break ;
            case 3:
                ObjectSetStance(p_obj, STANCE_STAND) ;
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 0)  {
                    /* Yes, continue going after target. */
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;
                }
                break ;
        }
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  CreatureUpdateFlyer                                           */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreatureUpdateFlyer is used to move a creature that flies.            */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_creature *p_creature      -- creature to update                     */
/*                                                                          */
/*    T_word32 delta              -- Amount to move/update creature         */
/*                                                                          */
/*    T_word32 newTime            -- Current time to update                 */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    MathArcTangent                                                        */
/*    CalculateDistance                                                     */
/*    ICreatureSendCreatureAttack                                           */
/*    View3dSetExceptObject                                                 */
/*    View3dMoveTo                                                          */
/*    CreatureLookForPlayer                                                 */
/*    View3dGetFloorAndCeiling                                              */
/*    MapGetFloorWalkingHeight                                              */
/*    ICreatureSendMoveCreaturePacket                                       */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  04/16/95  Created                                                */
/*    LES  09/15/95  Added code to make sure flyer doesn't go through       */
/*                   floor or ceiling.                                      */
/*                                                                          */
/****************************************************************************/

T_void CreatureUpdateFlyer(
           T_3dObject *p_obj,
           T_word32 delta,
           T_word32 newTime)
{
    T_sword16 playerX, playerY, playerHeight ;
    T_word16 angle ;
    T_sword16 diffangle ;
    T_sword16 absdiffangle ;
    T_word16 distance ;
    T_3dObject *p_newTarget ;
    T_word16 turnangle ;
    T_creature *p_creature ;
    T_sword16 moveDelta ;
    T_sword32 frontX, frontY ;
    E_Boolean wasBlocked = FALSE ;
    T_sword16 oldZ ;
    T_sword16 newZ ;
    T_sword16 limit ;

    DebugRoutine("CreatureUpdateFlyer") ;
    DebugCheck(p_obj != NULL) ;

    ObjectSetMaxVelocity(p_obj, 24) ;
    ObjectClearMoveFlags(p_obj, OBJMOVE_FLAG_FAST_MOVEMENT) ;

    p_creature = (T_creature *)ObjectGetExtraData(p_obj) ;

    if (p_creature != NULL)  {

        /** Is it time for a noise? **/
        if (TickerGet () > p_creature->nextScream)
        {
           if (p_creature->nextScream == 0)
           {
              /** First time.  Don't sound off, just set the timer. **/
           }
           else
           {
              if (p_creature->state < 2)  {
                  /** Time for a holler. **/
                  ServerAreaSoundGlobal (
                        (T_sword16)(ObjectGetX16 (p_obj)),
                        (T_sword16)(ObjectGetY16 (p_obj)),
                        1000,
                        124);
              }
           }
           /** Set the timer for the next scream. **/
           p_creature->nextScream = TickerGet() + 350 + (rand () % 350);
        }

        /* Make sure our movement doesn't keep pulling it down. */
        ObjectSetMoveFlags(
            p_obj,
            OBJMOVE_FLAG_IGNORE_GRAVITY |
            OBJMOVE_FLAG_IGNORE_FRICTION |
            OBJMOVE_FLAG_FAST_MOVEMENT) ;

        /* Check to see if we still have a target. */
        if (p_creature->p_target == NULL /* -1 */)
            /* Nope, go into the mode to get one. */
            p_creature->state = 0 ;

        switch(p_creature->state)  {
            case 0:   /* Initial state -- looking for a target. */
                ObjectSetStance(p_obj, STANCE_STAND) ;
                if ((p_creature->p_target =
                        CreatureLookForPlayer(p_obj)) != NULL)  {
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;

                    /* Creature sound that he sees you! */
                    ServerAreaSoundGlobal (
                           (T_sword16)(ObjectGetX16 (p_obj)),
                           (T_sword16)(ObjectGetY16 (p_obj)),
                           1000,
                           120);
                }
                break ;
            case 1:   /* Moving towards opponent state. */
                /* Make us animate as walking. */
                ObjectSetStance(p_obj, STANCE_WALK) ;

                /* First, see if the creature was blocked last time. */
                if (ObjectWasBlocked(p_obj))  {
                    /* We are hitting something, let's turn randomly */
                    /* 45 degrees to one side. */
                    if (p_creature->needTurn == TRUE)  {
                        p_creature->turnWay = TickerGet() & 1 ;
                        p_creature->needTurn = FALSE ;
                    }

                    if (p_creature->turnWay)  {

                        ObjectAddAngle(p_obj, (delta<<8)) ;
                    } else {

                        ObjectAddAngle(p_obj, -((T_sword32)(delta<<8))) ;
                    }

                    p_creature->goForward = TRUE ;
                    p_creature->stepAway = 20 ;
                    wasBlocked = TRUE ;

                    /* Make sure future access say we were not blocked. */
                    ObjectClearBlockedFlag(p_obj) ;
                } else {
                    p_creature->needTurn = TRUE ;
                }

                /* Locate where the target is. */
                playerX = ObjectGetX16(p_creature->p_target) ;
                playerY = ObjectGetY16(p_creature->p_target) ;
/*
                ServerGetPlayerLocation(
                    p_creature->target,
                    &playerX,
                    &playerY) ;
*/

/*
                ServerGetPlayerHeight(
                    p_creature->target,

                    &playerHeight) ;
*/
                playerHeight = ObjectGetZ16(p_creature->p_target) ;

                /* Move creature up and down to match player height */
                oldZ = ObjectGetZ16(p_obj) ;
                if (oldZ > playerHeight)  {
                    moveDelta = oldZ - playerHeight ;
                    if (moveDelta > delta)
                        moveDelta = delta ;
                    moveDelta = -moveDelta ;
                } else {
                    moveDelta = playerHeight - oldZ ;
                    if (moveDelta > delta)
                        moveDelta = delta ;
                }

                /* Check to see that the creatures isn't going to */
                /* go through the roof. */
                newZ = oldZ + moveDelta ;
                limit = (ObjectGetHighestPoint(p_obj)>>16) -
                            ObjectGetHeight(p_obj) ;
                if (newZ > limit)
                    newZ = limit ;

                /* Make sure creature doesn't go through floor. */
                limit = (ObjectGetLowestPoint(p_obj)>>16) ;
                if (newZ < limit)
                    newZ = limit ;
                ObjectSetZ16(p_obj, newZ) ;

                /* Get the angle to this target. */
                angle = MathArcTangent(
                            (T_sword16)(playerX - ObjectGetX16(p_obj)),
                            (T_sword16)(playerY - ObjectGetY16(p_obj))) ;

                /* Face directly. */



                /* What is the difference to the angle I'm currently */
                /* facing? */
                diffangle = ObjectGetAngle(p_obj) - angle ;
                if (diffangle < 0)
                    absdiffangle = -diffangle ;
                else
                    absdiffangle = diffangle ;

                if (p_creature->goForward == TRUE)  {
                    absdiffangle = 0 ;
                    p_creature->stepAway -= delta ;
                    if (p_creature->stepAway < 0)
                        p_creature->goForward = FALSE ;
                }

                /* Is that a big angle or a small one? */
                if (absdiffangle >= INT_ANGLE_45)  {
                    /* Yeah, a big angle.  Just turn on our heels. */
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, (delta << 8)) ;
                    else
                        ObjectAddAngle(p_obj, -((T_sword16)(delta << 8))) ;
                } else {
                    /* Not a big angle, we might as well go ahead */
                    /* and try moving a bit. */

                    /* Turn as needed. */
/*
                    turnangle = (delta<<6) ;
                    if (turnangle > absdiffangle)
                        turnangle = absdiffangle ;
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, turnangle) ;
                    else
                        ObjectAddAngle(p_obj, -turnangle) ;
*/
                    if (wasBlocked == FALSE)
                        ObjectSetAngle(p_obj, angle) ;

                    /* How close are we? */
                    distance = CalculateEstimateDistance(
                                   (T_sword16)(ObjectGetX16(p_obj)),
                                   (T_sword16)(ObjectGetY16(p_obj)),
                                   playerX,
                                   playerY) ;

    
    

                    /* Are we close enough to attack? */
                    if (distance < 70)  {
                        /* !!! Attack !!! */
                        ObjectSetStance(p_obj, STANCE_ATTACK) ;
                        ObjectSetAngle(p_obj, angle) ;

                        ObjectGetForwardPosition(
                            p_obj,
                            (T_sword16)(ObjectGetRadius(p_obj) + 20),
                            &frontX,
                            &frontY) ;

                        ServerDamageAt(
                            frontX,
                            frontY,
                            frontX, 
                            frontY, 
                            0, 
                            50) ;

                        /* Set the time for pausing on the attack. */
                        p_creature->state = 2 ;
                        p_creature->stateData = 27 ;
                    } else {
                        /* Too far, let's move closer. */
                        ObjectAccelFlat(p_obj, delta, ObjectGetAngle(p_obj)) ;
                        p_creature->stepping += delta ;

                        /* See if we are tired of moving and want */
                        /* a new opponent. */
                        if (p_creature->stateData <= newTime)  {
                            /* Look around for opponents. */
                            p_newTarget = CreatureLookForPlayer(p_obj) ;
                            if (p_newTarget != NULL)
                                p_creature->p_target = p_newTarget ;
                            p_creature->stateData = newTime+210 ;
                        }
                    }
                }
                break ;
            case 2:   /* Pause after attacking or getting hurt. */
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 10)
                    /* Yes, continue going after target. */
                    p_creature->state = 3 ;
                break ;
            case 3:
                ObjectSetStance(p_obj, STANCE_STAND) ;
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 0)  {
                    /* Yes, continue going after target. */
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;
                }
                break ;
        }
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  CreatureUpdateGriffon                                         */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreatureUpdateGriffon                                                 */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_creature *p_creature      -- creature to update                     */
/*                                                                          */
/*    T_word32 delta              -- Amount to move/update creature         */
/*                                                                          */
/*    T_word32 newTime            -- Current time to update                 */
/*                                                                          */
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
/*    LES  04/05/96  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void CreatureUpdateGriffon(
           T_3dObject *p_obj,
           T_word32 delta,
           T_word32 newTime)
{
    T_sword16 playerX, playerY, playerHeight ;
    T_word16 angle ;
    T_sword16 diffangle ;
    T_sword16 absdiffangle ;
    T_word16 distance ;
    T_3dObject *p_newTarget ;
    T_word16 turnangle ;
    T_creature *p_creature ;
    T_sword16 moveDelta ;
    T_sword32 frontX, frontY ;
    E_Boolean wasBlocked = FALSE ;
    T_sword16 oldZ ;
    T_sword16 newZ ;
    T_sword16 limit ;

    DebugRoutine("CreatureUpdateGriffon") ;
    DebugCheck(p_obj != NULL) ;

    ObjectSetMaxVelocity(p_obj, 24) ;
    ObjectClearMoveFlags(p_obj, OBJMOVE_FLAG_FAST_MOVEMENT) ;

    p_creature = (T_creature *)ObjectGetExtraData(p_obj) ;

    if (p_creature != NULL)  {

        /** Is it time for a noise? **/
        if (TickerGet () > p_creature->nextScream)
        {
           if (p_creature->nextScream == 0)
           {
              /** First time.  Don't sound off, just set the timer. **/
           }
           else
           {
              if (p_creature->state < 2)  {
                  /** Time for a holler. **/
                  ServerAreaSoundGlobal (
                        (T_sword16)(ObjectGetX16 (p_obj)),
                        (T_sword16)(ObjectGetY16 (p_obj)),
                        1000,
                        2111);
              }
           }
           /** Set the timer for the next scream. **/
           p_creature->nextScream = TickerGet() + 350 + (rand () % 350);
        }

        /* Make sure our movement doesn't keep pulling it down. */
        ObjectSetMoveFlags(
            p_obj,
            OBJMOVE_FLAG_IGNORE_GRAVITY |
            OBJMOVE_FLAG_IGNORE_FRICTION |
            OBJMOVE_FLAG_FAST_MOVEMENT) ;

        /* Check to see if we still have a target. */
        if (p_creature->p_target == NULL /* -1 */)
            /* Nope, go into the mode to get one. */
            p_creature->state = 0 ;

        switch(p_creature->state)  {
            case 0:   /* Initial state -- looking for a target. */
                ObjectSetStance(p_obj, STANCE_STAND) ;
                if ((p_creature->p_target =
                        CreatureLookForPlayer(p_obj)) != NULL)  {
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;

                    /* Creature sound that he sees you! */
                    ServerAreaSoundGlobal (
                           (T_sword16)(ObjectGetX16 (p_obj)),
                           (T_sword16)(ObjectGetY16 (p_obj)),
                           1000,
                           2110);
                }
                break ;
            case 1:   /* Moving towards opponent state. */
                /* Make us animate as walking. */
                ObjectSetStance(p_obj, STANCE_WALK) ;

                /* First, see if the creature was blocked last time. */
                if (ObjectWasBlocked(p_obj))  {
                    /* We are hitting something, let's turn randomly */
                    /* 45 degrees to one side. */
                    if (p_creature->needTurn == TRUE)  {
                        p_creature->turnWay = TickerGet() & 1 ;
                        p_creature->needTurn = FALSE ;
                    }

                    if (p_creature->turnWay)  {

                        ObjectAddAngle(p_obj, (delta<<8)) ;
                    } else {

                        ObjectAddAngle(p_obj, -((T_sword32)(delta<<8))) ;
                    }

                    p_creature->goForward = TRUE ;
                    p_creature->stepAway = 20 ;
                    wasBlocked = TRUE ;

                    /* Make sure future access say we were not blocked. */
                    ObjectClearBlockedFlag(p_obj) ;
                } else {
                    p_creature->needTurn = TRUE ;
                }

                /* Locate where the target is. */
                playerX = ObjectGetX16(p_creature->p_target) ;
                playerY = ObjectGetY16(p_creature->p_target) ;
/*
                ServerGetPlayerLocation(
                    p_creature->target,
                    &playerX,
                    &playerY) ;
*/

/*
                ServerGetPlayerHeight(
                    p_creature->target,

                    &playerHeight) ;
*/
                playerHeight = ObjectGetZ16(p_creature->p_target) ;

                /* Move creature up and down to match player height */
                oldZ = ObjectGetZ16(p_obj) ;
                if (oldZ > playerHeight)  {
                    moveDelta = oldZ - playerHeight ;
                    if (moveDelta > delta)
                        moveDelta = delta ;
                    moveDelta = -moveDelta ;
                } else {
                    moveDelta = playerHeight - oldZ ;
                    if (moveDelta > delta)
                        moveDelta = delta ;
                }

                /* Check to see that the creatures isn't going to */
                /* go through the roof. */
                newZ = oldZ + moveDelta ;
                limit = (ObjectGetHighestPoint(p_obj)>>16) -
                            ObjectGetHeight(p_obj) ;
                if (newZ > limit)
                    newZ = limit ;

                /* Make sure creature doesn't go through floor. */
                limit = (ObjectGetLowestPoint(p_obj)>>16) ;
                if (newZ < limit)
                    newZ = limit ;
                ObjectSetZ16(p_obj, newZ) ;

                /* Get the angle to this target. */
                angle = MathArcTangent(
                            (T_sword16)(playerX - ObjectGetX16(p_obj)),
                            (T_sword16)(playerY - ObjectGetY16(p_obj))) ;

                /* Face directly. */



                /* What is the difference to the angle I'm currently */
                /* facing? */
                diffangle = ObjectGetAngle(p_obj) - angle ;
                if (diffangle < 0)
                    absdiffangle = -diffangle ;
                else
                    absdiffangle = diffangle ;

                if (p_creature->goForward == TRUE)  {
                    absdiffangle = 0 ;
                    p_creature->stepAway -= delta ;
                    if (p_creature->stepAway < 0)
                        p_creature->goForward = FALSE ;
                }

                /* Is that a big angle or a small one? */
                if (absdiffangle >= INT_ANGLE_45)  {
                    /* Yeah, a big angle.  Just turn on our heels. */
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, (delta << 8)) ;
                    else
                        ObjectAddAngle(p_obj, -((T_sword16)(delta << 8))) ;
                } else {
                    /* Not a big angle, we might as well go ahead */
                    /* and try moving a bit. */

                    /* Turn as needed. */
/*
                    turnangle = (delta<<6) ;
                    if (turnangle > absdiffangle)
                        turnangle = absdiffangle ;
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, turnangle) ;
                    else
                        ObjectAddAngle(p_obj, -turnangle) ;
*/
                    if (wasBlocked == FALSE)
                        ObjectSetAngle(p_obj, angle) ;

                    /* How close are we? */
                    distance = CalculateEstimateDistance(
                                   (T_sword16)(ObjectGetX16(p_obj)),
                                   (T_sword16)(ObjectGetY16(p_obj)),
                                   playerX,
                                   playerY) ;

    
    

                    /* Are we close enough to attack? */
                    if (distance < 70)  {
                        /* !!! Attack !!! */
                        ObjectSetStance(p_obj, STANCE_ATTACK) ;
                        ObjectSetAngle(p_obj, angle) ;

                        ObjectGetForwardPosition(
                            p_obj,
                            (T_sword16)(ObjectGetRadius(p_obj) + 20),
                            &frontX,
                            &frontY) ;

                        ServerDamageAt(
                            frontX,
                            frontY,
                            frontX, 
                            frontY, 
                            0, 
                            50) ;

                        /* Set the time for pausing on the attack. */
                        p_creature->state = 2 ;
                        p_creature->stateData = 27 ;
                    } else {
                        /* Too far, let's move closer. */
                        ObjectAccelFlat(p_obj, delta, ObjectGetAngle(p_obj)) ;
                        p_creature->stepping += delta ;

                        /* See if we are tired of moving and want */
                        /* a new opponent. */
                        if (p_creature->stateData <= newTime)  {
                            /* Look around for opponents. */
                            p_newTarget = CreatureLookForPlayer(p_obj) ;
                            if (p_newTarget != NULL)
                                p_creature->p_target = p_newTarget ;
                            p_creature->stateData = newTime+210 ;
                        }
                    }
                }
                break ;
            case 2:   /* Pause after attacking or getting hurt. */
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 10)
                    /* Yes, continue going after target. */
                    p_creature->state = 3 ;
                break ;
            case 3:
                ObjectSetStance(p_obj, STANCE_STAND) ;
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 0)  {
                    /* Yes, continue going after target. */
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;
                }
                break ;
        }
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  CreatureUpdateBanshee                                         */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreatureUpdateBanshee is used to move a creature that flies.          */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_creature *p_creature      -- creature to update                     */
/*                                                                          */
/*    T_word32 delta              -- Amount to move/update creature         */
/*                                                                          */
/*    T_word32 newTime            -- Current time to update                 */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    MathArcTangent                                                        */
/*    CalculateDistance                                                     */
/*    ICreatureSendCreatureAttack                                           */
/*    View3dSetExceptObject                                                 */
/*    View3dMoveTo                                                          */
/*    CreatureLookForPlayer                                                 */
/*    View3dGetFloorAndCeiling                                              */
/*    MapGetFloorWalkingHeight                                              */
/*    ICreatureSendMoveCreaturePacket                                       */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  04/16/95  Created                                                */
/*    LES  09/15/95  Added code to make sure flyer doesn't go through       */
/*                   floor or ceiling.                                      */
/*                                                                          */
/****************************************************************************/

T_void CreatureUpdateBanshee(
           T_3dObject *p_obj,
           T_word32 delta,
           T_word32 newTime)
{
    T_sword16 playerX, playerY, playerHeight ;
    T_word16 angle ;
    T_sword16 diffangle ;
    T_sword16 absdiffangle ;
    T_word16 distance ;
    T_3dObject *p_newTarget ;
    T_word16 turnangle ;
    T_creature *p_creature ;
    T_sword16 moveDelta ;
    T_sword32 frontX, frontY ;
    E_Boolean wasBlocked = FALSE ;
    T_sword16 oldZ ;
    T_sword16 newZ ;
    T_sword16 limit ;

    DebugRoutine("CreatureUpdateBanshee") ;
    DebugCheck(p_obj != NULL) ;

    ObjectSetMaxVelocity(p_obj, 12) ;
    ObjectClearMoveFlags(p_obj, OBJMOVE_FLAG_FAST_MOVEMENT) ;

    p_creature = (T_creature *)ObjectGetExtraData(p_obj) ;

    if (p_creature != NULL)  {

        /** Is it time for a noise? **/
        if (TickerGet () > p_creature->nextScream)
        {
           if (p_creature->nextScream == 0)
           {
              /** First time.  Don't sound off, just set the timer. **/
           }
           else
           {
              if (p_creature->state < 2)  {
                  /** Time for a holler. **/
                  ServerAreaSoundGlobal (
                        (T_sword16)(ObjectGetX16 (p_obj)),
                        (T_sword16)(ObjectGetY16 (p_obj)),
                        1000,
                        4004);
              }
           }
           /** Set the timer for the next scream. **/
           p_creature->nextScream = TickerGet() + 350 + (rand () % 350);
        }

        /* Make sure our movement doesn't keep pulling it down. */
        ObjectSetMoveFlags(
            p_obj,
            OBJMOVE_FLAG_IGNORE_GRAVITY |
            OBJMOVE_FLAG_IGNORE_FRICTION |
            OBJMOVE_FLAG_FAST_MOVEMENT) ;

        /* Check to see if we still have a target. */
        if (p_creature->p_target == NULL /* -1 */)
            /* Nope, go into the mode to get one. */
            p_creature->state = 0 ;

        switch(p_creature->state)  {
            case 0:   /* Initial state -- looking for a target. */
                ObjectSetStance(p_obj, STANCE_STAND) ;
                if ((p_creature->p_target =
                        CreatureLookForPlayer(p_obj)) != NULL)  {
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;

                    /* Creature sound that he sees you! */
/*
                    ServerAreaSoundGlobal (
                           (T_sword16)(ObjectGetX16 (p_obj)),
                           (T_sword16)(ObjectGetY16 (p_obj)),
                           1000,
                           120);
*/
                }
                break ;
            case 1:   /* Moving towards opponent state. */
                /* Make us animate as walking. */
                ObjectSetStance(p_obj, STANCE_WALK) ;

                /* First, see if the creature was blocked last time. */
                if (ObjectWasBlocked(p_obj))  {
                    /* We are hitting something, let's turn randomly */
                    /* 45 degrees to one side. */
                    if (p_creature->needTurn == TRUE)  {
                        p_creature->turnWay = TickerGet() & 1 ;
                        p_creature->needTurn = FALSE ;
                    }

                    if (p_creature->turnWay)  {

                        ObjectAddAngle(p_obj, (delta<<8)) ;
                    } else {

                        ObjectAddAngle(p_obj, -((T_sword32)(delta<<8))) ;
                    }

                    p_creature->goForward = TRUE ;
                    p_creature->stepAway = 20 ;
                    wasBlocked = TRUE ;

                    /* Make sure future access say we were not blocked. */
                    ObjectClearBlockedFlag(p_obj) ;
                } else {
                    p_creature->needTurn = TRUE ;
                }

                /* Locate where the target is. */
                playerX = ObjectGetX16(p_creature->p_target) ;
                playerY = ObjectGetY16(p_creature->p_target) ;
/*
                ServerGetPlayerLocation(
                    p_creature->target,
                    &playerX,
                    &playerY) ;
*/

/*
                ServerGetPlayerHeight(
                    p_creature->target,

                    &playerHeight) ;
*/
                playerHeight = 25+ObjectGetZ16(p_creature->p_target) ;

                /* Move creature up and down to match player height */
                oldZ = ObjectGetZ16(p_obj) ;
                if (oldZ > playerHeight)  {
                    moveDelta = oldZ - playerHeight ;
                    if (moveDelta > delta)
                        moveDelta = delta ;
                    moveDelta = -moveDelta ;
                } else {
                    moveDelta = playerHeight - oldZ ;
                    if (moveDelta > delta)
                        moveDelta = delta ;
                }

                /* Check to see that the creatures isn't going to */
                /* go through the roof. */
                newZ = oldZ + moveDelta ;
                limit = (ObjectGetHighestPoint(p_obj)>>16) -
                            ObjectGetHeight(p_obj) ;
                if (newZ > limit)
                    newZ = limit ;

                /* Make sure creature doesn't go through floor. */
                limit = (ObjectGetLowestPoint(p_obj)>>16) ;
                if (newZ < limit)
                    newZ = limit ;
                ObjectSetZ16(p_obj, newZ) ;

                /* Get the angle to this target. */
                angle = MathArcTangent(
                            (T_sword16)(playerX - ObjectGetX16(p_obj)),
                            (T_sword16)(playerY - ObjectGetY16(p_obj))) ;

                /* Face directly. */



                /* What is the difference to the angle I'm currently */
                /* facing? */
                diffangle = ObjectGetAngle(p_obj) - angle ;
                if (diffangle < 0)
                    absdiffangle = -diffangle ;
                else
                    absdiffangle = diffangle ;

                if (p_creature->goForward == TRUE)  {
                    absdiffangle = 0 ;
                    p_creature->stepAway -= delta ;
                    if (p_creature->stepAway < 0)
                        p_creature->goForward = FALSE ;
                }

                /* Is that a big angle or a small one? */
                if (absdiffangle >= INT_ANGLE_45)  {
                    /* Yeah, a big angle.  Just turn on our heels. */
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, (delta << 8)) ;
                    else
                        ObjectAddAngle(p_obj, -((T_sword16)(delta << 8))) ;
                } else {
                    /* Not a big angle, we might as well go ahead */
                    /* and try moving a bit. */

                    /* Turn as needed. */
/*
                    turnangle = (delta<<6) ;
                    if (turnangle > absdiffangle)
                        turnangle = absdiffangle ;
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, turnangle) ;
                    else
                        ObjectAddAngle(p_obj, -turnangle) ;
*/
                    if (wasBlocked == FALSE)
                        ObjectSetAngle(p_obj, angle) ;

                    /* How close are we? */
                    distance = CalculateEstimateDistance(
                                   (T_sword16)(ObjectGetX16(p_obj)),
                                   (T_sword16)(ObjectGetY16(p_obj)),
                                   playerX,
                                   playerY) ;

    
    

                    /* Are we close enough to attack? */
                    if (distance < 60)  {
                        /* !!! Attack !!! */
                        ObjectSetStance(p_obj, STANCE_ATTACK) ;
                        ObjectSetAngle(p_obj, angle) ;

                        ObjectGetForwardPosition(
                            p_obj,
                            (T_sword16)(ObjectGetRadius(p_obj) + 30),
                            &frontX,
                            &frontY) ;

                        ServerDamageAt(
                            frontX,
                            frontY,
                            frontX, 
                            frontY, 
                            0, 
                            50) ;

                        /* Set the time for pausing on the attack. */
                        p_creature->state = 2 ;
                        p_creature->stateData = 27 ;
                    } else {
                        /* Too far, let's move closer. */
                        ObjectAccelFlat(p_obj, delta, ObjectGetAngle(p_obj)) ;
                        p_creature->stepping += delta ;

                        /* See if we are tired of moving and want */
                        /* a new opponent. */
                        if (p_creature->stateData <= newTime)  {
                            /* Look around for opponents. */
                            p_newTarget = CreatureLookForPlayer(p_obj) ;
                            if (p_newTarget != NULL)
                                p_creature->p_target = p_newTarget ;
                            p_creature->stateData = newTime+210 ;
                        }
                    }
                }
                break ;
            case 2:   /* Pause after attacking or getting hurt. */
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 10)
                    /* Yes, continue going after target. */
                    p_creature->state = 3 ;
                break ;
            case 3:
                ObjectSetStance(p_obj, STANCE_STAND) ;
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 0)  {
                    /* Yes, continue going after target. */
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;
                }
                break ;
        }
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  CreatureUpdateWalker                                          */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreatureUpdateWalker is used to move a creature that walks.           */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_creature *p_creature      -- creature to update                     */
/*                                                                          */
/*    T_word32 delta              -- Amount to move/update creature         */
/*                                                                          */
/*    T_word32 newTime            -- Current time to update                 */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    MathArcTangent                                                        */
/*    CalculateDistance                                                     */
/*    ICreatureSendCreatureAttack                                           */
/*    View3dSetExceptObject                                                 */
/*    View3dMoveTo                                                          */
/*    CreatureLookForPlayer                                                 */
/*    View3dGetFloorAndCeiling                                              */
/*    MapGetFloorWalkingHeight                                              */
/*    ICreatureSendMoveCreaturePacket                                       */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  04/16/95  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void CreatureUpdateWalker(
           T_3dObject *p_obj,
           T_word32 delta,
           T_word32 newTime)
{
    T_sword16 playerX, playerY ;
    T_word16 angle ;
    T_sword16 diffangle ;
    T_sword16 absdiffangle ;
    T_word16 distance ;
    T_3dObject *p_newTarget ;
    T_word16 turnangle ;
    T_creature *p_creature ;
    T_sword32 frontX, frontY ;

    DebugRoutine("CreatureUpdateWalker") ;
    DebugCheck(p_obj != NULL) ;

    ObjectSetMaxVelocity(p_obj, 8) ;

    p_creature = (T_creature *)ObjectGetExtraData(p_obj) ;

    if (p_creature != NULL)  {

        /** Is it time for a noise? **/
        if (TickerGet () > p_creature->nextScream)
        {
           if (p_creature->nextScream == 0)
           {
              /** First time.  Don't sound off, just set the timer. **/
           }
           else
           {
              /** Time for a holler. **/
              ServerAreaSoundGlobal (
                     (T_sword16)(ObjectGetX16 (p_obj)),
                     (T_sword16)(ObjectGetY16 (p_obj)),
                     1000,
                     4005);
           }
           /** Set the timer for the next scream. **/
           p_creature->nextScream = TickerGet() + 700 + (rand () % 700);
        }

        /* Check to see if we still have a target. */
        if (p_creature->p_target == NULL)
            /* Nope, go into the mode to get one. */
            p_creature->state = 0 ;

        switch(p_creature->state)  {
            case 0:   /* Initial state -- looking for a target. */
                ObjectSetStance(p_obj, STANCE_STAND) ;
                if ((p_creature->p_target =
                        CreatureLookForPlayer(p_obj)) != NULL)  {
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;

                    /* Creature sound that he sees you! */
                    ServerAreaSoundGlobal (
                           (T_sword16)(ObjectGetX16 (p_obj)),
                           (T_sword16)(ObjectGetY16 (p_obj)),
                           1000,
                           4001);
                }
                break ;
            case 1:   /* Moving towards opponent state. */
                /* Make us animate as walking. */
                ObjectSetStance(p_obj, STANCE_WALK) ;

                /* First, see if the creature was blocked last time. */
                if (ObjectWasBlocked(p_obj))  {
                    /* We are hitting something, let's turn randomly */
                    /* 45 degrees to one side. */
                    if (p_creature->goForward != TRUE)
                        p_creature->turnWay = TickerGet() & 1 ;

                    if (p_creature->turnWay)  {
                        ObjectAddAngle(p_obj, INT_ANGLE_45) ;
                    } else {
                        ObjectAddAngle(p_obj, -INT_ANGLE_45) ;
                    }

                    p_creature->goForward = TRUE ;
                    p_creature->stepAway = 70 ;

                    /* Make sure future access say we were not blocked. */
                    ObjectClearBlockedFlag(p_obj) ;
                }

                /* Locate where the target is. */
/*
                ServerGetPlayerLocation(
                    p_creature->target,
                    &playerX,
                    &playerY) ;
*/
                playerX = ObjectGetX16(p_creature->p_target) ;
                playerY = ObjectGetY16(p_creature->p_target) ;

                /* Get the angle to this target. */
                angle = MathArcTangent(
                            (T_sword16)(playerX - ObjectGetX16(p_obj)),
                            (T_sword16)(playerY - ObjectGetY16(p_obj))) ;

                /* What is the difference to the angle I'm currently */
                /* facing? */
                diffangle = ObjectGetAngle(p_obj) - angle ;
                if (diffangle < 0)
                    absdiffangle = -diffangle ;
                else
                    absdiffangle = diffangle ;

                if (p_creature->goForward == TRUE)  {
                    absdiffangle = 0 ;
                    p_creature->stepAway -= delta ;
                    if (p_creature->stepAway < 0)
                        p_creature->goForward = FALSE ;
                }

                /* Is that a big angle or a small one? */
                if (absdiffangle >= INT_ANGLE_45)  {
                    /* Yeah, a big angle.  Just turn on our heels. */
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, (delta << 8)) ;
                    else
                        ObjectAddAngle(p_obj, -((T_sword16)(delta << 8))) ;
                } else {
                    /* Not a big angle, we might as well go ahead */
                    /* and try moving a bit. */

                    /* Turn as needed. */
                    turnangle = (delta<<6) ;
                    if (turnangle > absdiffangle)
                        turnangle = absdiffangle ;
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, turnangle) ;
                    else
                        ObjectAddAngle(p_obj, -turnangle) ;

                    /* How close are we? */
                    distance = CalculateEstimateDistance(
                                   (T_sword16)(ObjectGetX16(p_obj)),
                                   (T_sword16)(ObjectGetY16(p_obj)),
                                   playerX,
                                   playerY) ;

    
    

                    /* Are we close enough to attack? */
                    if (distance < (ObjectGetRadius(p_obj) + 35))  {
                        /* !!! Attack !!! */
                        ObjectSetStance(p_obj, STANCE_ATTACK) ;
                        ObjectSetAngle(p_obj, angle) ;

                        ObjectGetForwardPosition(
                            p_obj,
                            (T_sword16)(ObjectGetRadius(p_obj) + 20),
                            &frontX,
                            &frontY) ;

                        ServerDamageAt(
                            frontX,
                            frontY,
                            frontX, 
                            frontY, 
                            0, 
                            100) ;

                        /* Set the time for pausing on the attack. */
                        p_creature->state = 2 ;
                        p_creature->stateData = 50 ;
                    } else {
                        /* Too far, let's move closer. */
                        ObjectAccelFlat(p_obj, delta, ObjectGetAngle(p_obj)) ;
                        p_creature->stepping += delta ;

                        /* See if we are tired of moving and want */
                        /* a new opponent. */
                        if (p_creature->stateData <= newTime)  {
                            /* Look around for opponents. */
                            p_newTarget = CreatureLookForPlayer(p_obj) ;
                            if (p_newTarget != NULL)
                                p_creature->p_target = p_newTarget ;
                            p_creature->stateData = newTime+210 ;
                        }
                    }
                }
                break ;
            case 2:   /* Pause after attacking or getting hurt. */
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 10)
                    /* Yes, continue going after target. */
                    p_creature->state = 3 ;
                break ;
            case 3:
                ObjectSetStance(p_obj, STANCE_STAND) ;
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 0)  {
                    /* Yes, continue going after target. */
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;
                }
                break ;
        }
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  CreatureUpdateDruid                                           */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreatureUpdateDruid                                                   */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_creature *p_creature      -- creature to update                     */
/*                                                                          */
/*    T_word32 delta              -- Amount to move/update creature         */
/*                                                                          */
/*    T_word32 newTime            -- Current time to update                 */
/*                                                                          */
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
/*    LES  04/06/96  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void CreatureUpdateDruid(
           T_3dObject *p_obj,
           T_word32 delta,
           T_word32 newTime)
{
    T_sword16 playerX, playerY ;
    T_word16 angle ;
    T_sword16 diffangle ;
    T_sword16 absdiffangle ;
    T_word16 distance ;
    T_3dObject *p_newTarget ;
    T_word16 turnangle ;
    T_creature *p_creature ;
    T_sword32 frontX, frontY ;

    DebugRoutine("CreatureUpdateDruid") ;
    DebugCheck(p_obj != NULL) ;

    ObjectSetMaxVelocity(p_obj, 8) ;

    p_creature = (T_creature *)ObjectGetExtraData(p_obj) ;

    if (p_creature != NULL)  {

        /** Is it time for a noise? **/
        if (TickerGet () > p_creature->nextScream)
        {
           if (p_creature->nextScream == 0)
           {
              /** First time.  Don't sound off, just set the timer. **/
           }
           else
           {
              /** Time for a holler. **/
/*
              ServerAreaSoundGlobal (
                     (T_sword16)(ObjectGetX16 (p_obj)),
                     (T_sword16)(ObjectGetY16 (p_obj)),
                     1000,
                     94);
*/
           }
           /** Set the timer for the next scream. **/
           p_creature->nextScream = TickerGet() + 700 + (rand () % 700);
        }

        /* Check to see if we still have a target. */
        if (p_creature->p_target == NULL)
            /* Nope, go into the mode to get one. */
            p_creature->state = 0 ;

        switch(p_creature->state)  {
            case 0:   /* Initial state -- looking for a target. */
                ObjectSetStance(p_obj, STANCE_STAND) ;
                if ((p_creature->p_target =
                        CreatureLookForPlayer(p_obj)) != NULL)  {
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;

                    /* Creature sound that he sees you! */
/*
                    ServerAreaSoundGlobal (
                           (T_sword16)(ObjectGetX16 (p_obj)),
                           (T_sword16)(ObjectGetY16 (p_obj)),
                           1000,
                           90);
*/
                }
                break ;
            case 1:   /* Moving towards opponent state. */
                /* Make us animate as walking. */
                ObjectSetStance(p_obj, STANCE_WALK) ;

                /* First, see if the creature was blocked last time. */
                if (ObjectWasBlocked(p_obj))  {
                    /* We are hitting something, let's turn randomly */
                    /* 45 degrees to one side. */
                    if (p_creature->goForward != TRUE)
                        p_creature->turnWay = TickerGet() & 1 ;

                    if (p_creature->turnWay)  {
                        ObjectAddAngle(p_obj, INT_ANGLE_45) ;
                    } else {
                        ObjectAddAngle(p_obj, -INT_ANGLE_45) ;
                    }

                    p_creature->goForward = TRUE ;
                    p_creature->stepAway = 70 ;

                    /* Make sure future access say we were not blocked. */
                    ObjectClearBlockedFlag(p_obj) ;
                }

                /* Locate where the target is. */
/*
                ServerGetPlayerLocation(
                    p_creature->target,
                    &playerX,
                    &playerY) ;
*/
                playerX = ObjectGetX16(p_creature->p_target) ;
                playerY = ObjectGetY16(p_creature->p_target) ;

                /* Get the angle to this target. */
                angle = MathArcTangent(
                            (T_sword16)(playerX - ObjectGetX16(p_obj)),
                            (T_sword16)(playerY - ObjectGetY16(p_obj))) ;

                /* What is the difference to the angle I'm currently */
                /* facing? */
                diffangle = ObjectGetAngle(p_obj) - angle ;
                if (diffangle < 0)
                    absdiffangle = -diffangle ;
                else
                    absdiffangle = diffangle ;

                if (p_creature->goForward == TRUE)  {
                    absdiffangle = 0 ;
                    p_creature->stepAway -= delta ;
                    if (p_creature->stepAway < 0)
                        p_creature->goForward = FALSE ;
                }

                /* Is that a big angle or a small one? */
                if (absdiffangle >= INT_ANGLE_45)  {
                    /* Yeah, a big angle.  Just turn on our heels. */
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, (delta << 8)) ;
                    else
                        ObjectAddAngle(p_obj, -((T_sword16)(delta << 8))) ;
                } else {
                    /* Not a big angle, we might as well go ahead */
                    /* and try moving a bit. */

                    /* Turn as needed. */
                    turnangle = (delta<<6) ;
                    if (turnangle > absdiffangle)
                        turnangle = absdiffangle ;
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, turnangle) ;
                    else
                        ObjectAddAngle(p_obj, -turnangle) ;

                    /* How close are we? */
                    distance = CalculateEstimateDistance(
                                   (T_sword16)(ObjectGetX16(p_obj)),
                                   (T_sword16)(ObjectGetY16(p_obj)),
                                   playerX,
                                   playerY) ;

    
    

                    /* Are we close enough to attack? */
                    if (distance < (ObjectGetRadius(p_obj) + 35))  {
                        /* !!! Attack !!! */
                        ObjectSetStance(p_obj, STANCE_ATTACK) ;
                        ObjectSetAngle(p_obj, angle) ;

                        ObjectGetForwardPosition(
                            p_obj,
                            (T_sword16)(ObjectGetRadius(p_obj) + 20),
                            &frontX,
                            &frontY) ;

                        ServerDamageAt(
                            frontX,
                            frontY,
                            frontX, 
                            frontY, 
                            0, 
                            100) ;

                        /* Set the time for pausing on the attack. */
                        p_creature->state = 2 ;
                        p_creature->stateData = 50 ;
                    } else {
                        /* Too far, let's move closer. */
                        ObjectAccelFlat(p_obj, delta, ObjectGetAngle(p_obj)) ;
                        p_creature->stepping += delta ;

                        /* See if we are tired of moving and want */
                        /* a new opponent. */
                        if (p_creature->stateData <= newTime)  {
                            /* Look around for opponents. */
                            p_newTarget = CreatureLookForPlayer(p_obj) ;
                            if (p_newTarget != NULL)
                                p_creature->p_target = p_newTarget ;
                            p_creature->stateData = newTime+210 ;
                        }
                    }
                }
                break ;
            case 2:   /* Pause after attacking or getting hurt. */
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 10)
                    /* Yes, continue going after target. */
                    p_creature->state = 3 ;
                break ;
            case 3:
                ObjectSetStance(p_obj, STANCE_STAND) ;
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 0)  {
                    /* Yes, continue going after target. */
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;
                }
                break ;
        }
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  CreatureUpdateElf                                             */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreatureUpdateElf                                                     */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_creature *p_creature      -- creature to update                     */
/*                                                                          */
/*    T_word32 delta              -- Amount to move/update creature         */
/*                                                                          */
/*    T_word32 newTime            -- Current time to update                 */
/*                                                                          */
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
/*    LES  04/06/96  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void CreatureUpdateElf(
           T_3dObject *p_obj,
           T_word32 delta,
           T_word32 newTime)
{
    T_sword16 playerX, playerY ;
    T_word16 angle ;
    T_sword16 diffangle ;
    T_sword16 absdiffangle ;
    T_word16 distance ;
    T_3dObject *p_newTarget ;
    T_word16 turnangle ;
    T_creature *p_creature ;
    T_sword32 frontX, frontY ;

    DebugRoutine("CreatureUpdateElf") ;
    DebugCheck(p_obj != NULL) ;

    ObjectSetMaxVelocity(p_obj, 8) ;

    p_creature = (T_creature *)ObjectGetExtraData(p_obj) ;

    if (p_creature != NULL)  {

        /** Is it time for a noise? **/
        if (TickerGet () > p_creature->nextScream)
        {
           if (p_creature->nextScream == 0)
           {
              /** First time.  Don't sound off, just set the timer. **/
           }
           else
           {
              /** Time for a holler. **/
/*
              ServerAreaSoundGlobal (
                     (T_sword16)(ObjectGetX16 (p_obj)),
                     (T_sword16)(ObjectGetY16 (p_obj)),
                     1000,
                     94);
*/
           }
           /** Set the timer for the next scream. **/
           p_creature->nextScream = TickerGet() + 700 + (rand () % 700);
        }

        /* Check to see if we still have a target. */
        if (p_creature->p_target == NULL)
            /* Nope, go into the mode to get one. */
            p_creature->state = 0 ;

        switch(p_creature->state)  {
            case 0:   /* Initial state -- looking for a target. */
                ObjectSetStance(p_obj, STANCE_STAND) ;
                if ((p_creature->p_target =
                        CreatureLookForPlayer(p_obj)) != NULL)  {
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;

                    /* Creature sound that he sees you! */
                    ServerAreaSoundGlobal (
                           (T_sword16)(ObjectGetX16 (p_obj)),
                           (T_sword16)(ObjectGetY16 (p_obj)),
                           1000,
                           5023);
                }
                break ;
            case 1:   /* Moving towards opponent state. */
                /* Make us animate as walking. */
                ObjectSetStance(p_obj, STANCE_WALK) ;

                /* First, see if the creature was blocked last time. */
                if (ObjectWasBlocked(p_obj))  {
                    /* We are hitting something, let's turn randomly */
                    /* 45 degrees to one side. */
                    if (p_creature->goForward != TRUE)
                        p_creature->turnWay = TickerGet() & 1 ;

                    if (p_creature->turnWay)  {
                        ObjectAddAngle(p_obj, INT_ANGLE_45) ;
                    } else {
                        ObjectAddAngle(p_obj, -INT_ANGLE_45) ;
                    }

                    p_creature->goForward = TRUE ;
                    p_creature->stepAway = 70 ;

                    /* Make sure future access say we were not blocked. */
                    ObjectClearBlockedFlag(p_obj) ;
                }

                /* Locate where the target is. */
/*
                ServerGetPlayerLocation(
                    p_creature->target,
                    &playerX,
                    &playerY) ;
*/
                playerX = ObjectGetX16(p_creature->p_target) ;
                playerY = ObjectGetY16(p_creature->p_target) ;

                /* Get the angle to this target. */
                angle = MathArcTangent(
                            (T_sword16)(playerX - ObjectGetX16(p_obj)),
                            (T_sword16)(playerY - ObjectGetY16(p_obj))) ;

                /* What is the difference to the angle I'm currently */
                /* facing? */
                diffangle = ObjectGetAngle(p_obj) - angle ;
                if (diffangle < 0)
                    absdiffangle = -diffangle ;
                else
                    absdiffangle = diffangle ;

                if (p_creature->goForward == TRUE)  {
                    absdiffangle = 0 ;
                    p_creature->stepAway -= delta ;
                    if (p_creature->stepAway < 0)
                        p_creature->goForward = FALSE ;
                }

                /* Is that a big angle or a small one? */
                if (absdiffangle >= INT_ANGLE_45)  {
                    /* Yeah, a big angle.  Just turn on our heels. */
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, (delta << 8)) ;
                    else
                        ObjectAddAngle(p_obj, -((T_sword16)(delta << 8))) ;
                } else {
                    /* Not a big angle, we might as well go ahead */
                    /* and try moving a bit. */

                    /* Turn as needed. */
                    turnangle = (delta<<6) ;
                    if (turnangle > absdiffangle)
                        turnangle = absdiffangle ;
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, turnangle) ;
                    else
                        ObjectAddAngle(p_obj, -turnangle) ;

                    /* How close are we? */
                    distance = CalculateEstimateDistance(
                                   (T_sword16)(ObjectGetX16(p_obj)),
                                   (T_sword16)(ObjectGetY16(p_obj)),
                                   playerX,
                                   playerY) ;

    
    

                    /* Are we close enough to attack? */
                    if (distance < (ObjectGetRadius(p_obj) + 35))  {
                        /* !!! Attack !!! */
                        ObjectSetStance(p_obj, STANCE_ATTACK) ;
                        ObjectSetAngle(p_obj, angle) ;

                        ObjectGetForwardPosition(
                            p_obj,
                            (T_sword16)(ObjectGetRadius(p_obj) + 20),
                            &frontX,
                            &frontY) ;

                        ServerDamageAt(
                            frontX,
                            frontY,
                            frontX, 
                            frontY, 
                            0, 
                            100) ;

                        /* Set the time for pausing on the attack. */
                        p_creature->state = 2 ;
                        p_creature->stateData = 50 ;
                    } else {
                        /* Too far, let's move closer. */
                        ObjectAccelFlat(p_obj, delta, ObjectGetAngle(p_obj)) ;
                        p_creature->stepping += delta ;

                        /* See if we are tired of moving and want */
                        /* a new opponent. */
                        if (p_creature->stateData <= newTime)  {
                            /* Look around for opponents. */
                            p_newTarget = CreatureLookForPlayer(p_obj) ;
                            if (p_newTarget != NULL)
                                p_creature->p_target = p_newTarget ;
                            p_creature->stateData = newTime+210 ;
                        }
                    }
                }
                break ;
            case 2:   /* Pause after attacking or getting hurt. */
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 10)
                    /* Yes, continue going after target. */
                    p_creature->state = 3 ;
                break ;
            case 3:
                ObjectSetStance(p_obj, STANCE_STAND) ;
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 0)  {
                    /* Yes, continue going after target. */
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;
                }
                break ;
        }
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  CreatureUpdateLich                                            */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreatureUpdateLich                                                    */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_creature *p_creature      -- creature to update                     */
/*                                                                          */
/*    T_word32 delta              -- Amount to move/update creature         */
/*                                                                          */
/*    T_word32 newTime            -- Current time to update                 */
/*                                                                          */
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
/*    LES  04/06/96  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void CreatureUpdateLich(
           T_3dObject *p_obj,
           T_word32 delta,
           T_word32 newTime)
{
    T_sword16 playerX, playerY ;
    T_word16 angle ;
    T_sword16 diffangle ;
    T_sword16 absdiffangle ;
    T_word16 distance ;
    T_3dObject *p_newTarget ;
    T_word16 turnangle ;
    T_creature *p_creature ;
    T_sword32 frontX, frontY ;

    DebugRoutine("CreatureUpdateLich") ;
    DebugCheck(p_obj != NULL) ;

    ObjectSetMaxVelocity(p_obj, 8) ;

    p_creature = (T_creature *)ObjectGetExtraData(p_obj) ;

    if (p_creature != NULL)  {

        /** Is it time for a noise? **/
        if (TickerGet () > p_creature->nextScream)
        {
           if (p_creature->nextScream == 0)
           {
              /** First time.  Don't sound off, just set the timer. **/
           }
           else
           {
              /** Time for a holler. **/
/*
              ServerAreaSoundGlobal (
                     (T_sword16)(ObjectGetX16 (p_obj)),
                     (T_sword16)(ObjectGetY16 (p_obj)),
                     1000,
                     94);
*/
           }
           /** Set the timer for the next scream. **/
           p_creature->nextScream = TickerGet() + 700 + (rand () % 700);
        }

        /* Check to see if we still have a target. */
        if (p_creature->p_target == NULL)
            /* Nope, go into the mode to get one. */
            p_creature->state = 0 ;

        switch(p_creature->state)  {
            case 0:   /* Initial state -- looking for a target. */
                ObjectSetStance(p_obj, STANCE_STAND) ;
                if ((p_creature->p_target =
                        CreatureLookForPlayer(p_obj)) != NULL)  {
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;

                    /* Creature sound that he sees you! */
                    ServerAreaSoundGlobal (
                           (T_sword16)(ObjectGetX16 (p_obj)),
                           (T_sword16)(ObjectGetY16 (p_obj)),
                           1000,
                           2009);
                }
                break ;
            case 1:   /* Moving towards opponent state. */
                /* Make us animate as walking. */
                ObjectSetStance(p_obj, STANCE_WALK) ;

                /* First, see if the creature was blocked last time. */
                if (ObjectWasBlocked(p_obj))  {
                    /* We are hitting something, let's turn randomly */
                    /* 45 degrees to one side. */
                    if (p_creature->goForward != TRUE)
                        p_creature->turnWay = TickerGet() & 1 ;

                    if (p_creature->turnWay)  {
                        ObjectAddAngle(p_obj, INT_ANGLE_45) ;
                    } else {
                        ObjectAddAngle(p_obj, -INT_ANGLE_45) ;
                    }

                    p_creature->goForward = TRUE ;
                    p_creature->stepAway = 70 ;

                    /* Make sure future access say we were not blocked. */
                    ObjectClearBlockedFlag(p_obj) ;
                }

                /* Locate where the target is. */
/*
                ServerGetPlayerLocation(
                    p_creature->target,
                    &playerX,
                    &playerY) ;
*/
                playerX = ObjectGetX16(p_creature->p_target) ;
                playerY = ObjectGetY16(p_creature->p_target) ;

                /* Get the angle to this target. */
                angle = MathArcTangent(
                            (T_sword16)(playerX - ObjectGetX16(p_obj)),
                            (T_sword16)(playerY - ObjectGetY16(p_obj))) ;

                /* What is the difference to the angle I'm currently */
                /* facing? */
                diffangle = ObjectGetAngle(p_obj) - angle ;
                if (diffangle < 0)
                    absdiffangle = -diffangle ;
                else
                    absdiffangle = diffangle ;

                if (p_creature->goForward == TRUE)  {
                    absdiffangle = 0 ;
                    p_creature->stepAway -= delta ;
                    if (p_creature->stepAway < 0)
                        p_creature->goForward = FALSE ;
                }

                /* Is that a big angle or a small one? */
                if (absdiffangle >= INT_ANGLE_45)  {
                    /* Yeah, a big angle.  Just turn on our heels. */
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, (delta << 8)) ;
                    else
                        ObjectAddAngle(p_obj, -((T_sword16)(delta << 8))) ;
                } else {
                    /* Not a big angle, we might as well go ahead */
                    /* and try moving a bit. */

                    /* Turn as needed. */
                    turnangle = (delta<<6) ;
                    if (turnangle > absdiffangle)
                        turnangle = absdiffangle ;
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, turnangle) ;
                    else
                        ObjectAddAngle(p_obj, -turnangle) ;

                    /* How close are we? */
                    distance = CalculateEstimateDistance(
                                   (T_sword16)(ObjectGetX16(p_obj)),
                                   (T_sword16)(ObjectGetY16(p_obj)),
                                   playerX,
                                   playerY) ;

    
    

                    /* Are we close enough to attack? */
                    if (distance < (ObjectGetRadius(p_obj) + 35))  {
                        /* !!! Attack !!! */
                        ObjectSetStance(p_obj, STANCE_ATTACK) ;
                        ObjectSetAngle(p_obj, angle) ;

                        ObjectGetForwardPosition(
                            p_obj,
                            (T_sword16)(ObjectGetRadius(p_obj) + 20),
                            &frontX,
                            &frontY) ;

                        ServerDamageAt(
                            frontX,
                            frontY,
                            frontX, 
                            frontY, 
                            0, 
                            100) ;

                        /* Set the time for pausing on the attack. */
                        p_creature->state = 2 ;
                        p_creature->stateData = 50 ;
                    } else {
                        /* Too far, let's move closer. */
                        ObjectAccelFlat(p_obj, delta, ObjectGetAngle(p_obj)) ;
                        p_creature->stepping += delta ;

                        /* See if we are tired of moving and want */
                        /* a new opponent. */
                        if (p_creature->stateData <= newTime)  {
                            /* Look around for opponents. */
                            p_newTarget = CreatureLookForPlayer(p_obj) ;
                            if (p_newTarget != NULL)
                                p_creature->p_target = p_newTarget ;
                            p_creature->stateData = newTime+210 ;
                        }
                    }
                }
                break ;
            case 2:   /* Pause after attacking or getting hurt. */
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 10)
                    /* Yes, continue going after target. */
                    p_creature->state = 3 ;
                break ;
            case 3:
                ObjectSetStance(p_obj, STANCE_STAND) ;
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 0)  {
                    /* Yes, continue going after target. */
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;
                }
                break ;
        }
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  CreatureUpdateBronzeBarbarian                                 */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreatureUpdateBronzeBarbarian                                         */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_creature *p_creature      -- creature to update                     */
/*                                                                          */
/*    T_word32 delta              -- Amount to move/update creature         */
/*                                                                          */
/*    T_word32 newTime            -- Current time to update                 */
/*                                                                          */
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
/*    LES  04/04/96  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void CreatureUpdateBronzeBarbarian(
           T_3dObject *p_obj,
           T_word32 delta,
           T_word32 newTime)
{
    T_sword16 playerX, playerY ;
    T_word16 angle ;
    T_sword16 diffangle ;
    T_sword16 absdiffangle ;
    T_word16 distance ;
    T_3dObject *p_newTarget ;
    T_word16 turnangle ;
    T_creature *p_creature ;
    T_sword32 frontX, frontY ;

    DebugRoutine("CreatureUpdateBronzeBarbarian") ;
    DebugCheck(p_obj != NULL) ;

    ObjectSetMaxVelocity(p_obj, 8) ;

    p_creature = (T_creature *)ObjectGetExtraData(p_obj) ;

    if (p_creature != NULL)  {

        /** Is it time for a noise? **/
        if (TickerGet () > p_creature->nextScream)
        {
           if (p_creature->nextScream == 0)
           {
              /** First time.  Don't sound off, just set the timer. **/
           }
           else
           {
              /** Time for a holler. **/
/*
              ServerAreaSoundGlobal (
                     (T_sword16)(ObjectGetX16 (p_obj)),
                     (T_sword16)(ObjectGetY16 (p_obj)),
                     1000,
                     94);
*/
           }
           /** Set the timer for the next scream. **/
           p_creature->nextScream = TickerGet() + 700 + (rand () % 700);
        }

        /* Check to see if we still have a target. */
        if (p_creature->p_target == NULL)
            /* Nope, go into the mode to get one. */
            p_creature->state = 0 ;

        switch(p_creature->state)  {
            case 0:   /* Initial state -- looking for a target. */
                ObjectSetStance(p_obj, STANCE_STAND) ;
                if ((p_creature->p_target =
                        CreatureLookForPlayer(p_obj)) != NULL)  {
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;

                    /* Creature sound that he sees you! */
/*
                    ServerAreaSoundGlobal (
                           (T_sword16)(ObjectGetX16 (p_obj)),
                           (T_sword16)(ObjectGetY16 (p_obj)),
                           1000,
                           90);
*/
                }
                break ;
            case 1:   /* Moving towards opponent state. */
                /* Make us animate as walking. */
                ObjectSetStance(p_obj, STANCE_WALK) ;

                /* First, see if the creature was blocked last time. */
                if (ObjectWasBlocked(p_obj))  {
                    /* We are hitting something, let's turn randomly */
                    /* 45 degrees to one side. */
                    if (p_creature->goForward != TRUE)
                        p_creature->turnWay = TickerGet() & 1 ;

                    if (p_creature->turnWay)  {
                        ObjectAddAngle(p_obj, INT_ANGLE_45) ;
                    } else {
                        ObjectAddAngle(p_obj, -INT_ANGLE_45) ;
                    }

                    p_creature->goForward = TRUE ;
                    p_creature->stepAway = 70 ;

                    /* Make sure future access say we were not blocked. */
                    ObjectClearBlockedFlag(p_obj) ;
                }

                /* Locate where the target is. */
/*
                ServerGetPlayerLocation(
                    p_creature->target,
                    &playerX,
                    &playerY) ;
*/
                playerX = ObjectGetX16(p_creature->p_target) ;
                playerY = ObjectGetY16(p_creature->p_target) ;

                /* Get the angle to this target. */
                angle = MathArcTangent(
                            (T_sword16)(playerX - ObjectGetX16(p_obj)),
                            (T_sword16)(playerY - ObjectGetY16(p_obj))) ;

                /* What is the difference to the angle I'm currently */
                /* facing? */
                diffangle = ObjectGetAngle(p_obj) - angle ;
                if (diffangle < 0)
                    absdiffangle = -diffangle ;
                else
                    absdiffangle = diffangle ;

                if (p_creature->goForward == TRUE)  {
                    absdiffangle = 0 ;
                    p_creature->stepAway -= delta ;
                    if (p_creature->stepAway < 0)
                        p_creature->goForward = FALSE ;
                }

                /* Is that a big angle or a small one? */
                if (absdiffangle >= INT_ANGLE_45)  {
                    /* Yeah, a big angle.  Just turn on our heels. */
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, (delta << 8)) ;
                    else
                        ObjectAddAngle(p_obj, -((T_sword16)(delta << 8))) ;
                } else {
                    /* Not a big angle, we might as well go ahead */
                    /* and try moving a bit. */

                    /* Turn as needed. */
                    turnangle = (delta<<6) ;
                    if (turnangle > absdiffangle)
                        turnangle = absdiffangle ;
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, turnangle) ;
                    else
                        ObjectAddAngle(p_obj, -turnangle) ;

                    /* How close are we? */
                    distance = CalculateEstimateDistance(
                                   (T_sword16)(ObjectGetX16(p_obj)),
                                   (T_sword16)(ObjectGetY16(p_obj)),
                                   playerX,
                                   playerY) ;

    
    

                    /* Are we close enough to attack? */
                    if (distance < (ObjectGetRadius(p_obj) + 35))  {
                        /* !!! Attack !!! */
                        ObjectSetStance(p_obj, STANCE_ATTACK) ;
                        ObjectSetAngle(p_obj, angle) ;

                        ObjectGetForwardPosition(
                            p_obj,
                            (T_sword16)(ObjectGetRadius(p_obj) + 20),
                            &frontX,
                            &frontY) ;

                        ServerDamageAt(
                            frontX,
                            frontY,
                            frontX, 
                            frontY, 
                            0, 
                            100) ;

                        /* Set the time for pausing on the attack. */
                        p_creature->state = 2 ;
                        p_creature->stateData = 50 ;
                    } else {
                        /* Too far, let's move closer. */
                        ObjectAccelFlat(p_obj, delta, ObjectGetAngle(p_obj)) ;
                        p_creature->stepping += delta ;

                        /* See if we are tired of moving and want */
                        /* a new opponent. */
                        if (p_creature->stateData <= newTime)  {
                            /* Look around for opponents. */
                            p_newTarget = CreatureLookForPlayer(p_obj) ;
                            if (p_newTarget != NULL)
                                p_creature->p_target = p_newTarget ;
                            p_creature->stateData = newTime+210 ;
                        }
                    }
                }
                break ;
            case 2:   /* Pause after attacking or getting hurt. */
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 10)
                    /* Yes, continue going after target. */
                    p_creature->state = 3 ;
                break ;
            case 3:
                ObjectSetStance(p_obj, STANCE_STAND) ;
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 0)  {
                    /* Yes, continue going after target. */
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;
                }
                break ;
        }
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  CreatureUpdateBlackBarbarian                                  */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreatureUpdateBlack Barbarian                                         */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_creature *p_creature      -- creature to update                     */
/*                                                                          */
/*    T_word32 delta              -- Amount to move/update creature         */
/*                                                                          */
/*    T_word32 newTime            -- Current time to update                 */
/*                                                                          */
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
/*    LES  04/04/96  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void CreatureUpdateBlackBarbarian(
           T_3dObject *p_obj,
           T_word32 delta,
           T_word32 newTime)
{
    T_sword16 playerX, playerY ;
    T_word16 angle ;
    T_sword16 diffangle ;
    T_sword16 absdiffangle ;
    T_word16 distance ;
    T_3dObject *p_newTarget ;
    T_word16 turnangle ;
    T_creature *p_creature ;
    T_sword32 frontX, frontY ;

    DebugRoutine("CreatureUpdateBlackBarbarian") ;
    DebugCheck(p_obj != NULL) ;

    ObjectSetMaxVelocity(p_obj, 8) ;

    p_creature = (T_creature *)ObjectGetExtraData(p_obj) ;

    if (p_creature != NULL)  {

        /** Is it time for a noise? **/
        if (TickerGet () > p_creature->nextScream)
        {
           if (p_creature->nextScream == 0)
           {
              /** First time.  Don't sound off, just set the timer. **/
           }
           else
           {
              /** Time for a holler. **/
              ServerAreaSoundGlobal (
                     (T_sword16)(ObjectGetX16 (p_obj)),
                     (T_sword16)(ObjectGetY16 (p_obj)),
                     1000,
                     5001);
           }
           /** Set the timer for the next scream. **/
           p_creature->nextScream = TickerGet() + 700 + (rand () % 700);
        }

        /* Check to see if we still have a target. */
        if (p_creature->p_target == NULL)
            /* Nope, go into the mode to get one. */
            p_creature->state = 0 ;

        switch(p_creature->state)  {
            case 0:   /* Initial state -- looking for a target. */
                ObjectSetStance(p_obj, STANCE_STAND) ;
                if ((p_creature->p_target =
                        CreatureLookForPlayer(p_obj)) != NULL)  {
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;

                    /* Creature sound that he sees you! */
                    ServerAreaSoundGlobal (
                           (T_sword16)(ObjectGetX16 (p_obj)),
                           (T_sword16)(ObjectGetY16 (p_obj)),
                           1000,
                           5007);
                }
                break ;
            case 1:   /* Moving towards opponent state. */
                /* Make us animate as walking. */
                ObjectSetStance(p_obj, STANCE_WALK) ;

                /* First, see if the creature was blocked last time. */
                if (ObjectWasBlocked(p_obj))  {
                    /* We are hitting something, let's turn randomly */
                    /* 45 degrees to one side. */
                    if (p_creature->goForward != TRUE)
                        p_creature->turnWay = TickerGet() & 1 ;

                    if (p_creature->turnWay)  {
                        ObjectAddAngle(p_obj, INT_ANGLE_45) ;
                    } else {
                        ObjectAddAngle(p_obj, -INT_ANGLE_45) ;
                    }

                    p_creature->goForward = TRUE ;
                    p_creature->stepAway = 70 ;

                    /* Make sure future access say we were not blocked. */
                    ObjectClearBlockedFlag(p_obj) ;
                }

                /* Locate where the target is. */
/*
                ServerGetPlayerLocation(
                    p_creature->target,
                    &playerX,
                    &playerY) ;
*/
                playerX = ObjectGetX16(p_creature->p_target) ;
                playerY = ObjectGetY16(p_creature->p_target) ;

                /* Get the angle to this target. */
                angle = MathArcTangent(
                            (T_sword16)(playerX - ObjectGetX16(p_obj)),
                            (T_sword16)(playerY - ObjectGetY16(p_obj))) ;

                /* What is the difference to the angle I'm currently */
                /* facing? */
                diffangle = ObjectGetAngle(p_obj) - angle ;
                if (diffangle < 0)
                    absdiffangle = -diffangle ;
                else
                    absdiffangle = diffangle ;

                if (p_creature->goForward == TRUE)  {
                    absdiffangle = 0 ;
                    p_creature->stepAway -= delta ;
                    if (p_creature->stepAway < 0)
                        p_creature->goForward = FALSE ;
                }

                /* Is that a big angle or a small one? */
                if (absdiffangle >= INT_ANGLE_45)  {
                    /* Yeah, a big angle.  Just turn on our heels. */
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, (delta << 8)) ;
                    else
                        ObjectAddAngle(p_obj, -((T_sword16)(delta << 8))) ;
                } else {
                    /* Not a big angle, we might as well go ahead */
                    /* and try moving a bit. */

                    /* Turn as needed. */
                    turnangle = (delta<<6) ;
                    if (turnangle > absdiffangle)
                        turnangle = absdiffangle ;
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, turnangle) ;
                    else
                        ObjectAddAngle(p_obj, -turnangle) ;

                    /* How close are we? */
                    distance = CalculateEstimateDistance(
                                   (T_sword16)(ObjectGetX16(p_obj)),
                                   (T_sword16)(ObjectGetY16(p_obj)),
                                   playerX,
                                   playerY) ;

    
    

                    /* Are we close enough to attack? */
                    if (distance < (ObjectGetRadius(p_obj) + 35))  {
                        /* !!! Attack !!! */
                        ObjectSetStance(p_obj, STANCE_ATTACK) ;
                        ObjectSetAngle(p_obj, angle) ;

                        ObjectGetForwardPosition(
                            p_obj,
                            (T_sword16)(ObjectGetRadius(p_obj) + 20),
                            &frontX,
                            &frontY) ;

                        ServerDamageAt(
                            frontX,
                            frontY,
                            frontX, 
                            frontY, 
                            0, 
                            100) ;

                        /* Set the time for pausing on the attack. */
                        p_creature->state = 2 ;
                        p_creature->stateData = 50 ;
                    } else {
                        /* Too far, let's move closer. */
                        ObjectAccelFlat(p_obj, delta, ObjectGetAngle(p_obj)) ;
                        p_creature->stepping += delta ;

                        /* See if we are tired of moving and want */
                        /* a new opponent. */
                        if (p_creature->stateData <= newTime)  {
                            /* Look around for opponents. */
                            p_newTarget = CreatureLookForPlayer(p_obj) ;
                            if (p_newTarget != NULL)
                                p_creature->p_target = p_newTarget ;
                            p_creature->stateData = newTime+210 ;
                        }
                    }
                }
                break ;
            case 2:   /* Pause after attacking or getting hurt. */
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 10)
                    /* Yes, continue going after target. */
                    p_creature->state = 3 ;
                break ;
            case 3:
                ObjectSetStance(p_obj, STANCE_STAND) ;
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 0)  {
                    /* Yes, continue going after target. */
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;
                }
                break ;
        }
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  CreatureUpdateWhiteBarbarian                                  */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreatureUpdateWhite Barbarian                                         */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_creature *p_creature      -- creature to update                     */
/*                                                                          */
/*    T_word32 delta              -- Amount to move/update creature         */
/*                                                                          */
/*    T_word32 newTime            -- Current time to update                 */
/*                                                                          */
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
/*    LES  04/04/96  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void CreatureUpdateWhiteBarbarian(
           T_3dObject *p_obj,
           T_word32 delta,
           T_word32 newTime)
{
    T_sword16 playerX, playerY ;
    T_word16 angle ;
    T_sword16 diffangle ;
    T_sword16 absdiffangle ;
    T_word16 distance ;
    T_3dObject *p_newTarget ;
    T_word16 turnangle ;
    T_creature *p_creature ;
    T_sword32 frontX, frontY ;

    DebugRoutine("CreatureUpdateWhiteBarbarian") ;
    DebugCheck(p_obj != NULL) ;

    ObjectSetMaxVelocity(p_obj, 8) ;

    p_creature = (T_creature *)ObjectGetExtraData(p_obj) ;

    if (p_creature != NULL)  {

        /** Is it time for a noise? **/
        if (TickerGet () > p_creature->nextScream)
        {
           if (p_creature->nextScream == 0)
           {
              /** First time.  Don't sound off, just set the timer. **/
           }
           else
           {
              /** Time for a holler. **/
              ServerAreaSoundGlobal (
                     (T_sword16)(ObjectGetX16 (p_obj)),
                     (T_sword16)(ObjectGetY16 (p_obj)),
                     1000,
                     5012);
           }
           /** Set the timer for the next scream. **/
           p_creature->nextScream = TickerGet() + 700 + (rand () % 700);
        }

        /* Check to see if we still have a target. */
        if (p_creature->p_target == NULL)
            /* Nope, go into the mode to get one. */
            p_creature->state = 0 ;

        switch(p_creature->state)  {
            case 0:   /* Initial state -- looking for a target. */
                ObjectSetStance(p_obj, STANCE_STAND) ;
                if ((p_creature->p_target =
                        CreatureLookForPlayer(p_obj)) != NULL)  {
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;

                    /* Creature sound that he sees you! */
                    ServerAreaSoundGlobal (
                           (T_sword16)(ObjectGetX16 (p_obj)),
                           (T_sword16)(ObjectGetY16 (p_obj)),
                           1000,
                           5008);
                }
                break ;
            case 1:   /* Moving towards opponent state. */
                /* Make us animate as walking. */
                ObjectSetStance(p_obj, STANCE_WALK) ;

                /* First, see if the creature was blocked last time. */
                if (ObjectWasBlocked(p_obj))  {
                    /* We are hitting something, let's turn randomly */
                    /* 45 degrees to one side. */
                    if (p_creature->goForward != TRUE)
                        p_creature->turnWay = TickerGet() & 1 ;

                    if (p_creature->turnWay)  {
                        ObjectAddAngle(p_obj, INT_ANGLE_45) ;
                    } else {
                        ObjectAddAngle(p_obj, -INT_ANGLE_45) ;
                    }

                    p_creature->goForward = TRUE ;
                    p_creature->stepAway = 70 ;

                    /* Make sure future access say we were not blocked. */
                    ObjectClearBlockedFlag(p_obj) ;
                }

                /* Locate where the target is. */
/*
                ServerGetPlayerLocation(
                    p_creature->target,
                    &playerX,
                    &playerY) ;
*/
                playerX = ObjectGetX16(p_creature->p_target) ;
                playerY = ObjectGetY16(p_creature->p_target) ;

                /* Get the angle to this target. */
                angle = MathArcTangent(
                            (T_sword16)(playerX - ObjectGetX16(p_obj)),
                            (T_sword16)(playerY - ObjectGetY16(p_obj))) ;

                /* What is the difference to the angle I'm currently */
                /* facing? */
                diffangle = ObjectGetAngle(p_obj) - angle ;
                if (diffangle < 0)
                    absdiffangle = -diffangle ;
                else
                    absdiffangle = diffangle ;

                if (p_creature->goForward == TRUE)  {
                    absdiffangle = 0 ;
                    p_creature->stepAway -= delta ;
                    if (p_creature->stepAway < 0)
                        p_creature->goForward = FALSE ;
                }

                /* Is that a big angle or a small one? */
                if (absdiffangle >= INT_ANGLE_45)  {
                    /* Yeah, a big angle.  Just turn on our heels. */
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, (delta << 8)) ;
                    else
                        ObjectAddAngle(p_obj, -((T_sword16)(delta << 8))) ;
                } else {
                    /* Not a big angle, we might as well go ahead */
                    /* and try moving a bit. */

                    /* Turn as needed. */
                    turnangle = (delta<<6) ;
                    if (turnangle > absdiffangle)
                        turnangle = absdiffangle ;
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, turnangle) ;
                    else
                        ObjectAddAngle(p_obj, -turnangle) ;

                    /* How close are we? */
                    distance = CalculateEstimateDistance(
                                   (T_sword16)(ObjectGetX16(p_obj)),
                                   (T_sword16)(ObjectGetY16(p_obj)),
                                   playerX,
                                   playerY) ;

    
    

                    /* Are we close enough to attack? */
                    if (distance < (ObjectGetRadius(p_obj) + 35))  {
                        /* !!! Attack !!! */
                        ObjectSetStance(p_obj, STANCE_ATTACK) ;
                        ObjectSetAngle(p_obj, angle) ;

                        ObjectGetForwardPosition(
                            p_obj,
                            (T_sword16)(ObjectGetRadius(p_obj) + 20),
                            &frontX,
                            &frontY) ;

                        ServerDamageAt(
                            frontX,
                            frontY,
                            frontX, 
                            frontY, 
                            0, 
                            100) ;

                        /* Set the time for pausing on the attack. */
                        p_creature->state = 2 ;
                        p_creature->stateData = 50 ;
                    } else {
                        /* Too far, let's move closer. */
                        ObjectAccelFlat(p_obj, delta, ObjectGetAngle(p_obj)) ;
                        p_creature->stepping += delta ;

                        /* See if we are tired of moving and want */
                        /* a new opponent. */
                        if (p_creature->stateData <= newTime)  {
                            /* Look around for opponents. */
                            p_newTarget = CreatureLookForPlayer(p_obj) ;
                            if (p_newTarget != NULL)
                                p_creature->p_target = p_newTarget ;
                            p_creature->stateData = newTime+210 ;
                        }
                    }
                }
                break ;
            case 2:   /* Pause after attacking or getting hurt. */
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 10)
                    /* Yes, continue going after target. */
                    p_creature->state = 3 ;
                break ;
            case 3:
                ObjectSetStance(p_obj, STANCE_STAND) ;
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 0)  {
                    /* Yes, continue going after target. */
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;
                }
                break ;
        }
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  CreatureUpdateHydra                                           */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreatureUpdateWalker is used to move a creature that walks.           */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_creature *p_creature      -- creature to update                     */
/*                                                                          */
/*    T_word32 delta              -- Amount to move/update creature         */
/*                                                                          */
/*    T_word32 newTime            -- Current time to update                 */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    MathArcTangent                                                        */
/*    CalculateDistance                                                     */
/*    ICreatureSendCreatureAttack                                           */
/*    View3dSetExceptObject                                                 */
/*    View3dMoveTo                                                          */
/*    CreatureLookForPlayer                                                 */
/*    View3dGetFloorAndCeiling                                              */
/*    MapGetFloorWalkingHeight                                              */
/*    ICreatureSendMoveCreaturePacket                                       */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  04/16/95  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void CreatureUpdateHydra(
           T_3dObject *p_obj,
           T_word32 delta,
           T_word32 newTime)
{
    T_sword16 playerX, playerY ;
    T_word16 angle ;
    T_sword16 diffangle ;
    T_sword16 absdiffangle ;
    T_word16 distance ;
    T_3dObject *p_newTarget ;
    T_word16 turnangle ;
    T_creature *p_creature ;
    T_sword32 frontX, frontY ;

    DebugRoutine("CreatureUpdateHydra") ;
    DebugCheck(p_obj != NULL) ;

    ObjectSetMaxVelocity(p_obj, 8) ;

    p_creature = (T_creature *)ObjectGetExtraData(p_obj) ;

/* The dragon is invincible. */
ObjectSetHealth(p_obj, 30000) ;
    if (p_creature != NULL)  {

        /** Is it time for a noise? **/
        if (TickerGet () > p_creature->nextScream)
        {
           if (p_creature->nextScream == 0)
           {
              /** First time.  Don't sound off, just set the timer. **/
           }
           else
           {
              /** Time for a holler. **/
              ServerAreaSoundGlobal (
                     (T_sword16)(ObjectGetX16 (p_obj)),
                     (T_sword16)(ObjectGetY16 (p_obj)),
                     1000,
                     4030);
           }
           /** Set the timer for the next scream. **/
           p_creature->nextScream = TickerGet() + 700 + (rand () % 700);
        }

        /* Check to see if we still have a target. */
        if (p_creature->p_target == NULL)
            /* Nope, go into the mode to get one. */
            p_creature->state = 0 ;

        switch(p_creature->state)  {
            case 0:   /* Initial state -- looking for a target. */
                ObjectSetStance(p_obj, STANCE_STAND) ;
                if ((p_creature->p_target =
                        CreatureLookForPlayer(p_obj)) != NULL)  {
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;

                    /* Creature sound that he sees you! */
                    ServerAreaSoundGlobal (
                           (T_sword16)(ObjectGetX16 (p_obj)),
                           (T_sword16)(ObjectGetY16 (p_obj)),
                           1000,
                           2116);
                }
                break ;
            case 1:   /* Moving towards opponent state. */
                /* Make us animate as walking. */
                ObjectSetStance(p_obj, STANCE_WALK) ;

                /* First, see if the creature was blocked last time. */
                if (ObjectWasBlocked(p_obj))  {
                    /* We are hitting something, let's turn randomly */
                    /* 45 degrees to one side. */
                    if (p_creature->goForward != TRUE)
                        p_creature->turnWay = TickerGet() & 1 ;

                    if (p_creature->turnWay)  {
                        ObjectAddAngle(p_obj, INT_ANGLE_45) ;
                    } else {
                        ObjectAddAngle(p_obj, -INT_ANGLE_45) ;
                    }

                    p_creature->goForward = TRUE ;
                    p_creature->stepAway = 70 ;

                    /* Make sure future access say we were not blocked. */
                    ObjectClearBlockedFlag(p_obj) ;
                }

                /* Locate where the target is. */
/*
                ServerGetPlayerLocation(
                    p_creature->target,
                    &playerX,
                    &playerY) ;
*/
                playerX = ObjectGetX16(p_creature->p_target) ;
                playerY = ObjectGetY16(p_creature->p_target) ;

                /* Get the angle to this target. */
                angle = MathArcTangent(
                            (T_sword16)(playerX - ObjectGetX16(p_obj)),
                            (T_sword16)(playerY - ObjectGetY16(p_obj))) ;

                /* What is the difference to the angle I'm currently */
                /* facing? */
                diffangle = ObjectGetAngle(p_obj) - angle ;
                if (diffangle < 0)
                    absdiffangle = -diffangle ;
                else
                    absdiffangle = diffangle ;

                if (p_creature->goForward == TRUE)  {
                    absdiffangle = 0 ;
                    p_creature->stepAway -= delta ;
                    if (p_creature->stepAway < 0)
                        p_creature->goForward = FALSE ;
                }

                /* Is that a big angle or a small one? */
                if (absdiffangle >= INT_ANGLE_45)  {
                    /* Yeah, a big angle.  Just turn on our heels. */
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, (delta << 8)) ;
                    else
                        ObjectAddAngle(p_obj, -((T_sword16)(delta << 8))) ;
                } else {
                    /* Not a big angle, we might as well go ahead */
                    /* and try moving a bit. */

                    /* Turn as needed. */
                    turnangle = (delta<<6) ;
                    if (turnangle > absdiffangle)
                        turnangle = absdiffangle ;
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, turnangle) ;
                    else
                        ObjectAddAngle(p_obj, -turnangle) ;

                    /* How close are we? */
                    distance = CalculateEstimateDistance(
                                   (T_sword16)(ObjectGetX16(p_obj)),
                                   (T_sword16)(ObjectGetY16(p_obj)),
                                   playerX,
                                   playerY) ;

    
    

                    /* Are we close enough to attack? */
                    if (distance < (ObjectGetRadius(p_obj) + 35))  {
                        /* !!! Attack !!! */
                        ObjectSetStance(p_obj, STANCE_ATTACK) ;
                        ObjectSetAngle(p_obj, angle) ;

                        ObjectGetForwardPosition(
                            p_obj,
                            (T_sword16)(ObjectGetRadius(p_obj) + 20),
                            &frontX,
                            &frontY) ;

                        ServerDamageAt(
                            frontX,
                            frontY,
                            frontX, 
                            frontY, 
                            0, 
                            100) ;

                        /* Set the time for pausing on the attack. */
                        p_creature->state = 2 ;
                        p_creature->stateData = 50 ;
                    } else {
                        /* Too far, let's move closer. */
                        ObjectAccelFlat(p_obj, delta, ObjectGetAngle(p_obj)) ;
                        p_creature->stepping += delta ;

                        /* See if we are tired of moving and want */
                        /* a new opponent. */
                        if (p_creature->stateData <= newTime)  {
                            /* Look around for opponents. */
                            p_newTarget = CreatureLookForPlayer(p_obj) ;
                            if (p_newTarget != NULL)
                                p_creature->p_target = p_newTarget ;
                            p_creature->stateData = newTime+210 ;
                        }
                    }
                }
                break ;
            case 2:   /* Pause after attacking or getting hurt. */
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 10)
                    /* Yes, continue going after target. */
                    p_creature->state = 3 ;
                break ;
            case 3:
                ObjectSetStance(p_obj, STANCE_STAND) ;
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 0)  {
                    /* Yes, continue going after target. */
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;
                }
                break ;
        }
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  CreatureTakeDamage                                            */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreatureTakeDamage does damage to an intelligent object (creature)    */
/*  and checks to see if it dies or is just hurt.                           */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_3dObject *p_obj           -- Creature to take damage.               */
/*                                                                          */
/*    T_word32 damage             -- Amount of damage done                  */
/*                                                                          */
/*    T_byte8 type                -- type   of damage done                  */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  07/11/95  Created                                                */
/*    LES  03/19/95  Added parameter type                                   */
/*                                                                          */
/****************************************************************************/

T_void CreatureTakeDamage(
           T_3dObject *p_obj,
           T_word32 damage,
           T_byte8 type)
{
    T_word16 health ;
    T_creature *p_creature ;

    DebugRoutine("CreatureTakeDamage") ;
    DebugCheck(p_obj != NULL) ;

    p_creature = ObjectGetExtraData(p_obj) ;
    if (p_creature != NULL)  {
        health = ObjectGetHealth(p_obj) ;

        if (damage >= health)  {
            /* Its dead now. */
            ObjectSetStance(p_obj, STANCE_DIE) ;
            ObjectSetHealth(p_obj, 0) ;
            ObjectStopMoving(p_obj) ;

            /* Stop thinking about the creature by removing its extra */
            /* data. */
            MemFree(p_creature) ;
            ObjectSetExtraData(p_obj, NULL) ;

            /* Make sure gravity takes effect. */
            ObjectClearMoveFlags(
                p_obj,
                OBJMOVE_FLAG_IGNORE_GRAVITY |
                OBJMOVE_FLAG_IGNORE_FRICTION |
                OBJMOVE_FLAG_FAST_MOVEMENT) ;
        } else {
            /* Ouch, that hurt. */
            ObjectSetStance(p_obj, STANCE_HURT) ;
            ObjectSetHealth(p_obj, health - damage) ;
            p_creature->state = 2 ;
            p_creature->stateData = 35 ;
        }
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  CreatureUpdateDragon                                          */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreatureUpdateDragon is used to move the dragon creature and shoot.   */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_creature *p_creature      -- creature to update                     */
/*                                                                          */
/*    T_word32 delta              -- Amount to move/update creature         */
/*                                                                          */
/*    T_word32 newTime            -- Current time to update                 */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    MathArcTangent                                                        */
/*    CalculateDistance                                                     */
/*    ICreatureSendCreatureAttack                                           */
/*    View3dSetExceptObject                                                 */
/*    View3dMoveTo                                                          */
/*    CreatureLookForPlayer                                                 */
/*    View3dGetFloorAndCeiling                                              */
/*    MapGetFloorWalkingHeight                                              */
/*    ICreatureSendMoveCreaturePacket                                       */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  07/25/95  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void CreatureUpdateDragon(
           T_3dObject *p_obj,
           T_word32 delta,
           T_word32 newTime)
{
    T_sword16 playerX, playerY ;
    T_word16 angle ;
    T_sword16 diffangle ;
    T_sword16 absdiffangle ;
    T_word16 distance ;
    T_3dObject *p_newTarget ;
    T_word16 turnangle ;
    T_creature *p_creature ;
    T_sword32 frontX, frontY ;
    T_3dObject *p_target ;

    DebugRoutine("CreatureUpdateDragon") ;
    DebugCheck(p_obj != NULL) ;

    ObjectSetMaxVelocity(p_obj, 8) ;

    ObjectClearMoveFlags(p_obj, OBJMOVE_FLAG_FAST_MOVEMENT) ;

/* The dragon is invincible. */
ObjectSetHealth(p_obj, 30000) ;

    p_creature = (T_creature *)ObjectGetExtraData(p_obj) ;

    if (p_creature != NULL)  {

        /** Is it time for a noise? **/
        if (TickerGet () > p_creature->nextScream)
        {
           if (p_creature->nextScream == 0)
           {
              /** First time.  Don't sound off, just set the timer. **/
           }
           else
           {
              /** Time for a holler. **/
              ServerAreaSoundGlobal (
                     (T_sword16)(ObjectGetX16 (p_obj)),
                     (T_sword16)(ObjectGetY16 (p_obj)),
                     1000,
                     114);
           }
           /** Set the timer for the next scream. **/
           p_creature->nextScream = TickerGet() + 350 + (rand () % 350);
        }

        /* Check to see if we still have a target. */
        if (p_creature->p_target == NULL)
            /* Nope, go into the mode to get one. */
            p_creature->state = 0 ;

        switch(p_creature->state)  {
            case 0:   /* Initial state -- looking for a target. */
                ObjectSetStance(p_obj, STANCE_STAND) ;
                if ((p_creature->p_target =
                        CreatureLookForPlayer(p_obj)))  {
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;

                    /* Creature sound that he sees you! */
                    ServerAreaSoundGlobal (
                           (T_sword16)(ObjectGetX16 (p_obj)),
                           (T_sword16)(ObjectGetY16 (p_obj)),
                           1000,
                           110);
                }
                break ;
            case 1:   /* Moving towards opponent state. */
                /* Make us animate as walking. */
                ObjectSetStance(p_obj, STANCE_WALK) ;

                /* First, see if the creature was blocked last time. */
                if (ObjectWasBlocked(p_obj))  {
                    /* We are hitting something, let's turn randomly */
                    /* 45 degrees to one side. */
                    if (p_creature->goForward != TRUE)
                        p_creature->turnWay = TickerGet() & 1 ;

                    if (p_creature->turnWay)  {
                        ObjectAddAngle(p_obj, INT_ANGLE_45) ;
                    } else {
                        ObjectAddAngle(p_obj, -INT_ANGLE_45) ;
                    }

                    p_creature->goForward = TRUE ;
                    p_creature->stepAway = 70 ;

                    /* Make sure future access say we were not blocked. */
                    ObjectClearBlockedFlag(p_obj) ;
                }

                /* Locate where the target is. */
/*
                ServerGetPlayerLocation(
                    p_creature->target,
                    &playerX,
                    &playerY) ;
*/
                playerX = ObjectGetX16(p_creature->p_target) ;
                playerY = ObjectGetY16(p_creature->p_target) ;

                /* Get the angle to this target. */
                angle = MathArcTangent(
                            (T_sword16)(playerX - ObjectGetX16(p_obj)),
                            (T_sword16)(playerY - ObjectGetY16(p_obj))) ;

                /* What is the difference to the angle I'm currently */
                /* facing? */
                diffangle = ObjectGetAngle(p_obj) - angle ;
                if (diffangle < 0)
                    absdiffangle = -diffangle ;
                else
                    absdiffangle = diffangle ;

                if (p_creature->goForward == TRUE)  {
                    absdiffangle = 0 ;
                    p_creature->stepAway -= delta ;
                    if (p_creature->stepAway < 0)
                        p_creature->goForward = FALSE ;
                }

                /* Is that a big angle or a small one? */
                if (absdiffangle >= INT_ANGLE_45)  {
                    /* Yeah, a big angle.  Just turn on our heels. */
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, (delta << 8)) ;
                    else
                        ObjectAddAngle(p_obj, -((T_sword16)(delta << 8))) ;
                } else {
                    /* Not a big angle, we might as well go ahead */
                    /* and try moving a bit. */

                    /* Turn as needed. */
                    turnangle = (delta<<6) ;
                    if (turnangle > absdiffangle)
                        turnangle = absdiffangle ;
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, turnangle) ;
                    else
                        ObjectAddAngle(p_obj, -turnangle) ;

                    /* How close are we? */
                    distance = CalculateEstimateDistance(
                                   (T_sword16)(ObjectGetX16(p_obj)),
                                   (T_sword16)(ObjectGetY16(p_obj)),
                                   playerX,
                                   playerY) ;

                    /* Are we close enough to attack? */
                    if (distance < 100)  {
                        /* !!! Attack !!! */
                        ObjectSetStance(p_obj, STANCE_ATTACK) ;
                        ObjectSetAngle(p_obj, angle) ;

                        ObjectGetForwardPosition(
                            p_obj,
                            110,
                            &frontX,
                            &frontY) ;

                        ServerDamageAt(
                            frontX,
                            frontY,
                            frontX, 
                            frontY, 
                            0, 
                            200) ;

                        /* Set the time for pausing on the attack. */
                        p_creature->state = 2 ;
                        p_creature->stateData = 27 ;
                    } else if ((distance < 2000) &&
                               (TickerGet() > p_creature->nextAttack))  {
                        /* Too far to hit, but close enough to shoot a */
                        /* fireball. */
                        ObjectSetStance(p_obj, STANCE_ATTACK) ;

/*
                        p_target = NULL ;
                        if (p_creature->target != 0xFFFF)
                            p_target =
                                ServerGetPlayerObject(p_creature->target) ;
*/
                        p_target = p_creature->p_target ;

                        /* Request that this creature shoots a fireball. */
                        ICreatureShootFireball(p_obj, p_target) ;

                        /* Compute when we can shoot again. */
                        /* We'll say one fireball per 2 seconds. */
                        p_creature->nextAttack =
                            TickerGet() + 140 + (rand() % 140) ;

                        /* Set the time for pausing on the attack. */
                        p_creature->state = 2 ;
                        p_creature->stateData = 27 ;
                    } else {
                        /* Too far, let's move closer. */
                        ObjectAccelFlat(p_obj, delta, ObjectGetAngle(p_obj)) ;
                        p_creature->stepping += delta ;

                        /* See if we are tired of moving and want */
                        /* a new opponent. */
                        if (p_creature->stateData <= newTime)  {
                            /* Look around for opponents. */
                            p_newTarget = CreatureLookForPlayer(p_obj) ;
                            if (p_newTarget != NULL)
                                p_creature->p_target = p_newTarget ;
                            p_creature->stateData = newTime+210 ;
                        }
                    }
                }
                break ;
            case 2:   /* Pause after attacking or getting hurt. */
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 10)
                    /* Yes, continue going after target. */
                    p_creature->state = 3 ;
                break ;
            case 3:
                ObjectSetStance(p_obj, STANCE_STAND) ;
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 0)  {
                    /* Yes, continue going after target. */
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;
                }
                break ;
        }
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  ICreatureShootFireball             * INTERNAL *               */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreatureShootFireball creates a fireball object and requests that the */
/*  server add it to the game.                                              */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_3dObject *p_creatureObj   -- Creature's object that is shooting     */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    TickerGet                                                             */
/*    ObjectCreate                                                          */
/*    PlayerGetX                                                            */
/*    PlayerGetY                                                            */
/*    ViewActiveObject                                                      */
/*    ObjectDeclareMoveable                                                 */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  07/25/95  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void ICreatureShootFireball(T_3dObject *p_creatureObj, T_3dObject *p_target)
{
    T_sword32 x, y ;
    T_sword16 height ;
    T_sword16 heightTarget ;
    T_sword16 tx, ty ;
    T_sword32 distance ;
    T_word16 angle ;
    T_3dObject *p_obj;
    T_sword32 deltaHeight;
    T_word32 objectRadius;

    DebugRoutine("ICreatureShootFireball") ;


    /* Let's make a fireball. */
    p_obj = ObjectCreate() ;
    DebugCheck(p_obj != NULL) ;

    /* Set up the shot and who to hit. */
    if (p_target != NULL)  {
        /* Use the given target. */
        ServerShootProjectile(
            p_creatureObj,
            ObjectGetServerId(p_target),
            36,
            30) ;
    } else {
        /* Use no particular target. */
        ServerShootProjectile(
            p_creatureObj,
            0,
            36,
            30) ;
    }

    DebugEnd() ;

    return ;

/***************************** OLD METHOD *******************************/

    ObjectSetType(p_obj, 36) ;

    /** I need to place the object's center a distance **/
    /** away from me equal to the sum of his and my radii. **/
    objectRadius = ObjectGetRadius(p_obj);
    objectRadius += ObjectGetRadius(p_creatureObj) ;
    objectRadius += objectRadius >> 1;

    ObjectGetForwardPosition(
         p_creatureObj,
         (T_word16)objectRadius,
         &x,
         &y) ;

    /** And his initial Z should be roughly at my belly. **/
    ObjectSetZ16 (p_obj, ObjectGetZ16 (p_creatureObj) + ((ObjectGetHeight(p_creatureObj)*3)/4));

    /* Check if there is a target. */
    if (p_target == NULL)  {
        /* There is none, just shoot straight. */
        deltaHeight = 0 ;
    } else  {
        /* Find where the target is located. */
        tx = ObjectGetX16(p_target) ;
        ty = ObjectGetY16(p_target) ;

        /* How far is it? */
        distance = CalculateDistance(x >> 16, y >> 16, tx, ty) ;
        /* How high is the target? */
        heightTarget = ObjectGetMiddleHeight(p_target) ;

        /* Calculate the steps necessary to draw a straight */
        /* line to the target. */
        deltaHeight = (((T_sword32)(heightTarget - height))<<16) / distance ;
        deltaHeight *= 40 ;
        /* Don't allow more than 45 degrees up. */
        if (deltaHeight >= 0x320000)
            deltaHeight = 0x320000 ;

        /* Don't allow more than 45 degrees down. */
        if (deltaHeight <= -0x320000)
            deltaHeight = -0x320000 ;
    }

    ObjectSetZVel (p_obj, deltaHeight);

    /* Declare the angle we are going to be shooting at. */
    angle = ObjectGetAngle(p_creatureObj) ;
    ObjectSetAngle(p_obj, angle) ;

    /* Set the velocity of the fireball. */
    ObjectSetAngularVelocity(
        p_obj,
        angle,
        50) ;

    /* Can the object exist there? **/
    if (ObjectCheckIfCollide (p_obj, x, y, ObjectGetZ (p_obj)))
    {
        ObjectDestroy (p_obj);
    }
    else
    {
        /** Finally, request the server to add it to the **/
        /** world.  NOTE that this command removes it from **/
        /** ours. **/

        ObjectSetX (p_obj, x);
        ObjectSetY (p_obj, y);

        /* have the object added to everyone's world. */
        ServerAddObjectGlobal(p_obj);
    }


/***************************** OLD METHOD *******************************/
    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  CreatureUpdateApe                                             */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreatureUpdateApe    is used to move a creature that walks.           */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_creature *p_creature      -- creature to update                     */
/*                                                                          */
/*    T_word32 delta              -- Amount to move/update creature         */
/*                                                                          */
/*    T_word32 newTime            -- Current time to update                 */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    MathArcTangent                                                        */
/*    CalculateDistance                                                     */
/*    ICreatureSendCreatureAttack                                           */
/*    View3dSetExceptObject                                                 */
/*    View3dMoveTo                                                          */
/*    CreatureLookForPlayer                                                 */
/*    View3dGetFloorAndCeiling                                              */
/*    MapGetFloorWalkingHeight                                              */
/*    ICreatureSendMoveCreaturePacket                                       */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  05/25/95  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void CreatureUpdateApe(
           T_3dObject *p_obj,
           T_word32 delta,
           T_word32 newTime)
{
    T_sword16 playerX, playerY ;
    T_word16 angle ;
    T_sword16 diffangle ;
    T_sword16 absdiffangle ;
    T_word16 distance ;
    T_3dObject *p_newTarget ;
    T_word16 turnangle ;
    T_creature *p_creature ;
    T_sword32 frontX, frontY ;

    DebugRoutine("CreatureUpdateApe") ;
    DebugCheck(p_obj != NULL) ;

    ObjectSetMaxVelocity(p_obj, 8) ;

    p_creature = (T_creature *)ObjectGetExtraData(p_obj) ;

    if (p_creature != NULL)  {

        /** Is it time for a noise? **/
        if (TickerGet () > p_creature->nextScream)
        {
           if (p_creature->nextScream == 0)
           {
              /** First time.  Don't sound off, just set the timer. **/
           }
           else
           {
              /** Time for a holler. **/
              ServerAreaSoundGlobal (
                     (T_sword16)(ObjectGetX16 (p_obj)),
                     (T_sword16)(ObjectGetY16 (p_obj)),
                     1000,
                     84);
           }
           /** Set the timer for the next scream. **/
           p_creature->nextScream = TickerGet() + 350 + (rand () % 350);
        }

        /* Check to see if we still have a target. */
        if (p_creature->p_target == NULL)
            /* Nope, go into the mode to get one. */
            p_creature->state = 0 ;

        switch(p_creature->state)  {
            case 0:   /* Initial state -- looking for a target. */
                ObjectSetStance(p_obj, STANCE_STAND) ;
                if ((p_creature->p_target =
                        CreatureLookForPlayer(p_obj)) != NULL)  {
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;

                    /* Creature sound that he sees you! */
                    ServerAreaSoundGlobal (
                           (T_sword16)(ObjectGetX16 (p_obj)),
                           (T_sword16)(ObjectGetY16 (p_obj)),
                           1000,
                           80);
                }
                break ;
            case 1:   /* Moving towards opponent state. */
                /* Make us animate as walking. */
                ObjectSetStance(p_obj, STANCE_WALK) ;

                /* First, see if the creature was blocked last time. */
                if (ObjectWasBlocked(p_obj))  {
                    /* We are hitting something, let's turn randomly */
                    /* 45 degrees to one side. */
                    if (p_creature->goForward != TRUE)
                        p_creature->turnWay = TickerGet() & 1 ;

                    if (p_creature->turnWay)  {
                        ObjectAddAngle(p_obj, INT_ANGLE_45) ;
                    } else {
                        ObjectAddAngle(p_obj, -INT_ANGLE_45) ;
                    }

                    p_creature->goForward = TRUE ;
                    p_creature->stepAway = 70 ;

                    /* Make sure future access say we were not blocked. */
                    ObjectClearBlockedFlag(p_obj) ;
                }

                /* Locate where the target is. */
/*
                ServerGetPlayerLocation(
                    p_creature->target,
                    &playerX,
                    &playerY) ;
*/
                playerX = ObjectGetX16(p_creature->p_target) ;
                playerY = ObjectGetY16(p_creature->p_target) ;

                /* Get the angle to this target. */
                angle = MathArcTangent(
                            (T_sword16)(playerX - ObjectGetX16(p_obj)),
                            (T_sword16)(playerY - ObjectGetY16(p_obj))) ;

                /* What is the difference to the angle I'm currently */
                /* facing? */
                diffangle = ObjectGetAngle(p_obj) - angle ;
                if (diffangle < 0)
                    absdiffangle = -diffangle ;
                else
                    absdiffangle = diffangle ;

                if (p_creature->goForward == TRUE)  {
                    absdiffangle = 0 ;
                    p_creature->stepAway -= delta ;
                    if (p_creature->stepAway < 0)
                        p_creature->goForward = FALSE ;
                }

                /* Is that a big angle or a small one? */
                if (absdiffangle >= INT_ANGLE_45)  {
                    /* Yeah, a big angle.  Just turn on our heels. */
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, (delta << 8)) ;
                    else
                        ObjectAddAngle(p_obj, -((T_sword16)(delta << 8))) ;
                } else {
                    /* Not a big angle, we might as well go ahead */
                    /* and try moving a bit. */

                    /* Turn as needed. */
                    turnangle = (delta<<6) ;
                    if (turnangle > absdiffangle)
                        turnangle = absdiffangle ;
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, turnangle) ;
                    else
                        ObjectAddAngle(p_obj, -turnangle) ;

                    /* How close are we? */
                    distance = CalculateEstimateDistance(
                                   (T_sword16)(ObjectGetX16(p_obj)),
                                   (T_sword16)(ObjectGetY16(p_obj)),
                                   playerX,
                                   playerY) ;

    
    

                    /* Are we close enough to attack? */
                    if (distance < 65)  {
                        /* !!! Attack !!! */
                        if (rand() & 1)
                            ObjectSetStance(p_obj, STANCE_ATTACK) ;
                        else
                            ObjectSetStance(p_obj, STANCE_ATTACK2) ;

                        ObjectSetAngle(p_obj, angle) ;

                        ObjectGetForwardPosition(
                            p_obj,
                            (T_word16)(ObjectGetRadius(p_obj) + 25),
                            &frontX,
                            &frontY) ;

                        ServerDamageAt(
                            frontX,
                            frontY,
                            frontX, 
                            frontY, 
                            0, 
                            100) ;

                        /* Set the time for pausing on the attack. */
                        p_creature->state = 2 ;
                        p_creature->stateData = 27 ;
                    } else {
                        /* Too far, let's move closer. */
                        ObjectAccelFlat(p_obj, delta, ObjectGetAngle(p_obj)) ;
                        p_creature->stepping += delta ;

                        /* See if we are tired of moving and want */
                        /* a new opponent. */
                        if (p_creature->stateData <= newTime)  {
                            /* Look around for opponents. */
                            p_newTarget = CreatureLookForPlayer(p_obj) ;
                            if (p_newTarget != NULL)
                                p_creature->p_target = p_newTarget ;
                            p_creature->stateData = newTime+210 ;
                        }
                    }
                }
                break ;
            case 2:   /* Pause after attacking or getting hurt. */
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 10)
                    /* Yes, continue going after target. */
                    p_creature->state = 3 ;
                break ;
            case 3:
                ObjectSetStance(p_obj, STANCE_STAND) ;
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 0)  {
                    /* Yes, continue going after target. */
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;
                }
                break ;
        }
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  CreatureUpdateSplat                                           */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreatureUpdateSplat    is the brains behind a splat.     (Doesn't     */
/*  that sound absurd?)                                                     */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_3dObject *p_obj           -- fireball object                        */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  07/27/95  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void CreatureUpdateSplat(T_3dObject *p_obj)
{
   T_packetShort destroyPacket;
   T_creature *p_creature ;
   T_word32 delta ;

   DebugRoutine ("CreatureUpdateSplat");

   /* Get the creature pointer. */
   p_creature = (T_creature *)ObjectGetExtraData(p_obj) ;

   if  (p_creature != NULL)  {
       /* Update the time and calculate how long it has been. */
       if (p_creature->nextAttack == 0)
           p_creature->nextAttack = TickerGet() ;

       delta = TickerGet() - p_creature->nextAttack ;

       p_creature->nextAttack = TickerGet() ;

       /* Make the damage more. */
       delta *= 32 ;

       /** Yes.  First, do damage all around me relative to the time */
       /* it has been. **/
    /*
       if (delta)
           ServerDamageAt (ObjectGetX (p_obj), ObjectGetY (p_obj), 60, delta);
    */

       /** Hmm..  this is simple.  Have I hit anything? **/
       if (ObjectWasBlocked (p_obj))
       {
          /* Make sure the object is passable. */
          ObjectAddAttributes(p_obj, OBJECT_ATTR_PASSABLE) ;

          /* Clear the movement flags. */
          ObjectClearMoveFlags(
              p_obj,
              OBJMOVE_FLAG_IGNORE_FRICTION |
                 OBJMOVE_FLAG_IGNORE_GRAVITY) ;

          ObjectSetStance(p_obj, 1) ;

          ObjectStopMoving(p_obj) ;

    /* Try to "turn off" this creature. */
    MemFree(p_creature) ;
    ObjectSetExtraData(p_obj, NULL) ;
    ObjectSetScript(p_obj, 0) ;
       }
   }

   DebugEnd ();
}

#else

#define MAX_CREATURES 100
#define CREATURE_BAD NULL

/* Extra data for a creature that is stored in an object's */
/* extra data slot. */
typedef struct T_creatureStruct {
//    T_sword16 height ;
    T_word16 state ;
    T_sword32 stateData ;
    T_word16 target ;
    E_Boolean goForward ;
    E_Boolean needTurn ;
    T_word16 turnWay ;
    T_sword32 stepAway ;
    T_word16 playerId ;
    T_word32 nextScream;
    T_word32 nextAttack ;
} T_creature ;

/* Note if we have initialized the creatuers module. */
static E_Boolean G_creaturesInit = FALSE ;

/* Note when the creatures were last updated. */
static T_word32 G_lastCreatureUpdate = 0 ;
static T_word32 G_newCreatureUpdate = 0 ;
static T_word32 G_deltaTime = 0 ;

/* Internal prototypes: */
T_sword16 CreatureLookForPlayer(T_3dObject *p_obj) ;

T_void ICreatureDestroy(T_creature *p_creature) ;

T_void CreatureUpdateSplat(T_3dObject *p_obj) ;

T_void CreatureUpdateFireball (T_3dObject *p_obj);

T_void ICreatureShootFireball(T_3dObject *p_creatureObj, T_3dObject *target) ;

E_Boolean ICreatureUpdate(T_3dObject *p_obj, T_word32 time) ;

E_Boolean ICreaturePlayerGone(T_3dObject *p_obj, T_word32 data) ;

/****************************************************************************/
/*  Routine:  CreaturesLoad                                                 */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreaturesLoad loads the creature information file and sets up all     */
/*  the creature memory.                                                    */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_word16 mapNumber          -- Map to load creatures from             */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
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
/*    LES  02/23/95  Created                                                */
/*    LES  05/30/95  Made creatures always impassable.                      */
/*    LES  07/10/95  Uses object system now.                                */
/*                                                                          */
/****************************************************************************/

T_void CreaturesLoad(T_word32 mapNumber)
{
    FILE *fp ;
    char filename[20] ;
    T_creature *p_creature ;
    T_word16 i ;
    T_sword32 valueA, valueB, valueC ;
    T_3dObject *p_obj ;
    T_word16 numberCreatures ;
    T_file file;

    DebugRoutine("CreaturesLoad") ;
    DebugCheck(G_creaturesInit == FALSE) ;

    /* Find the file name of the creature file. */
    sprintf(filename, "l%ld.cre", mapNumber) ;

    /* Open up the file. */
    fp = fopen(filename, "r") ;


    DebugCheck(fp != NULL) ;

    /* How many creatures is it telling us that we have. */
    fscanf(fp, "%d", &valueA) ;
    numberCreatures = valueA ;

    /* Loop and create each of the creatures. */
    for (i=0; i<numberCreatures; i++)  {
#if 1
        /* Each creature requires memory. */
        p_creature = MemAlloc(sizeof(T_creature)) ;

        /* If we don't get the memory.  Stop here and say this */
        /* is the last creature. */
        DebugCheck(p_creature != NULL) ;
        if (p_creature == NULL)  {
            numberCreatures = i ;
            break ;
        }

        /* Clear out the creature data before we start. */
        memset(p_creature, 0, sizeof(T_creature)) ;

        /* Read in some more information. */
        fscanf(fp, "%d %d %d",
            &valueA,
            &valueB,
            &valueC) ;

        /* Use the object number to get more information about the */
        /* creature. */
        p_obj = ObjectFind(valueA) ;
        if (p_obj != NULL)  {
            ObjectSetExtraData(p_obj, p_creature) ;
            ObjectSetHealth(p_obj, valueC) ;

            /* Make the creature impassable. */
//            ObjectMakeImpassable(p_obj) ;

            /* Start the creature in the initial state. */
            p_creature->state = 0 ;

            /* No packets have been sent about this creature. */
            ObjectSetNumPackets(p_obj, 0) ;
        } else {
            DebugCheck(FALSE) ;
        }
#else
/* NEW WAY WITH SCRIPTS */
        /* Read in creature info. */
        fscanf(fp, "%d %d %d",
            &valueA,
            &valueB,
            &valueC) ;

        /* Find the creature. */
        p_obj = ObjectFind(valueA) ;
        if (p_obj != NULL)  {
//            DebugCheck(ObjectGetExtraData(p_obj) == NULL) ;

            /* Each creature requires memory. */
//            p_creature = MemAlloc(sizeof(T_creature)) ;
            p_creature = ObjectAllocExtraData(p_obj, sizeof(T_creature)) ;

            /* If we don't get the memory.  Stop here and say this */
            /* is the last creature. */
            DebugCheck(p_creature != NULL) ;
            if (p_creature == NULL)  {
                numberCreatures = i ;
                break ;
            }

            /* Clear out the creature data before we start. */
            memset(p_creature, 0, sizeof(T_creature)) ;

            /* Store the script. */
//            ObjectSetExtraData(p_obj, p_creature) ;

            /* Make the creature impassable. */
//            ObjectMakeImpassable(p_obj) ;

            /* Start the creature in the initial state. */
            p_creature->state = 0 ;

            /* No packets have been sent about this creature. */
            ObjectSetNumPackets(p_obj, 0) ;
            ObjectSetHealth(p_obj, valueC) ;
        } else {
            DebugCheck(FALSE) ;
        }
#endif
    }

    fclose(fp) ;

    /* Start the creature updates from this time point. */
    G_lastCreatureUpdate = TickerGet() ;

    G_creaturesInit = TRUE ;

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  CreatureAttachToObject                                        */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreatureAttachToObject does the job of making an object into a        */
/*  creature by allocating a creature structure and binding the two         */
/*  together.                                                               */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_3dObject *p_obj           -- Object to turn into a creature         */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    MemAlloc                                                              */
/*    memset                                                                */
/*    ObjectSetExtraData                                                    */
/*    ObjectSetHealth                                                       */
/*    ObjectMakeImpassible                                                  */
/*    ObjectSetNumPackets                                                   */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  02/12/96  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void CreatureAttachToObject(T_3dObject *p_obj)
{
    T_creature *p_creature ;

    DebugRoutine("CreatureAttachToObject") ;
    DebugCheck(p_obj != NULL) ;
//printf("attaching creature to object %p (%d)\n", p_obj, ObjectGetServerId(p_obj)) ;
    if (p_obj)  {
        /* Each creature requires memory. */
        p_creature = MemAlloc(sizeof(T_creature)) ;

        /* If we don't get the memory.  Stop here and say this */
        /* is the last creature. */
        DebugCheck(p_creature != NULL) ;
        if (p_creature != NULL)  {
            /* Clear out the creature data before we start. */
            memset(p_creature, 0, sizeof(T_creature)) ;

            /* Make the object use this creature data as its extra */
            /* data. */
            ObjectSetExtraData(p_obj, p_creature) ;
            ObjectSetHealth(p_obj, ObjTypeGetHealth(p_obj->p_objType)) ;

            /* Make the creature impassable. */
//            ObjectMakeImpassable(p_obj) ;

            /* Start the creature in the initial state. */
            p_creature->state = 0 ;

            /* No packets have been sent about this creature. */
            ObjectSetNumPackets(p_obj, 0) ;
        }
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  CreaturesUnload                                               */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreaturesUnload removes any creature data that is in memory.          */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
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
/*    LES  02/23/95  Created                                                */
/*    LES  07/10/95  Uses object system now.                                */
/*                                                                          */
/****************************************************************************/

T_void CreaturesUnload(T_void)
{
    T_creature *p_next ;
    T_word16 i ;

    DebugRoutine("CreaturesUnload") ;
    DebugCheck(G_creaturesInit == TRUE) ;

    G_creaturesInit = FALSE ;

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  CreaturesUpdate                                               */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreaturesUpdate moves all the creatures in play.                      */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    ObjectCreate                                                          */
/*    PlayerGetX                                                            */
/*    PlayerGetY                                                            */
/*    ObjectDeclareStatic                                                   */
/*    TickerGet                                                             */
/*    ObjectDestroy                                                          */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  02/23/95  Created                                                */
/*    LES  03/01/95  Improved for movement and attacking.                   */
/*    LES  04/16/95  Broke apart logic to allow multiple types of creatures.*/
/*    LES  05/30/95  Only updates creatures that have health now.           */
/*    LES  07/10/95  Uses object system now.                                */
/*                                                                          */
/****************************************************************************/

T_void CreaturesUpdate(T_void)
{
    T_creature *p_creature ;
    static T_word32 lastTime = 0 ;
    T_word32 newTime ;
    T_word32 delta ;

    DebugRoutine("CreaturesUpdate") ;

    /* Get the update time, and calculate the delta time since */
    /* the last update.  Unfortunately, the callback to ObjectsDoToAll */
    /* doesn't take data. */
    G_newCreatureUpdate = TickerGet() ;
    G_deltaTime = G_newCreatureUpdate - G_lastCreatureUpdate ;

    ObjectsDoToAll(ICreatureUpdate, G_newCreatureUpdate) ;

    /* Note the time we updated. */
    G_lastCreatureUpdate = G_newCreatureUpdate ;

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  ICreatureUpdate                                               */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    ICreatureUpdate updates the movement of one creature as called by     */
/*  ObjectsDoToAll (via CreaturesUpdate).  It uses the G_newCreatureUpdata  */
/*  and G_deltaTime to determine update times.                              */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_3dObject *p_obj           -- object/creature to update              */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    ObjectGetScript                                                       */
/*    CreatureUpdateWalker                                                  */
/*    CreatureUpdateFlyer                                                   */
/*    CreatureUpdateRandom                                                  */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  07/10/95  Derived from old CreatureUpdate                        */
/*    AMT  07/18/95  Modified so it passes over weapons, who use the script */
/*                   field for their own devious purposes.                  */
/*                                                                          */
/****************************************************************************/

E_Boolean ICreatureUpdate(T_3dObject *p_obj, T_word32 time)
{
    T_word32 script ;

    DebugRoutine("ICreatureUpdate") ;

    /* Get the script attached to this object. */
    if (p_obj->p_objType != NULL)  {
        DebugCheck(p_obj->p_objType != NULL) ;
        script = ObjectGetScript(p_obj) ;

        /* If there IS a script, and it's not a weapon, do that script. */
        if ((!(ObjectGetAttributes (p_obj) & OBJECT_ATTR_WEAPON)) &&
	    (script != 0))  {
            if ((ObjectGetHealth(p_obj) > 0) || (script == 4))   {
#if 1
             /* !!! Actual script handling is called here.  However, */
             /* since we are hard-coding monster movement for now, */
             /* we MUST use a switch statement. */
             switch(script)  {
                 case 1:    /* Random walker (stefan) */
                     CreatureUpdateRandomWalker(
                         p_obj,
                         G_deltaTime,
                         time) ;
                     break ;
                 case 2:    /* attacker (biff) */
                     CreatureUpdateWalker(
                         p_obj,
                         G_deltaTime,
                         time) ;
                     break ;
                 case 22:   /* hydra */
                     CreatureUpdateHydra(
                         p_obj,
                         G_deltaTime,
                         time) ;
                     break ;
                 case 3:    /* flyer (shadow) */
                     CreatureUpdateFlyer(
                         p_obj,
                         G_deltaTime,
                         time) ;
                     break ;
                 case 18:
                     CreatureUpdateElf(
                         p_obj,
                         G_deltaTime,
                         time) ;
                     break ;
                 case 19:
                     CreatureUpdateDruid(
                         p_obj,
                         G_deltaTime,
                         time) ;
                     break ;
                 case 20:
                     CreatureUpdateLich(
                         p_obj,
                         G_deltaTime,
                         time) ;
                     break ;
                 case 23:   /* griffon */
                     CreatureUpdateGriffon(
                         p_obj,
                         G_deltaTime,
                         time) ;
                     break ;
                 case 12:   /* banshee */
                     CreatureUpdateBanshee(
                         p_obj,
                         G_deltaTime,
                         time) ;
                     break ;
                 case 4:    /* straight & boom (fireball) */
                     CreatureUpdateFireball (p_obj);
                     break ;
                 case 5:    /* Dragon */
                     CreatureUpdateDragon(p_obj, G_deltaTime, time);
                     break ;
                 case 6:    /* attacker (ape) */
                     CreatureUpdateApe(
                         p_obj,
                         G_deltaTime,
                         time) ;
                     break ;
                 case 7:    /* straight & boom (fireball) */
                     CreatureUpdateFireball (p_obj);
                     break ;
                 case 26:
                     CreatureUpdateBronzeBarbarian(
                         p_obj,
                         G_deltaTime,
                         time) ;
                     break ;
                 case 27:
                     CreatureUpdateBlackBarbarian(
                         p_obj,
                         G_deltaTime,
                         time) ;
                     break ;
                 case 28:
                     CreatureUpdateWhiteBarbarian(
                         p_obj,
                         G_deltaTime,
                         time) ;
                 default:
#if 1
                     CreatureUpdateWalker(
                         p_obj,
                         G_deltaTime,
                         time) ;
                     break ;
#else
#ifndef NDEBUG
                     printf("bad script %d\n", script) ;
#endif
                     /* Bad script, stop here. */
                     DebugCheck(FALSE) ;
                     break ;
#endif
             }
#else
             /* Now that we have determined that we have a creature */
             /* with a script ... call the script with a time update. */
             if (script == 4)
                 CreatureUpdateFireball(p_obj) ;
             else if (script == 1)
                 CreatureUpdateRandomWalker(
                     p_obj,
                     G_deltaTime,
                     time) ;
             else
                 ScriptEvent(
                     ObjectGetScriptHandle(p_obj),
                     SCRIPT_EVENT_TIME_UPDATE,
                     SCRIPT_DATA_TYPE_32_BIT_NUMBER,
                     &G_deltaTime,
                     SCRIPT_DATA_TYPE_NONE,
                     NULL,
                     SCRIPT_DATA_TYPE_NONE,
                     NULL) ;
#endif
             }
        }
    }

    DebugEnd() ;

    return FALSE ;
}

/****************************************************************************/
/*  Routine:  CreatureLookForPlayer                                         */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreatureLookForPlayer determines if there is player in the direction  */
/*  that a creature is facing.  If multiple players are seen, it takes the  */
/*  closest one.                                                            */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_creature *p_creature      -- Creature that is looking               */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    T_sword16                   -- Number of player, or -1.               */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    ServerGetPlayerLocation                                               */
/*    ServerGetNextPlayer                                                   */
/*    MathArcTangent                                                        */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  03/01/95  Created                                                */
/*                                                                          */
/****************************************************************************/

T_sword16 CreatureLookForPlayer(T_3dObject *p_obj)
{
    T_sword16 player ;
    T_word32 distance ;
    T_word32 bestDistance = 0xFFFFFFFF;
    T_sword16 closest = -1 ;
    T_sword16 playerX, playerY ;
    T_sword32 x, y ;
    T_sword32 dx, dy ;
    T_word16 angle ;

    DebugRoutine("CreatureLookForPlayer") ;
    DebugCheck(p_obj != NULL) ;

    x = ObjectGetX16(p_obj) ;
    y = ObjectGetY16(p_obj) ;

    /* Go through each player looking for the those in the view */
    /* and then find the closest of those. */
    player = ServerGetNextPlayer(-1) ;
    while (player != -1)  {
        /* See where player is located. */
        ServerGetPlayerLocation(player, &playerX, &playerY) ;
        dx = ((T_sword32)playerX) - x ;
        dy = ((T_sword32)playerY) - y ;

        /* What angle is that? */
        angle = MathArcTangent(dx, dy) ;

        /* Is that angle in the 90 degrees that the creature sees? */
        if (((T_word16)((ObjectGetAngle(p_obj) - angle)+INT_ANGLE_45))
                             < ((T_word16)INT_ANGLE_90))  {
            /* Yes, the player is in view. */
            /* How far is this one. */
            distance = (dx*dx) + (dy*dy) ;

            /* Is it closer than the last player we found. */
            if (distance < bestDistance)  {
                /* Yes.  Make this the new choice target. */
                bestDistance = distance ;
                closest = player ;
            }
        }
        player = ServerGetNextPlayer(player) ;
    }

    DebugEnd() ;

    return closest ;
}

/****************************************************************************/
/*  Routine:  CreaturePlayerGone                                            */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreaturePlayerGone is called when a player has left the game (either  */
/*  by choice or dies).  This routine goes through all the creatures and    */
/*  uses this information to get the creatures to change targets if their   */
/*  target was the given player.                                            */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_creature *p_creature      -- Creature that is being sent            */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    T_sword16                   -- Number of player, or -1.               */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    Nothing                                                               */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  03/01/95  Created                                                */
/*    LES  07/10/95  Changed to make use of the object system.              */
/*                                                                          */
/****************************************************************************/

T_void CreaturePlayerGone(T_word16 playerId)
{
    DebugRoutine("CreaturePlayerGone") ;

    ObjectsDoToAll(ICreaturePlayerGone, playerId) ;

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  ICreaturePlayerGone                * INTERNAL *               */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    ICreaturePlayerGone is called when a player has left the game (either */
/*  by choice or dies).  This routine goes through all the creatures and    */
/*  uses this information to get the creatures to change targets if their   */
/*  target was the given player.                                            */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_3dObject *p_obj           -- Creature/object to check target of     */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    E_Boolean                   -- always FALSE                           */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    Nothing                                                               */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  07/10/95  Created                                                */
/*                                                                          */
/****************************************************************************/

E_Boolean ICreaturePlayerGone(T_3dObject *p_obj, T_word32 data)
{
    T_creature *p_creature ;
    T_word16 playerId ;

    DebugRoutine("ICreaturePlayerGone") ;

    playerId = data ;

    /* Does this object have an intelligence? */
    if (ObjectGetScript(p_obj) != 0)  {
        /* Yes.  Get rid of the targeted item. */
        p_creature = (T_creature *)ObjectGetExtraData(p_obj) ;
        if (p_creature != NULL)  {
            if (p_creature->target == playerId)
                p_creature->state = 0 ;
        }
    }

    DebugEnd() ;

    return FALSE ;
}

/****************************************************************************/
/*  Routine:  ICreatureDestroy                                              */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    ICreatureDestroy removes a creature from the creature list and frees  */
/*  the memory used by the creature.                                        */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_creature *p_creature      -- Creature that was hit                  */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    Nothing.                                                              */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    MemFree                                                               */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  03/08/95  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void ICreatureDestroy(T_creature *p_creature)
{
    T_creature *p_prev ;

    DebugRoutine("ICreatureDestroy") ;


    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  CreatureUpdateRandomWalker                                    */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreatureUpdateRandomWalker is used to move creatures that walk        */
/*  randomly and DONT attack.                                               */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_creature *p_creature      -- creature to update                     */
/*                                                                          */
/*    T_word32 delta              -- Amount to move/update creature         */
/*                                                                          */
/*    T_word32 newTime            -- Current time to update                 */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    MathArcTangent                                                        */
/*    CalculateDistance                                                     */
/*    ICreatureSendCreatureAttack                                           */
/*    View3dSetExceptObject                                                 */
/*    View3dMoveTo                                                          */
/*    CreatureLookForPlayer                                                 */
/*    View3dGetFloorAndCeiling                                              */
/*    MapGetFloorWalkingHeight                                              */
/*    ICreatureSendMoveCreaturePacket                                       */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  04/28/95  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void CreatureUpdateRandomWalker(
           T_3dObject *p_obj,
           T_word32 delta,
           T_word32 newTime)
{
    T_sword16 playerX, playerY ;
    T_word16 angle ;
    T_sword16 diffangle ;
    T_sword16 absdiffangle ;
    T_word16 distance ;
    T_word16 newTarget ;
    T_word16 turnangle ;
    T_creature *p_creature ;

    DebugRoutine("CreatureUpdateRandomWalker") ;
    DebugCheck(p_obj != NULL) ;

    ObjectSetMaxVelocity(p_obj, 8) ;
    p_creature = (T_creature *)ObjectGetExtraData(p_obj) ;
//    DebugCheck(p_creature != NULL) ;
    if (p_creature != NULL)  {

        /** Is it time for a noise? **/
        if (TickerGet () > p_creature->nextScream)
        {
           if (p_creature->nextScream == 0)
           {
              /** First time.  Don't sound off, just set the timer. **/
           }
           else
           {
              /** Time for a holler. **/
              if (p_creature->state < 2)
                  ServerAreaSoundGlobal (
                        ObjectGetX16 (p_obj),
                        ObjectGetY16 (p_obj),
                        1000,
                        50);
           }
           /** Set the timer for the next scream. **/
           p_creature->nextScream = TickerGet() + 700 + (rand () % 700);
        }

        /* First, see if the creature was blocked last time. */
        if (ObjectWasBlocked(p_obj))  {
            /* We are hitting something, let's turn randomly */
            /* 45 degrees to one side. */
            if (p_creature->goForward != TRUE)
                p_creature->turnWay = TickerGet() & 1 ;

            if (p_creature->turnWay)  {
                ObjectAddAngle(p_obj, INT_ANGLE_45) ;
            } else {
                ObjectAddAngle(p_obj, -INT_ANGLE_45) ;
            }

            p_creature->goForward = TRUE ;
            p_creature->stepAway = 70 ;

            /* Make sure future access say we were not blocked. */
            ObjectClearBlockedFlag(p_obj) ;
        }

        /* Check to see if we still have a target. */
        if (p_creature->target == 0xFFFF /* -1 */)
            /* Nope, go into the mode to get one. */
            p_creature->state = 0 ;

        switch(p_creature->state)  {
            case 0:   /* Initial state -- looking for a target. */
                ObjectSetStance(p_obj, STANCE_STAND) ;
                if ((p_creature->target =
                        CreatureLookForPlayer(p_obj)) != 0xFFFF)  {
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;
                    /* !!! Creature sound that he sees you! */
                }
                break ;
            case 1:   /* Moving towards opponent state. */
                /* Make us animate as walking. */
                ObjectSetStance(p_obj, STANCE_WALK) ;

                /* Locate where the target is. */
/*
                ServerGetPlayerLocation(
                    p_creature->target,
                    &playerX,
                    &playerY) ;
*/

                /* Get the angle to this target. */
/*
                angle = MathArcTangent(
                            playerX - ObjectGetX16(p_obj),
                            playerY - ObjectGetY16(p_obj)) ;
*/
                angle = ObjectGetAngle(p_obj) ;

                /* What is the difference to the angle I'm currently */
                /* facing? */
                diffangle = ObjectGetAngle(p_obj) - angle ;
                if (diffangle < 0)
                    absdiffangle = -diffangle ;
                else
                    absdiffangle = diffangle ;

                if (p_creature->goForward == TRUE)  {
                    absdiffangle = 0 ;
                    p_creature->stepAway -= delta ;
                    if (p_creature->stepAway < 0)
                        p_creature->goForward = FALSE ;
                }

                /* Is that a big angle or a small one? */
                if (absdiffangle >= INT_ANGLE_45)  {
                    /* Yeah, a big angle.  Just turn on our heels. */
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, (delta << 8)) ;
                    else
                        ObjectAddAngle(p_obj, -(delta << 8)) ;
                } else {
                    /* Not a big angle, we might as well go ahead */
                    /* and try moving a bit. */

                    /* Turn as needed. */
                    turnangle = (delta<<6) ;
                    if (turnangle > absdiffangle)
                        turnangle = absdiffangle ;
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, turnangle) ;
                    else
                        ObjectAddAngle(p_obj, -turnangle) ;

                    /* How close are we? */
                    distance = CalculateEstimateDistance(
                                   ObjectGetX16(p_obj),
                                   ObjectGetY16(p_obj),
                                   playerX,
                                   playerY) ;

    //                origX = ObjectGetX16(p_obj) ;
    //                origY = ObjectGetX16(p_obj) ;

                    /* Are we close enough to attack? */
                    if (distance < 70)  {
                        /* !!! Attack !!! */
                        ObjectSetStance(p_obj, STANCE_ATTACK) ;
                        ObjectSetAngle(p_obj, angle) ;

                        /* Set the time for pausing on the attack. */
                        p_creature->state = 2 ;
                        p_creature->stateData = 27 ;
                    } else {
                        /* Too far, let's move closer. */
                        ObjectAccelFlat(p_obj, delta, ObjectGetAngle(p_obj)) ;

                        /* See if we are tired of moving and want */
                        /* a new opponent. */
                        if (p_creature->stateData <= newTime)  {
                            /* Look around for opponents. */
                            newTarget = CreatureLookForPlayer(p_obj) ;
                            if (newTarget != 0xFFFF /* -1 */)
                                p_creature->target = newTarget ;
                            p_creature->stateData = newTime+210 ;
                        }
                    }
                }
                break ;
            case 2:   /* Pause after attacking or getting hurt. */
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 10)
                    /* Yes, continue going after target. */
                    p_creature->state = 3 ;
                break ;
            case 3:
                ObjectSetStance(p_obj, STANCE_STAND) ;
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 0)  {
                    /* Yes, continue going after target. */
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;
                }
                break ;
        }
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  CreatureUpdateFlyer                                           */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreatureUpdateFlyer is used to move a creature that flies.            */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_creature *p_creature      -- creature to update                     */
/*                                                                          */
/*    T_word32 delta              -- Amount to move/update creature         */
/*                                                                          */
/*    T_word32 newTime            -- Current time to update                 */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    MathArcTangent                                                        */
/*    CalculateDistance                                                     */
/*    ICreatureSendCreatureAttack                                           */
/*    View3dSetExceptObject                                                 */
/*    View3dMoveTo                                                          */
/*    CreatureLookForPlayer                                                 */
/*    View3dGetFloorAndCeiling                                              */
/*    MapGetFloorWalkingHeight                                              */
/*    ICreatureSendMoveCreaturePacket                                       */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  04/16/95  Created                                                */
/*    LES  09/15/95  Added code to make sure flyer doesn't go through       */
/*                   floor or ceiling.                                      */
/*                                                                          */
/****************************************************************************/

T_void CreatureUpdateFlyer(
           T_3dObject *p_obj,
           T_word32 delta,
           T_word32 newTime)
{
    T_sword16 playerX, playerY, playerHeight ;
    T_word16 angle ;
    T_sword16 diffangle ;
    T_sword16 absdiffangle ;
    T_word16 distance ;
    T_word16 newTarget ;
    T_word16 turnangle ;
    T_creature *p_creature ;
    T_sword16 moveDelta ;
    T_sword32 frontX, frontY ;
    E_Boolean wasBlocked = FALSE ;
    T_sword16 oldZ ;
    T_sword16 newZ ;
    T_sword16 limit ;

    DebugRoutine("CreatureUpdateFlyer") ;
    DebugCheck(p_obj != NULL) ;

    ObjectSetMaxVelocity(p_obj, 24) ;
    ObjectClearMoveFlags(p_obj, OBJMOVE_FLAG_FAST_MOVEMENT) ;

    p_creature = (T_creature *)ObjectGetExtraData(p_obj) ;
//    DebugCheck(p_creature != NULL) ;
    if (p_creature != NULL)  {

        /** Is it time for a noise? **/
        if (TickerGet () > p_creature->nextScream)
        {
           if (p_creature->nextScream == 0)
           {
              /** First time.  Don't sound off, just set the timer. **/
           }
           else
           {
              if (p_creature->state < 2)  {
                  /** Time for a holler. **/
                  ServerAreaSoundGlobal (
                        ObjectGetX16 (p_obj),
                        ObjectGetY16 (p_obj),
                        1000,
                        124);
              }
           }
           /** Set the timer for the next scream. **/
           p_creature->nextScream = TickerGet() + 350 + (rand () % 350);
        }

        /* Make sure our movement doesn't keep pulling it down. */
        ObjectSetMoveFlags(
            p_obj,
            OBJMOVE_FLAG_IGNORE_GRAVITY |
            OBJMOVE_FLAG_IGNORE_FRICTION |
            OBJMOVE_FLAG_FAST_MOVEMENT) ;

        /* Check to see if we still have a target. */
        if (p_creature->target == 0xFFFF /* -1 */)
            /* Nope, go into the mode to get one. */
            p_creature->state = 0 ;

        switch(p_creature->state)  {
            case 0:   /* Initial state -- looking for a target. */
                ObjectSetStance(p_obj, STANCE_STAND) ;
                if ((p_creature->target =
                        CreatureLookForPlayer(p_obj)) != 0xFFFF)  {
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;

                    /* Creature sound that he sees you! */
                    ServerAreaSoundGlobal (
                           ObjectGetX16 (p_obj),
                           ObjectGetY16 (p_obj),
                           1000,
                           120);
                }
                break ;
            case 1:   /* Moving towards opponent state. */
                /* Make us animate as walking. */
                ObjectSetStance(p_obj, STANCE_WALK) ;

                /* First, see if the creature was blocked last time. */
                if (ObjectWasBlocked(p_obj))  {
                    /* We are hitting something, let's turn randomly */
                    /* 45 degrees to one side. */
                    if (p_creature->needTurn == TRUE)  {
                        p_creature->turnWay = TickerGet() & 1 ;
                        p_creature->needTurn = FALSE ;
                    }

                    if (p_creature->turnWay)  {
//                        ObjectAddAngle(p_obj, INT_ANGLE_45) ;
                        ObjectAddAngle(p_obj, (delta<<8)) ;
                    } else {
//                        ObjectAddAngle(p_obj, -INT_ANGLE_45) ;
                        ObjectAddAngle(p_obj, -((T_sword32)(delta<<8))) ;
                    }

                    p_creature->goForward = TRUE ;
                    p_creature->stepAway = 20 ;
                    wasBlocked = TRUE ;

                    /* Make sure future access say we were not blocked. */
                    ObjectClearBlockedFlag(p_obj) ;
                } else {
                    p_creature->needTurn = TRUE ;
                }

                /* Locate where the target is. */
                ServerGetPlayerLocation(
                    p_creature->target,
                    &playerX,
                    &playerY) ;

                ServerGetPlayerHeight(
                    p_creature->target,
                    &playerHeight) ;

                /* Move creature up and down to match player height */
                oldZ = ObjectGetZ16(p_obj) ;
                if (oldZ > playerHeight)  {
                    moveDelta = oldZ - playerHeight ;
                    if (moveDelta > delta)
                        moveDelta = delta ;
                    moveDelta = -moveDelta ;
                } else {
                    moveDelta = playerHeight - oldZ ;
                    if (moveDelta > delta)
                        moveDelta = delta ;
                }

                /* Check to see that the creatures isn't going to */
                /* go through the roof. */
                newZ = oldZ + moveDelta ;
                limit = (ObjectGetHighestPoint(p_obj)>>16) -
                            ObjectGetHeight(p_obj) ;
                if (newZ > limit)
                    newZ = limit ;

                /* Make sure creature doesn't go through floor. */
                limit = (ObjectGetLowestPoint(p_obj)>>16) ;
                if (newZ < limit)
                    newZ = limit ;
                ObjectSetZ16(p_obj, newZ) ;

                /* Get the angle to this target. */
                angle = MathArcTangent(
                            playerX - ObjectGetX16(p_obj),
                            playerY - ObjectGetY16(p_obj)) ;

                /* Face directly. */
//                absdiffangle = 0 ;
//                ObjectSetAngle(p_obj, angle) ;

                /* What is the difference to the angle I'm currently */
                /* facing? */
                diffangle = ObjectGetAngle(p_obj) - angle ;
                if (diffangle < 0)
                    absdiffangle = -diffangle ;
                else
                    absdiffangle = diffangle ;

                if (p_creature->goForward == TRUE)  {
                    absdiffangle = 0 ;
                    p_creature->stepAway -= delta ;
                    if (p_creature->stepAway < 0)
                        p_creature->goForward = FALSE ;
                }

                /* Is that a big angle or a small one? */
                if (absdiffangle >= INT_ANGLE_45)  {
                    /* Yeah, a big angle.  Just turn on our heels. */
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, (delta << 8)) ;
                    else
                        ObjectAddAngle(p_obj, -(delta << 8)) ;
                } else {
                    /* Not a big angle, we might as well go ahead */
                    /* and try moving a bit. */

                    /* Turn as needed. */
/*
                    turnangle = (delta<<6) ;
                    if (turnangle > absdiffangle)
                        turnangle = absdiffangle ;
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, turnangle) ;
                    else
                        ObjectAddAngle(p_obj, -turnangle) ;
*/
                    if (wasBlocked == FALSE)
                        ObjectSetAngle(p_obj, angle) ;

                    /* How close are we? */
                    distance = CalculateEstimateDistance(
                                   ObjectGetX16(p_obj),
                                   ObjectGetY16(p_obj),
                                   playerX,
                                   playerY) ;

    //                origX = ObjectGetX16(p_obj) ;
    //                origY = ObjectGetX16(p_obj) ;

                    /* Are we close enough to attack? */
                    if (distance < 70)  {
                        /* !!! Attack !!! */
                        ObjectSetStance(p_obj, STANCE_ATTACK) ;
                        ObjectSetAngle(p_obj, angle) ;

                        ObjectGetForwardPosition(
                            p_obj,
                            ObjectGetRadius(p_obj) + 20,
                            &frontX,
                            &frontY) ;

                        ServerDamageAt(
                            ObjectGetX(p_obj),
                            ObjectGetY(p_obj),
                            frontX,
                            frontY,
                            0,
                            50) ;

                        /* Set the time for pausing on the attack. */
                        p_creature->state = 2 ;
                        p_creature->stateData = 27 ;
                    } else {
                        /* Too far, let's move closer. */
                        ObjectAccelFlat(p_obj, delta, ObjectGetAngle(p_obj)) ;

                        /* See if we are tired of moving and want */
                        /* a new opponent. */
                        if (p_creature->stateData <= newTime)  {
                            /* Look around for opponents. */
                            newTarget = CreatureLookForPlayer(p_obj) ;
                            if (newTarget != 0xFFFF /* -1 */)
                                p_creature->target = newTarget ;
                            p_creature->stateData = newTime+210 ;
                        }
                    }
                }
                break ;
            case 2:   /* Pause after attacking or getting hurt. */
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 10)
                    /* Yes, continue going after target. */
                    p_creature->state = 3 ;
                break ;
            case 3:
                ObjectSetStance(p_obj, STANCE_STAND) ;
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 0)  {
                    /* Yes, continue going after target. */
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;
                }
                break ;
        }
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  CreatureUpdateGriffon                                         */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreatureUpdateGriffon                                                 */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_creature *p_creature      -- creature to update                     */
/*                                                                          */
/*    T_word32 delta              -- Amount to move/update creature         */
/*                                                                          */
/*    T_word32 newTime            -- Current time to update                 */
/*                                                                          */
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
/*    LES  04/06/96  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void CreatureUpdateGriffon(
           T_3dObject *p_obj,
           T_word32 delta,
           T_word32 newTime)
{
    T_sword16 playerX, playerY, playerHeight ;
    T_word16 angle ;
    T_sword16 diffangle ;
    T_sword16 absdiffangle ;
    T_word16 distance ;
    T_word16 newTarget ;
    T_word16 turnangle ;
    T_creature *p_creature ;
    T_sword16 moveDelta ;
    T_sword32 frontX, frontY ;
    E_Boolean wasBlocked = FALSE ;
    T_sword16 oldZ ;
    T_sword16 newZ ;
    T_sword16 limit ;

    DebugRoutine("CreatureUpdateGriffon") ;
    DebugCheck(p_obj != NULL) ;

    ObjectSetMaxVelocity(p_obj, 24) ;
    ObjectClearMoveFlags(p_obj, OBJMOVE_FLAG_FAST_MOVEMENT) ;

    p_creature = (T_creature *)ObjectGetExtraData(p_obj) ;
//    DebugCheck(p_creature != NULL) ;
    if (p_creature != NULL)  {

        /** Is it time for a noise? **/
        if (TickerGet () > p_creature->nextScream)
        {
           if (p_creature->nextScream == 0)
           {
              /** First time.  Don't sound off, just set the timer. **/
           }
           else
           {
              if (p_creature->state < 2)  {
                  /** Time for a holler. **/
                  ServerAreaSoundGlobal (
                        ObjectGetX16 (p_obj),
                        ObjectGetY16 (p_obj),
                        1000,
                        2111);
              }
           }
           /** Set the timer for the next scream. **/
           p_creature->nextScream = TickerGet() + 350 + (rand () % 350);
        }

        /* Make sure our movement doesn't keep pulling it down. */
        ObjectSetMoveFlags(
            p_obj,
            OBJMOVE_FLAG_IGNORE_GRAVITY |
            OBJMOVE_FLAG_IGNORE_FRICTION |
            OBJMOVE_FLAG_FAST_MOVEMENT) ;

        /* Check to see if we still have a target. */
        if (p_creature->target == 0xFFFF /* -1 */)
            /* Nope, go into the mode to get one. */
            p_creature->state = 0 ;

        switch(p_creature->state)  {
            case 0:   /* Initial state -- looking for a target. */
                ObjectSetStance(p_obj, STANCE_STAND) ;
                if ((p_creature->target =
                        CreatureLookForPlayer(p_obj)) != 0xFFFF)  {
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;

                    /* Creature sound that he sees you! */
                    ServerAreaSoundGlobal (
                           ObjectGetX16 (p_obj),
                           ObjectGetY16 (p_obj),
                           1000,
                           2110);
                }
                break ;
            case 1:   /* Moving towards opponent state. */
                /* Make us animate as walking. */
                ObjectSetStance(p_obj, STANCE_WALK) ;

                /* First, see if the creature was blocked last time. */
                if (ObjectWasBlocked(p_obj))  {
                    /* We are hitting something, let's turn randomly */
                    /* 45 degrees to one side. */
                    if (p_creature->needTurn == TRUE)  {
                        p_creature->turnWay = TickerGet() & 1 ;
                        p_creature->needTurn = FALSE ;
                    }

                    if (p_creature->turnWay)  {
//                        ObjectAddAngle(p_obj, INT_ANGLE_45) ;
                        ObjectAddAngle(p_obj, (delta<<8)) ;
                    } else {
//                        ObjectAddAngle(p_obj, -INT_ANGLE_45) ;
                        ObjectAddAngle(p_obj, -((T_sword32)(delta<<8))) ;
                    }

                    p_creature->goForward = TRUE ;
                    p_creature->stepAway = 20 ;
                    wasBlocked = TRUE ;

                    /* Make sure future access say we were not blocked. */
                    ObjectClearBlockedFlag(p_obj) ;
                } else {
                    p_creature->needTurn = TRUE ;
                }

                /* Locate where the target is. */
                ServerGetPlayerLocation(
                    p_creature->target,
                    &playerX,
                    &playerY) ;

                ServerGetPlayerHeight(
                    p_creature->target,
                    &playerHeight) ;

                /* Move creature up and down to match player height */
                oldZ = ObjectGetZ16(p_obj) ;
                if (oldZ > playerHeight)  {
                    moveDelta = oldZ - playerHeight ;
                    if (moveDelta > delta)
                        moveDelta = delta ;
                    moveDelta = -moveDelta ;
                } else {
                    moveDelta = playerHeight - oldZ ;
                    if (moveDelta > delta)
                        moveDelta = delta ;
                }

                /* Check to see that the creatures isn't going to */
                /* go through the roof. */
                newZ = oldZ + moveDelta ;
                limit = (ObjectGetHighestPoint(p_obj)>>16) -
                            ObjectGetHeight(p_obj) ;
                if (newZ > limit)
                    newZ = limit ;

                /* Make sure creature doesn't go through floor. */
                limit = (ObjectGetLowestPoint(p_obj)>>16) ;
                if (newZ < limit)
                    newZ = limit ;
                ObjectSetZ16(p_obj, newZ) ;

                /* Get the angle to this target. */
                angle = MathArcTangent(
                            playerX - ObjectGetX16(p_obj),
                            playerY - ObjectGetY16(p_obj)) ;

                /* Face directly. */
//                absdiffangle = 0 ;
//                ObjectSetAngle(p_obj, angle) ;

                /* What is the difference to the angle I'm currently */
                /* facing? */
                diffangle = ObjectGetAngle(p_obj) - angle ;
                if (diffangle < 0)
                    absdiffangle = -diffangle ;
                else
                    absdiffangle = diffangle ;

                if (p_creature->goForward == TRUE)  {
                    absdiffangle = 0 ;
                    p_creature->stepAway -= delta ;
                    if (p_creature->stepAway < 0)
                        p_creature->goForward = FALSE ;
                }

                /* Is that a big angle or a small one? */
                if (absdiffangle >= INT_ANGLE_45)  {
                    /* Yeah, a big angle.  Just turn on our heels. */
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, (delta << 8)) ;
                    else
                        ObjectAddAngle(p_obj, -(delta << 8)) ;
                } else {
                    /* Not a big angle, we might as well go ahead */
                    /* and try moving a bit. */

                    /* Turn as needed. */
/*
                    turnangle = (delta<<6) ;
                    if (turnangle > absdiffangle)
                        turnangle = absdiffangle ;
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, turnangle) ;
                    else
                        ObjectAddAngle(p_obj, -turnangle) ;
*/
                    if (wasBlocked == FALSE)
                        ObjectSetAngle(p_obj, angle) ;

                    /* How close are we? */
                    distance = CalculateEstimateDistance(
                                   ObjectGetX16(p_obj),
                                   ObjectGetY16(p_obj),
                                   playerX,
                                   playerY) ;

    //                origX = ObjectGetX16(p_obj) ;
    //                origY = ObjectGetX16(p_obj) ;

                    /* Are we close enough to attack? */
                    if (distance < 70)  {
                        /* !!! Attack !!! */
                        ObjectSetStance(p_obj, STANCE_ATTACK) ;
                        ObjectSetAngle(p_obj, angle) ;

                        ObjectGetForwardPosition(
                            p_obj,
                            ObjectGetRadius(p_obj) + 20,
                            &frontX,
                            &frontY) ;

                        ServerDamageAt(
                            ObjectGetX(p_obj),
                            ObjectGetY(p_obj),
                            frontX,
                            frontY,
                            0,
                            50) ;

                        /* Set the time for pausing on the attack. */
                        p_creature->state = 2 ;
                        p_creature->stateData = 27 ;
                    } else {
                        /* Too far, let's move closer. */
                        ObjectAccelFlat(p_obj, delta, ObjectGetAngle(p_obj)) ;

                        /* See if we are tired of moving and want */
                        /* a new opponent. */
                        if (p_creature->stateData <= newTime)  {
                            /* Look around for opponents. */
                            newTarget = CreatureLookForPlayer(p_obj) ;
                            if (newTarget != 0xFFFF /* -1 */)
                                p_creature->target = newTarget ;
                            p_creature->stateData = newTime+210 ;
                        }
                    }
                }
                break ;
            case 2:   /* Pause after attacking or getting hurt. */
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 10)
                    /* Yes, continue going after target. */
                    p_creature->state = 3 ;
                break ;
            case 3:
                ObjectSetStance(p_obj, STANCE_STAND) ;
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 0)  {
                    /* Yes, continue going after target. */
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;
                }
                break ;
        }
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  CreatureUpdateBanshee                                         */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreatureUpdateBanshee is used to move a creature that flies.          */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_creature *p_creature      -- creature to update                     */
/*                                                                          */
/*    T_word32 delta              -- Amount to move/update creature         */
/*                                                                          */
/*    T_word32 newTime            -- Current time to update                 */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    MathArcTangent                                                        */
/*    CalculateDistance                                                     */
/*    ICreatureSendCreatureAttack                                           */
/*    View3dSetExceptObject                                                 */
/*    View3dMoveTo                                                          */
/*    CreatureLookForPlayer                                                 */
/*    View3dGetFloorAndCeiling                                              */
/*    MapGetFloorWalkingHeight                                              */
/*    ICreatureSendMoveCreaturePacket                                       */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  04/16/95  Created                                                */
/*    LES  09/15/95  Added code to make sure flyer doesn't go through       */
/*                   floor or ceiling.                                      */
/*                                                                          */
/****************************************************************************/

T_void CreatureUpdateBanshee(
           T_3dObject *p_obj,
           T_word32 delta,
           T_word32 newTime)
{
    T_sword16 playerX, playerY, playerHeight ;
    T_word16 angle ;
    T_sword16 diffangle ;
    T_sword16 absdiffangle ;
    T_word16 distance ;
    T_word16 newTarget ;
    T_word16 turnangle ;
    T_creature *p_creature ;
    T_sword16 moveDelta ;
    T_sword32 frontX, frontY ;
    E_Boolean wasBlocked = FALSE ;
    T_sword16 oldZ ;
    T_sword16 newZ ;
    T_sword16 limit ;

    DebugRoutine("CreatureUpdateBanshee") ;
    DebugCheck(p_obj != NULL) ;

    ObjectSetMaxVelocity(p_obj, 12) ;
    ObjectClearMoveFlags(p_obj, OBJMOVE_FLAG_FAST_MOVEMENT) ;

    p_creature = (T_creature *)ObjectGetExtraData(p_obj) ;
//    DebugCheck(p_creature != NULL) ;
    if (p_creature != NULL)  {

        /** Is it time for a noise? **/
        if (TickerGet () > p_creature->nextScream)
        {
           if (p_creature->nextScream == 0)
           {
              /** First time.  Don't sound off, just set the timer. **/
           }
           else
           {
              if (p_creature->state < 2)  {
                  /** Time for a holler. **/
                  ServerAreaSoundGlobal (
                        ObjectGetX16 (p_obj),
                        ObjectGetY16 (p_obj),
                        1000,
                        4004);
              }
           }
           /** Set the timer for the next scream. **/
           p_creature->nextScream = TickerGet() + 350 + (rand () % 350);
        }

        /* Make sure our movement doesn't keep pulling it down. */
        ObjectSetMoveFlags(
            p_obj,
            OBJMOVE_FLAG_IGNORE_GRAVITY |
            OBJMOVE_FLAG_IGNORE_FRICTION |
            OBJMOVE_FLAG_FAST_MOVEMENT) ;

        /* Check to see if we still have a target. */
        if (p_creature->target == 0xFFFF /* -1 */)
            /* Nope, go into the mode to get one. */
            p_creature->state = 0 ;

        switch(p_creature->state)  {
            case 0:   /* Initial state -- looking for a target. */
                ObjectSetStance(p_obj, STANCE_STAND) ;
                if ((p_creature->target =
                        CreatureLookForPlayer(p_obj)) != 0xFFFF)  {
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;

                    /* Creature sound that he sees you! */
/*
                    ServerAreaSoundGlobal (
                           ObjectGetX16 (p_obj),
                           ObjectGetY16 (p_obj),
                           1000,
                           120);
*/
                }
                break ;
            case 1:   /* Moving towards opponent state. */
                /* Make us animate as walking. */
                ObjectSetStance(p_obj, STANCE_WALK) ;

                /* First, see if the creature was blocked last time. */
                if (ObjectWasBlocked(p_obj))  {
                    /* We are hitting something, let's turn randomly */
                    /* 45 degrees to one side. */
                    if (p_creature->needTurn == TRUE)  {
                        p_creature->turnWay = TickerGet() & 1 ;
                        p_creature->needTurn = FALSE ;
                    }

                    if (p_creature->turnWay)  {
//                        ObjectAddAngle(p_obj, INT_ANGLE_45) ;
                        ObjectAddAngle(p_obj, (delta<<8)) ;
                    } else {
//                        ObjectAddAngle(p_obj, -INT_ANGLE_45) ;
                        ObjectAddAngle(p_obj, -((T_sword32)(delta<<8))) ;
                    }

                    p_creature->goForward = TRUE ;
                    p_creature->stepAway = 20 ;
                    wasBlocked = TRUE ;

                    /* Make sure future access say we were not blocked. */
                    ObjectClearBlockedFlag(p_obj) ;
                } else {
                    p_creature->needTurn = TRUE ;
                }

                /* Locate where the target is. */
                ServerGetPlayerLocation(
                    p_creature->target,
                    &playerX,
                    &playerY) ;

                ServerGetPlayerHeight(
                    p_creature->target,
                    &playerHeight) ;

                playerHeight += 25 ;

                /* Move creature up and down to match player height */
                oldZ = ObjectGetZ16(p_obj) ;
                if (oldZ > playerHeight)  {
                    moveDelta = oldZ - playerHeight ;
                    if (moveDelta > delta)
                        moveDelta = delta ;
                    moveDelta = -moveDelta ;
                } else {
                    moveDelta = playerHeight - oldZ ;
                    if (moveDelta > delta)
                        moveDelta = delta ;
                }

                /* Check to see that the creatures isn't going to */
                /* go through the roof. */
                newZ = oldZ + moveDelta ;
                limit = (ObjectGetHighestPoint(p_obj)>>16) -
                            ObjectGetHeight(p_obj) ;
                if (newZ > limit)
                    newZ = limit ;

                /* Make sure creature doesn't go through floor. */
                limit = (ObjectGetLowestPoint(p_obj)>>16) ;
                if (newZ < limit)
                    newZ = limit ;
                ObjectSetZ16(p_obj, newZ) ;

                /* Get the angle to this target. */
                angle = MathArcTangent(
                            playerX - ObjectGetX16(p_obj),
                            playerY - ObjectGetY16(p_obj)) ;

                /* Face directly. */
//                absdiffangle = 0 ;
//                ObjectSetAngle(p_obj, angle) ;

                /* What is the difference to the angle I'm currently */
                /* facing? */
                diffangle = ObjectGetAngle(p_obj) - angle ;
                if (diffangle < 0)
                    absdiffangle = -diffangle ;
                else
                    absdiffangle = diffangle ;

                if (p_creature->goForward == TRUE)  {
                    absdiffangle = 0 ;
                    p_creature->stepAway -= delta ;
                    if (p_creature->stepAway < 0)
                        p_creature->goForward = FALSE ;
                }

                /* Is that a big angle or a small one? */
                if (absdiffangle >= INT_ANGLE_45)  {
                    /* Yeah, a big angle.  Just turn on our heels. */
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, (delta << 8)) ;
                    else
                        ObjectAddAngle(p_obj, -(delta << 8)) ;
                } else {
                    /* Not a big angle, we might as well go ahead */
                    /* and try moving a bit. */

                    /* Turn as needed. */
/*
                    turnangle = (delta<<6) ;
                    if (turnangle > absdiffangle)
                        turnangle = absdiffangle ;
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, turnangle) ;
                    else
                        ObjectAddAngle(p_obj, -turnangle) ;
*/
                    if (wasBlocked == FALSE)
                        ObjectSetAngle(p_obj, angle) ;

                    /* How close are we? */
                    distance = CalculateEstimateDistance(
                                   ObjectGetX16(p_obj),
                                   ObjectGetY16(p_obj),
                                   playerX,
                                   playerY) ;

    //                origX = ObjectGetX16(p_obj) ;
    //                origY = ObjectGetX16(p_obj) ;

                    /* Are we close enough to attack? */
                    if (distance < 60)  {
                        /* !!! Attack !!! */
                        ObjectSetStance(p_obj, STANCE_ATTACK) ;
                        ObjectSetAngle(p_obj, angle) ;

                        ObjectGetForwardPosition(
                            p_obj,
                            ObjectGetRadius(p_obj) + 30,
                            &frontX,
                            &frontY) ;

                        ServerDamageAt(
                            ObjectGetX(p_obj),
                            ObjectGetY(p_obj),
                            frontX,
                            frontY,
                            0,
                            50) ;

                        /* Set the time for pausing on the attack. */
                        p_creature->state = 2 ;
                        p_creature->stateData = 27 ;
                    } else {
                        /* Too far, let's move closer. */
                        ObjectAccelFlat(p_obj, delta, ObjectGetAngle(p_obj)) ;

                        /* See if we are tired of moving and want */
                        /* a new opponent. */
                        if (p_creature->stateData <= newTime)  {
                            /* Look around for opponents. */
                            newTarget = CreatureLookForPlayer(p_obj) ;
                            if (newTarget != 0xFFFF /* -1 */)
                                p_creature->target = newTarget ;
                            p_creature->stateData = newTime+210 ;
                        }
                    }
                }
                break ;
            case 2:   /* Pause after attacking or getting hurt. */
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 10)
                    /* Yes, continue going after target. */
                    p_creature->state = 3 ;
                break ;
            case 3:
                ObjectSetStance(p_obj, STANCE_STAND) ;
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 0)  {
                    /* Yes, continue going after target. */
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;
                }
                break ;
        }
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  CreatureUpdateWalker                                          */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreatureUpdateWalker is used to move a creature that walks.           */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_creature *p_creature      -- creature to update                     */
/*                                                                          */
/*    T_word32 delta              -- Amount to move/update creature         */
/*                                                                          */
/*    T_word32 newTime            -- Current time to update                 */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    MathArcTangent                                                        */
/*    CalculateDistance                                                     */
/*    ICreatureSendCreatureAttack                                           */
/*    View3dSetExceptObject                                                 */
/*    View3dMoveTo                                                          */
/*    CreatureLookForPlayer                                                 */
/*    View3dGetFloorAndCeiling                                              */
/*    MapGetFloorWalkingHeight                                              */
/*    ICreatureSendMoveCreaturePacket                                       */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  04/16/95  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void CreatureUpdateWalker(
           T_3dObject *p_obj,
           T_word32 delta,
           T_word32 newTime)
{
    T_sword16 playerX, playerY ;
    T_word16 angle ;
    T_sword16 diffangle ;
    T_sword16 absdiffangle ;
    T_word16 distance ;
    T_word16 newTarget ;
    T_word16 turnangle ;
    T_creature *p_creature ;
    T_sword32 frontX, frontY ;

    DebugRoutine("CreatureUpdateWalker") ;
    DebugCheck(p_obj != NULL) ;

    ObjectSetMaxVelocity(p_obj, 8) ;

    p_creature = (T_creature *)ObjectGetExtraData(p_obj) ;
//    DebugCheck(p_creature != NULL) ;
    if (p_creature != NULL)  {

        /** Is it time for a noise? **/
        if (TickerGet () > p_creature->nextScream)
        {
           if (p_creature->nextScream == 0)
           {
              /** First time.  Don't sound off, just set the timer. **/
           }
           else
           {
              /** Time for a holler. **/
              ServerAreaSoundGlobal (
                     ObjectGetX16 (p_obj),
                     ObjectGetY16 (p_obj),
                     1000,
                     4005);
           }
           /** Set the timer for the next scream. **/
           p_creature->nextScream = TickerGet() + 700 + (rand () % 700);
        }

        /* Check to see if we still have a target. */
        if (p_creature->target == 0xFFFF /* -1 */)
            /* Nope, go into the mode to get one. */
            p_creature->state = 0 ;

        switch(p_creature->state)  {
            case 0:   /* Initial state -- looking for a target. */
                ObjectSetStance(p_obj, STANCE_STAND) ;
                if ((p_creature->target =
                        CreatureLookForPlayer(p_obj)) != 0xFFFF)  {
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;

                    /* Creature sound that he sees you! */
                    ServerAreaSoundGlobal (
                           ObjectGetX16 (p_obj),
                           ObjectGetY16 (p_obj),
                           1000,
                           4001);
                }
                break ;
            case 1:   /* Moving towards opponent state. */
                /* Make us animate as walking. */
                ObjectSetStance(p_obj, STANCE_WALK) ;

                /* First, see if the creature was blocked last time. */
                if (ObjectWasBlocked(p_obj))  {
                    /* We are hitting something, let's turn randomly */
                    /* 45 degrees to one side. */
                    if (p_creature->goForward != TRUE)
                        p_creature->turnWay = TickerGet() & 1 ;

                    if (p_creature->turnWay)  {
                        ObjectAddAngle(p_obj, INT_ANGLE_45) ;
                    } else {
                        ObjectAddAngle(p_obj, -INT_ANGLE_45) ;
                    }

                    p_creature->goForward = TRUE ;
                    p_creature->stepAway = 70 ;

                    /* Make sure future access say we were not blocked. */
                    ObjectClearBlockedFlag(p_obj) ;
                }

                /* Locate where the target is. */
                ServerGetPlayerLocation(
                    p_creature->target,
                    &playerX,
                    &playerY) ;

                /* Get the angle to this target. */
                angle = MathArcTangent(
                            playerX - ObjectGetX16(p_obj),
                            playerY - ObjectGetY16(p_obj)) ;

                /* What is the difference to the angle I'm currently */
                /* facing? */
                diffangle = ObjectGetAngle(p_obj) - angle ;
                if (diffangle < 0)
                    absdiffangle = -diffangle ;
                else
                    absdiffangle = diffangle ;

                if (p_creature->goForward == TRUE)  {
                    absdiffangle = 0 ;
                    p_creature->stepAway -= delta ;
                    if (p_creature->stepAway < 0)
                        p_creature->goForward = FALSE ;
                }

                /* Is that a big angle or a small one? */
                if (absdiffangle >= INT_ANGLE_45)  {
                    /* Yeah, a big angle.  Just turn on our heels. */
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, (delta << 8)) ;
                    else
                        ObjectAddAngle(p_obj, -(delta << 8)) ;
                } else {
                    /* Not a big angle, we might as well go ahead */
                    /* and try moving a bit. */

                    /* Turn as needed. */
                    turnangle = (delta<<6) ;
                    if (turnangle > absdiffangle)
                        turnangle = absdiffangle ;
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, turnangle) ;
                    else
                        ObjectAddAngle(p_obj, -turnangle) ;

                    /* How close are we? */
                    distance = CalculateEstimateDistance(
                                   ObjectGetX16(p_obj),
                                   ObjectGetY16(p_obj),
                                   playerX,
                                   playerY) ;

    //                origX = ObjectGetX16(p_obj) ;
    //                origY = ObjectGetX16(p_obj) ;

                    /* Are we close enough to attack? */
                    if (distance < (ObjectGetRadius(p_obj) + 35))  {
                        /* !!! Attack !!! */
                        ObjectSetStance(p_obj, STANCE_ATTACK) ;
                        ObjectSetAngle(p_obj, angle) ;

                        ObjectGetForwardPosition(
                            p_obj,
                            ObjectGetRadius(p_obj) + 20,
                            &frontX,
                            &frontY) ;

                        ServerDamageAt(
                            ObjectGetX(p_obj),
                            ObjectGetY(p_obj),
                            frontX,
                            frontY,
                            0,
                            100) ;

                        /* Set the time for pausing on the attack. */
                        p_creature->state = 2 ;
                        p_creature->stateData = 50 ;
                    } else {
                        /* Too far, let's move closer. */
                        ObjectAccelFlat(p_obj, delta, ObjectGetAngle(p_obj)) ;

                        /* See if we are tired of moving and want */
                        /* a new opponent. */
                        if (p_creature->stateData <= newTime)  {
                            /* Look around for opponents. */
                            newTarget = CreatureLookForPlayer(p_obj) ;
                            if (newTarget != 0xFFFF /* -1 */)
                                p_creature->target = newTarget ;
                            p_creature->stateData = newTime+210 ;
                        }
                    }
                }
                break ;
            case 2:   /* Pause after attacking or getting hurt. */
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 10)
                    /* Yes, continue going after target. */
                    p_creature->state = 3 ;
                break ;
            case 3:
                ObjectSetStance(p_obj, STANCE_STAND) ;
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 0)  {
                    /* Yes, continue going after target. */
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;
                }
                break ;
        }
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  CreatureUpdateDruid                                           */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreatureUpdateDruid                                                   */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_creature *p_creature      -- creature to update                     */
/*                                                                          */
/*    T_word32 delta              -- Amount to move/update creature         */
/*                                                                          */
/*    T_word32 newTime            -- Current time to update                 */
/*                                                                          */
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
/*    LES  04/05/96  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void CreatureUpdateDruid(
           T_3dObject *p_obj,
           T_word32 delta,
           T_word32 newTime)
{
    T_sword16 playerX, playerY ;
    T_word16 angle ;
    T_sword16 diffangle ;
    T_sword16 absdiffangle ;
    T_word16 distance ;
    T_word16 newTarget ;
    T_word16 turnangle ;
    T_creature *p_creature ;
    T_sword32 frontX, frontY ;

    DebugRoutine("CreatureUpdateDruid") ;
    DebugCheck(p_obj != NULL) ;

    ObjectSetMaxVelocity(p_obj, 8) ;

    p_creature = (T_creature *)ObjectGetExtraData(p_obj) ;
//    DebugCheck(p_creature != NULL) ;
    if (p_creature != NULL)  {

        /** Is it time for a noise? **/
        if (TickerGet () > p_creature->nextScream)
        {
           if (p_creature->nextScream == 0)
           {
              /** First time.  Don't sound off, just set the timer. **/
           }
           else
           {
              /** Time for a holler. **/
/*
              ServerAreaSoundGlobal (
                     ObjectGetX16 (p_obj),
                     ObjectGetY16 (p_obj),
                     1000,
                     94);
*/
           }
           /** Set the timer for the next scream. **/
           p_creature->nextScream = TickerGet() + 700 + (rand () % 700);
        }

        /* Check to see if we still have a target. */
        if (p_creature->target == 0xFFFF /* -1 */)
            /* Nope, go into the mode to get one. */
            p_creature->state = 0 ;

        switch(p_creature->state)  {
            case 0:   /* Initial state -- looking for a target. */
                ObjectSetStance(p_obj, STANCE_STAND) ;
                if ((p_creature->target =
                        CreatureLookForPlayer(p_obj)) != 0xFFFF)  {
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;

                    /* Creature sound that he sees you! */
/*
                    ServerAreaSoundGlobal (
                           ObjectGetX16 (p_obj),
                           ObjectGetY16 (p_obj),
                           1000,
                           90);
*/
                }
                break ;
            case 1:   /* Moving towards opponent state. */
                /* Make us animate as walking. */
                ObjectSetStance(p_obj, STANCE_WALK) ;

                /* First, see if the creature was blocked last time. */
                if (ObjectWasBlocked(p_obj))  {
                    /* We are hitting something, let's turn randomly */
                    /* 45 degrees to one side. */
                    if (p_creature->goForward != TRUE)
                        p_creature->turnWay = TickerGet() & 1 ;

                    if (p_creature->turnWay)  {
                        ObjectAddAngle(p_obj, INT_ANGLE_45) ;
                    } else {
                        ObjectAddAngle(p_obj, -INT_ANGLE_45) ;
                    }

                    p_creature->goForward = TRUE ;
                    p_creature->stepAway = 70 ;

                    /* Make sure future access say we were not blocked. */
                    ObjectClearBlockedFlag(p_obj) ;
                }

                /* Locate where the target is. */
                ServerGetPlayerLocation(
                    p_creature->target,
                    &playerX,
                    &playerY) ;

                /* Get the angle to this target. */
                angle = MathArcTangent(
                            playerX - ObjectGetX16(p_obj),
                            playerY - ObjectGetY16(p_obj)) ;

                /* What is the difference to the angle I'm currently */
                /* facing? */
                diffangle = ObjectGetAngle(p_obj) - angle ;
                if (diffangle < 0)
                    absdiffangle = -diffangle ;
                else
                    absdiffangle = diffangle ;

                if (p_creature->goForward == TRUE)  {
                    absdiffangle = 0 ;
                    p_creature->stepAway -= delta ;
                    if (p_creature->stepAway < 0)
                        p_creature->goForward = FALSE ;
                }

                /* Is that a big angle or a small one? */
                if (absdiffangle >= INT_ANGLE_45)  {
                    /* Yeah, a big angle.  Just turn on our heels. */
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, (delta << 8)) ;
                    else
                        ObjectAddAngle(p_obj, -(delta << 8)) ;
                } else {
                    /* Not a big angle, we might as well go ahead */
                    /* and try moving a bit. */

                    /* Turn as needed. */
                    turnangle = (delta<<6) ;
                    if (turnangle > absdiffangle)
                        turnangle = absdiffangle ;
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, turnangle) ;
                    else
                        ObjectAddAngle(p_obj, -turnangle) ;

                    /* How close are we? */
                    distance = CalculateEstimateDistance(
                                   ObjectGetX16(p_obj),
                                   ObjectGetY16(p_obj),
                                   playerX,
                                   playerY) ;

    //                origX = ObjectGetX16(p_obj) ;
    //                origY = ObjectGetX16(p_obj) ;

                    /* Are we close enough to attack? */
                    if (distance < (ObjectGetRadius(p_obj) + 35))  {
                        /* !!! Attack !!! */
                        ObjectSetStance(p_obj, STANCE_ATTACK) ;
                        ObjectSetAngle(p_obj, angle) ;

                        ObjectGetForwardPosition(
                            p_obj,
                            ObjectGetRadius(p_obj) + 20,
                            &frontX,
                            &frontY) ;

                        ServerDamageAt(
                            ObjectGetX(p_obj),
                            ObjectGetY(p_obj),
                            frontX,
                            frontY,
                            0,
                            100) ;

                        /* Set the time for pausing on the attack. */
                        p_creature->state = 2 ;
                        p_creature->stateData = 50 ;
                    } else {
                        /* Too far, let's move closer. */
                        ObjectAccelFlat(p_obj, delta, ObjectGetAngle(p_obj)) ;

                        /* See if we are tired of moving and want */
                        /* a new opponent. */
                        if (p_creature->stateData <= newTime)  {
                            /* Look around for opponents. */
                            newTarget = CreatureLookForPlayer(p_obj) ;
                            if (newTarget != 0xFFFF /* -1 */)
                                p_creature->target = newTarget ;
                            p_creature->stateData = newTime+210 ;
                        }
                    }
                }
                break ;
            case 2:   /* Pause after attacking or getting hurt. */
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 10)
                    /* Yes, continue going after target. */
                    p_creature->state = 3 ;
                break ;
            case 3:
                ObjectSetStance(p_obj, STANCE_STAND) ;
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 0)  {
                    /* Yes, continue going after target. */
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;
                }
                break ;
        }
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  CreatureUpdateElf                                             */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreatureUpdateElf                                                     */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_creature *p_creature      -- creature to update                     */
/*                                                                          */
/*    T_word32 delta              -- Amount to move/update creature         */
/*                                                                          */
/*    T_word32 newTime            -- Current time to update                 */
/*                                                                          */
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
/*    LES  04/05/96  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void CreatureUpdateElf(
           T_3dObject *p_obj,
           T_word32 delta,
           T_word32 newTime)
{
    T_sword16 playerX, playerY ;
    T_word16 angle ;
    T_sword16 diffangle ;
    T_sword16 absdiffangle ;
    T_word16 distance ;
    T_word16 newTarget ;
    T_word16 turnangle ;
    T_creature *p_creature ;
    T_sword32 frontX, frontY ;

    DebugRoutine("CreatureUpdateElf") ;
    DebugCheck(p_obj != NULL) ;

    ObjectSetMaxVelocity(p_obj, 8) ;

    p_creature = (T_creature *)ObjectGetExtraData(p_obj) ;
//    DebugCheck(p_creature != NULL) ;
    if (p_creature != NULL)  {

        /** Is it time for a noise? **/
        if (TickerGet () > p_creature->nextScream)
        {
           if (p_creature->nextScream == 0)
           {
              /** First time.  Don't sound off, just set the timer. **/
           }
           else
           {
              /** Time for a holler. **/
/*
              ServerAreaSoundGlobal (
                     ObjectGetX16 (p_obj),
                     ObjectGetY16 (p_obj),
                     1000,
                     94);
*/
           }
           /** Set the timer for the next scream. **/
           p_creature->nextScream = TickerGet() + 700 + (rand () % 700);
        }

        /* Check to see if we still have a target. */
        if (p_creature->target == 0xFFFF /* -1 */)
            /* Nope, go into the mode to get one. */
            p_creature->state = 0 ;

        switch(p_creature->state)  {
            case 0:   /* Initial state -- looking for a target. */
                ObjectSetStance(p_obj, STANCE_STAND) ;
                if ((p_creature->target =
                        CreatureLookForPlayer(p_obj)) != 0xFFFF)  {
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;

                    /* Creature sound that he sees you! */
                    ServerAreaSoundGlobal (
                           ObjectGetX16 (p_obj),
                           ObjectGetY16 (p_obj),
                           1000,
                           5023);
                }
                break ;
            case 1:   /* Moving towards opponent state. */
                /* Make us animate as walking. */
                ObjectSetStance(p_obj, STANCE_WALK) ;

                /* First, see if the creature was blocked last time. */
                if (ObjectWasBlocked(p_obj))  {
                    /* We are hitting something, let's turn randomly */
                    /* 45 degrees to one side. */
                    if (p_creature->goForward != TRUE)
                        p_creature->turnWay = TickerGet() & 1 ;

                    if (p_creature->turnWay)  {
                        ObjectAddAngle(p_obj, INT_ANGLE_45) ;
                    } else {
                        ObjectAddAngle(p_obj, -INT_ANGLE_45) ;
                    }

                    p_creature->goForward = TRUE ;
                    p_creature->stepAway = 70 ;

                    /* Make sure future access say we were not blocked. */
                    ObjectClearBlockedFlag(p_obj) ;
                }

                /* Locate where the target is. */
                ServerGetPlayerLocation(
                    p_creature->target,
                    &playerX,
                    &playerY) ;

                /* Get the angle to this target. */
                angle = MathArcTangent(
                            playerX - ObjectGetX16(p_obj),
                            playerY - ObjectGetY16(p_obj)) ;

                /* What is the difference to the angle I'm currently */
                /* facing? */
                diffangle = ObjectGetAngle(p_obj) - angle ;
                if (diffangle < 0)
                    absdiffangle = -diffangle ;
                else
                    absdiffangle = diffangle ;

                if (p_creature->goForward == TRUE)  {
                    absdiffangle = 0 ;
                    p_creature->stepAway -= delta ;
                    if (p_creature->stepAway < 0)
                        p_creature->goForward = FALSE ;
                }

                /* Is that a big angle or a small one? */
                if (absdiffangle >= INT_ANGLE_45)  {
                    /* Yeah, a big angle.  Just turn on our heels. */
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, (delta << 8)) ;
                    else
                        ObjectAddAngle(p_obj, -(delta << 8)) ;
                } else {
                    /* Not a big angle, we might as well go ahead */
                    /* and try moving a bit. */

                    /* Turn as needed. */
                    turnangle = (delta<<6) ;
                    if (turnangle > absdiffangle)
                        turnangle = absdiffangle ;
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, turnangle) ;
                    else
                        ObjectAddAngle(p_obj, -turnangle) ;

                    /* How close are we? */
                    distance = CalculateEstimateDistance(
                                   ObjectGetX16(p_obj),
                                   ObjectGetY16(p_obj),
                                   playerX,
                                   playerY) ;

    //                origX = ObjectGetX16(p_obj) ;
    //                origY = ObjectGetX16(p_obj) ;

                    /* Are we close enough to attack? */
                    if (distance < (ObjectGetRadius(p_obj) + 35))  {
                        /* !!! Attack !!! */
                        ObjectSetStance(p_obj, STANCE_ATTACK) ;
                        ObjectSetAngle(p_obj, angle) ;

                        ObjectGetForwardPosition(
                            p_obj,
                            ObjectGetRadius(p_obj) + 20,
                            &frontX,
                            &frontY) ;

                        ServerDamageAt(
                            ObjectGetX(p_obj),
                            ObjectGetY(p_obj),
                            frontX,
                            frontY,
                            0,
                            100) ;

                        /* Set the time for pausing on the attack. */
                        p_creature->state = 2 ;
                        p_creature->stateData = 50 ;
                    } else {
                        /* Too far, let's move closer. */
                        ObjectAccelFlat(p_obj, delta, ObjectGetAngle(p_obj)) ;

                        /* See if we are tired of moving and want */
                        /* a new opponent. */
                        if (p_creature->stateData <= newTime)  {
                            /* Look around for opponents. */
                            newTarget = CreatureLookForPlayer(p_obj) ;
                            if (newTarget != 0xFFFF /* -1 */)
                                p_creature->target = newTarget ;
                            p_creature->stateData = newTime+210 ;
                        }
                    }
                }
                break ;
            case 2:   /* Pause after attacking or getting hurt. */
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 10)
                    /* Yes, continue going after target. */
                    p_creature->state = 3 ;
                break ;
            case 3:
                ObjectSetStance(p_obj, STANCE_STAND) ;
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 0)  {
                    /* Yes, continue going after target. */
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;
                }
                break ;
        }
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  CreatureUpdateLich                                            */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreatureUpdateLich                                                    */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_creature *p_creature      -- creature to update                     */
/*                                                                          */
/*    T_word32 delta              -- Amount to move/update creature         */
/*                                                                          */
/*    T_word32 newTime            -- Current time to update                 */
/*                                                                          */
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
/*    LES  04/05/96  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void CreatureUpdateLich(
           T_3dObject *p_obj,
           T_word32 delta,
           T_word32 newTime)
{
    T_sword16 playerX, playerY ;
    T_word16 angle ;
    T_sword16 diffangle ;
    T_sword16 absdiffangle ;
    T_word16 distance ;
    T_word16 newTarget ;
    T_word16 turnangle ;
    T_creature *p_creature ;
    T_sword32 frontX, frontY ;

    DebugRoutine("CreatureUpdateLich") ;
    DebugCheck(p_obj != NULL) ;

    ObjectSetMaxVelocity(p_obj, 8) ;

    p_creature = (T_creature *)ObjectGetExtraData(p_obj) ;
//    DebugCheck(p_creature != NULL) ;
    if (p_creature != NULL)  {

        /** Is it time for a noise? **/
        if (TickerGet () > p_creature->nextScream)
        {
           if (p_creature->nextScream == 0)
           {
              /** First time.  Don't sound off, just set the timer. **/
           }
           else
           {
              /** Time for a holler. **/
/*
              ServerAreaSoundGlobal (
                     ObjectGetX16 (p_obj),
                     ObjectGetY16 (p_obj),
                     1000,
                     94);
*/
           }
           /** Set the timer for the next scream. **/
           p_creature->nextScream = TickerGet() + 700 + (rand () % 700);
        }

        /* Check to see if we still have a target. */
        if (p_creature->target == 0xFFFF /* -1 */)
            /* Nope, go into the mode to get one. */
            p_creature->state = 0 ;

        switch(p_creature->state)  {
            case 0:   /* Initial state -- looking for a target. */
                ObjectSetStance(p_obj, STANCE_STAND) ;
                if ((p_creature->target =
                        CreatureLookForPlayer(p_obj)) != 0xFFFF)  {
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;

                    /* Creature sound that he sees you! */
                    ServerAreaSoundGlobal (
                           ObjectGetX16 (p_obj),
                           ObjectGetY16 (p_obj),
                           1000,
                           2009);
                }
                break ;
            case 1:   /* Moving towards opponent state. */
                /* Make us animate as walking. */
                ObjectSetStance(p_obj, STANCE_WALK) ;

                /* First, see if the creature was blocked last time. */
                if (ObjectWasBlocked(p_obj))  {
                    /* We are hitting something, let's turn randomly */
                    /* 45 degrees to one side. */
                    if (p_creature->goForward != TRUE)
                        p_creature->turnWay = TickerGet() & 1 ;

                    if (p_creature->turnWay)  {
                        ObjectAddAngle(p_obj, INT_ANGLE_45) ;
                    } else {
                        ObjectAddAngle(p_obj, -INT_ANGLE_45) ;
                    }

                    p_creature->goForward = TRUE ;
                    p_creature->stepAway = 70 ;

                    /* Make sure future access say we were not blocked. */
                    ObjectClearBlockedFlag(p_obj) ;
                }

                /* Locate where the target is. */
                ServerGetPlayerLocation(
                    p_creature->target,
                    &playerX,
                    &playerY) ;

                /* Get the angle to this target. */
                angle = MathArcTangent(
                            playerX - ObjectGetX16(p_obj),
                            playerY - ObjectGetY16(p_obj)) ;

                /* What is the difference to the angle I'm currently */
                /* facing? */
                diffangle = ObjectGetAngle(p_obj) - angle ;
                if (diffangle < 0)
                    absdiffangle = -diffangle ;
                else
                    absdiffangle = diffangle ;

                if (p_creature->goForward == TRUE)  {
                    absdiffangle = 0 ;
                    p_creature->stepAway -= delta ;
                    if (p_creature->stepAway < 0)
                        p_creature->goForward = FALSE ;
                }

                /* Is that a big angle or a small one? */
                if (absdiffangle >= INT_ANGLE_45)  {
                    /* Yeah, a big angle.  Just turn on our heels. */
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, (delta << 8)) ;
                    else
                        ObjectAddAngle(p_obj, -(delta << 8)) ;
                } else {
                    /* Not a big angle, we might as well go ahead */
                    /* and try moving a bit. */

                    /* Turn as needed. */
                    turnangle = (delta<<6) ;
                    if (turnangle > absdiffangle)
                        turnangle = absdiffangle ;
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, turnangle) ;
                    else
                        ObjectAddAngle(p_obj, -turnangle) ;

                    /* How close are we? */
                    distance = CalculateEstimateDistance(
                                   ObjectGetX16(p_obj),
                                   ObjectGetY16(p_obj),
                                   playerX,
                                   playerY) ;

    //                origX = ObjectGetX16(p_obj) ;
    //                origY = ObjectGetX16(p_obj) ;

                    /* Are we close enough to attack? */
                    if (distance < (ObjectGetRadius(p_obj) + 35))  {
                        /* !!! Attack !!! */
                        ObjectSetStance(p_obj, STANCE_ATTACK) ;
                        ObjectSetAngle(p_obj, angle) ;

                        ObjectGetForwardPosition(
                            p_obj,
                            ObjectGetRadius(p_obj) + 20,
                            &frontX,
                            &frontY) ;

                        ServerDamageAt(
                            ObjectGetX(p_obj),
                            ObjectGetY(p_obj),
                            frontX,
                            frontY,
                            0,
                            100) ;

                        /* Set the time for pausing on the attack. */
                        p_creature->state = 2 ;
                        p_creature->stateData = 50 ;
                    } else {
                        /* Too far, let's move closer. */
                        ObjectAccelFlat(p_obj, delta, ObjectGetAngle(p_obj)) ;

                        /* See if we are tired of moving and want */
                        /* a new opponent. */
                        if (p_creature->stateData <= newTime)  {
                            /* Look around for opponents. */
                            newTarget = CreatureLookForPlayer(p_obj) ;
                            if (newTarget != 0xFFFF /* -1 */)
                                p_creature->target = newTarget ;
                            p_creature->stateData = newTime+210 ;
                        }
                    }
                }
                break ;
            case 2:   /* Pause after attacking or getting hurt. */
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 10)
                    /* Yes, continue going after target. */
                    p_creature->state = 3 ;
                break ;
            case 3:
                ObjectSetStance(p_obj, STANCE_STAND) ;
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 0)  {
                    /* Yes, continue going after target. */
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;
                }
                break ;
        }
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  CreatureUpdateBronzeBarbarian                                 */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreatureUpdateBronzeBarbarian                                         */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_creature *p_creature      -- creature to update                     */
/*                                                                          */
/*    T_word32 delta              -- Amount to move/update creature         */
/*                                                                          */
/*    T_word32 newTime            -- Current time to update                 */
/*                                                                          */
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
/*    LES  04/05/96  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void CreatureUpdateBronzeBarbarian(
           T_3dObject *p_obj,
           T_word32 delta,
           T_word32 newTime)
{
    T_sword16 playerX, playerY ;
    T_word16 angle ;
    T_sword16 diffangle ;
    T_sword16 absdiffangle ;
    T_word16 distance ;
    T_word16 newTarget ;
    T_word16 turnangle ;
    T_creature *p_creature ;
    T_sword32 frontX, frontY ;

    DebugRoutine("CreatureUpdateBronzeBarbarian") ;
    DebugCheck(p_obj != NULL) ;

    ObjectSetMaxVelocity(p_obj, 8) ;

    p_creature = (T_creature *)ObjectGetExtraData(p_obj) ;
//    DebugCheck(p_creature != NULL) ;
    if (p_creature != NULL)  {

        /** Is it time for a noise? **/
        if (TickerGet () > p_creature->nextScream)
        {
           if (p_creature->nextScream == 0)
           {
              /** First time.  Don't sound off, just set the timer. **/
           }
           else
           {
              /** Time for a holler. **/
/*
              ServerAreaSoundGlobal (
                     ObjectGetX16 (p_obj),
                     ObjectGetY16 (p_obj),
                     1000,
                     94);
*/
           }
           /** Set the timer for the next scream. **/
           p_creature->nextScream = TickerGet() + 700 + (rand () % 700);
        }

        /* Check to see if we still have a target. */
        if (p_creature->target == 0xFFFF /* -1 */)
            /* Nope, go into the mode to get one. */
            p_creature->state = 0 ;

        switch(p_creature->state)  {
            case 0:   /* Initial state -- looking for a target. */
                ObjectSetStance(p_obj, STANCE_STAND) ;
                if ((p_creature->target =
                        CreatureLookForPlayer(p_obj)) != 0xFFFF)  {
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;

                    /* Creature sound that he sees you! */
                    ServerAreaSoundGlobal (
                           ObjectGetX16 (p_obj),
                           ObjectGetY16 (p_obj),
                           1000,
                           90);
                }
                break ;
            case 1:   /* Moving towards opponent state. */
                /* Make us animate as walking. */
                ObjectSetStance(p_obj, STANCE_WALK) ;

                /* First, see if the creature was blocked last time. */
                if (ObjectWasBlocked(p_obj))  {
                    /* We are hitting something, let's turn randomly */
                    /* 45 degrees to one side. */
                    if (p_creature->goForward != TRUE)
                        p_creature->turnWay = TickerGet() & 1 ;

                    if (p_creature->turnWay)  {
                        ObjectAddAngle(p_obj, INT_ANGLE_45) ;
                    } else {
                        ObjectAddAngle(p_obj, -INT_ANGLE_45) ;
                    }

                    p_creature->goForward = TRUE ;
                    p_creature->stepAway = 70 ;

                    /* Make sure future access say we were not blocked. */
                    ObjectClearBlockedFlag(p_obj) ;
                }

                /* Locate where the target is. */
                ServerGetPlayerLocation(
                    p_creature->target,
                    &playerX,
                    &playerY) ;

                /* Get the angle to this target. */
                angle = MathArcTangent(
                            playerX - ObjectGetX16(p_obj),
                            playerY - ObjectGetY16(p_obj)) ;

                /* What is the difference to the angle I'm currently */
                /* facing? */
                diffangle = ObjectGetAngle(p_obj) - angle ;
                if (diffangle < 0)
                    absdiffangle = -diffangle ;
                else
                    absdiffangle = diffangle ;

                if (p_creature->goForward == TRUE)  {
                    absdiffangle = 0 ;
                    p_creature->stepAway -= delta ;
                    if (p_creature->stepAway < 0)
                        p_creature->goForward = FALSE ;
                }

                /* Is that a big angle or a small one? */
                if (absdiffangle >= INT_ANGLE_45)  {
                    /* Yeah, a big angle.  Just turn on our heels. */
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, (delta << 8)) ;
                    else
                        ObjectAddAngle(p_obj, -(delta << 8)) ;
                } else {
                    /* Not a big angle, we might as well go ahead */
                    /* and try moving a bit. */

                    /* Turn as needed. */
                    turnangle = (delta<<6) ;
                    if (turnangle > absdiffangle)
                        turnangle = absdiffangle ;
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, turnangle) ;
                    else
                        ObjectAddAngle(p_obj, -turnangle) ;

                    /* How close are we? */
                    distance = CalculateEstimateDistance(
                                   ObjectGetX16(p_obj),
                                   ObjectGetY16(p_obj),
                                   playerX,
                                   playerY) ;

    //                origX = ObjectGetX16(p_obj) ;
    //                origY = ObjectGetX16(p_obj) ;

                    /* Are we close enough to attack? */
                    if (distance < (ObjectGetRadius(p_obj) + 35))  {
                        /* !!! Attack !!! */
                        ObjectSetStance(p_obj, STANCE_ATTACK) ;
                        ObjectSetAngle(p_obj, angle) ;

                        ObjectGetForwardPosition(
                            p_obj,
                            ObjectGetRadius(p_obj) + 20,
                            &frontX,
                            &frontY) ;

                        ServerDamageAt(
                            ObjectGetX(p_obj),
                            ObjectGetY(p_obj),
                            frontX,
                            frontY,
                            0,
                            100) ;

                        /* Set the time for pausing on the attack. */
                        p_creature->state = 2 ;
                        p_creature->stateData = 50 ;
                    } else {
                        /* Too far, let's move closer. */
                        ObjectAccelFlat(p_obj, delta, ObjectGetAngle(p_obj)) ;

                        /* See if we are tired of moving and want */
                        /* a new opponent. */
                        if (p_creature->stateData <= newTime)  {
                            /* Look around for opponents. */
                            newTarget = CreatureLookForPlayer(p_obj) ;
                            if (newTarget != 0xFFFF /* -1 */)
                                p_creature->target = newTarget ;
                            p_creature->stateData = newTime+210 ;
                        }
                    }
                }
                break ;
            case 2:   /* Pause after attacking or getting hurt. */
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 10)
                    /* Yes, continue going after target. */
                    p_creature->state = 3 ;
                break ;
            case 3:
                ObjectSetStance(p_obj, STANCE_STAND) ;
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 0)  {
                    /* Yes, continue going after target. */
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;
                }
                break ;
        }
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  CreatureUpdateBlack Barbarian                                 */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreatureUpdateBlack Barbarian                                         */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_creature *p_creature      -- creature to update                     */
/*                                                                          */
/*    T_word32 delta              -- Amount to move/update creature         */
/*                                                                          */
/*    T_word32 newTime            -- Current time to update                 */
/*                                                                          */
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
/*    LES  04/05/96  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void CreatureUpdateBlackBarbarian(
           T_3dObject *p_obj,
           T_word32 delta,
           T_word32 newTime)
{
    T_sword16 playerX, playerY ;
    T_word16 angle ;
    T_sword16 diffangle ;
    T_sword16 absdiffangle ;
    T_word16 distance ;
    T_word16 newTarget ;
    T_word16 turnangle ;
    T_creature *p_creature ;
    T_sword32 frontX, frontY ;

    DebugRoutine("CreatureUpdateBlackBarbarian") ;
    DebugCheck(p_obj != NULL) ;

    ObjectSetMaxVelocity(p_obj, 8) ;

    p_creature = (T_creature *)ObjectGetExtraData(p_obj) ;
//    DebugCheck(p_creature != NULL) ;
    if (p_creature != NULL)  {

        /** Is it time for a noise? **/
        if (TickerGet () > p_creature->nextScream)
        {
           if (p_creature->nextScream == 0)
           {
              /** First time.  Don't sound off, just set the timer. **/
           }
           else
           {
              /** Time for a holler. **/
              ServerAreaSoundGlobal (
                     ObjectGetX16 (p_obj),
                     ObjectGetY16 (p_obj),
                     1000,
                     5001);
           }
           /** Set the timer for the next scream. **/
           p_creature->nextScream = TickerGet() + 700 + (rand () % 700);
        }

        /* Check to see if we still have a target. */
        if (p_creature->target == 0xFFFF /* -1 */)
            /* Nope, go into the mode to get one. */
            p_creature->state = 0 ;

        switch(p_creature->state)  {
            case 0:   /* Initial state -- looking for a target. */
                ObjectSetStance(p_obj, STANCE_STAND) ;
                if ((p_creature->target =
                        CreatureLookForPlayer(p_obj)) != 0xFFFF)  {
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;

                    /* Creature sound that he sees you! */
                    ServerAreaSoundGlobal (
                           ObjectGetX16 (p_obj),
                           ObjectGetY16 (p_obj),
                           1000,
                           5007);
                }
                break ;
            case 1:   /* Moving towards opponent state. */
                /* Make us animate as walking. */
                ObjectSetStance(p_obj, STANCE_WALK) ;

                /* First, see if the creature was blocked last time. */
                if (ObjectWasBlocked(p_obj))  {
                    /* We are hitting something, let's turn randomly */
                    /* 45 degrees to one side. */
                    if (p_creature->goForward != TRUE)
                        p_creature->turnWay = TickerGet() & 1 ;

                    if (p_creature->turnWay)  {
                        ObjectAddAngle(p_obj, INT_ANGLE_45) ;
                    } else {
                        ObjectAddAngle(p_obj, -INT_ANGLE_45) ;
                    }

                    p_creature->goForward = TRUE ;
                    p_creature->stepAway = 70 ;

                    /* Make sure future access say we were not blocked. */
                    ObjectClearBlockedFlag(p_obj) ;
                }

                /* Locate where the target is. */
                ServerGetPlayerLocation(
                    p_creature->target,
                    &playerX,
                    &playerY) ;

                /* Get the angle to this target. */
                angle = MathArcTangent(
                            playerX - ObjectGetX16(p_obj),
                            playerY - ObjectGetY16(p_obj)) ;

                /* What is the difference to the angle I'm currently */
                /* facing? */
                diffangle = ObjectGetAngle(p_obj) - angle ;
                if (diffangle < 0)
                    absdiffangle = -diffangle ;
                else
                    absdiffangle = diffangle ;

                if (p_creature->goForward == TRUE)  {
                    absdiffangle = 0 ;
                    p_creature->stepAway -= delta ;
                    if (p_creature->stepAway < 0)
                        p_creature->goForward = FALSE ;
                }

                /* Is that a big angle or a small one? */
                if (absdiffangle >= INT_ANGLE_45)  {
                    /* Yeah, a big angle.  Just turn on our heels. */
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, (delta << 8)) ;
                    else
                        ObjectAddAngle(p_obj, -(delta << 8)) ;
                } else {
                    /* Not a big angle, we might as well go ahead */
                    /* and try moving a bit. */

                    /* Turn as needed. */
                    turnangle = (delta<<6) ;
                    if (turnangle > absdiffangle)
                        turnangle = absdiffangle ;
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, turnangle) ;
                    else
                        ObjectAddAngle(p_obj, -turnangle) ;

                    /* How close are we? */
                    distance = CalculateEstimateDistance(
                                   ObjectGetX16(p_obj),
                                   ObjectGetY16(p_obj),
                                   playerX,
                                   playerY) ;

    //                origX = ObjectGetX16(p_obj) ;
    //                origY = ObjectGetX16(p_obj) ;

                    /* Are we close enough to attack? */
                    if (distance < (ObjectGetRadius(p_obj) + 35))  {
                        /* !!! Attack !!! */
                        ObjectSetStance(p_obj, STANCE_ATTACK) ;
                        ObjectSetAngle(p_obj, angle) ;

                        ObjectGetForwardPosition(
                            p_obj,
                            ObjectGetRadius(p_obj) + 20,
                            &frontX,
                            &frontY) ;

                        ServerDamageAt(
                            ObjectGetX(p_obj),
                            ObjectGetY(p_obj),
                            frontX,
                            frontY,
                            0,
                            100) ;

                        /* Set the time for pausing on the attack. */
                        p_creature->state = 2 ;
                        p_creature->stateData = 50 ;
                    } else {
                        /* Too far, let's move closer. */
                        ObjectAccelFlat(p_obj, delta, ObjectGetAngle(p_obj)) ;

                        /* See if we are tired of moving and want */
                        /* a new opponent. */
                        if (p_creature->stateData <= newTime)  {
                            /* Look around for opponents. */
                            newTarget = CreatureLookForPlayer(p_obj) ;
                            if (newTarget != 0xFFFF /* -1 */)
                                p_creature->target = newTarget ;
                            p_creature->stateData = newTime+210 ;
                        }
                    }
                }
                break ;
            case 2:   /* Pause after attacking or getting hurt. */
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 10)
                    /* Yes, continue going after target. */
                    p_creature->state = 3 ;
                break ;
            case 3:
                ObjectSetStance(p_obj, STANCE_STAND) ;
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 0)  {
                    /* Yes, continue going after target. */
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;
                }
                break ;
        }
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  CreatureUpdateWhiteBarbarian                                 */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreatureUpdateWhite Barbarian                                         */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_creature *p_creature      -- creature to update                     */
/*                                                                          */
/*    T_word32 delta              -- Amount to move/update creature         */
/*                                                                          */
/*    T_word32 newTime            -- Current time to update                 */
/*                                                                          */
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
/*    LES  04/05/96  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void CreatureUpdateWhiteBarbarian(
           T_3dObject *p_obj,
           T_word32 delta,
           T_word32 newTime)
{
    T_sword16 playerX, playerY ;
    T_word16 angle ;
    T_sword16 diffangle ;
    T_sword16 absdiffangle ;
    T_word16 distance ;
    T_word16 newTarget ;
    T_word16 turnangle ;
    T_creature *p_creature ;
    T_sword32 frontX, frontY ;

    DebugRoutine("CreatureUpdateWhiteBarbarian") ;
    DebugCheck(p_obj != NULL) ;

    ObjectSetMaxVelocity(p_obj, 8) ;

    p_creature = (T_creature *)ObjectGetExtraData(p_obj) ;
//    DebugCheck(p_creature != NULL) ;
    if (p_creature != NULL)  {

        /** Is it time for a noise? **/
        if (TickerGet () > p_creature->nextScream)
        {
           if (p_creature->nextScream == 0)
           {
              /** First time.  Don't sound off, just set the timer. **/
           }
           else
           {
              /** Time for a holler. **/
              ServerAreaSoundGlobal (
                     ObjectGetX16 (p_obj),
                     ObjectGetY16 (p_obj),
                     1000,
                     5012);
           }
           /** Set the timer for the next scream. **/
           p_creature->nextScream = TickerGet() + 700 + (rand () % 700);
        }

        /* Check to see if we still have a target. */
        if (p_creature->target == 0xFFFF /* -1 */)
            /* Nope, go into the mode to get one. */
            p_creature->state = 0 ;

        switch(p_creature->state)  {
            case 0:   /* Initial state -- looking for a target. */
                ObjectSetStance(p_obj, STANCE_STAND) ;
                if ((p_creature->target =
                        CreatureLookForPlayer(p_obj)) != 0xFFFF)  {
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;

                    /* Creature sound that he sees you! */
                    ServerAreaSoundGlobal (
                           ObjectGetX16 (p_obj),
                           ObjectGetY16 (p_obj),
                           1000,
                           5008);
                }
                break ;
            case 1:   /* Moving towards opponent state. */
                /* Make us animate as walking. */
                ObjectSetStance(p_obj, STANCE_WALK) ;

                /* First, see if the creature was blocked last time. */
                if (ObjectWasBlocked(p_obj))  {
                    /* We are hitting something, let's turn randomly */
                    /* 45 degrees to one side. */
                    if (p_creature->goForward != TRUE)
                        p_creature->turnWay = TickerGet() & 1 ;

                    if (p_creature->turnWay)  {
                        ObjectAddAngle(p_obj, INT_ANGLE_45) ;
                    } else {
                        ObjectAddAngle(p_obj, -INT_ANGLE_45) ;
                    }

                    p_creature->goForward = TRUE ;
                    p_creature->stepAway = 70 ;

                    /* Make sure future access say we were not blocked. */
                    ObjectClearBlockedFlag(p_obj) ;
                }

                /* Locate where the target is. */
                ServerGetPlayerLocation(
                    p_creature->target,
                    &playerX,
                    &playerY) ;

                /* Get the angle to this target. */
                angle = MathArcTangent(
                            playerX - ObjectGetX16(p_obj),
                            playerY - ObjectGetY16(p_obj)) ;

                /* What is the difference to the angle I'm currently */
                /* facing? */
                diffangle = ObjectGetAngle(p_obj) - angle ;
                if (diffangle < 0)
                    absdiffangle = -diffangle ;
                else
                    absdiffangle = diffangle ;

                if (p_creature->goForward == TRUE)  {
                    absdiffangle = 0 ;
                    p_creature->stepAway -= delta ;
                    if (p_creature->stepAway < 0)
                        p_creature->goForward = FALSE ;
                }

                /* Is that a big angle or a small one? */
                if (absdiffangle >= INT_ANGLE_45)  {
                    /* Yeah, a big angle.  Just turn on our heels. */
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, (delta << 8)) ;
                    else
                        ObjectAddAngle(p_obj, -(delta << 8)) ;
                } else {
                    /* Not a big angle, we might as well go ahead */
                    /* and try moving a bit. */

                    /* Turn as needed. */
                    turnangle = (delta<<6) ;
                    if (turnangle > absdiffangle)
                        turnangle = absdiffangle ;
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, turnangle) ;
                    else
                        ObjectAddAngle(p_obj, -turnangle) ;

                    /* How close are we? */
                    distance = CalculateEstimateDistance(
                                   ObjectGetX16(p_obj),
                                   ObjectGetY16(p_obj),
                                   playerX,
                                   playerY) ;

    //                origX = ObjectGetX16(p_obj) ;
    //                origY = ObjectGetX16(p_obj) ;

                    /* Are we close enough to attack? */
                    if (distance < (ObjectGetRadius(p_obj) + 35))  {
                        /* !!! Attack !!! */
                        ObjectSetStance(p_obj, STANCE_ATTACK) ;
                        ObjectSetAngle(p_obj, angle) ;

                        ObjectGetForwardPosition(
                            p_obj,
                            ObjectGetRadius(p_obj) + 20,
                            &frontX,
                            &frontY) ;

                        ServerDamageAt(
                            ObjectGetX(p_obj),
                            ObjectGetY(p_obj),
                            frontX,
                            frontY,
                            0,
                            100) ;

                        /* Set the time for pausing on the attack. */
                        p_creature->state = 2 ;
                        p_creature->stateData = 50 ;
                    } else {
                        /* Too far, let's move closer. */
                        ObjectAccelFlat(p_obj, delta, ObjectGetAngle(p_obj)) ;

                        /* See if we are tired of moving and want */
                        /* a new opponent. */
                        if (p_creature->stateData <= newTime)  {
                            /* Look around for opponents. */
                            newTarget = CreatureLookForPlayer(p_obj) ;
                            if (newTarget != 0xFFFF /* -1 */)
                                p_creature->target = newTarget ;
                            p_creature->stateData = newTime+210 ;
                        }
                    }
                }
                break ;
            case 2:   /* Pause after attacking or getting hurt. */
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 10)
                    /* Yes, continue going after target. */
                    p_creature->state = 3 ;
                break ;
            case 3:
                ObjectSetStance(p_obj, STANCE_STAND) ;
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 0)  {
                    /* Yes, continue going after target. */
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;
                }
                break ;
        }
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  CreatureUpdateHydra                                           */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreatureUpdateHydra  is used to move a creature that walks.           */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_creature *p_creature      -- creature to update                     */
/*                                                                          */
/*    T_word32 delta              -- Amount to move/update creature         */
/*                                                                          */
/*    T_word32 newTime            -- Current time to update                 */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    MathArcTangent                                                        */
/*    CalculateDistance                                                     */
/*    ICreatureSendCreatureAttack                                           */
/*    View3dSetExceptObject                                                 */
/*    View3dMoveTo                                                          */
/*    CreatureLookForPlayer                                                 */
/*    View3dGetFloorAndCeiling                                              */
/*    MapGetFloorWalkingHeight                                              */
/*    ICreatureSendMoveCreaturePacket                                       */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  04/16/95  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void CreatureUpdateHydra(
           T_3dObject *p_obj,
           T_word32 delta,
           T_word32 newTime)
{
    T_sword16 playerX, playerY ;
    T_word16 angle ;
    T_sword16 diffangle ;
    T_sword16 absdiffangle ;
    T_word16 distance ;
    T_word16 newTarget ;
    T_word16 turnangle ;
    T_creature *p_creature ;
    T_sword32 frontX, frontY ;

    DebugRoutine("CreatureUpdateHydra") ;
    DebugCheck(p_obj != NULL) ;

    ObjectSetMaxVelocity(p_obj, 8) ;
/* The dragon is invincible. */
ObjectSetHealth(p_obj, 30000) ;

    p_creature = (T_creature *)ObjectGetExtraData(p_obj) ;
//    DebugCheck(p_creature != NULL) ;
    if (p_creature != NULL)  {

        /** Is it time for a noise? **/
        if (TickerGet () > p_creature->nextScream)
        {
           if (p_creature->nextScream == 0)
           {
              /** First time.  Don't sound off, just set the timer. **/
           }
           else
           {
              /** Time for a holler. **/
              ServerAreaSoundGlobal (
                     ObjectGetX16 (p_obj),
                     ObjectGetY16 (p_obj),
                     1000,
                     4030);
           }
           /** Set the timer for the next scream. **/
           p_creature->nextScream = TickerGet() + 700 + (rand () % 700);
        }

        /* Check to see if we still have a target. */
        if (p_creature->target == 0xFFFF /* -1 */)
            /* Nope, go into the mode to get one. */
            p_creature->state = 0 ;

        switch(p_creature->state)  {
            case 0:   /* Initial state -- looking for a target. */
                ObjectSetStance(p_obj, STANCE_STAND) ;
                if ((p_creature->target =
                        CreatureLookForPlayer(p_obj)) != 0xFFFF)  {
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;

                    /* Creature sound that he sees you! */
                    ServerAreaSoundGlobal (
                           ObjectGetX16 (p_obj),
                           ObjectGetY16 (p_obj),
                           1000,
                           2116);
                }
                break ;
            case 1:   /* Moving towards opponent state. */
                /* Make us animate as walking. */
                ObjectSetStance(p_obj, STANCE_WALK) ;

                /* First, see if the creature was blocked last time. */
                if (ObjectWasBlocked(p_obj))  {
                    /* We are hitting something, let's turn randomly */
                    /* 45 degrees to one side. */
                    if (p_creature->goForward != TRUE)
                        p_creature->turnWay = TickerGet() & 1 ;

                    if (p_creature->turnWay)  {
                        ObjectAddAngle(p_obj, INT_ANGLE_45) ;
                    } else {
                        ObjectAddAngle(p_obj, -INT_ANGLE_45) ;
                    }

                    p_creature->goForward = TRUE ;
                    p_creature->stepAway = 70 ;

                    /* Make sure future access say we were not blocked. */
                    ObjectClearBlockedFlag(p_obj) ;
                }

                /* Locate where the target is. */
                ServerGetPlayerLocation(
                    p_creature->target,
                    &playerX,
                    &playerY) ;

                /* Get the angle to this target. */
                angle = MathArcTangent(
                            playerX - ObjectGetX16(p_obj),
                            playerY - ObjectGetY16(p_obj)) ;

                /* What is the difference to the angle I'm currently */
                /* facing? */
                diffangle = ObjectGetAngle(p_obj) - angle ;
                if (diffangle < 0)
                    absdiffangle = -diffangle ;
                else
                    absdiffangle = diffangle ;

                if (p_creature->goForward == TRUE)  {
                    absdiffangle = 0 ;
                    p_creature->stepAway -= delta ;
                    if (p_creature->stepAway < 0)
                        p_creature->goForward = FALSE ;
                }

                /* Is that a big angle or a small one? */
                if (absdiffangle >= INT_ANGLE_45)  {
                    /* Yeah, a big angle.  Just turn on our heels. */
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, (delta << 8)) ;
                    else
                        ObjectAddAngle(p_obj, -(delta << 8)) ;
                } else {
                    /* Not a big angle, we might as well go ahead */
                    /* and try moving a bit. */

                    /* Turn as needed. */
                    turnangle = (delta<<6) ;
                    if (turnangle > absdiffangle)
                        turnangle = absdiffangle ;
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, turnangle) ;
                    else
                        ObjectAddAngle(p_obj, -turnangle) ;

                    /* How close are we? */
                    distance = CalculateEstimateDistance(
                                   ObjectGetX16(p_obj),
                                   ObjectGetY16(p_obj),
                                   playerX,
                                   playerY) ;

    //                origX = ObjectGetX16(p_obj) ;
    //                origY = ObjectGetX16(p_obj) ;

                    /* Are we close enough to attack? */
                    if (distance < (ObjectGetRadius(p_obj) + 35))  {
                        /* !!! Attack !!! */
                        ObjectSetStance(p_obj, STANCE_ATTACK) ;
                        ObjectSetAngle(p_obj, angle) ;

                        ObjectGetForwardPosition(
                            p_obj,
                            ObjectGetRadius(p_obj) + 20,
                            &frontX,
                            &frontY) ;

                        ServerDamageAt(
                            ObjectGetX(p_obj),
                            ObjectGetY(p_obj),
                            frontX,
                            frontY,
                            0,
                            100) ;

                        /* Set the time for pausing on the attack. */
                        p_creature->state = 2 ;
                        p_creature->stateData = 50 ;
                    } else {
                        /* Too far, let's move closer. */
                        ObjectAccelFlat(p_obj, delta, ObjectGetAngle(p_obj)) ;

                        /* See if we are tired of moving and want */
                        /* a new opponent. */
                        if (p_creature->stateData <= newTime)  {
                            /* Look around for opponents. */
                            newTarget = CreatureLookForPlayer(p_obj) ;
                            if (newTarget != 0xFFFF /* -1 */)
                                p_creature->target = newTarget ;
                            p_creature->stateData = newTime+210 ;
                        }
                    }
                }
                break ;
            case 2:   /* Pause after attacking or getting hurt. */
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 10)
                    /* Yes, continue going after target. */
                    p_creature->state = 3 ;
                break ;
            case 3:
                ObjectSetStance(p_obj, STANCE_STAND) ;
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 0)  {
                    /* Yes, continue going after target. */
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;
                }
                break ;
        }
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  CreatureTakeDamage                                            */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreatureTakeDamage does damage to an intelligent object (creature)    */
/*  and checks to see if it dies or is just hurt.                           */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_3dObject *p_obj           -- Creature to take damage.               */
/*                                                                          */
/*    T_word32 damage             -- Amount of damage done                  */
/*                                                                          */
/*    T_byte8 type                -- Type   of damage done                  */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  07/11/95  Created                                                */
/*    LES  03/19/95  Added parameter 'type'                                 */
/*                                                                          */
/****************************************************************************/

T_void CreatureTakeDamage(
           T_3dObject *p_obj,
           T_word32 damage,
           T_byte8 type)
{
    T_word16 health ;
    T_creature *p_creature ;

    DebugRoutine("CreatureTakeDamage") ;
    DebugCheck(p_obj != NULL) ;

    p_creature = ObjectGetExtraData(p_obj) ;
    if (p_creature != NULL)  {
        health = ObjectGetHealth(p_obj) ;

        if (damage >= health)  {
            /* Its dead now. */
            ObjectSetStance(p_obj, STANCE_DIE) ;
            ObjectSetHealth(p_obj, 0) ;
//            ObjectStopMoving(p_obj) ;

            /* Stop thinking about the creature by removing its extra */
            /* data. */
//            MemFree(p_creature) ;
//            ObjectSetExtraData(p_obj, NULL) ;

            ObjectRemoveScript(p_obj) ;
//            ObjectSetScript(p_obj, 0) ;

            /* Make sure gravity takes effect. */
            ObjectClearMoveFlags(
                p_obj,
                OBJMOVE_FLAG_IGNORE_GRAVITY |
                OBJMOVE_FLAG_IGNORE_FRICTION |
                OBJMOVE_FLAG_FAST_MOVEMENT) ;
        } else {
            /* Ouch, that hurt. */
            ObjectSetStance(p_obj, STANCE_HURT) ;
            ObjectSetHealth(p_obj, health - damage) ;
            p_creature->state = 2 ;
            p_creature->stateData = 35 ;
        }
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  CreatureUpdateDragon                                          */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreatureUpdateDragon is used to move the dragon creature and shoot.   */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_creature *p_creature      -- creature to update                     */
/*                                                                          */
/*    T_word32 delta              -- Amount to move/update creature         */
/*                                                                          */
/*    T_word32 newTime            -- Current time to update                 */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    MathArcTangent                                                        */
/*    CalculateDistance                                                     */
/*    ICreatureSendCreatureAttack                                           */
/*    View3dSetExceptObject                                                 */
/*    View3dMoveTo                                                          */
/*    CreatureLookForPlayer                                                 */
/*    View3dGetFloorAndCeiling                                              */
/*    MapGetFloorWalkingHeight                                              */
/*    ICreatureSendMoveCreaturePacket                                       */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  07/25/95  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void CreatureUpdateDragon(
           T_3dObject *p_obj,
           T_word32 delta,
           T_word32 newTime)
{
    T_sword16 playerX, playerY ;
    T_word16 angle ;
    T_sword16 diffangle ;
    T_sword16 absdiffangle ;
    T_word16 distance ;
    T_word16 newTarget ;
    T_word16 turnangle ;
    T_creature *p_creature ;
    T_sword32 frontX, frontY ;
    T_3dObject *p_target ;

    DebugRoutine("CreatureUpdateDragon") ;
    DebugCheck(p_obj != NULL) ;

    ObjectSetMaxVelocity(p_obj, 8) ;

    ObjectClearMoveFlags(p_obj, OBJMOVE_FLAG_FAST_MOVEMENT) ;

/* The dragon is invincible. */
ObjectSetHealth(p_obj, 30000) ;

    p_creature = (T_creature *)ObjectGetExtraData(p_obj) ;
//    DebugCheck(p_creature != NULL) ;
    if (p_creature != NULL)  {

        /** Is it time for a noise? **/
        if (TickerGet () > p_creature->nextScream)
        {
           if (p_creature->nextScream == 0)
           {
              /** First time.  Don't sound off, just set the timer. **/
           }
           else
           {
              /** Time for a holler. **/
              ServerAreaSoundGlobal (
                     ObjectGetX16 (p_obj),
                     ObjectGetY16 (p_obj),
                     1000,
                     114);
           }
           /** Set the timer for the next scream. **/
           p_creature->nextScream = TickerGet() + 350 + (rand () % 350);
        }

        /* Check to see if we still have a target. */
        if (p_creature->target == 0xFFFF /* -1 */)
            /* Nope, go into the mode to get one. */
            p_creature->state = 0 ;

        switch(p_creature->state)  {
            case 0:   /* Initial state -- looking for a target. */
                ObjectSetStance(p_obj, STANCE_STAND) ;
                if ((p_creature->target =
                        CreatureLookForPlayer(p_obj)) != 0xFFFF)  {
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;

                    /* Creature sound that he sees you! */
                    ServerAreaSoundGlobal (
                           ObjectGetX16 (p_obj),
                           ObjectGetY16 (p_obj),
                           1000,
                           110);
                }
                break ;
            case 1:   /* Moving towards opponent state. */
                /* Make us animate as walking. */
                ObjectSetStance(p_obj, STANCE_WALK) ;

                /* First, see if the creature was blocked last time. */
                if (ObjectWasBlocked(p_obj))  {
                    /* We are hitting something, let's turn randomly */
                    /* 45 degrees to one side. */
                    if (p_creature->goForward != TRUE)
                        p_creature->turnWay = TickerGet() & 1 ;

                    if (p_creature->turnWay)  {
                        ObjectAddAngle(p_obj, INT_ANGLE_45) ;
                    } else {
                        ObjectAddAngle(p_obj, -INT_ANGLE_45) ;
                    }

                    p_creature->goForward = TRUE ;
                    p_creature->stepAway = 70 ;

                    /* Make sure future access say we were not blocked. */
                    ObjectClearBlockedFlag(p_obj) ;
                }

                /* Locate where the target is. */
                ServerGetPlayerLocation(
                    p_creature->target,
                    &playerX,
                    &playerY) ;

                /* Get the angle to this target. */
                angle = MathArcTangent(
                            playerX - ObjectGetX16(p_obj),
                            playerY - ObjectGetY16(p_obj)) ;

                /* What is the difference to the angle I'm currently */
                /* facing? */
                diffangle = ObjectGetAngle(p_obj) - angle ;
                if (diffangle < 0)
                    absdiffangle = -diffangle ;
                else
                    absdiffangle = diffangle ;

                if (p_creature->goForward == TRUE)  {
                    absdiffangle = 0 ;
                    p_creature->stepAway -= delta ;
                    if (p_creature->stepAway < 0)
                        p_creature->goForward = FALSE ;
                }

                /* Is that a big angle or a small one? */
                if (absdiffangle >= INT_ANGLE_45)  {
                    /* Yeah, a big angle.  Just turn on our heels. */
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, (delta << 8)) ;
                    else
                        ObjectAddAngle(p_obj, -(delta << 8)) ;
                } else {
                    /* Not a big angle, we might as well go ahead */
                    /* and try moving a bit. */

                    /* Turn as needed. */
                    turnangle = (delta<<6) ;
                    if (turnangle > absdiffangle)
                        turnangle = absdiffangle ;
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, turnangle) ;
                    else
                        ObjectAddAngle(p_obj, -turnangle) ;

                    /* How close are we? */
                    distance = CalculateEstimateDistance(
                                   ObjectGetX16(p_obj),
                                   ObjectGetY16(p_obj),
                                   playerX,
                                   playerY) ;

                    /* Are we close enough to attack? */
                    if (distance < 100)  {
                        /* !!! Attack !!! */
                        ObjectSetStance(p_obj, STANCE_ATTACK) ;
                        ObjectSetAngle(p_obj, angle) ;

                        ObjectGetForwardPosition(
                            p_obj,
                            110,
                            &frontX,
                            &frontY) ;

                        ServerDamageAt(
                            ObjectGetX(p_obj),
                            ObjectGetY(p_obj),
                            frontX,
                            frontY,
                            0,
                            200) ;

                        /* Set the time for pausing on the attack. */
                        p_creature->state = 2 ;
                        p_creature->stateData = 27 ;
                    } else if ((distance < 2000) &&
                               (TickerGet() > p_creature->nextAttack))  {
                        /* Too far to hit, but close enough to shoot a */
                        /* fireball. */
                        ObjectSetStance(p_obj, STANCE_ATTACK) ;

                        p_target = NULL ;
                        if (p_creature->target != 0xFFFF /* -1 */)
                            p_target =
                                ServerGetPlayerObject(p_creature->target) ;

                        /* Request that this creature shoots a fireball. */
                        ICreatureShootFireball(p_obj, p_target) ;

                        /* Compute when we can shoot again. */
                        /* We'll say one fireball per 2 seconds. */
                        p_creature->nextAttack =
                            TickerGet() + 140 + (rand() % 140) ;

                        /* Set the time for pausing on the attack. */
                        p_creature->state = 2 ;
                        p_creature->stateData = 27 ;
                    } else {
                        /* Too far, let's move closer. */
                        ObjectAccelFlat(p_obj, delta, ObjectGetAngle(p_obj)) ;

                        /* See if we are tired of moving and want */
                        /* a new opponent. */
                        if (p_creature->stateData <= newTime)  {
                            /* Look around for opponents. */
                            newTarget = CreatureLookForPlayer(p_obj) ;
                            if (newTarget != 0xFFFF /* -1 */)
                                p_creature->target = newTarget ;
                            p_creature->stateData = newTime+210 ;
                        }
                    }
                }
                break ;
            case 2:   /* Pause after attacking or getting hurt. */
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 10)
                    /* Yes, continue going after target. */
                    p_creature->state = 3 ;
                break ;
            case 3:
                ObjectSetStance(p_obj, STANCE_STAND) ;
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 0)  {
                    /* Yes, continue going after target. */
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;
                }
                break ;
        }
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  ICreatureShootFireball             * INTERNAL *               */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreatureShootFireball creates a fireball object and requests that the */
/*  server add it to the game.                                              */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_3dObject *p_creatureObj   -- Creature's object that is shooting     */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    TickerGet                                                             */
/*    ObjectCreate                                                          */
/*    PlayerGetX                                                            */
/*    PlayerGetY                                                            */
/*    ViewActiveObject                                                      */
/*    ObjectDeclareMoveable                                                 */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  07/25/95  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void ICreatureShootFireball(T_3dObject *p_creatureObj, T_3dObject *target)
{
    T_sword32 x, y ;
    T_sword16 height ;
    T_sword16 heightTarget ;
    T_sword16 tx, ty ;
    T_sword32 distance ;
    T_word16 angle ;
    T_3dObject *p_obj;
    T_sword32 deltaHeight;
    T_word32 objectRadius;

    DebugRoutine("ICreatureShootFireball") ;

//ObjectPrint(stdout, target) ;
    /* Let's make a fireball. */
    p_obj = ObjectCreate() ;
    DebugCheck(p_obj != NULL) ;

    /* Set up the shot and who to hit. */
    if (target != NULL)  {
        /* Use the given target. */
        ServerShootProjectile(
            p_creatureObj,
            ObjectGetServerId(target),
            36,
            30) ;
    } else {
        /* Use no particular target. */
        ServerShootProjectile(
            p_creatureObj,
            0,
            36,
            30) ;
    }

    DebugEnd() ;

    return ;

/***************************** OLD METHOD *******************************/
//    ObjectSetType(p_obj, OBJECT_TYPE_FIREBALL) ;
    ObjectSetType(p_obj, 36) ;

    /** I need to place the object's center a distance **/
    /** away from me equal to the sum of his and my radii. **/
    objectRadius = ObjectGetRadius(p_obj);
    objectRadius += ObjectGetRadius(p_creatureObj) ;
    objectRadius += objectRadius >> 1;

    ObjectGetForwardPosition(
         p_creatureObj,
         objectRadius,
         &x,
         &y) ;

    /** And his initial Z should be roughly at my belly. **/
    ObjectSetZ16 (p_obj, ObjectGetZ16 (p_creatureObj) + ((ObjectGetHeight(p_creatureObj)*3)/4));

    /* Check if there is a target. */
    if (target == NULL)  {
        /* There is none, just shoot straight. */
        deltaHeight = 0 ;
    } else  {
        /* Find where the target is located. */
        tx = ObjectGetX16(target) ;
        ty = ObjectGetY16(target) ;

        /* How far is it? */
        distance = CalculateDistance(x >> 16, y >> 16, tx, ty) ;
        /* How high is the target? */
        heightTarget = ObjectGetMiddleHeight(target) ;

        /* Calculate the steps necessary to draw a straight */
        /* line to the target. */
        deltaHeight = (((T_sword32)(heightTarget - height))<<16) / distance ;
        deltaHeight *= 40 ;
        /* Don't allow more than 45 degrees up. */
        if (deltaHeight >= 0x320000)
            deltaHeight = 0x320000 ;

        /* Don't allow more than 45 degrees down. */
        if (deltaHeight <= -0x320000)
            deltaHeight = -0x320000 ;
    }

    ObjectSetZVel (p_obj, deltaHeight);

    /* Declare the angle we are going to be shooting at. */
    angle = ObjectGetAngle(p_creatureObj) ;
    ObjectSetAngle(p_obj, angle) ;

    /* Set the velocity of the fireball. */
    ObjectSetAngularVelocity(
        p_obj,
        angle,
        50) ;

    /* Can the object exist there? **/
    if (ObjectCheckIfCollide (p_obj, x, y, ObjectGetZ (p_obj)))
    {
        ObjectDestroy (p_obj);
    }
    else
    {
        /** Finally, request the server to add it to the **/
        /** world.  NOTE that this command removes it from **/
        /** ours. **/

        ObjectSetX (p_obj, x);
        ObjectSetY (p_obj, y);

        /* have the object added to everyone's world. */
        ServerAddObjectGlobal(p_obj);
    }


/***************************** OLD METHOD *******************************/
    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  CreatureUpdateApe                                             */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreatureUpdateApe    is used to move a creature that walks.           */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_creature *p_creature      -- creature to update                     */
/*                                                                          */
/*    T_word32 delta              -- Amount to move/update creature         */
/*                                                                          */
/*    T_word32 newTime            -- Current time to update                 */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    MathArcTangent                                                        */
/*    CalculateDistance                                                     */
/*    ICreatureSendCreatureAttack                                           */
/*    View3dSetExceptObject                                                 */
/*    View3dMoveTo                                                          */
/*    CreatureLookForPlayer                                                 */
/*    View3dGetFloorAndCeiling                                              */
/*    MapGetFloorWalkingHeight                                              */
/*    ICreatureSendMoveCreaturePacket                                       */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  05/25/95  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void CreatureUpdateApe(
           T_3dObject *p_obj,
           T_word32 delta,
           T_word32 newTime)
{
    T_sword16 playerX, playerY ;
    T_word16 angle ;
    T_sword16 diffangle ;
    T_sword16 absdiffangle ;
    T_word16 distance ;
    T_word16 newTarget ;
    T_word16 turnangle ;
    T_creature *p_creature ;
    T_sword32 frontX, frontY ;

    DebugRoutine("CreatureUpdateApe") ;
    DebugCheck(p_obj != NULL) ;

    ObjectSetMaxVelocity(p_obj, 8) ;

    p_creature = (T_creature *)ObjectGetExtraData(p_obj) ;
//    DebugCheck(p_creature != NULL) ;
    if (p_creature != NULL)  {

        /** Is it time for a noise? **/
        if (TickerGet () > p_creature->nextScream)
        {
           if (p_creature->nextScream == 0)
           {
              /** First time.  Don't sound off, just set the timer. **/
           }
           else
           {
              /** Time for a holler. **/
              ServerAreaSoundGlobal (
                     ObjectGetX16 (p_obj),
                     ObjectGetY16 (p_obj),
                     1000,
                     84);
           }
           /** Set the timer for the next scream. **/
           p_creature->nextScream = TickerGet() + 350 + (rand () % 350);
        }

        /* Check to see if we still have a target. */
        if (p_creature->target == 0xFFFF /* -1 */)
            /* Nope, go into the mode to get one. */
            p_creature->state = 0 ;

        switch(p_creature->state)  {
            case 0:   /* Initial state -- looking for a target. */
                ObjectSetStance(p_obj, STANCE_STAND) ;
                if ((p_creature->target =
                        CreatureLookForPlayer(p_obj)) != 0xFFFF)  {
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;

                    /* Creature sound that he sees you! */
                    ServerAreaSoundGlobal (
                           ObjectGetX16 (p_obj),
                           ObjectGetY16 (p_obj),
                           1000,
                           80);
                }
                break ;
            case 1:   /* Moving towards opponent state. */
                /* Make us animate as walking. */
                ObjectSetStance(p_obj, STANCE_WALK) ;

                /* First, see if the creature was blocked last time. */
                if (ObjectWasBlocked(p_obj))  {
                    /* We are hitting something, let's turn randomly */
                    /* 45 degrees to one side. */
                    if (p_creature->goForward != TRUE)
                        p_creature->turnWay = TickerGet() & 1 ;

                    if (p_creature->turnWay)  {
                        ObjectAddAngle(p_obj, INT_ANGLE_45) ;
                    } else {
                        ObjectAddAngle(p_obj, -INT_ANGLE_45) ;
                    }

                    p_creature->goForward = TRUE ;
                    p_creature->stepAway = 70 ;

                    /* Make sure future access say we were not blocked. */
                    ObjectClearBlockedFlag(p_obj) ;
                }

                /* Locate where the target is. */
                ServerGetPlayerLocation(
                    p_creature->target,
                    &playerX,
                    &playerY) ;

                /* Get the angle to this target. */
                angle = MathArcTangent(
                            playerX - ObjectGetX16(p_obj),
                            playerY - ObjectGetY16(p_obj)) ;

                /* What is the difference to the angle I'm currently */
                /* facing? */
                diffangle = ObjectGetAngle(p_obj) - angle ;
                if (diffangle < 0)
                    absdiffangle = -diffangle ;
                else
                    absdiffangle = diffangle ;

                if (p_creature->goForward == TRUE)  {
                    absdiffangle = 0 ;
                    p_creature->stepAway -= delta ;
                    if (p_creature->stepAway < 0)
                        p_creature->goForward = FALSE ;
                }

                /* Is that a big angle or a small one? */
                if (absdiffangle >= INT_ANGLE_45)  {
                    /* Yeah, a big angle.  Just turn on our heels. */
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, (delta << 8)) ;
                    else
                        ObjectAddAngle(p_obj, -(delta << 8)) ;
                } else {
                    /* Not a big angle, we might as well go ahead */
                    /* and try moving a bit. */

                    /* Turn as needed. */
                    turnangle = (delta<<6) ;
                    if (turnangle > absdiffangle)
                        turnangle = absdiffangle ;
                    if (diffangle < 0)
                        ObjectAddAngle(p_obj, turnangle) ;
                    else
                        ObjectAddAngle(p_obj, -turnangle) ;

                    /* How close are we? */
                    distance = CalculateEstimateDistance(
                                   ObjectGetX16(p_obj),
                                   ObjectGetY16(p_obj),
                                   playerX,
                                   playerY) ;

    //                origX = ObjectGetX16(p_obj) ;
    //                origY = ObjectGetX16(p_obj) ;

                    /* Are we close enough to attack? */
                    if (distance < 65)  {
                        /* !!! Attack !!! */
                        if (rand() & 1)
                            ObjectSetStance(p_obj, STANCE_ATTACK) ;
                        else
                            ObjectSetStance(p_obj, STANCE_ATTACK2) ;

                        ObjectSetAngle(p_obj, angle) ;

                        ObjectGetForwardPosition(
                            p_obj,
                            ObjectGetRadius(p_obj) + 25,
                            &frontX,
                            &frontY) ;

                        ServerDamageAt(
                            ObjectGetX(p_obj),
                            ObjectGetY(p_obj),
                            frontX,
                            frontY,
                            0,
                            100) ;

                        /* Set the time for pausing on the attack. */
                        p_creature->state = 2 ;
                        p_creature->stateData = 27 ;
                    } else {
                        /* Too far, let's move closer. */
                        ObjectAccelFlat(p_obj, delta, ObjectGetAngle(p_obj)) ;

                        /* See if we are tired of moving and want */
                        /* a new opponent. */
                        if (p_creature->stateData <= newTime)  {
                            /* Look around for opponents. */
                            newTarget = CreatureLookForPlayer(p_obj) ;
                            if (newTarget != 0xFFFF /* -1 */)
                                p_creature->target = newTarget ;
                            p_creature->stateData = newTime+210 ;
                        }
                    }
                }
                break ;
            case 2:   /* Pause after attacking or getting hurt. */
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 10)
                    /* Yes, continue going after target. */
                    p_creature->state = 3 ;
                break ;
            case 3:
                ObjectSetStance(p_obj, STANCE_STAND) ;
                p_creature->stateData -= delta ;

                /* Are we done pausing? */
                if (p_creature->stateData <= 0)  {
                    /* Yes, continue going after target. */
                    p_creature->state = 1 ;
                    p_creature->stateData = newTime+210 ;
                }
                break ;
        }
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  CreatureUpdateSplat                                           */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreatureUpdateSplat    is the brains behind a splat.     (Doesn't     */
/*  that sound absurd?)                                                     */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_3dObject *p_obj           -- fireball object                        */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  07/27/95  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void CreatureUpdateSplat(T_3dObject *p_obj)
{
   T_packetShort destroyPacket;
   T_creature *p_creature ;
   T_word32 delta ;

   DebugRoutine ("CreatureUpdateSplat");

   /* Get the creature pointer. */
   p_creature = (T_creature *)ObjectGetExtraData(p_obj) ;

   if  (p_creature != NULL)  {
       /* Update the time and calculate how long it has been. */
       if (p_creature->nextAttack == 0)
           p_creature->nextAttack = TickerGet() ;

       delta = TickerGet() - p_creature->nextAttack ;

       p_creature->nextAttack = TickerGet() ;

       /* Make the damage more. */
       delta *= 32 ;

       /** Yes.  First, do damage all around me relative to the time */
       /* it has been. **/
    /*
       if (delta)
           ServerDamageAt (
               ObjectGetX(p_obj),
               ObjectGetY(p_obj),
               ObjectGetX (p_obj),
               ObjectGetY (p_obj),
               60,
               delta);
    */

       /** Hmm..  this is simple.  Have I hit anything? **/
       if (ObjectWasBlocked (p_obj))
       {
          /* Make sure the object is passable. */
          ObjectAddAttributes(p_obj, OBJECT_ATTR_PASSABLE) ;

          /* Clear the movement flags. */
          ObjectClearMoveFlags(
              p_obj,
              OBJMOVE_FLAG_IGNORE_FRICTION |
                 OBJMOVE_FLAG_IGNORE_GRAVITY) ;

          ObjectSetStance(p_obj, 1) ;

          ObjectStopMoving(p_obj) ;

    /* Try to "turn off" this creature. */
    MemFree(p_creature) ;
    ObjectSetExtraData(p_obj, NULL) ;
    ObjectSetScript(p_obj, 0) ;
       }
   }

   DebugEnd ();
}


#endif

/****************************************************************************/
/*  Routine:  CreatureUpdateFireball                                        */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    CreatureUpdateFireball is the brains behind a fireball.  (Doesn't     */
/*  that sound absurd?)                                                     */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_3dObject *p_obj           -- fireball object                        */
/*                                                                          */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    AMT  07/17/95  Created                                                */
/*    LES  02/10/96  Modified to handle missile types                       */
/*                                                                          */
/****************************************************************************/

static E_effectDamageType G_missileToDamageType[EFFECT_MISSILE_UNKNOWN] = {
    EFFECT_DAMAGE_NORMAL,
    EFFECT_DAMAGE_NORMAL,
    EFFECT_DAMAGE_ACID,
    EFFECT_DAMAGE_ACID,
    EFFECT_DAMAGE_FIRE,
    EFFECT_DAMAGE_FIRE,
    EFFECT_DAMAGE_NORMAL |
        EFFECT_DAMAGE_FIRE |
        EFFECT_DAMAGE_ACID |
        EFFECT_DAMAGE_POISON |
        EFFECT_DAMAGE_ELECTRICITY |
        EFFECT_DAMAGE_PIERCING,
    EFFECT_DAMAGE_NORMAL,
    EFFECT_DAMAGE_ELECTRICITY,
    0,
    0,
    0,
    EFFECT_DAMAGE_MANA_DRAIN,
    0,
    0,
    0,
    EFFECT_DAMAGE_POISON,
    0,
    0,
    0,
    0,
    EFFECT_DAMAGE_NORMAL,
    EFFECT_DAMAGE_POISON,
    EFFECT_DAMAGE_PIERCING,
    EFFECT_DAMAGE_FIRE,
    EFFECT_DAMAGE_ELECTRICITY,
    EFFECT_DAMAGE_MANA_DRAIN,
    EFFECT_DAMAGE_ACID,
} ;

static T_word16 G_missileToObjectType[EFFECT_MISSILE_UNKNOWN] = {
    4011,
    4011,
    4004,
    4004,
    4005,
    4005,
    4003,
    4014,
    4006,
    4012,
    4013,
    4007,
    4001,
    31,
    31,
    4010,
    4015,
    4010,
    4000,
    4009,
    4016,
    206,
    222,
    229,
    218,
    241,
    242,
    240
} ;

static T_word16 G_missileDamageTable[EFFECT_MISSILE_UNKNOWN] = {
    100,        /* EFFECT_MISSILE_MAGIC_DART */
    100,        /* EFFECT_MISSILE_HOMING_DART */
    500,        /* EFFECT_MISSILE_ACID_BALL */
    500,        /* EFFECT_MISSILE_HOMING_ACID_BALL */
    1000,       /* EFFECT_MISSILE_FIREBALL */
    1000,       /* EFFECT_MISSILE_HOMING_FIREBALL */
    2000,       /* EFFECT_MISSILE_HOMING_DEATHBALL */
    500,        /* EFFECT_MISSILE_EARTH_SMITE */
    500,        /* EFFECT_MISSILE_LIGHTNING_BOLT */
    500,        /* EFFECT_MISSILE_LOCK_DOOR */
    500,        /* EFFECT_MISSILE_UNLOCK_DOOR */
    500,        /* EFFECT_MISSILE_DISPELL_MAGIC */
    500,        /* EFFECT_MISSILE_MANA_DRAIN */
    500,        /* EFFECT_MISSILE_PUSH */
    500,        /* EFFECT_MISSILE_PULL */
    500,        /* EFFECT_MISSILE_EARTHBIND */
    500,        /* EFFECT_MISSILE_POISON */
    500,        /* EFFECT_MISSILE_CONFUSION */
    500,        /* EFFECT_MISSILE_BERSERK */
    500,        /* EFFECT_MISSILE_SLOW */
    500,        /* EFFECT_MISSILE_PARALYZE */
    200,        /* EFFECT_BOLT_NORMAL */
    200,        /* EFFECT_BOLT_POISON */
    200,        /* EFFECT_BOLT_PIERCING */
    200,        /* EFFECT_BOLT_FIRE */
    200,        /* EFFECT_BOLT_ELECTRICITY */
    200,        /* EFFECT_BOLT_MANA_DRAIN */
    200,        /* EFFECT_BOLT_ACID */
} ;

static T_word16 G_missileRadiusTable[EFFECT_MISSILE_UNKNOWN] = {
    60,         /* EFFECT_MISSILE_MAGIC_DART */
    60,         /* EFFECT_MISSILE_HOMING_DART */
    60,         /* EFFECT_MISSILE_ACID_BALL */
    60,         /* EFFECT_MISSILE_HOMING_ACID_BALL */
    200,        /* EFFECT_MISSILE_FIREBALL */
    200,        /* EFFECT_MISSILE_HOMING_FIREBALL */
    60,         /* EFFECT_MISSILE_HOMING_DEATHBALL */
    60,         /* EFFECT_MISSILE_EARTH_SMITE */
    60,         /* EFFECT_MISSILE_LIGHTNING_BOLT */
    60,         /* EFFECT_MISSILE_LOCK_DOOR */
    60,         /* EFFECT_MISSILE_UNLOCK_DOOR */
    60,         /* EFFECT_MISSILE_DISPELL_MAGIC */
    60,         /* EFFECT_MISSILE_MANA_DRAIN */
    60,         /* EFFECT_MISSILE_PUSH */
    60,         /* EFFECT_MISSILE_PULL */
    60,         /* EFFECT_MISSILE_EARTHBIND */
    60,         /* EFFECT_MISSILE_POISON */
    60,         /* EFFECT_MISSILE_CONFUSION */
    60,         /* EFFECT_MISSILE_BERSERK */
    60,         /* EFFECT_MISSILE_SLOW */
    60,         /* EFFECT_MISSILE_PARALYZE */
    30,         /* EFFECT_BOLT_NORMAL */
    30,         /* EFFECT_BOLT_POISON */
    30,         /* EFFECT_BOLT_PIERCING */
    30,         /* EFFECT_BOLT_FIRE */
    30,         /* EFFECT_BOLT_ELECTRICITY */
    30,         /* EFFECT_BOLT_MANA_DRAIN */
    30          /* EFFECT_BOLT_ACID */
} ;

static T_word16 IFindTypeOfMissile(T_word16 objectType)
{
    T_word16 i ;

    DebugRoutine("IFindTypeOfMissile") ;

    /* Search for the missile with the given type. */
    for (i=0; i<EFFECT_MISSILE_UNKNOWN; i++)
        if (G_missileToObjectType[i] == objectType)
            break ;

    DebugCheck(i!=EFFECT_MISSILE_UNKNOWN) ;
    if (i==EFFECT_MISSILE_UNKNOWN)
        i = 0 ;

    DebugEnd() ;

    return i ;
}

T_void CreatureUpdateFireball (T_3dObject *p_obj)
{
   T_packetShort experiencePacket ;
   T_experiencePacket *p_exp ;
   T_packetShort destroyPacket;
   E_effectMissileType missileType ;

   DebugRoutine ("CreatureUpdateFireball");

   /** Hmm..  this is simple.  Have I hit anything? **/
   if (ObjectWasBlocked (p_obj))
   {
      /** Yes.  First, do damage all around me. **/
      missileType = (E_effectMissileType)
                    IFindTypeOfMissile((T_word16)
                        (ObjectGetBasicType(p_obj))) ;

      ServerClearTallyDamage() ;

      ServerDamageAtWithType(
           ((T_sword16)(ObjectGetX16(p_obj))),
           ((T_sword16)(ObjectGetY16(p_obj))),
           (T_sword16)(((T_sword16)(ObjectGetZ16(p_obj))) +
               (ObjectGetHeight(p_obj)>>1)),
           (T_word16)(G_missileDamageTable[(T_word16)missileType]),
           (T_word16)(G_missileDamageTable[(T_word16)missileType]),
           (T_byte8)(G_missileToDamageType[(T_word16)missileType])) ;

printf("Tally damage: %d\n", ServerGetTallyDamage()) ;
fflush(stdout) ;
      /* If we did any damage, get the experience for doing the damage. */
      if (ServerGetTallyDamage() > 0)  {
          experiencePacket.header.packetLength = sizeof(T_experiencePacket) ;
          p_exp = (T_experiencePacket *)(experiencePacket.data) ;
          p_exp->command = PACKET_COMMANDSC_EXPERIENCE ;
          p_exp->experience = ServerGetTallyDamage() ;

printf("Sending tally damage of %d to player %p\n", 
    p_exp->experience,
    ObjectGetOwnerID(p_obj)) ;
fflush(stdout) ;
          ServerSendToPlayerByUniqueID(
              (T_packetEitherShortOrLong *)&experiencePacket,
              (T_player)ObjectGetOwnerID(p_obj)) ;
      }

      /** Then, destroy myself. **/
      ServerDestroyObjectGlobal (p_obj, 0xFFFF);
   }

   DebugEnd ();
}

/****************************************************************************/
/*    END OF FILE:  CREATURE.C                                              */
/****************************************************************************/
