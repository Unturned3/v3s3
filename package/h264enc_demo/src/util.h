
#ifndef _util_h_
#define _util_h_

#include <stdarg.h>
#include <string.h>

#define CLEAR(x) memset(&(x), 0, sizeof(x))

#define check_ret(x, y) do{ if ((x)<0) {perror(y); return -1;} }while(0)
#define check_cleanup(x, y) do{ if ((x)<0) {perror(y); ret = -1; goto cleanup;} }while(0)

#define errno_cleanup(x) do{ perror(x); goto cleanup; }while(0)
#define err_cleanup(...) do{ dlog(__VA_ARGS__); goto cleanup; }while(0)

#define ALIGN_4K(x) (((x) + 4095) & ~4095)
#define ALIGN_1K(x) (((x) + 1023) & ~1023)
#define ALIGN_32B(x) (((x) + 31) & ~31)
#define ALIGN_16B(x) (((x) + 15) & ~15)
#define ALIGN_8B(x) (((x) + 7) & ~7)

// debug logging function (wrapper to fprintf)
void dlog(const char *format, ...);

int virt_to_phys_user(uintptr_t *paddr, uintptr_t vaddr);

#endif