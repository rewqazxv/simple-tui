#include "ui.h"

#include <string.h> // strcmp
#include <stdlib.h> // system, exit

#include <curses.h>


#if defined(__unix__)
#define SYSTEM_CLEAR_SCREEN system("clear")
#elif defined(_WIN32)
#define SYSTEM_CLEAR_SCREEN system("cls")
#endif


int menu(const char *title,const entry_t entry_list[],int selection) {
    // init curses
    SCREEN *local_scr=newterm(0,stdout,stdin); // assume that set_term() can be omitted
    noecho();
    cbreak();
    curs_set(0); // hide cursor
    nonl(); // no new line (in my opinion, treat Enter from '\n' to '\r')
#ifndef _WIN32
    // assume that pdcurses on windows has no esc delay, and you use ncurses on *nix
    set_escdelay(0);
#endif

    // create menu
#define MENU_WIDTH 30
#define ENTRY_WIDTH 20
    // entries
    int nentries,maxlen=ENTRY_WIDTH; // number of entries, max length of entries
    for (nentries=0; entry_list[nentries].name; nentries++);
    // window
    int wh=nentries+4,ww=MENU_WIDTH; // height & width of window
    int wy=(LINES-wh)/2,wx=(COLS-ww)/2; // base point, makes the window center
    WINDOW *menuwin=newwin(wh,ww,wy,wx); // create window
    box(menuwin,0,0); // draw border of window
    // line
    int ll=.8*ww,ly=2,lx=(ww-ll)/2; // length & point of the horizontal line, relative to the window
    mvwhline(menuwin,ly,lx,'-',ll); //  draw the line
    // contents
    mvwaddstr(menuwin,ly/2,(ww-strlen(title))/2,title); // print title
    int ex=(ww-maxlen)/2;
    for (int i=0; i<nentries; i++) // print entries
        mvwaddstr(menuwin,3+i,ex,entry_list[i].name);

    // select entry
    keypad(menuwin,TRUE); // enable the arrow keys support (treat each arrow key as one charactor instead of escape sequence)
    int key_pressed;
    // HACK: how to detect Enter & Esc key? here I defined two local macros
#define LOCAL_KEY_ENTER '\r'
    // rely on "nonl()"
#define LOCAL_KEY_ESC 27
    int i;
    for (i=0; entry_list[i].name; i++)
        if (entry_list[i].act) break; // check whether selectable
    if (i==nentries) { // no selectable entry
        do {
            key_pressed=wgetch(menuwin); // wgetch() auto refreshes window
        } while (key_pressed!=LOCAL_KEY_ENTER &&
                 key_pressed!=LOCAL_KEY_ESC); // both Enter and Esc can leave menu
        key_pressed=LOCAL_KEY_ESC; // treat Enter as Esc due to no selectable entry
    } else {
        if (selection<0 || selection >=nentries || !entry_list[selection].act)
            selection=i; // if default entry is out of limit, or has no action, set the first selectable entry as default

// highlight the entry
#define HETY(onoroff,nthety) { \
    wattr##onoroff(menuwin,A_REVERSE); \
    mvwprintw(menuwin,3+nthety,lx,"%*s%-*s",ex-lx,"",ll-ex+lx,entry_list[nthety].name); \
    } // use HETY(on,n) to highlight the n-th entry , and HETY(off,n) to cancel

        HETY(on,selection);
        int move_direction; // +1 or -1
        do {
            key_pressed=wgetch(menuwin);
            switch (key_pressed) {
            case KEY_UP:
            case KEY_DOWN:
                move_direction=(key_pressed==KEY_UP)?-1:+1;
                for (int i=selection+move_direction; i>=0 && i<nentries; i+=move_direction)
                    if (entry_list[i].act) {
                        HETY(off,selection);
                        HETY(on,i);
                        selection=i;
                        break;
                    }
                break;
            }
        } while (key_pressed!=LOCAL_KEY_ENTER &&
                 key_pressed!=LOCAL_KEY_ESC);
    }

    // end menu
    endwin(); // back to normal mode
    SYSTEM_CLEAR_SCREEN; // clear the normal mode screen
    delwin(menuwin);
    delscreen(local_scr);

    // jump & return
    if (key_pressed==LOCAL_KEY_ENTER)
        (*entry_list[selection].act)(); // run the selected function
    else selection=-1; // Esc
    return selection;
}

void menu_back() {} // as you see, it does nothing

void normal_quit() {exit(0);} // "Quit" entry


const char *inputbox(const char *tip) {
    // string to return
    static char inputs[256];

    // init curses
    SCREEN *local_scr=newterm(0,stdout,stdin);
    nocbreak();

    // create the outer box
#define IB_WIDTH 40
    int wh=4,ww=IB_WIDTH;
    int wy=(LINES-wh)/2,wx=(COLS-ww)/2;
    WINDOW *obox=newwin(wh,ww,wy,wx);
    wborder(obox,' ',' ',' ',0,' ',' ',0,0); // bottom line
    char *icon=">"; // NOTICE: this is string
    mvwprintw(obox,0,0,"%s %s",icon,tip); // print tip
    wrefresh(obox);

    // create the input part
    WINDOW *ibox=newwin(1,ww-4,wy+2,wx+2); // HACK: subwin or subpad?
    wmove(ibox,0,0);
    /* TODO: fix arrow key support */
    wgetnstr(ibox,inputs,sizeof(inputs)); // it uses wgetch() so wrefresh() is unnecessary)

    // end inputbox
    endwin();
    SYSTEM_CLEAR_SCREEN;
    delwin(ibox);
    delwin(obox);
    delscreen(local_scr);

    return inputs;
}


void msgbox(const char *msg,const char *title) {
    // init curses
    SCREEN *local_scr=newterm(0,stdout,stdin);
    noecho();
    cbreak();
    curs_set(0);
    nonl(); // NOTICE

    // outer window
#define MB_WIDTH 50
    int wh=7,ww=MB_WIDTH;
    int wy=(LINES-wh)/2,wx=(COLS-ww)/2;
    WINDOW *obox=newwin(wh,ww,wy,wx);
    box(obox,0,0);
    if (!title || !*title) title="Message";
    mvwprintw(obox,0,(ww-strlen(title)-4)/2,"[ %s ]",title);
    wattron(obox,A_REVERSE);
    mvwaddstr(obox,wh-2,ww-14,"  Enter  ");
    wattroff(obox,A_REVERSE);
    wrefresh(obox);

    // content window
    WINDOW *content=newwin(wh-4,ww-4,wy+2,wx+2);
    waddstr(content,msg);
    wrefresh(content);

    // wait for enter
    while (getchar()!='\r');

    // end msgbox
    endwin();
    SYSTEM_CLEAR_SCREEN;
    delwin(obox);
    delwin(content);
    delscreen(local_scr);
}
