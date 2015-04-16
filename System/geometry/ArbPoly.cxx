/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   geometry/ArbPoly.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <boost/bind.hpp>
#include <boost/multi_array.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "support.h"
#include "mathSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Transform.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Line.h"
#include "ArbPoly.h"

namespace Geometry
{
 
ArbPoly::ArbPoly() : Surface(),
		     nSurface(0)
 /*!
   Constructor sets void object
 */	     
{}

ArbPoly::ArbPoly(const ArbPoly& A) : Surface(A),
   nSurface(A.nSurface),CVec(A.CVec),
   CIndex(A.CIndex),Sides(A.Sides)
  /*!
    Copy Constructor
    \param A :: ArbPoly to copy
  */
{}


ArbPoly*
ArbPoly::clone() const
  /*!
    Makes a clone (implicit virtual copy constructor) 
    \return new(this)
  */
{
  return new ArbPoly(*this);
}

ArbPoly&
ArbPoly::operator=(const ArbPoly& A) 
  /*!
    Assignment operator
    \param A :: ArbPoly to copy
    \return *this
  */
{
  if (&A!=this)
    {
      this->Surface::operator=(A);
      nSurface=A.nSurface;
      CVec=A.CVec;
      CIndex=A.CIndex;
      Sides=A.Sides;
    }
  return *this;
}

ArbPoly::~ArbPoly()
  /*!
    Destructor
  */
{}

bool
ArbPoly::operator==(const ArbPoly& A) const
  /*!
    Determine if two plane are equal (within tolerance).
    \param A :: ArbPoly to compare
    \return A==This
    \todo Improve to deal with different surface settings
  */
{
  if (&A==this) return 1;
  if (CVec.size()!=A.CVec.size())
    return 0;
  for(size_t i=0;i!=CVec.size();i++)
    if (CVec[i].Distance(A.CVec[i])>Geometry::zeroTol)
      return 0;
  return 1;
}

bool
ArbPoly::operator!=(const ArbPoly& A) const
  /*!
    Determine if two ArbPolys are not equal (within tolerance).
    \param A :: ArbPoly to compare
    \return A!=This
  */
{
  return !(this->operator==(A));
}



const Geometry::Vec3D&
ArbPoly::getCVec(const size_t Index) const
  /*!
    Access the Edge vectors (CVec)
    \param Index :: Index item
    \return CVec[index]
   */
{
  if (Index>=CVec.size())
    throw ColErr::IndexError<size_t>(Index,CVec.size(),"ArbPoly::getCVec");
  return CVec[Index];
}

const std::vector<size_t>&
ArbPoly::getCIndex(const size_t Index) const
  /*!
    Access the Index set
    \param Index :: Index item
    \return CVec[index]
   */
{
  if (Index>=CIndex.size())
    throw ColErr::IndexError<size_t>(Index,CIndex.size(),
				     "ArbPoly::getCIndex");
  return CIndex[Index];
}

const Geometry::Plane&
ArbPoly::getPlane(const size_t Index) const
  /*!
    Access the Plane vectors (Sides)
    \param Index :: Index item
    \return Sides[index]
   */
{
  if (Index>=Sides.size())
    throw ColErr::IndexError<size_t>(Index,Sides.size(),"ArbPoly::getPlanes");
  return Sides[Index];
}

int
ArbPoly::setSurface(const std::string& Pstr)
  /*! 
    Processes a standard MCNPX arb string
    This expects:

    ARB (Vec3D) x 8 + Surface Index List (zero terminated)
    
    Each surface Index item is a sequientual number of
    line points 0-M
    
    \param Pstr :: String to make into a polynomial
    \return 0 on success, -ve of failure
  */
{
  ELog::RegMethod RegA("ArbPoly","setSurface");

  const size_t maxSurfaces(100);
  std::string Line=this->stripID(Pstr);

  int nx;
  if (StrFunc::section(Line,nx))
    setName(nx);                       // Assign the name

  std::string Btype;
  if (!StrFunc::section(Line,Btype))
    return -1;

  std::string Key=Btype.substr(0,3);
  if (Key!="arb" && Key!="ARB")       // Wrong type
    return -3;

  size_t NC(8);                          // Number of expected corners ==8 
  StrFunc::convert(Btype.substr(3),NC);
  if (NC<=3 | NC>maxSurfaces)
    return -4;

  std::vector<Geometry::Vec3D> Corners(NC);
  for(size_t i=0;i<NC;i++)
    {
      if (!StrFunc::section(Line,Corners[i]))
	return -5;
    }
  
  // The face chain is an arbitary list 1-9, a-z (for corner
  // 10-36)
  std::vector<std::vector<size_t> > CI;
  int flag;
  do 
    {
      flag=StrFunc::section(Line,Key);
      if (flag)
        {
	  std::vector<size_t> subCI;
	  for(size_t i=0;i<Key.length();i++)
	    {
	      const size_t KN=(isdigit(Key[i])) ?  
		static_cast<size_t>(Key[i]-'0') :
		static_cast<size_t>(Key[i]-'a');
	      if (KN>0 && KN<36)
		subCI.push_back(KN-1);
	      else
		break;
	    }
	  if (subCI.size()<3)
	    flag=0;
	  else
	    CI.push_back(subCI);
	}
    } while(flag);

  if (CI.size()<4)
    return -7;

  nSurface=CI.size();
  CVec=Corners;
  CIndex=CI;
  makeSides();
  return 0;
}

void
ArbPoly::rotate(const Geometry::Matrix<double>& MA) 
  /*!
    Rotate the plane about the origin by MA 
    \param MA direct rotation matrix (3x3)
  */
{
  for_each(CVec.begin(),CVec.end(),
	   boost::bind(&Vec3D::rotate<double>,_1,MA));

  // Calculate later
  makeSides();
  return;
}

void
ArbPoly::mirror(const Geometry::Plane& MP)
  /*!
    Apply a mirror plane to the ArbPoly
    This involves moving the centre and the 
    axis 
    \param MP :: Mirror point
   */
{
  for_each(CVec.begin(),CVec.end(),
	   boost::bind(&Plane::mirrorPt,MP,_1));

  for_each(Sides.begin(),Sides.end(),
	   boost::bind(&Plane::mirror,_1,MP));
  return;
}

void
ArbPoly::displace(const Geometry::Vec3D& Sp) 
  /*!
    Displace the plane by Point Sp.  
    i.e. r+sp now on the rectable
    \param Sp :: point value of displacement
  */
{
  std::vector<Vec3D>::iterator vc;
  for(vc=CVec.begin();vc!=CVec.end();vc++)
    (*vc)+=Sp;
  makeSides();
  return;
}

double
ArbPoly::distance(const Geometry::Vec3D& Pt) const
  /*!
    Determine the distance of point A to the surface
    returns a value relative to the normal
    \param Pt :: point to get distance from 
    \returns singled distance from point
  */
{
  // First calculate the distance to the points:
  double maxDistance(-1.0);
  for(size_t i=0;i<CVec.size();i++)
    {
      const double D=Pt.Distance(CVec[i]);
      if (i==0 || D<maxDistance)
	maxDistance=D;
    }
  const size_t nPoint(CVec.size());
  std::set<size_t> LineUsed;       // Sides [Index + nPoint*Index ] 
  // Loop over each surface 
  for(size_t i=0;i<CIndex.size();i++)
    {
      // Each surface is a set of points:
      const std::vector<size_t>& CV(CIndex[i]);
      const size_t CVS(CV.size());
      for(size_t j=0;j<CVS;j++)
        {
	  const size_t AI(CV[j]);              // index to point A of line 
	  const size_t BI(CV[(j+1) % CVS]);         // index to point B of line 
	  const size_t NV=AI+BI*nPoint;
	  if (LineUsed.find(NV)==LineUsed.end())
	    {
	      // direction of line
	      Geometry::Vec3D directV(CVec[BI]-CVec[AI]);
	      const double DL=directV.makeUnit();
	      const double frac=directV.dotProd(Pt-CVec[AI]);
	      if (frac>0.0 && frac<DL)
	        { 
		  const Vec3D LPoint=CVec[AI]+directV*frac;
		  const double D=Pt.Distance(LPoint);
		  if (D<maxDistance)
		    maxDistance=D;
		}
	      // Now add to set to avoid re-calculating line
	      LineUsed.insert(AI+BI*nPoint);
	      LineUsed.insert(BI+AI*nPoint);
	    }
	}
    }
  // Now determine the Plane : Point closest distance
  // However EACH point must be checked (EXPENSIVE)
  // Hence we keep a list of points in order
  std::vector<DTriple<double,size_t,Vec3D> > InterPoint;
  for(size_t i=0;i<nSurface;i++)
     {
       const Plane& P(Sides[i]);
       const double D=P.getDistance();
       const Vec3D& N=P.getNormal();
       Geometry::Vec3D IP= Pt+N*(D-Pt.dotProd(N));
       const double DX=Pt.Distance(IP);
       if (DX<maxDistance)
	 InterPoint.push_back(DTriple<double,size_t,Vec3D>(DX,i,IP));
     }
  sort(InterPoint.begin(),InterPoint.end());
  std::vector<DTriple<double,size_t,Vec3D> >::const_iterator ac;
  for(ac=InterPoint.begin();ac!=InterPoint.end();ac++)
    if (inLoop(ac->third,ac->second))
      return ac->first;
  // All failed 
  return maxDistance;
}

int
ArbPoly::inLoop(const Vec3D& Pt,const size_t Index) const
  /*!
    Given a point on a surface find if that point
    is in the loop.

    This is done by summing the angles of all the segments
    Note: that this works for non-convex objects. The
    normal sense does not.
    
    \param Pt :: Point to test
    \param Index :: surface index
    \retval 0  :: false
    \retval 1  :: inside loop
    \retval 2  :: on line (within Tolerance)
    \retval 3  :: at a corner
  */
{
  if (Index>=CIndex.size())
    throw ColErr::IndexError<size_t>(Index,CIndex.size(),"ArbPoly::inLoop");
  const std::vector<size_t>& CRef(CIndex[Index]);
  const size_t CRS(CRef.size());
  Vec3D AL;
  Vec3D BL=CVec[CRef.back()] - Pt;
  if (BL.makeUnit()<Geometry::zeroTol)        // on corner
    return 3;

  double dotSum(0.0);
  int line(0);
  for(size_t i=0;i<CRS;i++)
    {
      AL=BL;
      BL=CVec[CRef[i]] - Pt;
      if (BL.makeUnit()<Geometry::zeroTol)        // on corner
	return 3;
      // Now calculate Angle 
      const double dP=AL.dotProd(BL);
      const double angle=acos(dP);
      if (fabs(angle)<Geometry::zeroTol || fabs(angle-M_PI)<Geometry::zeroTol)
	line++;
      else
	dotSum+=angle;
    }
  
  int res;
  if (line && fabs(dotSum-M_PI)<Geometry::zeroTol*nSurface)
    res=2;
  else if (fabs(dotSum-2.0*M_PI)<Geometry::zeroTol*nSurface)
    res=1;
  else 
    res=0;
  return res;
}

void
ArbPoly::makeSides()
  /*!
    Build the edge surface planes
    - Corners are describe clockwize
    - Normals are positiv to the clockwize face
  */
{
  ELog::RegMethod RegA("ArbPoly","makeSides");
  Sides.resize(nSurface);
  for(size_t i=0;i<nSurface;i++)
    {
      const std::vector<size_t>& IRef(CIndex[i]);
      const Vec3D AD=CVec[IRef[1]]-CVec[IRef[0]];
      const Vec3D BD=CVec[IRef[2]]-CVec[IRef[1]];
      const Vec3D Norm(BD*AD);
      // Note : this is not normalised but Plane does so
      Sides[i].setPlane(CVec[IRef[0]],Norm);
    }
  return;
}

int
ArbPoly::side(const Geometry::Vec3D& Pt) const
  /*!
    Calculates if the point is inside the object :
    on the side or outside.
    
    Algorithm : 
       - Determine a line from point 
       and the intersections on all of the planes.
       - calcuate sum of the progressive angles APB+BPC+ ...
       -- It is on the surface if angle == 2pi
       -- It is outside if angle =0
   
       - The closest valid surface in the +ve and -ve directions
         must have the correct sense vs normal 

       -- Special case if P == Corner  [Return 0]
       -- Special case if P == Surface [if in loop return 0 or ignore]

       
    \param Pt :: test point
    \retval 1 :: Inside the box
    \retval -1 :: Outside the box
    \retval 0 :: Pt is on the plane itself (within tolerence) 
  */
{
  Vec3D unitVec(1.0,0,0);
  std::vector<Plane>::const_iterator vc;

  // Note: Because Line Pt + lambda (1,0,0) 
  //       allows a quick line/plane intersect
  // First find if we are in/out of the object:
  DTriple<int,int,double> negU(-1,0,0);   // side : surface : distance
  DTriple<int,int,double> posU(-1,0,0);
  for(size_t i=0;i<nSurface;i++)
    {
      const Plane& Pln(Sides[i]);
      // Intersect with line [ from Line.cxx + optimization ]
      const double OdotN=Pt.dotProd(Pln.getNormal());  
      const double DdotN=Pln.getNormal()[0];   // just x-coordinated
      // SURFACE NOT PARALLEL:
      if (fabs(DdotN)>Geometry::parallelTol)         // surface not parallel
        {
	  const double u=(Pln.getDistance()-OdotN)/DdotN;
	  if (fabs(u)<Geometry::zeroTol)   // If we are in a valid surface:
	    {
	      if (inLoop(Pt,i))
		return 0;
	    }
	  // Now determine if this should be considered
	  const Vec3D PlanePoint(Pt+unitVec*u);
	  if (u<0.0 && (negU.first<0 || u>negU.second))
	    {

	      const int IL=inLoop(PlanePoint,i);
	      if (IL)
	        {
		  negU.first=static_cast<int>(i);
		  negU.second=(IL==1) ? 1 : 0;
		  negU.third=u;
		}	      
	    }

	  if (u>0.0 && (posU.first<0 || posU.second>u))
	    {
	      const int IL=inLoop(PlanePoint,i);
	      if (IL)
	        {
		  posU.first=static_cast<int>(i);
		  posU.second=(IL==1) ? 1 : 0;
		  posU.third=u;
		}	      
	    }
	}
      // SURFACE PARALLEL:
      else
        {
	  if (inLoop(Pt,i))
	    return 0;
	}
    }
  // Now we have valid surfaces:
  if (posU.first<0 || negU.first<0)
    return -1;

  // Now check the direction of the normals:  
  if (Sides[static_cast<size_t>(posU.first)].getNormal()[0]<0.0 &&
      Sides[static_cast<size_t>(negU.first)].getNormal()[0]>0.0)
    {
      return negU.second*posU.second;
    }

  // Ok failed:
  return -1;
}

Geometry::Vec3D 
ArbPoly::surfaceNormal(const Geometry::Vec3D& Pt) const
  /*!
    Calculate the normal at the point. Assumes that if
    on an edge an average of the normals is required.
    \param Pt :: Point to use
    \return normal at point
  */
{
  std::vector<Plane>::const_iterator vc;
  std::vector<Plane>::const_iterator fc;
  
  Geometry::Vec3D Out;
  int found(0);
  double maxDist(0.0);
  for(vc=Sides.begin();vc!=Sides.end();vc++)
    {
      const double D=vc->distance(Pt);
      if (vc==Sides.begin() || maxDist>D)
	{
	  maxDist=D;
	  fc=vc;
	}
      if (D<Geometry::zeroTol)
	{
	  found++;
	  Out+=vc->getNormal();
	}
    }
  return (found) ? Out/found : fc->getNormal();
}

int
ArbPoly::onSurface(const Geometry::Vec3D& A) const
  /*! 
     Calcuate the side that the point is on
     and returns success if it is on the surface.
     - Uses ATolerance to determine the closeness
     \retval 1 if on the surface 
     \retval 0 if off the surface 
     
  */
{
  return (side(A)!=0) ? 0 : 1;
}

void 
ArbPoly::print() const
  /*!
    Prints out the surface info and
    the ArbPoly info.
  */
{
  Surface::print();
  std::cout<<"Corners : "<<std::endl;
  copy(CVec.begin(),CVec.end(),
       std::ostream_iterator<Vec3D>(std::cout,"\n"));
  std::cout<<"Planes: "<<std::endl;
  for(size_t i=0;i<nSurface;i++)
    {
      copy(CIndex[i].begin(),CIndex[i].end(),
	   std::ostream_iterator<int>(std::cout,""));
      if ((i+1) % 3 )
	std::cout<<std::endl;
      else
	std::cout<<"  ";
    }
  return;
}

void 
ArbPoly::write(std::ostream& OX) const
  /*! 
    Object of write is to output a MCNPX plane info 
    \param OX :: Output stream (required for multiple std::endl)  
    \todo Check trailing 0 requirements for MCNPX
  */
{
  std::ostringstream cx;
  Surface::writeHeader(cx);
  cx.precision(Geometry::Nprecision);
  
  cx<<"arb";
  if (CVec.size()!=8)
    {
      if (CVec.size()<9)
	cx<<CVec.size();
      else
	cx<<'a'+static_cast<char>(CVec.size()-10);
    }
  cx<<" ";
  copy(CVec.begin(),CVec.end(),
       std::ostream_iterator<Vec3D>(cx," "));

  for(size_t i=0;i<nSurface;i++)
    {
      const std::vector<size_t>& CI(CIndex[i]);
      std::vector<size_t>::const_iterator vc;
      for(vc=CI.begin();vc!=CI.end();vc++)
	cx<<(*vc+1);
      cx<<" ";
    }

  StrFunc::writeMCNPX(cx.str(),OX);
  return;
}

}  // NAMESPACE Geometry
