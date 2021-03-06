#ifndef __RANDWERTE_H__
#define __RANDWERTE_H__


/**
 * The boundary values of the problem are set.
 */
void boundaryvalues(
  int    imax,
  int    jmax,
  double **U,
  double **V,
  double **K,
  double **E,
  int    *boundrs,	/* vector representing different types of boundaries */
  int    **Flag		/* Flag field, describing the geometry of the problem */
);

/** 
 * Specify different boubdary conditions 
 * depending on the problem
 */
void spec_boundary_val( 
  char  *problem, 
  int    imax, 
  int    jmax,
  double **U, 
  double **V, 
  double **K, 
  double **E, 
  double Re, 
  double dp, 
  double cn, 
  double ylength
);

#endif

