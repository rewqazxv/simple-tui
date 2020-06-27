#ifndef DEMO_H
#define DEMO_H

extern double demo_speed; // 最小下落速度, 默认为7字符每秒
extern double refresh_rate; // 刷新率, 默认为25Hz
extern int earth_lines; // 地面厚度, 默认为5字符
extern int n_meteors; // 流星数量, 默认为10
extern int speed_levels; // 速度档位数量, 默认为4
extern int meteor_char; // 表示流星的字符, 默认为0随机生成大写字母
extern int color_enabled; // 是否开启颜色(终端不支持颜色则始终关闭), 默认开启
extern int earth_char; // 表示地面的字符, 默认为'#'
extern int endflag; // 地面被击穿后动画是否结束, 默认开启

int start_demo(); // 正常返回0

#endif // DEMO_H
