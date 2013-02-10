#include "standbor.h"
#include <string.h>
#include <ctype.h>

FILE *fp ;
FILE *fout ;
T_MapInformation mapInfo ;
int line = 0 ;

T_void dotpic(T_byte8 *name)
{
    strupr(name) ;
    while (*name != '.')
        name++ ;
    strcpy(name, ".PIC") ;
}

T_void namepastcomma(T_byte8 *buffer, T_byte8 *name)
{
    while ((*buffer != ',') && (*buffer != '\0'))
        buffer++ ;
    if (*buffer == '\0')  {
       printf("Error in line %d!", line) ;
       exit(1) ;
    }
    buffer++ ;
    sscanf(buffer, "%s", name) ;

    dotpic(name) ;
}

T_void ReadIn(T_void)
{
    char buffer[200] ;
    int mode = 0 ;
    int num ;
    char filename[80] ;
    int picnum ;

    fgets(buffer, 200, fp) ;
    line++ ;
    while (!feof(fp))  {
        if ((buffer[0] != '\n') && (buffer[0] != '\r') && (buffer[0] != ';'))  {
            switch (mode)  {
                case 0:
                    if (strncmp("Walls:", buffer, 6)==0)
                        mode = 1 ;
                    else if (strncmp("Objects:", buffer, 8)==0)
                        mode = 2 ;
                    break ;
                case 1:
                    if (strncmp("EndWalls:", buffer, 9)==0)
                        mode = 0 ;
                    else if (isdigit(buffer[0]))  {
                        sscanf(buffer, "%d", &num) ;
                        namepastcomma(buffer, filename) ;
                        printf("WALL #%3d: %s\n", num, filename) ;
                        strcpy(mapInfo.wallNames[num], filename) ;
                    }
                    break ;
                case 2:
                    if (strncmp("EndObjects:", buffer, 11)==0)
                        mode = 0 ;
                    else if (isdigit(buffer[0]))  {
                        sscanf(buffer, "%d", &num) ;
                        namepastcomma(buffer, filename) ;
                        printf("OBJECT #%3d: %s\n", num, filename) ;
                        strcpy(mapInfo.objectNames[num], filename) ;
                    } else if (strncmp("Number:", buffer, 7)==0)  {
                        sscanf(buffer+7, "%d", &num) ;
                        mode = 3 ;
                    }
                    break ;
                case 3:
                    if (strncmp("Bitmaps:", buffer, 8)==0)  {
                        sscanf(buffer+8, "%d", &picnum) ;
                        printf("Item #%d assigned to picture %d\n", num, picnum) ;
                        mapInfo.staticObjectBitmaps[num] = picnum ;
                        mode = 2 ;
                    }
                    break ;
            }
        }
        fgets(buffer, 200, fp) ;
        line++ ;
    }
}

T_void PlayerStarts(T_void)
{
    int num ;
    int i ;

    printf("How many player starts?  #") ;
    scanf("%d", &num) ;
    for (i=0; i<num; i++)  {
        printf("Player %d start X: ", i+1) ;
        scanf("%d", &mapInfo.playerX[i]) ;
        printf("Player %d start Y: ", i+1) ;
        scanf("%d", &mapInfo.playerY[i]) ;
        printf("Player %d start Angle: ", i+1) ;
        scanf("%d", &mapInfo.playerAngle[i]) ;
    }
}

T_void main(T_word16 argc, T_byte8 *argv[])
{
    memset(&mapInfo, 0, sizeof(mapInfo)) ;
    memset(mapInfo.staticObjectBitmaps, 255, 256) ;
    if (argc != 3)  {
        puts("USAGE: CONVEINF <info file> <output file>") ;
    } else {
        fp = fopen(argv[1], "r") ;
        if (fp == NULL)  {
            puts("Cannot open input file.") ;
        } else {
            fout = fopen(argv[2], "wb") ;
            if (fout == NULL)  {
                puts("Cannot open output file.") ;
            } else {
                puts("Reading input file.") ;
                ReadIn() ;
                PlayerStarts() ;
                puts("Outputting file.") ;
                fwrite(&mapInfo, sizeof(mapInfo), 1, fout) ;
                fclose(fout) ;
            }
        }
    }
}
