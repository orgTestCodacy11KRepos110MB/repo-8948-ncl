#include <stdio.h>
#include <stdlib.h>
#include "wrapper.h"

extern void NGCALLF(grid2triple,GRID2TRIPLE)(double*,double*,double*,int*,
					     int*,double*,int*,int*,double*,
					     int*);

extern void NGCALLF(triple2grid,TRIPLE2GRID)(int*,double*,double*,double*,
					     double*,int*,int*,double*,
					     double*,double*,double*,
					     logical*,int*);

NhlErrorTypes grid2triple_W( void )
{
/*
 * Input array variables
 */
  void *x, *y, *z;
  double *tmp_x, *tmp_y, *tmp_z;
  int dsizes_x[1], dsizes_y[1], dsizes_z[2];
  NclBasicDataTypes type_x, type_y, type_z;
  int has_missing_z;
  NclScalar missing_z, missing_dz, missing_rz;
/*
 * Output array variables
 */
  void *d;
  double *tmp_d;
  int dsizes_d[2];
  NclBasicDataTypes type_d;
/*
 * Various
 */
  int i, mx, ny, ldmax, ldmax2, ldmax3, ld, ld2, ier;
/*
 * Retrieve input array. 
 */
  x = (void*)NclGetArgValue(
           0,
           3,
           NULL,
           dsizes_x,
           NULL,
           NULL,
           &type_x,
           2);

  y = (void*)NclGetArgValue(
           1,
           3,
           NULL,
           dsizes_y,
           NULL,
           NULL,
           &type_y,
           2);

  z = (void*)NclGetArgValue(
           2,
           3,
           NULL,
           dsizes_z, 
           &missing_z,
           &has_missing_z,
           &type_z,
           2);

  mx  = dsizes_x[0];
  ny  = dsizes_y[0];
  ldmax = mx * ny;
  ldmax2 = 2 * ldmax;
  ldmax3 = 3 * ldmax;
/*
 * Check size of z array.
 */
  if(dsizes_z[0] != ny || dsizes_z[1] != mx) {
    NhlPError(NhlFATAL,NhlEUNKNOWN,"grid2triple: The last input array must be dimensioned ny x mx, where ny is the length of y, and mx is the length of x");
    return(NhlFATAL);
  }
/*
 * Coerce missing values.
 */
  coerce_missing(type_z,has_missing_z,&missing_z,&missing_dz,&missing_rz);
/*
 * Coerce input to double if necessary.
 */
  tmp_x = coerce_input_double(x,type_x,mx,0,NULL,NULL);
  tmp_y = coerce_input_double(y,type_y,ny,0,NULL,NULL);
  tmp_z = coerce_input_double(z,type_z,ldmax,0,NULL,NULL);

  if( tmp_x == NULL || tmp_y == NULL || tmp_z == NULL ) {
    NhlPError(NhlFATAL,NhlEUNKNOWN,"grid2triple: Unable to coerce input arrays to double");
    return(NhlFATAL);
  }

/*
 * Allocate space for output array. Since the output array can vary  in
 * size depending on how many missing values there are, we allocate
 * space for a temporary array no matter what.  When we return from the
 * function, we'll create a new array to hold the actual number of
 * non-missing values returned.
 */
  tmp_d = (double*)calloc(ldmax3,sizeof(double));
  if(tmp_d == NULL) {
    NhlPError(NhlFATAL,NhlEUNKNOWN,"grid2triple: Unable to allocate memory for temporary output array");
    return(NhlFATAL);
  }
/*
 * Get type of return variable. If any of the input is double, then return
 * a double. Return float otherwise.
 */
  if(type_x == NCL_double || type_y == NCL_double || type_z == NCL_double) {
    type_d = NCL_double;
  }
  else {
    type_d = NCL_float;
  }

  NGCALLF(grid2triple,GRID2TRIPLE)(tmp_x,tmp_y,tmp_z,&mx,&ny,tmp_d,&ldmax,
                                   &ld,&missing_dz.doubleval,&ier);
/*
 * if ld is zero, then this probably means that all of tmp_d is missing,
 * and thus we need to return 3*ldmax missing values.
 */
  if(ld == 0) ld = ldmax;

  dsizes_d[0] = 3;
  dsizes_d[1] = ld;
  if(type_d == NCL_double) {
    d = (void*)calloc(3*ld,sizeof(double));
  }
  else {
    d = (void*)calloc(3*ld,sizeof(float));
  }
  if(d == NULL) {
    NhlPError(NhlFATAL,NhlEUNKNOWN,"grid2triple: Unable to allocate memory for temporary output array");
    return(NhlFATAL);
  }

  if(!ier) {
    ld2 = 2*ld;
/*
 * ld contains the number of non-missing values. It should be <= ldmax.
 *
 * The first ld elements of tmp_d will be the non-missing ones.
 */
    coerce_output_float_or_double(d,tmp_d,type_d,ld,0);
    coerce_output_float_or_double(d,&tmp_d[ldmax],type_d,ld,ld);
    coerce_output_float_or_double(d,&tmp_d[ldmax2],type_d,ld,ld2);
  }
  else {
    if(ier == -10) {
      NhlPError(NhlWARNING,NhlEUNKNOWN,"grid2triple: all z input values are missing\n");
      set_subset_output_missing(d,0,type_d,ldmax3,missing_dz.doubleval);
    }
  }

/*
 * Free unneeded memory.
 */
  if(type_x != NCL_double) NclFree(tmp_x);
  if(type_y != NCL_double) NclFree(tmp_y);
  if(type_z != NCL_double) NclFree(tmp_z);
  NclFree(tmp_d);
/*
 * Return.
 */
  if(ier) {
    if(type_d == NCL_double) {
      return(NclReturnValue(d,2,dsizes_d,&missing_dz,type_d,0));
    }
    else {
      return(NclReturnValue(d,2,dsizes_d,&missing_rz,type_d,0));
    }
  }
  else {
    return(NclReturnValue(d,2,dsizes_d,NULL,type_d,0));
  }
}

NhlErrorTypes triple2grid_W( void )
{
/*
 * Input array variables
 */
  void *x, *y, *z, *gridx, *gridy;
  double *tmp_x, *tmp_y, *tmp_z, *tmp_gridx, *tmp_gridy;
  int dsizes_x[1], dsizes_y[1], dsizes_z[1], dsizes_gridx[1], dsizes_gridy[1];
  int has_missing_z;
  NclBasicDataTypes type_x, type_y, type_z, type_gridx, type_gridy;
  NclScalar missing_z, missing_rz, missing_dz;
  logical *option;
/*
 * Output array variables
 */
  void *grid;
  double *tmp_grid;
  int dsizes_grid[2];
  NclBasicDataTypes type_grid;
/*
 * Various
 */
  int i, npts, ngx, ngy, ier;
  logical strict = False;
  double domain = 0.;
/*
 * Variables for retrieving attributes from "options".
 */
  NclAttList  *attr_list;
  NclAtt  attr_obj;
  NclStackEntry   stack_entry;

/*
 * Retrieve input arrays. 
 */
  x = (void*)NclGetArgValue(
           0,
           6,
           NULL,
           dsizes_x,
           NULL,
           NULL,
           &type_x,
           2);

  y = (void*)NclGetArgValue(
           1,
           6,
           NULL,
           dsizes_y,
           NULL,
           NULL,
           &type_y,
           2);

  z = (void*)NclGetArgValue(
           2,
           6,
           NULL,
           dsizes_z,
           &missing_z,
           &has_missing_z,
           &type_z,
           2);

  gridx = (void*)NclGetArgValue(
           3,
           6,
           NULL,
           dsizes_gridx,
           NULL,
           NULL,
           &type_gridx,
           2);

  gridy = (void*)NclGetArgValue(
           4,
           6,
           NULL,
           dsizes_gridy,
           NULL,
           NULL,
           &type_gridy,
           2);

  option = (logical*)NclGetArgValue(
           5,
           6,
           NULL,
           NULL,
           NULL,
           NULL,
           NULL,
           2);

/*
 * Check sizes of x, y, and z arrays.
 */
  npts  = dsizes_x[0];
  if(dsizes_y[0] != npts || dsizes_z[0] != npts) {
    NhlPError(NhlFATAL,NhlEUNKNOWN,"triple2grid: The three input arrays must be the same length");
    return(NhlFATAL);
  }

/*
 * Get sizes for output array.
 * Remember, in NCL, the Y dimension is usually associated with dimension
 * '0, and the X dimension with dimension '1'.
 */
  ngx = dsizes_gridx[0];
  ngy = dsizes_gridy[0];

  dsizes_grid[0] = ngy;
  dsizes_grid[1] = ngx;
/*
 * Get type of return variable. If any of the input is double, then return
 * a double. Return float otherwise.
 */
  if(type_x == NCL_double || type_y == NCL_double || type_z == NCL_double || 
     type_gridx == NCL_double || type_gridy == NCL_double) {

    type_grid = NCL_double;
    grid      = (void*)calloc(ngx*ngy,sizeof(double));
  }
  else {
    type_grid = NCL_float;
    grid      = (void*)calloc(ngx*ngy,sizeof(float));
  }
  tmp_grid = coerce_output_double(grid,type_grid,ngx*ngy);

  if(grid == NULL || tmp_grid == NULL) {
    NhlPError(NhlFATAL,NhlEUNKNOWN,"triple2grid: Unable to allocate memory for output array");
    return(NhlFATAL);
  }
/*
 * Coerce missing values.  The z missing value will be used to determine
 * the grid missing value.  If z doesn't have a missing value, then the
 * default missing value will be used.
 */
  coerce_missing(type_z,has_missing_z,&missing_z,&missing_dz,&missing_rz);
/*
 * Coerce input to double if necessary.
 */
  tmp_x     = coerce_input_double(x,type_x,npts,0,NULL,NULL);
  tmp_y     = coerce_input_double(y,type_y,npts,0,NULL,NULL);
  tmp_z     = coerce_input_double(z,type_z,npts,0,NULL,NULL);
  tmp_gridx = coerce_input_double(gridx,type_gridx,ngx,0,NULL,NULL);
  tmp_gridy = coerce_input_double(gridy,type_gridy,ngy,0,NULL,NULL);

  if( tmp_x == NULL || tmp_y == NULL || tmp_z == NULL ||
      tmp_gridx == NULL || tmp_gridy == NULL ) {
    NhlPError(NhlFATAL,NhlEUNKNOWN,"triple2grid: Unable to coerce input arrays to double");
    return(NhlFATAL);
  }

/*
 * If "option" is True, then it may contain some attributes that we
 * need to retrieve.
 */
  if(*option) {
/*
 * Retrieve  "option" again, this time getting all the stuff that
 * might be attached to it (attributes).
 */
    stack_entry = _NclGetArg(5, 6, DONT_CARE);
    switch (stack_entry.kind) {
    case NclStk_VAR:
      if (stack_entry.u.data_var->var.att_id != -1) {
	attr_obj = (NclAtt) _NclGetObj(stack_entry.u.data_var->var.att_id);
	if (attr_obj == NULL) {
	  break;
	}
      }
      else {
	/* att_id == -1, no optional args given, defaults desired */
	break;
      }
      
      /* get optional arguments;  if none specified, use defaults */
      if (attr_obj->att.n_atts == 0) {
	break;
      }
      else {
	/* att_n_atts > 0, retrieve optional arguments */
	attr_list = attr_obj->att.att_list;
	while (attr_list != NULL) {
	  if ((strcmp(attr_list->attname, "domain")) == 0) {
	    domain = (double)(*(float *) attr_list->attvalue->multidval.val);
	  }

	  if ((strcmp(attr_list->attname, "strict")) == 0) {
	    strict = *(logical *) attr_list->attvalue->multidval.val;
	  }
	  
	  attr_list = attr_list->next;
	}
      }
      
    default:
      break;
    }
  }


/*
 * Call the Fortran subroutine.
 */
  NGCALLF(triple2grid,TRIPLE2GRID)(&npts,tmp_x,tmp_y,tmp_z,
				   &missing_dz.doubleval,&ngx,&ngy,
				   tmp_gridx,tmp_gridy,tmp_grid,&domain,
				   &strict,&ier);
/*
 * Coerce grid back to float if necessary.
 *
 */
  if(type_grid == NCL_float) {
    coerce_output_float_only(grid,tmp_grid,ngx*ngy,0);
  }
/*
 * Free unneeded memory.
 */
  if(type_x     != NCL_double) NclFree(tmp_x);
  if(type_y     != NCL_double) NclFree(tmp_y);
  if(type_z     != NCL_double) NclFree(tmp_z);
  if(type_gridx != NCL_double) NclFree(tmp_gridx);
  if(type_gridy != NCL_double) NclFree(tmp_gridy);
  if(type_grid  != NCL_double) NclFree(tmp_grid);

/*
 * Return with missing value set no matter what, b/c even though input
 * may not have missing values, the output grid is bound to not have all
 * of its values filled in.
 */
  if(type_grid == NCL_double) {
    return(NclReturnValue(grid,2,dsizes_grid,&missing_dz,type_grid,0));
  }
  else {
    return(NclReturnValue(grid,2,dsizes_grid,&missing_rz,type_grid,0));
  }
}
