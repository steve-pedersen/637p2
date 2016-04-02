/*
  Sample code for CSc 635 Project 2

  compile:
  gcc -O cpan.c header.c anread.c -o cpan -lm

  run:
  cpan [.an] 

*/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"
#define P printf
#define N 9
int anread(char*, int);             /* 05/06 /96 */

/*    global variables declared as externs in monan.h need a root position  */
HEADER header;
int nhar, nhar1, nchans, npts;
float *cmag, *dfr, *phase, *br, *time, tl, dt, fa, smax, *newmag, *newfr,
  *finalfr;
double ampscale;

float *w, *a;
int   *b, LR[2], brk, frames;

int findBreak();
void findLR(int Nbk);
void interpolate(int L, int R);

int main(int argc, char **argv)
{
  int i,j,k,harms,Nbk;

/* read in an analysis file */
  anread(argv[1],-1);

  printf("# harmonics = %d # timepoints = %d\n",nhar, npts);
  //frames = npts;
  harms = 1;
  frames = 18; 
  if (argc == 3) 
    Nbk = atoi(argv[2]);
  else
    Nbk = N;
  // dummy data for "actual" points
  static const float DUMMY[] = {0,17,15,18,8,9,16,15,12,14,11,7,9,8,4,1,2,0};

  a = (float *) calloc(frames,sizeof(float));
  w = (float *) calloc(frames,sizeof(float));
  b = (int *) calloc(Nbk,sizeof(int));

  memcpy(a, DUMMY, sizeof(DUMMY));
  
  // save startpoint as the first break point
  for (i=1;i<Nbk;i++) { b[i]=-1; }
  b[0]=0; brk=0;

  // for each harmonic
  for (k=1;k<=harms;k++) {
    
    for (i=0;i<frames-1;i++) {
      w[i] = 0; // set all working points to zero
    }    
    //P("i%d %4d\n",0,b[0]);  
    for (i=1;i<Nbk;i++) {
      b[i] = findBreak();
      //P("i%d %4d\n",i,b[i]);
      findLR(Nbk);
      interpolate(LR[0], b[i]);
      interpolate(b[i], LR[1]);
    }
  }
  // for (i=0;i<Nbk;i++) {
  //   P("i%d %4d\n",i,b[i]);
  // }
  P("working amps\n");
  for (i=0;i<frames;i++) {
    P("w%.2d %4.2f\n",i,w[i]);
  }
  free(a); free(w); free(b);
}

int findBreak() {
  int i; float max=0;          
  
  for (i=1;i<frames;i++) {
    if (fabs(w[i]-a[i]) > max) {
      max = a[i];
      brk = i;
    }   
  }
  w[brk] = max;
  return brk;
}
 
void findLR (int Nbk) {
  int i, L=0, R=9999;
  for (i=0;i<Nbk;i++) {
    if ((i==Nbk-1) && (R==9999)) {
      R = frames-1;
    } else 
    if (b[i] < brk) {
      if (b[i] >= L) { L = b[i]; }
      //else if (b[i] < 0) { R = frames-1; }
    }
    if (b[i] > brk) {
      if (b[i] < R) { R = b[i]; }
      else if (b[i] < 0) { R = frames-1; }
    } 
  }
  LR[0] = L; LR[1] = R;
  //P("L=%d, brk=%d, R=%d\n",L,brk,R); 
  //P("brk=%d\n",b[brk]); 
}

void interpolate(int L, int R) {
  int i,x, x1=L, x2=R;
  float y1=w[x1], y2=w[x2];

  for (i=x1;i<=x2;i++) {
    x=i;
    w[i]=y1 + (y2-y1) * (x - x1)/(x2 - x1);
  }
}
