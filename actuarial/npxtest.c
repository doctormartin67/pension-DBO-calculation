#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "actuarialfunctions.h"

static void setvars(int, char **);
static char *func;
static char *lt;
static double i, charge, ageX, ageXn;
static int corr;

int main(int argc, char *argv[]){
  if (argc < 4){
    printf("incorrect input\n");
    printf("syntax: ./npx table ageX ageXn corr\n");
    printf("If corr is omitted then 0 is taken by default.\n");
  }
  else{
    setvars(argc, argv);
    printf("%.8f\n", npx(lt, ageX, ageXn, corr));
  }
  return 0;
}

void setvars(int argc, char *argv[]){
  lt = *(argv+1);
  ageX = atof(*(argv+2));
  ageXn = atof(*(argv+3));
  corr = (argc == 5 ? atoi(*(argv+4)) : 0);
}