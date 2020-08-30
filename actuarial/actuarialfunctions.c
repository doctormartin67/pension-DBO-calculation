#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lifetables.h"
#include "actuarialfunctions.h"

static const double eps = 0.0000001;
double npx(char *lt, double ageX, double ageXn, int corr){ //lt = life table
  /* generally the rule is npx = lx(ageXn)/lx(ageX) but because lx has
     an integer value as its input we need to interpolate both these
     values
  */
  double ip1; //interpolation value 1
  double ip2; //interpolation value 2

  ageX += corr;
  ageXn += corr;

  ip1 = lx(lt, ageX) - (ageX - (int)ageX) * (lx(lt, ageX) - lx(lt, ageX + 1));
  ip2 = lx(lt, ageXn) - (ageXn - (int)ageXn) * (lx(lt, ageXn) - lx(lt, ageXn + 1));
  
  return ip2/ip1;
}

double nEx(char *lt, double i, double charge, double ageX, double ageXn, int corr){
  double im = (1+i)/(1+charge) - 1;
  double n = ageXn - ageX;
  double vn = 1/pow(1 + im, n);
  double nPx = npx(lt, ageX, ageXn, corr);
  return vn * nPx;
}

double axn(char *lt, double i, double charge, int prepost, int term,
	   double ageX, double ageXn, int corr){
  if (12 % term != 0) {
    printf("An incorrect term was chosen, payments are usually monthly (term = 12)\n");
    printf("but can also be yearly for example (term = 1). term should be divisible\n");
    printf("by 12 but \nterm = %d.\n", term);
    exit(0);
  }
  else if (ageX > ageXn) {
    printf("warning: ageXn = %.6f < ageX = %.6f\n", ageXn, ageX);
    printf("axn = 0 is taken in this case.\n");
    return 0;
  }
  else {
    // double im = pow(1 + i, 1.0/term) - 1; //=term interest rate (monthly: (1+i)^(1/12) - 1)
    // charge = pow(1 + charge, 1.0/term) - 1;
    double ageXk = ageX + (double)prepost/term; // current age in while loop
    int payments = (int)((ageXn - ageX) * term + eps);
    double value = 0; //return value;

    while (payments--) {
      value += nEx(lt, i, charge, ageX, ageXk, corr);
      ageXk += 1.0/term;
    }
    /* There is a final portion that needs to be added when the amount of payments don't consider
       fractions of age, for example if ageX = 40 and ageXn = 40,5 and term = 1. This would give 0 
       payments but we still need to add nEx/2 in this case.
       We also need to subtract one term from ageXk because the while loop adds one too many.
    */    
    ageXk -= 1.0/term * prepost;
    value /= term;
    value += (ageXn - ageXk) *
      nEx(lt, i, charge, ageX,
	  (double)((int)(ageXn*term + eps))/term + term*prepost, corr);
    return value;
  }
}

double Ax1n(char *lt, double i, double charge, double ageX, double ageXn, int corr){
  if (ageX > ageXn) {
    printf("warning: ageXn = %.6f < ageX = %.6f\n", ageXn, ageX);
    printf("Ax1n = 0 is taken in this case.\n");
    return 0;
  }
  else {
    double im = (1+i)/(1+charge) - 1;
    double v = 1/(1 + im);
    int payments = (int)(ageXn - ageX + eps);
    double value = 0; //return value;
    int k = 0;
    // nAx = v^(1/2)*1Qx + v^(1+1/2)*1Px*1q_{x+1} + ... + v^(n-1+1/2)*{n-1}_Px*1Q_{x+n-1}
    while (payments--) {
      value += pow(v, k + 1.0/2) *
	npx(lt, ageX, ageX + k, corr) *
	(1 - npx(lt, ageX + k, ageX + k + 1, corr));
      k++;
    }
    // below is the final fractional payment, for example 40 until 40.6 years old
    value += pow(v, k + 1.0/2) *
      npx(lt, ageX, ageX + k, corr) *
      (1 - npx(lt, ageX + k, ageXn, corr));
    return value;
  }
}

double IAx1n(char *lt, double i, double charge, double ageX, double ageXn, int corr){
  if (ageX > ageXn) {
    printf("warning: ageXn = %.6f < ageX = %.6f\n", ageXn, ageX);
    printf("Ax1n = 0 is taken in this case.\n");
    return 0;
  }
  else {
    int payments = (int)(ageXn - ageX + eps);
    double value = 0; //return value;
    int k = 1;
    // IAx1n = sum^{n-1}_k=1:k*1A_{x+k}*kEx
    while (payments--) {
      value += k * Ax1n(lt, i, charge, ageX + k, ageX + k + 1, corr) *
	nEx(lt, i, charge, ageX, ageX + k, corr);
      printf("Ax1n(%d) = %.6f\n", k, Ax1n(lt, i, charge, ageX + k, ageX + k + 1, corr));
      printf("nEx(%d) = %.6f\n", k, nEx(lt, i, charge, ageX, ageX + k, corr));
      k++;
    }
    // below is the final fractional payment, for example 40 until 40.6 years old
    value += 0;
    return value;
  }
}
