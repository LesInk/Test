/****************************************************************************/
/*    FILE:  HARDFORM.C                                                     */
/****************************************************************************/

#include "standard.h"

/* Test prototypes: */
#if 0
T_void HardFormStartTest(T_word32 formNum);
T_void HardFormEndTest(T_void);
T_void HardFormHandleMouseTest(
        E_mouseEvent event,
        T_word16 x,
        T_word16 y,
        T_buttonClick buttons);
T_void HardFormHandleMouseTest2(
        E_mouseEvent event,
        T_word16 x,
        T_word16 y,
        T_buttonClick buttons);
T_void HardFormUpdateTest(T_void);
T_void HardFormUpdateTest2(T_void);
#endif

static T_buttonID G_closeButton = NULL;
static T_void HardFormExit(T_buttonID buttonID);
static E_Boolean HardFormOpen = FALSE;

typedef struct {
    T_hardFormStart start;
    T_hardFormEnd end;
    T_hardFormUpdate update;
    T_hardFormHandleMouse handleMouse;
} T_hardFormCallbackGroup;

static T_hardFormCallbackGroup G_callbackGroups[HARD_FORM_UNKNOWN] = {
    {
        BankUIStart,                      /* start */
        BankUIEnd,                        /* end */
        BankUIUpdate,                     /* update */
        StoreHandleTransaction           /* handle mouse */
    },
    {
        StoreUIStart,
        StoreUIEnd,
        StoreUIUpdate,
        StoreHandleTransaction
    },
    {   InnUIStart,
        InnUIEnd,
        InnUIUpdate,
        NULL
    },
    {
        HouseUIStart,
        HouseUIEnd,
        HouseUIUpdate,
        StoreHandleTransaction
    },
    {
        TownUIStart,
        TownUIEnd,
        TownUIUpdate,
        NULL
    },
    {
        GuildUIStart,
        GuildUIEnd,
        GuildUIUpdate,
        NULL
    }
};

static T_word32 G_currentForm = HARD_FORM_UNKNOWN;

T_void HardFormStart(T_word32 formNum)
{
    T_word32 formID;

    DebugRoutine("HardFormStart");

    HardFormOpen = TRUE;
    /* All hard forms have an alternate output turned on. */
    MessageSetAlternateOutputOn();
    BannerUIModeOn();

    formNum -= STORE_ID_START;
    formID = formNum;
    DebugCheck(formNum < HARD_FORM_UNKNOWN);
    DebugCheck(G_currentForm == HARD_FORM_UNKNOWN);

//    if (formID!=HARD_FORM_TOWN)
    if (1) {
        /* create hardform close button */
        G_closeButton = ButtonCreate(196, 5, "UI/COMMON/CLOSEWIN", FALSE, 0,
                NULL, HardFormExit);
    } else {
        G_closeButton = NULL;
    }

    if (formNum >= 20)
        formNum = HARD_FORM_HOUSE;

    G_currentForm = formNum;

    if (G_callbackGroups[G_currentForm].start != NULL )
        G_callbackGroups[G_currentForm].start(formID);

    DebugEnd();
}

T_void HardFormEnd(T_void)
{
    DebugRoutine("HardFormEnd");

    if (G_closeButton) {
        ButtonDelete(G_closeButton);
        G_closeButton = NULL;
    }
//    if (G_currentForm != HARD_FORM_UNKNOWN)  {
    MessageSetAlternateOutputOff();
    if (BannerIsOpen())
        BannerUIModeOff();

    DebugCheck(G_currentForm != HARD_FORM_UNKNOWN);
    if (G_callbackGroups[G_currentForm].end != NULL )
        G_callbackGroups[G_currentForm].end();

    G_currentForm = HARD_FORM_UNKNOWN;
//    }

    HardFormOpen = FALSE;

    DebugEnd();
}

static T_void HardFormExit(T_buttonID buttonID)
{
    DebugRoutine("HardFormExit");

    /* Tell others that I'm no longer creating a game. */
    PeopleHereSetOurState(PLAYER_ID_STATE_NONE);

    /* Send out a message that this is what we are doing. */
    ClientSendPlayerIDSelf();

    PeopleHereSetOurAdventure(0);
    ClientSyncSetGameGroupID(*DirectTalkGetNullBlankUniqueAddress());
    /* Send out a message that this is what we are doing. */
    ClientSendPlayerIDSelf();

    ButtonDelete(G_closeButton);
    G_closeButton = NULL;
    if (TownUIIsOpen()) {
        ClientSetNextPlace(0, 0);
    } else {
        ClientSetNextPlace(HARDFORM_GOTO_PLACE_OFFSET + HARD_FORM_TOWN, 0);
    }

    DebugEnd();
}

T_void HardFormHandleMouse(
        E_mouseEvent event,
        T_word16 x,
        T_word16 y,
        T_buttonClick buttons)
{
    DebugRoutine("HardFormHandleMouse");

    if (G_currentForm < HARD_FORM_UNKNOWN) {
        if (G_callbackGroups[G_currentForm].handleMouse != NULL )
            G_callbackGroups[G_currentForm].handleMouse(event, x, y, buttons);
    }

    DebugEnd();
}

T_void HardFormUpdate(T_void)
{
    DebugRoutine("HardFormUpdate");
    if (G_currentForm < HARD_FORM_UNKNOWN) {
        GrScreenSet(GRAPHICS_ACTUAL_SCREEN);
        BannerUpdate();
        StatsUpdatePlayerStatistics();
        GraphicUpdateAllGraphics();
        ScheduleUpdateEvents();
        KeyboardUpdateEvents();
        MouseUpdateEvents();
        if (G_callbackGroups[G_currentForm].update != NULL )
            G_callbackGroups[G_currentForm].update();
    }

    DebugEnd();
}

E_Boolean HardFormIsOpen(T_void)
{
    return (HardFormOpen);
}

#if 0
T_void HardFormStartTest(T_word32 formNum)
{
    DebugRoutine("HardFormStartTest");
    DebugCheck(formNum < HARD_FORM_UNKNOWN);

    SpellsInitSpells();

    GrDrawRectangle(
            4,
            4,
            100,
            100,
            32);

    DebugEnd();
}

T_void HardFormHandleMouseTest(
        E_mouseEvent event,
        T_word16 x,
        T_word16 y,
        T_buttonClick buttons)
{
    DebugRoutine("HardFormHandleMouseTest");

    GrDrawRectangle(
            4,
            4,
            100,
            100,
            31);
    ColorUpdate(0);

    delay(100);

    GrDrawRectangle(
            4,
            4,
            100,
            100,
            0);
    ColorUpdate(0);

    ClientGotoPlace(75, 0);

    DebugEnd();
}

T_void HardFormUpdateTest(T_void)
{
    static T_byte8 color = 0;
    DebugRoutine("HardFormUpdateTest");

    GrDrawRectangle(
            4, 3, 4+VIEW3D_CLIP_RIGHT-VIEW3D_CLIP_LEFT-1, 3+VIEW3D_HEIGHT-1,
            12);
    GrDrawRectangle(
            4,
            4,
            10,
            10,
            color++);

    MessageDraw(4, 4, 10, COLOR_WHITE);

    ClientUpdateHealth();
    BannerUpdate();
    StatsUpdatePlayerStatistics();
    EffectUpdateAllPlayerEffects();

    DebugEnd();
}

T_void HardFormUpdateTest2(T_void)
{
    static T_byte8 color = 0;
    DebugRoutine("HardFormUpdateTest");

    GrDrawRectangle(
            4, 3, 4+VIEW3D_CLIP_RIGHT-VIEW3D_CLIP_LEFT-1, 3+VIEW3D_HEIGHT-1,
            48);
    GrDrawRectangle(
            4,
            4,
            100,
            10,
            color++);

    GrDrawRectangle(
            5,
            100,
            37,
            160,
            10);
    GrSetCursorPosition(5, 101);
    GrDrawText("1", 31);

    GrDrawRectangle(
            45,
            100,
            77,
            160,
            10);
    GrSetCursorPosition(45, 101);
    GrDrawText("75", 31);

    GrDrawRectangle(
            85,
            100,
            117,
            160,
            10);
    GrSetCursorPosition(85, 101);
    GrDrawText("74", 31);

    GrDrawRectangle(
            125,
            100,
            157,
            160,
            10);
    GrSetCursorPosition(125, 101);
    GrDrawText("32", 31);

    MessageDraw(4, 4, 10, COLOR_WHITE);

    ClientUpdateHealth();
    BannerUpdate();
    StatsUpdatePlayerStatistics();
    EffectUpdateAllPlayerEffects();

    DebugEnd();
}

T_void HardFormEndTest(T_void)
{
    DebugRoutine("HardFormEndTest");

    SpellsFinish();

    DebugEnd();
}

T_void HardFormHandleMouseTest2(
        E_mouseEvent event,
        T_word16 x,
        T_word16 y,
        T_buttonClick buttons)
{
    DebugRoutine("HardFormHandleMouseTest2");

    if ((y > 100) & (y < 160)) {
        if ((x > 5) && (x < 37)) {
            ClientGotoPlace(1, 0);
        }
        if ((x > 45) && (x < 77)) {
            ClientGotoPlace(75, 0);
        }
        if ((x > 85) && (x < 117)) {
            ClientGotoPlace(74, 0);
        }
        if ((x > 125) && (x < 157)) {
            ClientGotoPlace(32, 0);
        }
    }

    DebugEnd();
}

#endif

/****************************************************************************/
/*    END OF FILE:  HARDFORM.C                                              */
/****************************************************************************/
