/****************************************************************************/
/*    FILE:  files.h                                                        */
/****************************************************************************/

T_void *FilesLoadFile (T_word32 filename, T_word32 revlevel,
                       T_word32 *actual_revlevel, T_word32 *size);

T_file FilesOpenFile (T_word32 filename, T_word32 revlevel, E_fileMode mode);

T_file FilesCreateNewFile (T_word32 filename);

T_file FilesCreateFileWithRevlevel (T_word32 filename, T_word32 revlevel);

T_word32 FilesGetRevlevel (T_word32 filename);

T_void FilesObtainFile (T_word32 filename, T_word32 revlevel);

/****************************************************************************/
/*    END OF FILE:  files.h                                                 */
/****************************************************************************/


