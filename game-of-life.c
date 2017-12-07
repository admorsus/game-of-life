// Made by Miguel Bautista Pérez
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

typedef struct MAT
{
    int h; //heigth
    int l; //length
    char **rack;    //rack
} MAT;

void show_help (char*);
char conway (MAT*);
void input_mode (MAT*);
char locals (MAT*, int, int);
void make_r (MAT*, int, int);
void fill_r (MAT*, char);
void print_r (MAT*, char, char, char, char, char);

int main (char argc, char **argv)
{    
    const char *txtHelp= "[OPTION] [PARAMETER] [...]\n"
        "A customisable Conway's Game of Life for shell.\n"
        "\n"
        "Options:\n"
        " -x N       Set grid length to N\n"
        " -y N       Set grid heigth to N\n"
        " -t N       Set time between iterations to N\n"
        " -i N       Set maximum iterations to N\n"
        " -r N       Set appearance probability to N\n"
        " -p         Start input cells mode\n"
        " -l         Show all delimiters\n"
        " -a C       Represent alive cells with C\n"
        " -d C       Represent dead cells with C\n"
        " -z C       Represent horizontal delimiters with C\n"
        " -v C       Represent vertical delimiters with C\n"
        " -c C       Represent corner delimiters with C\n"
        " -q         Do not show final message\n"
        " -h         Show this message and exits\n";
    char txtReset[20];
    FILE *fp = popen ("tput reset", "r");
    int szReset= fread (txtReset, 1, sizeof(txtReset), fp);
    pclose (fp);
    
    MAT mat;
    char verbosity= 1;
    int length= 20;
    int heigth= 20;
    int time= 200;
    int iter= 0;
    int maxIter= 0; //0 equals infinite
    char appProb=33;
    char inputMode= 0;
    char showAllDel= 0;
    char cAlive='@', cDead=' ';
    char delHoriz=0, delVert=0, delCorner=0;
    
    const char *opts= "hqplx:y:t:i:r:a:d:z:v:c:";
    int opt;
    while ((opt= getopt(argc, argv, opts)) != -1)
    {
        switch (opt) {
            case 'h':
                printf("Usage: %s %s\n", argv[0], txtHelp);
                return 0;
                break;
            case 'x':
                length= atoi(optarg);
                break;
            case 'y':
                heigth= atoi(optarg);
                break;
            case 't':
                time= atoi(optarg);
                break;
            case 'i':
                maxIter= atoi(optarg);
                break;
            case 'r':
                appProb= atoi(optarg);
                break;
            case 'p':
                inputMode= 1;
                break;
            case 'l':
                showAllDel= 1;
                break;
            case 'a':
                cAlive= optarg[0];
                break;
            case 'd':
                cDead= optarg[0];
                break;
            case 'z':
                delHoriz= optarg[0];
                break;
            case 'v':
                delVert= optarg[0];
                break;
            case 'c':
                delCorner= optarg[0];
                break;
            case 'q':
                verbosity= 0;
                break;
            default:
                printf("Try '%s -h' for more information.\n", argv[0]);
                return 1;
        }
    }
    
    make_r(&mat, length, heigth);
    if (inputMode)
    {
        write(1, txtReset, szReset);
        input_mode(&mat);
    }
    else
        fill_r(&mat, appProb);
        
    if (showAllDel)
    {
        if (!delHoriz) delHoriz= '~';
        if (!delVert) delVert= '|';
        if (!delCorner) delCorner= '+';
    }
        
    do //main loop
    {
        write(1, txtReset, szReset);
        print_r(&mat, cAlive, cDead, delHoriz, delVert, delCorner);
        usleep(time *1000);
        iter++;
    } while (conway(&mat) && iter!=maxIter);
    
    if (verbosity)
        printf("%d iterations realised.\n", iter);
    free(mat.rack);
    return 0;
}

char conway (MAT *mat)
{
    MAT gen;
    int l= mat->l;
    int h= mat->h;
    char activity=0;
    make_r(&gen, l, h);
    for (int y=0; y<h; y++)
    {
        for (int x=0; x<l; x++)
        {
            char n= locals(mat, x, y);
            if (mat->rack[x][y] && n==2)
                gen.rack[x][y]= 1;
            else if (n==3)
                gen.rack[x][y]= 1;
            else
                gen.rack[x][y]= 0;
            if (mat->rack[x][y] != gen.rack[x][y])
                activity=1;
        }
    }
    *mat= gen;
    return activity;
}

void input_mode (MAT *mat)
{
    int c=0, nx, ny;
    int l= mat->l;
    int h= mat->h;
    printf("Input cells mode, input X and Y for each cell:\n"
        " at any time: -1: Show status, -2: Stop input.\n\n");
    while (1)
    {
        printf("X= ");
        scanf("%d", &nx);
        if (nx==-1)
        {
            print_r(mat,'@','.',0,0,0);
            continue;
        }
        else if (nx<=-2)
            break;
        printf("Y= ");
        scanf("%d", &ny);
        if (ny==-1)
        {
            print_r(mat,'@','.',0,0,0);
            continue;
        }
        else if (ny<=-2)
            break;
        if (nx<l && ny<h)
            mat->rack[nx][ny]=1;
        else
            printf("Grid size: %dx%d, change it with -s option\n",l,h);
    }
}

char locals (MAT *mat, int x, int y)
{
    char ul,um,ur,ml,mr,bl,bm,br;
    int l= mat->l -1;
    int h= mat->h -1;
    if (y<=0 || x<=0) ul=0;
    else ul= mat->rack[x-1][y-1];
    if (y<=0) um=0;
    else um= mat->rack[x-0][y-1];
    if (y<=0 || x>=l) ur=0;
    else ur= mat->rack[x+1][y-1];
    if (x<=0) ml=0;
    else ml= mat->rack[x-1][y-0];
    if (x>=l) mr=0;
    else mr= mat->rack[x+1][y-0];
    if (y>=h || x<=0) bl=0;
    else bl= mat->rack[x-1][y+1];
    if (y>=h) bm=0;
    else bm= mat->rack[x-0][y+1];
    if (y>=h || x>=l) br=0;
    else br= mat->rack[x+1][y+1];
    return ul+um+ur+ml+mr+bl+bm+br;
}

void make_r (MAT *mat, int l, int h)
{
    mat->rack= malloc(l * sizeof(char*));
    if (mat->rack == NULL)
        exit(1);
    for (int x=0; x<l; x++)
    {
        mat->rack[x]= malloc(h * sizeof(char*));
        if (mat->rack[x] == NULL)
            exit(1);
        for (int y=0; y<h; y++)
            mat->rack[x][y]= 0;
    }
    mat->l= l;
    mat->h= h;
}

void fill_r (MAT *mat, char prob)
{
    int l= mat->l;
    int h= mat->h;
    srand(time(NULL));
    for (int x=0; x<l; x++)
    {
        for (int y=0; y<h; y++)
        {
            char random= rand()%100;
            if (random<prob)
                mat->rack[x][y]=1;
        }
    }
}

void print_r (MAT *mat, char a, char d, char delim_z, char delim_v, char delim_c)
{
    int l= mat->l;
    int h= mat->h;
    if (delim_c)
        printf("%c",delim_c);
    if (delim_z)
        for (int i=0; i<=l*2; i++)
            printf("%c",delim_z);
    if (delim_c)
        printf("%c\n",delim_c);
    for (int y=0; y<h; y++)
    {
        if (delim_v)
            printf("%c ",delim_v);
        for (int x=0; x<l; x++)
        {
            if (mat->rack[x][y])
                printf("%c",a);
            else
                printf("%c",d);
            printf(" ");
        }
        if (delim_v)
            printf("%c",delim_v);
        printf("\n");
    }
    if (delim_c)
        printf("%c",delim_c);
    if (delim_z)
        for (int i=0; i<=l*2; i++)
            printf("%c",delim_z);
    if (delim_c)
        printf("%c\n",delim_c);
}
