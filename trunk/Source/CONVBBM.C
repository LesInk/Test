#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <mem.h>
#include <string.h>
#include <dos.h>
#include <time.h>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#include "ack3d.h"
#include "ackeng.h"
#include "kit.h"
#include "modplay.h"

/*
void freeall(void)
{
    struct _heapinfo entry ;
    int status ;

    entry._pentry = NULL ;
    while (_heapwalk(&entry) == _HEAPOK)  {
        if (entry._useflag == _USEDENTRY)  {
            puts("Freeing block") ;
            ffree(entry._pentry) ;
            break ;
        }
    }
}
*/

char rotated[64][64] ;

void AppendTrailer(FILE *fp, int sizex, int sizey, char *data)
{
    int x, y, i ;

    for (x=0; x<sizex; x++)  {
        i = 1 ;
        for (y=0; y<sizey; y++)  {
            if (data[x*sizey+y])
                i = 0 ;
        }
        fputc(i, fp) ;
    }
}

void Rotate(char *data)
{
    int i,j,k ;

    for (i=k=0; i<64; i++)
        for (j=0; j<64; j++, k++)
            rotated[j][i] = data[k] ;

    memcpy(data, rotated, 4096) ;
    printf("Rotated ...") ;
}

void todotpic(char *name, char *newname)
{
    while (*name != '.')
        *(newname++) = *(name++) ;
    strcpy(newname, ".PIC") ;
}

void Convert(char *postfix)
{
    struct find_t info ;
    unsigned rc ;
    short *where ;
    char newname[80] ;
    unsigned newsize ;
    FILE *fp ;

    rc = _dos_findfirst(postfix, _A_NORMAL, &info) ;
    while (rc == 0)  {
        printf("Converting %s (orig. size=%d) ... ", info.name, info.size) ;
        where = (short *)AckReadiff(info.name) ;
        if (where == NULL)  {
            puts("Nope.") ;
        } else {
            printf("(%d, %d) ", where[0], where[1]) ;
            todotpic(info.name, newname) ;
            newsize = where[0]*where[1]+4 ;
            printf("New size: %d -> ", newsize) ;
            if (newsize == 4100)
                Rotate(((char *)where)+4) ;
            fp = fopen(newname, "wb") ;
            fwrite(where, newsize, 1, fp) ;
            AppendTrailer(fp, where[0], where[1], (char *)where+2) ;
            fclose(fp) ;
//            free(where) ;
            puts("OK") ;
        }
        rc = _dos_findnext(&info) ;
    }
}

void main()
{
    Convert("*.bbm") ;
    Convert("*.lbm") ;
}
