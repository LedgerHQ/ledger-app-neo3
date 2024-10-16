#ifndef APP_NEO3_SHARED_CONTEXT_H
#define APP_NEO3_SHARED_CONTEXT_H

#include "os.h"

#define N_storage (*(volatile internalStorage_t *) PIC(&N_storage_real))

typedef struct internalStorage_t {
    unsigned char scriptsAllowed;
    unsigned char showScriptHash;
    unsigned char signerAccountFormat;
    uint8_t initialized;
} internalStorage_t;

typedef struct settings_strings_t {
    char scriptsAllowed[12];  // Allowed / Not Allowed
    char showScriptHash[6]; // Show / Hide
    char signerAccountFormat[12]; // Script hash / NEO address
} settings_strings_t;

extern settings_strings_t strings;
extern const internalStorage_t N_storage_real;

#endif  // APP_NEO3_SHARED_CONTEXT_H
