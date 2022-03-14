/* This version use predicted velocity from global model to calculate arrival
   estimates. Then the time window is estimate+/-500 sec.
   Maximum signal picks are chosen within signal window. 
   Standard deviation outside signal window is calculated.
   Data larger than 2 sigma are thrown away. Sigma is recalcuated
   using the remainder. In this way most of the earthquake signals will be
   cleaned. The SNR is estimated by maximum pick devided by sigma. */


/* This version include Misha's filter for faster narrow-band filtering for larger data sets. The program is written for a 5-100s period band. Modify minP/maxP if changes are desired. This version does not write out the narrow-band filtered waveforms (as SAC files) */

/* This version is particularly designed for earthquake data.*/
/* 
   CURRENTLY SET TO USE THE MULTIPLICATION FACTOR FOR CALCULATING NARROW BAND FILTERS
 */

/* Zhen (James) Xu, March, 2010 */


#define MAIN
#include "mysac.h"
#include "sac_db.h"
#include "koftan.h"
#include "gl_const.h"
#include "mymacro.h"
#define SLEN 200000

/* FUNCTION PROTOTYPES */
SAC_HD *read_sac (char *fname, float *sig, SAC_HD *SHD, long nmax);
void write_sac (char *fname, float *sig, SAC_HD *SHD);
void filter4_(double *f1, double *f2, double *f3, double *f4, int *npow, double *dt, 
	      int *n, float seis_in[], float seis_out[]);
int get_snr(char *fname, char *pname);


/*--------------------------------------------------------------------------*/
SAC_HD *read_sac (char *fname, float *sig, SAC_HD *SHD, long nmax)
/*--------------------------------------------------------------------------*/
/* function to read sac files given the name, fname. The function outputs the time signal to the pointer sig, fills the header SHD, if the signal has fewer than nmax points */
{
  FILE *fsac;

  if((fsac = fopen(fname, "rb")) == NULL) {
    printf("could not open sac file to read%s \n", fname);
    exit(1);
  }

  if ( !fsac ) {
    /*fprintf(stderr,"file %s not find\n", fname);*/
    return NULL;
  }

  if ( !SHD ) SHD = &SAC_HEADER;

  fread(SHD,sizeof(SAC_HD),1,fsac);

  if ( SHD->npts > nmax ) {
    fprintf(stderr,"ATTENTION !!! %s npts is limited to %d.\n",fname,nmax);
    SHD->npts = nmax;
  }

  fread(sig,sizeof(float),(int)(SHD->npts),fsac);
  fclose (fsac);

/*-------------  calcule de t0  ----------------*/
   {
     int eh, em ,i;
     float fes;
     char koo[9];

     for ( i = 0; i < 8; i++ ) koo[i] = SHD->ko[i];
     koo[8] = '\0';

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

  SHD->iftype = (long)ITIME;
  SHD->leven = (long)TRUE;
  SHD->lovrok = (long)TRUE;
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


float sig0[SLEN];
char fname[300];
/*--------------------------------------------------------------*/
int get_snr(char *fname, char *pname)
/*--------------------------------------------------------------*/
{
  FILE *fp1, *fp2, *fd, *fsp,*fprd;
  int k,i, iswap, cnt, cnt2, *ih, *nsamples;
  int nf = 30;
  int npow;
  double f_cent;
  double p_cent;
  static int n;
  double dt, delta, multfac, addfac;
  double maxP= 160.0; 	/* for a 5-160s period band filtering */
  double minP= 5.0  ;
  double per[nf],f[nf],snr[nf];
  double b, e, fb, fe, step;
  double dist, minV, maxV, minT, maxT, signalmax, noisemax;
  double predv[200],prd[200];
  double f1, f2, f3, f4;
  double v_cent;
  static float seis_in[400000],seis_out[400000];
  float hed[158];
  char name[160], fname1[300], fname2[300];
  char buff[200];
  int flag;
  double tt;
  int minN,maxN;
  int window;
  double sum1;
  double sum2; 
  int count;
  double ave,sd;
  /* printf("fname is %s\n\n", fname); */
     

  if ( read_sac (fname, sig0, &SAC_HEADER, SLEN) == NULL )
    {
      fprintf(stderr,"File %s not found\n", fname);
      return 0;
    }

  b=SAC_HEADER.b;
  e=SAC_HEADER.e;
  dist = SAC_HEADER.dist;

  for (i=0;i<SLEN;i++) {
     sig0[i]=0;
  }


  fb=1.0/maxP;
  fe=1.0/minP;
  step=(log(fb)-log(fe))/(nf-1);

/* Calculate the frequency steps */
  for(k=0;k<nf;k++)
    {
      f[k]=exp(log(fe)+k*step);
      per[k]=1/f[k];
    }

  int nprd = 0;
  window = 0;
  minV = 1.5;
  maxV = 4.0;
  minT = dist/maxV-maxP;
  minN = (int)((minT-b)/SAC_HEADER.delta);
  maxT = dist/minV+2*maxP;
  maxN = (int)((maxT-b)/SAC_HEADER.delta);
  sprintf(fname1,"%s_snr.txt",fname);
  fp2=fopen(fname1,"w");
  if ((minN >= 0) && (maxN < SAC_HEADER.npts)) window = maxN-minN;
  if ((fprd=fopen(pname,"r"))==NULL) {
      printf("%s does not exist. Using velocity range 1.5 to 4.0 instead.\n",pname);
      if (window < 1) {
         for (i=1;i<nf-1;i++) {
            fprintf(fp2,"%lf %lf\n",per[i],0.0);
         }
         fclose(fp2);
         return 1;
      }
  } else {
    fgets(buff,200,fprd);
    while(fscanf(fprd,"%lf%lf",&prd[nprd],&predv[nprd])!=EOF) {
        nprd++;
    }
  }
  fclose(fprd);
//  printf("%d\n",nprd);





/* Call to Misha's filter, filter4.f */
/* ==========================================================
 * Parameters for filter4 function:
 * Input parameters:
 * f1,f2   - low corner frequences, f2 > f1, Hz, (double)
 * f3,f4   - high corner frequences, f4 > f3, Hz, (double)
 * npow    - power of cosine tapering,  (int)
 * dt      - sampling rate of seismogram in seconds, (double)
 * n       - number of input samples, (int)
 * seis_in - input array length of n, (float)
 * Output parameters:
 * seis_out - output array length of n, (float)
 * ==========================================================
 */

  cnt2=0;
  for(k=1;k<nf-1;k++)
    {

       f2=f[k+1];
       f3=f[k-1];
       f_cent = 2/(f2+f3); 
       p_cent=per[k];
       flag=0;
// find signal window
       for (i=0; i<nprd-1; i++) {
	   if ((p_cent >= prd[i]) && (p_cent <=prd[i+1])) {
              v_cent=predv[i]+(predv[i+1]-predv[i])/(prd[i+1]-prd[i])*(p_cent-prd[i]);
	      flag=1;
              break;
           }
       }
       if (flag == 0) {
          if (window < 1) {
             snr[k] = 0;
             continue;
          }
       } else {
         tt = dist/v_cent;
         if (tt < b || tt > e) {
            snr[k] = 0;
            continue;
         } 
         minT = tt-500.0;
         minN = (int)((minT-b)/SAC_HEADER.delta);
         maxT = tt+500.0;
         maxN = (int)((maxT-b)/SAC_HEADER.delta);
         if (minN < 0) minN = 0;
         if (maxN > (SAC_HEADER.npts-1)) {
//            maxN = SAC_HEADER.npts-1;
            snr[k] = 0;
            continue;
         }
         window = maxN-minN;
       }
//       printf("%lf %lf %f %f %f %d %d\n",p_cent,v_cent,SAC_HEADER.dist,minT,maxT,minN,maxN);
    
/*       if (((float)window/(float)(SAC_HEADER.npts)) > 0.5) {
          snr[k] = 0;
          continue;
       }
*/


       addfac= .01;
       f1=f2 - addfac;
       f4=f3 + addfac; 

/* read sac header and seismogram */
      iswap = 0;
      if((fd = fopen(fname,"r")) == NULL) {
        printf("Cannot find file %s.\n",fname);
      }
      fread(hed,sizeof(float),158,fd);
      ih = (int *)(&hed[76]);
      if(*ih > 100 || *ih < -100) iswap = 1;
      if(iswap) swapn((unsigned char *)hed,(int)(sizeof(float)),158);
      dt = hed[0];
      delta = hed[50];
      nsamples = (int *)(&hed[79]);
      fread(seis_in,sizeof(float),*nsamples,fd);
      fclose(fd);
      if(iswap) swapn((unsigned char *)seis_in,(int)(sizeof(float)),*nsamples);
      /* printf("Delta= %f, Dt= %f, Nsamples= %d\n",delta,dt,*nsamples); */
      n = *nsamples;

/* call to filter4, which ouputs the filtered seismogram as the seis_out array */
      npow=1;
      filter4_(&f1,&f2,&f3,&f4,&npow,&dt,&n,seis_in,seis_out);
  
      for(cnt=0; cnt<n; cnt++) {
        if(seis_out[cnt] > 0) {
          sig0[cnt]=seis_out[cnt];
        }
        else {
          sig0[cnt]=(-1)*seis_out[cnt];
        }
      }

/* write narrow-band filtered files - commmented out in the nowrt program */
      
/*      SAC_HEADER.t1 = minT;
      SAC_HEADER.t2 = maxT;
      sprintf(fname2,"%s_snr.txt_%d", fname, cnt2);
       printf("filtered file: %s\n", fname2);
      cnt2++;
      write_sac(fname2, seis_out, &SAC_HEADER); 
*/
/* loop over sig0 to find maxima within signal window */
      signalmax=0;
      noisemax=0;

      for(i= minN;i <= maxN;i++)
	{
	  if(sig0[i]>signalmax) 
	    signalmax=sig0[i];
	}

      sum1 = 0;
      sum2 = 0;
      count = 0;
      for (i = (int)(100.0/dt); i < minN; i++) {
          sum1 += seis_out[i];
          sum2 = sum2+seis_out[i]*seis_out[i];
          count++;
      }
      for (i = maxN+1; i < n; i++) { 
	  sum1 += seis_out[i];
          sum2 = sum2+seis_out[i]*seis_out[i];
          count++;
      }
      if (count < (int)(100.0/dt)) {
         snr[k] = 0;
         continue;
      }
      ave = sum1/(double)count;
      sd = sqrt((sum2 - (double)count*ave*ave)/((double)count-1.0));
      double rms;
      rms=sqrt(sum2/count);
//      printf("%d %f %f %f %f\n",count,signalmax,ave,sd,rms);
     
      sum1 = 0;
      sum2 = 0;
      count = 0;
      for (i = (int)(100.0/dt); i < minN; i++) {
          if (fabs(seis_out[i]) < 2.0*sd) {
             sum1 += seis_out[i];
             sum2 = sum2+seis_out[i]*seis_out[i];
             count++;
          }
      }
      for (i = maxN+1; i < n; i++) {
          if (fabs(seis_out[i]) < 2.0*sd) {
             sum1 += seis_out[i];
             sum2 = sum2+seis_out[i]*seis_out[i];
             count++;
          }
      }
      if (count < (int)(100.0/dt)) {
         snr[k] = 0;
         continue;
      }
//      printf("%d\n",count);
      ave = sum1/(double)count;
      sd = sqrt((sum2 - (double)count*ave*ave)/((double)count-1.0));

      snr[k]=signalmax/sd;      

    }

  for(i=1;i<nf-1;i++)
    {
      fprintf(fp2,"%lf %lf\n",per[i],snr[i]);
    }
  fclose(fp2);
//  printf("Here\n");
  return 1;
}


SAC_DB sdb;
/*--------------------------------------------------------------*/
int main (int argc, char *argv[])
/*--------------------------------------------------------------*/
{
  FILE *ff;
  char filename[30];
  char dirname[100];
  char predname[200];

//check input command line arguments
  if(argc != 3) {
    fprintf(stderr,"USAGE: spectral_snr_f [list file] [prediction directory]\n");
    fprintf(stderr,"Req.s a list of all cut files for spectral SNR values.\n");
    exit(-1);
  }

  if((ff = fopen(argv[1], "r"))==NULL) {
    fprintf(stderr,"Cannot open file list: %s\n", argv[1]);
    exit(1);
  }
  strcpy(dirname,argv[2]);
  int i;
  for(i=0;;i++)
    {
      if(fscanf(ff,"%s",&filename)==EOF)
	break;
//        printf("%s %d\n",filename,i);
        sprintf(predname,"%s/%s_PRED\0",dirname,filename);
        get_snr(filename,predname);
    }
    fclose(ff);
}
