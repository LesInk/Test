/****************************************************************************/
/*    FILE:  CONFIG.C                                                       */
/****************************************************************************/

#include "standard.h"

#define MEMORY_SOMEWHAT_LOW_LEVEL          ((T_word32)7000000)

#define MEMORY_LOW_LEVEL                   ((T_word32)2400000)
#define MEMORY_LOW_CREATURE_FRAMES         2
#define MEMORY_LOW_PLAYER_FRAMES           2

#define MEMORY_VERY_LOW_LEVEL              ((T_word32)1800000)
#define MEMORY_VERY_LOW_CREATURE_FRAMES    7
#define MEMORY_VERY_LOW_PLAYER_FRAMES      7

#define MEMORY_NOT_ENOUGH_LEVEL            ((T_word32)1200000)

static E_Boolean G_configLoaded = FALSE ;
static T_iniFile G_configINIFile = INIFILE_BAD ;
static E_Boolean G_bobOffFlag = FALSE ;

T_word32 FreeMemory(T_void) ;

/****************************************************************************/
/*  Routine:  ConfigOpen                                                    */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    ConfigOpen opens up the config file for reading and changes.          */
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
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  03/11/96  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void ConfigOpen(T_void)
{
    DebugRoutine("ConfigOpen") ;
    DebugCheck(G_configINIFile == INIFILE_BAD) ;

    G_configINIFile = INIFileOpen("config.ini") ;
    DebugCheck(G_configINIFile != INIFILE_BAD) ;

    KeyMapInitialize(G_configINIFile) ;

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  ConfigClose                                                   */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    ConfigClose closes the config file and writes out any changes.        */
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
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  03/11/96  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void ConfigClose(T_void)
{
    DebugRoutine("ConfigClose") ;
    DebugCheck(G_configINIFile != INIFILE_BAD) ;

    if (G_configINIFile != INIFILE_BAD)  {
        KeyMapFinish() ;
        INIFileClose("config.ini", G_configINIFile) ;
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  ConfigLoad                                                    */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    ConfigLoad reads in a variety of variables on how the game is to      */
/*  operate.  In particular, the GAME.CFG file determines the amount of     */
/*  graphic detail is to be shown.                                          */
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
/*    fopen                                                                 */
/*    fgets                                                                 */
/*    fclose                                                                */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  01/16/95  Created                                                */
/*    LES  05/24/95  Added memory check to downgrade frames per image if    */
/*                   low memory constraints.                                */
/*                                                                          */
/****************************************************************************/

T_void ConfigLoad(T_void)
{
    FILE *fp ;
    T_word16 i ;
    T_byte8 buffer[80] ;
    T_sword32 num ;
    T_word32 memFree ;
    T_word16 neededResolution ;
    T_byte8 *p_stepdown ;
    T_word16 stepdown = 0 ;
    char *p_value;

    DebugRoutine("ConfigLoad") ;
    DebugCheck(G_configLoaded == FALSE) ;

    /* Get the amount of free memory as soon as we can. */
    memFree = FreeMemory() ;

    if (memFree < MEMORY_SOMEWHAT_LOW_LEVEL)  {
    /* If you are low on memory, cut down the frames per player */
        ObjTypeDeclareSomewhatLowOnMemory() ;
        puts("Lowering piecewise animations") ;
    }
    /* If you are low on memory, cut down the frames per player and creature */
    if (memFree < MEMORY_NOT_ENOUGH_LEVEL)  {
        puts("NOT ENOUGH MEMORY!!!\n") ;
        puts("Sorry, you do not have enough memory available in either conventional") ;
        puts("or expanded memory.  Please do not use SMARTDRV if you are using it.") ;
        puts("Please remove any unneeded TSR's or memory eating devices.  You must") ;
        puts("have at least 2500K free of expanded memory to run this program, ") ;
        puts("and you should have at least 3000K to run it adequately.\n") ;
        puts("PROGRAM ABORTED") ;
        exit(1) ;
    } else if (memFree < MEMORY_VERY_LOW_LEVEL)  {
        puts("VERY LOW memory system!  Lowering graphical resolution ALOT.") ;
        neededResolution = 2 ;
    } else if (memFree < MEMORY_LOW_LEVEL)  {
        puts("LOW memory system!  Lowering graphical resolution.") ;
        neededResolution = 1 ;
    } else {
        puts("Ahhh ... plenty of memory to run in.") ;
        neededResolution = 0 ;
    }
    p_stepdown = INIFileGet(G_configINIFile, "engine", "stepdown") ;
//printf("INI got: '%s'\n", p_stepdown) ;
    if (p_stepdown)  {
        stepdown = sscanf(p_stepdown, "%d", &stepdown) ;
stepdown = (*p_stepdown) - '0' ;
//printf("stepdown = %d\n", stepdown) ;
    } else  {
        stepdown = 0 ;
    }

    if (neededResolution < stepdown)
        neededResolution = stepdown ;

//printf("Needed resolution: %d (%d)\n", neededResolution, stepdown) ;
    ObjTypesSetResolution(neededResolution) ;

    G_configLoaded = TRUE ;

    /* Load the bob off flag */
    p_value = INIFileGet(G_configINIFile, "options", "boboff");
    if (p_value) {
        if (atoi(p_value)==1)  {
            G_bobOffFlag = TRUE ;
        } else {
            G_bobOffFlag = FALSE ;
        }
    } else {
        G_bobOffFlag = FALSE ;
    }

    DebugEnd() ;
}

/****************************************************************************/
/*  Routine:  ConfigCreateAccountInfoBlock                                  */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    ConfigCreateAccountInfoBlock creates a block of data that tells what  */
/*  the user has selected in the past concerning address and billing info.  */
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
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  03/11/96  Created                                                */
/*                                                                          */
/****************************************************************************/

T_dataBlockAccountInfo *ConfigCreateAccountInfoBlock(T_void)
{
    T_dataBlockAccountInfo *p_accountInfo ;
    T_byte8 *p_field ;

    /* Allocate memory for the block. */
    p_accountInfo = MemAlloc(sizeof(T_dataBlockAccountInfo)) ;
    DebugCheck(p_accountInfo != NULL) ;

    /* Make sure we have the block before we mess with it. */
    if (p_accountInfo != NULL)  {
        /* Null out the block. */
        memset(p_accountInfo, 0, sizeof(T_dataBlockAccountInfo)) ;

        INIFileGetString(
            G_configINIFile,
            "account",
            "name",
            p_accountInfo->ownersName,
            60) ;
        INIFileGetString(
            G_configINIFile,
            "account",
            "street",
            p_accountInfo->street,
            80) ;
        INIFileGetString(
            G_configINIFile,
            "account",
            "city",
            p_accountInfo->city,
            40) ;
        INIFileGetString(
            G_configINIFile,
            "account",
            "state",
            p_accountInfo->state,
            20) ;
        INIFileGetString(
            G_configINIFile,
            "account",
            "zipcode",
            p_accountInfo->zipcode,
            14) ;
        INIFileGetString(
            G_configINIFile,
            "account",
            "phoneHome",
            p_accountInfo->phoneHome,
            20) ;
        INIFileGetString(
            G_configINIFile,
            "account",
            "phoneWork",
            p_accountInfo->phoneWork,
            20) ;
        INIFileGetString(
            G_configINIFile,
            "account",
            "billingMethod",
            p_accountInfo->billingMethod,
            20) ;
        INIFileGetString(
            G_configINIFile,
            "account",
            "billingData",
            p_accountInfo->billingData,
            60) ;
        p_field = INIFileGet(G_configINIFile, "account", "accountID") ;
        if (p_field != NULL)
            sscanf(p_field, "%lu", &p_accountInfo->accountID) ;
        else
            p_accountInfo->accountID = 0 ;

        DebugCheck(p_accountInfo->accountID != 0) ;
    }


    return p_accountInfo ;
}

/****************************************************************************/
/*  Routine:  ConfigGetAccountNumber                                        */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    ConfigGetAccountNumber gets the account number of the client.         */
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
/*    T_word32                    -- Account number, else 0                 */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    LES  03/11/96  Created                                                */
/*                                                                          */
/****************************************************************************/

T_word32 ConfigGetAccountNumber(T_void)
{
    T_word32 accountNumber = 0 ;
    T_byte8 *p_field ;

    DebugRoutine("ConfigGetAccountNumber") ;

    p_field = INIFileGet(G_configINIFile, "account", "accountID") ;
    if (p_field != NULL)
        sscanf(p_field, "%lu", &accountNumber) ;

    DebugEnd() ;

    return accountNumber ;
}

T_iniFile ConfigGetINIFile(T_void)
{
    return G_configINIFile ;
}

E_musicType ConfigGetMusicType(T_void)
{
    E_musicType type ;

    DebugRoutine("ConfigGetMusicType") ;

    type = atoi(INIFileGet(G_configINIFile, "options", "musicType")) ;

    DebugCheck(type < MUSIC_TYPE_UNKNOWN) ;

    /* If a strange type, then don't do it. */
    if (type >= MUSIC_TYPE_UNKNOWN)
        type = MUSIC_TYPE_NONE ;

    DebugEnd() ;

    return type ;
}

T_byte8 ConfigGetCDROMDrive(T_void)
{
    return *INIFileGet(G_configINIFile, "cdrom", "drive") ;
}

E_Boolean ConfigGetBobOffFlag(T_void)
{
    return G_bobOffFlag ;
}

/****************************************************************************/
/*    END OF FILE:  CONFIG.C                                                */
/****************************************************************************/
