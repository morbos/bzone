#include <sys/types.h>

extern int _end;
caddr_t 
_sbrk(int incr)
{
    static unsigned char *heap = NULL;
    unsigned char *prev_heap;
    if(heap == NULL) {
	heap = (unsigned char *)&_end;
    }
    prev_heap = heap;
    // Check for out of space here...
    heap += incr;
    return (caddr_t) prev_heap;
}
