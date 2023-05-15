#include <ncurses.h>
#include <math.h>
#include <stdlib.h>

int main()
{
    initscr();
    WINDOW *win = newwin(80, 25, 5, 5);
    keypad(win, true);
    int shipRotation = 0;
    float shipRad = 0.0f;
    float shipPosition[2];
    shipPosition[0] = 40;
    shipPosition[1] = 12;
    mvprintw(40, 12, "*");
    mvprintw(40, 11, "*");
    while(true)
    {        
        mvprintw((int)shipPosition[1], (int)shipPosition[0], " ");  
        mvprintw((int)shipPosition[1] - (int)(1.75f * cosf(shipRad)),shipPosition[0] + (int)(1.75f * sinf(shipRad)), " ");       
        switch(wgetch(win))
        {
            case KEY_DOWN:
                shipPosition[0] = shipPosition[0] - sinf(shipRad);
                shipPosition[1] = shipPosition[1] + cosf(shipRad);
                break;
            case KEY_UP:
                shipPosition[0] = shipPosition[0] + sinf(shipRad);
                shipPosition[1] = shipPosition[1] - cosf(shipRad);
                break;
            case KEY_LEFT:
                shipRotation = (shipRotation - 5) % 360;
                break;
            case KEY_RIGHT:
                shipRotation = (shipRotation + 5) % 360;
                break;
            case KEY_BACKSPACE:
                delwin(win);
                exit(0);
                break;
        }

        shipRad = shipRotation * M_PI / 180.0f;

        if (shipPosition[0] < 0.0f)
        {
            shipPosition[0] += 80.0f;
        }
        else if (shipPosition[0] > 80.0f)
        {
            shipPosition[0] -= 80.0f;
        }
        if (shipPosition[1] < 0.0f)
        {
            shipPosition[1] += 25.0f;
        }
        else if (shipPosition[1] > 25.0f)
        {
            shipPosition[1] -= 25.0f;
        }

        mvprintw((int)shipPosition[1], (int)shipPosition[0], "*");
        mvprintw((int)shipPosition[1] - (int)(1.75f * cosf(shipRad)),shipPosition[0] + (int)(1.75f * sinf(shipRad)), "*");
        refresh();
        timeout(1.0f/60.0f);
    }
}