#include "lib/common.h"
#include "kernel/LinearByteKernel.h"
#include "kernel/ByteKernel.h"
#include "features/ByteFeatures.h"
#include "lib/io.h"

#include <assert.h>
#include <math.h>

CLinearByteKernel::CLinearByteKernel(LONG size)
  : CByteKernel(size),scale(1.0)
{
}

CLinearByteKernel::~CLinearByteKernel() 
{
}
  
bool CLinearByteKernel::init(CFeatures* l, CFeatures* r, bool do_init)
{
	CByteKernel::init(l, r, do_init); 

	if (do_init)
		init_rescale() ;

	CIO::message(M_INFO, "rescaling kernel by %g (num:%d)\n",scale, math.min(l->get_num_vectors(), r->get_num_vectors()));

	return true;
}

void CLinearByteKernel::init_rescale()
{
	LONGREAL sum=0;
	scale=1.0;
	for (INT i=0; (i<lhs->get_num_vectors() && i<rhs->get_num_vectors()); i++)
			sum+=compute(i, i);

	if ( sum > (pow((double) 2, (double) 8*sizeof(LONG))) )
		CIO::message(M_ERROR, "the sum %lf does not fit into integer of %d bits expect bogus results.\n", sum, 8*sizeof(LONG));
	scale=sum/math.min(lhs->get_num_vectors(), rhs->get_num_vectors());
}

void CLinearByteKernel::cleanup()
{
}

bool CLinearByteKernel::load_init(FILE* src)
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

bool CLinearByteKernel::save_init(FILE* dest)
{
    BYTE intlen=sizeof(UINT);
    BYTE doublelen=sizeof(double);
    UINT endian=0x12345678;
    BYTE fourcc[5]="LINK"; //id for linear kernel

    assert(fwrite(&intlen, sizeof(BYTE), 1, dest)==1);
    assert(fwrite(&doublelen, sizeof(BYTE), 1, dest)==1);
    assert(fwrite(&endian, sizeof(UINT), 1, dest)==1);
    assert(fwrite(&fourcc, sizeof(char), 4, dest)==1);
    assert(fwrite(&scale, sizeof(double), 1, dest)==1);
    CIO::message(M_INFO, "wrote: intsize=%d, doublesize=%d, scale=%g\n", intlen, doublelen, scale);

	return true;
}
  
REAL CLinearByteKernel::compute(INT idx_a, INT idx_b)
{
  INT alen, blen;
  bool afree, bfree;

  BYTE* avec=((CByteFeatures*) lhs)->get_feature_vector(idx_a, alen, afree);
  BYTE* bvec=((CByteFeatures*) rhs)->get_feature_vector(idx_b, blen, bfree);
  assert(alen==blen);

  double sum=0;
  for (INT i=0; i<alen; i++)
	  sum+=((LONG) avec[i])*((LONG) bvec[i]);
  REAL result=sum/scale;

  ((CByteFeatures*) lhs)->free_feature_vector(avec, idx_a, afree);
  ((CByteFeatures*) rhs)->free_feature_vector(bvec, idx_b, bfree);

  return result;
}
