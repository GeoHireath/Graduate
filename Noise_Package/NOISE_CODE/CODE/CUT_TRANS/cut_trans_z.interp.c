#define MAIN
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mysac.h"
#include "sac_db.h"


/* The program takes the sac files from sa_from_seed_holes, removes the instrument response and cuts the file based on the user input parameters [t1] and [npts]. The program is hard-wired for a broadband signal, in the period band 5-150 s, but this can be changed in the one_rec_trans function. */


/* FUNCTION PROTOTYPES */
SAC_HD *read_sac (char *fname, float *sig, SAC_HD *SHD, int nmax);
void write_sac (char *fname, float *sig, SAC_HD *SHD);
void one_rec_trans( SAC_DB *sd, int ne, int ns);
void one_rec_cut(SAC_DB *sd, int ne, int ns, float t1, float n);


/*--------------------------------------------------------------------------*/
SAC_HD *read_sac (char *fname, float *sig, SAC_HD *SHD, int nmax)
/*--------------------------------------------------------------------------*/
/* function to read sac files given the name, fname. The function outputs the time signal to the pointer sig, fills the header SHD, if the signal has fewer than nmax points */
{
  FILE *fsac;

  if((fsac = fopen(fname, "rb")) == NULL) {
    printf("could not open sac file to read%s \n", fname);
    exit(1);
  }

  if ( !fsac ) {
    /*fprintf(stderr,"file %s not found\n", fname);*/
    return NULL;
  }

  if ( !SHD ) SHD = &SAC_HEADER;

  fread(SHD,sizeof(SAC_HD),1,fsac);

  if ( SHD->npts > nmax ) {
    fprintf(stderr,"ATTENTION !!! dans le fichier %s npts est limite a %d",fname,nmax);
    SHD->npts = nmax;
  }

  fread(sig,sizeof(float),(int)(SHD->npts),fsac);
  fclose (fsac);

/*-------------  calcule de t0  ----------------*/
   {
     int eh, em ,i;
     float fes;
     char koo[9];

     for ( i = 0; i < 8; i++ ) {
       koo[i] = SHD->ko[i];
     }
     koo[8] = NULL;

     SHD->o = SHD->b + SHD->nzhour*3600. + SHD->nzmin*60 +
     SHD->nzsec + SHD->nzmsec*.001;

     sscanf(koo,"%d%*[^0123456789]%d%*[^.0123456789]%g",&eh,&em,&fes);

     SHD->o  -= (eh*3600. + em*60. + fes);
   /*-------------------------------------------*/}

   return SHD;
}


/*--------------------------------------------------------------------------*/
void write_sac (char *fname, float *sig, SAC_HD *SHD)
/*--------------------------------------------------------------------------*/
{
  FILE *fsac;
  int i;
  if((fsac = fopen(fname, "wb"))==NULL) {
    printf("could not open sac file to write\n");
    exit(1);
  }

  if ( !SHD ) {
    SHD = &SAC_HEADER;
  }

  SHD->iftype = (int)ITIME;
  SHD->leven = (int)TRUE;
  SHD->lovrok = (int)TRUE;
  SHD->internal4 = 6L;
  SHD->depmin = sig[0];
  SHD->depmax = sig[0];

  for ( i = 0; i < SHD->npts ; i++ ) {
    if ( SHD->depmin > sig[i] ) {
      SHD->depmin = sig[i];
    }
    if ( SHD->depmax < sig[i] ) {
      SHD->depmax = sig[i];
    }
   }

  fwrite(SHD,sizeof(SAC_HD),1,fsac);
  fwrite(sig,sizeof(float),(int)(SHD->npts),fsac);

  fclose (fsac);
}


  float sig1[10000000]; 
  float sig2[400000000];
/*--------------------------------------------------------------------------*/
void one_rec_cut(SAC_DB *sd, int ne, int ns, float t1, float n)
/*--------------------------------------------------------------------------*/
{
  double t1b, t1e, t2b, t2e, t2;
  int n1, n2;
  char ft_name[100];
  SAC_HD shd1;
  int npts;
  int i,j,k;
  int nsamples;

  t2 = t1 + (n-1)*sd->rec[ne][ns].dt;

//  printf("%lg    %lg\n",sd->rec[ne][ns].t0,sd->ev[ne].t0);
//  printf("%f\n",sd->rec[ne][ns].dt);
//  printf("%d\n",sd->rec[ne][ns].n);

  t1b = sd->rec[ne][ns].t0 - sd->ev[ne].t0;
  t1e = t1b + (sd->rec[ne][ns].n-1)*sd->rec[ne][ns].dt;

  fprintf(stderr,"t1 %lg  t2 %lg   t1b %lg  t1e %lg\n", t1, t2, t1b, t1e);

  if ( (t1b>t1) || (t1e<t2) ) {
    fprintf(stderr,"incompatible time limits for station %s and event %s\n", sd->st[ns].name, sd->ev[ne].name );
//    exit(-1);
    return;
  }

  if ( !read_sac ("s1.sac", sig1, &shd1, 10000000 ) ) {
    fprintf(stderr,"file %s not found\n", sd->rec[ne][ns].fname );
    return;
  }

// start interpolation
  npts=sd->rec[ne][ns].n;
//  nsamples=((double) (sd->rec[ne][ns].dt))/((double) 0.001);
  nsamples=((double) (sd->rec[ne][ns].dt))/((double) 0.001)+0.5; //xia_han 2014_7_14
  printf("%d\n",nsamples);
  k=0;
//  FILE *tmp;
//  ff = fopen("tmp1","w");
  for (i=0;i<=npts-2;i++) {
      for (j=0;j<=nsamples-1;j++) {
          sig2[k+j]=((sig1[i+1]-sig1[i]))/(double)(nsamples)*((double)j)+sig1[i];
//          fprintf(tmp,"%f,%ld,%d\n",sig2[k+j],k+j,i);
      }
      k=k+nsamples;
  }
  sig2[k]=sig1[npts-1];
//  fprintf(tmp,"%f,%ld\n",sig2[k],k);
//  fclose (tmp);
// finish interpolation
//  printf("%lf\n",(t1-t1b)*1000.0);
    n1 = (int)((t1-t1b)*1000.0);  //xia_han 2014_7_14
//  n1 = (int)((t1-t1b)*nsamples);//xia_han 2014_7_12
//  n1 = (int)((t1-t1b)/sd->rec[ne][ns].dt);
  printf("%ld\n",n1);
//  char temp[1];
//  gets(temp);

//WARNING: rewriting array sig1
  
  for (i=0;i<=n-1;i++) {
     sig1[i]=sig2[n1+i*nsamples];

  }
  shd1.npts = n;
  shd1.nzyear = 2000;
  shd1.nzjday = 1;
  shd1.nzhour = 0;
  shd1.nzmin = 0;
  shd1.nzsec = 0;
  shd1.nzmsec = 0;
  shd1.b = 0.;

  strcpy(ft_name, sd->rec[ne][ns].ft_fname);
  write_sac (ft_name, &(sig1[0]), &shd1 );
//  write_sac (ft_name, &(sig1[n1]), &shd1 );
//  system("cp s1.sac s1.sac_tmp");
  system("/bin/rm s1.sac");
}


/*--------------------------------------------------------------------------*/
void one_rec_trans( SAC_DB *sd, int ne, int ns)
/*--------------------------------------------------------------------------*/
{
  FILE *ff;
  float fl1, fl2, fl3, fl4;
  int n1, n2;
  char str[300];

/* ASSUME THAT THE DATA ARE WITHIN THE FOLLOWING FILTER BAND */
  fl1=1.0/180;		/* currently set for 5-150 s period band */
  fl2=1.0/150;
//  fl1=1.0/150.0;		/* currently set for 5-150 s period band */
//  fl2=1.0/120.0;
//  fl3=1.0/0.3;
//  fl4=1.0/0.2;
  fl3=1.0/3.0;
  fl4=1.0/2.0;
  if ( (fl1<=0.)||(fl2<=0.)||(fl3<=0.)||(fl4<=0.)||(fl1>=fl2)||(fl2>=fl3)||(fl3>=fl4) ) {
    fprintf(stderr,"Error with frequency limits for transfer from evalresp.\n");
    exit(1);
  }
  else {
    fprintf(stderr,"Frequency limits: %f %f %f %f\n", fl1, fl2, fl3, fl4);
  }

  if ( ne >= sd->nev ) return;
  if ( ns >= sd->nst  ) return;
  if ( sd->rec[ne][ns].n <= 0 ) return;

  sprintf(str,"/bin/cp %s s1.sac\0", sd->rec[ne][ns].fname ); 
  system(str);

/* GENERATE TEMPORARY FILES */
  sprintf(str,"/bin/cp %s resp1\0", sd->rec[ne][ns].resp_fname );
  system(str);

  ff = fopen("sac_bp_respcor","w");
//  fprintf(ff,"/home/nshapiro/PROGS/SAC/bin/sac2000 << END\n");
   //fprintf(ff,"/opt/sac100/sac/bin/sac  << END\n");
  fprintf(ff,"sac  << END\n");
  fprintf(ff,"r s1.sac\n");
  fprintf(ff,"rmean\n");
  fprintf(ff,"rtrend\n");
  fprintf(ff,"transfer from evalresp fname resp1 to vel freqlimits %f %f %f %f\n", fl1, fl2, fl3, fl4 );

  //fprintf(ff,"transfer from evalresp to none freqlimits %f %f %f %f\n", fl1, fl2, fl3, fl4 );

  fprintf(ff,"w s1.sac\n");
  fprintf(ff,"quit\n");
  fprintf(ff,"END\n");
  fclose(ff);

    system("sh sac_bp_respcor");
    system("/bin/rm resp1");

}


/*--------------------------------------------------------------------------*/
int main (int argc, char *argv[])
/*--------------------------------------------------------------------------*/
{
  FILE *ff;
  int ne, ns;
  float t1, npts;
  SAC_DB sdb;
  char str[300];

/* CHECK INPUT ARGUMENTS */
  if ( argc < 3 ) {
    fprintf(stderr,"USAGE: cut_trans [t1] [npts]\n");
    exit(1);
  }
/*  t1=atof(argv[1]);
  npts=atof(argv[2]); */

  sscanf(argv[1],"%f",&t1);
  sscanf(argv[2],"%f",&npts);

fprintf(stderr,"t1-%f. npts-%f.\n", t1, npts);
//  fprintf(stderr,"The program assumes the results are within the 5-150 s period band.\n");

/* OPEN SAC DATABASE FILE AND READ IN TO MEMORY */
  if((ff = fopen("sac_db.out", "rb"))==NULL) {
    fprintf(stderr,"sac_db.out file not found\n");
    exit(1);
  }
  fread(&sdb, sizeof(SAC_DB), 1, ff);
  fclose(ff);
  printf("ev->%d  sta-> %d\n", sdb.nev,sdb.nst);

/* REMOVE INSTRUMENT RESPONSE AND CUT TO DESIRED LENGTH */
  for ( ns = 0; ns < sdb.nst; ns++ ) for ( ne = 0; ne < sdb.nev; ne++ ) {
   one_rec_trans( &sdb, ne, ns); 
fprintf(stderr,"back to main prog\n");
    one_rec_cut( &sdb, ne, ns, t1, npts);
  }

  return 0;
}
