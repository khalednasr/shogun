#include "lib/common.h"
#include "kernel/LinearWordKernel.h"
#include "kernel/WordKernel.h"
#include "features/WordFeatures.h"
#include "lib/io.h"

#include <assert.h>
#include <math.h>

CLinearWordKernel::CLinearWordKernel(LONG size)
  : CWordKernel(size),scale(1.0)
{
}

CLinearWordKernel::~CLinearWordKernel() 
{
}
  
bool CLinearWordKernel::init(CFeatures* l, CFeatures* r, bool do_init)
{
	CWordKernel::init(l, r, do_init); 

	if (do_init)
		init_rescale() ;

	CIO::message(M_INFO, "rescaling kernel by %g (num:%d)\n",scale, math.min(l->get_num_vectors(), r->get_num_vectors()));

	return true;
}

void CLinearWordKernel::init_rescale()
{
	LONGREAL sum=0;
	scale=1.0;
	for (INT i=0; (i<lhs->get_num_vectors() && i<rhs->get_num_vectors()); i++)
			sum+=compute(i, i);

	if ( sum > (pow((double) 2, (double) 8*sizeof(LONG))) )
		CIO::message(M_ERROR, "the sum %lf does not fit into integer of %d bits expect bogus results.\n", sum, 8*sizeof(LONG));
	scale=sum/math.min(lhs->get_num_vectors(), rhs->get_num_vectors());
}

void CLinearWordKernel::cleanup()
{
}

bool CLinearWordKernel::load_init(FILE* src)
{
    assert(src!=NULL);
    UINT intlen=0;
    UINT endian=0;
    UINT fourcc=0;
    UINT r=0;
    UINT doublelen=0;
    double s=1;

    assert(fread(&intlen, sizeof(BYTE), 1, src)==1);
    assert(fread(&doublelen, sizeof(BYTE), 1, src)==1);
    assert(fread(&endian, (UINT) intlen, 1, src)== 1);
    assert(fread(&fourcc, (UINT) intlen, 1, src)==1);
    assert(fread(&r, (UINT) intlen, 1, src)==1);
    assert(fread(&s, (UINT) doublelen, 1, src)==1);
    CIO::message(M_INFO, "detected: intsize=%d, doublesize=%d, r=%d, scale=%g\n", intlen, doublelen, r, s);
	scale=s;
	return true;
}

bool CLinearWordKernel::save_init(FILE* dest)
{
	return false;
}
  
REAL CLinearWordKernel::compute(INT idx_a, INT idx_b)
{
  INT alen, blen;
  bool afree, bfree;

  WORD* avec=((CWordFeatures*) lhs)->get_feature_vector(idx_a, alen, afree);
  WORD* bvec=((CWordFeatures*) rhs)->get_feature_vector(idx_b, blen, bfree);

  assert(alen==blen);
  double sum=0;
  for (LONG i=0; i<alen; i++)
	  sum+=((LONG) avec[i])*((LONG) bvec[i]);

  REAL result=sum/scale;
  ((CWordFeatures*) lhs)->free_feature_vector(avec, idx_a, afree);
  ((CWordFeatures*) rhs)->free_feature_vector(bvec, idx_b, bfree);

  return result;
}

