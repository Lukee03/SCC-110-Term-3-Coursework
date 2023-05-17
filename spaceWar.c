#include <ncurses.h>
#include <math.h>
#include <stdlib.h>

typedef struct 
{
    float shipPosition[2], shipRad;
    int shipRotation;
    bool destroyed;
}Ship;

typedef struct item
{
    float position[2], direction;
    int id;
    struct item* next;
}Bullet;

int bulletCount;
Bullet* bullets = NULL;
float pos[2];
int score[2];

int createBullet(float dir, float pos[2])
{
    Bullet* created = NULL;
    if (bullets == NULL)
    {
        bullets = (Bullet*)malloc(sizeof(Bullet));
        created = bullets;
    }
    else
    {
        Bullet*last = bullets;
        while (last->next!=NULL)
        {
            last = last->next;
        }
        last->next = (Bullet*)malloc(sizeof(Bullet));
        created = last->next;
    }
    created->position[0] = pos[0];
    created->position[1] = pos[1];
    created->direction = dir;
    bulletCount++;
    created->id = bulletCount;
    created->next = NULL;
    return(0);
}

void deleteBullet(int id)
{
    if (id == 0)
    {
        Bullet* temp = bullets;
        if (bullets->next != NULL)
        {
            bullets = bullets->next;          
        }
        free(temp);
    }
    else
    {
        Bullet* current = bullets;
        for (int i = 0; i < id + 1; i++)
        {
            current = current->next;
            if (i == id)
            {    
                Bullet* temp = current->next;
                current->next = temp->next;
                free(temp);
            }
            else if (i > id)
            {
                current->id = id - 1;
            }
        }
    }
    bulletCount--;
}

int main()
{
    initscr();
    Ship *ships[2];
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
        ships[id] = malloc(sizeof(Ship));
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
            case KEY_BACKSPACE:
                pos[0] = (ships[0]->shipPosition[0] + (1.75f * sinf(ships[0]->shipRad)));
                pos[1] = (ships[0]->shipPosition[1] - (1.75f * cosf(ships[0]->shipRad)));
                createBullet(ships[0]->shipRad, pos);
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
            case KEY_ENTER:
                pos[0] = ships[1]->shipPosition[0] + (1.75f * sinf(ships[1]->shipRad));
                pos[1] = ships[1]->shipPosition[1] - (1.75f * cosf(ships[1]->shipRad));
                createBullet(ships[1]->shipRad, pos);
                break;
            case KEY_SEND:
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

            for (int k = 0; k < 2; k++)
            {
                if (k != j)
                {
                    if (abs(ships[j]->shipPosition[0] - ships[k]->shipPosition[0]) < 1.0f && abs(ships[j]->shipPosition[1] - ships[k]->shipPosition[1]) < 1.0f)
                    {
                        ships[j]->destroyed = true;
                        ships[k]->destroyed = true;
                    }

                    if (ships[j]->destroyed)
                    {
                        score[k]++;
                        ships[j]->destroyed = false;
                        if (j == 1)
                        {
                            ships[j]->shipPosition[0] = 70.0f;
                            ships[j]->shipRotation = -90.0f;
                        }
                        else
                        {
                            ships[j]->shipPosition[0] = 10.0f;
                            ships[j]->shipRotation = 90.0f;
                        }
                        ships[j]->shipRad = 0.0f;
                        ships[j]->shipPosition[1] = 12.0f;
                    }
                }
            }

            if (!ships[j]->destroyed)
            {
                mvprintw((int)ships[j]->shipPosition[1], (int)ships[j]->shipPosition[0], "*");
                mvprintw((int)ships[j]->shipPosition[1] - (int)(1.75f * cosf(ships[j]->shipRad)),ships[j]->shipPosition[0] + (int)(1.75f * sinf(ships[j]->shipRad)), "*");
            }
        }

        Bullet * current = bullets;

        for (int l = 0; l < bulletCount; l++)
        {
            if (current->position[0] > 80.0f || current->position[0] < 0.0f || current->position[1] < 0.0f || current->position[1] > 25.0f)
            {
                deleteBullet(l);
            }
            else
            {
                current->position[0] += 0.001f * sinf(current->direction);
                current->position[1] -= 0.001f * cosf(current->direction);
                mvprintw((int)current->position[1], (int)current->position[0], "*");
            }
            for (int m = 0; m < 2; m++)
            {
                if (abs(ships[m]->shipPosition[0] - current->position[0]) < 1.0f && abs(ships[m]->shipPosition[1] - current->position[1]) < 1.0f)
                {
                    deleteBullet(l);
                    ships[m]->destroyed = true;
                }
            }
            if (current->next != NULL)
            {
                current = current->next;
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

        mvprintw(2, 10, "%d", score[0]);
        mvprintw(2, 70, "%d", score[1]);
        move(0, 0);
        refresh();
        timeout(1.0f/60.0f);
    }
}