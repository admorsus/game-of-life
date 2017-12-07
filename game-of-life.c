// Made by Miguel Bautista Pérez
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

typedef struct MAT
{
    int h; //heigth
    int l; //length
    char **grid;    //grid
} MAT;

void show_help (char*);
char conway (MAT*);
void input_mode (MAT*);
char neighbors (MAT*, int, int);
void init_grid (MAT*, int, int);
void fill_grid (MAT*, char);
void print_grid (MAT*, char, char, char, char, char);


int main (char argc, char **argv)
{
    //Variable definitions
    char verbosity= 1;
    int length= 20;
    int heigth= 20;
    int time= 200;
    int iter= 0;
    int maxIter= 0;  //0 equals infinite
    char appProb= 33;
    char inputMode= 0;
    char showAllDel= 0;
    char cAlive= '@', cDead= ' ';
    char delHoriz= 0, delVert= 0, delCorner= 0;
    char *commTput= "tput reset";
    
    //Argument parsing
    const char *opts= "hqsplx:y:t:i:r:a:d:z:v:c:";
    int opt;
    while ((opt= getopt(argc, argv, opts)) != -1)
    {
        switch (opt) {
            case 'h':
                show_help(argv[0]);
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
            case 's':
                commTput= "tput clear";
                break;
            case 'q':
                verbosity= 0;
                break;
            default:
                printf("Try '%s -h' for more information.\n", argv[0]);
                return 1;
        }
    }
    //Creates a string to clear the terminal
    char txtReset[20];
    FILE *fp = popen (commTput, "r");
    int szReset= fread (txtReset, 1, sizeof(txtReset), fp);
    pclose (fp);
    
    //Action strats here
    MAT mat;
    init_grid(&mat, length, heigth);
    
    if (inputMode)
    {
        write(1, txtReset, szReset);
        input_mode(&mat);
    }
    else
        fill_grid(&mat, appProb);
        
    if (showAllDel)
    {
        if (!delHoriz) delHoriz= '~';
        if (!delVert) delVert= '|';
        if (!delCorner) delCorner= '+';
    }
    
    //Main loop  
    do
    {
        write(1, txtReset, szReset);
        print_grid(&mat, cAlive, cDead, delHoriz, delVert, delCorner);
        usleep(time *1000);
        iter++;
    } while (conway(&mat) && iter!=maxIter);
    
    if (verbosity)
        printf("%d iterations realised.\n", iter);
    free(mat.grid);
    return 0;
}


void show_help (char *command)
{
    printf("Usage: %s [OPTION] [PARAMETER] [...]\n"
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
        " -s         Allows scrolling\n"
        " -q         Do not show final message\n"
        " -h         Show this message and exits\n", command);
    exit(0);
}


char conway (MAT *mat)
{
    int l= mat->l;
    int h= mat->h;
    char activity=0;
    MAT gen;      //Gen is the next state of mat
    init_grid(&gen, l, h);
    
    for (int y=0; y<h; y++)
    {
        for (int x=0; x<l; x++)
        {
            char n= neighbors(mat, x, y);
            
            if (n==3)
                gen.grid[x][y]= 1;
            else if (n==2 && mat->grid[x][y])
                gen.grid[x][y]= 1;
            else
                gen.grid[x][y]= 0;
                
            if (mat->grid[x][y] != gen.grid[x][y])
                activity=1;
        }
    }
    *mat= gen;
    return activity;
}


void input_mode (MAT *mat)
{
    int l= mat->l;
    int h= mat->h;
    int x, y;
    printf("Input cells mode, input X and Y for each cell:\n"
        " at any time: -1: Show status, -2: Stop input.\n\n");
        
    for (int c=0;;)
    {
        printf("X= ");
        scanf("%d", &x);
        
        if (x==-1)
        {
            print_grid(mat, '@', '.', 0,0,0);
            continue;
        }
        else if (x<=-2)
            break;
        
        printf("Y= ");
        scanf("%d", &y);
        if (y==-1)
        {
            print_grid(mat, '@', '.', 0,0,0);
            continue;
        }
        else if (y<=-2)
            break;
            
        if (x<l && y<h)
            mat->grid[x][y]=1;
        else
            printf("Coord. outside grid. Grid size: %dx%d\n", l, h);
    }
}


char neighbors (MAT *mat, int x, int y)
{
    int l= mat->l -1;
    int h= mat->h -1;
    char ul,um,ur,ml,mr,bl,bm,br;
    if (y<=0 || x<=0) ul=0;
    else ul= mat->grid[x-1][y-1];
    
    if (y<=0) um=0;
    else um= mat->grid[x-0][y-1];
    
    if (y<=0 || x>=l) ur=0;
    else ur= mat->grid[x+1][y-1];
    
    if (x<=0) ml=0;
    else ml= mat->grid[x-1][y-0];
    
    if (x>=l) mr=0;
    else mr= mat->grid[x+1][y-0];
    
    if (y>=h || x<=0) bl=0;
    else bl= mat->grid[x-1][y+1];
    
    if (y>=h) bm=0;
    else bm= mat->grid[x-0][y+1];
    
    if (y>=h || x>=l) br=0;
    else br= mat->grid[x+1][y+1];
    
    return ul+um+ur+ml+mr+bl+bm+br;
}


void init_grid (MAT *mat, int l, int h)
{
    mat->grid= malloc(l * sizeof(char*));
    if (mat->grid == NULL)
        exit(1);
        
    for (int x=0; x<l; x++)
    {
        mat->grid[x]= malloc(h * sizeof(char*));
        if (mat->grid[x] == NULL)
            exit(1);
            
        for (int y=0; y<h; y++)
            mat->grid[x][y]= 0;
    }
    mat->l= l;
    mat->h= h;
}


void fill_grid (MAT *mat, char prob)
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
                mat->grid[x][y]= 1;
        }
    }
}


void print_grid (MAT *mat, char a, char d, char dHor, char dVer, char dCor)
{
    int l= mat->l;
    int h= mat->h;
    
    //First line deliniter
    if (dCor)
        printf("%c",dCor);
    if (dHor)
        for (int i=0; i<=l*2; i++)
            printf("%c",dHor);
    if (dCor)
        printf("%c\n",dCor);
    
    //Body
    for (int y=0; y<h; y++)
    {
        if (dVer)
            printf("%c ",dVer);
        for (int x=0; x<l; x++)
        {
            if (mat->grid[x][y])
                printf("%c",a);
            else
                printf("%c",d);
            printf(" ");
        }
        if (dVer)
            printf("%c",dVer);
        printf("\n");
    }
    
    //Last line delimiter
    if (dCor)
        printf("%c",dCor);
    if (dHor)
        for (int i=0; i<=l*2; i++)
            printf("%c",dHor);
    if (dCor)
        printf("%c\n",dCor);
}

// Made with <3