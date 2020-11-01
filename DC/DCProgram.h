#ifndef DCPROGRAM
#define DCPROGRAM

#include "libraryheader.h"
#include "hashtable.h"

typedef struct dataset {
  int keyrow; /* find the row in the excel file where 
		 the keys are to use in the hashtable */
  char keycolumn[3];
  char **keys; // This points to the array of keys in excel
  char datasheet[256]; // This is the sheet where the data lies
  XLfile *xl;
  Hashtable ***Data; // This will be set using createData function below
  int membercnt;
		 
} DataSet;

//---Define BIT constants---
//-  status BITS  -
#define ACT 0x1 // used to set ACT bit on if active member
#define ACTCON 0x2// used to set ACTCON bit on if it is an active contract
#define SEX 0x4 // used to set SEX bit on if it is a male
#define MS 0x8 // used to set MS bit on if it is married
//-  tariff BITS  -
#define UKMS 0x1
#define UKZT 0x2
#define UKMT 0x4
#define MIXED 0x8

//-  extra BITS  -
#define INCSAL 01; // put this bit on when we increase the salary in the first line
#define CCRA 02; // put this bit on if this is a prepensioner

//---Define constants---
#define MAXPROJ 65 // years to calculate of one affiliate
#define MAXPROJBEFOREPROL 50 // years up to NRA, afterwards we prolongate to RA assumption
#define MAXGEN 8 // amount of generations of insurer
#define ER 0 // Employer index
#define EE 1 // Employee index

typedef struct currentmember {
  Hashtable **Data; //Data for an affiliate is in the form of a hashtable

  //---Variable Declarations---  
  char *key; // KEY in data
  char *regl; // REGLEMENT
  char *name; // NAME
  char *contract; // CONTRACT number
  unsigned short status; // 0000 0000 0000 0111 means male active member and active contract
  Date DOB; // date of birth
  Date DOE; // date of entry
  Date DOL; // date of leaving
  Date DOS; // date of situation
  Date DOA; // date of affiliation
  Date DOR; // date of retirement
  char *category; // for example blue collar, white collar, management, ...
  double sal; // salary
  double PG; // pensioengrondslag ( I have never needed this)
  double PT; // part time
  unsigned short NRA; // normal retirement age
  unsigned short kids; // amount of kids
  double KO; // death lump sum (kapitaal overlijden)
  double annINV; // annuity in case of invalidity
  double contrINV; // contribution for invalidity insurance
  double *ART24_A_GEN1; // WAP: art24 reserves exployer part before 01/01/2016
  double *ART24_C_GEN1; // WAP: art24 reserves exployer part before 01/01/2016
  double *ART24_A_GEN2; // WAP: art24 reserves exployer part before 01/01/2016
  double *ART24_C_GEN2; // WAP: art24 reserves exployer part before 01/01/2016
  double *CAP[2][MAXGEN]; // Pension lump sum (Employer-Employee, generations, loops)
  double *CAPPS[2][MAXGEN]; /* Pension lump sum profit sharing 
  				       (Employer-Employee, generations, loops)*/
  double *REDCAP[2][MAXGEN]; // Reduced lump sum (Employer-Employee, generations, loops)
  double TAUX[2][MAXGEN]; /* return guarentee insurer
  				      (Employer-Employee, generations, loops)*/
  double *PREMIUM[2][MAXGEN]; // Contribution (Employer-Employee, generations, loops)
  double *RES[2][MAXGEN]; // Reserves (Employer-Employee, generations, loops)
  double *DELTACAP[2][MAXGEN]; // Delta Cap (AXA) (Employer-Employee, generations, loops)
  double X10; // MIXED combination
  double *CAPDTH[2][MAXGEN]; /* Death lump sum (used for UKMT)
  					(Employer-Employee, generations, loops)*/
  double CAO; // collectieve arbeidsovereenkomst
  char *ORU;
  char *CHOICEDTH; // Choice of death insurance
  char *CHOICEINVS; // Choice of invalide insurance sickness
  char *CHOICEINVW; // Choice of invalide insurance work
  double contrDTH; // Death contributions
  double percSALKO; // percentage of salary for death lump sum
  char *indexINV; // indexation for invalidity
  char *GRDGR;
  char *plan;
  double baranc; // baremic ancienity
  unsigned short extra; /* 0000 0000 0000 0011
			   means prepensioner whose salary we increase at k = -1*/
  
} CurrentMember;

void setDSvals(XLfile *xl, DataSet *ds);
void setCMvals(DataSet *ds, CurrentMember *cm);
char *getcmval(CurrentMember *cm, char *value);

/* This function will allocate memory based on membercnt for the underlying
   Hashtable used for the data.*/
void createData(DataSet *ds);

/* used to find the row and sheet where the keys lie for the data to be used
   for calculations. If the word KEY isn't found in the data then
   1 is returned */
void setkey(DataSet *ds);
void countMembers(DataSet *ds);
int printresults(DataSet *ds);
#endif
