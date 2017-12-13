/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geometry/Quadratic.cxx
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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <complex>
#include <cmath>
#include <vector>
#include <list>
#include <stack>
#include <map>
#include <string>
#include <algorithm>
#include <boost/format.hpp>

#include "Exception.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "mathSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "masterWrite.h"
#include "Quaternion.h"
#include "PolyFunction.h"
#include "PolyVar.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"

namespace Geometry
{

Quadratic::Quadratic() : Surface(),
  BaseEqn(10)
  /*!
    Constructor
  */
{}

Quadratic::Quadratic(const int N,const int T) : 
  Surface(N,T),BaseEqn(10)
  /*!
    Constructor
    \param N :: Name
    \param T :: Transform number
  */
{}

Quadratic::Quadratic(const Quadratic& A) : Surface(A),
  BaseEqn(A.BaseEqn)
  /*!
    Copy constructor
    \param A :: Quadratic to copy
  */
{ }


Quadratic&
Quadratic::operator=(const Quadratic& A)
  /*!
    Assignment operator
    \param A :: Quadratic to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Surface::operator=(A);
      BaseEqn=A.BaseEqn;
    }
  return *this;
}

Quadratic::~Quadratic()
  /*!
    Destructor
  */
{}

Quadratic*
Quadratic::clone() const
  /*!
    Makes a clone (implicit virtual copy constructor) 
    \return Quadratic(this)
  */
{
  return new Quadratic(*this);
}

bool
Quadratic::operator==(const Quadratic& A) const
  /*!
    Determine if two Quadratics are equal (within tolerance).
    \param A :: Quadratic to compare
    \return A==This
  */
{
  if (&A==this) return 1;
  for(size_t i=0;i<BaseEqn.size();i++)
    if (fabs(BaseEqn[i]-A.BaseEqn[i])>Geometry::zeroTol)
      {
	if (fabs(BaseEqn[i]+A.BaseEqn[i])>Geometry::zeroTol)
	  return 0;
	// Now test negative:
	for(size_t j=0;j<BaseEqn.size();j++)
	  if (fabs(BaseEqn[j]+A.BaseEqn[j])>Geometry::zeroTol)
	    return 0;
      }
  return 1;
}

bool
Quadratic::operator!=(const Quadratic& A) const
  /*!
    Determine if two Quadratics are not equal (within tolerance).
    \param A :: Quadratic to compare
    \return A!=This
  */
{
  return !(this->operator==(A));
}

double 
Quadratic::eqnValue(const Geometry::Vec3D& Pt) const
  /*!
    Helper function to calcuate the value
    of the equation at a fixed point 
    \param Pt :: Point to determine the equation surface 
    value at
    \return value Eqn(Pt) : -ve inside +ve outside
  */
{
  double res(0.0);
  res+=BaseEqn[0]*Pt[0]*Pt[0];
  res+=BaseEqn[1]*Pt[1]*Pt[1];
  res+=BaseEqn[2]*Pt[2]*Pt[2];
  res+=BaseEqn[3]*Pt[0]*Pt[1];    
  res+=BaseEqn[4]*Pt[0]*Pt[2];    
  res+=BaseEqn[5]*Pt[1]*Pt[2];    
  res+=BaseEqn[6]*Pt[0];    
  res+=BaseEqn[7]*Pt[1];    
  res+=BaseEqn[8]*Pt[2];    
  res+=BaseEqn[9];

  return res;
}

int
Quadratic::setSurface(const std::string&)
  /*!
    Use General Surface ?
    \return -1
   */
{
  return -1;
}

  
void 
Quadratic::setBaseEqn()
  /*!
    Set baseEqn (nothing to do) as it is 
    already a baseEqn driven system
  */
{
  return;
}
  
int
Quadratic::side(const Geometry::Vec3D& Pt) const
  /*!
    Determine if the the Point is true to the surface or
    on the other side
    \param Pt :: Point to check
    \retval 1 : if the point is "true" to the surface  
    \retval -1 : if the point is "false" to the surface  
    \retval 0 :: The point is on the surface
  */
{
  double res=eqnValue(Pt);
  if (fabs(res)<Geometry::zeroTol)
    return 0;
  return (res>0) ? 1 : -1;
}


Geometry::Vec3D
Quadratic::surfaceNormal(const Geometry::Vec3D& Pt) const
  /*!
    Given a point on the surface 
    Calculate the normal at the point 
    Some rather disturbing behaviour happens if 
    the point is a significant distance from the surface
    \param Pt :: Point to calcution
    \return normal unit vector
  */
{
   Geometry::Vec3D N(2*BaseEqn[0]*Pt[0]+BaseEqn[3]*Pt[1]+
		     BaseEqn[4]*Pt[2]+BaseEqn[6],
		     2*BaseEqn[1]*Pt[1]+BaseEqn[3]*Pt[0]+
		     BaseEqn[5]*Pt[2]+BaseEqn[7],
		     2*BaseEqn[2]*Pt[2]+BaseEqn[4]*Pt[0]+
		     BaseEqn[5]*Pt[1]+BaseEqn[8]);
   N.makeUnit();
   return N;
}

void
Quadratic::matrixForm(Geometry::Matrix<double>& A,
		      Geometry::Vec3D& B,double& C) const
  /*!
    Converts the baseEqn into the matrix form such that
    \f[ x^T A x + B^T x + C =0 \f]
    \param A :: Matrix to place equation into
    \param B :: Vector point 
    \param C :: Constant value
  */
{
  A.setMem(3,3);    // set incase memory out
  for(size_t i=0;i<3;i++)
    A[i][i]=BaseEqn[i];

  A[0][1]=A[1][0]=BaseEqn[3]/2.0;
  A[0][2]=A[2][0]=BaseEqn[4]/2.0;
  A[1][2]=A[2][1]=BaseEqn[5]/2.0;
  
  for(size_t i=0;i<3;i++)
    B[i]=BaseEqn[6+i];
  C=BaseEqn[9];
  return;
}

double
Quadratic::distance(const Geometry::Vec3D& Pt) const
  /*!
    Proper calcuation of a point to a general surface 
    \param Pt :: Point to calculate distance from surace
    \return distance from point to surface (signed)
  */
{
  // Job 1 :: Create matrix and vector representation
  Geometry::Matrix<double> A(3,3);
  Geometry::Vec3D B;
  double cc;
  matrixForm(A,B,cc);
  
  //Job 2 :: calculate the diagonal matrix
  Geometry::Matrix<double> D(3,3);
  Geometry::Matrix<double> R(3,3);
  if (!A.Diagonalise(R,D))
    {
      std::cerr<<"Problem with matrix :: distance now guessed at"<<std::endl;
      return distance(Pt);
    }

  Geometry::Vec3D alpha=R.Tprime()*Pt;
  Geometry::Vec3D beta=R.Tprime()*B;
    
  // Calculate fundermental equation:
  const double aa(alpha[0]);  const double aa2(aa*aa);
  const double ab(alpha[1]);  const double ab2(ab*ab);
  const double ac(alpha[2]);  const double ac2(ac*ac);

  const double ba(beta[0]);  const double ba2(ba*ba);
  const double bb(beta[1]);  const double bb2(bb*bb);
  const double bc(beta[2]);  const double bc2(bc*bc);

  const double da(D[0][0]);  const double da2(da*da);
  const double db(D[1][1]);  const double db2(db*db);
  const double dc(D[2][2]);  const double dc2(dc*dc);
  
  mathLevel::PolyVar<1> T(6UL);

  T[0]=aa*ba+ab*bb+ac*bc+cc+aa2*da+ab2*db+ac2*dc;

  T[1]= -ba2-bb2-bc2 + 4*ab*bb*da + 4*ac*bc*da + 4*cc*da + 4*aa*ba*db + 
	 4*ac*bc*db + 4*cc*db + 4*aa2*da*db + 4*ab2*da*db + 
	 4*aa*ba*dc + 4*ab*bb*dc + 4*cc*dc + 4*aa2*da*dc + 
	 4*ac2*da*dc + 4*ab2*db*dc +  4*ac2*db*dc;
	 
  T[2]= -ba2*da -4*bb2*da -4*bc2*da + 4*ab*bb*da2 + 4*ac*bc*da2 + 4*cc*da2  
    - 4*ba2*db - bb2*db - 4*bc2*db + 16*ac*bc*da*db + 
    16*cc*da*db + 4*ab2*da2*db + 4*aa*ba*db2 + 
    4*ac*bc*db2 + 4*cc*db2 + 4*aa2*da*db2 - 
    4*ba2*dc - 4*bb2*dc - bc2*dc + 16*ab*bb*da*dc + 
    16*cc*da*dc + 4*ac2*da2*dc + 16*aa*ba*db*dc + 
    16*cc*db*dc + 16*aa2*da*db*dc + 16*ab2*da*db*dc + 
    16*ac2*da*db*dc + 4*ac2*db2*dc + 4*aa*ba*dc2 + 
    4*ab*bb*dc2 + 4*cc*dc2 + 4*aa2*da*dc2 + 
    4*ab2*db*dc2;

  T[3]= -4*bb2*da2 - 4*bc2*da2 - 4*ba2*da*db - 4*bb2*da*db - 
         16*bc2*da*db + 16*ac*bc*da2*db + 16*cc*da2*db - 
          4*ba2*db2 - 4*bc2*db2 + 16*ac*bc*da*db2 + 
          16*cc*da*db2 - 4*ba2*da*dc - 16*bb2*da*dc - 
          4*bc2*da*dc + 16*ab*bb*da2*dc + 16*cc*da2*dc - 
          16*ba2*db*dc - 4*bb2*db*dc - 4*bc2*db*dc + 
          64*cc*da*db*dc + 16*ab2*da2*db*dc + 
          16*ac2*da2*db*dc + 16*aa*ba*db2*dc + 
          16*cc*db2*dc + 16*aa2*da*db2*dc + 
          16*ac2*da*db2*dc - 4*ba2*dc2 - 4*bb2*dc2 + 
          16*ab*bb*da*dc2 + 16*cc*da*dc2 + 16*aa*ba*db*dc2 + 
          16*cc*db*dc2 + 16*aa2*da*db*dc2 + 
          16*ab2*da*db*dc2;

  T[4]=-4*bb2*da2*db - 16*bc2*da2*db - 4*ba2*da*db2 - 16*bc2*da*db2 + 
         16*ac*bc*da2*db2 + 16*cc*da2*db2 - 16*bb2*da2*dc - 4*bc2*da2*dc - 
          16*ba2*da*db*dc - 16*bb2*da*db*dc - 16*bc2*da*db*dc + 64*cc*da2*db*dc - 
          16*ba2*db2*dc - 4*bc2*db2*dc + 64*cc*da*db2*dc + 16*ac2*da2*db2*dc - 
          4*ba2*da*dc2 - 16*bb2*da*dc2 + 16*ab*bb*da2*dc2 + 16*cc*da2*dc2 - 
          16*ba2*db*dc2 - 4*bb2*db*dc2 + 64*cc*da*db*dc2 + 16*ab2*da2*db*dc2 + 
          16*aa*ba*db2*dc2 + 16*cc*db2*dc2 + 16*aa2*da*db2*dc2;


  T[5]= -16*bc2*da2*db2 - 16*bb2*da2*db*dc - 16*bc2*da2*db*dc - 
          16*ba2*da*db2*dc - 16*bc2*da*db2*dc + 
          64*cc*da2*db2*dc - 16*bb2*da2*dc2 - 
          16*ba2*da*db*dc2 - 16*bb2*da*db*dc2 + 
          64*cc*da2*db*dc2 - 16*ba2*db2*dc2 + 
          64*cc*da*db2*dc2;

  T[6]= 16*da*db*dc*(-bc2*da*db -bb2*da*dc -ba2*db*dc + 
		     4*cc*da*db*dc);

  std::vector<double> TRange=T.realRoots(1e-10);
  if (TRange.empty())
    return -1.0;

  double Out= -1;
  Geometry::Vec3D xvec;
  for(const double& val : TRange)
    {
      const double daI=1.0+2* val *da;
      const double dbI=1.0+2* val *db;
      const double dcI=1.0+2* val *dc;
      if ((daI*daI)>Geometry::zeroTol && (dbI*dbI)>Geometry::zeroTol &&
	  (dcI*dcI)>Geometry::zeroTol)
        {
	  Geometry::Matrix<double> DI(3,3);
	  DI[0][0]=1.0/daI;
	  DI[1][1]=1.0/dbI;
	  DI[2][2]=1.0/dcI;
	  xvec = R*(DI*(alpha-beta* val));  // care here: to avoid 9*9 +9*3 in favour of 9*3+9*3 ops.
	  const double Dist=xvec.Distance(Pt);
	  if (Out<0 || Out>Dist)
	    Out=Dist;
	}
    }
  return Out;
}

int 
Quadratic::onSurface(const Geometry::Vec3D& Pt) const
  /*!
    Test to see if a point is on the surface 
    \param Pt :: Point to test
    \returns 0 : if not on surface; 1 if on surace
  */
{
  const double res=eqnValue(Pt);
  return (fabs(res)>sqrt(Geometry::zeroTol)) ? 0 : 1;
}


void
Quadratic::displace(const Geometry::Vec3D& Pt)
  /*!
    Apply a general displacement to the surface
    \param Pt :: Point to add to surface coordinate
  */
{
  BaseEqn[9]+= Pt[0]*(Pt[0]*BaseEqn[0]-BaseEqn[6])+
               Pt[1]*(Pt[1]*BaseEqn[1]-BaseEqn[7])+
               Pt[2]*(Pt[2]*BaseEqn[2]-BaseEqn[8])+
               BaseEqn[3]*Pt[0]*Pt[1]+
               BaseEqn[4]*Pt[0]*Pt[2]+
               BaseEqn[5]*Pt[1]*Pt[2];  
  BaseEqn[6]+= -2*BaseEqn[0]*Pt[0]-BaseEqn[3]*Pt[1]-BaseEqn[4]*Pt[2];
  BaseEqn[7]+= -2*BaseEqn[1]*Pt[1]-BaseEqn[3]*Pt[0]-BaseEqn[5]*Pt[2];
  BaseEqn[8]+= -2*BaseEqn[2]*Pt[2]-BaseEqn[4]*Pt[0]-BaseEqn[5]*Pt[1];
  return;
}

void 
Quadratic::rotate(const Geometry::Matrix<double>& MX) 
  /*!
    Rotate the surface by matrix MX
    \param MX :: Matrix for rotation (not inverted like MCNPX)
   */
{
  Geometry::Matrix<double> MA=MX;
  MA.Invert();
  const double a(MA[0][0]),b(MA[0][1]),c(MA[0][2]);
  const double d(MA[1][0]),e(MA[1][1]),f(MA[1][2]);
  const double g(MA[2][0]),h(MA[2][1]),j(MA[2][2]);
  double B[9];
  B[0]=BaseEqn[0]*a*a+BaseEqn[1]*d*d+BaseEqn[2]*g*g+
    BaseEqn[3]*a*d+BaseEqn[4]*a*g+BaseEqn[5]*d*g;

  B[1]=BaseEqn[0]*b*b+BaseEqn[1]*e*e+BaseEqn[2]*h*h+
    BaseEqn[3]*b*e+BaseEqn[4]*b*h+BaseEqn[5]*e*h;

  B[2]=BaseEqn[0]*c*c+BaseEqn[1]*f*f+BaseEqn[2]*j*j+
    BaseEqn[3]*c*f+BaseEqn[4]*c*j+BaseEqn[5]*f*j;

  B[3]=2.0*(BaseEqn[0]*a*b+BaseEqn[1]*d*e+BaseEqn[2]*g*h)+
    BaseEqn[3]*(a*e+b*d)+BaseEqn[4]*(a*h+b*g)+BaseEqn[5]*(d*h+e*g);

  B[4]=2.0*(BaseEqn[0]*a*c+BaseEqn[1]*d*f+BaseEqn[2]*g*j)+
    BaseEqn[3]*(a*f+c*d)+BaseEqn[4]*(a*j+c*g)+BaseEqn[5]*(d*j+f*g);

  B[5]=2.0*(BaseEqn[0]*b*c+BaseEqn[1]*e*f+BaseEqn[2]*h*j)+
    BaseEqn[3]*(b*f+c*e)+BaseEqn[4]*(b*j+c*h)+BaseEqn[5]*(e*j+f*h);

  B[6]=BaseEqn[6]*a+BaseEqn[7]*d+BaseEqn[8]*g;

  B[7]=BaseEqn[6]*b+BaseEqn[7]*e+BaseEqn[8]*h;

  B[8]=BaseEqn[6]*c+BaseEqn[7]*f+BaseEqn[8]*j;

  for(size_t i=0;i<9;i++)       // Item 9 left alone
    BaseEqn[i]=B[i];
  return;
}

void 
Quadratic::rotate(const Geometry::Quaternion& QA) 
  /*!
    Rotate the surface by matrix. Converts the item in to 
    a matrix and rotates
    \param QA :: Quaternion to rotate by 
  */
{
  const Geometry::Matrix<double> MA=QA.rMatrix();
  Quadratic::rotate(MA);
  return;
}

void
Quadratic::mirror(const Geometry::Plane& P)
  /*!
    Create a mirror of a quadratic based on a plane 
    \param P :: Plane to rotate from
  */
{
  ELog::RegMethod RegA("Quadratic","mirror");

  const Geometry::Vec3D NormV=P.getNormal();
  const double nx(NormV[0]);
  const double ny(NormV[1]);
  const double nz(NormV[2]);
  const double distX(2.0*P.getDistance()*nx);
  const double distY(2.0*P.getDistance()*ny);
  const double distZ(2.0*P.getDistance()*nz);

  const double A(BaseEqn[0]);
  const double B(BaseEqn[1]);
  const double C(BaseEqn[2]);
  const double D(BaseEqn[3]);
  const double E(BaseEqn[4]);
  const double F(BaseEqn[5]);
  const double G(BaseEqn[6]);
  const double H(BaseEqn[7]);
  const double J(BaseEqn[8]);
  const double K(BaseEqn[9]);

  // x_1: 
  const double a(1-2*nx*nx);
  const double b(-2*nx*ny);
  const double c(-2*nx*nz);

  // x_2: 
  const double d(-2*nx*ny);
  const double e(1-2*ny*ny);
  const double f(-2*ny*nz);

  // x_3: 
  const double g(-2*nx*nz);
  const double h(-2*ny*nz);
  const double j(1-2*nz*nz);

  std::vector<double> NB;         // New base

  BaseEqn[0]=A*a*a+B*d*d+C*g*g+D*a*d+
    E*a*g+F*d*g;
  BaseEqn[1]=A*b*b+B*e*e+C*h*h+D*b*e+
    E*b*h+F*e*h;
  BaseEqn[2]=A*c*c+B*f*f+C*j*j+D*c*f+
    E*c*j+F*f*j;

  // xy
  BaseEqn[3]=A*2*a*b+B*2*d*e+C*2*g*h+
    D*(a*e+b*d)+E*(a*h+b*g)+F*(d*h+e*g);

  // xz
  BaseEqn[4]=A*2*a*c+B*2*d*f+C*2*g*j+
    D*(a*f+c*d)+E*(a*j+c*g)+F*(d*j+f*g);

  // yz
  BaseEqn[5]=A*2*b*c+B*2*e*f+C*2*h*j+
    D*(b*f+c*e)+E*(b*j+c*h)+F*(e*j+f*h);

  // x
  BaseEqn[6]=A*2*distX*a+B*2*distY*d+C*2*distZ*g+
    D*(distX*a+distY*d)+E*(distX*a+distZ*g)+
    F*(distY*d+distZ*g)+G*a+H*d+J*g;

  // y
  BaseEqn[7]=A*2*distX*b+B*2*distY*e+C*2*distZ*h+
    D*(distX*b+distY*e)+E*(distX*b+distZ*h)+
    F*(distY*e+distZ*h)+G*b+H*e+J*h;

  // z
  BaseEqn[8]=A*2*distX*c+B*2*distY*f+C*2*distZ*j+
    D*(distX*c+distY*f)+E*(distX*c+distZ*j)+
    F*(distY*e+distZ*h)+G*c+H*f+J*j;

  // Const:
  BaseEqn[9]=A*distX*distX+B*distY*distY+C*distZ*distZ+
    D*distX*distY+ E*distX*distZ+ F*distY*distZ+
    G*distX+H*distY+J*distZ+K;

  return;
}
  
void
Quadratic::print() const
  /*! 
    Print out the genreal equation 
    for debugging.
  */
{
  Surface::print();
  std::cout<<" :: ";
  for(size_t i=0;i<10;i++)
    std::cout<<BaseEqn[i]<<" ";
  std::cout<<std::endl;
  return;
}

void
Quadratic::normalizeGEQ(const size_t index) 
  /*!
    normalize a single equation variable
    \param index :: index value to divide
  */
{
  ELog::RegMethod RegA("Quadratic","normalizeGEQ");
  if (index>10)
    throw ColErr::IndexError<size_t>(index,10,"index");
  if (fabs(BaseEqn[index])>Geometry::zeroTol)
    {
      const double DVal=BaseEqn[index];
      for(size_t i=0;i<10;i++)
	BaseEqn[i]/=DVal;
    }
  return;
}

void
Quadratic::writeFLUKA(std::ostream& OX) const
  /*!
    Writes out  an Fluka surface description [free format]
    \param OX :: Output stream (required for multiple std::endl)
  */
{
  ELog::RegMethod RegA("Quadratic","writeFLUKA");
  masterWrite& MW=masterWrite::Instance();
  
  std::ostringstream cx;
  cx.precision(Geometry::Nprecision);
  cx<<"QUA s"<<getName();
  // write all 10 items in order: as xy xz yz coeffients
  for(const double& val : BaseEqn)
    cx<<" "<<MW.Num(val);
  StrFunc::writeMCNPX(cx.str(),OX);
  return;
}

void
Quadratic::writePOVRay(std::ostream& OX) const
  /*!
  /*! 
    Write out the cylinder for POV-Ray
    POVray required Ax^2+By^2+Cz^2+Dxy+Exz+Fyz+Gx+Hy+Jz+K=0
    \param OX :: output stream
  */
{
  ELog::RegMethod RegA("Quadratic","writePOVRay");
  masterWrite& MW=masterWrite::Instance();
  
  OX<<"#declare s"<<getName()<<" = quadric{ \n"
    <<"<"
    <<MW.Num(BaseEqn[0])<<","<<MW.Num(BaseEqn[1])<<","<<MW.Num(BaseEqn[2])
    <<">,<"
    <<MW.Num(BaseEqn[3])<<","<<MW.Num(BaseEqn[4])<<","<<MW.Num(BaseEqn[5])
    <<">,<"
    <<MW.Num(BaseEqn[6])<<","<<MW.Num(BaseEqn[7])<<","<<MW.Num(BaseEqn[8])
    <<">,"<<MW.Num(BaseEqn[9])<<"\n"<<" }"<<std::endl;  
  return;
}
  
void
Quadratic::write(std::ostream& OX) const
  /*!
    Writes out  an MCNP surface description 
    Note : Swap since base equation is swapped in gq output 
    (mcnp 4c manual pg. 3-14)
    \param OX :: Output stream (required for multiple std::endl)
  */
{
  std::ostringstream cx;
  writeHeader(cx);
  cx.precision(Geometry::Nprecision);
  cx<<"gq ";
  for(size_t i=0;i<4;i++)
    cx<<" "<<BaseEqn[i]<<" ";
  cx<<" "<<BaseEqn[5]<<" ";
  cx<<" "<<BaseEqn[4]<<" ";
  for(size_t i=6;i<10;i++)
    cx<<" "<<BaseEqn[i]<<" ";
  StrFunc::writeMCNPX(cx.str(),OX);
  return;
}


void
Quadratic::writeXML(const std::string&) const
  /*!
    The generic XML writing system.
    This should call the virtual function procXML
    to correctly built the XMLcollection.
    \param :: Filename 
  */
{
  /*
  XML::XMLwriteVisitor XVisit;
  this->acceptVisitor(XVisit);
  XVisit.writeFile(Fname);
  return;
  */
}

  
}   // NAMESPACE Geometry
