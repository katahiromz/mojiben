// kakijun.h
// License: MIT

#include <vector>

// stroke info
struct STROKE {
    enum TYPE {
        WAIT,
        LINEAR,
        POLAR,
        DOT
    };
    TYPE type;
    INT angle0;
    INT angle1;
    INT cx;     // width
    INT cy;     // height
};
typedef std::vector<std::vector<STROKE> > KAKIJUN;

#define KAKIJUN_SIZE 254
#define KAKIJUN_CENTER_XY (KAKIJUN_SIZE / 2)
