#include <math.h>
#include <stdio.h>
#include <malloc.h>

unsigned char G_distanceTable[128] ;

void main(void)
{
    unsigned long x, y, i ;
    FILE *fp ;

    fp = fopen("sqrtsml.dat", "wb") ;

    for (x=i=0; x<128; x++)  {
        for (y=0; y<128; y++, i++)  {
            G_distanceTable[y] = sqrt(((double)x * (double)x) +
                                   ((double)y * (double)y)) ;
        }
        fwrite(G_distanceTable, sizeof(G_distanceTable), 1, fp) ;
    }

    fclose(fp) ;
}

