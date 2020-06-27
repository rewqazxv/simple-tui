#include "ui.h"

void input_echo() {
    msgbox(inputbox("Please input:"),"Your input is");
}

void submenu() {
    entry_t es[]= {
        {"* NORMAL",0},
        {"Input & echo",input_echo},
        {"Again",input_echo},
        {"* SPECIAL",0},
        {"Exit with 0",normal_quit},
        {"",0},
        {"Back",menu_back},
        {}
    };
    for (int selected=0; selected!=-1 && es[selected].act!=menu_back;
            selected=menu("Sub menu",es,selected));
}

int main() {
    entry_t es[]= {
        {"Input & echo",input_echo},
        {"Sub menu",submenu},
        {"",0},
        {"Quit",normal_quit},
        {}
    };
    for (int selected=0;; selected=menu("Test",es,selected));
}
