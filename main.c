#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <windows.h>
#include <winbase.h>
#define MAKSYMALNY_ROZMIAR_PLIKU 2000000   // Maksymalnie 2076248
#define ROZMIAR_BUFFORA_WPRZOD 10
#define ROZMIAR_BUFFORA_SZUKANIA 50
#define ILOSC_GENEROWANYCH_ZNAKOW 2000000
#define MAKSYMALNY_ROZMIAR_WYRAZU 10000

char buffor_wprzod[ROZMIAR_BUFFORA_WPRZOD]; char buffor_szukania[ROZMIAR_BUFFORA_SZUKANIA];
char dekodowane[MAKSYMALNY_ROZMIAR_PLIKU]; char zakodowane[MAKSYMALNY_ROZMIAR_PLIKU*7];
char tablica[];
char *tab = tablica;
int offset; char znak;
int i, j, k, l, m, n;
int odleglosc, dlugosc;
int debugowanie = 0; int benchmark = 0;
int index_tmp = 1;
char wybor; int ilosc;
clock_t zegar_s, zegar_k;

void delay(unsigned int mseconds)
{
    clock_t goal = mseconds + clock();
    while (goal > clock());
}

void LZ77_odczyt_pliku()
{
    FILE *plik_odczyt = fopen("1.txt", "r");
    struct stat st;
    stat("1.txt", &st);
    fgets(tablica, st.st_size+1, plik_odczyt);
    if (debugowanie == 1) printf("Tablica z pliku: %s\n", tablica);
    fclose(plik_odczyt);
}

void LZ77_zapis_pliku()
{
    FILE *plik_zapis = fopen ("2.txt", "wt");
    fwrite(zakodowane, strlen(zakodowane), 1, plik_zapis);
    fclose(plik_zapis);
}


void LZ77_zapis_pliku2()
{
    FILE *plik_zapis = fopen ("3.txt", "wt");
    fwrite(dekodowane, strlen(dekodowane), 1, plik_zapis);
    fclose(plik_zapis);
}

void LZ77_kodowanie(int odleglosc, int dlugosc, char znak)
{
    char wpis[((int)log10(odleglosc) + 1) + ((int)log10(dlugosc) + 1) + sizeof(char)];
    if (znak == NULL)  sprintf(wpis, "(%d,%d, )", odleglosc, dlugosc);
    else sprintf(wpis, "(%d,%d,%c)", odleglosc, dlugosc, znak);
    strcat(zakodowane, wpis);
}

char LZ77_czytaj()
{
    while (sscanf (tab,"%c%n", &znak, &offset) == 1)
    {
    tab += offset;
   // if (znak == '') znak = NULL;
    return znak;
    }
    return NULL;
}

void LZ77_ladowanie_buffora(int j)
{
    for (i = 0; i < j; i++)
    {
        buffor_wprzod[i] = LZ77_czytaj();
    }
}

void LZ77_wyswietl_buffor_wprzod()
{
    printf("Buffor wprzod - Rozmiar: %d - ", strlen(buffor_wprzod));
    for (i = 0; i < strlen(buffor_wprzod); i++)
    {
        printf("%c", buffor_wprzod[i]);
    }
    printf("\n");
}

void LZ77_wyswietl_buffor_szukania()
{
    printf("Buffor szukania - Rozmiar: %d - ", strlen(buffor_szukania));
    for (i = 0; i < strlen(buffor_szukania); i++)
    {
        printf("%c", buffor_szukania[i]);
    }
    printf("\n");
}

void LZ77_przesuwanie_buffora_wprzod(int j)
{
    k = j;
    for (i=0; i < strlen(buffor_wprzod); i++, j++)
    {
        buffor_wprzod[i] = buffor_wprzod[j];
    }

    j = strlen(buffor_wprzod);
    for(i =0; i< k; i++)
    {
       buffor_wprzod[j] = LZ77_czytaj();
       j++;
    }
}

void LZ77_dodaj_do_buffora_szukania(int j)
{

    for (i = 0; i < j; i++)
    {
        buffor_szukania[strlen(buffor_szukania)] = buffor_wprzod[i];
    }
buffor_szukania[ROZMIAR_BUFFORA_SZUKANIA] = NULL;
}

void LZ77_przesuwanie_buffora_szukania(int j)
{

    l = strlen(buffor_szukania);
    k = ROZMIAR_BUFFORA_SZUKANIA - strlen(buffor_szukania);

    for (i=0; i < l; i++)
    {
        buffor_szukania[i] = buffor_szukania[i+j-k];
        if (i+j > l) buffor_szukania[i] = NULL;
    }

    for (i = 0; i < j; i++)
    {
        buffor_szukania[ROZMIAR_BUFFORA_SZUKANIA-j+i] = buffor_wprzod[i];
    }
}

void LZ77_przesuwanie_buffora_szukania2(int j)
{

    l = strlen(buffor_szukania);
    for (i=0; i < l; i++)
    {
        buffor_szukania[i] = buffor_szukania[i+j];
        if (i+j > l) buffor_szukania[i] = NULL;
    }

    for (i = 0; i < j; i++)
    {
        buffor_szukania[l-j+i] = buffor_wprzod[i];
    }
}
void LZ77_przeszukiwanie_buffora_szukania()
{
    dlugosc=1;
    odleglosc = 0;
    if (strlen(buffor_szukania)<ROZMIAR_BUFFORA_SZUKANIA) l = strlen(buffor_szukania);
    else l = strlen(buffor_szukania);
    for (i = 0; i < strlen(buffor_szukania); i++)
    {

        if (buffor_szukania[i] == buffor_wprzod[0])
        {
        j = 1;

        if (debugowanie == 1) printf ("\nZnaleziono %c, w odleglosci: %d , dlugosc: %d, Rozmiar: %d", buffor_wprzod[0], l-i, dlugosc, l);

            while (buffor_szukania[i+j] == buffor_wprzod[j])
            {
               // if (buffor_szukania[i+j] == NULL) break;
                if (debugowanie == 1) printf ("\n\tNastepny: %c\t", buffor_szukania[i+j]);
                j++;
            }

            if (j >= dlugosc) {dlugosc = j; odleglosc = l-i; znak = buffor_wprzod[j];}
            if (debugowanie == 1) printf("\t\tDlugosc %d", j);
        }
    }
}

void LZ77_czyszczenie_bufforow()
{
    for (i = 0; i < ROZMIAR_BUFFORA_SZUKANIA; i++)
    {
        buffor_szukania[i] = NULL;
    }

        for (i = 0; i < ROZMIAR_BUFFORA_WPRZOD; i++)
    {
        buffor_wprzod[i] = NULL;
    }

}

void czyszczenie()
{
    for (i=0; i < strlen(dekodowane); i++)
    {
        dekodowane[i] = NULL;
    }
        for (i=0; i < strlen(zakodowane); i++)
    {
        zakodowane[i] = NULL;
    }

    index_tmp = 1;
}
void LZ77_kompresja()
{
    LZ77_czyszczenie_bufforow();
    zegar_s = clock();
    tab = tablica;
    LZ77_odczyt_pliku();
    LZ77_ladowanie_buffora(ROZMIAR_BUFFORA_WPRZOD);
    if (debugowanie == 1) LZ77_wyswietl_buffor_wprzod();
    if (debugowanie == 1) LZ77_wyswietl_buffor_szukania();

    while (strlen(buffor_wprzod) > 0)
    {

        LZ77_przeszukiwanie_buffora_szukania();
        if (odleglosc == 0)
        {
            dlugosc = 0;
            if (debugowanie == 1) printf("Nie znaleziono %c\n", buffor_wprzod[0]);
            LZ77_kodowanie(0, 0, buffor_wprzod[0]);
            if (strlen(buffor_szukania)+ 1> ROZMIAR_BUFFORA_SZUKANIA ) LZ77_przesuwanie_buffora_szukania(1);
            else LZ77_dodaj_do_buffora_szukania(1);

            LZ77_przesuwanie_buffora_wprzod(1);
            if (debugowanie == 1) LZ77_wyswietl_buffor_wprzod();
            if (debugowanie == 1) LZ77_wyswietl_buffor_szukania();
        }
        else
        {
            if (debugowanie == 1)  printf("\nNajlepsze dopasowanie - Odleglosc: %d, Dlugosc: %d, Nastepny znak: %c\n", odleglosc, dlugosc, znak);
            LZ77_kodowanie(odleglosc, dlugosc, znak);
            if (strlen(buffor_szukania)+ dlugosc+1> ROZMIAR_BUFFORA_SZUKANIA ) LZ77_przesuwanie_buffora_szukania(dlugosc+1);
            else LZ77_dodaj_do_buffora_szukania(dlugosc+1);
            LZ77_przesuwanie_buffora_wprzod(dlugosc+1);
            if (debugowanie == 1) LZ77_wyswietl_buffor_wprzod();
            if (debugowanie == 1) LZ77_wyswietl_buffor_szukania();
        }

    }
    zegar_k = clock();
    LZ77_zapis_pliku();
    printf("\n%s", zakodowane);
    printf("\nZapisano do pliku 2.txt");
    printf ("\nCzas wykonania wynosi: %.3f sekund\n", (float)(zegar_k - zegar_s) / CLOCKS_PER_SEC );
    logi(3, (float)(zegar_k - zegar_s) / CLOCKS_PER_SEC , ilosc, 0, 0, 0);
}

void LZ77_dekompresja()
{
    zegar_s = clock();
    FILE *plik_odczyt = fopen("2.txt", "r");
    struct stat st;
    stat("2.txt", &st);
    if (st.st_size > MAKSYMALNY_ROZMIAR_PLIKU) printf("Plik jest za duzy");
    else
    {
    char tablica[st.st_size];
    fgets(tablica, st.st_size+1, plik_odczyt);
    char *tab = tablica;

    int index_tmp = 0;

    while (sscanf (tab,"(%d,%d,%c)%n", &odleglosc, &dlugosc, &znak, &offset) == 3)
    {
    tab += offset;

        if (odleglosc == 0)
        {
            dekodowane[index_tmp] = znak;
            if (debugowanie == 1) printf("%c\n", dekodowane[index_tmp]);
            index_tmp++;
        }
        else
        {

            for (i=0; i < dlugosc; i++)
            {
            dekodowane[index_tmp] = dekodowane[index_tmp-odleglosc];
            if (debugowanie == 1) printf ("\t%c\n", dekodowane[index_tmp-odleglosc]);
            index_tmp++;
            }

            dekodowane[index_tmp] = znak;
            if (debugowanie == 1) printf("\t\t%c\n", dekodowane[index_tmp]);
            index_tmp++;
        }
    }

       if (dekodowane[strlen(dekodowane)-1] == ' ') dekodowane[strlen(dekodowane)-1] = NULL;

    LZ77_zapis_pliku2();
    zegar_k = clock();
    printf("\n%s", dekodowane);
    printf("\nZapisano do pliku 3.txt");
    printf ("\nCzas wykonania wynosi: %.3f sekund\n", (float)(zegar_k - zegar_s) / CLOCKS_PER_SEC );
    logi(4, (float)(zegar_k - zegar_s) / CLOCKS_PER_SEC , ilosc, 0, 0, 0);
    }
}

struct LZ78_drzewo
{
int index;
char wyraz[MAKSYMALNY_ROZMIAR_WYRAZU];
struct drzewo * right, * left;
};

typedef struct LZ78_drzewo node;

void LZ78_wstaw(node ** tree, char* wyraz)
{
    node *temp = NULL;
    if(!(*tree))
    {
        temp = (node *)malloc(sizeof(node));
        temp->left = temp->right = NULL;
        temp->index = index_tmp;
        strcpy(temp->wyraz, wyraz);
        *tree = temp;
        index_tmp++;
        return;
    }

    int cmp = strcmp((*tree)->wyraz, wyraz);
    if (cmp > 0) {
            LZ78_wstaw(&(*tree)->left, wyraz);
    }
    if (cmp < 0) {
            LZ78_wstaw(&(*tree)->right, wyraz);
    }
}

void LZ78_wstaw_dekodowanie(node ** tree, char* wyraz, int index)
{
    if (index == 0) index = index_tmp;
    node *temp = NULL;
    if(!(*tree))
    {

        temp = (node *)malloc(sizeof(node));
        temp->left = temp->right = NULL;
        temp->index = index;
        strcpy(temp->wyraz, wyraz);
        *tree = temp;
        index_tmp++;
        return;
    }

    if(index < (*tree)->index)
    {
        LZ78_wstaw_dekodowanie(&(*tree)->left, wyraz, index);
    }
    else if(index > (*tree)->index)
    {
        LZ78_wstaw_dekodowanie(&(*tree)->right, wyraz, index);
    }

}

void LZ78_print_inorder(node * tree)
{
    if (tree)
    {
        LZ78_print_inorder(tree->left);
        printf("Index: %d, Wyraz: %s \n", tree->index, tree->wyraz);
        LZ78_print_inorder(tree->right);
    }
}

void LZ78_usun(node * tree)
{
    if (tree)
    {
        LZ78_usun(tree->left);
        LZ78_usun(tree->right);
        free(tree);
    }
}

node* LZ78_znajdz(node ** tree, char* wyraz)
{
    if(!(*tree))
    {
        return NULL;
    }

    int cmp = strcmp((*tree)->wyraz, wyraz);
     if (cmp > 0)
    {
        LZ78_znajdz(&((*tree)->left), wyraz);
    }
    else if (cmp < 0)
    {
        LZ78_znajdz(&((*tree)->right), wyraz);
    }
    else
    {
        return *tree;
    }
}

node* LZ78_znajdz_index(node ** tree, int index)
{
    if(!(*tree))
    {
        return NULL;
    }

    if(index < (*tree)->index)
    {
        LZ78_znajdz_index(&((*tree)->left), index);
    }
    else if(index > (*tree)->index)
    {
        LZ78_znajdz_index(&((*tree)->right), index);
    }
    else if(index == (*tree)->index)
    {
        return *tree;
    }
}

void LZ78_kodowanie(int index, char znak)
{
    char wpis[MAKSYMALNY_ROZMIAR_WYRAZU+3+((int)log10(index_tmp) + 1)];

    sprintf(wpis, "(%d,%c)", index, znak);
    strcat(zakodowane, wpis);

}

void LZ78_kompresja()
{
    zegar_s = clock();
    node *root;
    node *tmp;
    root = NULL;
    FILE *plik_odczyt = fopen("1.txt", "r");
    FILE *plik_zapis = fopen ("2.txt", "wt");
    char znak;
    int index;
    char string[MAKSYMALNY_ROZMIAR_WYRAZU];
    char string2[MAKSYMALNY_ROZMIAR_WYRAZU];

    while(znak!=EOF)
    {
        znak=fgetc(plik_odczyt);
        if (debugowanie == 1) printf("\n%c",znak);
        sprintf(string, "%c", znak);
        tmp = LZ78_znajdz(&root, string);

        if (tmp)
        {
            while (tmp)

            {
                znak=fgetc(plik_odczyt);
                sprintf(string2, "%c", znak);
                if (debugowanie == 1) printf("    Znaleziono:  %s",string);
                if (debugowanie == 1) printf("    Nastepny Znak: %c",znak);
                strcat(string, string2);
                if (debugowanie == 1) printf("    Dodano: %s",string);
                index = tmp->index;
                tmp = LZ78_znajdz(&root, string);
            }
            LZ78_wstaw(&root,string);
            LZ78_kodowanie(index, znak);
        }
        else
        {
            LZ78_wstaw(&root,string);
            LZ78_kodowanie(0, znak);
        }
    }
    printf("%s", zakodowane);
    fwrite(zakodowane, strlen(zakodowane), 1, plik_zapis);
    fclose(plik_zapis);
    fclose(plik_odczyt);
    LZ78_usun(root);
    index_tmp = 1;
    zegar_k = clock();
    printf("\nZapisano do pliku 2.txt");
    printf ("\nCzas wykonania wynosi: %.3f sekund", (float)(zegar_k - zegar_s) / CLOCKS_PER_SEC );
    logi(5, (float)(zegar_k - zegar_s) / CLOCKS_PER_SEC , ilosc, 0, 0, 0);
}

void LZ78_dekompresja()
{
zegar_s = clock();
node *root;
node *tmp;
root = NULL;

FILE *plik_odczyt = fopen("2.txt", "r");
FILE *plik_zapis = fopen ("3.txt", "wt");
struct stat st;
stat("2.txt", &st);
if (st.st_size > MAKSYMALNY_ROZMIAR_PLIKU) printf("Plik jest za duzy");
else
{
char tablica[st.st_size];
char *tab = tablica;
fgets(tablica, st.st_size, plik_odczyt);

char znak;
int index;
int koniec;
char wyraz[MAKSYMALNY_ROZMIAR_WYRAZU];
char nowy[MAKSYMALNY_ROZMIAR_WYRAZU];

        if (debugowanie == 1) printf("%s\n", tablica);


        while (sscanf (tab,"(%d,%c)%n", &index, &znak, &koniec) == 2)
        {
        if (debugowanie == 1) printf("\n%d ", index);
        if (debugowanie == 1) printf("%c", znak);
        tab += koniec;
        if (znak == 'ÿ') znak = NULL;
        sprintf(wyraz, "%c", znak);


            tmp = LZ78_znajdz_index(&root, index);
            if (tmp)
            {

                if (debugowanie == 1) printf(" - Znaleziono = %s ", tmp->wyraz);
                strcpy(nowy, tmp->wyraz);
                strcat(nowy, wyraz);
                if (debugowanie == 1) printf("Nastepny: %s", wyraz);

                LZ78_wstaw_dekodowanie(&root, nowy, 0);
                strcat(dekodowane, nowy);
            }
            else
            {
                LZ78_wstaw_dekodowanie(&root, wyraz, 0);
                strcat(dekodowane, wyraz);
            }
        }
    dekodowane[sizeof(dekodowane)] = NULL;
    printf("\n%s", dekodowane);
    fwrite(dekodowane, strlen(dekodowane), 1, plik_zapis);
    fclose(plik_odczyt);
    fclose(plik_zapis);
    LZ78_usun(root);
    zegar_k = clock();
    printf("\nZapisano do pliku 3.txt");
    printf ("\nCzas wykonania wynosi: %.3f sekund", (float)(zegar_k - zegar_s) / CLOCKS_PER_SEC );
    logi(6, (float)(zegar_k - zegar_s) / CLOCKS_PER_SEC , ilosc, 0, 0, 0);
}
}

void generator()
{
    if (benchmark == 0)
        {
        printf("Jakie znaki maja byc losowane:\n");

        printf("1 - Male litery:\t");
        if (i == 1) printf("\x1B[32m" "#\n" "\x1B[0m");
        else printf("\x1B[31m" "#\n" "\x1B[0m");

        printf("2 - Duze litery:\t");
        if (j == 1) printf("\x1B[32m" "#\n" "\x1B[0m");
        else printf("\x1B[31m" "#\n" "\x1B[0m");

        printf("3 - Znaki specjalne:\t");
        if (k == 1) printf("\x1B[32m" "#\n" "\x1B[0m");
        else printf("\x1B[31m" "#\n" "\x1B[0m");

        printf("4 - Cyfry:\t\t");
        if (l == 1) printf("\x1B[32m" "#\n" "\x1B[0m");
        else printf("\x1B[31m" "#\n" "\x1B[0m");

        printf("\nENTER - Generuj\n");

        wybor = getch();
        if (wybor == '1') { if (i == 0) i = 1; else i = 0; system("cls"); generator();};
        if (wybor == '2') { if (j == 0) j = 1; else j = 0; system("cls"); generator();};
        if (wybor == '3') { if (k == 0) k = 1; else k = 0; system("cls"); generator();};
        if (wybor == '4') { if (l == 0) l = 1; else l = 0; system("cls"); generator();};
        }
        else
        {
            wybor = 13;
        }

    m = i+j+k+l;
    int t[4];

    if (wybor == 13)
    {
        zegar_s = clock();
        if (i == 1 && j == 1 && k == 1 && l == 1) {t[0] = 1; t[1] = 2; t[2] = 3; t[3] = 4;}
        if (i == 1 && j == 1 && k == 1 && l == 0) {t[0] = 1; t[1] = 2; t[2] = 3; t[3] = NULL;}
        if (i == 1 && j == 1 && k == 0 && l == 1) {t[0] = 1; t[1] = 2; t[2] = 4; t[3] = NULL;}
        if (i == 1 && j == 0 && k == 1 && l == 1) {t[0] = 1; t[1] = 3; t[2] = 4; t[3] = NULL;}
        if (i == 0 && j == 1 && k == 1 && l == 1) {t[0] = 2; t[1] = 3; t[2] = 4; t[3] = NULL;}
        if (i == 1 && j == 1 && k == 0 && l == 0) {t[0] = 1; t[1] = 2; t[2] = NULL; t[3] = NULL;}
        if (i == 1 && j == 0 && k == 1 && l == 0) {t[0] = 1; t[1] = 3; t[2] = NULL; t[3] = NULL;}
        if (i == 1 && j == 0 && k == 0 && l == 1) {t[0] = 1; t[1] = 4; t[2] = NULL; t[3] = NULL;}
        if (i == 0 && j == 1 && k == 1 && l == 0) {t[0] = 2; t[1] = 3; t[2] = NULL; t[3] = NULL;}
        if (i == 0 && j == 1 && k == 0 && l == 1) {t[0] = 2; t[1] = 4; t[2] = NULL; t[3] = NULL;}
        if (i == 0 && j == 0 && k == 1 && l == 1) {t[0] = 3; t[1] = 4; t[2] = NULL; t[3] = NULL;}
        if (i == 1 && j == 0 && k == 0 && l == 0) {t[0] = 1; t[1] = NULL; t[2] = NULL; t[3] = NULL;}
        if (i == 0 && j == 1 && k == 0 && l == 0) {t[0] = 2; t[1] = NULL; t[2] = NULL; t[3] = NULL;}
        if (i == 0 && j == 0 && k == 1 && l == 0) {t[0] = 3; t[1] = NULL; t[2] = NULL; t[3] = NULL;}
        if (i == 0 && j == 0 && k == 0 && l == 1) {t[0] = 4; t[1] = NULL; t[2] = NULL; t[3] = NULL;}

        if (benchmark == 0)
            {
            printf("\nProsze podac ilosc znakow: ");
            scanf("%d", &ilosc);
            if (ilosc >ILOSC_GENEROWANYCH_ZNAKOW) printf ("\nWybrano za duza ilosc znakow.\nMaksymalna ilosc mozna zmienic w kodzie zrodlowym.\n");
            }

        FILE *plik_zapis = fopen ("1.txt", "wt");
        char tab[ILOSC_GENEROWANYCH_ZNAKOW-1];
        srand(time(NULL));

        for (int i=0; i < ilosc; i++)
        {
            n =  t[0 + rand() % m];
            if (n==1) tab[i] = 'a' + rand()%('z' - 'a');
            if (n==2) tab[i] = 'A' + rand()%('Z' - 'A');
            if (n==3) tab[i] = '!' + rand()%('/' - '!');
            if (n==4) tab[i] = '0' + rand()%('9' - '0');
        }

        fwrite(tab, strlen(tab), sizeof(char), plik_zapis);
        fclose(plik_zapis);
        zegar_k = clock();

        printf("%s",tab);
        printf("\nPomyslnie wygenerowano %d znakow i zapisano do pliku 1.txt", ilosc);
        printf ("\nCzas wykonania wynosi: %.3f sekund\n", (float)(zegar_k - zegar_s) / CLOCKS_PER_SEC );

        logi(1, (float)(zegar_k - zegar_s) / CLOCKS_PER_SEC , ilosc, 0, 0, 0);
        wybor = 0;
    }

}

void test()
{
    zegar_s = clock();
    FILE *plik_zrodlowy = fopen("1.txt", "r"); FILE *plik_skompresowany = fopen("2.txt", "r"); FILE *plik_zdekompresowany = fopen("3.txt", "r");
    struct stat st1; struct stat st2; struct stat st3;
    stat("1.txt", &st1); stat("2.txt", &st2); stat("3.txt", &st3);
    char tablica1[st1.st_size]; char tablica3[st3.st_size];
    fgets(tablica1, st1.st_size, plik_zrodlowy); fgets(tablica3, st3.st_size, plik_zdekompresowany);
    fclose(plik_zrodlowy); fclose(plik_skompresowany); fclose(plik_zdekompresowany);

    int ilosc_zgodnych_znakow = 0;

    for (i = 0; i <= strlen(tablica1); i++ )
    {
        if (tablica1[i] == tablica3[i]) ilosc_zgodnych_znakow +=1;
        if (debugowanie == 1 && tablica1[i] != tablica3[i]) printf("Niepasujaca para znakow %c - %c", tablica1[i], tablica3[i]);
    }
    zegar_k = clock();

    printf("\nZgodnosc plikow przed kompresja i po dekompresji wynosi: %.2f %%\n", (double)ilosc_zgodnych_znakow/(double)st1.st_size*100);
    printf("Stopien kompresji wynosi: \t\t%.2f %%\n", ((double)st1.st_size-(double)st2.st_size)/(double)st1.st_size*100);
    printf("Rozmiar pliku zrodlowego: \t\t%d Bajtow\n", st1.st_size);
    printf("Rozmiar pliku skompresowanego: \t\t%d Bajtow\n", st2.st_size);
    printf("Rozmiar pliku zdekompresowanego: \t%d Bajtow\n", st3.st_size);
    printf ("Czas wykonania wynosi: %.3f sekund\n\n", (float)(zegar_k - zegar_s) / CLOCKS_PER_SEC );


    logi(2, (float)(zegar_k - zegar_s) / CLOCKS_PER_SEC , st1.st_size, st2.st_size, (float)(st1.st_size-(float)st2.st_size)/(float)st1.st_size*100, (float)ilosc_zgodnych_znakow/(float)st1.st_size*100);

}

void logi(int opcja, double czas, int parametr1, int parametr2, double parametr3, double parametr4)
{
    FILE *plik_logi = fopen ("logi.txt", "a+");

    SYSTEMTIME st;
    GetSystemTime(&st);


    if (opcja == 1)
    {
        fprintf(plik_logi,"-------------------- Generator znakow losowych -------------------\nData i godzina:\t%.2d.%.2d.%d - %.2d:%.2d",st.wDay, st.wMonth, st.wYear, st.wHour+2, st.wMinute);
        fprintf(plik_logi,"\nIlosc wygenerowanych znakow: %d\nRodzaj generowanych znakow: ", parametr1);
        if (i == 1) fprintf(plik_logi,"male litery, "); if (j == 1) fprintf(plik_logi,"duze litery, ");
        if (k == 1) fprintf(plik_logi,"znaki specjalne, ");  if (l == 1) fprintf(plik_logi,"cyfry, ");
        fprintf(plik_logi,"\nCzas wykonania: %.3f s\n\n", czas);
    }

    if (opcja == 2)
    {
        fprintf(plik_logi,"------------------------------ Test ------------------------------\nData i godzina:\t%.2d.%.2d.%d - %.2d:%.2d",st.wDay, st.wMonth, st.wYear, st.wHour+2, st.wMinute);
        fprintf(plik_logi,"\nRozmiar pliku kompresowanego: %d bajtow\nRozmiar pliku po kompresji: %d bajtow", parametr1, parametr2);
        fprintf(plik_logi,"\nStopien kompresji: %.2f %%\nZgodnosc plikow: %.2f %%", parametr3, parametr4);
        fprintf(plik_logi,"\nCzas wykonania testu: %.3f s\n\n", czas);
    }

    if (opcja == 3)
    {
        fprintf(plik_logi,"------------------------ LZ77 - Kompresja ------------------------\nData i godzina:\t%.2d.%.2d.%d - %.2d:%.2d",st.wDay, st.wMonth, st.wYear, st.wHour+2, st.wMinute);
        fprintf(plik_logi,"\nCzas wykonania: %.3f s\n\n", czas);
    }
    if (opcja == 4)
    {
        fprintf(plik_logi,"----------------------- LZ77 - Dekompresja -----------------------\nData i godzina:\t%.2d.%.2d.%d - %.2d:%.2d",st.wDay, st.wMonth, st.wYear, st.wHour+2, st.wMinute);
        fprintf(plik_logi,"\nCzas wykonania: %.3f s\n\n", czas);
    }
    if (opcja == 5)
    {
        fprintf(plik_logi,"------------------------ LZ78 - Kompresja ------------------------\nData i godzina:\t%.2d.%.2d.%d - %.2d:%.2d",st.wDay, st.wMonth, st.wYear, st.wHour+2, st.wMinute);
        fprintf(plik_logi,"\nCzas wykonania: %.3f s\n\n", czas);
    }
    if (opcja == 6)
    {
        fprintf(plik_logi,"----------------------- LZ78 - Dekompresja -----------------------\nData i godzina:\t%.2d.%.2d.%d - %.2d:%.2d",st.wDay, st.wMonth, st.wYear, st.wHour+2, st.wMinute);
        fprintf(plik_logi,"\nCzas wykonania: %.3f s\n\n", czas);
    }

    fclose (plik_logi);
}

void benchmark_funkcja()
{
ilosc = 100; i = 1 ; j = 0 ; k = 0 ; l = 0 ; generator();
LZ77_kompresja(); LZ77_dekompresja(); test(); czyszczenie();
LZ78_kompresja(); LZ78_dekompresja(); test(); czyszczenie();
delay(100);

ilosc = 1000; i = 1 ; j = 0 ; k = 0 ; l = 0 ; generator();
LZ77_kompresja(); LZ77_dekompresja(); test(); czyszczenie();
LZ78_kompresja(); LZ78_dekompresja(); test(); czyszczenie();
delay(100);

ilosc = 10000; i = 1 ; j = 0 ; k = 0 ; l = 0 ; generator();
LZ77_kompresja(); LZ77_dekompresja(); test(); czyszczenie();
LZ78_kompresja(); LZ78_dekompresja(); test(); czyszczenie();
delay(100);

ilosc = 100000; i = 1 ; j = 0 ; k = 0 ; l = 0 ; generator();
LZ77_kompresja(); LZ77_dekompresja(); test(); czyszczenie();
LZ78_kompresja(); LZ78_dekompresja(); test(); czyszczenie();
delay(100);

ilosc = rand()%100000; i = rand()%2 ; j = rand()%2 ; k = rand()%2 ; l = rand()%2 ; generator();
LZ77_kompresja(); LZ77_dekompresja(); test(); czyszczenie();
LZ78_kompresja(); LZ78_dekompresja(); test(); czyszczenie();
delay(100);

ilosc = rand()%100000; i = rand()%2 ; j = rand()%2 ; k = rand()%2 ; l = rand()%2 ; generator();
LZ77_kompresja(); LZ77_dekompresja(); test(); czyszczenie();
LZ78_kompresja(); LZ78_dekompresja(); test(); czyszczenie();
delay(100);

ilosc = rand()%100000; i = rand()%2 ; j = rand()%2 ; k = rand()%2 ; l = rand()%2 ; generator();
LZ77_kompresja(); LZ77_dekompresja(); test(); czyszczenie();
LZ78_kompresja(); LZ78_dekompresja(); test(); czyszczenie();
delay(100);

ilosc = rand()%100000; i = rand()%2 ; j = rand()%2 ; k = rand()%2 ; l = rand()%2 ; generator();
LZ77_kompresja(); LZ77_dekompresja(); test(); czyszczenie();
LZ78_kompresja(); LZ78_dekompresja(); test(); czyszczenie();
delay(100);

ilosc = rand()%100000; i = rand()%2 ; j = rand()%2 ; k = rand()%2 ; l = rand()%2 ; generator();
LZ77_kompresja(); LZ77_dekompresja(); test(); czyszczenie();
LZ78_kompresja(); LZ78_dekompresja(); test(); czyszczenie();
delay(100);
}

void menu()
{

    printf("1 - LZ77\n2 - LZ78\n3 - Test\n4 - Generator losowych znakow");
    if (debugowanie == 0) printf ("\n5 - Debugowanie : Wylaczone\n");
    if (debugowanie == 1) printf ("\n5 - Debugowanie : Wlaczone\n");
    printf("\n6 - Benchmark - Automatyczne testowanie wraz z zapisywanie wynikow do logow\n");

    wybor = getch();
    if (wybor == '1')
    {
        system("cls");
        printf("LZ77\n1 - Kompresja\n2 - Dekompresja\n");
        wybor = getch();
        if (wybor == '1') LZ77_kompresja();
        if (wybor == '2') LZ77_dekompresja();
        wybor = 0;
    }
    if (wybor == '2')
    {
        system("cls");
        printf("LZ78\n1 - Kompresja\n2 - Dekompresja\n");
        wybor = getch();
        if (wybor == '1') LZ78_kompresja();
        if (wybor == '2') LZ78_dekompresja();
        wybor = 0;
    }
    if (wybor == '3') {system("cls"); test();}
    if (wybor == '4') {system("cls"); generator();}
    if (wybor == '5') { if (debugowanie == 0) debugowanie = 1; else debugowanie = 0; system("cls"); menu();}
    if (wybor == '6') {system("cls"); benchmark=1; benchmark_funkcja();}

}

int main()
{
menu();
printf("\n"); system("pause");
return 0;
}
