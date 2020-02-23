
#ifndef DEFS_H
#define DEFS_H

#define lock asm { pushf; cli }

#define unlock asm popf

typedef void interrupt (*pInterrupt)(...); // getvect and setvect functions demand this prototype

int min(int l, int r);

int syncPrintf(const char *format, ...);

#endif
