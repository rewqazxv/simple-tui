#include <stdio.h> // sprintf, sscanf

#include "../simple-tui/ui.h"
#include "../meteor-shower/meteor-shower.h"

void Start() {
    int status=start_demo();
    static char uerr[128];
#define caseerr(num,dec) case num:msgbox("An error occurred: " dec,"Error");break;
    switch (status) {
    case 0:return;
    caseerr(32,"can not show full content");
    caseerr(33,"the slowest meteor's speed must >0");
    caseerr(34,"refresh rate must >0");
    caseerr(37,"earth must have >=0 lines");
    caseerr(35,"meteors must >0");
    caseerr(36,"speed levels must >0");
    default:
        sprintf(uerr,"Unknown error: %d",status);
        msgbox(uerr,"Error");
        break;
    }
#undef caseerr
}

void set_speed() {
    double set_value;
    static char *tip="Set minimum speed (1~50, float)";
    static char *errtip="Check your input! Minimum speed (can\nbe non-integer) is between 1~50";
    while (sscanf(inputbox(tip),"%lf",&set_value)!=1 || !(set_value>=1 && set_value<=50))
        msgbox(errtip,"Invalid input");
    demo_speed=set_value;
}
void set_refresh_rate() {
    double set_value;
    static char *tip="Set refresh rate (25~80, float)";
    static char *errtip="Check your input! Refresh rate (can\nbe non-integer) is between 25~80";
    while (sscanf(inputbox(tip),"%lf",&set_value)!=1 || !(set_value>=25 && set_value<=80))
        msgbox(errtip,"Invalid input");
    refresh_rate=set_value;
}
void Settings() {
    char enames[2][30];
    entry_t entry_list[]={
        {enames[0],set_speed},
        {enames[1],set_refresh_rate},
        {}
    };
    for (int selected_entry=0;selected_entry!=-1;) {
        sprintf(enames[0],"Minimum Speed  [%.3g]",demo_speed);
        sprintf(enames[1],"Refresh Rate   [%.3g]",refresh_rate);
        selected_entry=menu("Settings",entry_list,selected_entry);
    }
}

int main() {
#define E(f) {#f,f}
    entry_t entry_list[]={
        E(Start),
        E(Settings),
        {"",0},
        {"Quit",normal_quit},
        {}
    };
    for (;;) menu("Meteor Shower",entry_list,0);
}
