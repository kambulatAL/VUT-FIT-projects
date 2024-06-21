/*
**********************************************************
*   IOS projekt 2                                        *
*   Kambulat Alakaev(xalaka00)                           *
**********************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>


FILE *f = NULL;

// shared memory
int *main_counter = 0;
int *oxyg_counter = 0;
int *hydro_counter = 0;
int *molecule_counter = 0;
int *ox_queue = 0;
int *hyd_queue = 0;
int *atoms_counter = 0;

// semaphores
sem_t *main_sem = NULL;
sem_t *oxyg_sem = NULL;
sem_t *hydro_sem = NULL;
sem_t *print_sem = NULL;
sem_t *molecule_sem = NULL;
sem_t *turnstile = NULL;
sem_t *turnstile2 = NULL;
sem_t *creating_sem = NULL;


// open file, creating shared memory and semaphores
int init()
{
    f = fopen("proj2.out", "w");
    if (f == NULL)
        return 0;

    main_counter = mmap(NULL, sizeof(int),
                        PROT_READ | PROT_WRITE,
                        MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (main_counter == MAP_FAILED)
        return 0;
    *main_counter = 0;

    oxyg_counter = mmap(NULL, sizeof(int),
                        PROT_READ | PROT_WRITE,
                        MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (oxyg_counter == MAP_FAILED)
        return 0;
    *oxyg_counter = 0;

    hydro_counter = mmap(NULL, sizeof(int),
                         PROT_READ | PROT_WRITE,
                         MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (hydro_counter == MAP_FAILED)
        return 0;
    *hydro_counter = 0;

    molecule_counter = mmap(NULL, sizeof(int),
                            PROT_READ | PROT_WRITE,
                            MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (molecule_counter == MAP_FAILED)
        return 0;
    *molecule_counter = 0;

    ox_queue = mmap(NULL, sizeof(int),
                    PROT_READ | PROT_WRITE,
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (ox_queue == MAP_FAILED)
        return 0;
    *ox_queue = 0;

    hyd_queue = mmap(NULL, sizeof(int),
                     PROT_READ | PROT_WRITE,
                     MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (hyd_queue == MAP_FAILED)
        return 0;
    *hyd_queue = 0;

    atoms_counter = mmap(NULL, sizeof(int),
                         PROT_READ | PROT_WRITE,
                         MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (atoms_counter == MAP_FAILED)
        return 0;
    *atoms_counter = 0;

    main_sem = sem_open("/xalaka00_main_semaphore", O_CREAT | O_EXCL, 0660, 1);
    if (main_sem == SEM_FAILED)
        return 0;

    oxyg_sem = sem_open("/xalaka00_oxyg_semaphore", O_CREAT | O_EXCL, 0660, 0);
    if (oxyg_sem == SEM_FAILED)
        return 0;

    hydro_sem = sem_open("/xalaka00_hydro_semaphore", O_CREAT | O_EXCL, 0660, 0);
    if (hydro_sem == SEM_FAILED)
        return 0;

    print_sem = sem_open("/xalaka00_print_semaphore", O_CREAT | O_EXCL, 0660, 1);
    if (print_sem == SEM_FAILED)
        return 0;

    molecule_sem = sem_open("/xalaka00_molecule_semaphore", O_CREAT | O_EXCL, 0660, 1);
    if (molecule_sem == SEM_FAILED)
        return 0;

    turnstile = sem_open("/xalaka00_turn1_semaphore", O_CREAT | O_EXCL, 0660, 0);
    if (turnstile == SEM_FAILED)
        return 0;

    turnstile2 = sem_open("/xalaka00_turn2_semaphore", O_CREAT | O_EXCL, 0660, 1);
    if (turnstile2 == SEM_FAILED)
        return 0;
    creating_sem = sem_open("/xalaka00_creating_semaphore", O_CREAT | O_EXCL, 0660, 0);
    if (creating_sem == SEM_FAILED)
        return 0;

    return 1;
}

// free all shared memory and close/unlink all semaphores 
void clean()
{
    munmap(main_counter, sizeof(int));
    munmap(oxyg_counter, sizeof(int));
    munmap(hydro_counter, sizeof(int));
    munmap(ox_queue, sizeof(int));
    munmap(hyd_queue, sizeof(int));
    munmap(molecule_counter, sizeof(int));

    sem_close(main_sem);
    sem_unlink("/xalaka00_main_semaphore");
    sem_close(oxyg_sem);
    sem_unlink("/xalaka00_oxyg_semaphore");
    sem_close(hydro_sem);
    sem_unlink("/xalaka00_hydro_semaphore");
    sem_close(print_sem);
    sem_unlink("/xalaka00_print_semaphore");
    sem_close(molecule_sem);
    sem_unlink("/xalaka00_molecule_semaphore");
    sem_close(turnstile);
    sem_unlink("/xalaka00_turn1_semaphore");
    sem_close(turnstile2);
    sem_unlink("/xalaka00_turn2_semaphore");
    sem_close(creating_sem);
    sem_unlink("/xalaka00_creating_semaphore");

    fclose(f);
}

// check the number of arguments
int check_argc(int c_args)
{
    if (c_args != 5)
    {
        fprintf(stderr, "Error: Wrong number of agruments!\n");
        return 0;
    }
    return 1;
}

// check if parameters are valid
int check_params(int *arr, int argc, char *args[])
{
    char *p;

    for (int i = 1; i < argc; i++)
    {
        strtol(args[i], &p, 10);

        if ((p == args[i]) || (*p != '\0'))
        {
            fprintf(stderr, "Error: Wrong type of argument!\n");
            return 0;
        }
        else
        {
            arr[i - 1] = atoi(args[i]);
        }
    }
    if ((arr[2] > 1000 || arr[2] < 0) || (arr[3] > 1000 || arr[3] < 0))
    {
        fprintf(stderr, "Error: wrong range of one or two arguments!\n");
        return 0;
    }
    if ((arr[0] < 1) || (arr[1] < 1))
    {
        fprintf(stderr, "Error: wrong range of one or two arguments!\n");
        return 0;
    }
    return 1;
}

// check if process is created
void check_process(pid_t fd)
{
    if (fd == -1)
    {
        fprintf(stderr, "Error: rocess creation failed\n");
        clean();
        exit(1);
    }
}

// function for waiting
void stop(int delay)
{   
    if (delay != 0)
    {
        srand(getpid() * time(0));
        int s = rand() % delay + 1;
        usleep(s * 1000);
    }
    else
    {
        usleep(0);
    }
}

// print oxygen started and increase rows counter
void print_ox_started()
{
    sem_wait(print_sem);
    *main_counter += 1;
    *oxyg_counter += 1;
    fprintf(f, "%d: O %d: started\n", *main_counter, *oxyg_counter);
    sem_post(print_sem);
}

// print oxygen going to queue and increase rows counter
void print_ox_going_queue(int s)
{
    sem_wait(print_sem);
    *main_counter += 1;
    fprintf(f, "%d: O %d: going to queue\n", *main_counter, s);
    sem_post(print_sem);
}

// print oxygen creating and increase rows counter
void print_creating_o_mol(int s)
{
    sem_wait(print_sem);
    *main_counter += 1;
    fprintf(f, "%d: O %d: creating molecule %d\n", *main_counter, s, *molecule_counter + 1);
    sem_post(print_sem);
}

// print oxygen created and increase rows counter
void print_mol_o_created(int s)
{
    sem_wait(print_sem);
    *main_counter += 1;
    fprintf(f, "%d: O %d: molecule %d created\n", *main_counter, s, *molecule_counter + 1);
    sem_post(print_sem);
}

// print oxygen not enough and increase rows counter
void print_o_not_enough(int s)
{
    sem_wait(print_sem);
    *main_counter += 1;
    fprintf(f, "%d: O %d: not enough H\n", *main_counter, s);
    sem_post(print_sem);
}

// print hydrogen started and increase rows counter
void print_hyd_started()
{
    sem_wait(print_sem);
    *main_counter += 1;
    *hydro_counter += 1;
    fprintf(f, "%d: H %d: started\n", *main_counter, *hydro_counter);
    sem_post(print_sem);
}

// print hydrogen going to queue and increase rows counter
void print_hyd_going_queue(int s)
{
    sem_wait(print_sem);
    *main_counter += 1;
    fprintf(f, "%d: H %d: going to queue\n", *main_counter, s);
    sem_post(print_sem);
}

// print hydrogen creating and increase rows counter
void print_creating_h_mol(int s)
{
    sem_wait(print_sem);
    *main_counter += 1;
    fprintf(f, "%d: H %d: creating molecule %d\n", *main_counter, s, *molecule_counter + 1);
    sem_post(print_sem);
}

// print hydrogen created and increase rows counter
void print_mol_h_created(int s)
{
    sem_wait(print_sem);
    *main_counter += 1;
    fprintf(f, "%d: H %d: molecule %d created\n", *main_counter, s, *molecule_counter + 1);
    sem_post(print_sem);
}

// print hydrogen not enough and increase rows counter
void print_h_not_enough(int s)
{
    sem_wait(print_sem);
    *main_counter += 1;
    fprintf(f, "%d: H %d: not enough O or H\n", *main_counter, s);
    sem_post(print_sem);
}

// reusable barrier for 3 atoms
void barrier(int choice, int s, int mol_time, int num_of_max_mols, int oxyg_count, int hydro_count)
{
    sem_wait(molecule_sem);
    *atoms_counter += 1;

    if (*atoms_counter == 3)
    {
        sem_wait(turnstile2);
        sem_post(turnstile);
    }
    sem_post(molecule_sem);
    sem_wait(turnstile);
    sem_post(turnstile);

    //##### Critical section ######
    
    // if oxygen has entered
    if (choice == 1)
    {
        print_creating_o_mol(s);
        stop(mol_time);
        print_mol_o_created(s);
        // increase sem_counter of creating sem for 2 atoms of hydrogen
        sem_post(creating_sem);
        sem_post(creating_sem);
    }
    // if hydrogen has entered
    else
    {
        print_creating_h_mol(s);
        // wait untill oxygen will open a semaphore
        sem_wait(creating_sem);
        print_mol_h_created(s);
    }
    //##### Critical section ######

    sem_wait(molecule_sem);
    // decrease num of atoms for each atom that has entered
    *atoms_counter -= 1;

    // increase the number of molecules if all atoms have entered
    if (*atoms_counter == 0)
    {
        *molecule_counter += 1;
    }
    // if we have already all atoms created than free all semaphores of the remaining atoms
    if (*molecule_counter == num_of_max_mols)
    {   
        int ox_on_del = oxyg_count - num_of_max_mols;
        int hyd_on_del = hydro_count -num_of_max_mols*2;

        while (ox_on_del > 0)
        {
            ox_on_del -= 1;
            sem_post(oxyg_sem);
        }
        while (hyd_on_del > 0)
        {
            hyd_on_del -= 1;
            sem_post(hydro_sem);
        }
    }

    if (*atoms_counter == 0)
    {
        sem_wait(turnstile);
        sem_post(turnstile2);
    }
    sem_post(molecule_sem);
    sem_wait(turnstile2);
    sem_post(turnstile2);
}

// function for oxygen creation 
void create_oxygen(int delay, int mol_time, int num_of_max_mols, int oxyg_count, int hydro_count)
{
    print_ox_started();
    // save value of atom for others print commands
    int save_o = *oxyg_counter;

    stop(delay);

    // print_ox_going_queue(save_o);

    sem_wait(main_sem);
    print_ox_going_queue(save_o);
    *ox_queue += 1;
    // if we have at least 3 atoms (1 ox and 2 hyd) than free 2 hydrogen semaphores and one oxygen semaphore
    if (*hyd_queue >= 2)
    {
        sem_post(hydro_sem);
        sem_post(hydro_sem);
        *hyd_queue -= 2;
        sem_post(oxyg_sem);
        *ox_queue -= 1;
    }
    else
    {
        sem_post(main_sem);
    }

    // if we cant create even 1 molecule than just print not enough and leave(exit)
    if (num_of_max_mols == 0)
    {
        print_o_not_enough(save_o);
        exit(0);
    }
    sem_wait(oxyg_sem);

    // if we have already max num of the molecule than print not enough for remaining atoms and leave 
    if (*molecule_counter == num_of_max_mols)
    {
        print_o_not_enough(save_o);
        exit(0);
    }

    // enter to the barrier 
    barrier(1, save_o, mol_time, num_of_max_mols, oxyg_count, hydro_count);

    sem_post(main_sem);

}

// function for hydrogen creation 
void create_hydrogen(int delay, int mol_time, int num_of_max_mols, int oxyg_count, int hydro_count)
{
    print_hyd_started();
    // save value of atom for others print commands
    int save_h = *hydro_counter;

    stop(delay);

    // print_hyd_going_queue(save_h);

    sem_wait(main_sem);
    print_hyd_going_queue(save_h);

    *hyd_queue += 1;
    // free 2 hydrogen and one oxygen semaphores if we have at least 3 atoms(2 hydrogens and 1 oxygen)
    if ((*hyd_queue >= 2) && (*ox_queue >= 1))
    {
        sem_post(hydro_sem);
        sem_post(hydro_sem);
        *hyd_queue -= 2;
        sem_post(oxyg_sem);
        *ox_queue -= 1;
    }
    else
    {
        sem_post(main_sem);
    }

    // if we cant create even 1 molecule than just print not enough and leave(exit)
    if (num_of_max_mols == 0)
    {
        print_h_not_enough(save_h);
        exit(0);
    }
    sem_wait(hydro_sem);

    // if we have already max num of the molecule than print not enough for remaining atoms and leave
    if (*molecule_counter == num_of_max_mols)
    {
        print_h_not_enough(save_h);
        exit(0);
    }
    // enter to the barrier
    barrier(0, save_h, mol_time, num_of_max_mols, oxyg_count, hydro_count);
}


//############  MAIN FUNCTION   ############
int main(int argc, char *argv[])
{
    // check the number of arguments
    if (!check_argc(argc))
        exit(1);

    // create array for program arguments
    int params[argc - 1];

    // check if arguments satisfy the requirements
    if (!check_params(params, argc, argv))
        exit(1);

    // save program arguments
    int oxyg_count = params[0];
    int hydro_count = params[1];
    int queue_time = params[2];
    int mol_time = params[3];
    int num_of_max_mols = 0;

    // save the count of molecules that can be created
    if ((hydro_count / 2) < oxyg_count)
    {
        num_of_max_mols = hydro_count / 2;
    }
    else
    {
        num_of_max_mols = oxyg_count;
    }

    // try to initialize all shared memory and semaphores
    if (!init())
    {
        fprintf(stderr, "Error: Initialization has failed!\n");
        clean();
        exit(1);
    }

    // enable string buffer type
    setlinebuf(f);

    // create oxygen processes 
    for (int i = 0; i < oxyg_count; i++)
    {
        pid_t o_pid = fork();
        check_process(o_pid);
        if (o_pid == 0)
        {
            create_oxygen(queue_time, mol_time, num_of_max_mols, oxyg_count, hydro_count);
            exit(0);
        }
    }

    // create hydrogen processes 
    for (int j = 0; j < hydro_count; j++)
    {
        pid_t h_pid = fork();
        check_process(h_pid);
        if (h_pid == 0)
        {
            create_hydrogen(queue_time, mol_time, num_of_max_mols, oxyg_count, hydro_count);
            exit(0);
        }
    }

    // wait till all processes will die
    while (wait(NULL) > 0);

    // free all shared memory and close/unlink all semaphores
    clean();
    return 0;
}
