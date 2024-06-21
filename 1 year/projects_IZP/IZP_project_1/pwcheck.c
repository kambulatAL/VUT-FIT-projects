/*
**********************************************************
*   IZP projekt 1: práce s textem(Ověřování síly hesel)  *
*   Alakaev Kambulat(xalaka00)                           *
**********************************************************
*/

#include <stdio.h>
#include <stdlib.h>

// maximalní počet symbolů v hesle(včetně '\0', '\n' a 1 symbol pro ověření velkého hesla)
#define MAX_PASS_LENGTH 103
// počet symbolů ascii + '\0'
#define COUNT_OF_ASCII 128 


//dostat délku pole
int get_length(char arr[])
{
    int i = 0;
    while (arr[i] != '\0')
        i++;

    if (arr[i-1] == '\n')
        return i - 1; // delka minus symbol \n

    return i;
}


// první úroveň kontroly hesla
int first_level(char pass[])
{
    int big_letter = 0;
    int small_letter = 0;
    for (int i = 0; pass[i] != '\0'; i++)
    {
        if (pass[i] >= 'A' && pass[i] <= 'Z') // kontrola velkých pismen
            big_letter = 1;

        else if (pass[i] >= 'a' && pass[i] <= 'z') // kontrola malých pismen
            small_letter = 1;

        if (big_letter && small_letter)
            return 1;
    }
    return 0;
}


//druhá úroveň kontroly hesla
int second_level(char pass[], int param)
{
    int counter = 0;
    int has_num = 0;
    int has_smbl = 0;

    if (!first_level(pass)) //kontrola předchozí úrovně
        return 0;
    else
        counter += 2; //prvni uroven splni 2 podminky pro velke a mala pismena 

    if (param < 3)
        return 1;

    else if (param > 2)
    {
        for (int i = 0; pass[i] != '\0'; i++)
        {
            if (pass[i] >= '0' && pass[i] <= '9') //podmínka pro čísla
                has_num = 1;

            else if ((pass[i] >= ' ' && pass[i] <= '/') || //podmínka pro symboly
                     (pass[i] >= ':' && pass[i] <= '@') ||
                     (pass[i] >= '[' && pass[i] <= '`') ||
                     (pass[i] >= '{' && pass[i] <= '~'))
            {
                has_smbl = 1;
            }
            if (has_smbl && has_num)
                break;
        }
    }
    counter += has_num + has_smbl;

    if ((counter >= param) || (param > 4 && counter == 4))
        return 1;

    return 0;
}


//třetí úroveň kontroly hesla
int third_level(char pass[], int param)
{

    if (!second_level(pass, param)) //kontrola předchozí úrovně
        return 0;

    if (param == 1) // každý symbol už je delky 1 
        return 0;
    else if (param > get_length(pass)) //nebude stejnych znaku > delky hesla
        return 1;
        
    int length = get_length(pass) - (param - 1); // aby index nevyšel mimo pole
    int flag = 1;

    for (int i = 0; i < length; i++)
    {
        for (int j = i + param - 1; j != i; j--) // jdeme od i+param-1 (-1 je kvůli indexaci) do i
        {
            if (pass[i] == pass[j])
                continue;
            else
            {
                flag = 0; // pokud se pismena nerovnají - break
                break;
            }
        }
        if (flag) // jestli dosud je flag => byl nalezen podřetězec stejných symbolů délky "param" nebo >"param"
            return 0;
        flag = 1;
    }
    return 1;
}


//funkce pro porovnání podřetězce z 4 úrovně
int compare_strings(char arr1[], char arr2[],int pr){
    int d = 0;
    int res = 1;
            
    while (d < pr)
    {
        if (arr1[d] == arr2[d])
        {
            d++;
            continue;
        }
        else
        {
            res = 0;
            break;
        }
    }
    if (res)
        return 1;
    return 0;
}


//čtvrtá úroveň kontroly hesla
int fourth_level(char pass[], int param)
{
    if (!third_level(pass, param)) //kontrola předchozí úrovně
        return 0;

    if (param >= get_length(pass)) // heslo nebude obsahovat 2 stejne podretezce >= delce hesla 
        return 1;

    int index_param = param - 1;
    int length = get_length(pass);
    char arr1[param];
    char arr2[param];
    int flag = 1;

    for (int i = 0; i < length - index_param - 1; i++) // "length - index_param -1" aby index nevyšel mimo pole
    {
        //bereme první podřetězec
        int k = 0;
        for (int j = i; j < i + param; j++)
        {
            arr1[k] = pass[j];
            k++;
        }
        arr1[param] = '\0';
        
        // bereme řetězec o 1 symbol napravo od i
        for (int s = i + 1; s < length - index_param; s++)
        {
            int z = 0;
            //bereme druhy podřetězec
            for (int j = s; j < s + param; j++)
            {
                arr2[z] = pass[j];
                z++;
            }
            arr2[param] = '\0';

            flag = compare_strings(arr1,arr2,param);
            if (flag)
                return 0;
            flag = 1;
        }
    }
    return 1;
}


// výběr úrovní podle argumentu "level"
int select_level(char pass[], int lvl, int param)
{
    int result = 0;

    if (lvl == 1)
        result = first_level(pass);
    else if (lvl == 2)
        result = second_level(pass, param);
    else if (lvl == 3)
        result = third_level(pass, param);
    else if (lvl == 4)
        result = fourth_level(pass, param);

    return result;
}


// zpracování chyb při vstupu hesla
int check_pass(char pass[])
{
    if (get_length(pass) > 100)
    {
        printf("ERROR: Too long password was entered\n"); //jestli máme >100 symbolů a pass[101]!=('\0' nebo '\n')
        return 0;
    }
    if (pass[0] == '\n')
    {
        printf("ERROR: Invalid password was entered (has a new line symbol only)\n"); //nulový řetezec
        return 0;
    }
    return 1;
}


// zpracování chyb při vstupu argumentů
int check_params(int lvl, int param)
{
    if ((lvl == 0) || (param == 0))
    {
        printf("ERROR: Invalid arguments(level,param)\n");
        return 0;
    }
    else if (lvl > 4)
    {
        printf("ERROR: Too big argument \"level\"(only numbers from 1 to 4)\n");
        return 0;
    }
    return 1;
}


// funkce pro ověření počtu parametrů
int check_argc(int arg){
    if (arg < 3){
        printf("ERROR: Enter all required arguments(level, param)\n");
        return 0;
    }
    else if (arg > 4){
        printf("ERROR: Too many arguments were entered\n");
        return 0;
    }
    return 1;
}

// funkce pro ověření,že bylo zadáno "--stats", ale ne "--statsdfdf" atd.
int is_stats(char stcs[])
{
    char *ar = "--stats";

    if (get_length(stcs) == get_length(ar))
    {
        int flag = 1;

        for (int i = 0; ar[i] != '\0'; i++)
        {
            if (ar[i] == stcs[i])
                continue;
            else{
                flag = 0;
                break;
            }
        }
        if (flag)
            return 1;
    }
    return 0;
}


//funkce pro získání statistiky
void get_statistics(int *min_len, int *s_len, int *counter, char smbls[], char pass[])
{   
    for(int i = 0; pass[i]!='\0'; i++){
        // namísto indexu bude čislová hodnota ascii symbolu a pišeme do pole s takovým indexem 1
        smbls[(int)pass[i]] = 1;    
    }

    int len = get_length(pass);
    
    if (len < *min_len)
        *min_len = len;

    *s_len += len;
    *counter+=1;
}


// funce pro získání počtu různých symbolů v pole "symbols"
void count_dif_smbls(char smbls[],int *count)
{
    for(int i=0; i<COUNT_OF_ASCII;i++){
        if (smbls[i] == 1)
            *count += 1;
    }
    *count -= 1; // minus '\n' symbol
}


void show_statistics(int arg, char arr[], char smbls[], int s_len, int pas_counter, int min_len ,int *smbls_count)
{
    if (arg > 3)
    {   //kontrola, že bylo uvedeno "--stats"
        if (is_stats(arr)){ 
            // určit počet různých symbolů
            count_dif_smbls(smbls, smbls_count);
            
            printf("Statistics:\n");
            printf("Different symbols: %d\n", *smbls_count); 
            printf("Minimal length: %d\n", min_len);
            printf("Average length: %.1f\n", (double)s_len / pas_counter);
        }
        else
            printf("WARNING: Cant display statistics. Wrong third parameter (should be \"--stats\")\n");
    }
}


//hlavní funkce
int main(int argc, char *argv[])
{   
    if (!check_argc(argc))
        return 0;

    int level = atoi(argv[1]);
    int param = atoi(argv[2]);

    if (!check_params(level, param))
        return 0;
    
    char pass[MAX_PASS_LENGTH];         // pole pro heslo
    char symbols[COUNT_OF_ASCII] = {0}; // pole pro symboly ze všech hesel
    int dif_smbls = 0;                  // proměnna pro počet různých symbolů
    int min_pass_len = 150;             // proměnna pro minimalní délky hesla
    int sum_len = 0;                    // proměnna pro akumulaci délek hesel
    int pas_counter = 0;                //  proměnna pro počet hesel

    //kontrola vstupu(1 symbol pro "\n", 1 symbol pro "\0" a 1 pro ověření příliš velkých hesel)
    while (fgets(pass, MAX_PASS_LENGTH, stdin) != NULL)
    {   
        if (!check_pass(pass))
            return 0;

        // akumulujeme hodnoty a dodáváme nové symboly do pole "symbols"
        get_statistics(&min_pass_len, &sum_len, &pas_counter, symbols, pass); 
        
        // podmínka pro hesla podle parametrů
        if (select_level(pass, level, param))
            printf("%s", pass);
    }
    printf("\n");
    show_statistics(argc,argv[3],symbols,sum_len,pas_counter,min_pass_len,&dif_smbls);

return 0;
}
