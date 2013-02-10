#include <stdlib.h>
#include <stdio.h>
#include <mem.h>

unsigned char actMap[64][64] ;
unsigned char actVert[65][65] ;
unsigned char actHori[65][65] ;

void main(int argc, char *argv[])
{
    FILE *fp ;
    int x,y,a ;
    char type ;

    if (argc != 3)  {
        puts("USAGE: CONVXY <input xy file> <output file>") ;
    } else {
        memset(actMap, 0, sizeof(actMap)) ;
        memset(actVert, 0, sizeof(actVert)) ;
        memset(actHori, 0, sizeof(actHori)) ;

        fp = fopen(argv[1], "r") ;
        if (fp == NULL)  {
            puts("Cannot open file!") ;
            exit(1) ;
        }

        fscanf(fp, "%c %d %d %d", &type, &x, &y, &a) ;
        while (!feof(fp))  {
            if (type == 'f')  {
                actMap[y][x] = a ;
            } else if (type == 'v')  {
                actVert[y][x] = a ;
            } else if (type == 'h')  {
                actHori[y][x] = a ;
            }
            fscanf(fp, "%c %d %d %d", &type, &x, &y, &a) ;
        }
        fclose(fp) ;

        fp = fopen(argv[2], "wb") ;
        if (fp == NULL)  {
            puts("Cannot open output file!") ;
            exit(1) ;
        }

        fwrite(actMap, sizeof(actMap), 1, fp) ;
        fwrite(actVert, sizeof(actVert), 1, fp) ;
        fwrite(actHori, sizeof(actHori), 1, fp) ;
        fclose(fp) ;
    }
}
