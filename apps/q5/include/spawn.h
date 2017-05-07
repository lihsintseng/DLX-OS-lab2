#ifndef __USERPROG__
#define __USERPROG__
//#define DEBUG
typedef unsigned char uint8;
typedef struct atom {
//  int numprocs;
//
  int totalN;
 // int curO;
  int totalO;
  
  int N;
  int O;
  int O2;
  int N2;
  int O3;
  int NO2;

  int AN; // Active NO2
  int WN; // Waiting NO2

  int WO1;
  int WO2;
  
  int temp;
  int end_flag;
  int endN_flag;
  int endO_flag;
  

} atom;

#define NREADY_TO_RUN "nReady.dlx.obj"
#define OREADY_TO_RUN "oReady.dlx.obj"
#define MAKE_O2 "makeO2.dlx.obj"
#define MAKE_N2 "makeN2.dlx.obj"
#define MAKE_O3 "makeO3.dlx.obj"
#define MAKE_NO2 "makeNO2.dlx.obj"


#endif
