#include "meteor-shower.h"

#include <stdlib.h> // 随机数
#include <time.h> // 用时间初始化随机数种子

#include <curses.h>

// 控制变量
double demo_speed=7; // 实际速度=相对速度*该变量(单位: 字符/秒)
double refresh_rate=25; // 刷新率(单位: Hz)
int earth_lines=5; // 地面厚度
int n_meteors=10; // 流星数量
int speed_levels=4; // 速度档位数量
int meteor_char=0; // 代表流星的字符, 为0时随机生成大写字母
int color_enabled=1; // 开启颜色
int earth_char='#'; // 表示地面的字符
int endflag=20170629; // 允许动画结束(为0时即便有地面被击穿也不结束动画)

// 延迟函数 平台相关
#if defined(__unix__)
#include <unistd.h>
static inline void local_sleep_ms(int ms) {
    usleep(ms*1000);
}
#elif defined(_WIN32)
#include <windows.h>
static inline void local_sleep_ms(int ms) {
    Sleep(ms);
}
#endif

typedef struct {
    int y,x;
    int speed;
    int timepast_ms; // 从生成该点起至今的时间
    int char_pattern;
} meteor_t;

int start_demo() {
    // 合法性检查
    if (demo_speed<=0) return 33;
    if (refresh_rate<=0) return 34;
    if (earth_lines<0) return 37;
    if (n_meteors<=0) return 35;
    if (speed_levels<=0) return 36;
    // 需要启动curses之后进行的检查
    SCREEN *local_scr=newterm(0,stdout,stdin); // 为了避免潜在问题新建一个屏幕
    int width=COLS,height=LINES-1; // 防止运行时窗口尺寸改变, NOTICE: height是演示区域的高度
    if (width<n_meteors || height<earth_lines) { // 若尺寸不合适
        endwin();
        delscreen(local_scr);
        return 32;
    }

    // 初始化
    cbreak();
    noecho(); // 输入不回显
    nonl(); // 输入不换行(回车键为'\r'), 真的有用吗?
    curs_set(0); // 隐藏光标
    if (!has_colors()) color_enabled=0; // 若终端本身不支持颜色则禁用颜色, 假定终端不能中途改变颜色属性
    if (color_enabled) start_color(); // 开启颜色
#ifndef _WIN32
    // 假定在windows下使用pdcurses没有esc delay, *nix上使用的是ncurses
    set_escdelay(0);
#endif
    srand((unsigned)time(NULL)); // 初始化随机数生成器
    int sleep_time_ms=1000/refresh_rate; // 刷新的间隔时间(假设刷新的时间可以忽略不计)

    // 演示区域
    WINDOW *demo_area=newwin(height,width,0,0);
    scrollok(demo_area,FALSE); // 显式声明防止页面滚动

    // 提示区域
    WINDOW *tip_area=newwin(1,width,height,0);
    nodelay(tip_area,TRUE); // 若没按任何键就跳过(getch类函数直接返回ERR而不是等待按键)
    keypad(tip_area,TRUE); // 防止把方向键作为以Esc(^[)开头的转义序列处理
    scrollok(tip_area,FALSE); // 显式声明防止页面滚动
    mvwprintw(tip_area,0,4,"[Esc] Pause");
    wrefresh(tip_area);

#define INIT_METEOR(i) { \
    meteor[i].y=-1; /* 从零上一格开始 */ \
    do {meteor[i].x=rand()%width;} while (meteor_check[meteor[i].x]); \
    meteor_check[meteor[i].x]=1; \
    meteor[i].speed=rand()%speed_levels+1; \
    if (meteor_char==0) meteor[i].char_pattern=rand()%26+'A'; \
    else meteor[i].char_pattern=meteor_char; \
    meteor[i].timepast_ms=0; \
    /* 这里判断color_enabled防止出现除以0的情况, 注意第i个流星对应i+1号pair, 假定COLOR_PAIRS-1>=流星数量 (0号pair貌似不能自定义) */ \
    if (color_enabled) init_pair(i+1,rand()%COLORS,0); \
}

    // 初始化记录
    int earth[width]; // 地面
    for (int i=0; i<width; ++i) {
        earth[i]=earth_lines;
        for (int j=0; j<earth_lines; ++j)
            mvwaddch(demo_area,height-1-j,i,earth_char); // 生成地面
    }
    meteor_t meteor[n_meteors]; // 流星
    int meteor_check[width]; // 判断一条竖直线上是否已有流星
    for (int i=0; i<width; ++i) meteor_check[i]=0; // 清空记录
    for (int i=0; i<n_meteors; ++i) INIT_METEOR(i);

    // 演示
    for (int continueflag=1; continueflag; wrefresh(demo_area),local_sleep_ms(sleep_time_ms)) {
        if (wgetch(tip_area)==27) { // 检测是否按过Esc
            wclear(tip_area);
            wattron(tip_area,A_BLINK);
            mvwprintw(tip_area,0,4,"[Q] Leave  [Esc] Continue");
            wrefresh(tip_area);
            nodelay(tip_area,FALSE); // 没按键就一直等着
            for (int c;c=wgetch(tip_area),c!=27;)
                if (c=='q' || c=='Q') goto END;
            nodelay(tip_area,TRUE);
            wclear(tip_area);
            wattroff(tip_area,A_BLINK);
            mvwprintw(tip_area,0,4,"[Esc] Pause");
            wrefresh(tip_area);
        }
        for (int i=0; i<n_meteors; ++i) {
            int position=meteor[i].speed*demo_speed*meteor[i].timepast_ms/1000-1; // 位置(从上往下计)=相对速度*倍率*时间(注意单位换算)-1(零上一格开始)
            if (position!=meteor[i].y) {
                mvwaddch(demo_area,meteor[i].y,meteor[i].x,' '); // 擦除旧位置
                if (position+earth[meteor[i].x]>=height) { // 撞到地上
                    mvwaddch(demo_area,height-earth[meteor[i].x],meteor[i].x,' '); // 擦除地面
                    if (earth[meteor[i].x]>0) --earth[meteor[i].x]; // 更新地面记录
                    if (earth[meteor[i].x]==0 && endflag) continueflag=0; // 若地面击穿则不进行下一轮
                    meteor_check[meteor[i].x]=0; // 清除流星记录
                    INIT_METEOR(i); // 重新生成
                    continue; // 直接到下个
                } else { // 没撞到, 更新流星位置
                    wattron(demo_area,COLOR_PAIR(i+1)); // 第i个流星对应i+1号pair, 假定若颜色未开启此语句会被忽略
                    mvwaddch(demo_area,meteor[i].y=position,meteor[i].x,meteor[i].char_pattern);
                    wattroff(demo_area,COLOR_PAIR(i+1)); // 这里好像pair编号任取均可以消去颜色
                }
            }
            meteor[i].timepast_ms+=sleep_time_ms; // 更新时间至下次刷新时
        }
    }

    //结束
    wclear(tip_area);
    wattron(tip_area,A_BLINK);
    mvwprintw(tip_area,0,4,"Press [Enter] to leave...");
    wrefresh(tip_area);
    while (getchar()!='\r'); // cbreak模式下按键立即获得
END:
    endwin();
    delwin(demo_area);
    delwin(tip_area);
    delscreen(local_scr);
    return 0;
}
