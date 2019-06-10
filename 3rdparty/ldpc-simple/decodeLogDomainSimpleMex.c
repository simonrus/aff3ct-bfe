/* MATLAB's C-MEX Min-sum LDPC decoder
 *
 * Copyright Bagawan S. Nugroho, 2009
 * http://bsnugroho.googlepages.com
 *********************************************/

#include "mex.h"

#define INF 1e20

/* ------------ The main function ----------- */
void minSum( double *vHat, double *rx, float *H, int M, int N, int iter)
/* Min-sum algorithm for low density parity check codes 
 */

{
   int i, j, k, l, n, prodOfalphaij;
   int *alphaij;
   double *Lci, LQi, minOfbetaij, sumOfLrji, tmp;
   double **Lrji, **Lqij;
   
   /* Dynamic array allocation */
   alphaij =  mxCalloc( N, sizeof(int));
   Lci =  mxCalloc( N, sizeof(double));
   Lrji = (double **) mxCalloc( M, sizeof(double));
   Lqij = (double **) mxCalloc( M, sizeof(double));
   
   for( i = 0; i < M; i++) {
      *(Lrji + i) = (double *) mxCalloc( N, sizeof(double));
      *(Lqij + i) = (double *) mxCalloc( N, sizeof(double));
   } /* for i */

   /* Prior log-likelihood (simplified) */
   for( j = 0; j < N; j++) {
      *(Lci + j) = -*(rx + j);
   } /* for j */
   
   /* Asscociate the L(ci) with non-zero elements of H */
   for( i = 0; i < M; i++) {
      for( j = 0; j < N; j++)
         *(*(Lqij + i) + j) = *(H + i + j*M)**(Lci + j);
   } /* for i */

   /* Iteration */
   for( n = 0; n < iter; n++) {   
   
      /* -------------- Horizontal step -------------- */		
      for( i = 0; i < M; i++) {
      
         /* Get the sign of L(qij) */
         for( j = 0; j < N; j++) {
      
            if( *(H + i + j*M) != 0)
               *(alphaij + j) = (*(*(Lqij + i) + j) < 0)?-1:1;
            
         } /* for j */
            
         /* Find non-zeros in the column */ 
         for( j = 0; j < N; j++) {
    
            if( *(H + i + j*M) != 0) {
                 
               minOfbetaij = INF;
               prodOfalphaij = 1;
            
               for( k = 0; k < N; k++) { 
               
                  if( k != j && *(H + i + k*M) != 0) {
                  
                     /* Product of alpha(ij)\i */
                     prodOfalphaij *= *(alphaij + k);
         
                     /* Find minimum beta(ji)\i */
                     tmp = fabs( *(*(Lqij + i) + k));
                     if( tmp < minOfbetaij)
                        minOfbetaij = tmp;
                  
                  } /* if */
         
               } /* for k */
	
               /* Update L(rji) */   
               *(*(Lrji + i) + j) = prodOfalphaij*minOfbetaij;
         
            } /* if */
         
            else
               *(*(Lrji + i) + j) = 0;
		
         } /* for j */
     
      } /* for i */

      /* -------------- Vertical step ------------- */
      for( i = 0; i < N; i++) {
   
         /* Find non-zeros in the row */  
         for( j = 0; j < M; j++) {
         
            if( *(H + i*M + j) != 0) {
         
               sumOfLrji = 0;
               /* Get the sum of L(rji)\j */
               for( k = 0; k < M; k++) {   
                  if( k != j && *(H + i*M + k) != 0)
                     sumOfLrji += *(*(Lrji + k) + i);
                  if( k == j)
                     l = k;
               } /* for k */

               /* Update L(qij) by summation of L(rij)\r1(k) */
               *(*(Lqij + j) + i) = *(Lci + i) + sumOfLrji;
         
            }  /* if */ 
            
            else
               *(*(Lqij + j) + i) = 0;
         
         } /* for j */
    
         /* Get the sum of L(rji) */
         sumOfLrji += *(*(Lrji + l) + i);
      
         /* Final statistics for decision */
         LQi = *(Lci + i) + sumOfLrji;
         
         /* Hard decision: Decode L(Qi) */
         *(vHat + i) = (LQi < 0)?1:0;
      
      } /* for i */
      
   } /* for n */   
   
   mxFree(alphaij);
   mxFree(Lci);
   mxFree(Lrji);
   mxFree(Lqij);

} /* minSum() */

/* ----------- Gateway function ---------- */
void mexFunction( int nlhs, mxArray *plhs[],
                    int nrhs, const mxArray *prhs[] )

/* input : rx = prhs[0], received vector
 *         H = prhs[1], H transpose matrix
 *         iteration = prhs[2], number of iteration 
 *
 * output: vHat = plhs[0], hard decision output
 */         

{
   float *H;
   double *rx, *vHat, iteration; 
   int mrows, ncols, i, j;
   
  if( nrhs != 3) {
     mexErrMsgTxt( "Incorrect input argument(s)!");
  }
   
   /* Create pointer to the inputs */
   rx = mxGetPr( prhs[0]);      
   H = mxGetPr( prhs[1]);
   iteration = mxGetScalar( prhs[2]);
   
   /* Get the # of row of the H transpose */
   mrows = mxGetM( prhs[1]);

   /* Get the # of column of the H transpose */
   ncols = mxGetN( prhs[1]);

   /* Set the output pointer to the output vector */
   plhs[0] = mxCreateDoubleMatrix( 1, ncols, mxREAL);   
   
   /* Create a pointer to a copy ot the output vector LOut */
   vHat = mxGetPr( plhs[0]);
   
   /* Call the main subroutine */
   minSum( vHat, rx, H, mrows, ncols, (int) iteration);
}
