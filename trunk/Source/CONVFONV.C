#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

typedef struct {
    char fontID[3] ;
    unsigned char height ;
    unsigned char data[] ;
} FVFont ;

typedef struct {
    char fontID[4] ;
    unsigned char height ;
    unsigned char widths[256] ;
    unsigned char data[] ;
} GameFont ;

int DetermineWidth(unsigned char *ptr, int height)
{
    int i, j ;
    unsigned char mask = 0x01 ;


    for (j=8; j>0; j--, mask<<=1)  {
        for (i=0; i<height; i++)  {
            if ((ptr[i] & mask)!=0)
                break ;
        }
        if (i != height)
            break ;
    }

    printf("%3d ", j+1) ;
    return j+1 ;
}

void main(int argc, char *argv[])
{
    FILE *fp ;
    FILE *fout ;
    FVFont *p_fvFont ;
    GameFont *p_gameFont ;
    int i ;

    if (argc != 3)  {
        puts("USAGE:  CONVFONT <font filename> <file output>") ;
        exit(1) ;
    }

    fp = fopen(argv[1], "rb") ;
    if (fp == NULL)  {
        puts("Cannot open input font file.") ;
        exit(1) ;
    }

    p_fvFont = malloc(16384) ;
    if (p_fvFont == NULL)  {
        puts("Out of memory") ;
        exit(1) ;
    }

    p_gameFont = malloc(16384) ;
    if (p_gameFont == NULL)  {
        puts("Out of memory") ;
        exit(1) ;
    }

    fread(p_fvFont, 16384, 1, fp) ;
    if (strcmp(p_fvFont->fontID, "FV") != 0)  {
        puts("Input file is not FV font") ;
        exit(1) ;
    }

    fout = fopen(argv[2], "wb") ;
    if (fout == NULL)  {
        puts("Cannot open output font file.") ;
        exit(1) ;
    }

    strcpy(p_gameFont->fontID, "Fon") ;
    p_gameFont->height = p_fvFont->height ;

    for (i=0; i<256; i++)
        p_gameFont->widths[i] = DetermineWidth(
                                    &p_fvFont->data[p_fvFont->height*i],
                                    p_fvFont->height) ;

    p_gameFont->widths[32] = (p_fvFont->height+1)>>1 ;

    for (i=0; i<(p_gameFont->height * 256); i++)
        p_gameFont->data[i] = p_fvFont->data[i] ;

    fwrite(p_gameFont, (p_gameFont->data+i)-((unsigned char *)p_gameFont), 1, fout) ;

    fclose(fp) ;
    fclose(fout) ;

    puts("Done.") ;
}
