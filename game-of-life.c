// Made by Miguel Bautista Pérez
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

typedef struct MAT
{
    unsigned h; //heigth
    unsigned l; //length
    char **rack;    //rack
} MAT;

void help (char*);
unsigned aint (char, char, char**, unsigned);
char achar (char, char, char**, char);
char conway (MAT*);
void get_cells (MAT*);
char locals (MAT*, unsigned, unsigned);
void make_r (MAT*, unsigned, unsigned);
void fill_r (MAT*, char);
void print_r (MAT*, char, char, char, char, char);

unsigned main (char argc, char **args)
{
	char reset[20];
    FILE *fp = popen ("tput reset", "r");
    int resetsz= fread (reset, 1, sizeof(reset), fp);
    pclose (fp);
	
	MAT mat;
	
	unsigned l=20, h=20;
	unsigned t=200;
	int i=-1;
	char r=33;
	char p=0;
	char ca='@', cd=' ', ch=0, cv=0, cc=0;
	
	for (char a=1; a<argc; a++)
	{
		if (args[a][0]=='-')
		{
			switch (args[a][1])
			{
				case 's': l=aint(a,argc,args,l); h=aint(a+1,argc,args,h); break;
				case 't': t=aint(a,argc,args,t); break;
				case 'r': r=aint(a,argc,args,r); break;
				case 'p': p=1; break;
				case 'i': i=aint(a,argc,args,i); break;
				case 'c': switch (args[a][2])
				{
					case 'a': ca=achar(a,argc,args,ca); break;
					case 'd': cd=achar(a,argc,args,cd); break;
					case 'h': ch=achar(a,argc,args,ch); break;
					case 'v': cv=achar(a,argc,args,cv); break;
					case 'c': cc=achar(a,argc,args,cc); break;
					default: ch='~'; cv='|'; cc='+';
				}
				break;
				case 'h': help(args[0]); return 0;
				default:
					printf("%s: incorrect option -- «%c»\n",args[0] , args[a][1]);
					printf("Use '%s -h' for more info.\n", args[0]);
					return 1;
			}
		}
	}
	unsigned iter=0;
	
	make_r(&mat, l, h);
	
	if (p)
	    get_cells(&mat);
	else
	    fill_r(&mat, r);
	    
	do {
	    write(1, reset, resetsz);
		print_r(&mat, ca,cd,ch,cv,cc);
		usleep(t*1000);
		iter++;
	}
	while (conway(&mat) && iter<i);
	
    if (i)
        printf("%d iterations realised.\n", iter);
    free(mat.rack);
    return 0;
}

void help (char * name)
{
	printf("Usage: %s [OPTION] [PARAMETER]...\n", name);
	printf("A customisable Conway's Game of Life for shell.\n");
	printf(" see <https://en.wikipedia.org/wiki/Conway's_Game_of_Life>\n\n");
	printf("Options:\n");
	printf(" -s [X][Y]	Set grid legth to X and heigth to Y\n");
	printf(" -t [N]		Set time between iterations to N miliseconds\n");
	printf(" -i [N]		N is the maximum number of iterations (0 to not show)\n");
	printf(" -r [N]		Spread random cells with N probability\n");
	printf(" -p		Start input cells mode, instead of spread them randomly\n");
	printf(" -c		Print delimiters\n");
	printf(" -ca [C]	Represent living cells with C\n");
	printf(" -cd [C]	Represent dead cells with C\n");
	printf(" -ch [C]	Print horizontal delimiter, representd with C\n");
	printf(" -cv [C]	Print vertical delimiter, representd with C\n");
	printf(" -cc [C]	Print corner delimiter, representd with C\n");
}

unsigned aint (char a, char argc, char ** args, unsigned p)
{
	a++;
	if (a<argc && args[a][0]!='-') return atoi(args[a]);
	else return p;
}

char achar (char a, char argc, char ** args, char def)
{
	a++;
	if (a<argc && args[a][0]!='-') return args[a][0];
	else return def;
}

char conway (MAT *mat)
{
	MAT gen;
	unsigned l= mat->l;
	unsigned h= mat->h;
	char activity=0;
	make_r(&gen, l, h);
	for (unsigned y=0; y<h; y++)
	{
		for (unsigned x=0; x<l; x++)
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

void get_cells (MAT *mat)
{
	unsigned c=0, nx, ny;
	unsigned l= mat->l;
	unsigned h= mat->h;
	printf("\e[1;1H\e[2J");
	printf("Input cells mode, input X and Y for each cell:\n");
	printf(" at any time: -1: Show status, -2: Stop input.\n\n");
	while (!0)
	{
		printf("X= ");
		scanf("%d", &nx);
		if (nx==-1)
		{
			print_r(mat,'@','.',0,0,0);
			continue;
		}
		else if (nx<=-2) break;
		printf("Y= ");
		scanf("%d", &ny);
		if (ny==-1)
		{
			print_r(mat,'@','.',0,0,0);
			continue;
		}
		else if (ny<=-2) break;
		if (nx<l && ny<h) mat->rack[nx][ny]=1;
		else printf("Grid size: %dx%d, change it with -s option\n",l,h);
	}
}

char locals (MAT *mat, unsigned x, unsigned y)
{
	char ul,um,ur,ml,mr,bl,bm,br;
	unsigned l= mat->l -1;
	unsigned h= mat->h -1;
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

void make_r (MAT *mat, unsigned l, unsigned h)
{
	mat->rack= malloc(l * sizeof(char*));
	if (mat->rack == NULL)
	    exit(1);
	for (unsigned x=0; x<l; x++)
	{
		mat->rack[x]= malloc(h * sizeof(char*));
		if (mat->rack[x] == NULL)
		    exit(1);
		for (unsigned y=0; y<h; y++)
		    mat->rack[x][y]= 0;
	}
	mat->l= l;
	mat->h= h;
}

void fill_r (MAT *mat, char prob)
{
	unsigned l= mat->l;
	unsigned h= mat->h;
	srand(time(NULL));
	for (unsigned x=0; x<l; x++)
	{
		for (unsigned y=0; y<h; y++)
		{
			char random= rand()%100;
			if (random<prob)
			    mat->rack[x][y]=1;
		}
	}
}

void print_r (MAT *mat, char a, char d, char delim_z, char delim_v, char delim_c)
{
    unsigned l= mat->l;
	unsigned h= mat->h;
    if (delim_c)
	    printf("%c",delim_c);
	if (delim_z)
	    for (unsigned i=0; i<=l*2; i++)
	        printf("%c",delim_z);
	if (delim_c)
	    printf("%c\n",delim_c);
	for (unsigned y=0; y<h; y++)
	{
		if (delim_v)
		    printf("%c ",delim_v);
		for (unsigned x=0; x<l; x++)
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
	    for (unsigned i=0; i<=l*2; i++)
	        printf("%c",delim_z);
	if (delim_c)
	    printf("%c\n",delim_c);
}
