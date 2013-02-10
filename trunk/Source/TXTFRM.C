/****************************************************************************/
/*    FILE:  TXTFRM.C                                                       */
/****************************************************************************/

#include "standard.h"

static T_txtfrmID G_txtfrmarray[MAX_TXTFRMS];
static T_txtfrmID txtfrmInit (T_word16 x1,
										  T_word16 y1,
										  T_word16 x2,
										  T_word16 y2,
										  T_byte8 *fontname,
										  E_txtfrmType ftype);
static T_byte8 curform=0;

/****************************************************************************/
/*  Routine:  txtfrmCreate                                                    */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*  Adds a text form (field) to the screen.                                 */
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
/*  Location x1,y1,x2,y2 for the form, a background picture (not necessary) */
/*  and a form type                                                         */
/*                                                                          *//*                                                                          *//*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    T_txtfrmID (id of the txtfrm created)                                     */
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
/*    JDA  07/05/95  Created                                                */
/*                                                                          */
/****************************************************************************/

T_txtfrmID txtfrmCreate (T_word16 x1,
								 T_word16 y1,
								 T_word16 x2,
								 T_word16 y2,
								 T_byte8 *fontname,
								 E_txtfrmType formtype)
{
	T_word16 i;
	T_txtfrmStruct *p_txtfrm;

	DebugRoutine ("txtfrmCreate");

	for (i=0;i<MAX_TXTFRMS;i++)
	{
		if (G_txtfrmarray[i]==NULL)  //add a txtfrm to list
		{
			G_txtfrmarray[i]=txtfrmInit(x1,y1,x2,y2,fontname,formtype);
			p_txtfrm=(T_txtfrmStruct*)G_txtfrmarray[i];
			GraphicSetPostCallBack (p_txtfrm->p_graphicID,txtfrmTextDrawCallBack,i);
			break;
		}
	}

	DebugCheck (i<MAX_TXTFRMS);
	DebugEnd();
	return (G_txtfrmarray[i]);
}



/****************************************************************************/
/*  Routine:  txtfrmInit *internal*                                           */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*  Allocates memory and sets up initial variables required to display and  */
/*  control a text form                                                     */
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
/*  Location x1,y1,x2,y2 for the form, a background picture (not necessary) */
/*  and a form type                                                         */
/*                                                                          *//*                                                                          *//*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    T_txtfrmID (id of the txtfrm created)                                     */
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
/*    JDA  07/05/95  Created                                                */
/*                                                                          */
/****************************************************************************/

static T_txtfrmID txtfrmInit (T_word16 x1,
								 T_word16 y1,
								 T_word16 x2,
								 T_word16 y2,
								 T_byte8 *fontname,
								 E_txtfrmType ftype)
{
	T_word32 size;
	T_resourceFile res;
	T_txtfrmStruct *myID;
	T_bitfont *p_font;
	T_word16 i;
	T_word16 maxlines,fheight;

	DebugRoutine ("txtfrmInit");     /* make sure the form fits on the screen */
	DebugCheck (x1<=SCREEN_SIZE_X &&
					x2<=SCREEN_SIZE_X &&
					y1<=SCREEN_SIZE_Y &&
					y2<=SCREEN_SIZE_Y &&
					x1<x2 &&
					y1<y2);

	size=sizeof(T_txtfrmStruct);
	myID=(T_txtfrmID)MemAlloc(size); /* allocate memory for the form struct */

	DebugCheck (myID!=NULL);         /* make sure memory was allocated */

	if (myID!=NULL)
	{
		myID->p_graphicID=NULL;   /* set the 'graphics' to null */
		myID->p_graphicID=GraphicCreate (x1, y1, NULL); /* make a 'text graphic' field */
		GraphicSetSize (myID->p_graphicID, x2-x1,y2-y1); /* set the size of the text window graphic */
		DebugCheck (myID->p_graphicID != NULL);         /* make sure it worked */
		myID->fcolor=31; /* set the default fore and background colors */
		myID->bcolor=10;  /* for the text */
		res = ResourceOpen("sample.res") ;
		DebugCheck (fontname!=NULL); /* make sure a font name was passed in */
		myID->font=ResourceFind(res, fontname);  /* open the selected font */
		p_font=ResourceLock (myID->font);
		GrSetBitFont (p_font);
		fheight=p_font->height; /*determine the height of the font*/
		myID->maxvislines=(y2-y1)/(fheight); /* determine the maximum number */
														  /* of visible lines */
		myID->frmtype=ftype; /* set the form type */
		myID->hotkey=0; /* no hotkey currently defined */
		myID->cursorx=0;/* cursor at the start */
		myID->cursory=0;
		myID->dispwiny=0; /*display window at the top */

		for (i=0;i<MAX_TXTFRM_LINES;i++)
		{
			myID->data[i]=NULL; /* make sure the datablock is empty */
			myID->linechanged[i]=TRUE; /* make sure they update at least once */
		}
		size=sizeof(T_byte8)*MAX_TXTFRM_WIDTH; /* find the size of one line */
		for (i=0;i<myID->maxvislines;i++) txtfrmNewLine (myID,i);
		/* allocate a 'screen full' of lines */
	}
	DebugEnd();
	return (myID);
}


/****************************************************************************/
/*  Routine:  txtfrmDelete / txtfrmCleanUp                                  */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*                                                                          */
/*    Releases memory allocated to a txtfrm                                 */
/*    Cleanup releases memory allocated to all 'txtfrms'                    */
/*                                                                          */
/*                                                                          */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_txtfrmID txtfrmID / none                                                */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    JDA  07/05/95  Created                                                */
/*                                                                          */
/****************************************************************************/
T_void txtfrmDelete (T_txtfrmID txtfrmID)
{
	T_word16 i,j;
	T_txtfrmStruct *p_txtfrm;

	DebugRoutine ("txtfrmDelete");

	if (txtfrmID!=NULL)
	{
		for (i=0;i<MAX_TXTFRMS;i++)
		{
			if (G_txtfrmarray[i]==txtfrmID) /*found it, now kill it */
			{
				p_txtfrm = (T_txtfrmStruct *)txtfrmID;
				GraphicDelete (p_txtfrm->p_graphicID);
				for (j=0;j<MAX_TXTFRM_LINES;j++) /*kill the data block*/
				{
					if (p_txtfrm->data[j]!=NULL) memfree(p_txtfrm->data[j]);
				}
				MemFree (p_txtfrm); /*now get rid of the whole thing*/
				G_txtfrmarray[i]=NULL;
				break;
			}
		}
	}

	DebugEnd();
}


T_void txtfrmCleanUp (T_void)
{
	T_word16 i,j;
	T_txtfrmStruct *p_txtfrm;
	T_resourceFile res;

	DebugRoutine ("txtfrmCleanUp");

	for (i=0;i<MAX_TXTFRMS;i++)
	if (G_txtfrmarray[i]!=NULL)
	{
		p_txtfrm = (T_txtfrmStruct *)G_txtfrmarray[i];
		GraphicDelete (p_txtfrm->p_graphicID); /* release the graphics */
		for (j=0;j<MAX_TXTFRM_LINES;j++) if (p_txtfrm->data[j]!=NULL) /* kill the */
		  memfree (p_txtfrm->data[j]);                         /* text block */
		MemFree (p_txtfrm);  /* see ya! */
		G_txtfrmarray[i]=NULL;
	}
	res=ResourceOpen ("sample.res");
	ResourceClose (res);
	DebugEnd();
}




T_void txtfrmSetColor (T_txtfrmID txtfrmID, T_byte8 fc, T_byte8 bc)
{
	T_txtfrmStruct *p_txtfrm;

	DebugRoutine ("txtfrmSetColor");
	DebugCheck (txtfrmID != NULL);

	/* pretty self explanitory, here - get the pointer to the txtfrm structure
	and set the new values for color */

	p_txtfrm=(T_txtfrmStruct*)txtfrmID;
	p_txtfrm->fcolor=fc;
	p_txtfrm->bcolor=bc;

	DebugEnd();
}


T_void txtfrmSetHotKey (T_txtfrmID txtfrmID, T_word16 newkey)
{
	T_txtfrmStruct *p_txtfrm;

	DebugRoutine ("txtfrmSetHotKey");
	DebugCheck (txtfrmID != NULL);

	/* pretty self explanitory, here - get the pointer to the txtfrm structure
	and set the new values for color */

	p_txtfrm=(T_txtfrmStruct*)txtfrmID;
	p_txtfrm->hotkey=newkey;

	DebugEnd();
}


T_void txtfrmInsert (T_txtfrmID txtfrmID, char *string)
{
	T_txtfrmStruct *p_txtfrm;
	T_graphicStruct *p_graphic;
	T_word16 i;
	T_word16 len;
	T_word16 curwidth;
	T_byte8 tempstr[MAX_TXTFRM_WIDTH*2];

	DebugRoutine ("txtfrmInsert");
	DebugCheck (txtfrmID != NULL);

	p_txtfrm=(T_txtfrmStruct*)txtfrmID;
	p_graphic=(T_graphicStruct*)p_txtfrm->p_graphicID;

	DebugCheck (p_txtfrm->data[p_txtfrm->cursory]!=NULL);

	/* copy the current cursor line up to the x cursor position into */
	/* a temporary variable */
	for (i=0;i<p_txtfrm->cursorx;i++)
	{
		tempstr[i]=p_txtfrm->data[p_txtfrm->cursory][i];
	}
	tempstr[i]=NULL;

	/* append the incoming string to the temporary variable */
	strcat (tempstr, string);

	/* append the rest of the data line to the temp variable */
	len=strlen(tempstr);
	while (p_txtfrm->data[p_txtfrm->cursory][i]!=NULL)
	  tempstr[len++]=p_txtfrm->data[p_txtfrm->cursory][i++];
	tempstr[len]=NULL;

	/* now try to put the contents of the temporary variable into
	the current data block */
	curwidth=0;
	i=0;
	while (tempstr[i]!=NULL)
	{
		p_txtfrm->data[p_txtfrm->cursory][i]=tempstr[i];
		curwidth+=GrGetCharacterWidth(p_txtfrm->data[p_txtfrm->cursory][i]);
		if (curwidth>=p_graphic->width) break;
		if (i>=MAX_TXTFRM_WIDTH-1) break;
		i++;
	}
	p_txtfrm->data[p_txtfrm->cursory][i]=NULL;
	p_txtfrm->linechanged[p_txtfrm->cursory]=TRUE;
	p_graphic->changed=TRUE;

	DebugEnd();
}



T_void txtfrmAppend(T_txtfrmID txtfrmID, char *string)
{
	T_txtfrmStruct *p_txtfrm;

	DebugRoutine ("txtfrmAppend");
	DebugCheck (txtfrmID != NULL);

	p_txtfrm=(T_txtfrmStruct*)txtfrmID;

	txtfrmInsert (txtfrmID,string);
	p_txtfrm->cursorx+=strlen(string);

	DebugEnd();
}


T_byte8 *txtfrmGetLine (T_txtfrmID txtfrmID)
{
	T_txtfrmStruct *p_txtfrm;
	DebugRoutine ("txtfrmGetLine");
	DebugCheck (txtfrmID != NULL);

	p_txtfrm=(T_txtfrmStruct*)txtfrmID;

	DebugEnd();
	return (p_txtfrm->data[p_txtfrm->cursory]);
}


T_void txtfrmCursTop    (T_txtfrmID txtfrmID)
{
	T_txtfrmStruct *p_txtfrm;
	DebugRoutine ("txtfrmCursTop");
	DebugCheck (txtfrmID != NULL);

	p_txtfrm=(T_txtfrmStruct*)txtfrmID;
	p_txtfrm->cursorx=0;      /* set the cursor position to the top */
	p_txtfrm->cursory=0;
	p_txtfrm->dispwiny=0;     /* set the viewing window to the top */
	DebugEnd();
}


T_void txtfrmCursBottom (T_txtfrmID txtfrmID)
{
	T_sword16 i,len;
	T_txtfrmStruct *p_txtfrm;
	DebugRoutine ("txtfrmCursBottom");
	DebugCheck (txtfrmID != NULL);

	p_txtfrm=(T_txtfrmStruct*)txtfrmID;

	/* find first null line */
	for (i=0;i<MAX_TXTFRM_LINES;i++) if (p_txtfrm->data[i]==NULL) break;

	/* move back one line */
	i--;
	if (i<0) i=0;

	if (p_txtfrm->data[i]!=NULL) len=strlen(p_txtfrm->data[i]);
	else len=0;

	p_txtfrm->cursory=i;
	p_txtfrm->cursorx=len;
	p_txtfrm->dispwiny=p_txtfrm->cursory-p_txtfrm->maxvislines;
	if (p_txtfrm->dispwiny>=MAX_TXTFRM_LINES) p_txtfrm->dispwiny=0;

	DebugEnd();
}


T_void txtfrmCursMove   (T_txtfrmID txtfrmID,
								  T_byte8 xchar,
								  T_byte8 ychar)
{
	DebugRoutine ("txtfrmCursMove");
	DebugCheck (txtfrmID != NULL);


	DebugEnd();
}



T_void txtfrmPgDn (T_txtfrmID txtfrmID)
{
	T_txtfrmStruct *p_txtfrm;
	DebugRoutine ("txtfrmPgDn");
	DebugCheck (txtfrmID != NULL);

	p_txtfrm=(T_txtfrmStruct*)txtfrmID;
	/* add the maximum visible lines - 1 to the cursor location */
	p_txtfrm->cursory+=p_txtfrm->maxvislines-1;

	/* make sure the new cursor position is valid */
	if (p_txtfrm->cursory>=MAX_TXTFRM_LINES)
	  p_txtfrm->cursory=MAX_TXTFRM_LINES-1;

	/* if it's sitting at a 'null line' move the cursor to the
		bottom of the text screen */
	if (p_txtfrm->data[p_txtfrm->cursory]==NULL) txtfrmCursBottom(txtfrmID);

	/* calculate the new window position */
	p_txtfrm->dispwiny+=p_txtfrm->maxvislines-1;

	/* make sure the window is valid */
	if (p_txtfrm->dispwiny>=(MAX_TXTFRM_LINES-p_txtfrm->maxvislines))
	  p_txtfrm->dispwiny=(MAX_TXTFRM_LINES-p_txtfrm->maxvislines);

	DebugEnd();
}



T_void txtfrmPgUp (T_txtfrmID txtfrmID)
{
	DebugRoutine ("txtfrmPgUp");
	DebugCheck (txtfrmID != NULL);


	DebugEnd();
}


T_void txtfrmCursUp (T_txtfrmID txtfrmID)
{
	T_txtfrmStruct *p_txtfrm;
	T_word16 xloc;


	DebugRoutine ("txtfrmCursUp");
	DebugCheck (txtfrmID != NULL);

	p_txtfrm=(T_txtfrmStruct*)txtfrmID;

	/* find xlocation of current cursor in pixels */
	xloc=txtfrmCXtoX (txtfrmID, p_txtfrm->cursorx);

	/* move the cursor location up 1 character */
	p_txtfrm->cursory--;

	/* make sure it is in bounds */
	if (p_txtfrm->cursory>MAX_TXTFRM_LINES) p_txtfrm->cursory=0;

	/* calculate the new cursor x */
	p_txtfrm->cursorx=txtfrmXtoCX(txtfrmID,xloc);

	/* make sure we are still in the window */
	if (p_txtfrm->dispwiny>p_txtfrm->cursory)
	{
		p_txtfrm->dispwiny--;
		if (p_txtfrm->dispwiny>MAX_TXTFRM_LINES) p_txtfrm->dispwiny=0;
	}

//	printf ("Cursorx=%d, cursory=%d\r",p_txtfrm->cursorx,p_txtfrm->cursory);
//	fflush (stdout);
	DebugEnd();
}


T_void txtfrmCursDn (T_txtfrmID txtfrmID)
{
	T_txtfrmStruct *p_txtfrm;
	T_word16 xloc;
	DebugRoutine ("txtfrmCursDn");
	DebugCheck (txtfrmID != NULL);

	p_txtfrm=(T_txtfrmStruct*)txtfrmID;

	/* find xlocation of current cursor in pixels */
	xloc=txtfrmCXtoX (txtfrmID,p_txtfrm->cursorx);

	/* move the cursor location down one character */
	p_txtfrm->cursory++;

	/* make sure it is in bounds */
	if (p_txtfrm->cursory>=MAX_TXTFRM_LINES) p_txtfrm->cursory--;


	/* calculate the new cursor x */
	p_txtfrm->cursorx=txtfrmXtoCX(txtfrmID,xloc);

	/* allocate a new line if needed */
	if (p_txtfrm->data[p_txtfrm->cursory]==NULL) txtfrmNewLine (txtfrmID, p_txtfrm->cursory);

	/* check to see that we are still in the viewing window */
	if (p_txtfrm->dispwiny+p_txtfrm->maxvislines<=p_txtfrm->cursory)
	{
		/* move the window down a character */
		p_txtfrm->dispwiny++;
		/* make sure the window is in bounds */
		if (p_txtfrm->dispwiny+p_txtfrm->maxvislines>=MAX_TXTFRM_LINES)
		p_txtfrm->dispwiny--;
	}
//	printf ("Cursorx=%d, cursory=%d\r",p_txtfrm->cursorx,p_txtfrm->cursory);
//	fflush (stdout);
	DebugEnd();
}


T_void txtfrmCursLeft (T_txtfrmID txtfrmID)
{
	T_txtfrmStruct *p_txtfrm;
	DebugRoutine ("txtfrmCursLeft");
	DebugCheck (txtfrmID != NULL);

	p_txtfrm=(T_txtfrmStruct*)txtfrmID;
	p_txtfrm->cursorx--;
	if (p_txtfrm->cursorx>MAX_TXTFRM_WIDTH) p_txtfrm->cursorx=0;
//	printf ("Cursorx=%d, cursory=%d\r",p_txtfrm->cursorx,p_txtfrm->cursory);
//	fflush (stdout);

	DebugEnd();
}


T_void txtfrmCursRight (T_txtfrmID txtfrmID)
{
	T_txtfrmStruct *p_txtfrm;
	DebugRoutine ("txtfrmCursRight");
	DebugCheck (txtfrmID != NULL);

	p_txtfrm=(T_txtfrmStruct*)txtfrmID;
	p_txtfrm->cursorx++;

	if (p_txtfrm->data[p_txtfrm->cursory][p_txtfrm->cursorx]=='\0' ||
		 p_txtfrm->data[p_txtfrm->cursory][p_txtfrm->cursorx]==NULL )
	{
		if (p_txtfrm->data[p_txtfrm->cursory+1]==NULL) p_txtfrm->cursorx--;
		else
		{
			p_txtfrm->cursorx=0;
			txtfrmCursDn (txtfrmID);
		}
	}
//	printf ("Cursorx=%d, cursory=%d\r",p_txtfrm->cursorx,p_txtfrm->cursory);
//	fflush (stdout);

	DebugEnd();
}


T_void txtfrmNewLine (T_txtfrmID txtfrmID, T_byte8 y)
{
	T_word16 i;
	T_word32 size;
	T_txtfrmStruct *p_txtfrm;
	DebugRoutine ("txtfrmNewLine");
	DebugCheck (txtfrmID!=NULL);
	DebugCheck (y<MAX_TXTFRM_LINES);

	p_txtfrm=(T_txtfrmStruct*)txtfrmID;
	DebugCheck (p_txtfrm->data[y]==NULL);

	size=sizeof(T_byte8)*MAX_TXTFRM_WIDTH; /* find the size of one line */
	p_txtfrm->data[y]=(T_byte8*)memalloc(size);
	for (i=0;i<MAX_TXTFRM_WIDTH;i++) p_txtfrm->data[y][i]='\0';
	p_txtfrm->linechanged[i]=TRUE;
	DebugEnd();
}


T_void txtfrmBackSpace (T_txtfrmID txtfrmID)
{
	T_txtfrmStruct *p_txtfrm;
	DebugRoutine ("txtfrmBackSpace");
	DebugCheck (txtfrmID != NULL);

	p_txtfrm=(T_txtfrmStruct*)txtfrmID;
	p_txtfrm->cursorx--;
	if (p_txtfrm->cursorx>MAX_TXTFRM_WIDTH) p_txtfrm->cursorx=0;
	DebugEnd();
}




T_byte8 txtfrmXtoCX (T_txtfrmID txtfrmID, T_word16 xloc)
{
	T_txtfrmStruct *p_txtfrm;
	T_graphicStruct *p_graphic;
	T_byte8 retvalue=0;
	T_word16 xwidth=0;

	DebugRoutine ("txtfrmXtoCX");
	DebugCheck (txtfrmID != NULL);

	p_txtfrm=(T_txtfrmStruct*)txtfrmID;
	p_graphic=(T_graphicStruct*)p_txtfrm->p_graphicID;

	/* subtract the left boundary of the text box from the pixel x passed in */
	xloc-=(p_graphic->locx);

	if (p_txtfrm->data[p_txtfrm->cursory]!=NULL)
	{
		/* loop through the current line, adding the width of each character to
			a width counter until we either reach the end of the line or pass
			the x pixel width (xloc) */
		while ((p_txtfrm->data[p_txtfrm->cursory][retvalue] != '\0') &&
				 (p_txtfrm->data[p_txtfrm->cursory][retvalue] != NULL) &&
				 (xwidth < xloc))
		{
			xwidth+=GrGetCharacterWidth(p_txtfrm->data[p_txtfrm->cursory][retvalue++]);
		}
	}
	DebugEnd();
//	printf ("\ntxtfrmXtoCX returned %d",retvalue);
//	fflush (stdout);
	return (retvalue);
}


T_byte8 txtfrmYtoCY (T_txtfrmID txtfrmID, T_word16 yloc)
{
	DebugRoutine ("txtfrmYtoCY");
	DebugCheck (txtfrmID != NULL);


	DebugEnd();
	return (0);
}


T_word16 txtfrmCXtoX (T_txtfrmID txtfrmID, T_byte8 xcurs)
{
	T_txtfrmStruct *p_txtfrm;
	T_graphicStruct *p_graphic;
	T_word16 retvalue=0;
	T_word16 loopcnt=0;
	DebugRoutine ("txtfrmCXtoX");
	DebugCheck (txtfrmID != NULL);

	p_txtfrm=(T_txtfrmStruct*)txtfrmID;
	p_graphic=(T_graphicStruct*)p_txtfrm->p_graphicID;

	/* add the left x of the display window to the return value */
	retvalue+=p_graphic->locx;

	if (p_txtfrm->data[p_txtfrm->cursory]!=NULL)
	{
		/* loop through the current line, adding the width of each character to
			the return value until we run out of data or reach xcurs */
		while ((p_txtfrm->data[p_txtfrm->cursory][loopcnt] != '\0') &&
				 (p_txtfrm->data[p_txtfrm->cursory][loopcnt] != NULL) &&
				 (loopcnt < xcurs))
		{
			retvalue+=GrGetCharacterWidth(p_txtfrm->data[p_txtfrm->cursory][loopcnt++]);
		}
	}

	DebugEnd();
	return (retvalue);
}


T_word16 txtfrmCYtoY (T_txtfrmID txtfrmID, T_byte8 ycurs)
{
	DebugRoutine ("txtfrmCYtoY");
	DebugCheck (txtfrmID != NULL);


	DebugEnd();
	return (0);
}


T_graphicID txtfrmGetTextGraphicID (T_txtfrmID txtfrmID)
{
	DebugRoutine ("txtfrmGetTextGraphicID");
	DebugCheck (txtfrmID != NULL);


	DebugEnd();
	return (NULL);
}


T_graphicID txtfrmGetBackGraphicID (T_txtfrmID txtfrmID)
{
	DebugRoutine ("txtfrmGetBackGraphicID");
	DebugCheck (txtfrmID != NULL);


	DebugEnd();
	return (NULL);
}


T_txtfrmID txtfrmIsAt (T_word16 lx, T_word16 ly)
{
	DebugRoutine ("txtfrmIsAt");


	DebugEnd();
	return (NULL);
}


/****************************************************************************/
/*  Routine:  txtfrmDrawCallBack                                              */
/****************************************************************************/
/*                                                                          */
/*  Description:                                                            */
/*    A routine that is called to draw the txtfrm (callback routine assigned  */
/*    to p_graphicID, called from GraphicDraw)                              */
/*  Problems:                                                               */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Inputs:                                                                 */
/*                                                                          */
/*    T_graphicID - the graphic that called this routine                    */
/*    T_word16 index - used to find which txtfrm string to display            */
/*                                                                          */
/*  Outputs:                                                                */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*                                                                          */
/*  Calls:                                                                  */
/*                                                                          */
/*    None.                                                                 */
/*                                                                          */
/*  Revision History:                                                       */
/*                                                                          */
/*    Who  Date:     Comments:                                              */
/*    ---  --------  ---------                                              */
/*    JDA  07/05/95  Created                                                */
/*                                                                          */
/****************************************************************************/
T_void txtfrmTextDrawCallBack (T_graphicID graphicID, T_word16 index)
{
	T_graphicStruct *p_graphic;
	T_txtfrmStruct *p_txtfrm;
	T_bitfont *p_font;
	T_word16 i;
	T_word16 xstart=0,ystart=0;

	DebugRoutine ("txtfrmDrawCallBack");
	DebugCheck (graphicID != NULL);

	p_graphic=(T_graphicStruct*)graphicID;
	p_txtfrm=(T_txtfrmStruct*)G_txtfrmarray[index];

	p_font = ResourceLock(p_txtfrm->font) ;
	GrSetBitFont (p_font);

	GrDrawRectangle (p_graphic->locx-1,
						  p_graphic->locy,
						  p_graphic->locx+p_graphic->width,
						  p_graphic->locy+p_graphic->height,
						  p_txtfrm->bcolor);

	GrSetCursorPosition (p_graphic->locx,p_graphic->locy);

	for (i=p_txtfrm->dispwiny;i<p_txtfrm->dispwiny+p_txtfrm->maxvislines;i++)
	{
//		if (p_txtfrm->linechanged[i]==TRUE) //draw a new line
//		{
			if (p_txtfrm->data[i]!=NULL)
			{
				GrSetCursorPosition (p_graphic->locx,
											p_graphic->locy+((i-p_txtfrm->dispwiny)*p_font->height));
				GrDrawText (p_txtfrm->data[i],p_txtfrm->fcolor);
			}
			p_txtfrm->linechanged[i]=FALSE;
//		}
	}

	/* calculate width of current cursor line */

	if (p_txtfrm->data[p_txtfrm->cursory]!=NULL)
	{
		i=0;
		while (p_txtfrm->data[p_txtfrm->cursory][i] != '\0' &&
				 p_txtfrm->data[p_txtfrm->cursory][i] != NULL &&
				 i<p_txtfrm->cursorx ) i++;

		xstart=txtfrmCXtoX(G_txtfrmarray[index],i);
	}
//	xstart+=p_graphic->locx;
	ystart=(p_txtfrm->cursory-p_txtfrm->dispwiny)*p_font->height;
	ystart+=p_graphic->locy;
	GrDrawRectangle (xstart-1,ystart,xstart-1,ystart+p_font->height,50);
	ResourceUnlock (p_txtfrm->font);
	DebugEnd();
}


E_Boolean txtfrmKeyControl (E_keyboardEvent event, T_byte8 scankey)
{
	static T_byte8 keyheld=0;
	T_word16 delta=0;
	static T_word32 start_held=0;
	T_byte8 tempstr[MAX_TXTFRM_WIDTH];

	DebugRoutine ("txtfrmKeyControl");

	switch (event)
	{
		case KEYBOARD_EVENT_BUFFERED:
		printf ("scankey=%d\r",scankey);
		fflush (stdout);

		if (scankey==8) txtfrmBackSpace (G_txtfrmarray[curform]);
		else
		{
			sprintf (tempstr,"%c",scankey);
			tempstr[1]=NULL;
			txtfrmAppend (G_txtfrmarray[curform],tempstr);
		}
		txtfrmAppend (G_txtfrmarray[curform],"");
		break;

		case KEYBOARD_EVENT_HELD:
		if (scankey==170) break;
		if (scankey!=keyheld)
		{
			start_held=TickerGet();
			keyheld=scankey;
			break;
		}

		delta = (TickerGet() - start_held);
		if (delta<8) break;

		case KEYBOARD_EVENT_PRESS:
		start_held=TickerGet();
		keyheld=0;
		if (scankey==KEY_SCAN_CODE_LEFT) txtfrmCursLeft (G_txtfrmarray[curform]);
		else if (scankey==KEY_SCAN_CODE_RIGHT) txtfrmCursRight (G_txtfrmarray[curform]);
		else if (scankey==KEY_SCAN_CODE_UP) txtfrmCursUp (G_txtfrmarray[curform]);
		else if (scankey==KEY_SCAN_CODE_DOWN) txtfrmCursDn (G_txtfrmarray[curform]);
		break;

		default:
		break;
	}

	DebugEnd();
	return (FALSE);
}


T_void txtfrmMouseControl  (E_mouseEvent event,
									  T_word16 x,
									  T_word16 y,
									  E_Boolean button)
{
	DebugRoutine ("txtfrmMouseControl");


	DebugEnd();
}

