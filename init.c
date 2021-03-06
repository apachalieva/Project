#include "helper.h"
#include "init.h"


int read_parameters( const char *szFileName,   /* name of the file 		*/
                    double *Re,                /* reynolds number   		*/
                    double *UI,                /* velocity x-direction 		*/
                    double *VI,                /* velocity y-direction 		*/
                    double *PI,                /* pressure			*/
                    double *GX,                /* gravitation x-direction 	*/
                    double *GY,                /* gravitation y-direction 	*/
                    double *t_end,             /* end time 			*/
                    double *xlength,           /* length of the domain x-dir	*/
                    double *ylength,           /* length of the domain y-dir	*/
                    double *dt,                /* time step 			*/
                    double *dx,                /* length of a cell x-dir 	*/
                    double *dy,                /* length of a cell y-dir	*/
                    int    *imax,              /* number of cells x-direction	*/
                    int    *jmax,              /* number of cells y-direction	*/
                    double *alpha,             /* uppwind differencing factor	*/
                    double *omg,               /* relaxation factor 		*/
                    double *tau,               /* safety factor for time step	*/
                    int    *itermax,           /* max. number of iterations  	*/
		                               /* for pressure per time step 	*/
                    double *eps,               /* accuracy bound for pressure	*/
                    double *dt_value,          /* time for output 		*/
                    int    *boundrs, 	       /* vector for boundaries 	*/
                    double *dp,		       /* dp/dx gradient of pressure 	*/
                    int    *p,		       /* specification of the problem 	*/
		    double *K,		       /* kinetic energy intial value 	*/
        	    double *E,		       /* dissipation rate initial value*/
        	    double *cn,		       /* turbolent eddy viscosity 	*/
        	    double *ce,		       /* turbolent modelling constants */
        	    double *c1,		       /* turbolent modelling constants */
        	    double *c2,		       /* turbolent modelling constants */
		    char   *pgm		       /* specification of the problem  */
)
{
   int *wl,*wb,*wr,*wt;
   READ_DOUBLE( szFileName, *xlength );
   READ_DOUBLE( szFileName, *ylength );

   READ_DOUBLE( szFileName, *Re    );
   READ_DOUBLE( szFileName, *t_end );
   READ_DOUBLE( szFileName, *dt    );

   READ_INT   ( szFileName, *imax );
   READ_INT   ( szFileName, *jmax );

   READ_DOUBLE( szFileName, *omg   );
   READ_DOUBLE( szFileName, *eps   );
   READ_DOUBLE( szFileName, *tau   );
   READ_DOUBLE( szFileName, *alpha );

   READ_INT   ( szFileName, *itermax );
   READ_DOUBLE( szFileName, *dt_value );

   READ_DOUBLE( szFileName, *UI );
   READ_DOUBLE( szFileName, *VI );
   READ_DOUBLE( szFileName, *GX );
   READ_DOUBLE( szFileName, *GY );
   READ_DOUBLE( szFileName, *PI );

   READ_DOUBLE( szFileName, *K );
   READ_DOUBLE( szFileName, *E );

   READ_DOUBLE( szFileName, *cn );
   READ_DOUBLE( szFileName, *ce );
   READ_DOUBLE( szFileName, *c1 );
   READ_DOUBLE( szFileName, *c2 );

   /* change here: reading boundaries */
   wl = &boundrs[ 0 ];
   wr = &boundrs[ 1 ];
   wb = &boundrs[ 2 ];
   wt = &boundrs[ 3 ];
   read_int( szFileName, "wl", wl );
   read_int( szFileName, "wr", wr );
   read_int( szFileName, "wb", wb );
   read_int( szFileName, "wt", wt );

   READ_DOUBLE( szFileName, *dp );
   READ_INT   ( szFileName, *p );

   read_string(szFileName, "pgm", pgm);

   *dx = *xlength / (double)(*imax);
   *dy = *ylength / (double)(*jmax);

   return 1;
}

/**
 * The arrays U, V, P, K and E are initialized 
 * to the constant values UI, VI, PI, KI and EI 
 * on the whole domain.
 */
void init_uvp(
  double UI,
  double VI,
  double PI,
  double KI,
  double EI,
  int    imax,
  int    jmax,
  double **U,
  double **V,
  double **P,
  double **K,
  double **E,
  int    **Flagfield,
  char*  problem
){
	int i,j;

	for( i=0;i<=imax+1; i++ )
		for( j=0;j<=jmax+1; j++ ){
				U[i][j] = IS_FLUID(Flagfield[i][j]) * UI;
				V[i][j] = IS_FLUID(Flagfield[i][j]) * VI;
				P[i][j] = IS_FLUID(Flagfield[i][j]) * PI;
				K[i][j] = IS_FLUID(Flagfield[i][j]) * KI;
				E[i][j] = IS_FLUID(Flagfield[i][j]) * EI;
		}

	if(strcmp(problem, "step") == 0)
		for( i=1;i<=imax; i++ )
			for( j=1;j<=jmax/2; j++ )
					U[i][j] = 0;
}

/** 
 * Initialize the flagfield regarding the problem chosen. 
 * C_F	:	Fluid cell
 * C_B	:	Obstacle cell
 * 
 * B_E	:	Boundary cell with Eastern fluid cell
 * B_W	:	Boundary cell with Western fluid cell
 * B_S	:	Boundary cell with Southern fluid cell
 * B_N	:	Boundary cell with Northern fluid cell
 * B_SE	:	Boundary cell with South-Eastern fluid cell
 * B_SW	:	Boundary cell with South-Western fluid cell
 * B_NE	:	Boundary cell with North-Eastern fluid cell
 * B_NW	:	Boundary cell with North-Western fluid cell
 */
void init_flag( 
  const char *problem, 
  const int  imax, 
  const int  jmax, 
  int        *fluid_cells, 
  int        **Flag 
){
	char filename[35];
	char ext[] = ".pgm\0";
	
	int **buffer;
	int i, j;
	
	*fluid_cells=0;
	if (strcmp(problem, "none") != 0){
		snprintf( filename, sizeof filename, "%s%s", problem, ext );
		buffer = read_pgm( filename );

		for( i = 0; i <= imax+1; i++ )
			for( j = 0; j <= jmax+1; j++ ){
				Flag[i][j] = buffer[i][j]*C_F;
				(*fluid_cells)+=buffer[i][j];
			}

		free_imatrix(buffer,0,imax+1,0,jmax+1);

	} else {
		for( i = 1; i <= imax; i++ ){
			Flag[i][0] = C_B;
			for( j = 1; j <= jmax; j++ ){
				Flag[i][j] = C_F;
				(*fluid_cells)++;
			}
			Flag[i][jmax+1] = C_B;
		}

		for( j = 1; j <= jmax; j++ ){
			Flag[0][j] = C_B;
			Flag[imax+1][j] = C_B;
		}
	}
	
	for( i = 1; i <= imax; i++ ){
	    for( j = 1; j <= jmax; j++ ){
		if( Flag[i][j] == C_B  ){
		    if( Flag[i+1][j] == C_F )
		    	Flag[i][j] |= B_E;
		    if( Flag[i-1][j] == C_F )
		    	Flag[i][j] |= B_W;
		    if( Flag[i][j-1] == C_F )
		    	Flag[i][j] |= B_S;
		    if( Flag[i][j+1] == C_F )
		    	Flag[i][j] |= B_N;


		    /* Forbidden cells */
		    if( ( Flag[i][j] & ( B_E | B_W ) ) == ( B_E | B_W ) || 
			( Flag[i][j] & ( B_S | B_N ) ) == ( B_S | B_N ) ){
			  printf( "ERROR: Forbidden geometry!\n" );
			  exit(1);
		    }
		}
	    }
	}
}
