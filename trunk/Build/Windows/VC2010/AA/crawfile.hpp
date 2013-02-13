/***********************************************/
/*  FILE: CRAWFILE.H                           */
/***********************************************/


#ifndef _CRAWFILE_H_
#define _CRAWFILE_H_

#include <windows.h>
#include "debug.hpp"

typedef BYTE T_rawFileMode ;
#define RAW_FILE_MODE_READ        0
#define RAW_FILE_MODE_WRITE       1
#define RAW_FILE_MODE_READ_WRITE  2
#define RAW_FILE_MODE_APPEND      3
#define RAW_FILE_MODE_UNKNOWN     4

#define RAW_FILE_SIZE_UNKNOWN     0xFFFFFFFF

class CRawFile : public C_debugObject
{
    protected:
        OFSTRUCT fileInfo ;          // Structure about the file opened
        BOOL isOpen ;
        HANDLE fileHandle ;
    public:
        CRawFile() ;
        ~CRawFile() ;
        CRawFile(char *p_filename, T_rawFileMode mode) ;
        BOOL open(char *p_filename, T_rawFileMode mode) ;
        void close(void) ;
        DWORD read(VOID *p_buffer, DWORD size) ;
        DWORD write(VOID *p_buffer, DWORD size) ;
        DWORD seek(DWORD position) ;
        DWORD size(void) ;
        VOID *load(char *p_filename, DWORD *p_size) ;
        BOOL save(char *p_filename, void *p_data, DWORD size) ;
        DWORD sizeOfFile(char *p_filename) ;
        BOOL isFileOpen(void)  
            { return isOpen ; } 
} ;

#endif

/***********************************************/
/*  END OF FILE: CRAWFILE.H                    */
/***********************************************/
