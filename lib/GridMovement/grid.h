#ifndef "grid.h"
#define "grid.h"

struct veikryss {
    bool venstre;
    bool frem;
    bool hoyre;
}

void gridInit();
bool tilVeikryss(veikryss &out);

#endif