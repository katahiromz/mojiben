enum TYPE
{
    WAIT,
    LINEAR,
    POLAR,
    DOT
};

struct GA
{
    INT type;
    INT angle0;
    INT angle1;
    INT cx;
    INT cy;
    DWORD cb;
    const BYTE *pb;
};
typedef map<INT, vector<GA> > KAKIJUN;

extern KAKIJUN g_print_uppercase_kakijun;
extern KAKIJUN g_print_lowercase_kakijun;

VOID InitPrintUpperCase(VOID);
VOID InitPrintLowerCase(VOID);
