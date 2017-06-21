/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   poly/PolyVar.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#include <iostream>
#include <fstream>
#include <cmath>
#include <climits>
#include <complex>
#include <map>
#include <vector>
#include <algorithm>
#include <iterator>
#include <functional>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "mathSupport.h"
#include "MatrixBase.h"
#include "PolyFunction.h"
#include "PolyVar.h"

namespace mathLevel     
{

template<size_t VCount>
std::ostream& 
operator<<(std::ostream& OX,const PolyVar<VCount>& A)
  /*!
    External :: outputs point to a stream 
    \param OX :: output stream
    \param A :: PolyFunction to write
    \returns The output stream (OX)
  */
{
  if (!A.write(OX))
    OX<<0;
  return OX;
}

template<size_t VCount>
PolyVar<VCount>::PolyVar(const size_t iD) : 
  PolyFunction(),
  iDegree(iD),PCoeff(iDegree+1)
  /*!
    Constructor 
    \param iD :: degree
  */
{
  std::fill(PCoeff.begin(),PCoeff.end(),PolyVar<VCount-1>(0UL));
}

// template<size_t VCount>
// PolyVar<VCount>::PolyVar(const double V) : 
//   PolyFunction(),
//   iDegree(0),PCoeff(iDegree+1)
//   /*!
//     Constructor 
//     \param V :: value (constant)
//   */
// {
//   PCoeff.front()=PolyVar<VCount-1>(V);
// }


template<size_t VCount>
PolyVar<VCount>::PolyVar(const size_t iD,const double E) : 
  PolyFunction(E),iDegree(iD),
  PCoeff(iDegree+1)
  /*!
    Constructor 
    \param iD :: degree
    \param E :: Accuracy
  */
{
  std::fill(PCoeff.begin(),PCoeff.end(),PolyVar<VCount-1>
	    (static_cast<size_t>(0UL),E));
}

template<size_t VCount>
PolyVar<VCount>::PolyVar(const PolyVar<VCount>& A)  :
  PolyFunction(A),iDegree(A.iDegree),
  PCoeff(A.PCoeff)
  /*! 
    Copy Constructor
    \param A :: PolyVar to copy
   */
{}

template<size_t VCount>
template<size_t ICount>
PolyVar<VCount>::PolyVar(const PolyVar<ICount>& A) :
  iDegree(0),PCoeff(1)
  /*! 
    Copy constructor to a down reference
    \param A :: PolyVar to copy assign as PCoeff[0]
    \tparam ICount :: Polynominal of lower order than A
  */
{
  if (ICount>VCount)
    {
      std::cerr<<"ERROR WITH ICOUNT"<<std::endl;
      exit(1);
    }
  
  PCoeff[0]=A;
}

template<size_t VCount>
PolyVar<VCount>& 
PolyVar<VCount>::operator=(const PolyVar<VCount>& A)
  /*! 
    Assignment operator
    \param A :: PolyVar to copy
    \return *this
   */
{
  if (this!=&A)
    {
      PolyFunction::operator=(A);
      iDegree = A.iDegree;
      PCoeff=A.PCoeff;
    }
  return *this;
}

template<size_t VCount>
template<size_t ICount>
PolyVar<VCount>& 
PolyVar<VCount>::operator=(const PolyVar<ICount>& A)
  /*! 
    Assignment operator
    \param A :: PolyVar to copy
    \return *this
   */
{
  iDegree = 0;
  PCoeff.resize(1);
  PCoeff[0]=A;
  return *this;
}

template<size_t VCount>
PolyVar<VCount>& 
PolyVar<VCount>::operator=(const double& V)
  /*! 
    Assignment operator
    \param V :: Double value to set
    \return *this
   */
{
  iDegree = 0;
  PCoeff.resize(1);
  PCoeff[0]=V;
  return *this;
}

template<size_t VCount> 
PolyVar<VCount>::~PolyVar()
  /// Destructor
{}

template<size_t VCount>
void 
PolyVar<VCount>::setDegree(const size_t iD)
  /*!
    Set the degree value (assuming that we have
    a valid iDegree setup)
    \param iD :: degree
   */
{
  if (iD>iDegree)
    {
      for(size_t i=iDegree;i<iD;i++)
	PCoeff.push_back(PolyVar<VCount-1>
			 (static_cast<size_t>(0UL),this->Eaccuracy));
    }
  else   // reduction in power 
    PCoeff.resize(iD+1);

  iDegree=iD;
  return;
}

template<size_t VCount>
void 
PolyVar<VCount>::zeroPoly()
  /*!
    Zeros each polynominal coefficient
  */
{
  for(size_t i=0;i<=iDegree;i++)
    {
      PCoeff[i]=PolyVar<VCount-1>(0UL,this->Eaccuracy);
    }
  return;
}

template<size_t VCount>
size_t
PolyVar<VCount>::getDegree() const 
  /*!
    Accessor to degree size
    \return size 
  */
{
  return iDegree;
}

template<size_t VCount>
void
PolyVar<VCount>::setComp(const size_t Index,const double& V)
  /*!
    Set a component
    \param Index :: coefficient index [0->const]
    \param V :: Value
    \throw IndexError on Index range check
   */
{
  if (Index>iDegree)
    throw ColErr::IndexError<size_t>(Index,iDegree+1,"PolyVar::setComp(double)");
  PCoeff[Index] = V;
  return;  
}

template<size_t VCount>
template<size_t ICount>
void
PolyVar<VCount>::setComp(const size_t Index,const PolyVar<ICount>& FX)
  /*!
    Set a component. 
    - Condition VCount \> ICount
    \param Index :: Component index to set 
    \param FX :: Base component
   */
{
  if (Index>iDegree)
    throw ColErr::IndexError<size_t>(Index,iDegree+1,"PolyVar::setComp(PolyBase)");
  PCoeff[Index] = FX;
  return;
}

template<size_t VCount>
double 
PolyVar<VCount>::operator()(const double* DArray) const
  /*!
    Calculate the value of the polynomial at a point
    \param DArray :: Values [x,y,z]
    \return value of polynomial
  */
{
  double X(1.0);
  double sum(0.0);
  for(size_t i=0;i<=iDegree;i++)
    {
      sum+=PCoeff[i](DArray)*X;
      X*=DArray[VCount-1];
    }
  return sum;
}

template<size_t VCount>
double 
PolyVar<VCount>::operator()(const std::vector<double>& DArray) const
  /*!
    Calculate the value of the polynomial at a point
    \param DArray :: Values [x,y,z]
    \return result at the point DArray
  */
{
  if (DArray.size()<VCount)
    throw ColErr::IndexError<size_t>(VCount,DArray.size(),
				       "PolVar::operator()");
  double X(1.0);
  double sum(0.0);
  for(size_t i=0;i<=iDegree;i++)
    {
      sum+=PCoeff[i](DArray)*X;
      X*=DArray[VCount-1];
    }
  return sum;
}

template<size_t VCount>
double 
PolyVar<VCount>::operator()(const double x,
			    const double y) const
  /*!
    Calculate the value of the polynomial at a point
    \param x :: X coordinate [lower]
    \param y :: Y coordinate [higher]
    \return value of polynomial
  */
{
  std::vector<double> DArray(2);
  DArray.push_back(x);
  DArray.push_back(y);
  return this->operator()(DArray);
}

template<size_t VCount>
double 
PolyVar<VCount>::operator()(const double x,const double y,
			    const double z) const
  /*!
    Calculate the value of the polynomial at a point
    \param x :: X coordinate [lower]
    \param y :: Y coordinate [mid]
    \param z :: Z coordinate [higher]
    \return value of polynomial
  */
{
  std::vector<double> DArray;
  DArray.push_back(x);
  DArray.push_back(y);
  DArray.push_back(z);
  return operator()(DArray);
}

template<size_t VCount>
bool
PolyVar<VCount>::operator<(const PolyVar<VCount>& A) const
  /*!
    PolyVar comparison based on number of coefficients 
    then degree (simpler)
    \param A :: PolyVar to compare
    \return this < A
   */
{
  const size_t flagT=this->getVarFlag();
  const size_t flagA=A.getVarFlag();
  const size_t nT=countBits(flagT);
  const size_t nA=countBits(flagA);
  return (nT!=nA) ? (nT<nA) : (iDegree<A.iDegree);
}

template<size_t VCount>
PolyVar<VCount>&
PolyVar<VCount>::operator+=(const PolyVar<VCount>& A)
  /*!
    PolyVar addition
    \param A :: PolyVar 
    \return (*this+A);
   */
{
  const size_t iMax((iDegree>A.iDegree)  ? iDegree : A.iDegree);
  PCoeff.resize(iMax+1);

  for(size_t i=0;i<=A.iDegree;i++)
    PCoeff[i]+=A.PCoeff[i];
  iDegree=iMax;
  compress();
  return *this;
}

template<size_t VCount>
PolyVar<VCount>&
PolyVar<VCount>::operator-=(const PolyVar<VCount>& A)
  /*!
    PolyVar subtraction
    \param A :: PolyVar 
    \return (*this-A);
  */
{
  const size_t iMax((iDegree>A.iDegree)  ? iDegree : A.iDegree);
  PCoeff.resize(iMax+1);

  for(size_t i=0;i<=A.iDegree;i++)
    PCoeff[i]-=A.PCoeff[i];
  iDegree=iMax;
  compress();
  return *this;
}

template<size_t VCount>
PolyVar<VCount>&
PolyVar<VCount>::operator*=(const PolyVar<VCount>& A)
  /*!
    Multiply two Polynomials
    \param A :: PolyVar 
    \return (*this*A);
   */
{
  std::vector<PolyVar<VCount-1> > POut(iDegree+A.iDegree+2); 
  std::vector<int> Zero(A.iDegree+1);
  transform(A.PCoeff.begin(),A.PCoeff.end(),Zero.begin(),
    std::bind2nd(std::mem_fun_ref(&PolyVar<VCount-1>::isZero),this->Eaccuracy));
   
  for(size_t i=0;i<=iDegree;i++)
    {
      if (!PCoeff[i].isZero(this->Eaccuracy)) 
        {
	  for(size_t j=0;j<=A.iDegree;j++)
	    if (!Zero[j])
	      {
	        POut[i+j]+= PCoeff[i]*A.PCoeff[j];
	      }
	}
    }
  
  PCoeff=POut;
  iDegree=iDegree+A.iDegree+1;
  compress();  
  return *this;
}

template<size_t VCount>
PolyVar<VCount> 
PolyVar<VCount>::operator+(const PolyVar<VCount>& A) const
  /*!
    PolyVar addition
    \param A :: PolyVar addition
    \return (*this+A);
   */
{
  PolyVar<VCount> kSum(*this);
  return kSum+=A;
}

template<size_t VCount>
PolyVar<VCount> 
PolyVar<VCount>::operator-(const PolyVar<VCount>& A) const
  /*!
    PolyVar subtraction
    \param A :: PolyVar 
    \return (*this-A);
   */
{
  PolyVar<VCount> kSum(*this);
  return kSum-=A;
}

template<size_t VCount>
PolyVar<VCount> 
PolyVar<VCount>::operator*(const PolyVar<VCount>& A) const
  /*!
    PolyVar multiplication
    \param A :: PolyVar 
    \return (*this*A);
   */
{
  PolyVar<VCount> kSum(*this);
  return kSum*=A;
}


// SCALAR BASICS

template<size_t VCount>
PolyVar<VCount> 
PolyVar<VCount>::operator+(const double V) const
  /*!
    PolyVar addition
    \param V :: Value to add (constant)
    \return (*this+A);
   */
{
  PolyVar<VCount> kSum(*this);
  return kSum+=V;
}

template<size_t VCount>
PolyVar<VCount> 
PolyVar<VCount>::operator-(const double V) const
  /*!
    PolyVar substraction
    \param V :: Value to subtract
    \return (*this-A);
   */
{
  PolyVar<VCount> kSum(*this);
  return kSum-=V;
}

template<size_t VCount>
PolyVar<VCount>
PolyVar<VCount>::operator*(const double V) const
  /*!
    PolyVar multiplication
    \param V :: Value to Multiply
    \return (*this*V);
   */
{
  PolyVar<VCount> kSum(*this);
  return kSum*=V;
}

template<size_t VCount>
PolyVar<VCount>
PolyVar<VCount>::operator/(const double V) const
  /*!
    PolyVar division
    \param V :: Value to divide by
    \return (*this/A);
   */
{
  PolyVar<VCount> kSum(*this);
  return kSum/=V;
}

//
// ---------------- SCALARS --------------------------
//

template<size_t VCount>
PolyVar<VCount>&
PolyVar<VCount>::operator+=(const double V) 
  /*!
    PolyVar addition
    \param V :: Value to add
    \return (*this+V);
   */
{
  PCoeff[0]+=V;
  return *this;
}

template<size_t VCount>
PolyVar<VCount>&
PolyVar<VCount>::operator-=(const double V) 
  /*!
    PolyVar subtraction
    \param V :: Value to subtract
    \return (*this+V);
   */
{
  PCoeff[0]-=V;  // There is always zero component
  return *this;
}

template<size_t VCount>
PolyVar<VCount>&
PolyVar<VCount>::operator*=(const double V)  
  /*!
    PolyVar multiplication
    \param V :: Value to multipication
    \return (*this*V);
   */
{
  typename std::vector< PolyVar<VCount-1> >::iterator vc;
  for(vc=PCoeff.begin();vc!=PCoeff.end();vc++)
    (*vc)*=V;
  compress();
  return *this;
}

template<size_t VCount>
PolyVar<VCount>&
PolyVar<VCount>::operator/=(const double V) 
  /*!
    PolyVar division scalar
    \param V :: Value to divide
    \return (*this/V);
   */
{
  typename std::vector< PolyVar<VCount-1> >::iterator vc;
  for(vc=PCoeff.begin();vc!=PCoeff.end();vc++)
    (*vc)/=V;
  return *this;
}

template<size_t VCount>
PolyVar<VCount> 
PolyVar<VCount>::operator-() const
  /*!
    PolyVar negation operator
    \return -(*this);
   */
{
  PolyVar<VCount> KOut(*this);
  KOut *= -1.0;
  return KOut;
}

template<size_t VCount>
int
PolyVar<VCount>::operator==(const PolyVar<VCount>& A) const
  /*!
    Determine if two polynomials are equal
    \param A :: Other polynomial to use
    \return 1 :: true 0 on false
  */
{
  size_t i;   // I used to determine the excess zeros [if present]
  for(i=0;i<=A.iDegree && i<=iDegree;i++)
    if (PCoeff[i]!=A.PCoeff[i])
      return 0;

  if (A.iDegree>iDegree)
    {
      for(;i<=A.iDegree;i++)
	if (!A.PCoeff[i].isZero(this->Eaccuracy))
	  return 0;
    }
  else if (A.iDegree<iDegree)
    {
      for(;i<=iDegree;i++)
	if (!PCoeff[i].isZero(this->Eaccuracy))
	  return 0;
    }
  return 1;
}

template<size_t VCount>
int
PolyVar<VCount>::operator!=(const PolyVar<VCount>& A) const
  /*!
    Determine if two polynomials are different
    \param A :: Other polynomial to use
    \return 1 is polynomial differ:  0 on false
  */
{
  return (A==*this) ? 0 : 1;
}
 
template<size_t VCount>
PolyVar<VCount> 
PolyVar<VCount>::getDerivative() const
  /*!
    Take derivative
    \return dP / dx
  */
{
  PolyVar<VCount> KOut(*this);
  return KOut.derivative();
}

template<size_t VCount>
PolyVar<VCount>&
PolyVar<VCount>::derivative()
  /*!
    Take derivative of this polynomial
    \return dP / dx
  */
{
  if (iDegree<1)
    {
      PCoeff[0] *= 0.0;
      return *this;
    }

  for(size_t i=0;i<iDegree;i++)
    PCoeff[i]=PCoeff[i+1] * static_cast<double>(i+1);
  iDegree--;
  compress();
  return *this;
}

template<size_t VCount>
PolyVar<VCount>
PolyVar<VCount>::GetInversion() const
  /*!
    Inversion of the coefficients
    Note that this is useful for power balancing.
    \return Reversed poly
   */
{
  PolyVar<VCount> InvPoly(iDegree);
  for (size_t i=0; i<=iDegree; i++)
    InvPoly.PCoeff[i] = PCoeff[iDegree-i];  
  return InvPoly;
}

template<size_t VCount>
void 
PolyVar<VCount>::compress(const double epsilon)
  /*!
    Two part process remove coefficients of zero or near zero: 
    Reduce degree by eliminating all (nearly) zero leading coefficients
    and by making the leading coefficient one.  
    \param epsilon :: coeficient to use to decide if a values is zero
   */
{
  ELog::RegMethod RegA("PolyVar","Compress",VCount);
  
  const double eps((epsilon>0.0) ? epsilon : this->Eaccuracy);
  for (;iDegree>0 && PCoeff[iDegree].isZero(eps);iDegree--) ;
  PCoeff.resize(iDegree+1);

  for(PolyVar<VCount-1>& PC : PCoeff)
    PC.compress(epsilon);

  return;
}

template<size_t VCount>
size_t
PolyVar<VCount>::getCount(const double eps) const
  /*!
    Determines the full number of non-zero parameters
    \param eps :: Value to used
    \return Number of non-zero components
  */
{
  size_t cnt(0);
  for(size_t i=0;i<=iDegree;i++)
    if(!PCoeff[i].isZero(eps))
      cnt++;
  return cnt;
}


template<size_t VCount>
size_t
PolyVar<VCount>::getVarFlag() const
  /*!
    Determines the variables in the equation
    and returns a flag of bits of each variable
    \return Flag of variable type
  */
{
  size_t flag=(iDegree>0) ? 1<<(VCount-1) : 0;
  for(size_t i=0;i<=iDegree;i++)
    flag |= PCoeff[i].getVarFlag();
  return flag;
}

template<size_t VCount>
size_t
PolyVar<VCount>::getVarCount() const
  /*!
    Determines the number variables in the equation
    \return Count
  */
{
  size_t flag=getVarFlag();
  return countBits(flag);  
}

template<size_t VCount>
int 
PolyVar<VCount>::isComplete() const
  /*!
    Determines if the variables in the system are
    all present.
    \return 1 on success / 0 on failure
  */
{
  const int flag= (1<<VCount)-1;
  return (flag==getVarFlag()) ? 1 : 0;
}

template<size_t VCount>
int 
PolyVar<VCount>::isZero(const double eps) const
  /*!
    Determine if is zero
    \param eps :: Value to used
    \retval 1 :: all components  -eps<Value<eps
    \retval 0 :: one or more non zero po
  */
{
  size_t i;
  for(i=0;i<=iDegree && PCoeff[i].isZero(eps);i++) ;
  return (i<=iDegree) ? 0 : 1;
}

template<size_t VCount>
int 
PolyVar<VCount>::isUnit(const double eps) const
  /*!
    Determine if is a unit base value.
    Note: this needs to be suttle, since
    x is unit, as is y, as is xy
    \param eps :: Value to used
    \retval 1 :: +ve unit
    \retval 0 :: Not unit
    \retval -1 :: -ve unit 
  */
{
  size_t i;
  for(i=iDegree;i>0 && PCoeff[i].isZero(eps);i--) ;
  if (i)
    return 0;
  
  return PCoeff[0].isUnit(eps);
}

template<size_t VCount>
int 
PolyVar<VCount>::isUnitary(const double eps) const
  /*!
    Determine if is a unit base value.
    Note: this needs to be suttle, since
    x is unit, as is y, as is xy
    \param eps :: Value to used
    \retval 1 :: +ve unit
    \retval 0 :: Not unit
    \retval -1 :: -ve unit 
  */
{
  size_t item(0);
  int flag(0);
  for(size_t i=iDegree+1;i>0 && flag<2;)
    {
      i--;
      if (!PCoeff[i].isZero(eps))
        {
	  item=i;
	  flag++;
	}
    }
  if (flag==2 || flag==0) // all zeros are also NOT unit
    return 0;
  return ((item==0) ? 1 : 2) * PCoeff[item].isUnit(eps);
}

template<size_t VCount>
std::pair< PolyVar<VCount-1>,unsigned int >
PolyVar<VCount>::eliminateVar(const PolyVar<VCount>& A) const
  /*!
    The objective is to use A and this to reduce the
    variable count by 1. However, it can use the optimal 
    part of solution based on the variable count in A and this.
    Using the reduced matrix form from e.g. 
    http://en.wikipedia.org/wiki/Bezout_matrix
    \param A :: PolyVar to use
    \return new polynomial, variable flag
  */
{
  std::pair< PolyVar<VCount-1>,unsigned int > Out;
  const size_t Tflag=this->getVarFlag();
  const size_t Aflag=A.getVarFlag();
  
  if (countBits(Tflag)==1)
    {
      PolyVar<1> Solo=this->singleVar();
    }
  else if (countBits(Aflag)==1)
    {
      PolyVar<1> Solo=A.singleVar();
    }
  return Out;
}

template<size_t VCount>
PolyVar<VCount-1>
PolyVar<VCount>::reduce(const PolyVar<VCount>& A) const
  /*!
    The objective is to use A and this to reduce the
    variable count by 1.
    Using the reduced matrix form from e.g. 
    http://en.wikipedia.org/wiki/Bezout_matrix
    \param A :: PolyVar to use
    \return new polynomial
   */
{
  ELog::RegMethod RegA("PolyVar","reduce",VCount);
  const size_t ANum(iDegree+1);
  const size_t BNum(A.iDegree+1);
  const size_t MSize((ANum>BNum) ? ANum : BNum);      
  
  // SPECIAL CASE : primary variable is zero in both equations:
  if (MSize==1)
    {
      PolyVar<VCount-1> Out=PCoeff[0];
      Out+=A.PCoeff[0];
      return Out;
    }
  
  Geometry::MatrixBase<PolyVar<VCount-1> > MX(MSize-1,MSize-1);

  for(size_t i=1;i<MSize;i++)
    for(size_t j=1;j<MSize;j++)
      {
	const size_t mij=std::min(i,1+MSize-j);
	for(size_t k=1;k<=mij;k++)
	  {
	    // u_{j+k-1} v_{i-k} - u_{i-k} v_{j+k-1}
	    const size_t jkTerm(j+k-1);
	    const size_t ikTerm(i-k);
	    if (jkTerm<ANum && ikTerm<BNum)
	      MX[i-1][j-1] += PCoeff[jkTerm]*A.PCoeff[ikTerm];
	    if (jkTerm<BNum && ikTerm<ANum)
	      MX[i-1][j-1] -= PCoeff[ikTerm]*A.PCoeff[jkTerm];
	    
	  }
      }
  PolyVar<VCount-1> Out = MX.laplaceDeterminate();
  Out.compress();
  return Out;
}

template<size_t VCount>
int
PolyVar<VCount>::expand(const PolyVar<VCount>& A)
  /*!
    This takes the extra equation and create an equation that 
    has maximium number of dependent variables
    \param A :: PolyVar to use
    \return flag status -ve for error
  */
{
  ELog::RegMethod RegA("PolyVar","expand",VCount);
  // special case : function contains all the variables
  if (this->isComplete())
    return 0;
  // Special case : The other function is complete
  if (A.isComplete())
    {
      this->operator=(A);
      return 0;
    }

  const size_t aim((2UL<<(VCount-1))-1);
  PolyVar<VCount> Test(*this);
  PolyVar<VCount> Best(*this);
  // This loops over 3 items to ensure we don't have
  // a term cancellation
  size_t bestFlag=Test.getVarFlag();
  for(size_t i=0;i<3;i++)
    {
      Test+=A;
      const size_t newFlag=Test.getVarFlag();
      if (aim==newFlag)
        {
	  this->operator=(Test);
	  return 0;
	}
      if (bestFlag<newFlag)
        {
	  Best=Test;
	  bestFlag=newFlag;
	}
    }
  this->operator=(Best);
  return -1;
}

template<size_t VCount>
PolyVar<VCount-1>
PolyVar<VCount>::substitute(const double& V) const
  /*!
    The value of x is set as V, then move all the 
    zy etc values to xy values.
    \param V :: Value for X
    \return new polynomial
  */
{
  PolyVar<VCount-1> Out(iDegree,this->Eaccuracy);
  for(size_t index=0;index<=iDegree;index++)
    {
      Out.setComp(index,PCoeff[index].substitute(V));
    }
  return Out;
}

template<size_t VCount>
template<size_t ICount>
PolyVar<ICount>
PolyVar<VCount>::substitute(const double& VA,const double& VB) const
  /*!
    The value of x is set as V, then move all the 
    zy etc values to xy values.
    \param VA :: Value for X
    \param VB :: Value for Y
    \tparam ICount :: return degree
    \return new polynomial
   */
{
  PolyVar<ICount> Out(iDegree,this->Eaccuracy);
  for(size_t index=0;index<=iDegree;index++)
    {
      Out.setComp(index,PCoeff[index].substitute(VA,VB));
    }
  return Out;
}

template<size_t VCount>
int
PolyVar<VCount>::read(const std::string& Line)
  /*!
    Given a line of type 
    y^2+xy+3.0x 
    convert into a function:
    Variables in list are x,y,z,a,b,c,....
    \param Line to read
    \return 0 :: success / -ve on error
  */
{
  const char Variable("xyzabc"[VCount-1]);
  std::string CLine=StrFunc::removeSpace(Line);

  setDegree(PolyFunction::getMaxSize(CLine,Variable)); 
  zeroPoly();
  int sign(0);
  std::string::size_type pos=CLine.find(Variable);
  while (pos!=std::string::npos)
    {
      size_t compStart(pos);
      sign=0;     // default is positive but not found
      // get preFunction 
      int bracket(0);  // Depth of brackets
      size_t bCut(ULONG_MAX);    // Position of last ( 
      while(compStart>0)
        {
	  compStart--;
	  // Process brackets
	  if (CLine[compStart]=='(' || CLine[compStart]==')')
	    {
	      bracket+= CLine[compStart]=='(' ? 1 : -1;
	      if (!bracket)  // hit end of bracket:
		bCut=compStart;
	    }
	  else if (bracket==0)
	    {
	      if (CLine[compStart]=='+' || CLine[compStart]=='-')
	        {
		  sign=(CLine[compStart]=='+') ? 1 : -1;
		  break;
		}
	    } 
	}
      if (bracket)
	throw ColErr::InvalidLine("PolVar::read",Line,0);

      std::string Comp;      
      if (bCut!=ULONG_MAX)
        {
	  // Have bracket
	  Comp=CLine.substr(bCut+1,pos-bCut-2);  
	  if (sign)
	    {
	      CLine.erase(bCut-1,pos-bCut+2);
	    }
	  else
	    {
	      CLine.erase(bCut,pos-bCut+1);
	    }
	}
      else if (sign)
        {
	  Comp=CLine.substr(compStart+1,pos-compStart-1);
	  CLine.erase(compStart,pos-compStart+1);
	}
      else
        {
	  Comp=CLine.substr(compStart,pos-compStart);
	  CLine.erase(compStart,pos-compStart+1);
	  sign=1;
	}
      // Need -ve on bracket: ??
      // Find power
      size_t pV(1);
      if (!CLine.empty() && CLine[0]=='^')
        {
	  CLine.erase(0,1);
	  if (!StrFunc::sectPartNum(CLine,pV) || pV>iDegree)
	    return -1;
	}
      if (Comp.empty())
	PCoeff[pV]=sign;
      else 
        {
	  if (PCoeff[pV].read(Comp))
	    return -2;
	  if (sign<0) PCoeff[pV]*=-1.0;
	}
      // Find next value
      pos=CLine.find(Variable);
    }

  if (!CLine.empty())
    PCoeff[0].read(CLine);

  return 0;
}


template<size_t VCount>
double
PolyVar<VCount>::substitute(const double&,const double&,const double&) const
  /*!
    \throw Index error as incorrect call
    \return NONE
  */
{
  throw ColErr::IndexError<int>(3,VCount,"PolyVar::subsitute(a,b,c)");
}


template<size_t VCount>
int
PolyVar<VCount>::write(std::ostream& OX,const int prePlus) const
  /*!
    Basic write command
    \param OX :: output stream
    \param prePlus :: A + sign required infront of a +ve number
    \retval 0 :: nothing written 
    \retval 1 :: normal equation
    \retval -1 :: unitary equation ("1") 
  */
{
  const char Variable("xyzabc"[VCount-1]);
  int nowrite(0);
  for(size_t i=iDegree+1;i>0;)   
    {
      i--;
      const size_t cnt=PCoeff[i].getCount(this->Eaccuracy);
      if (cnt>1)
        {
	  if (i)
	    {
	      if (prePlus || nowrite ) 
		OX<<((prePlus>=0) ? "+" : "-");
	      OX<<"("<<PCoeff[i]<<")";
	    }
	  else
	    PCoeff[0].write(OX,(prePlus || nowrite));

	  nowrite=1;
	}
      else if (cnt==1)
        {
	  const int oneFlag=PCoeff[i].isUnit(this->Eaccuracy);
	  if (oneFlag>0 && (nowrite || prePlus)) 
	    OX<<"+";
	  else if (oneFlag<0) 
	    OX<<"-";
	  
	  if (!oneFlag)
	    PCoeff[i].write(OX,nowrite|prePlus);
	  else if (!i)
	    OX<<"1";

	  nowrite=1;
	}
      if (i && cnt)
        {
	  OX<<Variable;
	  if (i!=1)
	    OX<<"^"<<i;
	}
    } 
  return nowrite;
}  

template<>
double
PolyVar<2>::substitute(const double& VA,const double& VB) const
  /*!
    Special case to deal with <2> reducing to a value
    \param VA :: x variable
    \param VB :: y variable
    \return Polynominal value
  */
{
  const double A[]={VA,VB};
  return operator()(A);
}

template<>
double
PolyVar<3>::substitute(const double& VA,const double& VB,const double& VC) const
  /*!
    Special case to deal with <2> reducing to a value
    \param VA :: x variable
    \param VB :: y variable
    \param VC :: z variable
    \return polynominal value
  */
{
  double A[]={VA,VB,VC};
  return operator()(A);
}

template<>
PolyVar<3>&
PolyVar<3>::makeTriplet(const std::vector<double>& Pre)
  /*!
    Pre sets a polynominal of the type
    Ax^2+By^2+Cz^2+Dxy+Exz+Fyz+Gx+Hy+Jz+K=0
    \param Pre :: A vector of size 10.
    \return Polnomial (this)
  */
{
  enum {A,B,C,D,E,F,G,H,J,K};
  if (Pre.size()!=10)
    throw ColErr::IndexError<size_t>(Pre.size(),10,
				       "PolyVar<3>::makeTriplet");
  
  // Set XYZ:
  setDegree(3);
  PCoeff[2]=PolyVar<2>();
  PCoeff[2].setComp(0,Pre[C]);                // Cz^2
  // 
  PolyVar<2> YZ(2UL);                            // 
  YZ.setComp(1,Pre[F]);                        // Fy
  YZ.setComp(0,PolyVar<1>(2,Pre[E],Pre[J]));     // Ex+J
  PCoeff[1]=YZ;

  YZ.setDegree(3);      
  YZ.setComp(2,Pre[B]);                        // By^2
  YZ.setComp(1,PolyVar<1>(2,Pre[D],Pre[H]));     // (Dx+H)y
  YZ.setComp(0,PolyVar<1>(3,Pre[A],Pre[G],Pre[K]));  // (Az^2+Gz+K)
  PCoeff[0]=YZ;
  compress();
  return *this;
}

template<size_t VCount>
PolyVar<VCount-1>
PolyVar<VCount>::subVariable(const size_t Index,const double& V) const
  /*!
    Reduce the number of variables by 1.
    \param Index :: Index of variable to reduce
    \param V :: Value of the variable
    \return new polynomial
   */ 
{
  ELog::RegMethod RegA("PolyVar","subVariable",VCount);
  if (Index>=VCount)
    throw ColErr::IndexError<size_t>(Index,VCount,RegA.getBase());
  
  PolyVar<VCount-1> Out;

  // Special case:
  if (Index==VCount-1)
    {
      double VCoeff(1.0);
      for(size_t i=0;i<=iDegree;i++)
	{
	  Out+=PCoeff[i]*VCoeff;
	  VCoeff*=V;
	}
      Out.compress();
      return Out;
    }
  Out.setDegree(iDegree);
  for(size_t i=0;i<=iDegree;i++)
    Out.setComp(i,PCoeff[i].subVariable(Index,V));

  Out.compress();
  return Out;
}

template<size_t VCount>
PolyVar<1>
PolyVar<VCount>::singleVar() const
  /*!
    If the system is a single variable return
    the single system
    \return new polynomial
   */
{
  ELog::RegMethod RegA("PolyVar","singleVar",VCount);

  const size_t flag=getVarFlag();
  if (countBits(flag)!=1)
    {
      ELog::EM<<"Value == "<<*this<<ELog::endCrit;
      throw ColErr::IndexError<size_t>(flag,0,RegA.getBase());
    }
  // If it is final then just convert
  if (flag & (2 << (VCount-2)))
    {
      PolyVar<1> Out(iDegree);
      for(size_t i=0;i<=iDegree;i++)
	Out.setComp(i,PCoeff[i].getDouble());
      return Out;
    }
  // If intermediate / then must have only one PCoeff
  // and a partial of that coeff
  return PCoeff[0].singleVar();
}

/// \cond TEMPLATE

template class PolyVar<2>;   // f(x,y)
template class PolyVar<3>;   // f(x,y,z)

template std::ostream& operator<<(std::ostream&,const PolyVar<1>&);
template std::ostream& operator<<(std::ostream&,const PolyVar<2>&);
template std::ostream& operator<<(std::ostream&,const PolyVar<3>&);

template void PolyVar<3>::setComp(const size_t,const PolyVar<2>&);
template void PolyVar<2>::setComp(const size_t,const PolyVar<1>&);

template PolyVar<1> PolyVar<3>::substitute(const double&,const double&) const;

template PolyVar<2>& PolyVar<2>::operator=(const PolyVar<1>&);
template PolyVar<3>& PolyVar<3>::operator=(const PolyVar<1>&);
template PolyVar<3>& PolyVar<3>::operator=(const PolyVar<2>&);

template PolyVar<2>::PolyVar(const PolyVar<1>&);
template PolyVar<3>::PolyVar(const PolyVar<1>&);
template PolyVar<3>::PolyVar(const PolyVar<2>&);


/// \endcond TEMPLATE

}  // NAMESPACE  mathLevel

