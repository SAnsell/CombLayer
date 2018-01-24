/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geometry/Cylinder.cxx
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
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <map>
#include <stack>
#include <string>
#include <algorithm>
#include <boost/format.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Line.h"
#include "Surface.h"
#include "masterWrite.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"

#include "Debug.h"

namespace Geometry
{

Cylinder::Cylinder() : Quadratic(),
   Centre(),Normal(1,0,0),Nvec(0),Radius(0.0)
  /*!
    Standard Constructor creats a cylinder (radius 0)
    along the x axis
  */
{
  // Called after it has been sized by Quadratic
  Cylinder::setBaseEqn();
}

Cylinder::Cylinder(const int N,const int T) : 
  Quadratic(N,T),
  Centre(),Normal(1,0,0),Nvec(0),Radius(0.0)
  /*!
    Standard Constructor creates a cylinder (radius 0)
    along the x axis
    \param N :: Name
    \param T :: Transform
  */
{
  // Called after it has been sized by Quadratic
  Cylinder::setBaseEqn();
}

Cylinder::Cylinder(const Cylinder& A) :
  Quadratic(A),Centre(A.Centre),Normal(A.Normal),
  Nvec(A.Nvec),Radius(A.Radius)
  /*!
    Standard Copy Constructor
    \param A :: Cyclinder to copy
  */
{}

Cylinder*
Cylinder::clone() const
  /*!
    Makes a clone (implicit virtual copy constructor) 
    \return Copy(*this)
  */
{
  return new Cylinder(*this);
}

Cylinder&
Cylinder::operator=(const Cylinder& A) 
  /*!
    Standard Assignment operator
    \param A :: Cylinder object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Quadratic::operator=(A);
      Centre=A.Centre;
      Normal=A.Normal;
      Nvec=A.Nvec;
      Radius=A.Radius;
    }
  return *this;
}

Cylinder::~Cylinder()
  /*!
    Standard Destructor
  */
{}

bool
Cylinder::operator!=(const Cylinder& A) const
  /*!
    Determine if two cylinders are equal (within tolerance).
    This is complicated by the fact that both centres can be different
    but it is the same cylinder. Or that the axis can be 180 degrees.
    \param A :: Cylinder to compare
    \return this!=A
  */
{
  return !(this->operator==(A));
}

bool
Cylinder::operator==(const Cylinder& A) const
  /*!
    Determine if two cylinders are equal (within tolerance).
    This is complicated by the fact that both centres can be different
    but it is the same cylinder. Or that the axis can be 180 degrees.
    \param A :: Cylinder to compare
    \return this==A
  */
{
  ELog::RegMethod RegA("Cylinder","operator==");

  if (&A==this) return 1;

  if (fabs(Radius-A.Radius)>Geometry::zeroTol)
    return 0;

  // Ok Normal can only be + -
  if (A.Normal!=Normal && A.Normal!= -Normal)
    return 0;
  // Now centre 
  if (Centre==A.Centre) return 1;
  // Centres can be displaced along the normal
  Geometry::Vec3D X=Centre-A.Centre;
  X.makeUnit();
  const double D=fabs(X.dotProd(Normal));
  return (fabs(D-1.0)<Geometry::zeroTol) ? 1 : 0;
}

int
Cylinder::setSurface(const std::string& Pstr)
  /*! 
    Processes a standard MCNPX cone string    
    Recall that cones can only be specified on an axis
     Valid input is: 
     - c/x cen_y cen_z radius 
     - cx radius 
     \param Pstr :: Input string
    \retval : 0 on success
    \retval : -1 :: Description (Hard fail)
    \retval : -2 :: Axis (Hard fail)
    \retval : -3 :: Centre (Hard fail)
    \retval : -4 :: Radius (Hard fail)
    \retval : -101 :: Radius (Hard fail)
  */
{
  ELog::RegMethod RegA("Cylinder","setSurface");

  enum { errDesc=-1, errAxis=-2,
	 errCent=-3, errRadius=-4};

  std::string Line=Pstr;
  std::string item;
  processSetHead(Line);
    
  if (!StrFunc::section(Line,item) || 
      tolower(item[0])!='c' || item.length()<2 || 
      item.length()>3)
    return errDesc;

  // Cylinders on X/Y/Z axis
  const size_t itemPt((item[1]=='/' && item.length()==3) ? 2 : 1);
  const size_t ptype=static_cast<size_t>(tolower(item[itemPt])-'x');
  if (ptype>=3)
    return errAxis;
  double norm[3]={0.0,0.0,0.0};
  double cent[3]={0.0,0.0,0.0};
  norm[ptype]=1.0;

  if (itemPt!=1)
    {
      // get the other two coordinates
      size_t index((!ptype) ? 1 : 0);
      while(index<3 &&  StrFunc::section(Line,cent[index]))
	{
	  index++;
	  if (index==ptype)
	    index++;
	}
      if (index!=3)
	return errCent;
    }
  // Now get radius
  double R;
  if (!StrFunc::section(Line,R))
    return errRadius;
  if (R<=0.0)
    return -100+errRadius;

  Centre=Geometry::Vec3D(cent);
  Normal=Geometry::Vec3D(norm);
  Radius=R;
  Nvec=static_cast<int>(ptype)+1;
  setBaseEqn();
  return 0;
} 

int
Cylinder::setCylinder(const Geometry::Vec3D& Cpoint,
		      const Geometry::Vec3D& Axis,
		      const double R) 
  /*!
    Given a point and axid direction set the cylinder
    \param Cpoint :: Centre point
    \param Axis :: Axis direction
    \param R :: radius of cylinder
    \retval 0 :: success / -ve on failure
  */
{
  if (R<=0)
    return -1;
  
  Centre=Cpoint;
  Normal=Axis.unit();
  setNvec();
  Radius=R;
  setBaseEqn();
  return 0;
}

int 
Cylinder::side(const Geometry::Vec3D& Pt) const 
  /*!
    Calculate if the point PT within the middle
    of the cylinder 
    \param Pt :: Point to check
    \retval -1 :: within cylinder 
    \retval 1 :: outside the cylinder
    \retval 0 :: on the surface 
  */
{
  if (Nvec)      // Nvec =1-3 (point to exclude == Nvec-1)
    {
      double x=Pt[Nvec % 3]-Centre[Nvec % 3];
      x*=x;
      double y=Pt[(Nvec+1) % 3]-Centre[(Nvec+1) % 3];;
      y*=y;
      const double displace=x+y-Radius*Radius;
      if (fabs(displace)<Geometry::parallelTol)
	return 0;
      return (displace>0.0) ? 1 : -1;
    }
  return Quadratic::side(Pt);
}

int 
Cylinder::onSurface(const Geometry::Vec3D& Pt) const 
  /*!
    Calculate if the point PT on the cylinder 
    \param Pt :: Geometry::Vec3D to test
    \retval 1 :: on the surface 
    \retval 0 :: not on the surface
  */
{
  ELog::RegMethod RegA("Cylinder","onSurface");
  if (Nvec)      // Nvec =1-3 (point to exclude == Nvec-1)
    {
      double x=Pt[Nvec % 3]-Centre[Nvec % 3];
      x*=x;
      double y=Pt[(Nvec+1) % 3]-Centre[(Nvec+1) % 3];;
      y*=y;
      return (fabs((x+y)-Radius*Radius)>
	      sqrt(2.0*Radius*Geometry::zeroTol)) ? 0 : 1;
    }
  return Quadratic::onSurface(Pt);
}

void
Cylinder::setNvec()
  /*! 
     Find if the normal vector allows it to be a special
     type of cylinder on the x,y or z axis.
     \return 1,2,3 :: corresponding to a x,y,z alignment
  */
{
  Nvec=0;
  // Note that Normal is a unit vector
  for(int i=0;i<3;i++)
    if (fabs(Normal[i])>(1.0-Geometry::zeroTol))  
      {
	Nvec=i+1;
	return;
      }
  return;
}

void
Cylinder::rotate(const Geometry::Matrix<double>& MA)
/*!
  Apply a rotation to the cylinder and re-check the
  status of the main axis.
  \param MA :: Rotation Matrix (not inverted)
*/
{
  Centre.rotate(MA);
  Normal.rotate(MA);
  Normal.makeUnit();
  setNvec();
  Quadratic::rotate(MA);
  return;
}

void
Cylinder::rotate(const Geometry::Quaternion& QA)
  /*!
    Apply a rotation to the cylinder and re-check the
    status of the main axis.
    \param QA :: Quaternion to rotate by
  */
{
  QA.rotate(Centre);
  QA.rotate(Normal);
  Normal.makeUnit();
  setNvec();
  Quadratic::rotate(QA);
  return;
}

void 
Cylinder::displace(const Geometry::Vec3D& Pt)
  /*!
    Apply a displacement Pt 
    \param Pt :: Displacement to add to the centre
  */ 
{
  Centre+=Pt;
  if (Nvec)
    Centre[Nvec-1]=0;
  Quadratic::displace(Pt);
  return;
}

void
Cylinder::setCentre(const Geometry::Vec3D& A)
  /*!
    Sets the centre Geometry::Vec3D
    \param A :: centre point 
  */
{
  Centre=A;
  setBaseEqn();
  return;
}

void
Cylinder::setNorm(const Geometry::Vec3D& A)
  /*! 
    Sets the centre line unit vector 
    A does not need to be a unit vector
    \param A :: Vector along the centre line 
  */
{
  Normal=A;
  Normal.makeUnit();
  setBaseEqn();
  return;
}

void
Cylinder::setRadius(const double R)
  /*! 
    Sets the radius
    \param R :: New radius (forced +ve)
  */
{
  Radius=fabs(R);
  setBaseEqn();
  return;
}

void
Cylinder::setBaseEqn()
  /*!
    Sets an equation of type (cylinder)
    \f[ Ax^2+By^2+Cz^2+Dxy+Exz+Fyz+Gx+Hy+Jz+K=0 \f]
  */
{
  const double CdotN(Centre.dotProd(Normal));
  BaseEqn[0]= 1.0-Normal[0]*Normal[0];     // A x^2
  BaseEqn[1]= 1.0-Normal[1]*Normal[1];     // B y^2
  BaseEqn[2]= 1.0-Normal[2]*Normal[2];     // C z^2 
  BaseEqn[3]= -2.0*Normal[0]*Normal[1];     // D xy
  BaseEqn[4]= -2.0*Normal[0]*Normal[2];     // E xz 
  BaseEqn[5]= -2.0*Normal[1]*Normal[2];     // F yz 
  BaseEqn[6]= 2.0*(Normal[0]*CdotN-Centre[0]);  // G x
  BaseEqn[7]= 2.0*(Normal[1]*CdotN-Centre[1]);  // H y
  BaseEqn[8]= 2.0*(Normal[2]*CdotN-Centre[2]);  // J z
  BaseEqn[9]= Centre.dotProd(Centre)-CdotN*CdotN -Radius*Radius;  // K const
  return;
}

void
Cylinder::mirror(const Geometry::Plane& MP)
  /*!
    Apply a mirror plane to the cylinder
    This involves moving the centre and the 
    axis 
    \param MP :: Mirror point
   */
{
  MP.mirrorAxis(Normal);
  MP.mirrorPt(Centre);
  setBaseEqn();
  return;
}

double
Cylinder::distance(const Geometry::Vec3D& A) const
  /*!
    Calculates the distance of point A from 
    the surface of the  cylinder.
    \param A :: Point to calculate distance from
    \return :: +ve distance to the surface.
  */
{
  // First find the normal going to the point
  const Geometry::Vec3D Amov=A-Centre;
  double lambda=Amov.dotProd(Normal);
  const Geometry::Vec3D Ccut= Normal*lambda;
  // The distance is from the centre line to the 
  return  fabs(Ccut.Distance(Amov)-Radius);
}

void
Cylinder::write(std::ostream& OX) const
  /*! 
    Write out the cylinder for MCNPX
    \param OX :: output stream
  */
{
  //               -3 -2 -1 0 1 2 3        
  const char Tailends[]="zyx xyz";
  const int Ndir=Normal.masterDir(Geometry::zeroTol);
  if (Ndir==0)
    {
      // general surface
      Quadratic::write(OX);
      return;
    }
  
  const int Cdir=Centre.masterDir(Geometry::zeroTol);
  std::ostringstream cx;

  writeHeader(cx);
  cx.precision(Geometry::Nprecision);
  // Name and transform 
   
  if (Cdir*Cdir==Ndir*Ndir || Centre.nullVector(Geometry::zeroTol))
    {
      cx<<"c";
      cx<< Tailends[Ndir+3]<<" ";          // set x,y,z based on Ndir
      cx<< Radius;
    }
  else
    {
      cx<<"c/";
      cx<< Tailends[Ndir+3]<<" ";          // set x,y,z based on Ndir

      if(Ndir==1 || Ndir==-1)
	cx<< Centre[1] << " " <<Centre[2] << " ";
      else if(Ndir==2 || Ndir==-2)
	cx<< Centre[0] << " " <<Centre[2] << " ";
      else
	cx<< Centre[0] << " " <<Centre[1] << " ";
      
      cx<< Radius;
    }

  StrFunc::writeMCNPX(cx.str(),OX);
  return;
}

void
Cylinder::writeFLUKA(std::ostream& OX) const
  /*! 
    Write out the cylinder for MCNPX
    \param OX :: output stream
  */
{
  ELog::RegMethod RegA("Cylinder","writeFLUKA");

  masterWrite& MW=masterWrite::Instance();
  const int Ndir=Normal.masterDir(Geometry::zeroTol);
  
  if (Ndir==0)
    {
      // general surface
      Quadratic::writeFLUKA(OX);
      return;
    }

  std::ostringstream cx;

  cx.precision(Geometry::Nprecision);  
  if (Ndir==-1 || Ndir==1)
    {
      cx<<"XCC s"<<getName()<<" " 
	<<MW.Num(Centre[1])<<" "
	<<MW.Num(Centre[2])<<" "
	<<MW.Num(Radius);
    }
  else if (Ndir==-2 || Ndir==2)
    {
      // note the reversed order -- see sec 8.2.4.12 of the fluka manual.
      cx<<"YCC s"<<getName()<<" "
	<<MW.Num(Centre[2])<<" "
	<<MW.Num(Centre[0])<<" "
	<<MW.Num(Radius);
    }
  else if (Ndir==-3 || Ndir==3)
    {
      cx<<"ZCC s"<<getName()<<" "
	<<MW.Num(Centre[0])<<" "
	<<MW.Num(Centre[1])<<" "
	<<MW.Num(Radius);
    }
  StrFunc::writeMCNPX(cx.str(),OX);
  return;
}

  
void
Cylinder::print() const
 /*!
   Debug routine to print out basic information 
 */
{
  Quadratic::print();
  std::cout<<"Axis =="<<Normal<<" ";
  std::cout<<"Centre == "<<Centre<<" ";
  std::cout<<"Radius == "<<Radius<<std::endl;
  return;
}


}   // NAMESPACE Geometry

