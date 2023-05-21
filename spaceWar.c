#include <ncurses.h>
#include <math.h>
#include <stdlib.h>

//Defines ship properties as a struct for easier duplication
typedef struct 
{
    float shipPosition[2], shipRad, shipMomentum[2], fuel;
    int shipRotation, shootTimer, bulletCount;
    bool destroyed;
}Ship;

//Defines bullet properties, storing struct as item for linked list functionality
typedef struct item
{
    float position[2], direction;
    int id;
    struct item* next;
}Bullet;

//Defines global game variables for other functions to use
int bulletCount = 0;
Bullet* bullets = NULL;
float pos[2];
int score[2];

//Adds new bullet to linked list
int createBullet(float dir, float pos[2])
{
    Bullet* created = NULL;
    if (bullets == NULL)
    {
        //Assigns 1st bullet to head if no other bullets are present...
        bullets = (Bullet*)malloc(sizeof(Bullet));
        created = bullets;
    }
    else
    {
        //...Otherwise adding them to the end of list
        Bullet*last = bullets;
        while (last->next!=NULL)
        {
            last = last->next;
        }
        last->next = (Bullet*)malloc(sizeof(Bullet));
        created = last->next;
    }

    //Defines initial properties for the bullet based on input to function and assigns id based on number of bullets on screen
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
        //Deletes value stored at head if at root, shifting remaining values if there are other bullets on screen
        Bullet* temp = bullets;
        if (bullets->next != NULL)
        {
            bullets = bullets->next;   
            Bullet* current = bullets;
            for (int i = 0; i < bulletCount - 1; i++)
            {
                current->id = current->id - 1;
                current = current->next;
            }       
        }
        free(temp);
    }
    else
    {
        //Otherwise sets destroyed bullet's next in list to place bullet was (or frees last item if most recently created)
        Bullet* current = bullets;
        bool freed = false;
        for (int i = 0; i < bulletCount; i++)
        {
            if (current->next != NULL)
            {
                if (i == id - 1 && !freed)
                {    
                    Bullet* temp = current->next;
                    current->next = temp->next;
                    freed = true;
                    free(temp);
                }
                else if (i > id - 1)
                {
                    current->id = current->id - 1;
                }
                current = current->next;
            }
            else if (!freed)
            {
                free(current);
            }
        }
    }

    //Always decreases bullet count to help id tracking
    bulletCount--;
}

int main()
{
    //Initialises window and input settings
    initscr();
    WINDOW *win = newwin(80, 25, 5, 5);
    keypad(win, true);
    nodelay(win, true);

    //Sets splash screen for game
    while(true)
    {
        mvprintw(3, 36, "Spacewar!");
        mvprintw(8, 10, "Player 1 controls:");
        mvprintw(8, 50, "Player 2 controls:");
        mvprintw(10, 10, "Up/down arrows - Thrust");
        mvprintw(10, 50, "7/3 on numpad - Thrust");
        mvprintw(11, 10, "Left/right arrows - Turn");
        mvprintw(11, 50, "1/9 on numpad - Thrust");
        mvprintw(12, 10, "BACKSPACE - Shoot");
        mvprintw(12, 50, "ENTER (on numpad) - Shoot");
        mvprintw(13, 29, "(Press SHIFT + END to close game)");
        mvprintw(15, 15, "Black hole in the centre gets stronger after each round");
        mvprintw(17, 25, "Eliminate other player to win!");
        mvprintw(20, 30, "Press ENTER to begin!");

        //Only escapes splash screen if enter is pressed
        if (wgetch(win) == KEY_ENTER)
        {
            break;
        }

        move(0, 0);
        refresh();
        timeout(1.0f/60.0f);
    }

    //Sets initial game properties, and assigns the ship objects to code
    Ship *ships[2];
    float gravityMultiplier = 1.0f;

    void resetShips()
    {
        //Initialises / resets ships to default values
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
            ships[id]->shipPosition[1] = 12.5f;
            ships[id]->fuel = 10.0f;
            ships[id]->bulletCount = 20;
            ships[id]->destroyed = false;
        }
    }

    resetShips();
    
    while(true)
    {        
        //Refreshes screen
        erase();    

        //Checks input
        switch(wgetch(win))
        {
            //Thrusts ship up or down in direction if the ship has fuel and isn't destroyed
            case KEY_DOWN:
                if (!ships[0]->destroyed && ships[0]->fuel > 0.0f)
                {
                    ships[0]->fuel -= 0.1f;
                    ships[0]->shipMomentum[0] -= 0.00003f * sinf(ships[0]->shipRad);
                    ships[0]->shipMomentum[1] -= 0.00003f * cosf(ships[0]->shipRad);
                }
                break;
            case KEY_UP:
                if (!ships[0]->destroyed && ships[0]->fuel > 0.0f)
                {
                    ships[0]->fuel -= 0.1f;
                    ships[0]->shipMomentum[0] += 0.00003f * sinf(ships[0]->shipRad);
                    ships[0]->shipMomentum[1] += 0.00003f * cosf(ships[0]->shipRad);
                }
                break;
            //Rotates ship's direction for movement/aiming
            case KEY_LEFT:
                if (!ships[0]->destroyed)    ships[0]->shipRotation = (ships[0]->shipRotation - 5) % 360;
                break;
            case KEY_RIGHT:
                if (!ships[0]->destroyed)    ships[0]->shipRotation = (ships[0]->shipRotation + 5) % 360;
                break;
            //Shoots bullet in direction if ammo is available and has recharged
            case KEY_BACKSPACE:
                if (ships[0]->shootTimer <= 0 && ships[0]->bulletCount > 0)
                {
                    ships[0]->bulletCount--;
                    ships[0]->shootTimer = 30000;
                    pos[0] = (ships[0]->shipPosition[0] + (1.75f * sinf(ships[0]->shipRad)));
                    pos[1] = (ships[0]->shipPosition[1] - (1.75f * cosf(ships[0]->shipRad)));
                    createBullet(ships[0]->shipRad, pos);
                }
                break;
            //Does same as above with other ship/different keys
            case KEY_NPAGE:
                if (!ships[1]->destroyed && ships[1]->fuel > 0.0f)
                {
                    ships[1]->fuel -= 0.1f;
                    ships[1]->shipMomentum[0] -= 0.00003f * sinf(ships[1]->shipRad);
                    ships[1]->shipMomentum[1] -= 0.00003f * cosf(ships[1]->shipRad);
                }
                break;
            case KEY_HOME:
                if (!ships[1]->destroyed && ships[1]->fuel > 0.0f)
                {
                    ships[1]->fuel -= 0.1f;
                    ships[1]->shipMomentum[0] += 0.00003f * sinf(ships[1]->shipRad);
                    ships[1]->shipMomentum[1] += 0.00003f * cosf(ships[1]->shipRad);
                }
                break;
            case KEY_END:
                if (!ships[1]->destroyed)    ships[1]->shipRotation = (ships[1]->shipRotation - 5) % 360;
                break;
            case KEY_PPAGE:
                if (!ships[1]->destroyed)    ships[1]->shipRotation = (ships[1]->shipRotation + 5) % 360;
                break;
            case KEY_ENTER:
                if (ships[1]->shootTimer <= 0 && ships[1]->bulletCount > 0)
                {
                    ships[1]->bulletCount--;
                    ships[1]->shootTimer = 30000;
                    pos[0] = ships[1]->shipPosition[0] + (1.75f * sinf(ships[1]->shipRad));
                    pos[1] = ships[1]->shipPosition[1] - (1.75f * cosf(ships[1]->shipRad));
                    createBullet(ships[1]->shipRad, pos);
                }
                break;
            //Exits game
            case KEY_SEND:
                delwin(win);
                exit(0);
                break;
            default:
                break;
        }   
        
        for (int j = 0; j < 2; j++)
        {
            //Allows cooldown for bullets
            ships[j]->shootTimer--;

            //Converts direction in degrees to radians for trig functions above
            ships[j]->shipRad = ships[j]->shipRotation * M_PI / 180.0f;

            //Loops ship around if it reaches screen bounds
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
            //Destroys ship if it collides with black hole
            else if (ships[j]->shipPosition[1] > 11.5f && ships[j]->shipPosition[1] < 13.5f && ships[j]->shipPosition[0] > 38.5f && ships[j]->shipPosition[0] < 41.5f)
            {
                ships[j]->destroyed = true;
            }

            //Black hole gravity
            if (40.0f - ships[j]->shipPosition[0] !=0.0f)
            {
                float xBlackHoleSpeed = 0.0f;
                xBlackHoleSpeed = gravityMultiplier/((40.0f - ships[j]->shipPosition[0]) * 460000000.0f);
                ships[j]->shipMomentum[0] += xBlackHoleSpeed;

                if ((13.0f - ships[j]->shipPosition[1]) != 0.0f && (40.0f - ships[j]->shipPosition[0]) > 0.5f)
                {
                    ships[j]->shipMomentum[1] -= fabs(xBlackHoleSpeed) * gravityMultiplier * (13.0f - ships[j]->shipPosition[1]);
                }
            }

            //Assigns change in position to position variables
            ships[j]->shipPosition[0] += ships[j]->shipMomentum[0];
            ships[j]->shipPosition[1] -= ships[j]->shipMomentum[1];

            for (int k = 0; k < 2; k++)
            {
                if (k != j)
                {
                    //Checks for ship collision
                    if (abs(ships[j]->shipPosition[0] - ships[k]->shipPosition[0]) < 1.0f && abs(ships[j]->shipPosition[1] - ships[k]->shipPosition[1]) < 1.0f)
                    {
                        ships[j]->destroyed = true;
                        ships[k]->destroyed = true;
                    }

                    //Resets game if a ship is destroyed
                    if (ships[j]->destroyed)
                    {
                        score[k]++;
                        resetShips();
                        gravityMultiplier+= 0.05f;
                    }
                }
            }

            //Draws ship if it isn't destroyed
            if (!ships[j]->destroyed)
            {
                mvprintw((int)ships[j]->shipPosition[1], (int)ships[j]->shipPosition[0], "*");
                mvprintw((int)ships[j]->shipPosition[1] - (int)(1.75f * cosf(ships[j]->shipRad)),ships[j]->shipPosition[0] + (int)(1.75f * sinf(ships[j]->shipRad)), "*");
            }
        }

        Bullet * current = bullets;

        for (int l = 0; l < bulletCount; l++)
        {
            //Destroys off-screen bullets, otherwise moves and draws them
            if (current->position[0] > 80.0f || current->position[0] < 0.0f || current->position[1] < 0.0f || current->position[1] > 25.0f)
            {
                deleteBullet(l);
            }
            else
            {
                current->position[0] += 0.001f * sinf(current->direction);
                current->position[1] -= 0.001f * cosf(current->direction);
                mvprintw((int)current->position[1], (int)current->position[0], "+");
            }
            for (int m = 0; m < 2; m++)
            {
                //Destroys ship and bullet if they collide
                if (abs(ships[m]->shipPosition[0] - current->position[0]) < 1.0f && abs(ships[m]->shipPosition[1] - current->position[1]) < 1.0f)
                {
                    deleteBullet(l);
                    ships[m]->destroyed = true;
                }
            }

            //Moves to next item for looping
            if (current->next != NULL)
            {
                current = current->next;
            }
        }

        //Draws random character for black hole
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

        //Displays important variables for player to see
        mvprintw(2, 10, "SCORE: %d", score[0]);
        mvprintw(2, 60, "SCORE: %d", score[1]);
        mvprintw(3, 10, "FUEL: %f", ships[0]->fuel);
        mvprintw(3, 60, "FUEL: %f", ships[1]->fuel);
        mvprintw(4, 10, "BULLETS: %d/20", ships[0]->bulletCount);
        mvprintw(4, 60, "BULLETS: %d/20", ships[1]->bulletCount);

        //Resets cursor to corner of screen to not obscure game view
        move(0, 0);

        //Refreshes screen and waits 1/60 seconds for next loop
        refresh();
        timeout(1.0f/60.0f);
    }
}