#include <ncurses.h>

int main()
{
    initscr();
    WINDOW *win = newwin(50, 50, 5, 5);
    int x = 5;
    int y = 5;
    keypad(win, true);
    while(true)
    {        
        mvprintw(y, x, " ");       
        switch(wgetch(win))
        {
            case KEY_DOWN:
                y++;
                break;
            case KEY_UP:
                y--;
                break;
            case KEY_LEFT:
                x--;
                break;
            case KEY_RIGHT:
                x++;
                break;
            case KEY_BACKSPACE:
                endwin();
                break;
        }
        mvprintw(y, x, "*");
        refresh();
        timeout(1.0f/60.0f);
    }
}