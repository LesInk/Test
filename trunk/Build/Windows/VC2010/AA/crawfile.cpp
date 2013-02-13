/***********************************************/
/*  FILE: CRAWFILE.C                           */
/***********************************************/

#include <windowsx.h>
#include "crawfile.hpp"

/*------------------------------------------------------------------------*/
/* Initialize the CRawFile structure */
CRawFile::CRawFile()
{
    _debug("CRawFile::CRawFile()") ;

    isOpen = FALSE ;
    fileHandle = NULL ;
}

/*------------------------------------------------------------------------*/
/* Initialize the CRawFile structure and go ahead and open a file */
CRawFile::CRawFile(char *p_filename, T_rawFileMode mode)
{
    _debug("CRawFile::CRawFile(filename, mode)") ;

    isOpen = FALSE ;
    fileHandle = NULL ;

    /* Attempt opening the file automatically */
    open(p_filename, mode) ;
}

/*------------------------------------------------------------------------*/
/* Destroy the handle (making sure the file is closed) */
CRawFile::~CRawFile()
{
    _debug("CRawFile::CRawFile") ;

    /* Make sure the file is closed */
    if (isOpen)
        close() ;
}
        
/*------------------------------------------------------------------------*/
/* Open a new file */
BOOL CRawFile::open(char *p_filename, T_rawFileMode mode)
{
    static UINT accessType[] = {
        GENERIC_READ,
        GENERIC_WRITE,
        GENERIC_READ | GENERIC_WRITE,
        GENERIC_WRITE
    } ;
    static UINT createType[] = {
        OPEN_EXISTING,          /* Read only opens existing files */
        CREATE_ALWAYS,          /* Write always creates a new file */
        OPEN_ALWAYS,            /* Read/Write creates if necessary */
        OPEN_ALWAYS             /* Append creates if necessary */
    } ;

    _debug("CRawFile::open") ;
    _check(isOpen == FALSE) ;
    _check(mode < RAW_FILE_MODE_UNKNOWN) ;
    _check(fileHandle == NULL) ;

    /* Attempt opening the file */
	fileHandle = CreateFile(
                     p_filename,        /* File name */
                     accessType[mode],  /* Appropriate read or write mode */
                     FILE_SHARE_READ | FILE_SHARE_WRITE,  /* Full sharing */
                     NULL,
                     createType[mode],  /* Create if necessary */
                     FILE_ATTRIBUTE_NORMAL,   /* Normal file, please */
                     NULL) ;             /* No duplication of handles */



    /* Determine if we opened it or not. */
    if (fileHandle != INVALID_HANDLE_VALUE)
        isOpen = TRUE ;
    else
        isOpen = FALSE ;

    /* If appending to file, seek the end */
    if ((isOpen) && (mode == RAW_FILE_MODE_APPEND))
        seek(size()) ;

    return isOpen ;
}

/*------------------------------------------------------------------------*/
/* Close a currently open file */
void CRawFile::close(void)
{
    _debug("CRawFile::close") ;
    _check(isOpen) ;
    _check(fileHandle != NULL) ;

    if (isOpen)  {
        CloseHandle(fileHandle) ;
        fileHandle = NULL ;
        isOpen = FALSE ;
    }
}

/*------------------------------------------------------------------------*/
/* Read a block of memory from the currently open file */
DWORD CRawFile::read(VOID *p_buffer, DWORD size)
{
    DWORD bytesRead ;
    BOOL result ;

    _debug("CRawFile::read") ;
    _check(isOpen) ;
    _check(fileHandle != NULL) ;

    if (isOpen)  {
        /* Read from the file */
        result = ReadFile(fileHandle, p_buffer, size, &bytesRead, NULL) ;
        _check(result == TRUE) ;
        if (result == FALSE)
            bytesRead = 0 ;
    } else {
        bytesRead = 0 ;
    }

    return bytesRead ;
}

/*------------------------------------------------------------------------*/
/* Write a block to the currently open file */
DWORD CRawFile::write(VOID *p_buffer, DWORD size)
{
    DWORD bytesWritten ;
    BOOL result ;

    _debug("CRawFile::write") ;
    _check(isOpen) ;
    _check(fileHandle != NULL) ;

    if (isOpen)  {
        /* Write to the file */
        result = WriteFile(fileHandle, p_buffer, size, &bytesWritten, NULL) ;
        _check(result == TRUE) ;
        if (result == FALSE)
            bytesWritten = 0 ;
    } else {
        bytesWritten = 0 ;
    }

    return bytesWritten ;
}

/*------------------------------------------------------------------------*/
/* Seek a byte position in the file up to 4 Gig */
DWORD CRawFile::seek(DWORD position)
{
    _debug("CRawFile::seek") ;
    _check(isOpen) ;
    DWORD pos = 0xFFFFFFFF ;

    if (isOpen)
        pos = SetFilePointer(fileHandle, position, NULL, FILE_BEGIN) ;

    return pos ;
}

/*------------------------------------------------------------------------*/
/* Get the size of the currently open file */
DWORD CRawFile::size(void)
{
    DWORD filesize ;

    _debug("CRawFile::size") ;
    _check(isOpen) ;
    _check(fileHandle != NULL) ;

    filesize = GetFileSize(fileHandle, NULL) ;

    return filesize ;
}

/*------------------------------------------------------------------------*/
/* Load a full file into global memory */
VOID *CRawFile::load(char *p_filename, DWORD *p_size)
{
    VOID *p_data = NULL ;               // Pointer to data to return
    DWORD sizeOfFile ;
    DWORD sizeRead ;
    
    _debug("CRawFile::load") ;
    _check(isOpen == FALSE) ;
    _check(p_filename != NULL) ;

    /* Don't do this command if the file is already opened */
    if (isOpen == FALSE)  {
        if (open(p_filename, RAW_FILE_MODE_READ) == TRUE)  {
            /* Get the size of the file */
            sizeOfFile = size() ;

            /* Store the size of the file (if a pointer is given) */
            if (p_size)
                *p_size = sizeOfFile ; 

            /* Allocate the memory to store this block of data */
            p_data = GlobalAllocPtr(GMEM_MOVEABLE, sizeOfFile) ;

            _check(p_data != NULL) ;
            if (p_data)  {
                /* Read in the whole file */
                sizeRead = read(p_data, sizeOfFile) ;

                /* Hmmm ... size read and size of file different! */
                /* An error has occured */
                _check(sizeRead == sizeOfFile) ;
                if (sizeRead != sizeOfFile)  {
                    /* Delete the block of memory */
                    GlobalFreePtr(p_data) ;
                    p_data = NULL ;
                }
            }

            /* Close the file in all cases */
            close() ;
        } 
    }

    return p_data ;
}

/*------------------------------------------------------------------------*/
/* Save a block of memory as a single file */
BOOL CRawFile::save(char *p_filename, void *p_data, DWORD size)
{
    BOOL isSuccess = FALSE ;
    DWORD sizeWritten ;
    
    _debug("CRawFile::save") ;
    _check(isOpen == FALSE) ;
    _check(p_filename != NULL) ;
    _check(p_data != NULL) ;

    /* Don't do this command if the file is already opened */
    if (isOpen == FALSE)  {
        if (open(p_filename, RAW_FILE_MODE_WRITE) == TRUE)  {
            /* Write out the whole file */
            sizeWritten = write(p_data, size) ;

            /* Hmmm ... size read and size of file different! */
            /* An error has occured */
            _check(sizeWritten == size) ;
            if (sizeWritten == size)  
                isSuccess = TRUE ;

            /* Close out the file now that it has been written */
            close() ;
        } 
    }

    return isSuccess ;
}

/*------------------------------------------------------------------------*/
/* Get the size of a file without keeping it open */
DWORD CRawFile::sizeOfFile(char *p_filename)
{
    DWORD fileSize = RAW_FILE_SIZE_UNKNOWN ;

    _debug("CRawFile::sizeOfFile") ;
    _check(isOpen == FALSE) ;

    /* Unfortunately, I don't see any file size routines that */
    /* don't work on an open file, so I must open the file. */
    /* Therefore, this routine will probably not work on files */
    /* already opened. */

    /* Don't do this if this file is already open */
    if (isOpen == FALSE)  {
        /* Open the file. */
        if (open(p_filename, RAW_FILE_MODE_READ))  {
            /* Get the size */
            fileSize = size() ;

            /* Close the file */
            close() ;
        }
    }

    return fileSize ;
}

/***********************************************/
/*  END OF FILE: CRAWFILE.C                    */
/***********************************************/
