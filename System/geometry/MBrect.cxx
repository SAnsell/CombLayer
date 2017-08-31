/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geometry/MBrect.cxx
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
#include <set>
#include <map>
#include <stack>
#include <string>
#include <algorithm>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "support.h"
#include "mathSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Line.h"
#include "Triple.h"
#include "MBrect.h"

namespace Geometry
{
  
MBrect::MBrect() : Surface(),
		   Corner(0,0,0)
  /*!
    Constructor: sets box at 0,0,0 to 1,1,1
  */
{
  for(int i=0;i<3;i++)
    LVec[i][i]=1.0;
  makeSides();
}

MBrect::MBrect(const MBrect& A) : Surface(A),
				  Corner(A.Corner),Sides(A.Sides)
  /*!
    Copy Constructor
    \param A :: MBrect to copy
  */
{
  for(int i=0;i<3;i++)
    LVec[i]=A.LVec[i];
}

MBrect*
MBrect::clone() const
  /*!
    Makes a clone (implicit virtual copy constructor) 
    \return new(this)
  */
{
  return new MBrect(*this);
}

MBrect&
MBrect::operator=(const MBrect& A) 
  /*!
    Assignment operator
    \param A :: MBrect to copy
    \return *this
  */
{
  if (&A!=this)
    {
      this->Surface::operator=(A);
      Corner=A.Corner;
      for(int i=0;i<3;i++)
	LVec[i]=A.LVec[i];
      Sides=A.Sides;
    }
  return *this;
}

MBrect::~MBrect()
  /*!
    Destructor
  */
{}

bool
MBrect::operator==(const MBrect& A) const
  /*!
    Equality operator: 
    Care is taken to allow the corners to be defined in 
    any order
    \param A :: Object to compare
    \return true: if all points are within PTolerance
  */
{
  if (this==&A) return 1;
  // Basic case
  int cnt(0);
  int flag[]={1,1,1};
  if (Corner.Distance(A.Corner)<Geometry::zeroTol)    
    {
      for(int i=0;i<3;i++)
        {
	  for(int j=0;j<3;j++)
	    if (flag[j] &&
		LVec[i].Distance(A.LVec[j])<Geometry::zeroTol)
	      {
		cnt++;
		flag[j]=0;
		break;
	      }
	}
      return (cnt==3) ? 1 : 0;
    }
  // Maybe corners are diametrically opposite:
  if (Corner.Distance(A.Corner+A.LVec[0]+A.LVec[1]+A.LVec[2])<Geometry::zeroTol)
    {
      for(int i=0;i<3;i++)
        {
	  for(int j=0;j<3;j++)
	    if (flag[j] &&
		LVec[i].Distance(-A.LVec[j])<Geometry::zeroTol)
	      {
		cnt++;
		flag[j]=0;
		break;
	      }
	}
      return (cnt==3) ? 1 : 0;
    }
  return 0;
}

bool
MBrect::operator!=(const MBrect& A) const
  /*!
    Determine if two plane are not equal (within tolerance).
    \param A :: Plane to compare
    \return A!=This
  */
{
  return !(this->operator==(A));
}

const Geometry::Vec3D&
MBrect::getLVec(const size_t Index) const
  /*!
    Access the Edge vectors (LVec)
    \param Index :: Index item
    \return LVec[index]
   */
{
  if (Index>2)
    throw ColErr::IndexError<size_t>(Index,3,"MBrect::getLVec");
  return LVec[Index];
}

const Geometry::Plane&
MBrect::getPlane(const size_t Index) const
  /*!
    Access the Plane vectors (Sides)
    \param Index :: Index item
    \return Sides[index]
   */
{
  if (Index>=Sides.size())
    throw ColErr::IndexError<size_t>(Index,Sides.size(),"MBrect::getSides");
  return Sides[Index];
}

void
MBrect::setCorner(const Geometry::Vec3D& A)
  /*!
    Sets a corner and allows the LVec to be unchanged
    This allows the rectangle to be moved
    \param A :: New corner position
  */
{
  Corner=A;
  makeSides();
  return;
}

int
MBrect::setMBrect(const Geometry::Vec3D& A,const Geometry::Vec3D& B) 
  /*!
    Given corner and the opposite corner set the
    box. Assumes the sides are along the axis directions. 
    \param A :: Corner to add
    \param B :: Corner to add
    \retval 0 :: success
  */
{
  Corner=A;
  Geometry::Vec3D X=B-A;
  for(int i=0;i<3;i++)
    {
      Geometry::Vec3D Direction;
      Direction[i]=X[i];
      LVec[i]=Direction;
    }
  makeSides();
  return 0;
}

int
MBrect::setMBrect(const Geometry::Vec3D& A,const Geometry::Vec3D& L1,
		  const Geometry::Vec3D& L2,const Geometry::Vec3D& L3) 
  /*!
    Given corner and the direction vectors (with length)
    create the parapiped.
    \param A :: Corner to add
    \param L1 :: Direction Vector
    \param L2 :: Direction Vector
    \param L3 :: Direction Vector
    \retval 0 :: success
    \retval -1 :: vectors are in a plane
  */
{
  // first check that L1,L2,L3 are in a plane
  if (fabs(L1.dotProd(L2*L3))<Geometry::zeroTol)
    {
      ELog::EM.Estream()<<"MBrect::setMBrect NON-3D box:"<<
	L1<<" : "<<L2<<" : "<<L3;
      ELog::EM.warning();
      ELog::EM.warning("  Box not being changed");
      return -1;
    }

  Corner=A;
  LVec[0]=L1;
  LVec[1]=L2;
  LVec[2]=L3;
  makeSides();
  return 0;
}

int
MBrect::setSurface(const std::string& Pstr)
  /*! 
     Processes a standard MCNPX plane string    
     - rpp : Corner : Corner 
     \param Pstr :: String to make into a rectangle
     \return 0 on success, -ve of failure
  */
{
  std::string Line=Pstr;

  int nx;
  if (StrFunc::section(Line,nx))
    setName(nx);                       // Assign the name

  std::string Btype;
  
  int boxType(0);
  if (!StrFunc::section(Line,Btype))
    return -1;

  if (Btype=="rpp" || Btype=="RPP")
    boxType=6;
  else if (Btype=="box" || Btype=="BOX")
    boxType=12;
  
  if (!boxType)   // NOT A BOX
    return -2;

  double surf[12];    // MUST be for biggest number
  int ii;
  for(ii=0;ii<boxType && StrFunc::section(Line,surf[ii]);ii++) ;
  if (ii!=boxType)
    return -3;       //Failed somewhere
        
  if (boxType==6)
    {
      // Note: MCNPX is [0,2,4] : [1,3,5]
      Geometry::Vec3D A=Geometry::Vec3D(surf[0],surf[2],surf[4]);
      Geometry::Vec3D B=Geometry::Vec3D(surf[1],surf[3],surf[5]);
      Corner=A;
      B-=A;
      for(int i=0;i<3;i++)
        {
	  Geometry::Vec3D Direct;     // zero 0,0,0 vector
	  Direct[i]=B[i];
	  LVec[i]=Direct;
	}
    }
  else          // BOX:
    {
      Corner=Geometry::Vec3D(surf[0],surf[1],surf[2]);
      for(int i=0;i<3;i++)
	LVec[i]=Geometry::Vec3D(surf[3*i+3],surf[3*i+4],surf[3*i+5]);
    }
  makeSides();
  return 0;
}

void
MBrect::rotate(const Geometry::Matrix<double>& MA) 
  /*!
    Rotate the plane about the origin by MA 
    \param MA direct rotation matrix (3x3)
  */
{
  Corner.rotate(MA);
  for(int i=0;i<3;i++)
    LVec[i].rotate(MA);
  // Calculate later
  makeSides();
  return;
}

void
MBrect::displace(const Geometry::Vec3D& Sp) 
  /*!
    Displace the plane by Point Sp.  
    i.e. r+sp now on the rectable
    \param Sp :: point value of displacement
  */
{
  Corner+=Sp;
  for(int i=0;i<3;i++)
    LVec[i]+=Sp;
  // Calculate later
  makeSides();
  return;
}

void
MBrect::mirror(const Geometry::Plane& MP)
  /*!
    Apply a mirror plane to the MBrect
    This involves moving the centre and the 
    axis 
    \param MP :: Mirror point
   */
{
  MP.mirrorPt(Corner);
  for(size_t i=0;i<3;i++)
    MP.mirrorPt(LVec[i]);

  for(Geometry::Plane& sc : Sides)
    sc.mirror(MP);

  return;
}


double
MBrect::distance(const Geometry::Vec3D& Pt) const
  /*!
    Determine the distance of point Pt from the planes 
    returns a value relative to the normal
    \param Pt :: point to get distance from 
    \returns distance from the surface to the point
  */
{
  Geometry::Vec3D Base;
  // Express this 
  Base=Pt-Corner;
  Base.reBase(LVec[0],LVec[1],LVec[2]);
  // Condition 1: Points inside: (slightly extended box)
  int inside(0);
  Vec3D Apt(Corner);
  Vec3D Bpt(Corner);  // Line points if needed
  for(int i=0;i<3;i++)
    {
      if (Base[i]>1.0+Geometry::parallelTol)       // Top equivilent:
        {    
	  Apt+=LVec[i];
	  Bpt+=LVec[i];
	}
      else if (Base[i]<-Geometry::parallelTol)     // Lower
        {
	  Bpt+=LVec[i];
	}
      else 
	inside++;
    }
  
  if (inside==3)    // Inside box : hence 6 plane check:
    {
      double DOut(-1.0);
      for(size_t i=0;i<6;i++)
        {
	  const Plane& P(Sides[i]);
	  const  double D=P.getDistance();
	  const Vec3D& N=P.getNormal();
	  Geometry::Vec3D IP= Pt+N*(D-Pt.dotProd(N));
	  if (this->side(IP)>=0)
	    {
	      const double DV(Pt.Distance(IP));
	      if (DOut<0 || DV<DOut)
		DOut=DV;
	    }
	}
      return DOut;
    }

  if (inside==0)       // Special case for corners: 
    {
      // Note:  Apt and Bpt should be the same:
      return Apt.Distance(Pt);
    }

  // Determine the Line:
  Line LX(Apt,Bpt);
  return LX.distance(Pt);
}

void
MBrect::makeSides()
  /*!
    Build the edge surface planes
   */
{
  
  Sides.resize(6);
  const Vec3D Cextra=Corner+LVec[0]+LVec[1]+LVec[2];
  for(size_t i=0;i<3;i++)
    {
      // Note " this is not normalised but Plane does so
      Vec3D Norm(LVec[(i+1) % 3]*LVec[(i+2) % 3]);
      Sides[i].setPlane(Corner,Norm);
      Norm*= -1.0;
      Sides[i+3].setPlane(Cextra,Norm);
    }
  return;
}

int
MBrect::side(const Geometry::Vec3D& Pt) const
  /*!
    Calcualates the side that the point is on a side
    \param Pt :: test point
    \retval 1 :: Inside the box
    \retval -1 :: Outside the box
    \retval 0 :: Pt is on the plane itself (within tolerence) 
  */
{
  int retVal(0);
  std::vector<Plane>::const_iterator vc;
  for(vc=Sides.begin();vc!=Sides.end();vc++)
    {
      const int flag(vc->side(Pt));
      if (flag<0)  // failed
	return -1;
      retVal+=flag;
    }
  return (retVal==static_cast<int>(Sides.size())) ? 1 : 0;
}

Geometry::Vec3D 
MBrect::surfaceNormal(const Geometry::Vec3D& Pt) const
  /*!
    Calculate the normal at the point
    \param Pt :: Point to use
    \return normal at point
  */
{
  return Pt;
}

int
MBrect::onSurface(const Geometry::Vec3D& A) const
  /*! 
     Calcuate the side that the point is on
     and returns success if it is on the surface.
     - Uses Geometry::zeroTol to determine the closeness
     \retval 1 if on the surface 
     \retval 0 if off the surface 
     
  */
{
  return (side(A)!=0) ? 0 : 1;
}

Triple<Vec3D>
MBrect::calcMainPlane() const
  /*!
    Calculate the main plane (ie avoiding the short direction)
    The last part of the triple is the highest
    Z component vector.
    \return Centre : Lx Vec : Lz Vec 
  */
{
  // First calculate the minimiun LVector 
  size_t index=0;
  double Dist=LVec[0].abs();
  Vec3D sumVec(LVec[0]);
  for(size_t i=1;i<3;i++)
    {
      sumVec+=LVec[i];
      const double A=LVec[i].abs();
      if (A<Dist)
        {
	  Dist=A;
	  index=i;
	}
    }
  sumVec/=2.0;
  // Calculate the vector with the most Z:
  size_t AI,BI;
  if ( std::abs(LVec[(index+1) % 3].Z()) >
       std::abs(LVec[(index+2) % 2].Z()) )
    {
      AI=(index+2) % 3;
      BI=(index+1) % 3;
    }
  else
    {
      AI=(index+1) % 3;
      BI=(index+2) % 3;
    }
 
  // (a) Take the corner (Low point) and 
  //     add half the small vector 
  // (b) Add the other two vectors to point (a)
  return Triple<Vec3D>(Corner+sumVec,LVec[AI],LVec[BI]);
}

void 
MBrect::print() const
  /*!
    Prints out the surface info and
    the MBrect info.
  */
{
  Surface::print();
  std::cout<<"Corner : "<<Corner;
  for(int i=0;i<3;i++)
    std::cout<<" :: "<<LVec[i];
  return;
}

int
MBrect::boxType() const
  /*! 
    Find if it is a rcc box:
    \retval 1 :: rcc box
    \retval 0 :: general box
  */
{
  for(size_t i=0;i<3;i++)
    if (!LVec[i].masterDir(Geometry::zeroTol))
      return 0;
  
  return 1;
}

void 
MBrect::write(std::ostream& OX) const
  /*! 
    Object of write is to output a MCNPX plane info 
    \param OX :: Output stream (required for multiple std::endl)  
    \todo (Needs precision) 
  */
{
  std::ostringstream cx;
  Surface::writeHeader(cx);
  cx.precision(Geometry::Nprecision);
  const int btype=boxType();
  
  if (!btype)
    {
      cx<<"box "<<Corner[0]<<" "
	<<Corner[1]<<" "
	<<Corner[2]<<" ";
      for(size_t i=0;i<3;i++)
	for(size_t j=0;j<3;j++)
	  cx<<LVec[i][j]<<" ";
    }
  else
    {
      Geometry::Vec3D OCorn(Corner+LVec[0]+LVec[1]+LVec[2]);
      cx<<"rpp ";
      for(int i=0;i<3;i++)
	cx<<Corner[i]<<" "<<OCorn[i]<<" ";
    }
  StrFunc::writeMCNPX(cx.str(),OX);
  return;
}

void
MBrect::writeFLUKA(std::ostream&) const
  /*!
    Write out the cone class in an mcnpx
    format.
    \param  :: Output Stream (required for multiple std::endl)
  */
{
  ELog::EM<<"Unsupported output"<<ELog::endErr;
  return;
}

void
MBrect::writePOVRay(std::ostream&) const
  /*!
    Write out the cone class in a POV-Ray
    format.
    \param  :: Output Stream (required for multiple std::endl)
  */
{
  ELog::EM<<"MBrect::writePOVRay: Unsupported output"<<ELog::endErr;
  return;
}

}  // NAMESPACE Geometry
