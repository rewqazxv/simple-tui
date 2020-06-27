/* simple tui, 2017 */

/* NOTICE:
 * to prevent some potential bugs, if your one or more functions also use the
 * curses library, please use SCREEN *<screen_name>=newterm(0,stdout,stdin)
 * instead of initscr(), endwin() before use s-tui, and if one screen is no
 * longer needed, delscreen(<screen_name>) after endwin(). */

#ifndef UI_H
#define UI_H

typedef struct {
    char *name;
    void (*act)();
} entry_t;
/* act=NULL (act=0) makes the entry can not be selected */

int menu(const char *title,const entry_t entry_list[],int selection);
/* selection is the entry selected by default
 * it will return user's final selection, if user press Esc, return -1
 * entries without act(act=0 or NULL) can not be selected
 * lenth of entries' name is treated as 20 */
void menu_back();
/* it does nothing and causes the menu page exit */
void normal_quit();
/* exit program with 0, used to create the entry "Quit" */

const char *inputbox(const char *tip);
/* designed to accept short input
 * NOTICE: extract your information immediately! */

void msgbox(const char *msg,const char *title);
/* if title is NULL or empty string, "Message" will be used as default title */

#endif // UI_H
