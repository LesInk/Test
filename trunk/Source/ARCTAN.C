#include <math.h>
#include <stdio.h>
#include <malloc.h>

unsigned int G_arcTan[256] ;

void main(void)
{
    long x, y ;
    FILE *fp ;

    fp = fopen("arctan.dat", "wb") ;

    for (x=-128; x<128; x++)  {
        for (y=-128; y<128; y++)  {
            if ((x != 0) || (y != 0))
                G_arcTan[y+128] = 32768L*(atan2((double)y, (double)x)/M_PI) ;
            else
                G_arcTan[y+128] = 0 ;
//printf("x=%ld, y=%ld, arcTan=%04X\n", x, y, G_arcTan[y+128]) ;
        }
        fwrite(G_arcTan, sizeof(G_arcTan), 1, fp) ;
    }

    fclose(fp) ;
}

