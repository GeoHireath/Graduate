#include <stdio.h>
#include <math.h>

extern double vel[100][400][200];
extern float longitude[400],latitude[200];
double velocity(int k, double lon,double lat);

float ttime(int k,double *ptlat,double *ptlon,int npt)
{
	int nseg;
	int i;
	double pi=3.14165926;
	double the1,phe1,the2,phe2;
	double x1,x2,y1,y2,z1,z2;
	double dis;
	double rv1,rv2,sm;
	float tt;

	nseg=npt-1;
	tt=0.0;
	rv1=1.0/velocity(k,ptlon[0],ptlat[0]);
	for (i=0;i<=nseg;i++) {
//calculate the distance between every two succesive points
	    the1=(90.0-ptlat[i])*pi/180.0;
	    phe1=ptlon[i]*pi/180.0;
	    the2=(90.0-ptlat[i+1])*pi/180.0;
	    phe2=ptlon[i+1]*pi/180.0;

	    x1=sin(the1)*cos(phe1);
	    y1=sin(the1)*sin(phe1);
	    z1=cos(the1);
	    x2=sin(the2)*cos(phe2);
	    y2=sin(the2)*sin(phe2);
	    z2=cos(the2);

	    dis=acos(x1*x2+y1*y2+z1*z2);    
	    dis=dis*6371.0;
	    rv2=1/velocity(k,ptlon[i+1],ptlat[i+1]);
	    sm=(rv1+rv2)/2;
	    tt=tt+dis*sm;
	    rv1=rv2;
	    }
//	printf("tt=%f\n",tt);
	return tt;
	}



float geoc_to_geog(float lat);

double velocity(int k,double lon,double lat)
{
	int ip,jp,ip1,jp1;
	float interval=1.0;
	float lonf,latf,lonf1,latf1;
	float wv11,wv12,wv21,wv22;
	double v;
	
	lat=geoc_to_geog(lat);
	ip=(int)(lon/interval);
	jp=(int)((lat+90.0)/interval);
/*      printf (" lon= %f lat= %f\n", lon, lat);
        printf (" ip= %d jp= %d longitude= %f latitude = %f \n", ip, jp, longitude[ip], latitude[jp]);
i*/
	ip1=ip+1;
	jp1=jp+1;
	lonf=(lon-longitude[ip])/interval;
	latf=(lat-latitude[jp])/interval;
	lonf1=1.0-lonf;
	latf1=1.0-latf;	
//      printf("%f %f\n", lonf,latf);

	wv11=lonf1*latf1;
	wv21=lonf*latf1;
	wv12=lonf1*latf;
	wv22=lonf*latf;
	v=wv11*vel[k][ip][jp]+wv21*vel[k][ip1][jp]+wv12*vel[k][ip][jp1]+wv22*vel[k][ip1][jp1];
	return v;
}
