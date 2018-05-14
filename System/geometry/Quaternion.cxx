/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geometry/Quaternion.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cmath>
#include <complex>
#include <string>
#include <vector>
#include <map>

#include "Exception.h"
#include "mathSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"

/// Use boost float comparison 	
// boost::test_tools::close_at_tolerance<double> 
//qTol(boost::test_tools::percent_tolerance(1e-6));


std::ostream& 
Geometry::operator<<(std::ostream& OX,const Geometry::Quaternion& A)
  /*!
    Write out a quaternion
    \param A :: Quaternion to write
    \param OX :: output stream
    \returns The output stream (OX)
  */
{
  A.write(OX);
  return OX;
}

std::istream& 
Geometry::operator>>(std::istream& IX,Geometry::Quaternion& A)
  /*!
    Calls Vec3D method write to output class
    \param IX :: Input stream
    \param A :: Vec3D to write
    \return Current state of stream
  */
{
  A.read(IX);
  return IX;
}

namespace Geometry
{

double
Quaternion::calcQ0(const double Angle) 
  /*!
    Calculate q0 value for a given angle (radians)
    \param Angle :: angle to calculate Q0 from
    \return q0
  */
{
  return cos(Angle/2.0);
}

double
Quaternion::calcQ0deg(const double Angle)
  /*!
    Calculate q0 value for a given angle 
    \param Angle :: angle to calculate Q0 from [deg]
    \return q0
  */
{
  return cos(M_PI*Angle/360.0);
}

Quaternion
Quaternion::calcQRot(const double Angle,const double a,
		     const double b,const double c)
  /*!
    Calculate Quaternion value for a given angle 
    \param Angle :: angle to calculate Q0 from [radians]
    \param a :: x coord of rotation axis
    \param b :: y coord of rotation axis
    \param c :: z coord of rotation axis
    \return Quaternion
  */
{
  Vec3D A(a,b,c);
  A.makeUnit();
  A*=sin(Angle/2.0);
  return Quaternion(cos(Angle/2.0),A);
}

Quaternion
Quaternion::calcQRotDeg(const double Angle,const double a,const double b,const double c)
  /*!
    Calculate Quaternion value for a given angle 
    \param Angle :: angle to calculate Q0 from [deg]
    \param a :: x coord of rotation axis
    \param b :: y coord of rotation axis
    \param c :: z coord of rotation axis
    \return Q
  */
{
  Vec3D A(a,b,c);
  A.makeUnit();
  A*=sin(M_PI*Angle/360.0);
  return Quaternion(cos(M_PI*Angle/360.0),A);
}

Quaternion
Quaternion::calcQVRot(const Geometry::Vec3D& A,
                      const Geometry::Vec3D& APrime,
		      const Geometry::Vec3D& ZRotDef)
  /*!
    Calculate Quaternion value for a given 
    a vector and the new target vector rotated about the
    origin
    \param A :: Original vector
    \param APrime :: Vector that is rotated to A
    \param ZRotDef :: Z-default rotation if A and APrime are colinear
    \return Quaternion that rotates APrime to A
  */
{
  Geometry::Vec3D Axis=A*APrime;
  Axis.makeUnit();
  double Angle=A.dotProd(APrime)/(A.abs()*APrime.abs());

  // Note the test for the 1.0+epsilon error:
  if (std::abs(Angle)<1.0-Geometry::zeroTol)
    Angle=(acos(Angle));
  else if (Angle>0.0)
    return Quaternion(1.0,Geometry::Vec3D(0,0,0));
  else
    return Quaternion(0.0,ZRotDef.unit());
    
  Axis*=sin(Angle/2.0);
  return Quaternion(cos(Angle/2.0),Axis);
}

Quaternion
Quaternion::calcQRot(const double Angle,Geometry::Vec3D Axis)
  /*!
    Calculate Quaternion value for a given angle 
    \param Angle :: angle to calculate Q0 from [radian]
    \param Axis :: Rotation axis
    \return Q
  */
{
  Axis.makeUnit();
  Axis*=sin(Angle/2.0);
  return Quaternion(cos(Angle/2.0),Axis);
}

Quaternion
Quaternion::calcQRotDeg(const double Angle,Geometry::Vec3D Axis)
  /*!
    Calculate Quaternion value for a given angle 
    \param Angle :: angle to calculate Q0 from [deg]
    \param Axis :: Rotation axis
    \return Q
  */
{
  Axis.makeUnit();
  Axis*=sin(M_PI*Angle/360.0);
  return Quaternion(cos(M_PI*Angle/360.0),Axis);
}

Quaternion::Quaternion() :
  q0(0),Qvec()
  /*!
    Constructor
  */
{}

Quaternion::Quaternion(const double qx,const double qa,
		       const double qb,const double qc) :
  q0(qx),Qvec(qa,qb,qc)
  /*!
    Constructor
    \param qx :: q0 component
    \param qa :: q1
    \param qb :: q2
    \param qc :: q3
  */
{}

Quaternion::Quaternion(const double qx,const Vec3D& AP) :
  q0(qx),Qvec(AP)
  /*!
    Constructor
    \param qx :: q0 component
    \param AP :: (q1, q2, q3)
  */
{}

Quaternion::Quaternion(const Quaternion& A) :
  q0(A.q0),Qvec(A.Qvec)
  /*!
    Copy Constructor
    \param A :: Quaternion to copy
  */
{}

Quaternion&
Quaternion::operator=(const Quaternion& A)
  /*!
    Assignment operator
    \param A :: Quaternion to copy
    \return *this
  */
{
  if (this!=&A)
    {
      q0=A.q0;
      Qvec=A.Qvec;
    }
  return *this;
}

Quaternion::~Quaternion()
  /*!
    Destructor
  */
{}

double
Quaternion::operator[](const size_t Index) const
  /*!
    Returns the individual component of the quaternion
    \param Index :: Index to search
    \throw RangeError if Index out of range
    \return  \f$ {\it q_i} \f$
  */
{
  if (Index==0)
    return q0;
  return Qvec[Index-1];
}

double&
Quaternion::operator[](const size_t Index) 
  /*!
    Returns the individual component of the quaternion
    \param Index :: Index to search
    \throw RangeError if Index out of range
    \return  accessed \f$ {\it q_i} \f$
  */
{
  if (Index==0)
    return q0;
  return Qvec[Index-1];
}

bool
Quaternion::operator==(const Quaternion& A) const
  /*!
    Determine if two Quaternions are equal (within tolerance).
    \param A :: Quaternion to compare
    \return A==This
  */
{
  const double QTolerance(1e-6); 
  if (&A==this) return 1;
  if (fabs(q0-A.q0)>QTolerance)
    return 0;
  return (Qvec==A.Qvec) ? 1 : 0;
}

bool
Quaternion::operator!=(const Quaternion& A) const
  /*!
    Determine if two Quaternion are not equal (within tolerance).
    \param A :: Quaternion to compare
    \return A!=This
  */
{
  return !(this->operator==(A));
}


Quaternion&
Quaternion::complement()
  /*!
    Takes the standard quaternion complemnary
    \return *this
  */
{
  Qvec*=-1.0;
  return *this;
}

Quaternion&
Quaternion::inverse()
  /*!
    Takes the standard quaternion complemnary
    \return *this
  */
{
  Qvec*=-1.0;
  makeUnit();
  return *this;
}

Quaternion&
Quaternion::operator+=(const Quaternion& Q)
  /*!
    Standard Quarternion component addition
    \param Q :: Quaternion to add
    \return self added 
  */
{
  q0 += Q.q0;
  Qvec += Q.Qvec;
  return *this;
}

Quaternion
Quaternion::operator+(const Quaternion& Q) const
  /*!
    Standard Quarternion component addition
    \param Q :: Quaternion to add
    \return self added Quaternion
  */
{
  Quaternion Out(*this);
  return Out+=Q;
}

Quaternion&
Quaternion::operator-=(const Quaternion& Q)
  /*!
    Standard Quarternion component addition
    \param Q :: Quaternion to subtract
    \return self subtracted value
  */
{
  q0-=Q.q0;
  Qvec-=Q.Qvec;
  return *this;
}

Quaternion
Quaternion::operator-(const Quaternion& Q) const
  /*!
    Standard Quarternion component addition
    \param Q :: Quaternion to add
    \return  p-q
  */
{
  Quaternion Out(*this);
  return Out-=Q;
}

Quaternion&
Quaternion::operator*=(const Quaternion& Q)
  /*!
    Standard Quarternion multiplication
    normally represented as 
    \f[
    {\bf pq } = [ p_0q_0 - {\bf p} . {\bf q}, 
         p_0 {\bf q } + q_0 {\bf p } + {\bf p} \times {\bf q}  ] 
    \f]

    \param Q :: Quaternion to multipy
    \return Full shifted quaternion
  */
{
  const double pq0=q0*Q.q0 - Qvec.dotProd(Q.Qvec);
  Qvec=Qvec*Q.q0 + Q.Qvec*q0 + Qvec*Q.Qvec;
  q0=pq0;
  return *this;
}

Quaternion&
Quaternion::operator*=(const Vec3D& R)
  /*!
    Product with a vector
    \f[
    {\bf q}.\vec{r}
    \f]
    \param R :: standard vector (3d) 
    \return Quaternion produce Q*[0,r]
  */
{
  const double qr= - Qvec.dotProd(R);
  Qvec= R*q0+Qvec*R;
  q0=qr;
  return *this;
}

Quaternion
Quaternion::operator*(const Vec3D& R) const
  /*!
    Product with a vector
    \f$ {\bf q}.\vec{r} \f$
    \param R :: standard vector (3d) 
    \return Quaternion produce Q*[0,r]
  */
{
  Quaternion Out(*this);
  return Out*=R;
}

Quaternion
Quaternion::operator*(const Quaternion& Q) const
  /*!
    Standard Quarternion multiplication
    normally represented as 
    \f[
    {\bf pq } = [ p_0q_0 - {\bf p} . {\bf q}, 
         p_0 {\bf q } + q_0 {\bf p } + {\bf p} \times {\bf q}  ] 
    \f]
    The order has to be respected hence we calculate
    Out*=Q.
    \param Q :: Quaternion to multipy
    \return Full shifted quaternion
  */
{
  Quaternion Out(*this);
  return Out*=Q;
}

double
Quaternion::makeUnit()
  /*!
    Make the Quaternion unitary
    \return Old magnitde
   */
{
  double UVal=modulus();
  if (fabs(UVal)>1e-10)
    {
      q0/=UVal;
      Qvec/=UVal;
    }
  return UVal;
}


double 
Quaternion::modulus() const
  /*!
    Calculates the modulus
    \return |q| 
  */
{
  return sqrt(q0*q0+Qvec.dotProd(Qvec));
}

Matrix<double>
Quaternion::qMatrix() const
  /*!
    Generate the full 4x4 Q(q) matrix
    \return Q(q)
  */
{
  Matrix<double> Out(4,4);
  Out[0][0]=q0;
  for(size_t i=1;i<4;i++)
    {
      Out[0][i]= Qvec[i-1];
      Out[i][0]= -Qvec[i-1];
      Out[i][i]= q0;
    }

  Out[1][2]= -Qvec[2];
  Out[2][1]= Qvec[2];

  Out[1][3]= Qvec[1];
  Out[3][1]= -Qvec[1];

  Out[2][3]= -Qvec[0];
  Out[3][2]= Qvec[0];

  return Out;    
}

Matrix<double>
Quaternion::rMatrix() const
  /*!
    Generate the 3x3 rotation R(q) matrix
    \return R(q)
  */
{
//  const Quaternion& q(*this);   // avoid lot of this->

  Matrix<double> Out(3,3);
  const double q1=Qvec[0];
  const double q2=Qvec[1];
  const double q3=Qvec[2];
  
  Out[0][0]=1.0 - 2.0*(q2*q2+q3*q3);
  Out[1][1]=1.0 - 2.0*(q3*q3+q1*q1);
  Out[2][2]=1.0 - 2.0*(q1*q1+q2*q2);

  Out[0][1]= 2.0*(q1*q2-q0*q3);
  Out[0][2]= 2.0*(q1*q3+q0*q2);
  
  Out[1][0]= 2.0*(q1*q2+q0*q3);
  Out[1][2]= 2.0*(q2*q3-q0*q1);
  
  Out[2][0]= 2.0*(q3*q1-q0*q2);
  Out[2][1]= 2.0*(q3*q2+q0*q1);

  return Out;
}

Geometry::Vec3D&
Quaternion::rotate(Vec3D& V) const
  /*!
    Rotate a vector (in place)
    \param V :: Vector to be rotated 
    \return V_rotated
    The quaternion is assumed to be normalized
    then \f$ q.v.q^{-1} \f$ is the rotated value.
  */
{
   Quaternion QI(*this);
   Quaternion RV(0.0,V);
   RV*=QI.inverse();
   RV=(*this)*RV;
   V=RV.getVec();
   return V;
 }

Geometry::Vec3D&
Quaternion::invRotate(Vec3D& V) const
  /*!
    Rotate a vector (in place) in the inverse sense
    \param V :: Vector to be rotated 
    \return V_rotated
    The quaternion is assumed to be normalized
    then \f$ q.v.q^{-1} \f$ is the rotated value.
  */
{
   Quaternion QI(*this);
   Quaternion RV(0.0,V);
   RV*=QI;
   RV=QI.inverse()*RV;
   V=RV.getVec();
   return V;
 }

Geometry::Vec3D
Quaternion::getAxis() const
  /*!
    Calculates the axis of rotation
    \return Axis [unit vec]
  */
{
  Vec3D Out=Qvec;
  Out.makeUnit();
  return Out;
}

double
Quaternion::getTheta() const
  /*!
    Calculates the rotation angle.
    Which is checked against the sin(theta) and cos(theta)
    to calculate the sign.
    \return Theta [rad]
  */
{
  if (fabs(q0)>1)
    throw ColErr::RangeError<double>(q0,-1,1,"Quaternion::getAngle");
  
  return 2.0*acos(q0);
}


void 
Quaternion::read(std::istream& IX)
  /*!
    Read in a quaternion
    \param IX :: Input stream
  */
{
  IX>>q0>>Qvec;
  return;
}

bool
Quaternion::zeroAngle(const double& tolerance) const
  /*!
    Determine in the system is about a zero angle rotation
    \param tolerance
    \return true is zero rotation
   */
{
  return (fabs(1.0-q0)<tolerance) ? 1 : 0;
}

void 
Quaternion::write(std::ostream&  OX) const
  /*!
    Write out quaternion in bracket form
    \param OX :: Output stream
  */
{
  OX<<q0<<" "<<Qvec;
  return;
}

Quaternion 
Quaternion::basisRotate(const Vec3D& xa,const Vec3D& xb,
			const Vec3D& xc,const Vec3D& ya,
			const Vec3D& yb,const Vec3D& yc)
  /*!
    Given basis set A convert to basis set B
    Following the method in http://charles.karney.info/biblio/quat.html
    \param xa :: initial X
    \param xb :: initial Y
    \param xc :: initial Z
    \param ya :: new X
    \param yb :: new Y
    \param yc :: new Z
    \return Quaternion
  */
{
  Matrix<double> B(4,4);
  const Vec3D xPrime[3]={xa,xb,xc};
  const Vec3D yPrime[3]={ya,yb,yc};
  for(size_t i=0;i<3;i++)
    {
      Vec3D a=yPrime[i]+xPrime[i];
      Vec3D b=yPrime[i]-xPrime[i];
      // Form A Matrix
      Matrix<double> A(4,4);
      for(size_t ii=0;ii<3;ii++)
        {
	  A[0][ii+1]= -b[ii];
	  A[ii+1][0]= b[ii];
	}
      A[1][2]=-a[2];
      A[1][3]=a[1];
      A[2][1]=a[2];
      A[2][3]= -a[0];
      A[3][1]= -a[1];
      A[3][2]= a[0];
      Matrix<double> AT(A);
      AT.Transpose();
      B += AT*A;
    } 
  B /= 3.0;

  Matrix<double> EigenVec(4,4);                    ///< EigenVec
  Matrix<double> EigenVal(4,4);                    ///< Eignen Val

  if (!B.Diagonalise(EigenVec,EigenVal))
    throw ColErr::NumericalAbort
      ("B diagonalization failure Quaternion::basicRotate");

  Quaternion Out;
  EigenVec.sortEigen(EigenVal);
  for(size_t i=0;i<4;i++)
    Out[i]=EigenVec[i][0];
  return Out;
}

}  // NAMESPACE Geometry
