/* dnlaso.f -- translated by f2c (version 19950102).
   You must link the resulting object file with the libraries:
        -lf2c -lm   (in that order)
*/

#include "f2c.h"

/* Table of constant values */

static integer c__0 = 0;
static integer c__1 = 1;
static doublereal c_b88 = 0.;

/*   VERSION 2    DOES NOT USE EISPACK */

/* ------------------------------------------------------------------ */

/* Subroutine */ void dnlaso_(op, iovect, n, nval, nfig, nperm, nmval, val,
        nmvec, vec, nblock, maxop, maxj, work, ind, ierr)
int (*op) (), (*iovect) ();
integer *n, *nval, *nfig, *nperm, *nmval;
doublereal *val;
integer *nmvec;
doublereal *vec;
integer *nblock, *maxop, *maxj;
doublereal *work;
integer *ind, *ierr;
{
    /* Local variables */
    static doublereal temp, tarr;
    extern doublereal dnrm2_();
    static integer i, m, nband;
    static doublereal delta;
    extern /* Subroutine */ void dnwla_();
    static logical small;
    extern /* Subroutine */ void dcopy_();
    static integer i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, nv;
    extern /* Subroutine */ void dnppla_();
    static logical raritz;
    extern /* Subroutine */ void dortqr_(), dvsort_();
    static doublereal eps;
    static integer nop;

/* AUTHOR/IMPLEMENTER D.S.SCOTT-B.N.PARLETT/D.S.SCOTT			*/
/*									*/
/* COMPUTER SCIENCES DEPARTMENT						*/
/* UNIVERSITY OF TEXAS AT AUSTIN					*/
/* AUSTIN, TX 78712							*/
/*									*/
/* VERSION 2 ORIGINATED APRIL 1982					*/
/*									*/
/* CURRENT VERSION  JUNE 1983						*/
/*									*/
/* DNLASO FINDS A FEW EIGENVALUES AND EIGENVECTORS AT EITHER END OF	*/
/* THE SPECTRUM OF A LARGE SPARSE SYMMETRIC MATRIX.  THE SUBROUTINE	*/
/* DNLASO IS PRIMARILY A DRIVER FOR SUBROUTINE DNWLA WHICH IMPLEMENTS	*/
/* THE LANCZOS ALGORITHM WITH SELECTIVE ORTHOGONALIZATION AND		*/
/* SUBROUTINE DNPPLA WHICH POST PROCESSES THE OUTPUT OF DNWLA.		*/
/* HOWEVER DNLASO DOES CHECK FOR INCONSISTENCIES IN THE CALLING		*/
/* PARAMETERS AND DOES PREPROCESS ANY USER SUPPLIED EIGENPAIRS.		*/
/* DNLASO ALWAYS LOOKS FOR THE SMALLEST (LEFTMOST) EIGENVALUES.  IF	*/
/* THE LARGEST EIGENVALUES ARE DESIRED DNLASO IMPLICITLY USES THE	*/
/* NEGATIVE OF THE MATRIX.						*/
/*									*/
/*									*/
/* ON INPUT								*/
/*									*/
/*									*/
/*   OP   A USER SUPPLIED SUBROUTINE WITH CALLING SEQUENCE		*/
/*     OP(N,M,P,Q).  P AND Q ARE N X M MATRICES AND Q IS		*/
/*     RETURNED AS THE MATRIX TIMES P.					*/
/*									*/
/*   IOVECT   A USER SUPPLIED SUBROUTINE WITH CALLING SEQUENCE		*/
/*     IOVECT(N,M,Q,J,K).  Q IS AN N X M MATRIX.  IF K = 0		*/
/*     THE COLUMNS OF Q ARE STORED AS THE (J-M+1)TH THROUGH		*/
/*     THE JTH LANCZOS VECTORS.  IF K = 1 THEN Q IS RETURNED		*/
/*     AS THE (J-M+1)TH THROUGH THE JTH LANCZOS VECTORS.  SEE		*/
/*     DOCUMENTATION FOR FURTHER DETAILS AND EXAMPLES.			*/
/*									*/
/*   N   THE ORDER OF THE MATRIX.					*/
/*									*/
/*   NVAL   NVAL SPECIFIES THE EIGENVALUES TO BE FOUND.			*/
/*     DABS(NVAL)  IS THE NUMBER OF EIGENVALUES DESIRED.		*/
/*     IF NVAL < 0 THE ALGEBRAICALLY SMALLEST (LEFTMOST)		*/
/*     EIGENVALUES ARE FOUND.  IF NVAL > 0 THE ALGEBRAICALLY		*/
/*     LARGEST (RIGHTMOST) EIGENVALUES ARE FOUND.  NVAL MUST NOT	*/
/*     BE ZERO.  DABS(NVAL) MUST BE LESS THAN  MAXJ/2.			*/
/*									*/
/*   NFIG   THE NUMBER OF DECIMAL DIGITS OF ACCURACY DESIRED IN THE	*/
/*     EIGENVALUES.  NFIG MUST BE GREATER THAN OR EQUAL TO 1.		*/
/*									*/
/*   NPERM   AN INTEGER VARIABLE WHICH SPECIFIES THE NUMBER OF USER	*/
/*     SUPPLIED EIGENPAIRS.  IN MOST CASES NPERM WILL BE ZERO.  SEE	*/
/*     DOCUMENTAION FOR FURTHER DETAILS OF USING NPERM GREATER		*/
/*     THAN ZERO.  NPERM MUST NOT BE LESS THAN ZERO.			*/
/*									*/
/*   NMVAL   THE ROW DIMENSION OF THE ARRAY VAL.  NMVAL MUST BE GREATER	*/
/*     THAN OR EQUAL TO DABS(NVAL).					*/
/*									*/
/*   VAL   A TWO DIMENSIONAL DOUBLE PRECISION ARRAY OF ROW		*/
/*     DIMENSION NMVAL AND COLUMN DIMENSION AT LEAST 4.  IF NPERM	*/
/*     IS GREATER THAN ZERO THEN CERTAIN INFORMATION MUST BE STORED	*/
/*     IN VAL.  SEE DOCUMENTATION FOR DETAILS.				*/
/*									*/
/*   NMVEC   THE ROW DIMENSION OF THE ARRAY VEC.  NMVEC MUST BE GREATER	*/
/*     THAN OR EQUAL TO N.						*/
/*									*/
/*   VEC   A TWO DIMENSIONAL DOUBLE PRECISION ARRAY OF ROW		*/
/*     DIMENSION NMVEC AND COLUMN DIMENSION AT LEAST DABS(NVAL).  IF	*/
/*     NPERM > 0 THEN THE FIRST NPERM COLUMNS OF VEC MUST		*/
/*     CONTAIN THE USER SUPPLIED EIGENVECTORS.				*/
/*									*/
/*   NBLOCK   THE BLOCK SIZE.  SEE DOCUMENTATION FOR CHOOSING		*/
/*     AN APPROPRIATE VALUE FOR NBLOCK.  NBLOCK MUST BE GREATER		*/
/*     THAN ZERO AND LESS THAN  MAXJ/6.					*/
/*									*/
/*   MAXOP   AN UPPER BOUND ON THE NUMBER OF CALLS TO THE SUBROUTINE	*/
/*     OP.  DNLASO TERMINATES WHEN MAXOP IS EXCEEDED.  SEE		*/
/*     DOCUMENTATION FOR GUIDELINES IN CHOOSING A VALUE FOR MAXOP.	*/
/*									*/
/*   MAXJ   AN INDICATION OF THE AVAILABLE STORAGE (SEE WORK AND	*/
/*     DOCUMENTATION ON IOVECT).  FOR THE FASTEST CONVERGENCE MAXJ	*/
/*     SHOULD BE AS LARGE AS POSSIBLE, ALTHOUGH IT IS USELESS TO HAVE	*/
/*     MAXJ LARGER THAN MAXOP*NBLOCK.					*/
/*									*/
/*   WORK   A DOUBLE PRECISION ARRAY OF DIMENSION AT LEAST AS		*/
/*     LARGE AS								*/
/*									*/
/*         2*N*NBLOCK + MAXJ*(NBLOCK+NV+2) + 2*NBLOCK*NBLOCK + 3*NV	*/
/*									*/
/*            + THE MAXIMUM OF						*/
/*                 N*NBLOCK						*/
/*                   AND						*/
/*         MAXJ*(2*NBLOCK+3) + 2*NV + 6 + (2*NBLOCK+2)*(NBLOCK+1)	*/
/*									*/
/*     WHERE NV = DABS(NVAL)						*/
/*									*/
/*     THE FIRST N*NBLOCK ELEMENTS OF WORK MUST CONTAIN THE DESIRED	*/
/*     STARTING VECTORS.  SEE DOCUMENTATION FOR GUIDELINES IN		*/
/*     CHOOSING STARTING VECTORS.					*/
/*									*/
/*   IND   AN INTEGER ARRAY OF DIMENSION AT LEAST DABS(NVAL).		*/
/*									*/
/*   IERR   AN INTEGER VARIABLE.					*/
/*									*/
/*									*/
/* ON OUTPUT								*/
/*									*/
/*									*/
/*   NPERM   THE NUMBER OF EIGENPAIRS NOW KNOWN.			*/
/*									*/
/*   VEC   THE FIRST NPERM COLUMNS OF VEC CONTAIN THE EIGENVECTORS.	*/
/*									*/
/*   VAL   THE FIRST COLUMN OF VAL CONTAINS THE CORRESPONDING		*/
/*     EIGENVALUES.  THE SECOND COLUMN CONTAINS THE RESIDUAL NORMS OF	*/
/*     THE EIGENPAIRS WHICH ARE BOUNDS ON THE ACCURACY OF THE EIGEN-	*/
/*    VALUES.  THE THIRD COLUMN CONTAINS MORE DOUBLE PRECISIONISTIC ESTIMATES	*/
/*     OF THE ACCURACY OF THE EIGENVALUES.  THE FOURTH COLUMN CONTAINS	*/
/*     ESTIMATES OF THE ACCURACY OF THE EIGENVECTORS.  SEE		*/
/*     DOCUMENTATION FOR FURTHER INFORMATION ON THESE QUANTITIES.	*/
/*									*/
/*   WORK   IF WORK IS TERMINATED BEFORE COMPLETION (IERR = -2)		*/
/*     THE FIRST N*NBLOCK ELEMENTS OF WORK CONTAIN THE BEST VECTORS	*/
/*     FOR RESTARTING THE ALGORITHM AND DNLASO CAN BE IMMEDIATELY	*/
/*     RECALLED TO CONTINUE WORKING ON THE PROBLEM.			*/
/*									*/
/*   IND   IND(1)  CONTAINS THE ACTUAL NUMBER OF CALLS TO OP.  ON SOME	*/
/*     OCCASIONS THE NUMBER OF CALLS TO OP MAY BE SLIGHTLY LARGER	*/
/*     THAN MAXOP.							*/
/*									*/
/*   IERR   AN ERROR COMPLETION CODE.  THE NORMAL COMPLETION CODE IS	*/
/*     ZERO.  SEE THE DOCUMENTATION FOR INTERPRETATIONS OF NON-ZERO	*/
/*     COMPLETION CODES.						*/
/*									*/
/*									*/
/* INTERNAL VARIABLES.							*/
/*									*/
/*									*/
/*									*/
/* NOP   RETURNED FROM DNWLA AS THE NUMBER OF CALLS TO THE		*/
/*   SUBROUTINE OP.							*/
/*									*/
/* NV   SET EQUAL TO DABS(NVAL), THE NUMBER OF EIGENVALUES DESIRED,	*/
/*   AND PASSED TO DNWLA.						*/
/*									*/
/* SMALL   SET TO .TRUE. IF THE SMALLEST EIGENVALUES ARE DESIRED.	*/
/*									*/
/* RARITZ   RETURNED FROM DNWLA AND PASSED TO DNPPLA.  RARITZ IS .TRUE.	*/
/*   IF A FINAL RAYLEIGH-RITZ PROCEDURE IS NEEDED.			*/
/*									*/
/* DELTA   RETURNED FROM DNWLA AS THE EIGENVALUE OF THE MATRIX		*/
/*   WHICH IS CLOSEST TO THE DESIRED EIGENVALUES.			*/
/*									*/
/* DNPPLA   A SUBROUTINE FOR POST-PROCESSING THE EIGENVECTORS COMPUTED	*/
/*   BY DNWLA.								*/
/*									*/
/* DNWLA   A SUBROUTINE FOR IMPLEMENTING THE LANCZOS ALGORITHM		*/
/*   WITH SELECTIVE ORTHOGONALIZATION.					*/
/*									*/
/* DMVPC   A SUBROUTINE FOR COMPUTING THE RESIDUAL NORM AND		*/
/*   ORTHOGONALITY COEFFICIENT OF GIVEN RITZ VECTORS.			*/
/*									*/
/* DORTQR   A SUBROUTINE FOR ORTHONORMALIZING A BLOCK OF VECTORS	*/
/*   USING HOUSEHOLDER REFLECTIONS.					*/
/*									*/
/* DAXPY,DCOPY,DDOT,DNRM2,DSCAL,DSWAP   A SUBSET OF THE BASIC LINEAR	*/
/*   ALGEBRA SUBPROGRAMS USED FOR VECTOR MANIPULATION.			*/
/*									*/
/* DLARAN   A SUBROUTINE TO GENERATE RANDOM VECTORS			*/
/*									*/
/* DLAEIG, DLAGER, DLABCM, DLABFC   SUBROUTINES FOR BAND EIGENVALUE	*/
/*   CALCULATIONS.							*/
/*									*/
/* ------------------------------------------------------------------	*/

/* THIS SECTION CHECKS FOR INCONSISTENCY IN THE INPUT PARAMETERS. */

    nv = abs(*nval);
    ind[0] = 0;
    *ierr = 0;
    if (*n < *nblock * 6) {
        *ierr = 1;
    }
    if (*nfig <= 0) {
        *ierr += 2;
    }
    if (*nmvec < *n) {
        *ierr += 4;
    }
    if (*nperm < 0) {
        *ierr += 8;
    }
    if (*maxj < *nblock * 6) {
        *ierr += 16;
    }
    if (nv < max(1,*nperm)) {
        *ierr += 32;
    }
    if (nv > *nmval) {
        *ierr += 64;
    }
    if (nv > *maxop) {
        *ierr += 128;
    }
    if (nv >= *maxj / 2) {
        *ierr += 256;
    }
    if (*nblock < 1) {
        *ierr += 512;
    }
    if (*ierr != 0) {
        return;
    }

    small = *nval < 0;

/* ------------------------------------------------------------------ */

/* THIS SECTION SORTS AND ORTHONORMALIZES THE USER SUPPLIED VECTORS. */
/* IF A USER SUPPLIED VECTOR IS ZERO OR IF DSIGNIFICANT CANCELLATION */
/* OCCURS IN THE ORTHOGONALIZATION PROCESS THEN IERR IS SET TO  -1 */
/* AND DNLASO TERMINATES. */

    if (*nperm == 0) {
        goto L110;
    }

/* THIS NEGATES THE USER SUPPLIED EIGENVALUES WHEN THE LARGEST */
/* EIGENVALUES ARE DESIRED, SINCE DNWLA WILL IMPLICITLY USE THE */
/* NEGATIVE OF THE MATRIX. */

    if (small) {
        goto L20;
    }
    for (i = 0; i < *nperm; ++i) {
        val[i] = -val[i];
    }

/* THIS SORTS THE USER SUPPLIED VALUES AND VECTORS. */

L20:
    dvsort_(nperm, val, &val[*nmval], &c__0, &tarr, nmvec, n, vec);

/* THIS STORES THE NORMS OF THE VECTORS FOR LATER COMPARISON. */
/* IT ALSO INSURES THAT THE RESIDUAL NORMS ARE POSITIVE. */

    for (i = 0; i < *nperm; ++i) {
        val[i + *nmval] = abs(val[i + *nmval]);
        val[i + *nmval * 2] = dnrm2_(n, &vec[i * *nmvec], &c__1);
    }

/* THIS PERFORMS THE ORTHONORMALIZATION. */

    m = *n * *nblock;
    dortqr_(nmvec, n, nperm, vec, &work[m]);
    for (i = 0; i < *nperm; ++i, m += *nperm + 1) {
        if (abs(work[m]) <= val[i + *nmval * 2] * .9f) {
            *ierr = -1;
            return;
        }
    }

/* THIS COPIES THE RESIDUAL NORMS INTO THE CORRECT LOCATIONS IN */
/* THE ARRAY WORK FOR LATER REFERENCE IN DNWLA. */

    m = (*n << 1) * *nblock;
    dcopy_(nperm, &val[*nmval], &c__1, &work[m], &c__1);

/* THIS SETS EPS TO AN APPROXIMATION OF THE RELATIVE MACHINE */
/* PRECISION */

/* ***THIS SHOULD BE REPLACED BY AN ASDSIGNMENT STATEMENT */
/* ***IN A PRODUCTION CODE */

L110:
    eps = 1.;
    for (i = 1; i <= 1000; ++i) {
        eps *= .5;
        temp = eps + 1.;
        if (temp == 1.)
            break;
    }

/* ------------------------------------------------------------------ */

/* THIS SECTION CALLS DNWLA WHICH IMPLEMENTS THE LANCZOS ALGORITHM */
/* WITH SELECTIVE ORTHOGONALIZATION. */

    nband = *nblock + 1;
    i1 = *n * *nblock;
    i2 = i1 + *n * *nblock;
    i3 = i2 + nv;
    i4 = i3 + nv;
    i5 = i4 + nv;
    i6 = i5 + *maxj * nband;
    i7 = i6 + *nblock * *nblock;
    i8 = i7 + *nblock * *nblock;
    i9 = i8 + *maxj * (nv + 1);
    i10 = i9 + *nblock;
    i11 = i10 + (nv << 1) + 6;
    i12 = i11 + *maxj * ((*nblock << 1) + 1);
    i13 = i12 + *maxj;
    dnwla_(op, iovect, n, &nband, &nv, nfig, nperm, val, nmvec, vec, nblock,
           maxop, maxj, &nop, work, &work[i1], &work[i2], &work[i3], &work[i4],
           &work[i5], &work[i6], &work[i7], &work[i8], &work[i9], &work[i10],
           &work[i11], &work[i12], &work[i13], ind, &small, &raritz, &delta, &eps, ierr);

/* ------------------------------------------------------------------ */

/* THIS SECTION CALLS DNPPLA (THE POST PROCESSOR). */

    if (*nperm == 0) {
        ind[0] = nop;
        return;
    }
    i1 = *n * *nblock;
    i2 = i1 + *nperm * *nperm;
    i3 = i2 + *nperm * *nperm;
    i4 = i3 + max(*n * *nblock, (*nperm << 1) * *nperm);
    i5 = i4 + *n * *nblock;
    i6 = i5 + (*nperm << 1) + 4;
    dnppla_(op, iovect, n, nperm, &nop, nmval, val, nmvec, vec, nblock,
            &work[i1], &work[i2], &work[i3], &work[i4], &work[i5], &work[i6],
            &delta, &small, &raritz, &eps);
} /* dnlaso_ */


/* ------------------------------------------------------------------ */

/* Subroutine */ void dnwla_(op, iovect, n, nband, nval, nfig, nperm, val,
        nmvec, vec, nblock, maxop, maxj, nop, p1, p0, res, tau, otau, t, alp,
        bet, s, p2, bound, atemp, vtemp, d, ind, small, raritz, delta, eps,
        ierr)
/* Subroutine */ void (*op) (), (*iovect) ();
integer *n, *nband, *nval, *nfig, *nperm;
doublereal *val;
integer *nmvec;
doublereal *vec;
integer *nblock, *maxop, *maxj, *nop;
doublereal *p1, *p0, *res, *tau, *otau, *t, *alp, *bet, *s, *p2, *bound, *
        atemp, *vtemp, *d;
integer *ind;
logical *small, *raritz;
doublereal *delta, *eps;
integer *ierr;
{
    /* System generated locals */
    integer vec_dim1, vec_offset, p0_dim1, p0_offset, p1_dim1, p1_offset,
            p2_dim1, p2_offset, t_dim1, t_offset, alp_dim1, alp_offset,
            bet_dim1, bet_offset, s_dim1, s_offset, i__1;
    doublereal d__1;

    /* Builtin functions */
    double sqrt(), pow();

    /* Local variables */
    extern doublereal ddot_();
    static doublereal tola, temp, tolg, tmin, tmax, tarr;
    static logical test;
    static doublereal utol;
    extern doublereal dnrm2_();
    static integer i, j, k, l, m;
    extern /* Subroutine */ void dscal_();
    static integer ngood, nleft;
    static doublereal anorm;
    extern /* Subroutine */ void dmvpc_(), dcopy_();
    static integer mtemp;
    static doublereal dzero;
    extern /* Subroutine */ void daxpy_();
    static integer i1;
    static doublereal pnorm, epsrt, rnorm;
    static integer ii, ng;
    extern /* Subroutine */ void dlaeig_(), dlager_(), dlaran_();
    static doublereal betmin, alpmin, betmax, alpmax;
    static integer ntheta;
    static logical enough;
    static integer number, nstart;
    extern /* Subroutine */ void dortqr_(), dvsort_();

/* DNWLA IMPLEMENTS THE LANCZOS ALGORITHM WITH SELECTIVE */
/* ORTHOGONALIZATION. */
/*	*/
/*   NBAND  NBLOCK + 1  THE BAND WIDTH OF T. */
/*   NVAL   THE NUMBER OF DESIRED EIGENVALUES. */
/*   NPERM   THE NUMBER OF PERMANENT VECTORS (THOSE EIGENVECTORS */
/*     INPUT BY THE USER OR THOSE EIGENVECTORS SAVED WHEN THE */
/*     ALGORITHM IS ITERATED).  PERMANENT VECTORS ARE ORTHOGONAL */
/*     TO THE CURRENT KRYLOV SUBSPACE. */
/*   NOP   THE NUMBER OF CALLS TO OP. */
/*   P0, P1, AND P2   THE CURRENT BLOCKS OF LANCZOS VECTORS. */
/*   RES   THE (APPROXIMATE) RESIDUAL NORMS OF THE PERMANENT VECTORS. */
/*   TAU AND OTAU   USED TO MONITOR THE NEED FOR ORTHOGONALIZATION. */
/*   T   THE BAND MATRIX. */
/*   ALP   THE CURRENT DIAGONAL BLOCK. */
/*   BET   THE CURRENT OFF DIAGONAL BLOCK. */
/*   BOUND, ATEMP, VTEMP, D  TEMPORARY STORAGE USED BY THE BAND */
/*      EIGENVALUE SOLVER DLAEIG. */
/*   S   EIGENVECTORS OF T. */
/*   SMALL   .TRUE.  IF THE SMALL EIGENVALUES ARE DESIRED. */
/*   RARITZ  RETURNED AS  .TRUE.  IF A FINAL RAYLEIGH-RITZ PROCEDURE */
/*     IS TO BE DONE. */
/*   DELTA   RETURNED AS THE VALUE OF THE (NVAL+1)TH EIGENVALUE */
/*     OF THE MATRIX.  USED IN ESTIMATING THE ACCURACY OF THE */
/*     COMPUTED EIGENVALUES. */
/*	*/
/* INTERNAL VARIABLES USED */
/*	*/
/* J   THE CURRENT DIMENSION OF T.  (THE DIMENSION OF THE CURRENT */
/*    KRYLOV SUBSPACE. */
/* NGOOD   THE NUMBER OF GOOD RITZ VECTORS (GOOD VECTORS */
/*    LIE IN THE CURRENT KRYLOV SUBSPACE). */
/* NLEFT   THE NUMBER OF VALUES WHICH REMAIN TO BE DETERMINED, */
/*    I.E.  NLEFT = NVAL - NPERM. */
/* NUMBER = NPERM + NGOOD. */
/* ANORM   AN ESTIMATE OF THE NORM OF THE MATRIX. */
/* EPS   THE RELATIVE MACHINE PRECISION. */
/* UTOL   THE USER TOLERANCE. */
/* TARR  AN ARRAY OF LENGTH ONE USED TO INSURE TYPE CONSISTENCY IN CALLS TO */
/*       DLAEIG */
/* DZERO AN ARRAY OF LENGTH ONE CONTAINING DZERO, USED TO INSURE TYPE */
/*       CONSISTENCY IN CALLS TO DCOPY */

    /* Parameter adjustments */
    p2_dim1 = *n;
    p2_offset = p2_dim1 + 1;
    p2 -= p2_offset;
    p0_dim1 = *n;
    p0_offset = p0_dim1 + 1;
    p0 -= p0_offset;
    p1_dim1 = *n;
    p1_offset = p1_dim1 + 1;
    p1 -= p1_offset;
    t_dim1 = *nband;
    t_offset = t_dim1 + 1;
    t -= t_offset;
    --val;
    vec_dim1 = *nmvec;
    vec_offset = vec_dim1 + 1;
    vec -= vec_offset;
    bet_dim1 = *nblock;
    bet_offset = bet_dim1 + 1;
    bet -= bet_offset;
    alp_dim1 = *nblock;
    alp_offset = alp_dim1 + 1;
    alp -= alp_offset;
    s_dim1 = *maxj;
    s_offset = s_dim1 + 1;
    s -= s_offset;
    --res;
    --tau;
    --otau;
    --bound;
    --atemp;
    --vtemp;
    --d;
    --ind;

    dzero = 0.;
    rnorm = 0.;
    if (*nperm != 0) {
        rnorm = max(-val[1],val[*nperm]);
    }
    pnorm = rnorm;
    *delta = 1e31;
    epsrt = sqrt(*eps);
    nleft = *nval - *nperm;
    *nop = 0;
    number = *nperm;
    *raritz = FALSE_;
    utol = max((*n) * *eps, pow(10., -(doublereal)(*nfig)));
    j = *maxj;

/* ------------------------------------------------------------------ */

/* ANY ITERATION OF THE ALGORITHM BEGINS HERE. */

L30:
    for (i = 1; i <= *nblock; ++i) {
        temp = dnrm2_(n, &p1[i * p1_dim1 + 1], &c__1);
        if (temp == 0.) {
            dlaran_(n, &p1[i * p1_dim1 + 1]);
        }
    }
    if (*nperm != 0)
    for (i = 1; i <= *nperm; ++i) {
        tau[i] = 1.;
        otau[i] = 0.;
    }
    i__1 = *n * *nblock;
    dcopy_(&i__1, &dzero, &c__0, &p0[p0_offset], &c__1);
    i__1 = *nblock * *nblock;
    dcopy_(&i__1, &dzero, &c__0, &bet[bet_offset], &c__1);
    i__1 = j * *nband;
    dcopy_(&i__1, &dzero, &c__0, &t[t_offset], &c__1);
    mtemp = *nval + 1;
    for (i = 1; i <= mtemp; ++i) {
        dcopy_(&j, &dzero, &c__0, &s[i * s_dim1 + 1], &c__1);
    }
    ngood = 0;
    tmin = 1e30;
    tmax = -1e30;
    test = TRUE_;
    enough = FALSE_;
    betmax = 0.;
    j = 0;

/* ------------------------------------------------------------------ */

/* THIS SECTION TAKES A SINGLE BLOCK LANCZOS STEP. */

L80:
    j += *nblock;

/* THIS IS THE SELECTIVE ORTHOGONALIZATION. */

    if (number != 0)
    for (i = 1; i <= number; ++i) {
        if (tau[i] < epsrt) {
            continue;
        }
        test = TRUE_;
        tau[i] = 0.;
        if (otau[i] != 0.) {
            otau[i] = 1.;
        }
        for (k = 1; k <= *nblock; ++k) {
            temp = -ddot_(n, &vec[i * vec_dim1 + 1], &c__1, &p1[k * p1_dim1 + 1], &c__1);
            daxpy_(n, &temp, &vec[i * vec_dim1 + 1], &c__1, &p1[k * p1_dim1 + 1], &c__1);

/* THIS CHECKS FOR TOO GREAT A LOSS OF ORTHOGONALITY BETWEEN A */
/* NEW LANCZOS VECTOR AND A GOOD RITZ VECTOR.  THE ALGORITHM IS */
/* TERMINATED IF TOO MUCH ORTHOGONALITY IS LOST. */

            if (abs(temp * bet[k + k * bet_dim1]) > (*n) * epsrt * anorm && i > *nperm) {
                goto L380;
            }
        }
    }

/* IF NECESSARY, THIS REORTHONORMALIZES P1 AND UPDATES BET. */

    if (test)
        dortqr_(n, n, nblock, &p1[p1_offset], &alp[alp_offset]);
    if (test && j != *nblock)
    for (i = 1; i <= *nblock; ++i) {
        if (alp[i + i * alp_dim1] > 0.) {
            continue;
        }
        m = j - (*nblock << 1) + i;
        l = *nblock + 1;
        for (k = i; k <= *nblock; ++k) {
            bet[i + k * bet_dim1] = -bet[i + k * bet_dim1];
            t[l + m * t_dim1] = -t[l + m * t_dim1];
            --l;
            ++m;
        }
    }
    test = FALSE_;

/* THIS IS THE LANCZOS STEP. */

    (*op)(n, nblock, &p1[p1_offset], &p2[p2_offset]);
    ++(*nop);
    (*iovect)(n, nblock, &p1[p1_offset], &j, &c__0);

/* THIS COMPUTES P2=P2-P0*BET(TRANSPOSE) */

    for (i = 1; i <= *nblock; ++i) {
        for (k = i; k <= *nblock; ++k) {
            d__1 = -bet[i + k * bet_dim1];
            daxpy_(n, &d__1, &p0[k * p0_dim1 + 1], &c__1, &p2[i * p2_dim1 + 1], &c__1);
        }
    }

/* THIS COMPUTES ALP AND P2=P2-P1*ALP. */

    for (i = 1; i <= *nblock; ++i) {
        for (k = 1; k <= i; ++k) {
            ii = i - k + 1;
            alp[ii + k * alp_dim1] = ddot_(n, &p1[i * p1_dim1 + 1], &c__1, &
                    p2[k * p2_dim1 + 1], &c__1);
            d__1 = -alp[ii + k * alp_dim1];
            daxpy_(n, &d__1, &p1[i * p1_dim1 + 1], &c__1, &p2[k * p2_dim1 + 1] , &c__1);
            if (k != i) {
                d__1 = -alp[ii + k * alp_dim1];
                daxpy_(n, &d__1, &p1[k * p1_dim1 + 1], &c__1, &p2[i * p2_dim1 + 1], &c__1);
            }
        }
    }

/*  REORTHOGONALIZATION OF THE SECOND BLOCK */

    if (j == *nblock)
    for (i = 1; i <= *nblock; ++i) {
        for (k = 1; k <= i; ++k) {
            temp = ddot_(n, &p1[i * p1_dim1 + 1], &c__1, &p2[k * p2_dim1 + 1], &c__1);
            d__1 = -temp;
            daxpy_(n, &d__1, &p1[i * p1_dim1 + 1], &c__1, &p2[k * p2_dim1 + 1] , &c__1);
            if (k != i) {
                d__1 = -temp;
                daxpy_(n, &d__1, &p1[k * p1_dim1 + 1], &c__1, &p2[i * p2_dim1 + 1], &c__1);
            }
            ii = i - k + 1;
            alp[ii + k * alp_dim1] += temp;
        }
    }

/* THIS ORTHONORMALIZES THE NEXT BLOCK */

    dortqr_(n, n, nblock, &p2[p2_offset], &bet[bet_offset]);

/* THIS STORES ALP AND BET IN T. */

    for (i = 1; i <= *nblock; ++i) {
        m = j - *nblock + i;
        for (k = i; k <= *nblock; ++k) {
            l = k - i + 1;
            t[l + m * t_dim1] = alp[l + i * alp_dim1];
        }
        for (k = 1; k <= i; ++k) {
            l = *nblock - i + k + 1;
            t[l + m * t_dim1] = bet[k + i * bet_dim1];
        }
    }

/* THIS NEGATES T IF SMALL IS FALSE. */

    if (! *small)
    for (i = j - *nblock + 1; i <= j; ++i) {
        for (k = 1; k <= l; ++k) {
            t[k + i * t_dim1] = -t[k + i * t_dim1];
        }
    }

/* THIS SHIFTS THE LANCZOS VECTORS */

    i__1 = *nblock * *n;
    dcopy_(&i__1, &p1[p1_offset], &c__1, &p0[p0_offset], &c__1);
    dcopy_(&i__1, &p2[p2_offset], &c__1, &p1[p1_offset], &c__1);
    i__1 = j - *nblock + 1;
    dlager_(&j, nband, &i__1, &t[t_offset], &tmin, &tmax);
    anorm = max(max(rnorm,tmax),-tmin);
    if (number == 0) {
        goto L305;
    }

/* THIS COMPUTES THE EXTREME EIGENVALUES OF ALP. */

    dcopy_(nblock, &dzero, &c__0, &p2[p2_offset], &c__1);
    dlaeig_(nblock, nblock, &c__1, &c__1, &alp[alp_offset], &tarr, nblock, &p2[
            p2_offset], &bound[1], &atemp[1], &d[1], &vtemp[1], eps, &tmin, &tmax);
    alpmin = tarr;
    dcopy_(nblock, &dzero, &c__0, &p2[p2_offset], &c__1);
    dlaeig_(nblock, nblock, nblock, nblock, &alp[alp_offset], &tarr, nblock, &
            p2[p2_offset], &bound[1], &atemp[1], &d[1], &vtemp[1], eps, &tmin, &tmax);
    alpmax = tarr;

/* THIS COMPUTES ALP = BET(TRANSPOSE)*BET. */

L305:
    for (i = 1; i <= *nblock; ++i) {
        for (k = 1; k <= i; ++k) {
            l = i - k + 1;
            i__1 = *nblock - i + 1;
            alp[l + k * alp_dim1] = ddot_(&i__1, &bet[i + i * bet_dim1], nblock,
                                          &bet[k + i * bet_dim1], nblock);
        }
    }
    if (number == 0) {
        goto L330;
    }

/* THIS COMPUTES THE SMALLEST SINGULAR VALUE OF BET. */

    dcopy_(nblock, &dzero, &c__0, &p2[p2_offset], &c__1);
    d__1 = anorm * anorm;
    dlaeig_(nblock, nblock, &c__1, &c__1, &alp[alp_offset], &tarr, nblock, &p2[p2_offset],
            &bound[1], &atemp[1], &d[1], &vtemp[1], eps, &c_b88, &d__1);
    betmin = sqrt(tarr);

/* THIS UPDATES TAU AND OTAU. */

    for (i = 1; i <= number; ++i) {
        temp = (tau[i] * max(alpmax - val[i],val[i] - alpmin) + otau[i] * betmax + *eps * anorm) / betmin;
        if (i <= *nperm) {
            temp += res[i] / betmin;
        }
        otau[i] = tau[i];
        tau[i] = temp;
    }

/* THIS COMPUTES THE LARGEST SINGULAR VALUE OF BET. */

L330:
    dcopy_(nblock, &dzero, &c__0, &p2[p2_offset], &c__1);
    d__1 = anorm * anorm;
    dlaeig_(nblock, nblock, nblock, nblock, &alp[alp_offset], &tarr, nblock, &p2[p2_offset],
            &bound[1], &atemp[1], &d[1], &vtemp[1], eps, &c_b88, &d__1);
    betmax = sqrt(tarr);
    if (j <= *nblock << 1) {
        goto L80;
    }

/* ------------------------------------------------------------------ */

/* THIS SECTION COMPUTES AND EXAMINES THE SMALLEST NONGOOD AND */
/* LARGEST DESIRED EIGENVALUES OF T TO SEE IF A CLOSER LOOK */
/* IS JUSTIFIED. */

    tolg = epsrt * anorm;
    tola = utol * rnorm;
    if (*maxj - j < *nblock || *nop >= *maxop && nleft != 0) {
        goto L390;
    }
    goto L400;

/* ------------------------------------------------------------------ */

/* THIS SECTION COMPUTES SOME EIGENVALUES AND EIGENVECTORS OF T TO */
/* SEE IF FURTHER ACTION IS INDICATED, ENTRY IS AT 380 OR 390 IF AN */
/* ITERATION (OR TERMINATION) IS KNOWN TO BE NEEDED, OTHERWISE ENTRY */
/* IS AT 400. */

L380:
    j -= *nblock;
    *ierr = -8;
L390:
    if (nleft == 0) {
        return;
    }
    test = TRUE_;
L400:
    ntheta = min(j/2, nleft+1);
    dlaeig_(&j, nband, &c__1, &ntheta, &t[t_offset], &val[number + 1], maxj, &
            s[s_offset], &bound[1], &atemp[1], &d[1], &vtemp[1], eps, &tmin, &tmax);
    dmvpc_(nblock, &bet[bet_offset], maxj, &j, &s[s_offset], &ntheta, &atemp[1], &vtemp[1], &d[1]);

/* THIS CHECKS FOR TERMINATION OF A CHECK RUN */

    if (nleft != 0 || j < *nblock * 6) {
        goto L410;
    }
    if (val[number + 1] - atemp[1] > val[*nperm] - tola) {
        goto L790;
    }

/* THIS UPDATES NLEFT BY EXAMINING THE COMPUTED EIGENVALUES OF T */
/* TO DETERMINE IF SOME PERMANENT VALUES ARE NO LONGER DESIRED. */

L410:
    if (ntheta <= nleft) {
        goto L470;
    }
    if (*nperm == 0) {
        goto L430;
    }
    m = number + nleft + 1;
    if (val[m] >= val[*nperm]) {
        goto L430;
    }
    --(*nperm);
    ngood = 0;
    number = *nperm;
    ++nleft;
    goto L400;

/* THIS UPDATES DELTA. */

L430:
    m = number + nleft + 1;
    *delta = min(*delta,val[m]);
    enough = TRUE_;
    if (nleft == 0) {
        goto L80;
    }
    ntheta = nleft;
    vtemp[ntheta + 1] = 1.;

/* ------------------------------------------------------------------ */

/* THIS SECTION EXAMINES THE COMPUTED EIGENPAIRS IN DETAIL. */

/* THIS CHECKS FOR ENOUGH ACCEPTABLE VALUES. */

    if (! (test || enough)) {
        goto L470;
    }
    *delta = min(*delta,anorm);
    pnorm = max(rnorm,max(-val[number + 1],*delta));
    tola = utol * pnorm;
    nstart = 0;
    for (i = 1; i <= ntheta; ++i) {
        m = number + i;
        if (min(atemp[i] * atemp[i] / (*delta - val[m]),atemp[i]) <= tola) {
            ind[i] = -1;
            continue;
        }
        enough = FALSE_;
        if (! test) {
            goto L470;
        }
        ind[i] = 1;
        ++nstart;
    }

/*  COPY VALUES OF IND INTO VTEMP */

    for (i = 1; i <= ntheta; ++i) {
        vtemp[i] = (doublereal) ((real) ind[i]);
    }
    goto L500;

/* THIS CHECKS FOR NEW GOOD VECTORS. */

L470:
    ng = 0;
    for (i = 1; i <= ntheta; ++i) {
        if (vtemp[i] > tolg) {
            vtemp[i] = 1.;
        }
        else {
            ++ng;
            vtemp[i] = -1.;
        }
    }

    if (ng <= ngood) {
        goto L80;
    }
    nstart = ntheta - ng;

/* ------------------------------------------------------------------ */

/* THIS SECTION COMPUTES AND NORMALIZES THE INDICATED RITZ VECTORS. */
/* IF NEEDED (TEST = .TRUE.), NEW STARTING VECTORS ARE COMPUTED. */

L500:
    test = test && ! enough;
    ngood = ntheta - nstart;
    ++nstart;
    ++ntheta;

/* THIS ALIGNS THE DESIRED (ACCEPTABLE OR GOOD) EIGENVALUES AND */
/* EIGENVECTORS OF T.  THE OTHER EIGENVECTORS ARE SAVED FOR */
/* FORMING STARTING VECTORS, IF NECESSARY.  IT ALSO SHIFTS THE */
/* EIGENVALUES TO OVERWRITE THE GOOD VALUES FROM THE PREVIOUS */
/* PAUSE. */

    dcopy_(&ntheta, &val[number + 1], &c__1, &val[*nperm + 1], &c__1);
    if (nstart == 0) {
        goto L580;
    }
    if (nstart != ntheta)
        dvsort_(&ntheta, &vtemp[1], &atemp[1], &c__1, &val[*nperm + 1], maxj, &j, &s[s_offset]);

/* THES ACCUMULATES THE J-VECTORS USED TO FORM THE STARTING */
/* VECTORS. */

    if (! test) {
        nstart = 0;
    }
    if (! test) {
        goto L580;
    }

/*  FIND MINIMUM ATEMP VALUE TO AVOID POSSIBLE OVERFLOW */

    temp = atemp[1];
    for (i = 1; i <= nstart; ++i) {
        temp = min(temp,atemp[i]);
    }
    m = ngood + 1;
    l = ngood + min(nstart,*nblock);
    for (i = m; i <= l; ++i) {
        d__1 = temp / atemp[i];
        dscal_(&j, &d__1, &s[i * s_dim1 + 1], &c__1);
    }
    m = (nstart - 1) / *nblock;
    l = ngood + *nblock;
    for (i = 1; i <= m; ++i) {
        for (k = 1; k <= *nblock; ++k) {
            ++l;
            if (l > ntheta) {
                goto L570;
            }
            i1 = ngood + k;
            d__1 = temp / atemp[l];
            daxpy_(&j, &d__1, &s[l * s_dim1 + 1], &c__1, &s[i1 * s_dim1 + 1], &c__1);
        }
    }
L570:
    nstart = min(nstart,*nblock);

/* THIS STORES THE RESIDUAL NORMS OF THE NEW PERMANENT VECTORS. */

L580:
    if (test || enough)
    for (i = 1; i <= ngood; ++i) {
        m = *nperm + i;
        res[m] = atemp[i];
    }

/* THIS COMPUTES THE RITZ VECTORS BY SEQUENTIALLY RECALLING THE */
/* LANCZOS VECTORS. */

    number = *nperm + ngood;
    if (test || enough) {
        i__1 = *n * *nblock;
        dcopy_(&i__1, &dzero, &c__0, &p1[p1_offset], &c__1);
    }
    if (ngood != 0)
    for (i = *nperm + 1; i <= number; ++i) {
        dcopy_(n, &dzero, &c__0, &vec[i * vec_dim1 + 1], &c__1);
    }
    for (i = *nblock; *nblock < 0 ? i >= j : i <= j; i += *nblock) {
        (*iovect)(n, nblock, &p2[p2_offset], &i, &c__1);
        for (k = 1; k <= *nblock; ++k) {
            m = i - *nblock + k;
            for (l = 1; l <= nstart; ++l) {
                i1 = ngood + l;
                daxpy_(n, &s[m + i1 * s_dim1], &p2[k * p2_dim1 + 1], &c__1, &p1[l * p1_dim1 + 1], &c__1);
            }
            for (l = 1; l <= ngood; ++l) {
                i1 = l + *nperm;
                daxpy_(n, &s[m + l * s_dim1], &p2[k * p2_dim1 + 1], &c__1, &vec[i1 * vec_dim1 + 1], &c__1);
            }
        }
    }
    if (test || enough) {
        goto L690;
    }

/* THIS NORMALIZES THE RITZ VECTORS AND INITIALIZES THE */
/* TAU RECURRENCE. */

    m = *nperm + 1;
    for (i = m; i <= number; ++i) {
        temp = 1. / dnrm2_(n, &vec[i * vec_dim1 + 1], &c__1);
        dscal_(n, &temp, &vec[i * vec_dim1 + 1], &c__1);
        tau[i] = 1.;
        otau[i] = 1.;
    }

/*  SHIFT S VECTORS TO ALIGN FOR LATER CALL TO DLAEIG */

    dcopy_(&ntheta, &val[*nperm + 1], &c__1, &vtemp[1], &c__1);
    dvsort_(&ntheta, &vtemp[1], &atemp[1], &c__0, &tarr, maxj, &j, &s[s_offset]);
    goto L80;

/* ------------------------------------------------------------------ */

/* THIS SECTION PREPARES TO ITERATE THE ALGORITHM BY SORTING THE */
/* PERMANENT VALUES, RESETTING SOME PARAMETERS, AND ORTHONORMALIZING */
/* THE PERMANENT VECTORS. */

L690:
    if (ngood == 0 && *nop >= *maxop) {
        goto L810;
    }
    if (ngood == 0) {
        goto L30;
    }

/* THIS ORTHONORMALIZES THE VECTORS */

    i__1 = *nperm + ngood;
    dortqr_(nmvec, n, &i__1, &vec[vec_offset], &s[s_offset]);

/* THIS SORTS THE VALUES AND VECTORS. */

    if (*nperm != 0) {
        i__1 = *nperm + ngood;
        dvsort_(&i__1, &val[1], &res[1], &c__0, &temp, nmvec, n, &vec[vec_offset]);
    }
    *nperm += ngood;
    nleft -= ngood;
    rnorm = max(-val[1],val[*nperm]);

/* THIS DECIDES WHERE TO GO NEXT. */

    if (*nop >= *maxop && nleft != 0) {
        goto L810;
    }
    if (nleft != 0) {
        goto L30;
    }
    if (val[*nval] - val[1] < tola) {
        goto L790;
    }

/* THIS DOES A CLUSTER TEST TO SEE IF A CHECK RUN IS NEEDED */
/* TO LOOK FOR UNDISCLOSED MULTIPLICITIES. */

    m = *nperm - *nblock + 1;
    if (m <= 0) {
        return;
    }
    for (i = 1; i <= m; ++i) {
        l = i + *nblock - 1;
        if (val[l] - val[i] < tola) {
            goto L30;
        }
    }

/* THIS DOES A CLUSTER TEST TO SEE IF A FINAL RAYLEIGH-RITZ */
/* PROCEDURE IS NEEDED. */

L790:
    m = *nperm - *nblock;
    if (m <= 0) {
        return;
    }
    for (i = 1; i <= m; ++i) {
        l = i + *nblock;
        if (val[l] - val[i] < tola) {
            *raritz = TRUE_;
            return;
        }
    }

    return;

/* ------------------------------------------------------------------ */

/* THIS REPORTS THAT MAXOP WAS EXCEEDED. */

L810:
    *ierr = -2;
    goto L790;

} /* dnwla_ */


/* *********************************************************************** */

/* Subroutine */ void dlabax_(n, nband, a, x, y)
integer *n, *nband;
doublereal *a, *x, *y;
{
    /* Local variables */
    static doublereal zero = 0.;
    static integer i, k, m;
    extern /* Subroutine */ void dcopy_();

/*  THIS SUBROUTINE SETS Y = A*X */
/*  WHERE X AND Y ARE VECTORS OF LENGTH N */
/*  AND A IS AN  N X NBAND  SYMMETRIC BAND MATRIX */

    dcopy_(n, &zero, &c__0, y, &c__1);
    for (k = 0; k < *n; ++k) {
        y[k] += a[k * *nband] * x[k];
        m = min(*n - k,*nband);
        for (i = 1; i < m; ++i) {
            y[k+i] += a[i + k * *nband] * x[k];
            y[k]   += a[i + k * *nband] * x[k+i];
        }
    }
} /* dlabax_ */


/* *********************************************************************** */

/* Subroutine */ void dlabcm_(n, nband, nl, nr, a, eigval, lde, eigvec, atol, artol, bound, atemp, d, vtemp)
integer *n, *nband, *nl, *nr;
doublereal *a, *eigval;
integer *lde;
doublereal *eigvec, *atol, *artol, *bound, *atemp, *d, *vtemp;
{
    /* System generated locals */
    integer a_dim1, a_offset, eigvec_dim1, eigvec_offset, i__1;
    doublereal d__1;

    /* Local variables */
    static logical flag__;
    static doublereal errb;
    extern doublereal ddot_();
    static integer nval, numl;
    extern doublereal dnrm2_();
    static integer i, j, m;
    extern /* Subroutine */ void dscal_();
    static doublereal sigma, resid;
    extern /* Subroutine */ void dcopy_(), daxpy_();
    static doublereal vnorm;
    extern /* Subroutine */ void dlabfc_(), dlabax_(), dlaran_();
    static doublereal rq;
    static integer numvec;
    static doublereal gap;


/*  THIS SUBROUTINE ORGANIZES THE CALCULATION OF THE EIGENVALUES */
/*  FOR THE BNDEIG PACKAGE.  EIGENVALUES ARE COMPUTED BY */
/*  A MODIFIED RAYLEIGH QUOTIENT ITERATION.  THE EIGENVALUE COUNT */
/*  OBTAINED BY EACH FACTORIZATION IS USED TO OCCASIONALLY OVERRIDE */
/*  THE COMPUTED RAYLEIGH QUOTIENT WITH A DIFFERENT SHIFT TO */
/*  INSURE CONVERGENCE TO THE DESIRED EIGENVALUES. */

/*  SUBROUTINES CALLED */
/*     DLABAX, DLABFC, DLARAN, DAXPY, DCOPY, DSCAL */

    /* Parameter adjustments */
    a_dim1 = *nband;
    a_offset = a_dim1 + 1;
    a -= a_offset;
    --eigval;
    eigvec_dim1 = *lde;
    eigvec_offset = eigvec_dim1 + 1;
    eigvec -= eigvec_offset;
    bound -= 3;
    --atemp;
    --d;
    --vtemp;

/*  REPLACE ZERO VECTORS BY RANDOM */

    nval = *nr - *nl + 1;
    flag__ = FALSE_;
    for (i = 1; i <= nval; ++i) {
        if (ddot_(n, &eigvec[i * eigvec_dim1 + 1], &c__1, &eigvec[i *
                eigvec_dim1 + 1], &c__1) == 0.) {
            dlaran_(n, &eigvec[i * eigvec_dim1 + 1]);
        }
    }

/*  LOOP OVER EIGENVALUES */

    sigma = bound[(nval << 1) + 4];
    for (j = 1; j <= nval; ++j) {
        numl = j;

/*  PREPARE TO COMPUTE FIRST RAYLEIGH QUOTIENT */

L10:
        dlabax_(n, nband, &a[a_offset], &eigvec[j * eigvec_dim1 + 1], &vtemp[1]);
        vnorm = dnrm2_(n, &vtemp[1], &c__1);
        if (vnorm == 0.) {
            goto L20;
        }
        d__1 = 1. / vnorm;
        dscal_(n, &d__1, &vtemp[1], &c__1);
        d__1 = 1. / vnorm;
        dscal_(n, &d__1, &eigvec[j * eigvec_dim1 + 1], &c__1);
        d__1 = -sigma;
        daxpy_(n, &d__1, &eigvec[j * eigvec_dim1 + 1], &c__1, &vtemp[1], &c__1);

/*  LOOP OVER SHIFTS */

/*  COMPUTE RAYLEIGH QUOTIENT, RESIDUAL NORM, AND CURRENT TOLERANCE */

L20:
        vnorm = dnrm2_(n, &eigvec[j * eigvec_dim1 + 1], &c__1);
        if (vnorm != 0.) {
            goto L30;
        }
        dlaran_(n, &eigvec[j * eigvec_dim1 + 1]);
        goto L10;

L30:
        rq = sigma + ddot_(n, &eigvec[j * eigvec_dim1 + 1], &c__1, &vtemp[1], &c__1) / vnorm / vnorm;
        d__1 = sigma - rq;
        daxpy_(n, &d__1, &eigvec[j * eigvec_dim1 + 1], &c__1, &vtemp[1], &c__1);
        resid = max(*atol,dnrm2_(n, &vtemp[1], &c__1) / vnorm);
        d__1 = 1.f / vnorm;
        dscal_(n, &d__1, &eigvec[j * eigvec_dim1 + 1], &c__1);

/*  ACCEPT EIGENVALUE IF THE INTERVAL IS SMALL ENOUGH */

        if (bound[(j << 1) + 4] - bound[(j << 1) + 3] < *atol * 3.) {
            goto L300;
        }

/*  COMPUTE MINIMAL ERROR BOUND */

        errb = resid;
        gap = min(bound[(j << 1) + 5] - rq,rq - bound[(j << 1) + 2]);
        if (gap > resid) {
            errb = max(*atol,resid * resid / gap);
        }

/*  TENTATIVE NEW SHIFT */

        sigma = (bound[(j << 1) + 3] + bound[(j << 1) + 4]) * .5;

/*  CHECK FOR TERMINALTION */

        if (resid > *atol * 2.) {
            goto L40;
        }
        if (rq - errb > bound[(j << 1) + 2] && rq + errb < bound[(j << 1) + 5]) {
            goto L310;
        }

/*  RQ IS TO THE LEFT OF THE INTERVAL */

L40:
        if (rq >= bound[(j << 1) + 3]) {
            goto L50;
        }
        if (rq - errb > bound[(j << 1) + 2]) {
            goto L100;
        }
        if (rq + errb < bound[(j << 1) + 3]) {
            dlaran_(n, &eigvec[j * eigvec_dim1 + 1]);
        }
        goto L200;

/*  RQ IS TO THE RIGHT OF THE INTERVAL */

L50:
        if (rq <= bound[(j << 1) + 4]) {
            goto L100;
        }
        if (rq + errb < bound[(j << 1) + 5]) {
            goto L100;
        }

/*  SAVE THE REJECTED VECTOR IF INDICATED */

        if (rq - errb <= bound[(j << 1) + 4]) {
            goto L200;
        }
        for (i = j; i <= nval; ++i) {
            if (bound[(i << 1) + 4] > rq) {
                goto L70;
            }
        }
        goto L80;
L70:
        dcopy_(n, &eigvec[j * eigvec_dim1 + 1], &c__1, &eigvec[i * eigvec_dim1 + 1], &c__1);
L80:
        dlaran_(n, &eigvec[j * eigvec_dim1 + 1]);
        goto L200;

/*  PERTURB RQ TOWARD THE MIDDLE */

L100:
        if (sigma < rq) {
            sigma = max(sigma,rq - errb);
        }
        if (sigma > rq) {
            sigma = min(sigma,rq + errb);
        }

/*  FACTOR AND SOLVE */

L200:
        for (i = j; i <= nval; ++i) {
            if (sigma < bound[(i << 1) + 3]) {
                break;
            }
        }
        numvec = i - j;
        numvec = min(numvec,*nband + 2);
        if (resid < *artol) {
            numvec = min(1,numvec);
        }
        dcopy_(n, &eigvec[j * eigvec_dim1 + 1], &c__1, &vtemp[1], &c__1);
        i__1 = (*nband << 1) - 1;
        dlabfc_(n, nband, &a[a_offset], &sigma, &numvec, lde, &eigvec[j *
                eigvec_dim1 + 1], &numl, &i__1, &atemp[1], &d[1], atol);

/*  PARTIALLY SCALE EXTRA VECTORS TO PREVENT UNDERFLOW OR OVERFLOW */

        for (i = 1; i < numvec; ++i) {
            m = j + i;
            d__1 = 1. / vnorm;
            dscal_(n, &d__1, &eigvec[m * eigvec_dim1 + 1], &c__1);
        }

/*  UPDATE INTERVALS */

        numl -= *nl - 1;
        if (numl >= 0) {
            bound[4] = min(bound[4],sigma);
        }
        for (i = j; i <= nval; ++i) {
            if (sigma < bound[(i << 1) + 3]) {
                goto L20;
            }
            if (numl < i) {
                bound[(i << 1) + 3] = sigma;
            }
            if (numl >= i) {
                bound[(i << 1) + 4] = sigma;
            }
        }
        if (numl < nval + 1) {
            if (sigma > bound[(nval << 1) + 5])
                bound[(nval << 1) + 5] = sigma;
        }
        goto L20;

/*  ACCEPT AN EIGENPAIR */

L300:
        dlaran_(n, &eigvec[j * eigvec_dim1 + 1]);
        flag__ = TRUE_;
        goto L310;

L305:
        flag__ = FALSE_;
        rq = (bound[(j << 1) + 3] + bound[(j << 1) + 4]) * .5;
        i__1 = (*nband << 1) - 1;
        dlabfc_(n, nband, &a[a_offset], &rq, &numvec, lde, &eigvec[j *
                eigvec_dim1 + 1], &numl, &i__1, &atemp[1], &d[1], atol);
        vnorm = dnrm2_(n, &eigvec[j * eigvec_dim1 + 1], &c__1);
        if (vnorm != 0.f) {
            d__1 = 1. / vnorm;
            dscal_(n, &d__1, &eigvec[j * eigvec_dim1 + 1], &c__1);
        }

/*  ORTHOGONALIZE THE NEW EIGENVECTOR AGAINST THE OLD ONES */

L310:
        eigval[j] = rq;
        if (j == 1) {
            goto L330;
        }
        m = j - 1;
        for (i = 1; i <= m; ++i) {
            d__1 = -ddot_(n, &eigvec[i * eigvec_dim1 + 1], &c__1, &eigvec[j * eigvec_dim1 + 1], &c__1);
            daxpy_(n, &d__1, &eigvec[i * eigvec_dim1 + 1], &c__1, &eigvec[j * eigvec_dim1 + 1], &c__1);
        }
L330:
        vnorm = dnrm2_(n, &eigvec[j * eigvec_dim1 + 1], &c__1);
        if (vnorm == 0.) {
            goto L305;
        }
        d__1 = 1. / vnorm;
        dscal_(n, &d__1, &eigvec[j * eigvec_dim1 + 1], &c__1);

/*   ORTHOGONALIZE LATER VECTORS AGAINST THE CONVERGED ONE */

        if (flag__) {
            goto L305;
        }
        if (j == nval) {
            return;
        }
        m = j + 1;
        for (i = m; i <= nval; ++i) {
            d__1 = -ddot_(n, &eigvec[j * eigvec_dim1 + 1], &c__1, &eigvec[i * eigvec_dim1 + 1], &c__1);
            daxpy_(n, &d__1, &eigvec[j * eigvec_dim1 + 1], &c__1, &eigvec[i * eigvec_dim1 + 1], &c__1);
        }
    }
} /* dlabcm_ */


/* *********************************************************************** */

/* Subroutine */ void dlabfc_(n, nband, a, sigma, number, lde, eigvec, numl, ldad, atemp, d, atol)
integer *n, *nband;
doublereal *a, *sigma;
integer *number, *lde;
doublereal *eigvec;
integer *numl, *ldad;
doublereal *atemp, *d, *atol;
{
    /* System generated locals */
    integer a_dim1, a_offset, eigvec_dim1, eigvec_offset, atemp_dim1,
            atemp_offset, d_dim1, d_offset, i__1;
    doublereal d__1;

    /* Builtin functions */
    double d_sign();

    /* Local variables */
    static doublereal zero;
    static integer i, j, k, l, m;
    extern /* Subroutine */ void dcopy_(), dswap_(), daxpy_();
    static integer la, ld, kk, nb1, lpm;


/*  THIS SUBROUTINE FACTORS (A-SIGMA*I) WHERE A IS A GIVEN BAND */
/*  MATRIX AND SIGMA IS AN INPUT PARAMETER.  IT ALSO SOLVES ZERO */
/*  OR MORE SYSTEMS OF LINEAR EQUATIONS.  IT RETURNS THE NUMBER */
/*  OF EIGENVALUES OF A LESS THAN SIGMA BY COUNTING THE STURM */
/*  SEQUENCE DURING THE FACTORIZATION.  TO OBTAIN THE STURM */
/*  SEQUENCE COUNT WHILE ALLOWING NON-SYMMETRIC PIVOTING FOR */
/*  STABILITY, THE CODE USES A GUPTA'S MULTIPLE PIVOTING */
/*  ALGORITHM. */

/*  SUBROUTINES CALLED */
/*     DAXPY, DCOPY, DSWAP */

    /* Parameter adjustments */
    a_dim1 = *nband;
    a_offset = a_dim1 + 1;
    a -= a_offset;
    eigvec_dim1 = *lde;
    eigvec_offset = eigvec_dim1 + 1;
    eigvec -= eigvec_offset;
    d_dim1 = *ldad;
    d_offset = d_dim1 + 1;
    d -= d_offset;
    atemp_dim1 = *ldad;
    atemp_offset = atemp_dim1 + 1;
    atemp -= atemp_offset;

/*  INITIALIZE */

    zero = 0.;
    nb1 = *nband - 1;
    *numl = 0;
    i__1 = *ldad * *nband;
    dcopy_(&i__1, &zero, &c__0, &d[d_offset], &c__1);

/*   LOOP OVER COLUMNS OF A */

    for (k = 1; k <= *n; ++k) {

/*   ADD A COLUMN OF A TO D */

        d[*nband + *nband * d_dim1] = a[k * a_dim1 + 1] - *sigma;
        m = min(k,*nband) - 1;
        for (i = 1; i <= m; ++i) {
            la = k - i;
            ld = *nband - i;
            d[ld + *nband * d_dim1] = a[i + 1 + la * a_dim1];
        }
        m = min(*n - k,nb1);
        for (i = 1; i <= m; ++i) {
            ld = *nband + i;
            d[ld + *nband * d_dim1] = a[i + 1 + k * a_dim1];
        }

/*   TERMINATE */

        lpm = 1;
        for (i = 1; i <= nb1; ++i) {
            l = k - *nband + i;
            if (d[i + *nband * d_dim1] == 0.) {
                continue;
            }
            if (abs(d[i + i * d_dim1]) >= abs(d[i + *nband * d_dim1])) {
                goto L50;
            }
            if (d[i + *nband * d_dim1] < 0. && d[i + i * d_dim1] < 0. || d[i
                    + *nband * d_dim1] > 0. && d[i + i * d_dim1] >= 0.) {
                lpm = -lpm;
            }
            i__1 = *ldad - i + 1;
            dswap_(&i__1, &d[i + i * d_dim1], &c__1, &d[i + *nband * d_dim1], &c__1);
            dswap_(number, &eigvec[l + eigvec_dim1], lde, &eigvec[k + eigvec_dim1], lde);
L50:
            i__1 = *ldad - i;
            d__1 = -d[i + *nband * d_dim1] / d[i + i * d_dim1];
            daxpy_(&i__1, &d__1, &d[i + 1 + i * d_dim1], &c__1, &d[i + 1 + *nband * d_dim1], &c__1);
            d__1 = -d[i + *nband * d_dim1] / d[i + i * d_dim1];
            daxpy_(number, &d__1, &eigvec[l + eigvec_dim1], lde, &eigvec[k + eigvec_dim1], lde);
        }

/*  UPDATE STURM SEQUENCE COUNT */

        if (d[*nband + *nband * d_dim1] < 0.) {
            lpm = -lpm;
        }
        if (lpm < 0) {
            ++(*numl);
        }
        if (k == *n) {
            goto L110;
        }

/*   COPY FIRST COLUMN OF D INTO ATEMP */
        if (k >= *nband) {
            l = k - nb1;
            dcopy_(ldad, &d[d_offset], &c__1, &atemp[l * atemp_dim1 + 1], &c__1);
        }

/*   SHIFT THE COLUMNS OF D OVER AND UP */

        for (i = 1; i <= nb1; ++i) {
            i__1 = *ldad - i;
            dcopy_(&i__1, &d[i + 1 + (i + 1) * d_dim1], &c__1, &d[i + i * d_dim1], &c__1);
            d[*ldad + i * d_dim1] = 0.;
        }
    }

/*  TRANSFER D TO ATEMP */

L110:
    for (i = 1; i <= *nband; ++i) {
        l = *n - *nband + i;
        i__1 = *nband - i + 1;
        dcopy_(&i__1, &d[i + i * d_dim1], &c__1, &atemp[l * atemp_dim1 + 1], &c__1);
    }

/*   BACK SUBSTITUTION */

    if (*number == 0) {
        return;
    }
    for (kk = 1; kk <= *n; ++kk) {
        k = *n - kk + 1;
        if (abs(atemp[k * atemp_dim1 + 1]) <= *atol) {
            atemp[k * atemp_dim1 + 1] = d_sign(atol, &atemp[k * atemp_dim1 + 1]);
        }

        for (i = 1; i <= *number; ++i) {
            eigvec[k + i * eigvec_dim1] /= atemp[k * atemp_dim1 + 1];
            m = min(*ldad,k) - 1;
            for (j = 1; j <= m; ++j) {
                l = k - j;
                eigvec[l + i * eigvec_dim1] -= atemp[j + 1 + l * atemp_dim1] *
                         eigvec[k + i * eigvec_dim1];
            }
        }
    }
} /* dlabfc_ */



/* Subroutine */ void dlaeig_(n, nband, nl, nr, a, eigval, lde, eigvec, bound,
        atemp, d, vtemp, eps, tmin, tmax)
integer *n, *nband, *nl, *nr;
doublereal *a, *eigval;
integer *lde;
doublereal *eigvec, *bound, *atemp, *d, *vtemp, *eps, *tmin, *tmax;
{
    /* System generated locals */
    integer a_dim1, a_offset, eigvec_dim1, eigvec_offset;

    /* Builtin functions */
    double sqrt();

    /* Local variables */
    static doublereal atol;
    static integer nval, i, m;
    static doublereal artol;
    extern /* Subroutine */ void dlabcm_();


/*  THIS IS A SPECIALIZED VERSION OF THE SUBROUTINE BNDEIG TAILORED */
/*  SPECIFICALLY FOR USE BY THE LASO PACKAGE. */


/*  LOCAL VARIABLES */


/*  FUNCTIONS CALLED */


/*  SUBROUTINES CALLED */

/*     DLABCM, DLABFC, DLAGER, DCOPY */

/*  SET PARAMETERS */

    /* Parameter adjustments */
    a_dim1 = *nband;
    a_offset = a_dim1 + 1;
    a -= a_offset;
    --eigval;
    eigvec_dim1 = *lde;
    eigvec_offset = eigvec_dim1 + 1;
    eigvec -= eigvec_offset;
    bound -= 3;
    --atemp;
    --d;
    --vtemp;

    atol = (doublereal) ((real) (*n)) * *eps * max(*tmax,-(*tmin));
    artol = atol / sqrt(*eps);
    nval = *nr - *nl + 1;

/*   CHECK FOR SPECIAL CASE OF N = 1 */

    if (*n == 1) {
        eigval[1] = a[a_dim1 + 1];
        eigvec[eigvec_dim1 + 1] = 1.;
        return;
    }

/*   SET UP INITIAL EIGENVALUE BOUNDS */

    m = nval + 1;
    for (i = 2; i <= m; ++i) {
        bound[(i << 1) + 1] = *tmin;
        bound[(i << 1) + 2] = *tmax;
    }
    bound[4] = *tmax;
    bound[(nval << 1) + 5] = *tmin;
    if (*nl == 1) {
        bound[4] = *tmin;
    }
    if (*nr == *n) {
        bound[(nval << 1) + 5] = *tmax;
    }

    dlabcm_(n, nband, nl, nr, &a[a_offset], &eigval[1], lde, &eigvec[
            eigvec_offset], &atol, &artol, &bound[3], &atemp[1], &d[1], &
            vtemp[1]);
} /* dlaeig_ */


/* *********************************************************************** */

/* Subroutine */ void dlager_(n, nband, nstart, a, tmin, tmax)
integer *n, *nband, *nstart;
doublereal *a, *tmin, *tmax;
{
    /* Local variables */
    static doublereal temp;
    static integer i, k, l, m;

/*  THIS SUBROUTINE COMPUTES BOUNDS ON THE SPECTRUM OF A BY */
/*  EXAMINING THE GERSCHGORIN CIRCLES. ONLY THE NEWLY CREATED */
/*  CIRCLES ARE EXAMINED */

    for (k = *nstart - 1; k < *n; ++k) {
        temp = 0.;
        for (i = 1; i < *nband; ++i) {
            temp += abs(a[i + k * *nband]);
        }
        l = min(k,*nband-1);
        for (i = 1; i <= l; ++i) {
            m = k - i;
            temp += abs(a[i + m * *nband]);
        }
        *tmin = min(*tmin,a[k * *nband] - temp);
        *tmax = max(*tmax,a[k * *nband] + temp);
    }
    return;
} /* dlager_ */


/* *********************************************************************** */

/* Subroutine */ void dlaran_(n, x)
integer *n;
doublereal *x;
{
    /* Initialized data */
    static integer iurand = 0;
    /* Local variables */
    static integer i;
    extern doublereal urand_();

/*  THIS SUBROUTINE SETS THE VECTOR X TO RANDOM NUMBERS */

/*  INITIALIZE SEED */

    for (i = 0; i < *n; ++i) {
        x[i] = (doublereal) urand_(&iurand) - .5;
    }
} /* dlaran_ */


/* ------------------------------------------------------------------ */

/* Subroutine */ void dmvpc_(nblock, bet, maxj, j, s, number, resnrm, orthcf, rv)
integer *nblock;
doublereal *bet;
integer *maxj, *j;
doublereal *s;
integer *number;
doublereal *resnrm, *orthcf, *rv;
{
    /* System generated locals */
    integer bet_dim1, bet_offset, s_dim1, s_offset;

    /* Local variables */
    extern doublereal ddot_(), dnrm2_();
    static integer i, k, m;

/* THIS SUBROUTINE COMPUTES THE NORM AND THE SMALLEST ELEMENT */
/* (IN ABSOLUTE VALUE) OF THE VECTOR BET*SJI, WHERE SJI */
/* IS AN NBLOCK VECTOR OF THE LAST NBLOCK ELEMENTS OF THE ITH */
/* EIGENVECTOR OF T.  THESE QUANTITIES ARE THE RESIDUAL NORM */
/* AND THE ORTHOGONALITY COEFFICIENT RESPECTIVELY FOR THE */
/* CORRESPONDING RITZ PAIR.  THE ORTHOGONALITY COEFFICIENT IS */
/* NORMALIZED TO ACCOUNT FOR THE LOCAL REORTHOGONALIZATION. */

    /* Parameter adjustments */
    bet_dim1 = *nblock;
    bet_offset = bet_dim1 + 1;
    bet -= bet_offset;
    s_dim1 = *maxj;
    s_offset = s_dim1 + 1;
    s -= s_offset;
    --resnrm;
    --orthcf;
    --rv;

    m = *j - *nblock + 1;
    for (i = 1; i <= *number; ++i) {
        for (k = 1; k <= *nblock; ++k) {
            rv[k] = ddot_(nblock, &s[m + i * s_dim1], &c__1, &bet[k +
                    bet_dim1], nblock);
            if (k == 1) {
                orthcf[i] = abs(rv[k]);
            }
            orthcf[i] = min(orthcf[i], abs(rv[k]));
        }
        resnrm[i] = dnrm2_(nblock, &rv[1], &c__1);
    }
} /* dmvpc_ */


/* ------------------------------------------------------------------ */

/* Subroutine */ void dnppla_(op, iovect, n, nperm, nop, nmval, val, nmvec,
        vec, nblock, h, hv, p, q, bound, d, delta, small, raritz, eps)
/* Subroutine */ void (*op) (), (*iovect) ();
integer *n, *nperm, *nop, *nmval;
doublereal *val;
integer *nmvec;
doublereal *vec;
integer *nblock;
doublereal *h, *hv, *p, *q, *bound, *d, *delta;
logical *small, *raritz;
doublereal *eps;
{
    /* System generated locals */
    integer val_dim1, val_offset, vec_dim1, vec_offset, h_dim1, h_offset,
            hv_dim1, hv_offset, p_dim1, p_offset, q_dim1, q_offset, i__1;
    doublereal d__1;

    /* Local variables */
    extern doublereal ddot_();
    static doublereal hmin, hmax, temp;
    extern doublereal dnrm2_();
    static integer i, j, k, l, m;
    extern /* Subroutine */ void dcopy_();
    static doublereal dzero;
    extern /* Subroutine */ void daxpy_();
    static integer jj;
    extern /* Subroutine */ void dlaeig_();
    static integer kk;
    extern /* Subroutine */ void dlager_();

/* THIS SUBROUTINE POST PROCESSES THE EIGENVECTORS.  BLOCK MATRIX */
/* VECTOR PRODUCTS ARE USED TO MINIMIZED THE NUMBER OF CALLS TO OP. */

    /* Parameter adjustments */
    q_dim1 = *n;
    q_offset = q_dim1 + 1;
    q -= q_offset;
    p_dim1 = *n;
    p_offset = p_dim1 + 1;
    p -= p_offset;
    hv_dim1 = *nperm;
    hv_offset = hv_dim1 + 1;
    hv -= hv_offset;
    h_dim1 = *nperm;
    h_offset = h_dim1 + 1;
    h -= h_offset;
    val_dim1 = *nmval;
    val_offset = val_dim1 + 1;
    val -= val_offset;
    vec_dim1 = *nmvec;
    vec_offset = vec_dim1 + 1;
    vec -= vec_offset;
    --bound;
    --d;

/* IF RARITZ IS .TRUE.  A FINAL RAYLEIGH-RITZ PROCEDURE IS APPLIED */
/* TO THE EIGENVECTORS. */

    dzero = 0.;
    if (! (*raritz)) {
        goto L190;
    }

/* ------------------------------------------------------------------ */

/* THIS CONSTRUCTS H=Q*AQ, WHERE THE COLUMNS OF Q ARE THE */
/* APPROXIMATE EIGENVECTORS.  TEMP = -1 IS USED WHEN SMALL IS */
/* FALSE TO AVOID HAVING TO RESORT THE EIGENVALUES AND EIGENVECTORS */
/* COMPUTED BY DLAEIG. */

    i__1 = *nperm * *nperm;
    dcopy_(&i__1, &dzero, &c__0, &h[h_offset], &c__1);
    temp = -1.;
    if (*small) {
        temp = 1.;
    }
    m = *nperm % *nblock;
    if (m == 0) {
        goto L40;
    }
    for (i = 1; i <= m; ++i) {
        dcopy_(n, &vec[i * vec_dim1 + 1], &c__1, &p[i * p_dim1 + 1], &c__1);
    }
    (*iovect)(n, &m, &p[p_offset], &m, &c__0);
    (*op)(n, &m, &p[p_offset], &q[q_offset]);
    ++(*nop);
    for (i = 1; i <= m; ++i) {
        for (j = i; j <= *nperm; ++j) {
            jj = j - i + 1;
            h[jj + i * h_dim1] = temp * ddot_(n, &vec[j * vec_dim1 + 1], &
                    c__1, &q[i * q_dim1 + 1], &c__1);
        }
    }
    if (*nperm < *nblock) {
        goto L90;
    }
L40:
    m += *nblock;
    for (i = m; *nblock < 0 ? i >= *nperm : i <= *nperm; i += *nblock) {
        for (j = 1; j <= *nblock; ++j) {
            l = i - *nblock + j;
            dcopy_(n, &vec[l * vec_dim1 + 1], &c__1, &p[j * p_dim1 + 1], &
                    c__1);
        }
        (*iovect)(n, nblock, &p[p_offset], &i, &c__0);
        (*op)(n, nblock, &p[p_offset], &q[q_offset]);
        ++(*nop);
        for (j = 1; j <= *nblock; ++j) {
            l = i - *nblock + j;
            for (k = l; k <= *nperm; ++k) {
                kk = k - l + 1;
                h[kk + l * h_dim1] = temp * ddot_(n, &vec[k * vec_dim1 + 1], &
                        c__1, &q[j * q_dim1 + 1], &c__1);
            }
        }
    }

/* THIS COMPUTES THE SPECTRAL DECOMPOSITION OF H. */

L90:
    hmin = h[h_dim1 + 1];
    hmax = h[h_dim1 + 1];
    dlager_(nperm, nperm, &c__1, &h[h_offset], &hmin, &hmax);
    dlaeig_(nperm, nperm, &c__1, nperm, &h[h_offset], &val[val_offset], nperm,
            &hv[hv_offset], &bound[1], &p[p_offset], &d[1], &q[q_offset], eps, &hmin, &hmax);

/* THIS COMPUTES THE RITZ VECTORS--THE COLUMNS OF */
/* Y = QS WHERE S IS THE MATRIX OF EIGENVECTORS OF H. */

    for (i = 1; i <= *nperm; ++i) {
        dcopy_(n, &dzero, &c__0, &vec[i * vec_dim1 + 1], &c__1);
    }
    m = *nperm % *nblock;
    if (m == 0) {
        goto L150;
    }
    (*iovect)(n, &m, &p[p_offset], &m, &c__1);
    for (i = 1; i <= m; ++i) {
        for (j = 1; j <= *nperm; ++j) {
            daxpy_(n, &hv[i + j * hv_dim1], &p[i * p_dim1 + 1], &c__1, &vec[j
                    * vec_dim1 + 1], &c__1);
        }
    }
    if (*nperm < *nblock) {
        goto L190;
    }
L150:
    m += *nblock;
    for (i = m; *nblock < 0 ? i >= *nperm : i <= *nperm; i += *nblock) {
        (*iovect)(n, nblock, &p[p_offset], &i, &c__1);
        for (j = 1; j <= *nblock; ++j) {
            l = i - *nblock + j;
            for (k = 1; k <= *nperm; ++k) {
                daxpy_(n, &hv[l + k * hv_dim1], &p[j * p_dim1 + 1], &c__1, &
                        vec[k * vec_dim1 + 1], &c__1);
            }
        }
    }

/* ------------------------------------------------------------------ */

/* THIS SECTION COMPUTES THE RAYLEIGH QUOTIENTS (IN VAL(*,1)) */
/* AND RESIDUAL NORMS (IN VAL(*,2)) OF THE EIGENVECTORS. */

L190:
    if (! (*small)) {
        *delta = -(*delta);
    }
    m = *nperm % *nblock;
    if (m == 0) {
        goto L220;
    }
    for (i = 1; i <= m; ++i) {
        dcopy_(n, &vec[i * vec_dim1 + 1], &c__1, &p[i * p_dim1 + 1], &c__1);
    }
    (*op)(n, &m, &p[p_offset], &q[q_offset]);
    ++(*nop);
    for (i = 1; i <= m; ++i) {
        val[i + val_dim1] = ddot_(n, &p[i * p_dim1 + 1], &c__1, &q[i * q_dim1 + 1], &c__1);
        d__1 = -val[i + val_dim1];
        daxpy_(n, &d__1, &p[i * p_dim1 + 1], &c__1, &q[i * q_dim1 + 1], &c__1);
        val[i + (val_dim1 << 1)] = dnrm2_(n, &q[i * q_dim1 + 1], &c__1);
    }
    if (*nperm < *nblock) {
        goto L260;
    }
L220:
    ++m;
    for (i = m; *nblock < 0 ? i >= *nperm : i <= *nperm; i += *nblock) {
        for (j = 1; j <= *nblock; ++j) {
            l = i - 1 + j;
            dcopy_(n, &vec[l * vec_dim1 + 1], &c__1, &p[j * p_dim1 + 1], &
                    c__1);
        }
        (*op)(n, nblock, &p[p_offset], &q[q_offset]);
        ++(*nop);
        for (j = 1; j <= *nblock; ++j) {
            l = i - 1 + j;
            val[l + val_dim1] = ddot_(n, &p[j * p_dim1 + 1], &c__1, &q[j *
                    q_dim1 + 1], &c__1);
            d__1 = -val[l + val_dim1];
            daxpy_(n, &d__1, &p[j * p_dim1 + 1], &c__1, &q[j * q_dim1 + 1], &c__1);
            val[l + (val_dim1 << 1)] = dnrm2_(n, &q[j * q_dim1 + 1], &c__1);
        }
    }

/* THIS COMPUTES THE ACCURACY ESTIMATES.  FOR CONSISTENCY WITH DILASO */

L260:
    for (i = 1; i <= *nperm; ++i) {
        temp = *delta - val[i + val_dim1];
        if (! (*small)) {
            temp = -temp;
        }
        val[i + (val_dim1 << 2)] = 0.;
        if (temp > 0.) {
            val[i + (val_dim1 << 2)] = val[i + (val_dim1 << 1)] / temp;
        }
        val[i + val_dim1 * 3] = val[i + (val_dim1 << 2)] * val[i + (val_dim1 << 1)];
    }

} /* dnppla_ */

/* ------------------------------------------------------------------ */

/* Subroutine */ void dortqr_(nz, n, nblock, z, b)
integer *nz, *n, *nblock;
doublereal *z, *b;
{
    /* System generated locals */
    integer z_dim1, z_offset, b_dim1, b_offset;
    doublereal d__1;

    /* Builtin functions */
    double d_sign();

    /* Local variables */
    extern doublereal ddot_();
    static doublereal temp;
    extern doublereal dnrm2_();
    static integer i, k, m;
    extern /* Subroutine */ void dscal_();
    static doublereal sigma;
    extern /* Subroutine */ void daxpy_();
    static integer length;
    static doublereal tau;

/* THIS SUBROUTINE COMPUTES THE QR FACTORIZATION OF THE N X NBLOCK */
/* MATRIX Z.  Q IS FORMED IN PLACE AND RETURNED IN Z.  R IS */
/* RETURNED IN B. */

    /* Parameter adjustments */
    z_dim1 = *nz;
    z_offset = z_dim1 + 1;
    z -= z_offset;
    b_dim1 = *nblock;
    b_offset = b_dim1 + 1;
    b -= b_offset;

/* THIS SECTION REDUCES Z TO TRIANGULAR FORM. */

    for (i = 1; i <= *nblock; ++i) {

/* THIS FORMS THE ITH REFLECTION. */

        length = *n - i + 1;
        d__1 = dnrm2_(&length, &z[i + i * z_dim1], &c__1);
        sigma = d_sign(&d__1, &z[i + i * z_dim1]);
        b[i + i * b_dim1] = -sigma;
        z[i + i * z_dim1] += sigma;
        tau = sigma * z[i + i * z_dim1];
        if (i == *nblock) {
            continue;
        }

/* THIS APPLIES THE ROTATION TO THE REST OF THE COLUMNS. */

        for (k = i+1; k <= *nblock; ++k) {
            if (tau != 0.) {
                temp = -ddot_(&length, &z[i + i * z_dim1], &c__1, &z[i + k * z_dim1], &c__1) / tau;
                daxpy_(&length, &temp, &z[i + i * z_dim1], &c__1, &z[i + k * z_dim1], &c__1);
            }
            b[i + k * b_dim1] = z[i + k * z_dim1];
            z[i + k * z_dim1] = 0.;
        }
    }

/* THIS ACCUMULATES THE REFLECTIONS IN REVERSE ORDER. */

    for (m = 1; m <= *nblock; ++m) {

/* THIS RECREATES THE ITH = NBLOCK-M+1)TH REFLECTION. */

        i = *nblock + 1 - m;
        sigma = -b[i + i * b_dim1];
        tau = z[i + i * z_dim1] * sigma;
        if (tau == 0.) {
            goto L60;
        }
        length = *n - *nblock + m;
        if (i == *nblock) {
            goto L50;
        }

/* THIS APPLIES IT TO THE LATER COLUMNS. */

        for (k = i+1; k <= *nblock; ++k) {
            temp = -ddot_(&length, &z[i + i * z_dim1], &c__1, &z[i + k * z_dim1], &c__1) / tau;
            daxpy_(&length, &temp, &z[i + i * z_dim1], &c__1, &z[i + k * z_dim1], &c__1);
        }
L50:
        d__1 = -1. / sigma;
        dscal_(&length, &d__1, &z[i + i * z_dim1], &c__1);
L60:
        z[i + i * z_dim1] += 1.;
    }
} /* dortqr_ */


/* ------------------------------------------------------------------- */

/* Subroutine */ void dvsort_(num, val, res, iflag, v, nmvec, n, vec)
integer *num;
doublereal *val, *res;
integer *iflag;
doublereal *v;
integer *nmvec, *n;
doublereal *vec;
{
    /* Local variables */
    static doublereal temp;
    static integer i, k, m;
    extern /* Subroutine */ void dswap_();

/*  THIS SUBROUTINE SORTS THE EIGENVALUES (VAL) IN ASCENDING ORDER */
/*  WHILE CONCURRENTLY SWAPPING THE RESIDUALS AND VECTORS. */

    for (m = *num - 1; m > 0; --m) {
        for (k = 0; k < m; ++k) {
            if (val[k] <= val[k+1])
                continue;
            temp = val[k]; val[k] = val[k+1]; val[k+1] = temp;
            temp = res[k]; res[k] = res[k+1]; res[k+1] = temp;
            dswap_(n, &vec[k * *nmvec], &c__1, &vec[(k+1) * *nmvec], &c__1);
            if (*iflag != 0) {
                temp = v[k]; v[k] = v[k+1]; v[k+1] = temp;
            }
        }
    }
} /* dvsort_ */

doublereal urand_(iy)
integer *iy;
{
    /* Initialized data */

    static integer m2 = 0;
    static integer itwo = 2;

    /* Builtin functions */
    double atan(), sqrt();

    /* Local variables */
    static integer m;
    static real s;
    static doublereal halfm;
    static integer ia, ic, mic;

/*      URAND IS A UNIFORM RANDOM NUMBER GENERATOR BASED  ON  THEORY  AND */
/*  SUGGESTIONS  GIVEN  IN  D.E. KNUTH (1969),  VOL  2.   THE INTEGER  IY */
/*  SHOULD BE INITIALIZED TO AN ARBITRARY INTEGER PRIOR TO THE FIRST CALL */
/*  TO URAND.  THE CALLING PROGRAM SHOULD  NOT  ALTER  THE  VALUE  OF  IY */
/*  BETWEEN  SUBSEQUENT CALLS TO URAND.  VALUES OF URAND WILL BE RETURNED */
/*  IN THE INTERVAL (0,1). */

    if (m2 != 0) {
        goto L20;
    }

/*  IF FIRST ENTRY, COMPUTE MACHINE INTEGER WORD LENGTH */

    m = 1;
    do {
        m2 = m;
        m = itwo * m2;
    } while (m > m2);

    halfm = (doublereal) m2;

/*  COMPUTE MULTIPLIER AND INCREMENT FOR LINEAR CONGRUENTIAL METHOD */

    ia = ((integer) (halfm * atan(1.) / 8.) << 3) + 5;
    ic = ((integer) (halfm * (.5 - sqrt(3.) / 6.)) << 1) + 1;
    mic = m2 - ic + m2;

/*  S IS THE SCALE FACTOR FOR CONVERTING TO FLOATING POINT */

    s = .5f / m2;

/*  COMPUTE NEXT RANDOM NUMBER */

L20:
    *iy *= ia;

/*  THE FOLLOWING STATEMENT IS FOR COMPUTERS WHICH DO NOT ALLOW */
/*  INTEGER OVERFLOW ON ADDITION */

    if (*iy > mic) {
        *iy = *iy - m2 - m2;
    }

    *iy += ic;

/*  THE FOLLOWING STATEMENT IS FOR COMPUTERS WHERE THE */
/*  WORD LENGTH FOR ADDITION IS GREATER THAN FOR MULTIPLICATION */

    if (*iy / 2 > m2) {
        *iy = *iy - m2 - m2;
    }

/*  THE FOLLOWING STATEMENT IS FOR COMPUTERS WHERE INTEGER */
/*  OVERFLOW AFFECTS THE SIGN BIT */

    if (*iy < 0) {
        *iy = *iy + m2 + m2;
    }
    return (real) (*iy) * s;
} /* urand_ */
