/****************************************************************************/
/*    FILE:  LOOK   .C                                                      */
/****************************************************************************/

#include "standard.h"


/* routine is called when form is initialized */
T_void LookInit               (T_void)
{
    DebugRoutine ("LookInit");


    DebugEnd();
}


/* Called by ControlExamineObject */
/* routine is called to update 'look at' banner with creature info */
T_void LookUpdateCreatureInfo (T_3dObject *p_obj)
{
    T_byte8 stmp[512];
    T_word16 objType;
    T_word32 size;
    T_word16 inX,outX;
    T_byte8 *dataIn;
    T_byte8 *name;
    T_byte8 *description;
    T_byte8 *picturename;
    T_byte8 charIn;
    T_byte8 iteration=0;
    T_resource res;
    T_txtboxID txtboxID;

    DebugRoutine ("LookUpdateCreatureInfo");
    DebugCheck(p_obj != NULL) ;

    if (p_obj != NULL)   {
        /* No matter what happens, draw the picture. */
        /* must fit in bounds 215,16 to 314,85 */
        ObjectDrawFrontScaled(
            p_obj,
            215,
            16,
            99,
            69) ;

        /* get monster info text for this obj */
        objType=ObjectGetType(p_obj);

        /* try to open up monster description block */
        sprintf (stmp,"CREDESC/DES%05d.TXT",objType);
//        MessageAdd(stmp);
        if (PictureExist(stmp))
        {
            /* lock in data structure 'text file' */
            dataIn = (T_byte8 *)PictureLockData (stmp,&res);
            size=ResourceGetSize(res);

            /* scan data 'text file' and place into temporary field for */
            /* transfer to banner display fields */

            inX=0;
            outX=0;
            while (inX<size)
            {
                /* get a character from input data */
                charIn=dataIn[inX++];
                stmp[outX++]=charIn;
                /* check for temporary overflow */
                DebugCheck (outX<256);

                if (charIn=='\n')
                {
                    /* reached 'end of line' */
                    stmp[outX]='\0';
                    outX=0;

                    /* newline reached. */
                    if (iteration==0)
                    {
                        /* store stmp in name field */
                        txtboxID=FormGetObjID(500);
                        TxtboxSetData(txtboxID,stmp);
                    }
                    else if (iteration==1)
                    {
                        /* store stmp in description field */
                        txtboxID=FormGetObjID(501);
                        TxtboxSetData(txtboxID,stmp);
                        /* exit, we are done */
                        break;
                    }
                    iteration++;
                }
            }
            PictureUnlockAndUnfind(res) ;
        }
        else
        {
            txtboxID=FormGetObjID(500);
            TxtboxSetData(txtboxID,"Unknown Creature");

            txtboxID=FormGetObjID(501);
            TxtboxSetData(txtboxID,"No other information available.");
        }
    }

    DebugEnd();
}


/* Called by ControlExamineObject */
/* routine is called to request player info from server */
T_void LookRequestPlayerInfo  (T_3dObject *p_obj)
{
    T_txtboxID txtboxID;

    DebugRoutine ("LookRequestPlayerInfo");
    DebugCheck (BannerFormIsOpen(BANNER_FORM_LOOK));

    /* get name/title field pointer */
    txtboxID=FormGetObjID(500);
    TxtboxSetData (txtboxID,"Retrieving info..");

    DebugEnd();
}


T_void LookUpdatePlayerInfo (T_lookDataStruct *p_lookData)
{
    T_txtboxID txtboxID;
    DebugRoutine ("LookUpdatePlayerInfo");
    /* in case banner was abruptly closed */
    if (BannerFormIsOpen(BANNER_FORM_LOOK))
    {
        txtboxID=FormGetObjID(500);
        TxtboxSetData(txtboxID,p_lookData->name);

        txtboxID=FormGetObjID(501);
        TxtboxSetData(txtboxID,p_lookData->description);

        /* draw player picture here */
        /* must fit in bounds 215,16 to 314,85 */

    }

    DebugEnd();
}




/****************************************************************************/
/*    END OF FILE:  LOOK   .C                                               */
/****************************************************************************/
