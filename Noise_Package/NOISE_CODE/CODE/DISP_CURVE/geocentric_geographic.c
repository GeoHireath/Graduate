#include <stdio.h>
#include <math.h>
#define RAD_PER_DEG     0.0174532925199432955

/*
#define B2A_SQ  0.99327733
#define B2A_SQ  0.9931177
*/
#define B2A_SQ  0.993305521
/*
  convert geographic latitude (degree) to geocentric latitude (degree)
*/
float geog_to_geoc(float lat)
{
        return(atan(B2A_SQ*tan(RAD_PER_DEG*lat))/RAD_PER_DEG);
}

/*
  convert geocentric latitude (degree) to geographic latitude (degree)
*/
float geoc_to_geog(float lat)
{
        return(atan(tan(RAD_PER_DEG*lat)/B2A_SQ)/RAD_PER_DEG);
}
