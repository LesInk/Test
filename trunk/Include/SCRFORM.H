/****************************************************************************/
/*    FILE:  SCRFORM.H                                                      */
/****************************************************************************/

T_void ScriptFormStart(T_word32 uiFormNumber) ;
T_void ScriptFormEnd(T_void) ;
T_void ScriptFormCallback(
           E_formObjectType objtype,
		   T_word16 objstatus,
		   T_word32 objID) ;
T_void ScriptFormUpdate(T_void) ;

/* Commands to affect the current form. */
T_void ScriptFormTextBoxSetSelection(
           T_word16 id,
           T_word16 selection) ;

/****************************************************************************/
/*    END OF FILE: SCRFORM.H                                                */
/****************************************************************************/
