#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <ctype.h>

#define MAX_ROW_LEN 30000   // max délka řetezce v souboru
#define MAX_ROWS_COUNT 1000 // max počet řetězců
#define MAX_ELEMENT_LEN 30  // max délka prvku množiny

//struktura pro množinu
typedef struct _set
{
  int id;          // identifikator množiny v operacích
  int size;        // velikost množiny
  char **elements; // ukazátel na pole ukazátelů na prvky množiny
} set_t;

//struktura pro relaci
typedef struct Rel
{
  int id;
  int size;
  set_t **sets;

} rel;

// určit počet prvků množiny
int count_words(char str[])
{
  int num_of_words = 0;
  for (int i = 0; str[i] != '\0'; i++) // cyklus po řetezce souboru
  {
    if (str[i] == ' ') // pokud je mezera tak zvětšíme počet prvků o jeden
      num_of_words++;
  }
  if (num_of_words > 0)
  {
    return num_of_words + 1; // +1 pro první prvek řetězce, který nemá mezery vlevo
  }
  num_of_words = 1; // pokud řetězec obsahuje jenom identifikující prvek jako "S"
  return num_of_words;
}

//kontrola počtu paramentrů programu
int check_args_count(int arg)
{
  if (arg != 2)
  {
    printf("ERROR: Wrong number of arguments\n");
    return 0;
  }
  return 1;
}

//kontrola uspěšnosti otevření souboru
int check_file_opening(FILE *f, char *arr[])
{
  if (f == NULL)
  {
    printf("ERROR: Can't read the file \"%s\"\n", arr[1]);
    return 0;
  }
  return 1;
}

// kontrola počtu řetězců
int check_rows_count(int r_counter)
{
  if (r_counter > MAX_ROWS_COUNT - 1) // -1 protože začínáme s rows_counter =0 (viz main)
  {
    printf("ERROR: File contains too many rows");
    return 0;
  }
  return 1;
}

//kontrola délky prvku množiny
int check_element_len(char *str)
{
  if (strlen(str) > MAX_ELEMENT_LEN)
  {
    printf("ERROR: Too long string is in the file\n");
    return 0;
  }
  return 1;
}

//kontrola "špatných" řetězců
int check_row_errors(char str[], int r_count)
{
  if ((str[0] == '\n') || (str[0] == ' '))
  {
    printf("ERROR: Wrong format of row is in the file\n");
    return 0;
  }
  else if ((r_count == 0) && (str[0] != 'U')) // kontrola, že první řetězec souboru je hlavní množina
  {
    printf("ERROR: There is no main set in the file\n");
    return 0;
  }
  return 1;
}

// kontrola prvku, že není jeden z zakázaných slov
int check_element_value(char *str)
{
  int num_of_baned_words = 21; // počet zakázaných slov
  // ty zakázané slova
  char *wrong_elements[] = {
      "empty",
      "card",
      "complement",
      "union",
      "intersect",
      "minus",
      "subseteq",
      "subset",
      "equals",
      "true",
      "false",
      "reflexive",
      "symmetric",
      "antisymmetric",
      "transitive",
      "function",
      "domain",
      "codomain",
      "injective",
      "surjective",
      "bijective"};

  for (int i = 0; i < num_of_baned_words; i++)
  {
    if (strcmp(str, wrong_elements[i]) == 0) // kontrola zda je prvek roven jednemu z těchto slov
    {
      printf("ERROR: row of set contains element \"%s\"\n", str);
      return 0;
    }
  }
  return 1;
}

// kontrola možných chyb budoucích prvků množiny
int check_row_of_set(char str[])
{
  char r[MAX_ROW_LEN];         // pole typu char s délkou = delce řetezce souboru
  memcpy(r, str, strlen(str)); // kopirování řetezce souboru, aby se nesmazal funkcí "strtok"

  char *pch = strtok(r, " "); // bereme prvek do mezery

  while (pch != NULL)
  {
    if (!check_element_len(pch))
    {
      return 0;
    }
    else if (!check_element_value(pch))
    {
      return 0;
    }
    pch = strtok(NULL, " ");
  }
  return 1;
}

//kontrola možnosti allokace paměti
int check_ptr_malloc(char *p)
{
  if (p == NULL)
  {
    printf("ERROR: Memory allocation fault\n");
    return 0;
  }
  return 1;
}

// alokace paměti pro prvky množiny
int malloc_set_elements(set_t *s)
{
  for (int i = 0; i < s->size; i++)
  {
    s->elements[i] = malloc(MAX_ELEMENT_LEN); // vždy allokujeme 30 bytů pro jeden prvek

    if (!check_ptr_malloc(s->elements[i])) //kontrola allokace
    {
      return 0;
    }
  }
  return 1;
}

//uvolnit allokovanou paměť množiny
void free_memory_of_set(set_t *s)
{
  for (int i = 0; i < s->size; i++)
  {
    free(s->elements[i]);
  }
  free(s->elements);
}

//uvolnit allokovanou paměť všech množin
void free_memory_of_sets(set_t arr[], int num_of_sets)
{
  for (int i = 0; i < num_of_sets + 1; i++) //+1 protože na začátku num_of_sets=0(viz main)
  {
    free_memory_of_set(&arr[i]);
  }
}

// uvolnit allokovanou paměť množin a zavřit soubor
void free_sets_and_close_f(FILE *f, set_t arr[], int num_of_sets)
{
  if (num_of_sets > 0) // // pokud máme allokovánou množinu, tak uvolníme a zavřeme soubor
  {
    free_memory_of_sets(arr, num_of_sets);
    fclose(f);
  }
  else
  {
    fclose(f);
  }
}

// inicializace jedne množizy
int initialize_set(char str[], set_t arr[], int num_of_sets, int set_rel_counter)
{
  arr[num_of_sets].size = count_words(str);        // do atributu "size" jedné množiny z pole typu "set" pišeme počet slov
  arr[num_of_sets].id =  set_rel_counter + 1;      // +1 protože v funkce "main" num_of_sets na začátku má hodnotu 0(viz main)
  arr[num_of_sets].elements = malloc(arr[num_of_sets].size * sizeof(char *)); //allokace místa v množině pro prvky podle počtů slov

  if (arr[num_of_sets].elements == NULL) // kontrola allokace
  {
    printf("ERROR: Failed to allocate memory for a set");
    return 0;
  }
  return 1;
}

// kontrola inicializace spolu s kontrolou allokace a kontrolou prvků
int check_set_errors(char str[], set_t arr[], int num_of_sets, int set_rel_counter, FILE *f)
{
  if (!check_row_of_set(str))
  {
    free_sets_and_close_f(f, arr, num_of_sets);
    return 0;
  }

  if (!initialize_set(str, arr, num_of_sets,set_rel_counter))
  {
    free_sets_and_close_f(f, arr, num_of_sets);
    return 0;
  }

  if (!malloc_set_elements(&arr[num_of_sets]))
  {
    free_sets_and_close_f(f, arr, num_of_sets);
    return 0;
  }
  return 1;
}

// uložení prvků řetezce souboru do množiny
void save_set(char str[], set_t arr[], int num_of_sets)
{
  char *pch = strtok(str, " ");

  for (int i = 0; i < arr[num_of_sets].size; i++)
  {
    strcpy(arr[num_of_sets].elements[i], pch);
    pch = strtok(NULL, " ");
  }
}

// kontrola duplikátů ve množině
int check_set_duplication(set_t *s, set_t arr[], int num_of_sets, FILE *f)
{
  int num_of_duplicates = 0;
  for (int i = 1; i < s->size; i++) // začínáme od 2 prvku pro vyloučení elementů jako "U","S"
  {
    for (int j = 1; j < s->size; j++)
    {
      if ((strcmp(s->elements[i], s->elements[j]) == 0)) // jestli se 2 prvky množine rovnají
      {
        num_of_duplicates++;
      }
    }
    if (num_of_duplicates > 1) // jestli ve množině více než 1 stejných prvků
    {
      free_sets_and_close_f(f, arr, num_of_sets);
      printf("ERROR: Set contains duplicates\n");
      return 0;
    }
    else
    {
      num_of_duplicates = 0;
    }
  }
  return 1;
}

//kontrola, že  množina je podmnožina jiné
int check_on_subset(set_t *s, set_t arr[], int num_of_sets, FILE *f, int key)
{
  set_t main_set = arr[key]; // bereme množinu podle klíče
  int elements_counter = 0;  // počet stejných prvků v obou množinách

  if ((strcmp(s->elements[0], "S") == 0) && (s->size == 1)) //pokud s je prázdná množina
  {
    return 1;
  }

  for (int i = 1; i < main_set.size; i++) // začínáme od 2 prvku pro vyloučení elementů jako "U","S"
  {
    for (int j = 1; j < s->size; j++)
    {
      if (strcmp(main_set.elements[i], s->elements[j]) == 0) // jestli se prvky rovnají
      {
        elements_counter++;
      }
    }
  }

  if (elements_counter == (s->size - 1)) // -1 pro vyloučení elementu jako "U","S"
  {
    return 1;
  }

  if (main_set.id == 1) // pokud  množina není podmnožinou "Univerzum"
  {
    free_sets_and_close_f(f, arr, num_of_sets);
    printf("ERROR: Set isnt a subset of the U-set\n");
  }
  return 0;
}

// kontrola, že množina má správný format
int check_set_format(set_t *s, set_t arr[], int num_of_sets, FILE *f)
{
  if (!check_set_duplication(s, arr, num_of_sets, f))
  {
    return 0;
  }
  if (!check_on_subset(s, arr, num_of_sets, f, 0)) // 0 - id prvni množiny
  {
    return 0;
  }
  return 1;
}

// vývod prvků množiny
void print_set(set_t *s)
{
  for (int i = 0; i < s->size; i++)
  {
    if ((s->size - i) == 1) // pokud je to poslední prvek množiny tak výstup už bez mezery
    {
      printf("%s", s->elements[i]);
      break;
    }
    else
    {
      printf("%s ", s->elements[i]);
    }
  }
}

// hledání "id" jedné množiny podle uvedeného čísla --- v řetězci "C" ---
int search_set(set_t arr[], int num_of_sets, char key_id)
{
  int int_key_id = atoi(&key_id); //strtol(key_id,NULL,10); // převod čísla z char do typu int

  for (int i = 0; i < num_of_sets; i++)
  {
    if (arr[i].id == int_key_id) // jestli máme
    {
      return i; // vrácení indexu množiny
    }
  }
  return -1;
}

//hledání případného id mmnožiny v řetězci a ověření že existuje množina s takovým id
void search_id_from_str(char *str, set_t arr[], int num_of_sets, int id_position, int *set_id)
{
  int counter = 1;             //1 pro prvek "C"
  char r[MAX_ROW_LEN];         // pole typu char s délkou = delce řetezce souboru
  memcpy(r, str, strlen(str)); // kopirování řetezce souboru, aby se nesmazal funkcí "strtok"
  char *pch = strtok(r, " ");

  while (pch != NULL)
  {
    if (isdigit(*pch) && counter == id_position) // kontrola čísla na  pozici "id_position" řetězcu souboru
    {
      char ch = *pch; // bereme číslo jako char, ale ne ukazátel
      *set_id = search_set(arr, num_of_sets, ch);
      return;
    }
    pch = strtok(NULL, " ");
    counter++;
  }
}

// kontrola existence id množiny
int check_id_of_sets(int set1_id, int set2_id)
{
  if ((set1_id == -1) || (set2_id == -1)) // jestli nebyla nalezena aspoň 1 z množin
  {
    printf("WARNING: Sets with one or both identifiers doesnt exist\n");
    return 0;
  }
  return 1;
}

// operace "subseteq"
void is_subseteq(char str[], set_t arr[], int num_of_sets, FILE *f)
{
  int set_key = 0;    // proměnna pro id množiny
  int subset_key = 0; // proměnna pro id podmnožiny

  search_id_from_str(str, arr, num_of_sets, 3, &subset_key); // hledání id množiny na 3 pozicí řetězci "C"
  search_id_from_str(str, arr, num_of_sets, 4, &set_key);    // hledání id množiny na 4 pozicí řetězci "C"

  if (!check_id_of_sets(set_key, subset_key)) // pokud neni jeden z identifikátorů
  {
    return;
  }

  if (check_on_subset(&arr[subset_key], arr, num_of_sets, f, set_key)) // pokud je podmnožina
  {
    printf("true\n");
    return;
  }
  printf("false\n");
}

// operace "equals"
void is_equals(char *str, set_t arr[], int num_of_sets, FILE *f)
{
  int set1_id = 0;
  int set2_id = 0;
  search_id_from_str(str, arr, num_of_sets, 3, &set1_id); // hledání id množiny na 3 pozicí řetězci "C"
  search_id_from_str(str, arr, num_of_sets, 4, &set2_id); // hledání id množiny na 4 pozicí řetězci "C"

  if (!check_id_of_sets(set1_id, set2_id))
  {
    return;
  }
  if (check_on_subset(&arr[set2_id], arr, num_of_sets, f, set1_id)) // pokud je množina podmnožinou jiné množiny
  {
    if (arr[set1_id].size == arr[set2_id].size) // pokud mají stejný počet prvků
    {
      printf("true\n");
      return;
    }
  }
  printf("false\n");
}

// operace "subset" {oveření, že jedna množina je vlastní}
void is_subset(char *str, set_t arr[], int num_of_sets, FILE *f)
{
  int subset_id = 0;
  int set_id = 0;

  search_id_from_str(str, arr, num_of_sets, 3, &subset_id);
  search_id_from_str(str, arr, num_of_sets, 4, &set_id);

  if (!check_id_of_sets(subset_id, set_id))
  {
    return;
  }
  if (check_on_subset(&arr[subset_id], arr, num_of_sets, f, set_id)) // jestli množina je podmnožinou
  {
    if (arr[set_id].size != arr[subset_id].size) // jestli se jejich velikosti nerovnají
    {
      printf("true\n");
      return;
    }
  }
  printf("false\n");
}

//operace "minus"
void difference_of_sets(char *str, set_t arr[], int num_of_sets)
{
  int set1_id = 0;
  int set2_id = 0;
  int flag = 0;

  search_id_from_str(str, arr, num_of_sets, 3, &set1_id); // hledání id množiny na 3 pozicí řetězci "C"
  search_id_from_str(str, arr, num_of_sets, 4, &set2_id); // hledání id množiny na 4 pozicí řetězci "C"

  if (!check_id_of_sets(set1_id, set2_id)) // pokud neni jeden z identifikátorů
  {
    return;
  }
  printf("S");

  set_t set1 = arr[set1_id];
  set_t set2 = arr[set2_id];

  for (int i = 1; i < set1.size; i++) // i,j=1 pro vyloučení elementů množiny jako "U" a "S"
  {
    for (int j = 1; j < set2.size; j++)
    {
      if (strcmp(set1.elements[i], set2.elements[j]) == 0) // pokud máme stejné prvky
      {
        flag = 1;
      }
    }
    if (!flag) //pokud prvek ze set1 není v set2
    {
      printf(" %s", set1.elements[i]);
    }
    flag = 0;
  }
  printf("\n");
}

// operace "empty"
void set_is_empty(char *str, set_t arr[], int num_of_sets)
{
  int set_id = 0;
  search_id_from_str(str, arr, num_of_sets, 3, &set_id); // hledání id množiny na 3 pozicí řetězci "C"

  if (set_id == -1) //pokud není nalezen id
  {
    printf("WARNING: Set with that id doesnt exist\n");
    return;
  }
  if (arr[set_id].size == 1) // jestli máme jenom "S" prvek
  {
    printf("true\n");
    return;
  }
  printf("false\n");
}

// operace "complement"
void complement_of_set(char *str, set_t arr[], int num_of_sets)
{
  int set_id = 0;
  int flag = 0;

  search_id_from_str(str, arr, num_of_sets, 3, &set_id); // hledání id množiny na 3 pozicí řetězci "C"
  if (set_id == -1)                                      //pokud není nalezen id
  {
    printf("WARNING: Set with that id doesnt exist\n");
    return;
  }
  set_t main_set = arr[0];
  set_t subset = arr[set_id];

  printf("S");
  for (int i = 1; i < main_set.size; i++) // i,j=1 pro vyloučení elementů množiny jako "U" a "S"
  {
    for (int j = 1; j < subset.size; j++)
    {
      if (strcmp(main_set.elements[i], subset.elements[j]) == 0)
      {
        flag = 1;
      }
    }
    if (!flag)
    {
      printf(" %s", main_set.elements[i]);
    }
    flag = 0;
  }
  printf("\n");
}

// operace "card"
void card(char str[], set_t arr[], int num_of_sets)
{
  int set_id = 0;
  search_id_from_str(str, arr, num_of_sets, 3, &set_id); // hledání id množiny na 3 pozicí řetězci "C"

  if (set_id == -1) //pokud není nalezen id
  {
    printf("WARNING: Set with that id doesnt exist\n");
    return;
  }
  if (arr[set_id].size == 1) // pokud je v množině jenom prvek "S"(prázdná množina)
  {
    printf("0\n");
    return;
  }
  printf("%d\n", arr[set_id].size - 1); // -1 pro prvky jako "S,U"
}

// operace "intersect"
void find_intersection(char *str, set_t arr[], int num_of_sets)
{
  int set1_id = 0;
  int set2_id = 0;

  search_id_from_str(str, arr, num_of_sets, 3, &set1_id); // hledání id množiny na 3 pozicí řetězci "C"
  search_id_from_str(str, arr, num_of_sets, 4, &set2_id); // hledání id množiny na 4 pozicí řetězci "C"

  if (!(check_id_of_sets(set1_id, set2_id)))
  {
    return;
  }

  set_t set1 = arr[set1_id];
  set_t set2 = arr[set2_id];
  printf("S");

  for (int i = 1; i < set1.size; i++)
  {
    for (int j = 1; j < set2.size; j++)
    {
      if (strcmp(set1.elements[i], set2.elements[j]) == 0)
      {
        printf(" %s", set1.elements[i]);
      }
    }
  }
  printf("\n");
}

//operace "union"
void find_union(char *str, set_t arr[], int num_of_sets)
{
  int set1_id = 0;
  int set2_id = 0;
  int flag = 0;

  search_id_from_str(str, arr, num_of_sets, 3, &set1_id);
  search_id_from_str(str, arr, num_of_sets, 4, &set2_id);

  if (!(check_id_of_sets(set1_id, set2_id)))
  {
    return;
  }

  set_t set1 = arr[set1_id];
  set_t set2 = arr[set2_id];

  printf("S");
  for (int i = 1; i < set1.size; i++)
  {
    printf(" %s", set1.elements[i]);
  }

  for (int i = 1; i < set2.size; i++)
  {
    for (int j = 1; j < set1.size; j++)
    {
      if (strcmp(set1.elements[j], set2.elements[i]) == 0)
      {
        flag = 1;
      }
    }
    if (!flag)
    {
      printf(" %s", set2.elements[i]);
    }
    flag = 0;
  }
  printf("\n");
}

// uvolnit paměť relace
void free_rel(rel r)
{
  for (int i = 0; i < r.size; i++)
  {

    for (int j = 0; j < r.sets[i]->size; j++)
    {
      // printf("%s ", se.sets[i]->set[j]);
      free(r.sets[i]->elements[j]);
    }
    // printf("\n");

    free(r.sets[i]->elements);
    free(r.sets[i]);
  }
  free(r.sets);
}

// uvolnit paměť všech relací
void free_rels(rel re[], int rel_counter)
{
  for (int i = 0; i < rel_counter; i++)
  {
    free_rel(re[i]);
  }
}

// uvolnit paměť realcí a zavřit soubor
void free_rels_close(rel re[], int rel_counter, FILE *fl)
{
  free_rels(re, rel_counter);
  fclose(fl);
}

// funkce pro výstup relace
void print_rel(rel r)
{
  printf("R ");
  for (int i = 0; i < r.size; i++)
  {
    printf("(");
    for (int j = 0; j < r.sets[i]->size; j++)
    {
      if (r.sets[i]->size - j == 1)
      {
        printf("%s", r.sets[i]->elements[j]);
      }
      else
      {
        printf("%s ", r.sets[i]->elements[j]);
      }
    }
    if (r.size - 1 == i)
    {
      printf(")");
    }
    else
    {
      printf(") ");
    }
  }
  printf("\n");
}

// funkce tvoření realce
void initialize_rel(rel arr[], int row_counter, int rel_counter)
{
  arr[rel_counter].id = row_counter + 1;
  arr[rel_counter].size = 0;
  arr[rel_counter].sets = malloc(sizeof(set_t));
}

//uložení dvojic prvku pro relace
int save_set_for_rel(char *pch, set_t *s, int id)
{
  strcpy(s->elements[id % 2], pch);
  return 1;
}

// uložení prvků řetezce souboru do relace
int save_set_in_rel(set_t *s, rel *re)
{
  re->size++;
  re->sets = realloc(re->sets, sizeof(set_t) * re->size);
  re->sets[re->size - 1] = s;
  if (re->sets == NULL)
  {
    return 0;
  }

  return 1;
}

// inicializace jedneho ¨set¨ pro realace
int initialize_set_rel(set_t *s)
{
  s->size = 2;
  s->id = 0;
  s->elements = malloc(s->size * sizeof(char *));
  if (!malloc_set_elements(s))
  {
    return 0;
  }
  return 1;
}

// kontrola,že všechny závorky v realci jsou v pořádku
int row_checker_rel_parentheses(char row[])
{
  int l_parentheses = 0;
  int r_parentheses = 0;
  int row_len = (int)strlen(row);
  for (int i = 0; i < row_len; i++)
  {
    if (row[i] == '(')
    {
      l_parentheses++;
    }
    if (row[i] == ')')
    {
      r_parentheses++;
    }
  }

  if (l_parentheses == r_parentheses && r_parentheses != 0)
  {
    return 1;
  }

  return 0;
}

// dvojice v relaci opakuje, jedná se o chybu.
int checker_on_duplication_rel(rel r)
{

  for (int i = 0; i < r.size; i++)
  {
    int c = 0;
    char *first_element = r.sets[i]->elements[0];
    char *second_element = r.sets[i]->elements[1];
    for (int j = 0; j < r.size; j++)
    {
      if (!strcmp(first_element, r.sets[j]->elements[0]))
      {
        if (!strcmp(second_element, r.sets[j]->elements[1]))
        {
          c++;
        }
      }
    }

    if (c > 1)
    {
      return 0;
    }
  }
  return 1;
}

// kontrola,že všechny prvky relace jsou ve množině ¨Univerzum¨
int rel_contaning_u(set_t u, rel r)
{
  for (int i = 0; i < r.size; i++)
  {
    int c = 0;
    for (int j = 0; j < r.sets[i]->size; j++)
    {
      for (int k = 1; k < u.size; k++)
      {
        if (!strcmp(r.sets[i]->elements[j], u.elements[k]))
        {
          c++;
        }
      }
    }
    if (c != 2)
    {
      return 0;
    }
  }
  return 1;
}

// uložení relace
int save_relation(rel rels[], char row[], int row_counter, int rel_counter, set_t u)
{
  char *pch;
  set_t *s;
  int sets_c = 0;

  initialize_rel(rels, row_counter, rel_counter);

  if (!row_checker_rel_parentheses(row))
  {
    printf("ERROR: Bad format of the relation!\n");
    free(rels[rel_counter].sets);
    return 0;
  }

  pch = strtok(row, " )(");
  pch = strtok(NULL, " )(");
  while (pch != NULL)
  {

    if (sets_c != 0 && sets_c % 2 == 0)
    {

      if (!save_set_in_rel(s, &rels[rel_counter]))
      {
        return 0;
      }
    }

    if (sets_c % 2 == 0)
    {
      s = malloc(sizeof(set_t));
      initialize_set_rel(s);
    }
    if (!save_set_for_rel(pch, s, sets_c))
    {
      return 0;
    }

    pch = strtok(NULL, " )(");

    sets_c++;
  }

  // kvůli tomu, že poprve v while loopu kontrolujeme jestli už máme set pro dvojice a uložíme dvojice do relace, tak vždycky musíme mit poslední dvojice, kterou musíme uložit po skončeni while loopu
  if (!save_set_in_rel(s, &rels[rel_counter]))
  {
    return 0;
  }
  // kontrola, jestli relace obsahuje prvky z univerzu + uvolněni alokované paměti u relace, jestli podmínka nesplňuje
  if (!rel_contaning_u(u, rels[rel_counter]))
  {
    printf("ERROR: Relace has values, that are not in U!\n");
    free_rel(rels[rel_counter]);
    return 0;
  }
  // kontrola, jestli relace obsahuje duplikáty + uvolněni alokované paměti u relace, jestli podmínka nesplňuje
  if (!checker_on_duplication_rel(rels[rel_counter]))
  {
    printf("ERROR: Relace has duplicates!\n");
    free_rel(rels[rel_counter]);
    return 0;
  }

  return 1;
}

// hledání relace podle jeho ¨id¨ v poli relací
int search_relace(rel rels[], int id, int rel_counter)
{
  for (int i = 0; i < rel_counter; i++)
  {
    if (rels[i].id == id)
    {
      return i;
    }
  }
  return -1;
}

////////// Metody s relace

//najít id relace v řetezci
int find_rel_id(char str[])
{
  int counter = 1;             //1 pro prvek "C"
  char r[MAX_ROW_LEN];         // pole typu char s délkou = delce řetezce souboru
  memcpy(r, str, strlen(str)); // kopirování řetezce souboru, aby se nesmazal funkcí "strtok"
  char *pch = strtok(r, " ");

  while (pch != NULL)
  {
    if (isdigit(*pch) && counter == 3) // kontrola čísla na  pozici 3 řetězcu souboru
    {
      int num = atoi(pch);
      return num;
    }
    pch = strtok(NULL, " ");
    counter++;
  }
  return -1;
}

// operace ¨reflexive¨
void is_reflexive(char str[], rel rels[],int rel_counter)
{
  int id = find_rel_id(str);
  int rel_c = search_relace(rels, id, rel_counter);
  if (rel_c == -1)
  {
    printf("None of the relations has this index!\n");
    return;
  }

  for (int i = 0; i < rels[rel_c].size; i++)
  {
    for (int z = 0; z < 2; z++)
    {
      int c = 0;
      char *element = rels[rel_c].sets[i]->elements[z];
      for (int j = 0; j < rels[rel_c].size; j++)
      {
        if (!strcmp(rels[rel_c].sets[j]->elements[0], element))
        {
          if (!strcmp(rels[rel_c].sets[j]->elements[1], element))
          {
            c++;
            break;
          }
        }
      }
      if (c == 0)
      {
        printf("False\n");
        return;
      }
    }
  }
  printf("True\n");
}

// operace ¨symmetric¨
void is_symmetric(char str[],rel rels[], int rel_counter)
{
  int id = find_rel_id(str);
  int rel_c = search_relace(rels, id, rel_counter);

  if (rel_c == -1)
  {
    printf("None of the relations has this index!\n");
    return;
  }

  for (int i = 0; i < rels[rel_c].size; i++)
  {
    int c = 0;
    char *first_element = rels[rel_c].sets[i]->elements[0];
    char *second_element = rels[rel_c].sets[i]->elements[1];

    // kontrola, ze prvky nejsou stejné, protože v tomto případě tato dvojice může patrit i k symetrické relaci, i k antisymetrické relaci
    if (!strcmp(first_element, second_element)) {
        continue;
    }

    for (int j = 0; j < rels[rel_c].size; j++)
    {
      if (!(strcmp(rels[rel_c].sets[j]->elements[0], second_element) || strcmp(rels[rel_c].sets[j]->elements[1], first_element)))
      {
        c++;
        break;
      }
    }
    if (c == 0)
    {
      printf("False\n");
      return;
    }
  }
  printf("True\n");
}

// operace "antisymmetric"
void is_antisymmetric(char str[], rel rels[], int rel_counter) {
  int id = find_rel_id(str);
  int rel_c = search_relace(rels, id, rel_counter);

  if (rel_c == -1) {
    printf("None of the relations has this index!\n");
    return;
  }

  for (int i = 0; i < rels[rel_c].size; i++) {
    int c = 0;
    char * first_element = rels[rel_c].sets[i] -> elements[0];
    char * second_element = rels[rel_c].sets[i] -> elements[1];


    // kontrola, ze prvky nejsou stejné, protože v tomto případě tato dvojice může patrit i k symetrické relaci, i k antisymetrické relaci
    if (!strcmp(first_element, second_element)) {
      continue;
    }

    for (int j = 0; j < rels[rel_c].size; j++) {
      if (!(strcmp(rels[rel_c].sets[j] -> elements[0], second_element) || strcmp(rels[rel_c].sets[j] -> elements[1], first_element))) {
        c++;
        break;
      }
    }
    // jestli dvojice má symetrickou dvojice, tak vrátíme nulu
    if (c > 0) {
      printf("False\n");
      return;
    }
  }

  printf("True\n");
}

// operace "function"
void is_function(char str[], rel rels[], int rel_counter) {
  int id = find_rel_id(str);
  int rel_c = search_relace(rels, id, rel_counter);

  if (rel_c == -1) {
    printf("None of the relations has this index!\n");
    return;
  }

// kontrola, ze pro jeden vstup je jenom jeden vystup
  for (int i = 0; i < rels[rel_c].size; i++) {
    char * first_element = rels[rel_c].sets[i] -> elements[0];
    int c = 0;

    for (int j = 0; j < rels[rel_c].size; j++) {
      if (!strcmp(rels[rel_c].sets[j] -> elements[0], first_element)) {
        c++;
      }
    }
    if (c > 1) {
      printf("False\n");
      return;
    }
  }
  printf("True\n");
}

// operace "domain"
void domain(char str[], rel rels[], int rel_counter) {
  int id = find_rel_id(str);
  int rel_c = search_relace(rels, id, rel_counter);

  // vytvoříme pole a counter pro číslo prvků v poli pro kontrolu na duplikáty 
  char * domain_elements[MAX_ROW_LEN];
  int domain_elements_counter = 0;

  if (rel_c == -1) {
    printf("None of the relations has this index!\n");
    return;
  }

  // kontrola na duplikáty 
  for (int i = 0; i < rels[rel_c].size; i++) {

    char * first_element = rels[rel_c].sets[i] -> elements[0];
    int c = 0;

    for (int j = 0; j < domain_elements_counter; j++) {
      if (!strcmp(first_element, domain_elements[j])) {
        c++;
      }
    }
    if (c == 0) {

      domain_elements[domain_elements_counter] = first_element;
      domain_elements_counter++;
    }
  }

// vystup
  printf("S ");

  for (int i = 0; i < domain_elements_counter; i++) {
    if (domain_elements_counter - i == 1) {
      printf("%s", domain_elements[i]);
    } else {
      printf("%s ", domain_elements[i]);
    }
  }
  printf("\n");
}

// operace "codomain"
void codomain(char str[], rel rels[], int rel_counter) {
  int id = find_rel_id(str);
  int rel_c = search_relace(rels, id, rel_counter);
  // vytvoříme pole a counter pro číslo prvků v poli pro kontrolu na duplikáty 
  char * codomain_elements[MAX_ROW_LEN];
  int codomain_elements_counter = 0;

  if (rel_c == -1) {
    printf("None of the relations has this index!\n");
    return;
  }

// kontrola na duplikáty 
  for (int i = 0; i < rels[rel_c].size; i++) {

    char * second_element = rels[rel_c].sets[i] -> elements[1];
    int c = 0;

    for (int j = 0; j < codomain_elements_counter; j++) {
      if (!strcmp(second_element, codomain_elements[j])) {
        c++;
      }
    }
    if (c == 0) {
      codomain_elements[codomain_elements_counter] = second_element;
      codomain_elements_counter++;
    }
  }

// vystup
  printf("S ");

  for (int i = 0; i < codomain_elements_counter; i++) {
    if (codomain_elements_counter - i == 1) {
      printf("%s", codomain_elements[i]);
    } else {
      printf("%s ", codomain_elements[i]);
    }
  }
  printf("\n");
}


// podle operace z řetězcu "C" v souboru vykonává příkaz nad množiny
void select_set_command(char str[], set_t arr[], int num_of_sets, FILE *f, char *pch)
{
  if (strcmp(pch, "subseteq") == 0)
  {
    is_subseteq(str, arr, num_of_sets, f);
  }
  else if (strcmp(pch, "minus") == 0)
  {
    difference_of_sets(str, arr, num_of_sets);
  }
  else if (strcmp(pch, "empty") == 0)
  {
    set_is_empty(str, arr, num_of_sets);
  }
  else if (strcmp(pch, "complement") == 0)
  {
    complement_of_set(str, arr, num_of_sets);
  }
  else if (strcmp(pch, "card") == 0)
  {
    card(str, arr, num_of_sets);
  }
  else if (strcmp(pch, "equals") == 0)
  {
    is_equals(str, arr, num_of_sets, f);
  }
  else if (strcmp(pch, "subset") == 0)
  {
    is_subset(str, arr, num_of_sets, f);
  }
  else if (strcmp(pch, "intersect") == 0)
  {
    find_intersection(str, arr, num_of_sets);
  }
  else if (strcmp(pch, "union") == 0)
  {
    find_union(str, arr, num_of_sets);
  }
}

// kontrola toho, že řetězec "C" má operaci pro relaci
int is_rel_operation(char str[])
{
  char *operations[] = {"reflexive","symmetric","antisymmetric",
                        "transitive","function","domain",
                        "codomain", "injective","surjective",
                        "bijective"};
  int operations_count = 10;

  for (int i = 0; i < operations_count; i++)
  {
    if(strcmp(operations[i],str)==0)
    {
      return 1;
    }
  }
  return 0;
}

// podle operace z řetězcu "C" v souboru vykonává příkaz nad relacemi
void select_rel_command(char str[],rel rels[], int rel_counter, char *pch)
{
  if (strcmp(pch,"reflexive")==0)
  {
    is_reflexive(str,rels,rel_counter);
  }
  else if (strcmp(pch,"symmetric")==0)
  {
    is_symmetric(str,rels,rel_counter);
  }
  else if (strcmp(pch,"antisymmetric")==0)
  {
    is_antisymmetric(str,rels,rel_counter);
  }
  else if(strcmp(pch,"function")==0)
  {
    is_function(str,rels,rel_counter);
  }
  else if (strcmp(pch,"domain")==0)
  {
    domain(str,rels,rel_counter);
  }
  else if(strcmp(pch,"codomain")==0)
  {
    codomain(str,rels,rel_counter);
  }
}

// uřcíme jaka je operace v řetězci a pak ona se vykonává
void define_operation(char str[], rel rels[],set_t arr[], int num_of_sets,int rel_counter, FILE *f)
{
  char r[MAX_ROW_LEN];         // pole typu char s délkou = delce řetezce souboru
  memcpy(r, str, strlen(str)); // kopirování řetezce souboru, aby se nesmazal funkcí "strtok"

  char *pch = strtok(r, " "); //  bereme prvni prvek z řetězce (C)
  pch = strtok(NULL, " ");    //bereme přikaz z řetězce

  if (is_rel_operation(pch)) // pokud řetězec "C" obsahue operace pro relaci
  {
    select_rel_command(str,rels,rel_counter,pch);
    return;
  }
  select_set_command(str, arr, num_of_sets, f, pch); // výběr operace pro množiny
}


///////////////////////////MAIN////////////////////////////
int main(int argc, char *argv[])
{
  FILE *fl;

  if (!check_args_count(argc)) //kontrola počtů paramentů programu
  {
  	return 0;
  }

  fl = fopen(argv[1], "r");

  if (!check_file_opening(fl, argv)) // kontrola otevření souboru
  {
    return 0;
  }

  char row[MAX_ROW_LEN];      // pole typu "char" se stanovenou max délkou(která není...)
  set_t sets[MAX_ROWS_COUNT]; // pole typu "set" , kde každý prvek pole je množina
  rel rels[MAX_ROWS_COUNT];   // pole typu "rel", kde každý prvek pole je relace
  int rows_counter = 0;       // počet řetezců
  int set_counter = 0;        // počet množin
  int rel_counter = 0;        // počet relací
  int set_rel_counter = 0;    // čítač id pro množiny a relac

  while (fgets(row, MAX_ROW_LEN, fl) != NULL)
  {
    if (!check_rows_count(rows_counter))
    {
      free_sets_and_close_f(fl, sets, set_counter);
      return 0;
    }

    if (!check_row_errors(row, rows_counter)) // kontrola řetezce souboru, jestli má "špatný" format
    {
      free_sets_and_close_f(fl, sets, set_counter); // uvolnit paměť a zavřit soubor nebo jenom zavřit soubor
      return 0;
    }

    row[strcspn(row, "\n")] = '\0'; // náhrada symnbolem "|0" symbolu "|n", který dodává funkce fgets

    if ((row[0] == 'U') || (row[0] == 'S')) // pokud řetezec je množina
    {
      if (!check_set_errors(row, sets, set_counter, set_rel_counter, fl)) // kontrola prvků řetezce
      {
        if (rel_counter > 0)
        {
          free_rels(rels, rel_counter);
        }
        return 0;
      }

      save_set(row, sets, set_counter); //ukládáme  řetežec podle mezer do množiny

      if (!check_set_format(&sets[set_counter], sets, set_counter, fl)) // kontrola duplikátů množiny
      {
        if (rel_counter > 0)
        {
          free_rels(rels, rel_counter);
        }
        return 0;
      }

      print_set(&sets[set_counter]); // výstup prvků množiny
      printf("\n");                  // převod na nový řádek
      set_counter++;                 // zvětšujeme počet množin o 1
      set_rel_counter++;             
    }
    else if (row[0] == 'R')
    {
      if (!save_relation(rels, row, set_rel_counter, rel_counter, sets[0]))
      {
        free_rels_close(rels, rel_counter, fl);
        free_memory_of_sets(sets, set_counter);
        return 0;
      }

    //vývod relaci
      print_rel(rels[rel_counter]);
      rel_counter++;
      set_rel_counter++;
    }
    else if (row[0] == 'C')
    {
      define_operation(row,rels, sets, set_counter,rel_counter, fl);
    }

    rows_counter++;
  }
  if (set_counter > 0)
  {
    free_memory_of_sets(sets, set_counter); // uvolníme alokovánou paměť a zavřeme soubor
  }

  if (rel_counter > 0)//pokud máme relaci k uvolnění
  {
    free_rels(rels, rel_counter);
  }
  fclose(fl);
  return 0;
}