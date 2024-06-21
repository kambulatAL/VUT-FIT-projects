/*
****************************************
*   Project IMS. SHO v osobní přepravě *
*   Author: Kambulat Alakaev(xalaka00) *
****************************************
*/
#include "simlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

int COUNT_OF_TOURS = 3;
int COUNT_OF_PASSANGERS = 4900;
int COUNT_OF_SHIPS = 1;

Store Ship("Lod", COUNT_OF_SHIPS);
Store Tour("Tour", COUNT_OF_TOURS);

Queue Q_ENROLLED_CLIENTS;
Queue Q_FLAG_YEAR_END;
Queue Q_CRUISE_GROUP;
Queue Q_EXIT1;
Queue Q_EXIT2;
Queue Q_SHIP_DOCK;
int flag = false;

int num_of_people = 0;
int num_of_clients = 0;
int ended_cruises = 0;

class Cruise : public Process
{
    void Behavior()
    {
        Into(Q_CRUISE_GROUP);
        while (Ship.Free() == 0);
        Enter(Ship, 1);

        Wait(1440 * 7);
        Leave(Ship, 1);
        ended_cruises++;
        (Q_CRUISE_GROUP.GetFirst())->Cancel();
    }
};

class Year : public Process
{
    void Behavior()
    {
        Wait(365 * 1440);
        Into(Q_FLAG_YEAR_END);
        Passivate();
    }
};

class ShipDock : public Process
{
    void Behavior()
    {
        // ship is in the dock from 14 to 60 days with exponential distribution
        Into(Q_SHIP_DOCK);
        Enter(Ship, 1);
        Wait(Uniform(1440 * 14, 1440 * 60));
        // if ship is in the dock, then it is available for new passangers
        Leave(Ship, 1);
        (Q_SHIP_DOCK.GetFirst())->Cancel();
    }
};

class Client : public Process
{
    void Behavior()
    {
        // if year is ended
        if (Q_FLAG_YEAR_END.Length() > 0)
        {
            (Q_FLAG_YEAR_END.GetFirst())->Cancel();

            Enter(Tour, Tour.Free());
            Leave(Tour, 3);

            (new Year)->Activate();

            (new ShipDock)->Activate();
        }
        if (Random() < 0.20)
        {
            // 20% of clients want to go on our tours
            // If there are no available tours, the client leaves(goes to another company)
            if (Tour.Full())
            {
                Into(Q_EXIT2);
                Passivate();
            }
            else
            {
                if (Tour.Full())
                {
                    Into(Q_EXIT2);
                    Passivate();
                }
                else
                {
                    Enter(Tour, 1);
                    Into(Q_ENROLLED_CLIENTS);
                    Leave(Tour, 1);
                    // if capacity of the ship is full for some tour and ship is in the dock, then cruise starts
                    if (Q_ENROLLED_CLIENTS.Length() == 4900)
                    {
                        Enter(Tour, 1);
                        num_of_clients++;

                        (new Cruise)->Activate();

                        for (int i = 0; i < 4900; i++)
                        {
                            (Q_ENROLLED_CLIENTS.GetFirst())->Cancel();
                        }
                    }

                    num_of_clients++;
                    Passivate();
                }
            }
        }
        else
        {
            // 80% of clients go to another company
            Into(Q_EXIT1);
            Passivate();
        }
    }
};

class Generator : public Event
{
    void Behavior()
    {
        (new Client)->Activate();
        num_of_people++;
        Activate(Time + Exponential(3));
    }
};

void print_statistics()
{
    printf("Number of people: %d\n", num_of_people);
    printf("Number of clients: %d\n", num_of_clients);
    printf("Number of ended cruises: %d\n", ended_cruises);
    Tour.Output();
    Ship.Output();
    Q_EXIT1.Output();
    Q_EXIT2.Output();
    Q_FLAG_YEAR_END.Output();
    Q_ENROLLED_CLIENTS.Output();
    Q_CRUISE_GROUP.Output();
    Q_SHIP_DOCK.Output();
}

/*
 *  Function for reading arguments
 * param argc - number of arguments
 * param argv - array of arguments
 * param year - pointer to a current number of years
 */
void read_args(int argc, char *argv[], int *year)
{
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-y") == 0)
        {
            *year = atoi(argv[i + 1]);
        }
    }
}


/*
 * Function for clearing queues
 */
void clear_queues()
{
    for(u_int i = 0; i < Q_ENROLLED_CLIENTS.Length(); i++)
    {
        (Q_ENROLLED_CLIENTS.GetFirst())->Cancel();
    }
    for(u_int i = 0; i< Q_EXIT1.Length(); i++)
    {
        (Q_EXIT1.GetFirst())->Cancel();
    }
    for(u_int i=0; i< Q_EXIT2.Length(); i++)
    {
        (Q_EXIT2.GetFirst())->Cancel();
    }
}

/*
 * Main function
 */
int main(int argc, char *argv[])
{
    int year = 1;

    read_args(argc, argv, &year);

    Init(0, 1440 * 365 * year);
    (new Year)->Activate();
    (new Generator)->Activate();

    Run();
    print_statistics();
    clear_queues();

    return 0;
}

