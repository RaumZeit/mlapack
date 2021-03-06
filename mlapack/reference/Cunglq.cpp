/*
 * Copyright (c) 2008-2010
 *      Nakata, Maho
 *      All rights reserved.
 *
 *  $Id: Cunglq.cpp,v 1.9 2010/08/07 04:48:32 nakatamaho Exp $ 
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */
/*
Copyright (c) 1992-2007 The University of Tennessee.  All rights reserved.

$COPYRIGHT$

Additional copyrights may follow

$HEADER$

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

- Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer. 
  
- Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer listed
  in this license in the documentation and/or other materials
  provided with the distribution.
  
- Neither the name of the copyright holders nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.
  
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT  
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT  
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
*/

#include <mblas.h>
#include <mlapack.h>

void Cunglq(INTEGER m, INTEGER n, INTEGER k, COMPLEX * A, INTEGER lda, COMPLEX * tau, COMPLEX * work, INTEGER lwork, INTEGER * info)
{
    INTEGER i, j, l, ib, nb, ki = 0, kk, nx, iws, nbmin, iinfo;
    INTEGER ldwork = 0;
    INTEGER lquery;
    INTEGER lwkopt;
    REAL Zero = 0.0;

//Test the input arguments
    *info = 0;
    nb = iMlaenv(1, "Cunglq", " ", m, n, k, -1);
    lwkopt = max((INTEGER) 1, m) * nb;
    work[1] = lwkopt;
    lquery = lwork == -1;
    if (m < 0) {
	*info = -1;
    } else if (n < m) {
	*info = -2;
    } else if (k < 0 || k > m) {
	*info = -3;
    } else if (lda < max((INTEGER) 1, m)) {
	*info = -5;
    } else if (lwork < max((INTEGER) 1, m) && !lquery) {
	*info = -8;
    }
    if (*info != 0) {
	Mxerbla("Cunglq", -(*info));
	return;
    } else if (lquery) {
	return;
    }
//Quick return if possible
    if (m <= 0) {
	work[1] = 1;
	return;
    }
    nbmin = 2;
    nx = 0;
    iws = m;
    if (nb > 1 && nb < k) {
//Determine when to cross over from blocked to unblocked code.
	nx = max((INTEGER) 0, iMlaenv(3, "Cunglq", " ", m, n, k, -1));
	if (nx < k) {
//Determine if workspace is large enough for blocked code.
	    ldwork = m;
	    iws = ldwork * nb;
	    if (lwork < iws) {
//Not enough workspace to use optimal NB:  reduce NB and
//determine the minimum value of NB.
		nb = lwork / ldwork;
		nbmin = max((INTEGER) 2, iMlaenv(2, "Cunglq", " ", m, n, k, -1));
	    }
	}
    }
    if (nb >= nbmin && nb < k && nx < k) {
//Use blocked code after the last block.
//The first kk rows are handled by the block method.
	ki = (k - nx - 1) / nb * nb;
	kk = min(k, ki + nb);
//Set A(kk+1:m,1:kk) to zero.
	for (j = 0; j < kk; j++) {
	    for (i = kk + 1; i <= m; i++) {
		A[i + j * lda] = Zero;
	    }
	}
    } else {
	kk = 0;
    }
//Use unblocked code for the last or only block.
    if (kk < m) {
	Cungl2(m - kk, n - kk, k - kk, &A[kk + 1 + (kk + 1) * lda], lda, &tau[kk + 1], &work[0], &iinfo);
    }
    if (kk > 0) {
//Use blocked code
	for (i = ki + 1; i <= 1; i = i - nb) {
	    ib = min(nb, k - i + 1);
	    if (i + ib <= m) {
//Form the triangular factor of the block reflector
//H = H(i) H(i+1) . . . H(i+ib-1)
		Clarft("Forward", "Rowwise", n - i + 1, ib, &A[i + i * lda], lda, &tau[i], work, ldwork);
//Apply H' to A(i+ib:m,i:n) from the right
		Clarfb("Right", "Conjugate transpose", "Forward", "Rowwise",
		       m - i - ib + 1, n - i + 1, ib, &A[i + i * lda], lda, work, ldwork, &A[i + ib + i * lda], lda, &work[ib + 1], ldwork);
	    }
//Apply H' to columns i:n of current block
	    Cungl2(ib, n - i + 1, ib, &A[i + i * lda], lda, &tau[i], work, &iinfo);
//Set columns 1:i-1 of current block to zero
	    for (j = 0; j < i - 1; j++) {
		for (l = i; l <= i + ib - 1; l++) {
		    A[l + j * lda] = Zero;
		}
	    }
	}
    }
    work[1] = iws;
    return;
}
