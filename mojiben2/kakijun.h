enum TYPE
{
    WAIT,
    LINEAR,
    POLAR,
    DOT
};

struct STROKE
{
    INT type;
    INT angle0;
    INT angle1;
    INT cx;
    INT cy;
    INT res;
};
typedef std::vector<STROKE> KAKIJUN[26];

extern KAKIJUN g_print_uppercase_kakijun;
extern KAKIJUN g_print_lowercase_kakijun;

VOID InitPrintUpperCase(VOID);
VOID InitPrintLowerCase(VOID);
