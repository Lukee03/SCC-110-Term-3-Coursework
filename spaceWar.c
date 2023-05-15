#include <ncurses.h>
#include <math.h>
#include <stdlib.h>

typedef struct 
{
    float shipPosition[2], shipRad;
    int shipRotation;
    bool destroyed;
}Ship;

int main()
{
    initscr();
    Ship *ships[2];
    ships[0] = malloc(sizeof(Ship));
    ships[1] = malloc(sizeof(Ship));
    WINDOW *win = newwin(80, 25, 5, 5);
    keypad(win, true);
    nodelay(win, true);
    for (int i = -1; i < 2; i+=2)
    {
        int id = i;
        if (i == -1)
        {
            id = 0;
        }
        ships[id]->shipRotation = -90*i;
        ships[id]->shipRad = 0.0f;
        ships[id]->shipPosition[0] = 40.0f + (30.0f * i);
        ships[id]->shipPosition[1] = 12.0f;
    }
    while(true)
    {        
        erase();    

        switch(wgetch(win))
        {
            case KEY_DOWN:
                if (!ships[0]->destroyed)
                {
                    ships[0]->shipPosition[0] = ships[0]->shipPosition[0] - sinf(ships[0]->shipRad);
                    ships[0]->shipPosition[1] = ships[0]->shipPosition[1] + cosf(ships[0]->shipRad);
                }
                break;
            case KEY_UP:
                if (!ships[0]->destroyed)
                {
                    ships[0]->shipPosition[0] = ships[0]->shipPosition[0] + sinf(ships[0]->shipRad);
                    ships[0]->shipPosition[1] = ships[0]->shipPosition[1] - cosf(ships[0]->shipRad);
                }
                break;
            case KEY_LEFT:
                if (!ships[0]->destroyed)    ships[0]->shipRotation = (ships[0]->shipRotation - 5) % 360;
                break;
            case KEY_RIGHT:
                if (!ships[0]->destroyed)    ships[0]->shipRotation = (ships[0]->shipRotation + 5) % 360;
                break;
            case KEY_NPAGE:
                if (!ships[1]->destroyed)
                {
                    ships[1]->shipPosition[0] = ships[1]->shipPosition[0] - sinf(ships[1]->shipRad);
                    ships[1]->shipPosition[1] = ships[1]->shipPosition[1] + cosf(ships[1]->shipRad);
                }
                break;
            case KEY_HOME:
                if (!ships[1]->destroyed)
                {
                    ships[1]->shipPosition[0] = ships[1]->shipPosition[0] + sinf(ships[1]->shipRad);
                    ships[1]->shipPosition[1] = ships[1]->shipPosition[1] - cosf(ships[1]->shipRad);
                }
                break;
            case KEY_END:
                if (!ships[1]->destroyed)    ships[1]->shipRotation = (ships[1]->shipRotation - 5) % 360;
                break;
            case KEY_PPAGE:
                if (!ships[1]->destroyed)    ships[1]->shipRotation = (ships[1]->shipRotation + 5) % 360;
                break;
            case KEY_BACKSPACE:
                delwin(win);
                exit(0);
                break;
            default:
                break;
        }   
        
        for (int j = 0; j < 2; j++)
        {
            ships[j]->shipRad = ships[j]->shipRotation * M_PI / 180.0f;

            if (ships[j]->shipPosition[0] < 0.0f)
            {
                ships[j]->shipPosition[0] += 80.0f;
            }
            else if (ships[j]->shipPosition[0] > 80.0f)
            {
                ships[j]->shipPosition[0] -= 80.0f;
            }
            if (ships[j]->shipPosition[1] < 0.0f)
            {
                ships[j]->shipPosition[1] += 25.0f;
            }
            else if (ships[j]->shipPosition[1] > 25.0f)
            {
                ships[j]->shipPosition[1] -= 25.0f;
            }
            else if (ships[j]->shipPosition[1] > 11.5f && ships[j]->shipPosition[1] < 13.5f && ships[j]->shipPosition[0] > 38.5f && ships[j]->shipPosition[0] < 41.5f)
            {
                ships[j]->destroyed = true;
            }

            if (!ships[j]->destroyed)
            {
                mvprintw((int)ships[j]->shipPosition[1], (int)ships[j]->shipPosition[0], "*");
                mvprintw((int)ships[j]->shipPosition[1] - (int)(1.75f * cosf(ships[j]->shipRad)),ships[j]->shipPosition[0] + (int)(1.75f * sinf(ships[j]->shipRad)), "*");
            }
        }

        int blackHoleChar = rand() % 4;

        switch(blackHoleChar)
        {
            case 0:
                mvprintw(13, 40, "-");
                break;
            case 1:
                mvprintw(13, 40, "/");
                break;
            case 2:
                mvprintw(13, 40, "|");
                break;
            case 3:
                mvprintw(13, 40, "\\");
                break;
        }
        move(0, 0);
        refresh();
        timeout(1.0f/60.0f);
    }
}