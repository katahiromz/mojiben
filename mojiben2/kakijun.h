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
    INT cx;
    INT cy;
    INT res;
};
typedef std::vector<std::vector<STROKE> > KAKIJUN;

// 書き順イメージのサイズ。
#define KAKIJUN_SIZE 254
// 書き順イメージの中心点。
#define KAKIJUN_CENTER_XY (KAKIJUN_SIZE / 2)
