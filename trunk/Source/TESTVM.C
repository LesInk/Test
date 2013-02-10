#include "standard.h"
#include "vm.h"

T_void main(T_void)
{
    T_vmFile file ;
    T_word16 i ;
    T_vmBlock blocks[10] ;
    T_vmBlock newBlock ;
    T_byte8 *ptr ;

    file = VMOpen("testfile") ;
    for (i=0; i<10; i++)  {
        printf("Allocated %d at %d\n", i, blocks[i] = VMAlloc(file, 1000)) ;
    }

    ptr = VMLock(file, blocks[5]) ;
    memset(ptr, 'A', 1000) ;
    strcpy(ptr, "I am here.") ;
    VMDirty(file, blocks[5]) ;
    VMUnlock(ptr) ;

    VMDecRefCount(file, blocks[5]) ;
    printf("Allocated block %d\n", newBlock = VMAlloc(file, 1000)) ;

    VMClose(file) ;

    puts("Cleaning up...") ;
    VMCleanUp("testfile") ;
    puts("Done.") ;
}

