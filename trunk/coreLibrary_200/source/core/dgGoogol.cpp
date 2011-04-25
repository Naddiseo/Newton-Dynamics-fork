/* Copyright (c) <2003-2011> <Julio Jerez, Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 
* 3. This notice may not be removed or altered from any source distribution.
*/


// based of the paper of: Jonathan Richard Shewchuk October 1, 1997
// "Adaptive Precision Floating-Point Arithmetic and Fast Robust Geometric Predicates"

// only using the exact arithmetic 
// This is an awesome paper of exact arithmetic 
// more than a million time better than my previous exact arithmetic based on integers
// this class is by far faster an much more complete than n my old Googol big number based in integers
// int newton 1.00


#include "dgStdafx.h"
#include "dgGoogol.h"


static dgFloat64 m_splitter = dgFloat64 (0.0f);
//static dgFloat64 m_precision = dgFloat64 (0.0f);

dgGoogol::dgGoogol(void)
	:dgArray (2, GetAllocator())
	,m_significantCount (0)
{
/*
#ifdef _DEBUG
	memset (m_elements, 0, sizeof (m_elements));
#endif
*/
}

dgGoogol::dgGoogol(dgFloat64 value)
	:dgArray (2, GetAllocator())
{
	InitFloatFloat (value);
}

dgGoogol::dgGoogol(const dgGoogol& copy)
	:dgArray (2, GetAllocator())
	,m_significantCount (copy.m_significantCount)
{
	if (m_significantCount) {
		(*this)[m_significantCount - 1] = 0.0;
		memcpy (&(*this)[0], &copy[0], m_significantCount * sizeof (dgFloat64));
	}
}

dgGoogol::~dgGoogol(void)
{
}


dgMemoryAllocator* dgGoogol::GetAllocator ()
{
	static dgMemoryAllocator allocator;
	return &allocator;
}


dgFloat64 dgGoogol::GetAproximateValue() const
{
	dgFloat64 val = 0.0f;

	const dgFloat64* const src = &(*this)[0];
	_ASSERTE (src);

	for (dgInt32 i = m_significantCount - 1; i >= 0; i --) {
		val += src[i];
	}
	return val;
}

void dgGoogol::InitFloatFloat (dgFloat64 value)
{
	if (m_splitter == 0.0) {

		dgInt32 every_other = 1;
		dgFloat64 check = dgFloat64 (1.0);
		dgFloat64 epsilon = dgFloat64 (1.0);
		dgFloat64 lastcheck = dgFloat64 (0.0);
		m_splitter = dgFloat64 (1.0);
		
		do {
			lastcheck = check;
			epsilon *= dgFloat64 (0.5);
			if (every_other) {
				m_splitter *= dgFloat64 (2.0);
			}
			every_other = !every_other;
			check = dgFloat64 (1.0) + epsilon;
		} while ((check != dgFloat64 (1.0)) && (check != lastcheck));
//		m_splitter *= 2.0;

		m_splitter += dgFloat64 (1.0);

//		m_precision = epsilon;
//		for (dgInt32 i = 1; i < DG_GOOGOL_SIZE; i ++) {
//			m_precision *= epsilon;
//		}
	}

	(*this)[0] = value;
	m_significantCount = 1;
	
}



inline void dgGoogol::AddFloat (dgFloat64 a, dgFloat64 b, dgFloat64& x, dgFloat64& y) const
{
	x = a + b; 
	dgFloat64 bvirt = x - a; 
	dgFloat64 avirt = x - bvirt; 
	dgFloat64 bround = b - bvirt; 
	dgFloat64 around = a - avirt; 
	y = around + bround;
}

void dgGoogol::PackFloat ()
{
	if (m_significantCount > 1) {
//		dgFloat64 elements[DG_GOOGOL_SIZE];
		
		dgFloat64 elements[1024];
		_ASSERTE (m_significantCount < sizeof (elements) / sizeof (elements[0]));
		dgInt32 bottom = m_significantCount - 1;

		dgFloat64* const dst = &(*this)[0];
		_ASSERTE (dst);

		dgFloat64 Q = dst[bottom];
		for (dgInt32 i = m_significantCount - 2; i >= 0; i--) {
			dgFloat64 q;
			dgFloat64 Qnew;
			dgFloat64 enow = dst[i];

			AddFloat (Q, enow, Qnew, q);
			if (q != 0) {
				elements[bottom--] = Qnew;
				Q = q;
			} else {
				Q = Qnew;
			}
		}

		dgInt32 top = 0;
		for (dgInt32 i = bottom + 1; i < m_significantCount; i ++) {
			dgFloat64 q;
			dgFloat64 Qnew;
			dgFloat64 hnow = elements[i];

			AddFloat (hnow, Q, Qnew, q);
			if (q != 0) {
				elements[top] = q;
				top ++;
				_ASSERTE (top < sizeof (elements) / sizeof (elements[0]));
			}
			Q = Qnew;
		}
		elements[top] = Q;
		m_significantCount = top + 1;
		_ASSERTE (m_significantCount < sizeof (elements) / sizeof (elements[0]));
		memcpy (dst, elements, m_significantCount * sizeof (dgFloat64));
	}

}


inline void dgGoogol::SplitFloat (dgFloat64 a, dgFloat64& ahi, dgFloat64& alo) const
{
	dgFloat64 c = m_splitter * a;
	dgFloat64 abig = c - a; 
	ahi = c - abig; 
	alo = a - ahi;
}

inline void dgGoogol::MulFloat (dgFloat64 a, dgFloat64 b, dgFloat64& x, dgFloat64& y) const
{
	dgFloat64 ahi;
	dgFloat64 alo;
	dgFloat64 bhi;
	dgFloat64 blo;

	x = a * b;

	SplitFloat (a, ahi, alo); 
	SplitFloat (b, bhi, blo); 

	dgFloat64 err1 = x - ahi * bhi; 
	dgFloat64 err2 = err1 - (alo * bhi); 
	dgFloat64 err3 = err2 - (ahi * blo); 
	y = alo * blo - err3;
}


dgGoogol dgGoogol::ScaleFloat(dgFloat64 scale) const
{
	dgFloat64 Q;
	dgGoogol tmp;
	tmp[m_significantCount + 2] = 0.0f;

	dgFloat64* const dst = &tmp[0];
	_ASSERTE (dst);

	const dgFloat64* const src = &(*this)[0];
	_ASSERTE (src);

	MulFloat (src[0], scale, Q, dst[0]);

	tmp.m_significantCount = 1;
	for (dgInt32 i = 1; i < m_significantCount; i++) {
		dgFloat64 sum;
		dgFloat64 product0;
		dgFloat64 product1;

		dgFloat64 enow = src[i];
		MulFloat (enow, scale, product1, product0);

		AddFloat (Q, product0, sum, dst[tmp.m_significantCount]);
		tmp.m_significantCount++;
		_ASSERTE (tmp.m_significantCount < (m_significantCount + 2));

		AddFloat (product1, sum, Q, dst[tmp.m_significantCount]);
		tmp.m_significantCount++;
		_ASSERTE (tmp.m_significantCount <= (m_significantCount + 2));
		
		tmp.PackFloat ();
	}

	dst[tmp.m_significantCount] = Q;
	tmp.m_significantCount++;
	_ASSERTE (tmp.m_significantCount <= (m_significantCount + 2));

	tmp.PackFloat ();
	_ASSERTE (tmp.m_significantCount <= (m_significantCount + 2));

	return tmp;
}

dgGoogol dgGoogol::operator= (const dgGoogol &copy)
{
	m_significantCount = copy.m_significantCount;
	if (m_significantCount) {
		(*this)[m_significantCount - 1] = 0.0;
		memcpy (&(*this)[0], &copy[0], m_significantCount * sizeof (dgFloat64));
	}
	return *this;
}

dgGoogol dgGoogol::operator+ (const dgGoogol &A) const
{
	dgGoogol tmp(*this);

	const dgFloat64* const src = &A[0];
	_ASSERTE (src);
	for (dgInt32 i = 0; i < A.m_significantCount; i++) {
		dgFloat64 q = src[i];
		dgFloat64* const dst = &tmp[0];
		_ASSERTE (dst);
		for (dgInt32 j = 0; j < tmp.m_significantCount; j++) {
			dgFloat64 Qnew;
			dgFloat64 hnow = dst[j];
			AddFloat (q, hnow, Qnew, dst[j]);
			q = Qnew;
		}
		tmp.PackFloat ();
		tmp[tmp.m_significantCount] = q;
		tmp.m_significantCount ++;
	}

	tmp.PackFloat ();
	return tmp;
}

dgGoogol dgGoogol::operator- ()
{
	dgFloat64* const dst = &(*this)[0];
	for (dgInt32 i = 0; i < m_significantCount; i ++) {
		dst[i] = - dst[i];
	}
	return *this;
}

dgGoogol dgGoogol::operator- (const dgGoogol &A) const
{
	dgGoogol tmp (A);
	dgFloat64* const dst = &tmp[0];
	const dgFloat64* const src = &A[0];
	_ASSERTE (dst);
	_ASSERTE (src);
	for (dgInt32 i = 0; i < tmp.m_significantCount; i ++) {
		dst[i] = - src[i];
	}
	return *this + tmp;
}

dgGoogol dgGoogol::operator* (const dgGoogol &A) const
{
	const dgFloat64* const src = &A[0];
	_ASSERTE (src);


	dgGoogol tmp (ScaleFloat(src[0]));	
	for (dgInt32 i = 1; i < A.m_significantCount; i ++) {
		tmp = tmp + ScaleFloat(src[i]);
	}
	return tmp;
}

dgGoogol dgGoogol::operator/ (const dgGoogol &A) const
{
//  I still do not know why this Newton Raphson iteration is not yielding the expected result 
//	dgGoogol tmp (1.0 / A.GetAproximateValue());
//	dgGoogol two (2.0);
//	for (dgInt32 i = 0; i < 3; i ++) {
//		tmp = tmp * (two - A * tmp);
//	}
//	return (*this) * tmp;

	// for now return the approximation
	return dgGoogol (GetAproximateValue() / A.GetAproximateValue());
}


dgGoogol dgGoogol::operator+= (const dgGoogol &A)
{
	*this = *this + A;
	return *this;
}

dgGoogol dgGoogol::operator-= (const dgGoogol &A)
{
	*this = *this - A;
	return *this;
}
