#include <stdio.h>
#include <math.h>
#include "wrapper.h"

extern void NGCALLF(dmapgci,DMAPGCI)(double *,double *,double *,double *,
                                     int *,double *,double *);

extern double NGCALLF(dgcdist,DGCDIST)(double *,double *,double *,double *,
                                       int *);

NhlErrorTypes gc_latlon_W( void )
{
/*
 * Input variables
 */
  void *lat1, *lon1, *lat2, *lon2;
  int npts, *nlatlon, nlatlon_new, *code;
  NclBasicDataTypes type_lat1, type_lon1, type_lat2, type_lon2;
  double *tmp_lat1, *tmp_lon1, *tmp_lat2, *tmp_lon2, *tmp_lat, *tmp_lon;
/*
 * Output variables.
 */
  void *lat, *lon;
  NclBasicDataTypes type_lon, type_lat, type_dist;
  float *rdist, *rspac;
  double *ddist, *dspac;
  int dsizes[1];
  NclQuark *units;
/*
 * Attribute variables
 */
  int att_id;
  NclMultiDValData att_md, return_md;
  NclVar tmp_var;
  NclStackEntry return_data;
/*
 * Other variables
 */
  int i, j;
/*
 * Retrieve parameters
 *
 * Note that any of the pointer parameters can be set to NULL,
 * which implies you don't care about its value.
 */
  lat1 = (void*)NclGetArgValue(
          0,
          6,
          NULL,
          NULL,
          NULL,
          NULL,
          &type_lat1,
          2);

  lon1 = (void*)NclGetArgValue(
          1,
          6,
          NULL,
          NULL,
          NULL,
          NULL,
          &type_lon1,
          2);

  lat2 = (void*)NclGetArgValue(
          2,
          6,
          NULL,
          NULL,
          NULL,
          NULL,
          &type_lat2,
          2);

  lon2 = (void*)NclGetArgValue(
          3,
          6,
          NULL,
          NULL,
          NULL,
          NULL,
          &type_lon2,
          2);

  nlatlon = (int*)NclGetArgValue(
          4,
          6,
          NULL,
          NULL,
          NULL,
          NULL,
          NULL,
          2);

  code = (int*)NclGetArgValue(
          5,
          6,
          NULL,
          NULL,
          NULL,
          NULL,
          NULL,
          2);

  if(*nlatlon <= 2) {
    npts        = 0;
    nlatlon_new = 2;
  }
  else {
    npts        = *nlatlon - 2;
    nlatlon_new = *nlatlon;
  }
/*
 * Coerce input to double if necessary.
 */
  tmp_lat1 = coerce_input_double(lat1,type_lat1,1,0,NULL,NULL);
  tmp_lon1 = coerce_input_double(lon1,type_lon1,1,0,NULL,NULL);
  tmp_lat2 = coerce_input_double(lat2,type_lat2,1,0,NULL,NULL);
  tmp_lon2 = coerce_input_double(lon2,type_lon2,1,0,NULL,NULL);

  if(tmp_lon1 == NULL || tmp_lat1 == NULL ||
     tmp_lon2 == NULL || tmp_lat2 == NULL) {
    NhlPError(NhlFATAL,NhlEUNKNOWN,"gc_latlon: Unable to coerce lat/lon arrays to double precision");
    return(NhlFATAL);
  }

  if(type_lat1 == NCL_double || type_lat2 == NCL_double) {
    type_lat = NCL_double;
  }
  else {
    type_lat = NCL_float;
  }

  if(type_lon1 == NCL_double || type_lon2 == NCL_double) {
    type_lon = NCL_double;
  }
  else {
    type_lon = NCL_float;
  }

/*
 * Allocate space for output arrays.
 */
  type_dist = NCL_float;
  if(type_lat == NCL_double) {
    type_dist = NCL_double;
    lat  = (void*)calloc(nlatlon_new,sizeof(double));
  }
  else {
    lat  = (void*)calloc(nlatlon_new,sizeof(float));
  }
  if(type_lon == NCL_double) {
    type_dist = NCL_double;
    lon  = (void*)calloc(nlatlon_new,sizeof(double));
  }
  else {
    lon  = (void*)calloc(nlatlon_new,sizeof(float));
  }
  if(lat == NULL || lon == NULL) {
    NhlPError(NhlFATAL,NhlEUNKNOWN,"gc_latlon: Unable to allocate memory for output arrays");
    return(NhlFATAL);
  }

/*
 * Allocate space for temporary output arrays.
 */
  ddist = (double*)calloc(1,sizeof(double));
  dspac = (double*)calloc(1,sizeof(double));
  if(ddist == NULL || dspac == NULL) {
    NhlPError(NhlFATAL,NhlEUNKNOWN,"gc_latlon: Unable to allocate memory for temporary output values");
    return(NhlFATAL);
  }
  if(npts > 0) {
    tmp_lat = (double*)calloc(npts,sizeof(double));
    tmp_lon = (double*)calloc(npts,sizeof(double));
    if(tmp_lat == NULL || tmp_lon == NULL) {
      NhlPError(NhlFATAL,NhlEUNKNOWN,"gc_latlon: Unable to allocate memory for temporary output arrays");
      return(NhlFATAL);
    }
/*
 * Call Fortran function.
 */
    NGCALLF(dmapgci,DMAPGCI)(tmp_lat1,tmp_lon1,tmp_lat2,tmp_lon2,&npts,
                             tmp_lat,tmp_lon);
  }
/*
 * Copy latitudes to output array. Start with second point of lat
 * array, since we copy begin/end lat points later.
 */
  if(npts > 0) {
    coerce_output_float_or_double(lat,tmp_lat,type_lat,npts,1);
  }
/*
 * Include the beginning and ending lat points.
 */
  coerce_output_float_or_double(lat,tmp_lat1,type_lat,1,0);
  coerce_output_float_or_double(lat,tmp_lat2,type_lat,1,nlatlon_new-1);
/*
 * Copy longitudes to output array.
 */
  if(npts > 0) {
    for(j = 0; j < npts; j++) {
/*
 * code >= 0 --> lon:    0 to 360
 * code <  0 --> lon: -180 to 180
 */
      if((*code >= 0 && tmp_lon[j] <    0.) ||
         (*code <  0 && tmp_lon[j] < -180.)) {
        tmp_lon[j] += 360.;
      }
      else if(*code < 0 && tmp_lon[j] > 180.) {
        tmp_lon[j] -= 360.;
      }
      coerce_output_float_or_double(lon,&tmp_lon[j],type_lon,1,j+1);
    }
  }
/*
 * Include the beginning lon points.
 */
  if((*code >= 0 && *tmp_lon1 <    0.) ||
     (*code <  0 && *tmp_lon1 < -180.)) {
    *tmp_lon1 += 360;
  }
  else if(*code < 0 && *tmp_lon1 > 180.) {
    *tmp_lon1 -= 360.;
  }
  coerce_output_float_or_double(lon,tmp_lon1,type_lon,1,0);
/*
 * Include the ending lon points.
 */
  if((*code >= 0 && *tmp_lon2 <    0.) ||
     (*code <  0 && *tmp_lon2 < -180.)) {
    *tmp_lon2 += 360;
  }
  else if(*code < 0. && *tmp_lon2 > 180.) {
    *tmp_lon2 -= 360.;
  }
  coerce_output_float_or_double(lon,tmp_lon2,type_lon,1,nlatlon_new-1);

  *code = abs(*code);
  *ddist = NGCALLF(dgcdist,DGCDIST)(tmp_lat1,tmp_lon1,tmp_lat2,tmp_lon2,code);
/*
 * Calculate the spacing between points.  If npts = 0, then the spacing
 * is just the distance between the two end points.
 */
  if(npts > 0) {
    *dspac = NGCALLF(dgcdist,DGCDIST)(&tmp_lat[0],&tmp_lon[0],
                                      &tmp_lat[1],&tmp_lon[1],code);
  }
  else {
    *dspac = *ddist;
  }
/*
 * Free tmp arrays.
 */
  if(npts > 0) {
    NclFree(tmp_lat);
    NclFree(tmp_lon);
  }
  if(type_lat1 != NCL_double) NclFree(tmp_lat1);
  if(type_lon1 != NCL_double) NclFree(tmp_lon1);
  if(type_lat2 != NCL_double) NclFree(tmp_lat2);
  if(type_lon2 != NCL_double) NclFree(tmp_lon2);
/*
 * Set up attributes to return.
 */
  att_id = _NclAttCreate(NULL,NULL,Ncl_Att,0,NULL);
/*
 * Set up variable to return.
 */
  dsizes[0] = 1;
  if(type_dist == NCL_float) {
    rdist  = (float*)calloc(1,sizeof(float));
    rspac  = (float*)calloc(1,sizeof(float));
    if(rdist == NULL || rspac == NULL) {
      NhlPError(NhlFATAL,NhlEUNKNOWN,"gc_latlon: Unable to allocate memory for output arrays");
      return(NhlFATAL);
    }
    *rdist = (float)*ddist;
    *rspac = (float)*dspac;
    NclFree(ddist);
    return_md = _NclCreateVal(
                              NULL,
                              NULL,
                              Ncl_MultiDValData,
                              0,
                              (void*)rdist,
                              NULL,
                              1,
                              dsizes,
                              TEMPORARY,
                              NULL,
                              (NclObjClass)nclTypefloatClass
                              );
    att_md = _NclCreateVal(
                           NULL,
                           NULL,
                           Ncl_MultiDValData,
                           0,
                           rspac,
                           NULL,
                           1,
                           dsizes,
                           TEMPORARY,
                           NULL,
                           (NclObjClass)nclTypefloatClass
                           );
  }
  else {
    return_md = _NclCreateVal(
                              NULL,
                              NULL,
                              Ncl_MultiDValData,
                              0,
                              (void*)ddist,
                              NULL,
                              1,
                              dsizes,
                              TEMPORARY,
                              NULL,
                              (NclObjClass)nclTypedoubleClass
                              );
    att_md = _NclCreateVal(
                           NULL,
                           NULL,
                           Ncl_MultiDValData,
                           0,
                           dspac,
                           NULL,
                           1,
                           dsizes,
                           TEMPORARY,
                           NULL,
                           (NclObjClass)nclTypedoubleClass
                           );
  }
  _NclAddAtt(
             att_id,
             "spacing",
             att_md,
             NULL
             );

  dsizes[0] = nlatlon_new;
  if(type_lat == NCL_double) {
    att_md = _NclCreateVal(
                           NULL,
                           NULL,
                           Ncl_MultiDValData,
                           0,
                           lat,
                           NULL,
                           1,
                           dsizes,
                           TEMPORARY,
                           NULL,
                           (NclObjClass)nclTypedoubleClass
                           );
  }
  else {
    att_md = _NclCreateVal(
                           NULL,
                           NULL,
                           Ncl_MultiDValData,
                           0,
                           lat,
                           NULL,
                           1,
                           dsizes,
                           TEMPORARY,
                           NULL,
                           (NclObjClass)nclTypefloatClass
                           );
  }
  _NclAddAtt(
             att_id,
             "gclat",
             att_md,
             NULL
             );

  if(type_lon == NCL_double) {
    att_md = _NclCreateVal(
                           NULL,
                           NULL,
                           Ncl_MultiDValData,
                           0,
                           lon,
                           NULL,
                           1,
                           dsizes,
                           TEMPORARY,
                           NULL,
                           (NclObjClass)nclTypedoubleClass
                           );
  }
  else {
    att_md = _NclCreateVal(
                           NULL,
                           NULL,
                           Ncl_MultiDValData,
                           0,
                           lon,
                           NULL,
                           1,
                           dsizes,
                           TEMPORARY,
                           NULL,
                           (NclObjClass)nclTypefloatClass
                           );
  }
  _NclAddAtt(
             att_id,
             "gclon",
             att_md,
             NULL
             );

  if(1 <= *code && *code <=4) {
    dsizes[0] = 1;
    units  = (NclQuark*)NclMalloc(sizeof(NclQuark));
    switch(*code) {
    case  1:
      *units = NrmStringToQuark("radians");
      break;
    case  2:
      *units = NrmStringToQuark("degrees");
      break;
    case  3:
      *units = NrmStringToQuark("meters");
      break;
    case  4:
      *units = NrmStringToQuark("kilometers");
      break;
    }

    att_md = _NclCreateVal(
                           NULL,
                           NULL,
                           Ncl_MultiDValData,
                           0,
                           units,
                           NULL,
                           1,
                           dsizes,
                           TEMPORARY,
                           NULL,
                           (NclObjClass)nclTypestringClass
                           );
    _NclAddAtt(
               att_id,
               "units",
               att_md,
               NULL
               );
  }
  tmp_var = _NclVarCreate(
                          NULL,
                          NULL,
                          Ncl_Var,
                          0,
                          NULL,
                          return_md,
                          NULL,
                          att_id,
                          NULL,
                          RETURNVAR,
                          NULL,
                          TEMPORARY
                          );
/*
 * Return output grid and attributes to NCL.
 */
  return_data.kind = NclStk_VAR;
  return_data.u.data_var = tmp_var;
  _NclPlaceReturn(return_data);
  return(NhlNOERROR);
}

