#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#define byte char
#define NAME "gl"

int aint		(byte, byte, char**, int);
char achar		(byte, byte, char**, char);
int ** maker	(int, int);
int ** fillr	(int, int, int**, byte);
int ** cornway	(int, int, int**);
int ** getcells	(int, int, int**);
void printr		(int, int, int**, char, char, char, char, char);
byte locals		(int, int, int**, int, int);
void help		(char*);
static byte activity=1;

int main (byte argc, char ** args)
{
	int l=20, h=20;
	int t=200;
	int i=-1;
	byte r=33;
	byte p=0;
	char ca='@', cd=' ', ch=0, cv=0, cc=0;
	
	for (byte a=1; a<argc; a++)
	{
		if (args[a][0]=='-')
		{
			switch (args[a][1])
			{
				case 's': l=aint(a,argc,args,l); h=aint(a+1,argc,args,h); break;
				case 't': t=aint(a,argc,args,t); break;
				case 'r': r=aint(a,argc,args,r); break;
				case 'p': p=1; break;
				case 'i': i=aint(a,argc,args,i);
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
	int iter=0;
	int ** rack=maker(l, h);
	if (p)	rack=getcells(l,h,rack);
	else	rack=fillr(l,h,rack,r);
	do
	{
		activity=0;
		printf("\e[1;1H\e[2J");
		printr(l,h,rack,ca,cd,ch,cv,cc);
		rack=cornway(l, h, rack);
		iter++;
		usleep(t*1000);
	}
	while (activity && iter!=i);
	if (i) printf("%d iterations realised.\n", iter);
	free(rack);
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

int aint (byte a, byte argc, char ** args, int p)
{
	a++;
	if (a<argc && args[a][0]!='-') return atoi(args[a]);
	else return p;
}

char achar (byte a, byte argc, char ** args, char def)
{
	a++;
	if (a<argc && args[a][0]!='-') return args[a][0];
	else return def;
}

int ** cornway (int l, int h, int ** rack)
{
	int ** newr=maker(l, h);
	for (int y=0; y<h; y++)
	{
		for (int x=0; x<l; x++)
		{
			byte n=locals(l,h,rack,x,y);
			if (rack[x][y] && n==2) newr[x][y]=1;
			else if (n==3) newr[x][y]=1;
			else newr[x][y]=0;
			if (rack[x][y]!=newr[x][y]) activity=1;
		}
	}
	return newr;
}

int ** getcells (int l, int h, int ** rack)
{
	int c=0, nx, ny;
	printf("\e[1;1H\e[2J");
	printf("Input cells mode, input X and Y for each cell:\n");
	printf(" at any time: -1: Show status, -2: Stop input.\n\n");
	while (!0)
	{
		printf("X= ");
		scanf("%d", &nx);
		if (nx==-1)
		{
			printr(l,h,rack,'@','.',0,0,0);
			continue;
		}
		else if (nx<=-2) break;
		printf("Y= ");
		scanf("%d", &ny);
		if (ny==-1)
		{
			printr(l,h,rack,'@','.',0,0,0);
			continue;
		}
		else if (ny<=-2) break;
		if (l>nx && h>ny) rack[nx][ny]=1;
		else printf("Grid size: %dx%d, change it with -s option\n",l,h);
	}
	return rack;
}

byte locals (int l, int h, int ** rack, int x, int y)
{
	byte ul,um,ur,ml,mr,bl,bm,br;
	l--; h--;
	if (y<=0 || x<=0)	ul=0; else ul=rack[x-1][y-1];
	if (y<=0)			um=0; else um=rack[x-0][y-1];
	if (y<=0 || x>=l)	ur=0; else ur=rack[x+1][y-1];
	if		   (x<=0)	ml=0; else ml=rack[x-1][y-0];
	if		   (x>=l)	mr=0; else mr=rack[x+1][y-0];
	if (y>=h || x<=0)	bl=0; else bl=rack[x-1][y+1];
	if (y>=h)			bm=0; else bm=rack[x-0][y+1];
	if (y>=h || x>=l)	br=0; else br=rack[x+1][y+1];
	return ul+um+ur+ml+mr+bl+bm+br;
}

int ** maker (int l, int h)
{
	int ** rack=malloc(l*sizeof(int**));
	if (rack==NULL) exit(1);
	for (int x=0; x<l; x++)
	{
		rack[x]=malloc(h*sizeof(int*));
		if (rack[x]==NULL) exit(1);
		for (int y=0; y<h; y++)
		{
			rack[x][y]=0;
		}
	}
	return rack;
}

int ** fillr (int l, int h, int ** rack, byte prob)
{
	int seed=time(NULL);
	for (int x=0; x<l; x++)
	{
		for (int y=0; y<h; y++)
		{
			srand(seed+(x*x*y+y+x/2));
			byte random=rand()%100;
			if (random<prob) rack[x][y]=1;
		}
	}
	return rack;
}

void printr (int l, int h, int ** rack, char a, char d, char delim_z, char delim_v, char delim_c)
{
	printf("%c",delim_c);
	if (delim_z) for (int i=0; i<=l*2; i++) printf("%c",delim_z);
	if (delim_c) printf("%c\n",delim_c);
	for (int y=0; y<h; y++)
	{
		if (delim_v) printf("%c ",delim_v);
		for (int x=0; x<l; x++)
		{
			if (rack[x][y]) printf("%c",a);
			else printf("%c",d);
			printf(" ");
		}
		if (delim_v) printf("%c",delim_v);
		printf("\n");
	}
	if (delim_c) printf("%c",delim_c);
	if (delim_z) for (int i=0; i<=l*2; i++) printf("%c",delim_z);
	if (delim_c) printf("%c\n",delim_c);
}
