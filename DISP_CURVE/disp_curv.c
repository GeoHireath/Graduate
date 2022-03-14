#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

float vel_mod[80][400][200];
double vel[100][400][200];
float longitude[400],latitude[200];
#define nst 510

void surfpath(double lat1,double lat2,double lon1,double lon2,float ddel,int *npt,double *ptlat,double *ptlon,double *distance);
float geog_to_geoc(float lat);
float geoc_to_geog(float lat);
float ttime(int k, double *ptlat,double *ptlon,int npt);

main(){
	double lat1,lon1,lat2,lon2;
	double ptlat[100000],ptlon[100000];
	double distance;
	float ddel;
	int npt;
	int ilat,ilon;
	char f_name[80][30];
	char stnm[nst][6];
	char str[50];
	float stlon[nst],stlat[nst];
	float prd[80];
	float ave_vel;
	float lat;
	int nstation;
	int nprd;
	int nout;
	float outprd[100];
	int i,j,k;
	int iprd;
	FILE *fp1;
	FILE *input;
	FILE *v_mod;
	FILE *output;
	FILE *temp;
	FILE *ff;

//set interval
	ddel=0.01;

// read in station information, same format as in sa_from_seed_holes.c
	if((ff=fopen("station.lst","r"))==NULL){
		fprintf(stderr,"Cannot open station file station.lst\n");
		exit (-1);
	}
	nstation=0;
	while(fscanf(ff,"%s%f%f",stnm[nstation],&stlon[nstation],&stlat[nstation])!=EOF){
		nstation=nstation+1;
	}
	fclose(ff);
	nstation=nstation-1;

// read in desired period.
	if((ff=fopen("period","r"))==NULL){
		fprintf(stderr,"Cannot open desired period file: period\n");
		exit(-1);
	}
	nout=0;
	while(fscanf(ff,"%f",&outprd[nout])!=EOF){
		nout=nout+1;
	}
	fclose(ff);
	nout=nout-1;


// read in global velocity file model.
// Format: filename period
	if((input=fopen("INPUT_MODEL","r"))==NULL) {
	    printf("Can not open input file: INPUT_MODEL\n");
	    exit(-1);
	    }
	nprd=0;
	while(fscanf(input,"%s%f",f_name[nprd],&prd[nprd])!=EOF){
		nprd=nprd+1;
	}
	fclose(input);
	nprd=nprd-1;


// read in global velocity model.
	for(i=0;i<=nprd;i++){
		if((v_mod=fopen(f_name[i],"r"))==NULL){
		     printf("Cannot open global velocity model file: %s\n",f_name[i]);
		     exit(-1);
		}
		for(ilat=0;ilat<=180;ilat++){
                     for(ilon=0;ilon<=359;ilon++){
                        fscanf(v_mod,"%f%f%f",&longitude[ilon],&latitude[ilat],&vel_mod[i][ilon][ilat]);
                   }
                   vel_mod[i][360][ilat]=vel_mod[i][0][ilat];
                }
            	longitude[360]=360.0;
            	fclose(v_mod);
	}
// prepare global velocity model for calculation. linear interpolation
//	output=fopen("tmp","w");
	for(i=0;i<=nout;i++){
		iprd=-1;
		for(j=0;j<=nprd-1;j++) {
		   	if (outprd[i]>=prd[j] && outprd[i]<=prd[j+1]){
			   iprd=j;	
			   break;
			}
		}
		if (iprd == -1) {
		   printf ("Cannot find period %f\n", outprd[i]);
		   exit(-1);
		}
//			printf("i=%d\n",iprd);
		for(ilat=0;ilat<=180;ilat++){
		    for(ilon=0;ilon<=360;ilon++){
			vel[i][ilon][ilat]=(vel_mod[iprd+1][ilon][ilat]-vel_mod[iprd][ilon][ilat])/(prd[iprd+1]-prd[iprd])*(outprd[i]-prd[iprd])+vel_mod[iprd][ilon][ilat];
//			fprintf(output,"%f  %f  %f\n",longitude[ilon],latitude[ilat],vel[i][ilon][ilat]);
		    }
		}
	}
//		fclose(output);
//now array vel contains the velocity model of all desired periods

	float eps=0.00001;
//loop to calculate all possible pairs
	for (i=0;i<=nstation-1;i++) {
		for (j=i+1;j<=nstation;j++) {
//check duplicated stations
		    if ((fabs(stlon[i]-stlon[j]))<eps && (fabs(stlat[i]-stlat[j])<eps)){
//		    printf ("%f,%f\n",abs((stlon[i]-stlon[j])),abs((stlat[i]-stlat[j])));
//			printf("%f\n",eps);
			printf("Warning: Duplicated stations %s,%s\n", stnm[i],stnm[j]);
			printf("skipped\n");
			continue;
		    }
		    sprintf(str,"COR_%s_%s.SAC_PRED\0", stnm[i],stnm[j]);
		    if ((output=fopen(str,"w"))==NULL){
			fprintf(stderr,"cannot create output: %s\n",str);
			exit(-1);
		    }
		    fprintf(output,"  %d %d %d %s %s %f %f %f %f\n",i+1,j+1,nout+1,stnm[i],stnm[j],stlat[i],stlon[i],stlat[j],stlon[j]);
		    lat1=geog_to_geoc(stlat[i]);
		    lat2=geog_to_geoc(stlat[j]);
		    lon1=stlon[i];
		    lon2=stlon[j];
	            surfpath(lat1,lat2,lon1,lon2,ddel,&npt,ptlat,ptlon,&distance);
		    for (k=0;k<=nout;k++){
			ave_vel=distance*6371.0*3.1415926/180.0/ttime(k,ptlat,ptlon,npt);
			fprintf(output,"    %5.1f    %6.3f\n", outprd[k], ave_vel);
		    }
		    fclose(output);
		}
	} 
}
