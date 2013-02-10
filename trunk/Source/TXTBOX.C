/****************************************************************************/
/*    FILE:  TXTBOX.C                                                       */
/****************************************************************************/

#include "standard.h"

static T_txtboxID G_txtboxArray[MAX_TXTBOXES];
static T_word16 G_currentTextBox = 0;
static E_txtboxAction G_currentAction = TXTBOX_ACTION_NO_ACTION;
static T_void TxtboxAppendKeyNoRepag (T_txtboxID txtboxID, T_byte8 scankey);
extern T_byte8 G_extendedColors[MAX_EXTENDED_COLORS];

/****************************************************************************/
/*  Routine:  TxtboxCreate/TxtboxInit (internal routine)                    */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          *//*                                                                          *//*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*                                                                          */
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
/*    JDA  09/05/95  Created                                                */
/*                                                                          */
/****************************************************************************/


T_txtboxID TxtboxCreate (T_word16 x1,
                         T_word16 y1,
                         T_word16 x2,
                         T_word16 y2,
                         T_byte8 *fontname,
                         T_word32 maxlength,
                         T_byte8 hotkey,
                         E_Boolean numericonly,
                         E_txtboxJustify justify,
                         E_txtboxMode boxmode,
                         T_txtboxHandler callback)
{
    T_word16 i,j;
    T_word16 tx1,ty1,tx2,ty2;
    T_word16 windowheight;
    T_word32 size;
    T_txtboxStruct *p_txtbox;
    T_graphicStruct *p_graphic;
    T_resourceFile res;
    T_bitfont *p_font;

    DebugRoutine ("TxtboxCreate");
    /* make sure coordinates for textbox are valid */
    DebugCheck (x1<=SCREEN_SIZE_X &&
                x2<=SCREEN_SIZE_X &&
                y1<=SCREEN_SIZE_Y &&
                y2<=SCREEN_SIZE_Y &&
                x1<x2 &&
                y1<y2);
    /* make sure we have a fontname */
    DebugCheck (fontname!=NULL);

    /* search through global txtbox pointer list and find an empty slot */
    for (i=0;i<MAX_TXTBOXES;i++)
    {

        if (G_txtboxArray[i]==NULL)
        {
            /* found an empty slot, create a new text box */
            p_txtbox = (T_txtboxStruct *)G_txtboxArray[i];
            size=sizeof (T_txtboxStruct);
            p_txtbox = (T_txtboxID)MemAlloc(size);
            /* make sure the memory was allocated */
            DebugCheck (p_txtbox != NULL);
            /* now, set the default variables */
            p_txtbox->p_graphicID=NULL;
            p_txtbox->p_graphicID=GraphicCreate (x1,y1,NULL);
            GraphicSetSize (p_txtbox->p_graphicID,x2-x1,y2-y1);
            GraphicSetPostCallBack (p_txtbox->p_graphicID, TxtboxDrawCallBack, i);
            DebugCheck (p_txtbox->p_graphicID!=NULL);
            p_graphic=(T_graphicStruct *)p_txtbox->p_graphicID;
            DebugCheck (p_graphic->graphicpic==NULL);

            /* allocate inital data char */
            p_txtbox->data = MemAlloc(sizeof(T_byte8)*2);
            MemCheck (308);
            DebugCheck(p_txtbox->data != NULL) ;
            p_txtbox->data[0]='\0';

            /* allocate initial linestart/linewidth data space */
            p_txtbox->linestarts=MemAlloc(sizeof(T_word32)*2);
            MemCheck (309);
            DebugCheck (p_txtbox->linestarts != NULL);
            p_txtbox->linestarts[0]=0;

            p_txtbox->linewidths=MemAlloc(sizeof(T_word16)*2);
            MemCheck (310);
            DebugCheck (p_txtbox->linewidths != NULL);
            p_txtbox->linewidths[0]=0;

            /* copy passed in variables */
            p_txtbox->lx1 = x1;
            p_txtbox->lx2 = x2;
            p_txtbox->ly1 = y1;
            p_txtbox->ly2 = y2;
            p_txtbox->mode = boxmode;
            p_txtbox->txtboxcallback = callback;
            p_txtbox->justify=justify;
            p_txtbox->maxlength=maxlength;
            p_txtbox->hotkey=hotkey;
            p_txtbox->numericonly=numericonly;
            p_txtbox->isselected=FALSE;
            p_txtbox->isfull=FALSE;
            p_txtbox->sbupID=NULL;
            p_txtbox->sbdnID=NULL;
            p_txtbox->sbgrID=NULL;
            p_txtbox->sbstart=0;
            p_txtbox->sblength=0;
            if (p_txtbox->mode==TXTBOX_MODE_EDIT_FORM ||
                p_txtbox->mode==TXTBOX_MODE_EDIT_FIELD ||
                p_txtbox->mode==TXTBOX_MODE_FIXED_WIDTH_FIELD)
            {
                DebugCheck (p_txtbox->justify==FALSE);
            }


            /* set default colors (grey text box) */
/*
            p_txtbox->textcolor=29;
            p_txtbox->textshadow=6;
            p_txtbox->backcolor=8;
            p_txtbox->bordercolor1=11;
            p_txtbox->bordercolor2=4;
            p_txtbox->hbackcolor=9;
            p_txtbox->hbordercolor1=12;
            p_txtbox->hbordercolor2=5;
            p_txtbox->htextcolor=31;
*/
            p_txtbox->textcolor=210;
            p_txtbox->textshadow=0;
            p_txtbox->backcolor=68;
            p_txtbox->bordercolor1=66;
            p_txtbox->bordercolor2=70;
            p_txtbox->hbackcolor=67;
            p_txtbox->hbordercolor1=65;
            p_txtbox->hbordercolor2=69;
            p_txtbox->htextcolor=209;

/*
            switch (boxmode)
            {
                case TXTBOX_MODE_SELECTION_BOX:
                p_txtbox->textcolor=210;
                p_txtbox->textshadow=0;
                p_txtbox->backcolor=68;
                p_txtbox->bordercolor1=66;
                p_txtbox->bordercolor2=70;
                p_txtbox->hbackcolor=67;
                p_txtbox->hbordercolor1=65;
                p_txtbox->hbordercolor2=69;
                p_txtbox->htextcolor=209;
                p_txtbox->textcolor=168;
                p_txtbox->htextcolor=168;
                p_txtbox->textshadow=0;
                p_txtbox->backcolor=154;
                p_txtbox->hbackcolor=154;
                p_txtbox->bordercolor1=152;
                p_txtbox->hbordercolor1=152;
                p_txtbox->bordercolor2=156;
                p_txtbox->hbordercolor2=156;

                break;

                case TXTBOX_MODE_VIEW_NOSCROLL_FORM:
                case TXTBOX_MODE_VIEW_SCROLL_FORM:
                p_txtbox->textcolor=210;
                p_txtbox->textshadow=0;
                p_txtbox->backcolor=68;
                p_txtbox->bordercolor1=66;
                p_txtbox->bordercolor2=70;
                default:
                break;
            }
*/
            /* set the cursor to 0,0 */
            p_txtbox->cursorl=0;
            p_txtbox->cursorline=0;
            p_txtbox->cursorx=0;
            p_txtbox->cursory=0;
            p_txtbox->windowstartline=0;
            p_txtbox->totalrows=1;

            res = ResourceOpen ("sample.res");
            /* open up the selected font */
            p_txtbox->font=ResourceFind (res,fontname);
            p_font=ResourceLock (p_txtbox->font);
            GrSetBitFont (p_font);

            /* determine the height of the font */
            p_txtbox->fontheight=p_font->height;

            /* close the font */
            ResourceUnlock (p_txtbox->font);
            ResourceUnfind (p_txtbox->font);
            ResourceClose (res);

            /* determine how many rows we can fit in the given area */
            windowheight=y2-y1;
            p_txtbox->windowrows=windowheight/p_txtbox->fontheight;

            /* we made a new textbox, exit from loop */
            G_txtboxArray[i]=p_txtbox;
            break;
        }
    }

    /* make sure we haven't exceeded any limits */
    DebugCheck (i<MAX_TXTBOXES);
    DebugCheck (G_txtboxArray[i]!=NULL);

    DebugEnd();
    return (G_txtboxArray[i]);

}




/****************************************************************************/
/*  Routine:  TxtboxDelete/TxtboxCleanUp                                    */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*  TxtboxDelete removes all data associated with txtboxID (passed in).     */
/*  TxtboxCleanup removes all data associated with all textboxes.           */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*  txtboxID to delete with TxtboxDelete                                    */
/*  void for TxtboxCleanup                                                  */
/*                                                                          *//*                                                                          *//*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*  None.                                                                   */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*   Memfree                                                                */
/*                                                                          */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    JDA  09/05/95  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void TxtboxDelete (T_txtboxID txtboxID)
{
    T_word16 i,j;
    T_txtboxStruct *p_txtbox;

    DebugRoutine ("TxtboxDelete");

    if (txtboxID != NULL)
    {
        /* search for txtboxID in list of current textboxes */
        for (i=0;i<MAX_TXTBOXES;i++)
        {
            if (G_txtboxArray[i]==txtboxID)
            {
                if (G_currentTextBox==i) G_currentTextBox=-0;

                /* found it, now kill it */
                p_txtbox=(T_txtboxStruct *)txtboxID;
                /* get rid of the data string */
                MemFree (p_txtbox->data);
                MemCheck (300);
                p_txtbox->data=NULL;
                /* get rid of the linestarts */
                MemFree (p_txtbox->linestarts);
                MemCheck (305);
                p_txtbox->linestarts=NULL;
                MemFree (p_txtbox->linewidths);
                MemCheck (306);
                p_txtbox->linewidths=NULL;
                /* get rid of the graphic */
                GraphicDelete (p_txtbox->p_graphicID);
                /* get rid of the structure */
                MemFree (G_txtboxArray[i]);
                MemCheck (301);
                G_txtboxArray[i]=NULL;
                /* we found it, break */
                break;
            }
        }
    }

    /* make sure we found it */
//    DebugCheck (i<MAX_TXTBOXES);

    DebugEnd();
}


T_void TxtboxCleanUp (T_void)
{
    T_word16 i;
    T_resourceFile res;

    DebugRoutine ("TxtboxCleanUp");

    for (i=0;i<MAX_TXTBOXES;i++)
    {
        if (G_txtboxArray[i]!=NULL) TxtboxDelete (G_txtboxArray[i]);
        /* not efficient, but it works */
    }
    G_currentTextBox=0;
    res=ResourceOpen ("sample.res");
    ResourceClose (res);

    DebugEnd();
}


/****************************************************************************/
/*  Routine:  TxtboxCursUp/TxtboxCursDn/TxtboxCursPgUp/TxtboxCursPgDn       */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          *//*                                                                          *//*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*                                                                          */
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
/*    JDA  09/05/95  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void TxtboxCursTop  (T_txtboxID txtboxID)
{
    T_txtboxStruct *p_txtbox;

    DebugRoutine ("TxtboxCursTop");
    DebugCheck (txtboxID != NULL);

    p_txtbox=(T_txtboxStruct *)txtboxID;
    /* move cursor position to first character */
    p_txtbox->cursorl=0;
    p_txtbox->cursorline=0;
    /* move window position to first character */
    p_txtbox->windowstartline=0;

    /* update the view */
    TxtboxUpdate(txtboxID);

    DebugEnd();
}


T_void TxtboxCursBot  (T_txtboxID txtboxID)
{
    T_word16 wrow;
    T_txtboxStruct *p_txtbox;

    DebugRoutine ("TxtboxCursBot");
    DebugCheck (txtboxID != NULL);

    p_txtbox=(T_txtboxStruct *)txtboxID;

    switch (p_txtbox->mode)
    {
        case TXTBOX_MODE_EDIT_FIELD:
        case TXTBOX_MODE_EDIT_FORM:
        case TXTBOX_MODE_FIXED_WIDTH_FIELD:
        /* move cursor position to last character */
        p_txtbox->cursorl=strlen(p_txtbox->data);
        p_txtbox->cursorline=p_txtbox->totalrows;

        /* calculate the window start line for new window positon */
        wrow=p_txtbox->cursorline;

        /* back the window start row up a number of lines */
        /* equal to the height of the window */
        if (wrow>=p_txtbox->windowrows) wrow-=p_txtbox->windowrows;
        p_txtbox->windowstartline=wrow;

        break;

        case TXTBOX_MODE_VIEW_SCROLL_FORM:
        case TXTBOX_MODE_SELECTION_BOX:

        /* move the cursor to the beginning of the last line */
        p_txtbox->cursorl=p_txtbox->linestarts[p_txtbox->totalrows];
        p_txtbox->cursorline=p_txtbox->totalrows;
        /* calculate the window start line for new window positon */
        wrow=p_txtbox->cursorline;

        /* back the window start row up a number of lines */
        /* equal to the height of the window */
        if (wrow>=p_txtbox->windowrows) wrow-=p_txtbox->windowrows;
        p_txtbox->windowstartline=wrow;
        break;

        default:
        break;
    }
    /* update the view */
    TxtboxUpdate(txtboxID);

    DebugEnd();
}


T_void TxtboxCursHome  (T_txtboxID txtboxID)
{
    T_txtboxStruct *p_txtbox;
    T_word16 x,y;

    DebugRoutine ("TxtboxCursHome");
    DebugCheck (txtboxID != NULL);

    p_txtbox=(T_txtboxStruct *)txtboxID;
    /* move cursor position x to leftmost screen position possible */
    p_txtbox->cursorl=TxtboxScanRow(txtboxID, 0, 0);

    /* update the view */
    TxtboxUpdate(txtboxID);

    DebugEnd();
}

T_void TxtboxCursEnd  (T_txtboxID txtboxID)
{
    T_txtboxStruct *p_txtbox;
    T_word16 x,y;

    DebugRoutine ("TxtboxCursEnd");
    DebugCheck (txtboxID != NULL);
    p_txtbox=(T_txtboxStruct *)txtboxID;

    if (p_txtbox->mode < TXTBOX_MODE_VIEW_SCROLL_FORM ||
        p_txtbox->mode == TXTBOX_MODE_FIXED_WIDTH_FIELD)
    /* move cursor to the rightmost screen position possible */
    p_txtbox->cursorl=TxtboxScanRow(txtboxID, 0, 320);

    /* update the view */
    TxtboxUpdate(txtboxID);

    DebugEnd();
}


T_void TxtboxCursUp   (T_txtboxID txtboxID)
{
    T_txtboxStruct *p_txtbox;

    DebugRoutine ("TxtboxCursUp");
    DebugCheck (txtboxID != NULL);

    p_txtbox=(T_txtboxStruct *)txtboxID;

    /* if we are already on the first line */
    if (p_txtbox->cursorline==0)
    {
        /* set the cursor to the first character */
        p_txtbox->cursorl=0;
    } else
    {
        switch (p_txtbox->mode)
        {
            case TXTBOX_MODE_EDIT_FIELD:
            case TXTBOX_MODE_EDIT_FORM:
            case TXTBOX_MODE_FIXED_WIDTH_FIELD:
            p_txtbox->cursorl=TxtboxScanRow(txtboxID, -1, p_txtbox->cursorx);
            p_txtbox->cursorline--;
            break;

            case TXTBOX_MODE_VIEW_SCROLL_FORM:
            case TXTBOX_MODE_SELECTION_BOX:
            p_txtbox->cursorline--;
            p_txtbox->cursorl=p_txtbox->linestarts[p_txtbox->cursorline];
            break;

            default:
            break;
        }
    }

    /* check to see if we've moved the window */
    if (p_txtbox->windowstartline > p_txtbox->cursorline)
    {
        p_txtbox->windowstartline--;
    }

    /* update the view */
    TxtboxUpdate(txtboxID);

    DebugEnd();
}


T_void TxtboxCursDn (T_txtboxID txtboxID)
{
    T_txtboxStruct *p_txtbox;
    DebugRoutine ("TxtboxCursDn");
    DebugCheck (txtboxID != NULL);

    p_txtbox=(T_txtboxStruct *)txtboxID;

    switch (p_txtbox->mode)
    {
        case TXTBOX_MODE_EDIT_FIELD:
        case TXTBOX_MODE_FIXED_WIDTH_FIELD:
        case TXTBOX_MODE_EDIT_FORM:
        if (p_txtbox->cursorline>=p_txtbox->totalrows)
        {
            /* we're already at the bottom line */
            /* move the cursor to the last character */
            p_txtbox->cursorline=p_txtbox->totalrows;
            p_txtbox->cursorl=strlen (p_txtbox->data);
        }
        else
        {
            p_txtbox->cursorl=TxtboxScanRow(txtboxID, 1, p_txtbox->cursorx);
            p_txtbox->cursorline++;
        }
        break;

        case TXTBOX_MODE_VIEW_SCROLL_FORM:
        /* move window down a click */
        if (p_txtbox->windowstartline <= p_txtbox->totalrows - p_txtbox->windowrows)
        {
            p_txtbox->windowstartline++;
            p_txtbox->cursorl=p_txtbox->linestarts[p_txtbox->windowstartline];
            p_txtbox->cursorline=p_txtbox->windowstartline;
        }
        break;

        case TXTBOX_MODE_SELECTION_BOX:
        if (p_txtbox->cursorline < p_txtbox->totalrows)
        {
            p_txtbox->cursorline++;
            p_txtbox->cursorl=p_txtbox->linestarts[p_txtbox->cursorline];
        }
        break;

        default:
        break;
    }

    /* check to see if we've moved the window */
    if (p_txtbox->cursorline >= p_txtbox->windowstartline + p_txtbox->windowrows )
    {
        p_txtbox->windowstartline++;
    }

    TxtboxUpdate(txtboxID);

    DebugEnd();
}


T_void TxtboxCursLeft   (T_txtboxID txtboxID)
{
    T_txtboxStruct *p_txtbox;

    DebugRoutine ("TxtboxCursLeft");
    DebugCheck (txtboxID != NULL);

    p_txtbox=(T_txtboxStruct *)txtboxID;
    /* subtract one from the cursor l position */
    if (p_txtbox->cursorl>0)
    {
        p_txtbox->cursorl--;
    }

    /* check to see if we've changed rows */
    if (p_txtbox->cursorline > 0)
    {
        if (p_txtbox->cursorl<p_txtbox->linestarts[p_txtbox->cursorline])
        {
            /* subtract one from the cursor line start */
            p_txtbox->cursorline--;
            if (p_txtbox->windowstartline > p_txtbox->cursorline)
            {
                p_txtbox->windowstartline--;
            }
        }
    }

    TxtboxUpdate(txtboxID);

    DebugEnd();
}


T_void TxtboxCursRight   (T_txtboxID txtboxID)
{
    T_txtboxStruct *p_txtbox;

    DebugRoutine ("TxtboxCursRight");
    DebugCheck (txtboxID != NULL);

    p_txtbox=(T_txtboxStruct *)txtboxID;
    /* add one to the cursor l position */
    if (p_txtbox->cursorl < strlen (p_txtbox->data))
    {
        p_txtbox->cursorl++;
    }

    /* check to see if we've changed rows */
    if (p_txtbox->cursorline < p_txtbox->totalrows)
    {
        if (p_txtbox->cursorl > p_txtbox->linestarts[p_txtbox->cursorline+1])
        {
            /* increment the cursor line */
            p_txtbox->cursorline++;
            /* see if we've moved the window */
            if (p_txtbox->cursorline >= p_txtbox->windowstartline+p_txtbox->windowrows)
            {
                p_txtbox->windowstartline++;
            }
        }
    }

    TxtboxUpdate(txtboxID);

    DebugEnd();
}


T_void TxtboxCursPgUp (T_txtboxID txtboxID)
{
    T_txtboxStruct *p_txtbox;

    DebugRoutine ("TxtboxCursPgUp");
    DebugCheck (txtboxID != NULL);

    p_txtbox=(T_txtboxStruct *)txtboxID;

    switch (p_txtbox->mode)
    {
        case TXTBOX_MODE_EDIT_FIELD:
        case TXTBOX_MODE_FIXED_WIDTH_FIELD:
        case TXTBOX_MODE_EDIT_FORM:
        if (p_txtbox->cursorline > p_txtbox->windowrows)
        {
            p_txtbox->cursorl=TxtboxScanRow(txtboxID, -p_txtbox->windowrows, p_txtbox->cursorx);
            p_txtbox->cursorline-=p_txtbox->windowrows;
            if (p_txtbox->windowstartline < p_txtbox->cursorline)
              p_txtbox->windowstartline = p_txtbox->cursorline;
        } else
        {
            p_txtbox->cursorline=0;
            p_txtbox->windowstartline=0;
            p_txtbox->cursorl=0;
        }
        break;

        case TXTBOX_MODE_VIEW_SCROLL_FORM:
        if (p_txtbox->windowstartline > p_txtbox->windowrows)
        {
            p_txtbox->windowstartline -= p_txtbox->windowrows;
        } else
        {
            p_txtbox->windowstartline = 0;
        }
        p_txtbox->cursorline = p_txtbox->windowstartline;
        p_txtbox->cursorl=p_txtbox->linestarts[p_txtbox->cursorline];
        break;

        case TXTBOX_MODE_SELECTION_BOX:

        if (p_txtbox->cursorline > p_txtbox->windowrows)
        {
            p_txtbox->cursorline -= p_txtbox->windowrows;
            p_txtbox->cursorl=p_txtbox->linestarts[p_txtbox->cursorline];
        } else
        {
            p_txtbox->cursorline=0;
            p_txtbox->cursorl=0;
        }

        /* move viewing window */
        if (p_txtbox->windowstartline < p_txtbox->cursorline)
          p_txtbox->windowstartline = p_txtbox->cursorline;
        break;

        default:
        break;
    }
    /* update the view */
    TxtboxUpdate(txtboxID);

    DebugEnd();
}


T_void TxtboxCursPgDn (T_txtboxID txtboxID)
{
    T_txtboxStruct *p_txtbox;

    DebugRoutine ("TxtboxCursPgDn");
    DebugCheck (txtboxID != NULL);

    p_txtbox=(T_txtboxStruct *)txtboxID;

    switch (p_txtbox->mode)
    {
        case TXTBOX_MODE_EDIT_FIELD:
        case TXTBOX_MODE_FIXED_WIDTH_FIELD:
        case TXTBOX_MODE_EDIT_FORM:
        if (p_txtbox->cursorline+p_txtbox->windowrows > p_txtbox->totalrows)
        {
            p_txtbox->cursorline=p_txtbox->totalrows;
            p_txtbox->windowstartline = p_txtbox->cursorline;
            p_txtbox->cursorl=strlen(p_txtbox->data);
        } else
        {
            p_txtbox->cursorl=TxtboxScanRow(txtboxID, p_txtbox->windowrows, p_txtbox->cursorx);
            p_txtbox->cursorline += p_txtbox->windowrows;
            if (p_txtbox->windowstartline < p_txtbox->cursorline-p_txtbox->windowrows)
              p_txtbox->windowstartline = p_txtbox->cursorline-p_txtbox->windowrows;
        }
        break;

        case TXTBOX_MODE_VIEW_SCROLL_FORM:
        /*move view down a whole screen */
        p_txtbox->windowstartline += p_txtbox->windowrows;
        if (p_txtbox->windowstartline > p_txtbox->totalrows - p_txtbox->windowrows+1)
          p_txtbox->windowstartline = p_txtbox->totalrows - p_txtbox->windowrows+1;
        p_txtbox->cursorline = p_txtbox->windowstartline;
        p_txtbox->cursorl=p_txtbox->linestarts[p_txtbox->cursorline];
        break;

        case TXTBOX_MODE_SELECTION_BOX:

        p_txtbox->cursorline+=p_txtbox->windowrows;
        if (p_txtbox->cursorline > p_txtbox->totalrows)
          p_txtbox->cursorline=p_txtbox->totalrows;

        p_txtbox->cursorl=p_txtbox->linestarts[p_txtbox->cursorline];

        /* move viewing window down if necessary */
        if (p_txtbox->windowstartline + p_txtbox->windowrows < p_txtbox->cursorline)
        {
            p_txtbox->windowstartline = p_txtbox->cursorline - p_txtbox->windowrows;
        }
        break;

        default:
        break;
    }

    /* update the view */
    TxtboxUpdate (txtboxID);

    DebugEnd();
}


T_void TxtboxCursSetRow (T_txtboxID txtboxID, T_word16 row)
{
    T_txtboxStruct *p_txtbox;

    DebugRoutine ("TxtboxCursSetRow");
    DebugCheck (txtboxID != NULL);

    p_txtbox=(T_txtboxStruct *)txtboxID;
    if (row > p_txtbox->totalrows) row=p_txtbox->totalrows;

    /* set row */
    p_txtbox->cursorline=row;
    p_txtbox->cursorl=p_txtbox->linestarts[row];
    if (p_txtbox->windowstartline > row)
     p_txtbox->windowstartline=row;

    /* force update */
    TxtboxUpdate (txtboxID);

    DebugEnd();
}



T_word32 TxtboxScanRow (T_txtboxID txtboxID, T_word16 rowinc, T_word16 ox)
{
    T_txtboxStruct *p_txtbox;
    T_bitfont *p_font;

    T_word16 startrow,startch,startx,targetx,wsize;

    T_word32 i=0,retvalue=0;

    DebugRoutine ("TxtboxScanRow");

    p_txtbox=(T_txtboxStruct *)txtboxID;

    /* open the font */
    p_font = ResourceLock(p_txtbox->font) ;
	GrSetBitFont (p_font);

    wsize=GrGetCharacterWidth('W');

    startrow=p_txtbox->cursorline+rowinc;
    if (startrow > p_txtbox->totalrows) startrow=p_txtbox->totalrows;

    startch=p_txtbox->linestarts[startrow];
    startx=p_txtbox->lx1;
    targetx=ox;

    /* scan the row of text for a near-x cursor positon */
    for (i=startch;i<strlen(p_txtbox->data);i++)
    {
        if (p_txtbox->data[i]==13)
        {
            /* we found a return, set the cursor here */
            retvalue=i;
            break;
        } else if (p_txtbox->data[i]==9)
        {
            /* we have a tab here, advance to the nearest tab position */
            /* currently 3*wsize or 3 capital doubleyous :) */
            startx=((((startx-p_txtbox->lx1)/(3*wsize))+1)*(3*wsize))+p_txtbox->lx1;
            /* make sure we haven't reached the end of the line */
            if (startx+wsize>p_txtbox->lx2)
            {
                /* out of room, set the cursor here */
                retvalue=i;
                break;
            }
        }
        else
        {   /* normal character */
            startx+=GrGetCharacterWidth (p_txtbox->data[i]);
            if (startx+wsize>p_txtbox->lx2)
            {
                /* reached end of line, drop down a row */
                retvalue=i;
                break;
            }
        }

        if (startx>targetx) /* here we are */
        {
            retvalue=i;
            break;
        }
    }

    if (retvalue>=strlen(p_txtbox->data)) retvalue=strlen(p_txtbox->data);


    /* check for total failure */
    if (retvalue==0 && startx < targetx) retvalue=strlen(p_txtbox->data);

    /* close the font */
    ResourceUnlock (p_txtbox->font);

    DebugEnd();

    return (retvalue);
}






/****************************************************************************/
/*  Routine:  TxtboxAppendKey/TxtboxBackSpace                               */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          *//*                                                                          *//*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*                                                                          */
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
/*    JDA  09/05/95  Created                                                */
/*                                                                          */
/****************************************************************************/


T_void TxtboxAppendKey (T_txtboxID txtboxID, T_word16 scankey)
{
    T_txtboxStruct *p_txtbox;
    T_byte8 *newdata;
    T_word32 len;
    T_word32 i;

    DebugRoutine ("TxtboxAppendKey");
    DebugCheck (txtboxID != NULL);

    p_txtbox=(T_txtboxStruct *)txtboxID;

    /* make sure the data string is valid */
    DebugCheck (p_txtbox->data!=NULL);

    /* make sure we have room for another character */
    if ((p_txtbox->isfull==FALSE) &&
        (p_txtbox->numericonly==FALSE || (scankey>='0' && scankey<='9')) &&
        (strlen(p_txtbox->data)<p_txtbox->maxlength))
    {

        /* get the length of the current data string */
        len=strlen(p_txtbox->data);

        /* create a new string with old length +1 */
        newdata=MemAlloc((sizeof(T_byte8))*(len+2));
        MemCheck (1);

        /* copy the old contents, up to the cursorl position to the new data area */
        memcpy (newdata,p_txtbox->data,p_txtbox->cursorl);

        /* insert the new character */
        newdata[p_txtbox->cursorl]=scankey;

        /* copy the rest of the old string to the new one */
        memcpy (newdata+p_txtbox->cursorl+1,
            p_txtbox->data+p_txtbox->cursorl,len+1-p_txtbox->cursorl);

        /* delete the old data string */
        MemFree (p_txtbox->data);
        MemCheck (302);
        p_txtbox->data=NULL;

        /* set the pointer */
        p_txtbox->data=newdata;

        if ((scankey==13) &&
            (p_txtbox->cursorline==p_txtbox->windowstartline+p_txtbox->windowrows-1))
            p_txtbox->windowstartline++;

        if (scankey==9 || scankey==13 || scankey==31)
          TxtboxRepaginateAll (txtboxID);
        /* repaginate the form */
        else TxtboxRepaginate(txtboxID);

        /* add one to the cursor position */
        TxtboxCursRight (txtboxID);

    }
    DebugEnd();
}



T_void TxtboxAppendKeyNoRepag (T_txtboxID txtboxID, T_byte8 scankey)
{
    T_txtboxStruct *p_txtbox;
    T_byte8 *newdata;
    T_word32 len;
    T_word32 i;

    DebugRoutine ("TxtboxAppendKeyNoRepag");
    DebugCheck (txtboxID != NULL);

    p_txtbox=(T_txtboxStruct *)txtboxID;

    /* make sure the data string is valid */
    DebugCheck (p_txtbox->data!=NULL);

    /* make sure we have room for another character */
    if ((p_txtbox->isfull==FALSE) &&
        (p_txtbox->numericonly==FALSE || (scankey>='0' && scankey<='9')) &&
        (strlen(p_txtbox->data)<p_txtbox->maxlength))
    {

        /* get the length of the current data string */
        len=strlen(p_txtbox->data);

        /* create a new string with old length +1 */
        newdata=MemAlloc((sizeof(T_byte8))*(len+2));
        MemCheck (1);

        /* copy the old contents, up to the cursorl position to the new data area */
        memcpy (newdata,p_txtbox->data,p_txtbox->cursorl);

        /* insert the new character */
        newdata[p_txtbox->cursorl]=scankey;

        /* copy the rest of the old string to the new one */
        memcpy (newdata+p_txtbox->cursorl+1,
            p_txtbox->data+p_txtbox->cursorl,len+1-p_txtbox->cursorl);

        /* delete the old data string */
        MemFree (p_txtbox->data);
        p_txtbox->data=NULL;

        /* set the pointer */
        p_txtbox->data=newdata;

        if ((scankey==13) &&
            (p_txtbox->cursorline==p_txtbox->windowstartline+p_txtbox->windowrows-1))
            p_txtbox->windowstartline++;

        if (scankey==9 || scankey==13 || scankey==31)
          TxtboxRepaginate (txtboxID);
        /* repaginate the form */
//        else TxtboxRepaginate(txtboxID);

        /* add one to the cursor position */
        TxtboxCursRight (txtboxID);

    }
    DebugEnd();
}



T_void TxtboxBackSpace (T_txtboxID txtboxID)
{
    T_txtboxStruct *p_txtbox;
    T_byte8 *newdata;
    T_word32 len;
    T_word32 i;

    DebugRoutine ("TxtboxBackSpace");
    DebugCheck (txtboxID != NULL);

    p_txtbox=(T_txtboxStruct *)txtboxID;

    /* make sure the data string is valid */
    DebugCheck (p_txtbox->data!=NULL);

    /* get the length of the current data string */
    len=strlen(p_txtbox->data);

    /* make sure we have a character to delete ! */
    if (len>0 && p_txtbox->cursorl>0)
    {

        /* create a new string with old length -1 */
        newdata=MemAlloc((sizeof(T_byte8))*(len+1));

        /* copy the old contents, up to the cursorl-1 position to the new data area */
        memcpy (newdata,p_txtbox->data,p_txtbox->cursorl-1);

        /* copy the rest of the old string to the new one */
        memcpy (newdata+p_txtbox->cursorl-1,
                p_txtbox->data+p_txtbox->cursorl,
                len-p_txtbox->cursorl+1);

        /* delete the old data string */
        MemFree (p_txtbox->data);
        MemCheck (303);
        p_txtbox->data=NULL;

        /* set the pointer */
        p_txtbox->data=newdata;

        /* repaginate the form */
        TxtboxRepaginateAll(txtboxID);

        /* subtract one to the cursor position */
        TxtboxCursLeft (txtboxID);
    }

    DebugEnd();
}




T_void TxtboxAppendString (T_txtboxID txtboxID, T_byte8 *data)
{
    T_txtboxStruct *p_txtbox;
    T_byte8 ch;
    T_byte8 val;
    T_word16 i;

    DebugRoutine ("TxtboxAppendString");
    DebugCheck (txtboxID != NULL);

    /* scan for control shifted (^) characters */
    for (i=0;i<strlen(data);i++)
    {
        if (data[i]=='^')
        {
            /* found a 'control' char, read in 3 digits */
            /* calculate the value */
            val=0;
            val+=((data[++i]-'0')*100);
            val+=((data[++i]-'0')*10);
            val+=(data[++i]-'0');

            TxtboxAppendKeyNoRepag(txtboxID,val+128);
//tempstr2[j++]=val+128;
        } else TxtboxAppendKeyNoRepag (txtboxID, data[i]);
//tempstr2[j++]=tempstr[i];
    }

//    TxtboxRepaginateAll (txtboxID);
    DebugEnd();
}


T_void TxtboxSetData (T_txtboxID txtboxID, T_byte8 *string)
{
    T_txtboxStruct *p_txtbox;
    T_word16 i,cnt=0;
    T_byte8 val;
    DebugRoutine ("TxtboxSetData");
    p_txtbox=(T_txtboxStruct *)txtboxID;

    /* delete old data string */
    MemFree (p_txtbox->data);
    MemCheck (304);
    p_txtbox->data=NULL;

    /* allocate a new data block the size of the string parameter */
    p_txtbox->data= MemAlloc(sizeof(T_byte8)*strlen(string)+2);

    /* make sure it worked */
    DebugCheck (p_txtbox->data != NULL);

    /* copy the data string */
    for (i=0;i<strlen(string);i++)
    {
        if (string[i]=='^')
        {
            /* found a 'control' char, read in 3 digits */
            /* calculate the value */
            val=0;
            val+=((string[++i]-'0')*100);
            val+=((string[++i]-'0')*10);
            val+=(string[++i]-'0');

            p_txtbox->data[cnt++]=val+128;
        } else p_txtbox->data[cnt++]=string[i];
    }
    p_txtbox->data[cnt]='\0';

    /* move the cursor to the top */
    p_txtbox->cursorl=0;
    p_txtbox->cursorline=0;
    p_txtbox->windowstartline=0;

    /* repaginate the form */
    TxtboxRepaginateAll (txtboxID);

    /* force a screen update */
    TxtboxUpdate (txtboxID);

    DebugEnd();
}

T_void TxtboxSetNData (T_txtboxID txtboxID, T_byte8 *string, T_word32 len)
{
    T_txtboxStruct *p_txtbox;
    T_word16 i,cnt=0;
    T_byte8 val;
    DebugRoutine ("TxtboxSetData");
    p_txtbox=(T_txtboxStruct *)txtboxID;

    /* delete old data string */
    MemFree (p_txtbox->data);
    MemCheck (304);
    p_txtbox->data=NULL;

    /* allocate a new data block the size of the string parameter */
    p_txtbox->data= MemAlloc(sizeof(T_byte8)*len+2);

    /* make sure it worked */
    DebugCheck (p_txtbox->data != NULL);

    /* copy the data string */
    for (i=0;i<len;i++)
    {
        if (string[i]=='^')
        {
            /* found a 'control' char, read in 3 digits */
            /* calculate the value */
            val=0;
            val+=((string[++i]-'0')*100);
            val+=((string[++i]-'0')*10);
            val+=(string[++i]-'0');

            p_txtbox->data[cnt++]=val+128;
        } else p_txtbox->data[cnt++]=string[i];
    }
    p_txtbox->data[cnt]='\0';

    /* move the cursor to the top */
    p_txtbox->cursorl=0;
    p_txtbox->cursorline=0;
    p_txtbox->windowstartline=0;

    /* repaginate the form */
    TxtboxRepaginateAll (txtboxID);

    /* force a screen update */
    TxtboxUpdate (txtboxID);

    DebugEnd();
}





T_byte8 *TxtboxGetData (T_txtboxID txtboxID)
{
    T_txtboxStruct *p_txtbox;

    DebugRoutine ("TxtboxGetData");
    DebugCheck (txtboxID != NULL);
    p_txtbox=(T_txtboxStruct *)txtboxID;

    DebugEnd();
    return (p_txtbox->data);
}

T_word32 TxtboxGetDataLength (T_txtboxID txtboxID)
{
    T_txtboxStruct *p_txtbox;

    DebugRoutine ("TxtboxGetDataLength");
    DebugCheck (txtboxID != NULL);
    p_txtbox=(T_txtboxStruct *)txtboxID;

    DebugEnd();
    return (strlen(p_txtbox->data));
}


T_void TxtboxSetColor (T_txtboxID txtboxID,
                       T_byte8 txtcolor,
                       T_byte8 bkcolor,
                       T_byte8 txtshadow,
                       T_byte8 bordclr1,
                       T_byte8 bordclr2)
{
    T_txtboxStruct *p_txtbox;

    DebugRoutine ("TxtboxSetColor");
    DebugCheck (txtboxID != NULL);

    p_txtbox=(T_txtboxStruct *)txtboxID;

    p_txtbox->textcolor=txtcolor;
    p_txtbox->backcolor=bkcolor;
    p_txtbox->textshadow=txtshadow;
    p_txtbox->bordercolor1=bordclr1,
    p_txtbox->bordercolor2=bordclr2;

    /* force an update */
    TxtboxUpdate (txtboxID);

    DebugEnd();
}




/****************************************************************************/
/*  Routine:  TxtboxIsAt                                                    */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*  Returns TRUE if a text box is at the given coordinates                  */
/*  Otherwise returns FALSE                                                 */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*  lx and ly of test                                                       */
/*                                                                          */
/*                                                                          *//*                                                                          *//*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*  E_boolean                                                               */
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
/*    JDA  09/05/95  Created                                                */
/*                                                                          */
/****************************************************************************/


E_Boolean TxtboxIsAt (T_txtboxID txtboxID, T_word16 x, T_word16 y)
{
    T_txtboxStruct *p_txtbox;
    E_Boolean retvalue=FALSE;

    DebugRoutine ("TxtboxIsAt");
    DebugCheck (txtboxID != NULL);

    p_txtbox=(T_txtboxStruct *)txtboxID;

    if ((x>=p_txtbox->lx1) &&
        (x<=p_txtbox->lx2) &&
        (y>=p_txtbox->ly1) &&
        (y<=p_txtbox->ly2)) retvalue=TRUE;

    DebugEnd();

    return (retvalue);
}

/****************************************************************************/
/*  Routine:  TxtboxHandleKey/TxtboxHandleMouse                             */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          *//*                                                                          *//*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*                                                                          */
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
/*    JDA  09/05/95  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void TxtboxKeyControl (E_keyboardEvent event, T_word16 scankey)
{

    T_txtboxStruct *p_txtbox;
    T_byte8 *data,*tempstr;
    DebugRoutine ("TxtboxKeyControl");

    p_txtbox=(T_txtboxStruct *)G_txtboxArray[G_currentTextBox];

    G_currentAction = TXTBOX_ACTION_NO_ACTION;

    switch (event)
    {
        case KEYBOARD_EVENT_BUFFERED:

        if (scankey==8)
        {
            /* recieved a backspace, see what to do with it */
            if (p_txtbox->mode<TXTBOX_MODE_VIEW_SCROLL_FORM ||
                p_txtbox->mode==TXTBOX_MODE_FIXED_WIDTH_FIELD)
            {
                TxtboxBackSpace (G_txtboxArray[G_currentTextBox]);
                G_currentAction = TXTBOX_ACTION_DATA_CHANGED;
            }
        }
        else if (scankey>31 && scankey < 128)
        {
            if (p_txtbox->mode<TXTBOX_MODE_VIEW_SCROLL_FORM)
            {
                TxtboxAppendKey(G_txtboxArray[G_currentTextBox],scankey);
                G_currentAction = TXTBOX_ACTION_DATA_CHANGED;
            }
            else if (p_txtbox->mode==TXTBOX_MODE_FIXED_WIDTH_FIELD)
            {
                /* see if the new character will fit */
                data=TxtboxGetData(G_txtboxArray[G_currentTextBox]);
                tempstr=MemAlloc(strlen(data)+4);
                sprintf (tempstr,"%sWW",data);
                if (TxtboxCanFit (G_txtboxArray[G_currentTextBox],tempstr)==strlen(tempstr))
                {
                    TxtboxAppendKey(G_txtboxArray[G_currentTextBox],scankey);
                }
                MemFree(tempstr);
            }
        }
        else if (scankey==9)
        {
            /* we've got a tab, figure out what to do with it */
            switch (p_txtbox->mode)
            {
                /* in these modes the tab key will advance a field */
                case TXTBOX_MODE_EDIT_FIELD:
                case TXTBOX_MODE_FIXED_WIDTH_FIELD:
                case TXTBOX_MODE_VIEW_SCROLL_FORM:
                case TXTBOX_MODE_SELECTION_BOX:
                if (KeyboardGetScanCode(KEY_SCAN_CODE_LEFT_SHIFT)==TRUE ||
                    KeyboardGetScanCode(KEY_SCAN_CODE_RIGHT_SHIFT)==TRUE)
                {
                    /* shift is being held, back up a field */
                    TxtboxLastBox();
                }
                else
                {
                    TxtboxNextBox();
                }
                p_txtbox=(T_txtboxStruct *)G_txtboxArray[G_currentTextBox];
                G_currentAction = TXTBOX_ACTION_GAINED_FOCUS;
                break;

                /* in these modes, ctrl-shift is needed to advance a field */
                case TXTBOX_MODE_EDIT_FORM:
                if (KeyboardGetScanCode(KEY_SCAN_CODE_LEFT_CTRL)==TRUE ||
                    KeyboardGetScanCode(KEY_SCAN_CODE_RIGHT_CTRL)==TRUE)
                {
                    /* ctrl is being pushed, move to the next or last field */
                    if (KeyboardGetScanCode(KEY_SCAN_CODE_LEFT_SHIFT)==TRUE ||
                        KeyboardGetScanCode(KEY_SCAN_CODE_RIGHT_SHIFT)==TRUE)
                    {
                        /* shift is being held, back up a field */
                        TxtboxLastBox();  \
                    }
                    else
                    {
                        TxtboxNextBox();
                    }
                    p_txtbox=(T_txtboxStruct *)G_txtboxArray[G_currentTextBox];
                    G_currentAction = TXTBOX_ACTION_GAINED_FOCUS;
                }
                else  /* ctrl is not being held, add a tab to the field */
                {
                    TxtboxAppendKey(G_txtboxArray[G_currentTextBox],scankey);
                    G_currentAction = TXTBOX_ACTION_DATA_CHANGED;
                }
                break;
                default:
                break;
            }
        }
        else if (scankey==13)
        {
            /* we've got a return here, figure out what to do with it */
            switch (p_txtbox->mode)
            {
                /* in these modes the return key will advance a field */
                case TXTBOX_MODE_EDIT_FIELD:
                case TXTBOX_MODE_FIXED_WIDTH_FIELD:
                case TXTBOX_MODE_VIEW_SCROLL_FORM:
                G_currentAction = TXTBOX_ACTION_ACCEPTED;
                if (p_txtbox->txtboxcallback != NULL)
                {
                    p_txtbox->txtboxcallback (G_txtboxArray[G_currentTextBox]);
                }

//                TxtboxNextBox();
                p_txtbox=(T_txtboxStruct *)G_txtboxArray[G_currentTextBox];
                G_currentAction = TXTBOX_ACTION_GAINED_FOCUS;
                break;

                /* in these modes, return is sent as a character */
                case TXTBOX_MODE_EDIT_FORM:
                TxtboxAppendKey(G_txtboxArray[G_currentTextBox],scankey);
                G_currentAction = TXTBOX_ACTION_DATA_CHANGED;
                default:
                break;
            }
        }
        case KEYBOARD_EVENT_HELD:
        break ;

        case KEYBOARD_EVENT_PRESS:

        if (scankey==KEY_SCAN_CODE_LEFT)
        {
            if (p_txtbox->mode<TXTBOX_MODE_VIEW_SCROLL_FORM ||
                p_txtbox->mode==TXTBOX_MODE_FIXED_WIDTH_FIELD)
            {
                TxtboxCursLeft (G_txtboxArray[G_currentTextBox]);
                G_currentAction = TXTBOX_ACTION_SELECTION_CHANGED;
            }
        }
        else if (scankey==KEY_SCAN_CODE_RIGHT)
        {
            if (p_txtbox->mode<TXTBOX_MODE_VIEW_SCROLL_FORM ||
                p_txtbox->mode==TXTBOX_MODE_FIXED_WIDTH_FIELD)
            {
                TxtboxCursRight (G_txtboxArray[G_currentTextBox]);
                G_currentAction = TXTBOX_ACTION_SELECTION_CHANGED;
            }
        }
        else if (scankey==KEY_SCAN_CODE_UP)
        {
            TxtboxCursUp (G_txtboxArray[G_currentTextBox]);
            G_currentAction = TXTBOX_ACTION_SELECTION_CHANGED;
        }
        else if (scankey==KEY_SCAN_CODE_DOWN)
        {
            TxtboxCursDn (G_txtboxArray[G_currentTextBox]);
            G_currentAction = TXTBOX_ACTION_SELECTION_CHANGED;
        }
        else if (scankey==KEY_SCAN_CODE_PGUP)
        {
            TxtboxCursPgUp (G_txtboxArray[G_currentTextBox]);
            G_currentAction = TXTBOX_ACTION_SELECTION_CHANGED;
        }
        else if (scankey==KEY_SCAN_CODE_PGDN)
        {
            TxtboxCursPgDn (G_txtboxArray[G_currentTextBox]);
            G_currentAction = TXTBOX_ACTION_SELECTION_CHANGED;
        }
        else if (scankey==KEY_SCAN_CODE_HOME)
        {
            if (KeyboardGetScanCode (KEY_SCAN_CODE_CTRL)==TRUE)
            {
               TxtboxCursTop (G_txtboxArray[G_currentTextBox]);
            }
            else
            {
                TxtboxCursHome (G_txtboxArray[G_currentTextBox]);
            }
            G_currentAction = TXTBOX_ACTION_SELECTION_CHANGED;
        }
        else if (scankey==KEY_SCAN_CODE_END)
        {
            if (KeyboardGetScanCode (KEY_SCAN_CODE_CTRL)==TRUE)
            {
               TxtboxCursBot (G_txtboxArray[G_currentTextBox]);
            }
            else
            {
                TxtboxCursEnd (G_txtboxArray[G_currentTextBox]);
            }
            G_currentAction = TXTBOX_ACTION_SELECTION_CHANGED;
        }
        else if (scankey==KEY_SCAN_CODE_DELETE)
        {
            if (p_txtbox->mode<TXTBOX_MODE_VIEW_SCROLL_FORM ||
                p_txtbox->mode==TXTBOX_MODE_FIXED_WIDTH_FIELD)
            {
                /* make sure we have a character to delete */
                if (p_txtbox->cursorl<strlen(p_txtbox->data))
                {
                    /* move cursor right */
                    TxtboxCursRight (G_txtboxArray[G_currentTextBox]);
                    /* backspace once */
                    TxtboxBackSpace (G_txtboxArray[G_currentTextBox]);
                }
                G_currentAction = TXTBOX_ACTION_DATA_CHANGED;
            }
        }
        break;

        default:
        break;
    }

    if (p_txtbox->txtboxcallback != NULL)
    {
        p_txtbox->txtboxcallback (G_txtboxArray[G_currentTextBox]);
    }

    DebugEnd();
}


T_void TxtboxMouseControl (E_mouseEvent event, T_word16 x, T_word16 y, T_buttonClick button)
{
    T_txtboxStruct *p_txtbox,*p_txtbox2;
    static T_txtboxID *selected,*selectedsb;
    T_sword16 row;
    T_word16 i;

    DebugRoutine ("TxtboxMouseControl");


    switch (event)
    {
        case MOUSE_EVENT_START:
        selectedsb=NULL;
        selected=NULL;

        for (i=0;i<MAX_TXTBOXES;i++)
        {
            if (G_txtboxArray[i]!=NULL)
            {
                p_txtbox=(T_txtboxStruct *)G_txtboxArray[i];
                if (p_txtbox->sbgrID!=NULL)
                {
                    if (GraphicIsAt(p_txtbox->sbgrID,x,y))
                    {
                        TxtboxMoveSB (G_txtboxArray[i],y);
                        /* set selected scroll bar for dragging */
                        selectedsb=G_txtboxArray[i];
                        G_currentAction = TXTBOX_ACTION_SELECTION_CHANGED;
                        if (p_txtbox->txtboxcallback != NULL) p_txtbox->txtboxcallback (G_txtboxArray[i]);
                    }
                }

                if (TxtboxIsAt(G_txtboxArray[i],x,y))
                {
                    if (p_txtbox->mode<TXTBOX_MODE_VIEW_NOSCROLL_FORM ||
                        p_txtbox->mode>TXTBOX_MODE_VIEW_NOSCROLL_FORM)
                    {
                        /* select this text box */
                        p_txtbox2=(T_txtboxStruct *)G_txtboxArray[G_currentTextBox];
                        p_txtbox2->isselected=FALSE;
                        G_currentAction = TXTBOX_ACTION_LOST_FOCUS;
                        if (p_txtbox2->txtboxcallback != NULL) p_txtbox2->txtboxcallback (G_txtboxArray[G_currentTextBox]);

                        if (G_txtboxArray[G_currentTextBox] != NULL)
                          TxtboxUpdate (G_txtboxArray[G_currentTextBox]);
                        p_txtbox->isselected=TRUE;
                        G_currentTextBox=i;
                        G_currentAction = TXTBOX_ACTION_GAINED_FOCUS;
                        if (p_txtbox->txtboxcallback != NULL) p_txtbox->txtboxcallback (G_txtboxArray[i]);

                        /* set selected */
                        selected=G_txtboxArray[i];

                        /* move cursor to selected row or character */
                        switch (p_txtbox->mode)
                        {
                            case TXTBOX_MODE_EDIT_FORM:
                            case TXTBOX_MODE_FIXED_WIDTH_FIELD:
                            case TXTBOX_MODE_EDIT_FIELD:
                            /* determine row, col pointed at */
                            row=(y-p_txtbox->ly1)/p_txtbox->fontheight;
                            p_txtbox->cursorline=p_txtbox->windowstartline+row;
                            if (p_txtbox->cursorline>p_txtbox->totalrows) p_txtbox->cursorline=p_txtbox->totalrows;
                            p_txtbox->cursorl=TxtboxScanRow (G_txtboxArray[i],0,x);
                            break;

                            case TXTBOX_MODE_SELECTION_BOX:
                            G_currentAction = TXTBOX_ACTION_SELECTION_CHANGED;
                            /* determine the row pointed at */
                            row=(y-p_txtbox->ly1)/p_txtbox->fontheight;
                            p_txtbox->cursorline=p_txtbox->windowstartline+row;
                            if (p_txtbox->cursorline>p_txtbox->totalrows)
                              p_txtbox->cursorline=p_txtbox->totalrows;
                            p_txtbox->cursorl=p_txtbox->linestarts[p_txtbox->cursorline];
                            /* notify callback */
                            if (p_txtbox->txtboxcallback != NULL) p_txtbox->txtboxcallback (G_txtboxArray[i]);
                            break;

                            default:
                            break;
                        }

                        if (G_txtboxArray[i]!=NULL) TxtboxUpdate (G_txtboxArray[i]);
                        break;
                    }
                }
            }
        }
        break;

        case MOUSE_EVENT_DRAG:
        case MOUSE_EVENT_HELD:
        if (selected != NULL)
        {
            if (!TxtboxValidateID (selected))
            {
                selected=NULL;
                break;
            }

            p_txtbox=(T_txtboxStruct *)selected;
            switch (p_txtbox->mode)
            {
                case TXTBOX_MODE_EDIT_FORM:
                case TXTBOX_MODE_EDIT_FIELD:
                case TXTBOX_MODE_FIXED_WIDTH_FIELD:
                break;

                case TXTBOX_MODE_SELECTION_BOX:

                if (y < p_txtbox->ly1)
                {
                    if (p_txtbox->cursorline > p_txtbox->windowstartline)
                      p_txtbox->cursorline=p_txtbox->windowstartline;

                    if (p_txtbox->cursorline > 0)
                    {
                        p_txtbox->cursorline--;
                        p_txtbox->cursorl=p_txtbox->linestarts[p_txtbox->cursorline];
                        if (p_txtbox->windowstartline > p_txtbox->cursorline)
                          p_txtbox->windowstartline --;
                    } else
                    {
                        p_txtbox->cursorline=0;
                        p_txtbox->cursorl=0;
                        if (p_txtbox->windowstartline > 0) p_txtbox->windowstartline--;
                    }
                }
                else if (y>p_txtbox->ly2)
                {
                    if (p_txtbox-> cursorline < p_txtbox->windowstartline + p_txtbox->windowrows+1)
                    {
                        p_txtbox->cursorline = p_txtbox->windowstartline + p_txtbox->windowrows+1;
                        if (p_txtbox->cursorline > p_txtbox->totalrows) p_txtbox->cursorline=p_txtbox->totalrows;
                        p_txtbox->cursorl=p_txtbox->linestarts[p_txtbox->cursorline];
                    }

                    if (p_txtbox-> cursorline < p_txtbox->totalrows)
                    {
                        p_txtbox->cursorline++;
                        p_txtbox->cursorl=p_txtbox->linestarts[p_txtbox->cursorline];
                    }
                }
                else
                {
                    row=(y-p_txtbox->ly1)/p_txtbox->fontheight;
                    p_txtbox->cursorline=p_txtbox->windowstartline+row;
                    if (p_txtbox->cursorline>p_txtbox->totalrows)
                       p_txtbox->cursorline=p_txtbox->totalrows;
                    p_txtbox->cursorl=p_txtbox->linestarts[p_txtbox->cursorline];
                }

                TxtboxUpdate (selected);
                /* notify callback */
                G_currentAction = TXTBOX_ACTION_SELECTION_CHANGED;
                if (p_txtbox->txtboxcallback != NULL) p_txtbox->txtboxcallback (selected);
                break;

                default:
                break;
            }
        }

        if (selectedsb != NULL)
        {
            if (!TxtboxValidateID(selectedsb)) selectedsb=NULL;
            else
            /* move selected scroll bar */
            TxtboxMoveSB (selectedsb,y);
        }

        break;

        case MOUSE_EVENT_IDLE:
        default:
        selected=NULL;
        selectedsb=NULL;

        break;
    }
    DebugEnd();
}


/****************************************************************************/
/*  Routine:  TxtboxNextBox/TxtboxLastBox                                   */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*  Increments or decrements G_currentTextBox                               */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          *//*                                                                          *//*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*                                                                          */
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
/*    JDA  09/05/95  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void TxtboxNextBox (T_void)
{
	T_word16 oldtxtbox;
	T_txtboxStruct *p_txtbox;

    DebugRoutine ("TxtboxNextBox");
	oldtxtbox=G_currentTextBox;
	/* turn off the cursor of the old text field */
	p_txtbox=(T_txtboxStruct*)G_txtboxArray[oldtxtbox];

    /* report lost focus */
    G_currentAction = TXTBOX_ACTION_LOST_FOCUS;

    if (p_txtbox->txtboxcallback != NULL)
    {
        p_txtbox->txtboxcallback (G_txtboxArray[G_currentTextBox]);
    }

	p_txtbox->isselected=FALSE;
	/* force redraw the old text field so the cursor goes away*/
	TxtboxUpdate (G_txtboxArray[oldtxtbox]);
    /* increment the pointer to the active box */
	G_currentTextBox++;
	if (G_currentTextBox>=MAX_TXTBOXES) G_currentTextBox=0;
	/* scan through txtboxarray list until a valid txtbox is found */
	/* or we pass the original point */
	while (G_txtboxArray[G_currentTextBox]==NULL)
	{
		G_currentTextBox++;
		if (G_currentTextBox>=MAX_TXTBOXES) G_currentTextBox=0;
		if (G_currentTextBox==oldtxtbox) break; /* we looped around */
	}

    /* skip this field if it is not editable */
    p_txtbox=(T_txtboxStruct*)G_txtboxArray[G_currentTextBox];

    DebugEnd();

    if (p_txtbox->mode==TXTBOX_MODE_VIEW_NOSCROLL_FORM)
    {
        TxtboxNextBox();
        /* recursive call */
    } else
    {
        p_txtbox->isselected=TRUE;
        TxtboxRepaginateAll (G_txtboxArray[G_currentTextBox]);
        TxtboxUpdate (G_txtboxArray[G_currentTextBox]);
    }
}


T_void TxtboxLastBox (T_void)
{
	T_word16 oldtxtbox;
	T_txtboxStruct *p_txtbox;

    DebugRoutine ("TxtboxLastBox");

	oldtxtbox=G_currentTextBox;
	/* turn off the cursor of the old text field */
	p_txtbox=(T_txtboxStruct*)G_txtboxArray[oldtxtbox];

    /* report lost focus */
    G_currentAction = TXTBOX_ACTION_LOST_FOCUS;

    if (p_txtbox->txtboxcallback != NULL)
    {
        p_txtbox->txtboxcallback (G_txtboxArray[G_currentTextBox]);
    }

	p_txtbox->isselected=FALSE;
	/* force redraw the old text field so the cursor goes away*/
	TxtboxUpdate (G_txtboxArray[oldtxtbox]);
    /* increment the pointer to the active box */
	G_currentTextBox--;
	if (G_currentTextBox>=MAX_TXTBOXES) G_currentTextBox=MAX_TXTBOXES;
	/* scan through txtboxarray list until a valid txtbox is found */
	/* or we pass the original point */
	while (G_txtboxArray[G_currentTextBox]==NULL)
	{
		G_currentTextBox--;
		if (G_currentTextBox>=MAX_TXTBOXES) G_currentTextBox=MAX_TXTBOXES;
		if (G_currentTextBox==oldtxtbox) break; /* we looped around */
	}

    /* skip this field if it is not editable */
    p_txtbox=(T_txtboxStruct*)G_txtboxArray[G_currentTextBox];

    DebugEnd();

    if (p_txtbox->mode==TXTBOX_MODE_VIEW_NOSCROLL_FORM)
    {
        TxtboxLastBox();
        /* recursive call */
    } else
    {
        p_txtbox->isselected=TRUE;
        TxtboxRepaginateAll (G_txtboxArray[G_currentTextBox]);
        TxtboxUpdate (G_txtboxArray[G_currentTextBox]);
    }
}


T_void TxtboxFirstBox (T_void)
{
    T_txtboxStruct *p_txtbox;
    T_word16 i;

    DebugRoutine ("TxtboxFirstBox");

    for (i=0;i<MAX_TXTBOXES;i++)
    {
        if (G_txtboxArray[i] != NULL)
        {
            p_txtbox=(T_txtboxStruct *)G_txtboxArray[i];
            if (p_txtbox->mode != TXTBOX_MODE_VIEW_NOSCROLL_FORM)
            {
                p_txtbox->isselected=TRUE;
                G_currentTextBox=i;
                break;
            }
        }
    }

    DebugEnd();
}

/****************************************************************************/
/*  Routine:  TxtboxUpdate                                                  */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          *//*                                                                          *//*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*                                                                          */
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
/*    JDA  09/05/95  Created                                                */
/*                                                                          */
/****************************************************************************/

T_void TxtboxUpdate (T_txtboxID txtboxID)
{
    T_txtboxStruct *p_txtbox;
    T_graphicStruct *p_graphic;

    DebugRoutine ("TxtboxUpdate");
    DebugCheck (txtboxID != NULL);

    p_txtbox=(T_txtboxStruct *)txtboxID;
    p_graphic=(T_graphicStruct *)p_txtbox->p_graphicID;

    p_graphic->changed=TRUE;

    DebugEnd();
}


T_void TxtboxDrawCallBack(T_graphicID graphicID, T_word16 index)
{
    T_word32 i,j,k;
    T_word16 wsize,width;
    T_word16 curposx, curposy;
    T_word16 startline,endline;
    T_word32 loopstart,loopend;
    T_word16 newcolor;
    T_graphicStruct *p_graphic;
    T_txtboxStruct *p_txtbox;
    T_txtboxID txtboxID;
    T_bitfont *p_font;
    T_byte8 bcolor1, bcolor2, pcolor;
    T_byte8 tempstr[10];
    E_Boolean cursordrawn=FALSE;
    T_byte8 linespassed=0;
    T_screen tempscreen,oldscreen;

    DebugRoutine ("TxtboxDrawCallBack");
    DebugCheck (graphicID != NULL);

    txtboxID=G_txtboxArray[index];
    DebugCheck (txtboxID != NULL);
    p_txtbox=(T_txtboxStruct *)txtboxID;

    /* highlight if selected */
    if (p_txtbox->isselected==TRUE)
    {
        bcolor1=p_txtbox->hbordercolor1;
        bcolor2=p_txtbox->hbordercolor2;
        pcolor=p_txtbox->hbackcolor;
    } else
    {
        bcolor1=p_txtbox->bordercolor1;
        bcolor2=p_txtbox->bordercolor2;
        pcolor=p_txtbox->backcolor;
    }

    /* allocate a temporary drawing field */
//    tempscreen=GrScreenAlloc();
//    oldscreen=GrScreenGet();
//    GrScreenSet (tempscreen);
//    GraphicDrawToCurrentScreen();
    /* first, draw the background box */
    GrDrawRectangle (p_txtbox->lx1,
                     p_txtbox->ly1,
                     p_txtbox->lx2,
                     p_txtbox->ly2,
                     pcolor);

    /* draw window border striping */
    GrDrawHorizontalLine (p_txtbox->lx1,
                          p_txtbox->ly1,
                          p_txtbox->lx2-1,
                          bcolor1);

    GrDrawVerticalLine   (p_txtbox->lx1,
                          p_txtbox->ly1,
                          p_txtbox->ly2-1,
                          bcolor1);

    GrDrawHorizontalLine (p_txtbox->lx1+1,
                          p_txtbox->ly2,
                          p_txtbox->lx2,
                          bcolor2);

    GrDrawVerticalLine   (p_txtbox->lx2,
                          p_txtbox->ly1+1,
                          p_txtbox->ly2,
                          bcolor2);

    /* open the font */
    p_font = ResourceLock(p_txtbox->font) ;
	GrSetBitFont (p_font);

    wsize=GrGetCharacterWidth ('W');

    /* check to make sure the cursor is in the window */
    if (p_txtbox->cursorline < p_txtbox->windowstartline)
    {
        p_txtbox->windowstartline=p_txtbox->cursorline;
    } else if (p_txtbox->cursorline >= (p_txtbox->windowstartline+p_txtbox->windowrows))
    {
        p_txtbox->windowstartline++;
    }

    startline=p_txtbox->windowstartline;
    endline=p_txtbox->windowstartline+p_txtbox->windowrows;
    if (endline>p_txtbox->totalrows) endline=p_txtbox->totalrows;

//    bcolor1=p_txtbox->textcolor;
//    bcolor2=p_txtbox->textshadow;

    /* figure out our last color */
    for (i=0;i<p_txtbox->linestarts[startline];i++)
    {
        if (p_txtbox->data[i]>128)
        {
            newcolor=p_txtbox->data[i]-128;
            if (newcolor < MAX_EXTENDED_COLORS)
            {
                newcolor=G_extendedColors[newcolor];
                p_txtbox->textcolor=newcolor;
                p_txtbox->htextcolor=newcolor;
             }
        }
    }

    /* loop through the data lines, drawing each line */
    for (i=startline; i<=endline; i++)
    {
        /* set the y cursor start position */
        curposy=p_txtbox->ly1+(i-startline)*p_txtbox->fontheight;

        /* set the x cursor start position */
        if (p_txtbox->justify==TXTBOX_JUSTIFY_CENTER)
        {
            curposx=((p_txtbox->lx2-p_txtbox->lx1)/2)-(p_txtbox->linewidths[i]/2)+p_txtbox->lx1;
        } else
        {
            curposx=p_txtbox->lx1;
        }

        /* loop through each character in the line, drawing as we go */
        loopstart=p_txtbox->linestarts[i];
        if (i+1>p_txtbox->totalrows) loopend=strlen(p_txtbox->data);
        else loopend=p_txtbox->linestarts[i+1];

        /* set the color if selection box */
//        if (p_txtbox->mode==TXTBOX_MODE_SELECTION_BOX && i==p_txtbox->cursorline)
//        {
//            p_txtbox->textcolor=47;
//            p_txtbox->textshadow=53;
//        }

        if (i==endline) loopend++;

        for (j=loopstart;j<loopend;j++)
        {

            if (curposy+p_txtbox->fontheight > p_txtbox->ly2) break;

            /* draw the cursor if here */
            if (p_txtbox->cursorl==j)
            {
                if (p_txtbox->mode==TXTBOX_MODE_SELECTION_BOX)
                {
                    /* draw a highlighted line */
                    GrDrawRectangle (p_txtbox->lx1+1,
                                     curposy+1,
                                     p_txtbox->lx2-1,
                                     curposy+p_txtbox->fontheight,
                                     215);
                }
                else if (p_txtbox->isselected==TRUE)
                {
                    if (p_txtbox->mode<TXTBOX_MODE_VIEW_SCROLL_FORM ||
                        p_txtbox->mode==TXTBOX_MODE_FIXED_WIDTH_FIELD)
                    {
                        /* draw a cursor box */
                        for (k=0;k<p_txtbox->fontheight;k++)
                        {
			                GrDrawHorizontalLine (curposx+1,
					                      curposy+k+1,
								          curposx+wsize,
								          226+k);
                        }
                    }
		        }
                p_txtbox->cursorx=curposx;
                p_txtbox->cursory=curposy;
                p_txtbox->cursorline=i;
            }

            /* examine each character and draw */
            if (p_txtbox->data[j]==9)
            {
                /* we have a tab here, advance to the nearest tab position */
                /* currently 3*wsize or 3 capital doubleyous :) */
                curposx=((((curposx-p_txtbox->lx1)/(3*wsize))+1)*(3*wsize))+p_txtbox->lx1;
            } else if (p_txtbox->data[j]==13)
            {

            } else if (p_txtbox->data[j]>31 && p_txtbox->data[j]<128)
            {
                /* must be a normal character */
                tempstr[0]=p_txtbox->data[j];
                tempstr[1]='\0';

                GrSetCursorPosition (curposx+1,curposy+1);

                if (j==p_txtbox->cursorl && p_txtbox->isselected==TRUE && (p_txtbox->mode <= TXTBOX_MODE_EDIT_FORM ||
                                                                           p_txtbox->mode==TXTBOX_MODE_FIXED_WIDTH_FIELD))
                {
                    /* cursor is over a character, change character color */
                    GrDrawShadowedText (tempstr,240,0);
                } else
                {
                    if (p_txtbox->mode==TXTBOX_MODE_SELECTION_BOX && i==p_txtbox->cursorline)
                    {
                        GrDrawShadowedText (tempstr,29,0);
                    }
                    else if (p_txtbox->isselected==TRUE)
                    {
                        GrDrawShadowedText (tempstr,p_txtbox->htextcolor,p_txtbox->textshadow);
                    } else GrDrawShadowedText (tempstr,p_txtbox->textcolor,p_txtbox->textshadow);
                }
                curposx+=GrGetCharacterWidth (p_txtbox->data[j]);
            } else if (p_txtbox->data[j]>128)
            {
                newcolor=p_txtbox->data[j]-128;
                if (newcolor < MAX_EXTENDED_COLORS)
                {
                    newcolor=G_extendedColors[newcolor];
                    p_txtbox->textcolor=newcolor;
                    p_txtbox->htextcolor=newcolor;
                }
            }
        }

        /* restore the text colors */
//        p_txtbox->textcolor=bcolor1;
//        p_txtbox->textshadow=bcolor2;
    }

    /* close the font */
    /* close the font */
    ResourceUnlock (p_txtbox->font);

    /* copy the contents of the temporary screen area to the visual one */
//    GrTransferRectangle (oldscreen,
//                         p_txtbox->lx1,
//                         p_txtbox->ly1,
//                         p_txtbox->lx2,
//                         p_txtbox->ly2,
//                         p_txtbox->lx1,
//                         p_txtbox->ly1);

//    GrScreenSet (oldscreen);
//    GrScreenFree (tempscreen);
//    GraphicDrawToActualScreen();
    /* update the scroll bar if applicable */
    if (p_txtbox->sbgrID != NULL)
    {
        TxtboxUpdateSB (txtboxID);
    }

    DebugEnd();
}



T_void TxtboxRepaginateAll (T_txtboxID txtboxID)
{
    T_txtboxStruct *p_txtbox;
    T_word16 templn;

    DebugRoutine ("TxtboxRepaginateAll");
    DebugCheck (txtboxID != NULL);

    p_txtbox=(T_txtboxStruct *)txtboxID;

    templn=p_txtbox->cursorline;
    p_txtbox->cursorline=0;
    TxtboxRepaginate (txtboxID);
    p_txtbox->cursorline=templn;

    DebugEnd();
}


T_void TxtboxRepaginate (T_txtboxID txtboxID)
{
    T_word16 i,j;
    T_word16 wsize;
    T_word16 linecnt;
    T_word16 curposx, curposy;
    T_txtboxStruct *p_txtbox;
    T_bitfont *p_font;
    static T_word16 cnt=0;
    T_byte8 stmp[64];
    T_word32 end,tcurposx;

    DebugRoutine ("TxtboxRepaginate");
    DebugCheck (txtboxID != NULL);

    p_txtbox=(T_txtboxStruct *)txtboxID;

    /* open the font */
    p_font = ResourceLock(p_txtbox->font) ;
	GrSetBitFont (p_font);

    /* set the tab size using the character 'w' */

    wsize=GrGetCharacterWidth ('W');
    curposx=p_txtbox->lx1;

    linecnt=p_txtbox->cursorline;
    p_txtbox->linestarts[0]=0;

    /* now format the data string */
    for (i=p_txtbox->linestarts[p_txtbox->cursorline];i<=strlen(p_txtbox->data);i++)
    {
        /* examine each character and draw */
        if (p_txtbox->data[i]==13)
        {
            /* we got a return here, advance a line */
            p_txtbox->linewidths[linecnt]=curposx-p_txtbox->lx1;
            curposx=p_txtbox->lx1;
            if (linecnt >= p_txtbox->totalrows) TxtboxAllocLine (txtboxID);
            p_txtbox->linestarts[++linecnt]=i+1;

        } else if (p_txtbox->data[i]==9)
        {
            /* we have a tab here, advance to the nearest tab position */
            /* currently 3*wsize or 3 capital doubleyous :) */
            curposx=((((curposx-p_txtbox->lx1)/(3*wsize))+1)*(3*wsize))+p_txtbox->lx1;
            /* make sure we haven't reached the end of the line */
            if (curposx+wsize>p_txtbox->lx2)
            {
                /* out of room, advance a line */
                p_txtbox->linewidths[linecnt]=curposx-p_txtbox->lx1;
                curposx=p_txtbox->lx1;
                if (linecnt >= p_txtbox->totalrows) TxtboxAllocLine (txtboxID);
                p_txtbox->linestarts[++linecnt]=i+1;

            }
        }
        else if (p_txtbox->data[i]>31 && p_txtbox->data[i]<128)
        {   /* normal character */
            curposx+=GrGetCharacterWidth (p_txtbox->data[i]);
            if (curposx+wsize>p_txtbox->lx2)
            {
                /* reached end of line, traverse backwards until we
                   find a space */
                if (linecnt>0) end=p_txtbox->linestarts[linecnt-1];
                else end=0;

                tcurposx=curposx;

                for (j=i;j>end;j--)
                {
                    if (p_txtbox->data[j]==32 || p_txtbox->data[j]==9)
                    {
                        /* found a space, do it. */
                        break;
                    } else
                    {
                        tcurposx-=GrGetCharacterWidth (p_txtbox->data[j]);
                        if (tcurposx<wsize)
                        {
                            j=end;
                            break;
                        }
                    }
                }
                if (j==end) /* wow, can't do it */
                {
                    p_txtbox->linewidths[linecnt]=curposx-p_txtbox->lx1;
                    curposx=p_txtbox->lx1;
                    if (linecnt >= p_txtbox->totalrows) TxtboxAllocLine (txtboxID);
                    p_txtbox->linestarts[++linecnt]=i+1;
                } else
                {
                    p_txtbox->linewidths[linecnt]=tcurposx-p_txtbox->lx1;
                    curposx=p_txtbox->lx1;
                    if (linecnt >= p_txtbox->totalrows) TxtboxAllocLine (txtboxID);
                    p_txtbox->linestarts[++linecnt]=j+1;
                    i=j+1;
                }
            }
        }
    }

    p_txtbox->linewidths[linecnt]=curposx-p_txtbox->lx1;
    p_txtbox->totalrows=linecnt;

    /* close the font */
    ResourceUnlock (p_txtbox->font);

    DebugEnd();
}


T_void TxtboxSetScrollBarObjIDs (T_txtboxID txtboxID,
                                 T_buttonID sbupID,
                                 T_buttonID sbdnID,
                                 T_graphicID sbgrID)
{
    T_txtboxStruct *p_txtbox;

    DebugRoutine ("TxtboxSetScrollBarObjIDs");
    DebugCheck (txtboxID != NULL);
    DebugCheck (sbupID != NULL);
    DebugCheck (sbdnID != NULL);
    DebugCheck (sbgrID != NULL);

    p_txtbox=(T_txtboxStruct *)txtboxID;
    p_txtbox->sbupID=sbupID;
    p_txtbox->sbdnID=sbdnID;
    p_txtbox->sbgrID=sbgrID;

    DebugEnd();
}


T_void TxtboxHandleSBDn (T_buttonID buttonID)
{
    T_txtboxID txtboxID;
    T_buttonStruct *p_button;

    DebugRoutine ("TxtboxHandleSBDn");
    DebugCheck (buttonID != NULL);

    p_button=(T_buttonStruct *)buttonID;
    txtboxID=FormGetObjID(p_button->data);
    DebugCheck (txtboxID != NULL);
    TxtboxCursDn (txtboxID);
    DebugEnd();
}


T_void TxtboxHandleSBUp (T_buttonID buttonID)
{
    T_txtboxID txtboxID;
    T_buttonStruct *p_button;

    DebugRoutine ("TxtboxHandleSBUp");
    DebugCheck (buttonID != NULL);

    p_button=(T_buttonStruct *)buttonID;
    txtboxID=FormGetObjID(p_button->data);
    DebugCheck (txtboxID != NULL);
    TxtboxCursUp (txtboxID);
    DebugEnd();
}


T_void TxtboxUpdateSB (T_txtboxID txtboxID)
{
    T_txtboxStruct *p_txtbox;
    T_graphicStruct *p_graphic;
    T_screen tempscreen,oldscreen;

    T_word16 sbx1,sby1,sbx2,sby2,dy;
    float ystart,ylength,yleft;
    float ratio;

    DebugRoutine ("TxtboxUpdateSB");
    DebugCheck (txtboxID != NULL);

    p_txtbox=(T_txtboxStruct *)txtboxID;
    DebugCheck (p_txtbox->sbgrID != NULL);

   /* allocate a temporary drawing field */
//    oldscreen=GrScreenGet();
//    tempscreen=GrScreenAlloc();
//    GrScreenSet (tempscreen);
//    GraphicDrawToCurrentScreen();
    /* force redraw of the scroll bar graphic */
    p_graphic=(T_graphicStruct *)p_txtbox->sbgrID;
    p_graphic->changed=TRUE;
    GraphicUpdate (p_txtbox->sbgrID);

    /* get the loci of the scroll bar graphic */
    sbx1=p_graphic->locx+1;
    sbx2=p_graphic->locx+p_graphic->width-2;
    sby1=p_graphic->locy+1;
    sby2=p_graphic->locy+p_graphic->height-2;

    /* figure the percentage of the total document that is visible */
    if (p_txtbox->totalrows < p_txtbox->windowrows)
    {
        /* fully visible */
        ylength=sby2-sby1;
        ystart=sby1;
    } else
    {
        ratio=(float)p_txtbox->windowrows/(float)(p_txtbox->totalrows+1);

        /* partially visible document */
        ylength=(((float)(sby2-sby1))*ratio);

        yleft=(sby2-sby1)-ylength;
        if (p_txtbox->mode==TXTBOX_MODE_VIEW_SCROLL_FORM)
        {
            ratio=((float)p_txtbox->windowstartline)/((float)p_txtbox->totalrows+1-p_txtbox->windowrows);
            ystart=(yleft*ratio)+sby1;
        }
        else
        {
            ratio=((float)p_txtbox->cursorline)/((float)p_txtbox->totalrows+1);
            ystart=(yleft*ratio)+sby1;
        }
    }

    /* draw the rectangle */
    GrDrawRectangle (sbx1,(int)ystart,sbx2,(int)ystart+(int)ylength,68);
    GrDrawFrame (sbx1,(int)ystart,sbx2,(int)ystart+(int)ylength,66);
    GrDrawHorizontalLine (sbx1,(int)ystart,sbx2,70);
    GrDrawVerticalLine (sbx1,(int)ystart,(int)ystart+(int)ylength,70);

    /* set structure records */
    p_txtbox->sbstart=(int)ystart;
    p_txtbox->sblength=(int)ylength;

    /* copy the contents of the temporary screen area to the visual one */
//    GrTransferRectangle (oldscreen,
//                         sbx1,
//                         sby1,
//                         sbx2,
//                         sby2,
//                         sbx1,
//                         sby1);

    /* restore buffer */
//    GrScreenSet (oldscreen);
//    GrScreenFree (tempscreen);
//    GraphicDrawToActualScreen();
    DebugEnd();
}


T_void TxtboxMoveSB (T_txtboxID txtboxID, T_word16 y)
{
    T_txtboxStruct *p_txtbox;
    T_graphicStruct *p_graphic;
    float pratio;
    T_word16 pline;
    T_word16 center;
    T_word16 spaceleft;
    T_sword16 dy;
    T_word16 cnt;

    DebugRoutine ("TxtboxMoveSB");
    DebugCheck (txtboxID != NULL);

    p_txtbox=(T_txtboxStruct *)txtboxID;
    p_graphic=(T_graphicStruct *)p_txtbox->sbgrID;

    if (y<p_txtbox->sbstart) TxtboxCursPgUp (txtboxID);
    else if (y>p_txtbox->sbstart+p_txtbox->sblength) TxtboxCursPgDn (txtboxID);
    else
    {
        /* calculate center of scroll bar */
        center=p_txtbox->sbstart+(p_txtbox->sblength/2);
        if (y>center+1)
        {
            cnt=0;
            while (y>center+2)
            {
                cnt++;
                TxtboxCursDn (txtboxID);
                TxtboxUpdateSB (txtboxID);
                center=p_txtbox->sbstart+(p_txtbox->sblength/2);
                if (cnt>5) break;
            }
        }
        else if (y<center-1)
        {
            cnt=0;
            while (y<center-2)
            {
                cnt++;
                TxtboxCursUp (txtboxID);
                TxtboxUpdateSB (txtboxID);
                center=p_txtbox->sbstart+(p_txtbox->sblength/2);
                if (cnt>5) break;
            }
        }

        G_currentAction = TXTBOX_ACTION_SELECTION_CHANGED;
        if (p_txtbox->txtboxcallback != NULL) p_txtbox->txtboxcallback (txtboxID);

//          pratio=((float)(y-(p_txtbox->sbstart)))/p_txtbox->sblength;
//          pline=(int)(pratio*(float)p_txtbox->totalrows);

        /* calc the center of the scroll bar */
//        center=p_txtbox->sbstart+(p_txtbox->sblength/2);

        /* calc the space around the edges */
//        spaceleft=(p_graphic->height-2) - p_txtbox->sblength;

        /* calculate the distance moved */
//        dy=y-center;

        /* calc the new sb center */
//        center+=dy;

        /* limit the movement */
//        if (center - (p_txtbox->sblength/2) < p_graphic->locy+1)
//          center=p_graphic->locy+(p_txtbox->sblength/2);
//        else if ((center + p_txtbox->sblength/2) > p_graphic->locy+p_graphic->height-1)
//          center=p_graphic->locy+p_graphic->height-1-(p_txtbox->sblength/2);

        /* distance from top edge of sb graphic to top edge of sb is our ratio */
//        pratio=((center-(p_txtbox->sblength/2))-(p_txtbox->ly1+1))/spaceleft;
//        pline=(int)(pratio*(float)p_txtbox->totalrows);

//
/*        switch (p_txtbox->mode)
        {
            case TXTBOX_MODE_EDIT_FIELD:
            case TXTBOX_MODE_EDIT_FORM:

            break;

            case TXTBOX_MODE_VIEW_SCROLL_FORM:
            case TXTBOX_MODE_SELECTION_BOX:

                 p_txtbox->cursorline=pline;
                 p_txtbox->cursorl=p_txtbox->linestarts[pline];
                 TxtboxUpdate (txtboxID);
            break;

            default:
            break;
        }
*/
//        if (pline < p_txtbox->cursorline) TxtboxCursUp (txtboxID);
//        else TxtboxCursDn (txtboxID);
    }

    /* force redraw */
    TxtboxUpdateSB (txtboxID);

    DebugEnd();
}


T_void TxtboxAllocLine (T_txtboxID txtboxID)
{
    T_txtboxStruct *p_txtbox;
    T_word32 *newlinestarts;
    T_word16 *newlinewidths;
    T_word16 i;

    DebugRoutine ("TxtboxAllocLine");
    DebugCheck (txtboxID != NULL);

    p_txtbox=(T_txtboxStruct *)txtboxID;

    DebugCheck (p_txtbox->linewidths != NULL);
    DebugCheck (p_txtbox->linestarts != NULL);
    DebugCheck (p_txtbox->data != NULL);

    p_txtbox->totalrows++;

    /* allocate a new chunk for linewidths / linestarts */
    newlinestarts=MemAlloc (sizeof(T_word32)*(p_txtbox->totalrows+1));
    MemCheck (311);
    DebugCheck (newlinestarts != NULL);

    newlinewidths=MemAlloc (sizeof(T_word16)*(p_txtbox->totalrows+1));
    MemCheck (312);
    DebugCheck (newlinewidths != NULL);

    /* copy old data */
    for (i=0;i<p_txtbox->totalrows;i++)
    {
        newlinestarts[i]=p_txtbox->linestarts[i];
        newlinewidths[i]=p_txtbox->linewidths[i];
    }
    newlinestarts[i]=0;
    newlinewidths[i]=0;

    /* delete old data */
    MemFree (p_txtbox->linewidths);
    MemCheck (313);
    p_txtbox->linewidths=newlinewidths;

    MemFree (p_txtbox->linestarts);
    MemCheck (314);
    p_txtbox->linestarts=newlinestarts;

    DebugEnd();
}


T_void TxtboxSetCallback (T_txtboxID txtboxID, T_txtboxHandler newcallback)
{
    T_txtboxStruct *p_txtbox;

    DebugRoutine ("TxtboxSetCallBack");
    DebugCheck (txtboxID != NULL);

    p_txtbox=(T_txtboxStruct *)txtboxID;
    p_txtbox->txtboxcallback = newcallback;

    DebugEnd();
}

T_void TxtboxSetNumericOnlyFlag (T_txtboxID txtboxID, E_Boolean newflag)
{
    T_txtboxStruct *p_txtbox;

    DebugRoutine ("TxtboxSetNumericOnlyFlag");
    DebugCheck (txtboxID != NULL);

    p_txtbox=(T_txtboxStruct *)txtboxID;
    p_txtbox->numericonly=newflag;

    DebugEnd();
}


E_txtboxAction TxtboxGetAction (T_void)
{
    return (G_currentAction);
}

E_Boolean TxtboxValidateID (T_txtboxID txtboxID)
{
    T_word16 i;
    E_Boolean retvalue=FALSE;

    DebugRoutine ("TxtboxValidateID");
    for (i=0;i<MAX_TXTBOXES;i++)
    {
        if (txtboxID==G_txtboxArray[i])
        {
            retvalue=TRUE;
            break;
        }
    }

    DebugEnd();
    return (retvalue);
}


T_word16 TxtboxGetSelectionNumber (T_txtboxID txtboxID)
{
    T_txtboxStruct *p_txtbox;
    T_word16 retvalue;

    DebugRoutine ("TxtboxGetSelectionNumber");
    DebugCheck (txtboxID != NULL);

    p_txtbox=(T_txtboxStruct *)txtboxID;
    DebugCheck (p_txtbox->mode==TXTBOX_MODE_SELECTION_BOX);

    retvalue=p_txtbox->cursorline;

    DebugEnd();

    return (retvalue);
}


E_Boolean TxtboxIsSelected (T_txtboxID txtboxID)
{
   T_txtboxStruct *p_txtbox;
   E_Boolean retvalue;

   DebugRoutine ("TxtboxIsSelected");
   DebugCheck (txtboxID != NULL);

   p_txtbox=(T_txtboxStruct *)txtboxID;

   retvalue=p_txtbox->isselected;
   DebugEnd();
   return (retvalue);
}


T_void TxtboxSetMaxLength (T_txtboxID txtboxID,T_word32 newmaxlen)
{
    T_txtboxStruct *p_txtbox;

    DebugRoutine ("TxtboxSetMaxLength");
    DebugCheck (txtboxID != NULL);

    p_txtbox=(T_txtboxStruct *)txtboxID;

    p_txtbox->maxlength=newmaxlen;

    DebugEnd();
}

/* routine will take stringToFit and return n where n is the number of */
/* characters from the left that will fit in txtboxID's field width */
T_word16 TxtboxCanFit (T_txtboxID txtboxID, T_byte8 *stringToFit)
{
    T_word16 widthcnt=0;
    T_word16 boxwidth=0;
    T_word16 charcnt=0;
    T_word16 length;
    T_txtboxStruct *p_txtbox;
    DebugRoutine ("TxtboxCanFit");
    DebugCheck (txtboxID != NULL);
    DebugCheck (stringToFit != NULL);

    p_txtbox=(T_txtboxStruct *)txtboxID;
    boxwidth=p_txtbox->lx2-p_txtbox->lx1 - (GrGetCharacterWidth('W')+3);
    length=strlen(stringToFit);
/*
    printf ("string=%d\n",stringToFit);
    printf ("length=%d\n",length);
    printf ("boxwidth=%d\n",boxwidth);
*/
    while ((widthcnt < boxwidth) &&
           (charcnt < length))
    {
        widthcnt+=GrGetCharacterWidth(stringToFit[charcnt++]);
    }


    DebugEnd();
//    if (charcnt > 0) return (charcnt-1);
    return (charcnt);
}



T_void *TxtboxGetStateBlock(T_void)
{
    T_txtboxID *p_txtboxs;

    p_txtboxs = MemAlloc(sizeof(G_txtboxArray)) ;
    DebugCheck(p_txtboxs != NULL) ;
    memcpy(p_txtboxs, G_txtboxArray, sizeof(G_txtboxArray)) ;
    memset(G_txtboxArray, 0, sizeof(G_txtboxArray)) ;

    return p_txtboxs ;
}

T_void TxtboxSetStateBlock(T_void *p_state)
{
    memcpy(G_txtboxArray, p_state, sizeof(G_txtboxArray)) ;
}

/****************************************************************************/
/* END OF FILE: TXTBOX.C                                                    */
/****************************************************************************/
