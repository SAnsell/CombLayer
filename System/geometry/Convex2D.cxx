/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geometry/Convex2D.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
 * along with this program. If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cmath>
#include <vector>
#include <map>
#include <list>
#include <string>
#include <algorithm>
#include <iterator>
#include <functional>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "SVD.h"
#include "Vec3D.h"
#include "Vert2D.h"
#include "Convex2D.h"

namespace Geometry
{

Convex2D::Convex2D() : normal(1,0,0),
		       distIndex(0)
  /*! 
    Constructor
  */
{}

Convex2D::Convex2D(const std::vector<Geometry::Vec3D>& PVec) :
  normal(1,0,0)
  /*!
    Constructor 
    \param PVec :: Points to add
  */
{
  setPoints(PVec);
}

Convex2D::Convex2D(const Convex2D& A) :
  Pts(A.Pts),VList(A.VList),
  centroid(A.centroid),normal(A.normal)
  /*!
    Copy Constructor 
    \param A :: Convex2D object 
  */
{}

Convex2D&
Convex2D::operator=(const Convex2D& A) 
  /*! 
    Assignment constructor
    \param A :: Convex2D object 
    \return *this
  */
{
  if (this!=&A)
    {
      Pts=A.Pts;
      VList=A.VList;
      centroid=A.centroid;
      normal=A.normal;
    }
  return *this;
}

Convex2D::~Convex2D()
  /*!
    Deletion Method
  */
{}

void 
Convex2D::setPoints(const std::vector<Geometry::Vec3D>& PVec)
  /*!
    Sets the points
    \param PVec :: Points
   */
{  
  ELog::RegMethod RegItem("Convex2D","setPoints");
  Pts=PVec;
  std::vector<Vec3D>::const_iterator vc;
  for(vc=Pts.begin();vc!=Pts.end();vc++)
    VList.push_back(Vert2D(VList.size(),*vc));

  centroid=Geometry::Vec3D(0,0,0);
  normal=Geometry::Vec3D(1,0,0);
  
  return;
}

void 
Convex2D::addPoint(const Geometry::Vec3D& PItem)
  /*!
    Adds a point to the structure
    \param PItem :: Point to add
  */
{  
  ELog::RegMethod RegItem("Convex2D","addPoint");

  Pts.push_back(PItem);
  VList.push_back(Vert2D(VList.size(),PItem));

  return;
}

double
Convex2D::calcArea() const
  /*!  
    Assuming the pane normal and centroids have been     - C
    Uses points enclose This method used the formula of Van Gelder
    "Efficient Computation of Polygon Area and Polyhedron Volume" in
    Graphics Gems V (1995)
    \return area of plane projection
  */
{
  ELog::RegMethod RegA("Convex2D","calcArea");

  const size_t N=VList.size();
  const size_t k=(N % 2) ? 0 : N-1;      // N-1 for even
  const size_t h=(N-1)/2;             
  
  Geometry::Vec3D Cprod;
  const Geometry::Vec3D V0=VList[0].getV();
  for(size_t i=1;i<h;i++)
    {
      Cprod+=(VList[2*i].getV()-V0)*
	(VList[2*i+1].getV()-VList[2*i-1].getV());
    }
  Cprod+=(VList[2*h].getV()-V0)*(VList[k].getV()-VList[2*h-1].getV());

  return fabs(0.5*normal.dotProd(Cprod));
}

double
Convex2D::calcNormal()
  /*!
    Calculates the normal to a set of points in an
    approximate plane
    - Calc centroid
    - Calc M Matrix : Points -centroid 3xN
    - Calc \f$ A = M^TM \f$
    - Calc SVD to get eigenvalues
    - Determine min eigenvalue
    - Calc eigenvector 
    - set the norm
    \return component value
   */
{
  ELog::RegMethod RegItem("Convex2D","calcNormal");
  if (Pts.size()<3)
    {
      ELog::EM<<"Insufficient points to calculate "
	      <<"plane normal"<<ELog::endErr;
      return -1.0;
    }
  centroid=Geometry::Vec3D(0,0,0);

  std::vector<Vec3D>::const_iterator vc;
  for(vc=Pts.begin();vc!=Pts.end();vc++)
    centroid+=*vc;
  centroid/=static_cast<double>(Pts.size());
  
  Matrix<double> M(Pts.size(),3);

  // Construct matrix M 
  for(size_t index=0;index<Pts.size();index++)
    for(size_t i=0;i<3;i++)
      M[index][i]=Pts[index][i]-centroid[i];

  SVD solA;
  solA.setMatrix(M);
  solA.calcDecomp();
  
//  Spow*=Spow;
  
  // Get Normal
  const Matrix<double>& V=solA.getV();
  normal = Geometry::Vec3D(V[0][2],V[1][2],V[2][2]);

  return solA.getS()[2][2];  
}

size_t
Convex2D::calcMaxIndex()
  /*!
    Calculates the maximum point
    based on the distance from the centroid
    \return the index of the maxium point
  */
{
  ELog::RegMethod RegItem("Convex2D","calcMaxIndex");

  distIndex=0;
  double maxDist(0.0);
  for(size_t i=0;i<Pts.size();i++)
    {
      const double D= centroid.Distance(Pts[i]);
      if (D>maxDist)
	{
	  maxDist=D;
	  distIndex=i;
	}
    }
  return distIndex;
}

void
Convex2D::createVertex()
  /*!
    Creates an ordered list of item.
    - calculate the line to the 
  */
{
  ELog::RegMethod RegItem("Convex2D","createVertex");

  if (Pts.size()<=distIndex)
    throw ColErr::IndexError<size_t>(distIndex,Pts.size(),"createVertex");

  // Construct a angle system based on 
  // the line from the centre to the max point
  // in the plane
  VList.clear();
  Geometry::Vec3D Line=(Pts[distIndex]-centroid)*normal;  
  Line.makeUnit();
  for(size_t i=0;i<Pts.size();i++)
    {
      if (i!=distIndex)
        {
	  VList.push_back(Vert2D(i,Pts[i]));
	  VList.back().calcAngle(Pts[distIndex],Line);
	}
    }

  std::sort(VList.begin(),VList.end(),
	    [](const Vert2D& A,const Vert2D& B)
	    { return (A.getAngle() < B.getAngle()); }
	    );

  VTYPE::const_iterator vc=VList.begin();
  
  std::list<Vert2D> cList;
  std::list<Vert2D>::const_iterator lc;

  // Note : VList  does not contain distIndex point
  cList.push_front(Vert2D(distIndex,Pts[distIndex]));
  cList.push_front(*vc);
  vc++;
  
  int listSize(2);
  while(vc!=VList.end())
    {
      lc=cList.begin();
      Geometry::Vec3D Line=lc->getV();
      lc++;
      const Geometry::Vec3D Origin=lc->getV();
      Line-=Origin;
      const Geometry::Vec3D A=vc->getV()-Origin;

      if ((A*Line).dotProd(normal)<0)       // Point is left
        {
	  cList.push_front(*vc); 
	  vc++;
	  listSize++;
	}
      else
        {
	  if (listSize==2)
	    throw ColErr::IndexError<int>
	      (listSize,2,"Convex wrong direction listSize == 2");
	  
	  cList.pop_front();
	  listSize--;
	}
    }

  VList.clear();
  for(lc=cList.begin();lc!=cList.end();lc++)
    VList.push_back(*lc);
  
  return;
}

int
Convex2D::inHull(const Geometry::Vec3D& testPt) const
  /*!
    Check the point is within/out of the hull
    \param testPt :: Test point
    \return 1 : in  / -1 out and 0 on the edge
   */
{
  ELog::RegMethod RegA("Convex2D","inHull");
  // First remove from Z comonents"
  const double alpha=testPt.dotProd(normal);
  Geometry::Vec3D Pt= testPt-normal*alpha;
 
  const size_t nP=Pts.size();
  double tD(0.0),tX;
  int onLine(0);
  Geometry::Vec3D PtZero=Pts[0]-normal*(Pts[0].dotProd(normal));
  for(size_t i=0;i<nP;i++)
    {
      const size_t iPlus=(i+1) % nP;
      Geometry::Vec3D PtPlus=Pts[iPlus]-normal*(Pts[iPlus].dotProd(normal));
      const Geometry::Vec3D dVec=PtPlus-PtZero;
      tX=dVec.dotProd(testPt-PtZero);
      if (!i) tD=tX;
      if (fabs(tX)<Geometry::zeroTol)
	onLine=1;
      else if ( tX * tD < 0.0 )
	return -1;
      PtZero=PtPlus;
    }
  return (onLine) ? 0 : 1;

}

std::vector<Geometry::Vec3D>
Convex2D::getSequence() const
  /*!
    Convert the list of vertex into a list of points
    \return Vector of sorted points
   */
{
  std::vector<Geometry::Vec3D> Out(VList.size());
  transform(VList.begin(),VList.end(),Out.begin(),
	    [](const Geometry::Vert2D& A)
	    { return A.getV(); });

  return Out;
}

void
Convex2D::constructHull()
  /*!
    Construct hull onto a completed Hull.
    Adds point by point
  */
{
  ELog::RegMethod RegItem("Convex2D","constructHull");
  calcNormal();
  calcMaxIndex();
  createVertex();
  return;
}

void
Convex2D::scalePoints(const double shift)
  /*!
    Given a shift value move each point approiately
    bse on shifting the planes out by the appropiate
    amount. Note that plane inside the convex have to move in
    \param shift :: shift values
  */
{
  ELog::RegMethod RegA("Convex2D","scalePoints");

  // create two orthonmal  (needed ???)
  const Geometry::Vec3D X=normal.crossNormal();
  const Geometry::Vec3D Y=normal*X;

  std::vector<Geometry::Vec3D> newPts;
  for(size_t i=0;i<VList.size();i++)
    {
      const size_t aIndex=(i) ? i-1 : VList.size()-1;
      const size_t bIndex=(i+1) % VList.size();

      const Geometry::Vec3D& O=VList[i].getV();
      const Geometry::Vec3D& A=VList[aIndex].getV();
      const Geometry::Vec3D& B=VList[bIndex].getV();
      // vectors point
      const Geometry::Vec3D aMidPlane(centroid-(O+A)/2.0);
      const Geometry::Vec3D bMidPlane(centroid-(O+B)/2.0);

      Geometry::Vec3D aPlaneNorm((A-O).unit());
      Geometry::Vec3D bPlaneNorm((B-O).unit());
      
      aPlaneNorm.makePosCos(aMidPlane);
      bPlaneNorm.makePosCos(bMidPlane);

      // construct New points on both of the planes
      const double signedShift=(VList[i].isOnHull()) ? shift : -shift;
      const Geometry::Vec3D aPA=A-aPlaneNorm*signedShift;
      const Geometry::Vec3D bPA=O-aPlaneNorm*signedShift;

      const Geometry::Vec3D aPB=B-bPlaneNorm*signedShift;
      const Geometry::Vec3D bPB=O-bPlaneNorm*signedShift;

      const Geometry::Line aPlane(aPA,bPA-aPA);
      const Geometry::Line bPlane(aPB,bPB-aPB);
      newPts.push_back(aPlane.closestPoint(bPlane));
    }
  // Finished so now can track points back to Vect2D
  for(size_t i=0;i<Pts.size();i++)
  for(const Geometry::Vec3D& Pt : newPts )
    VList[i].setPoint(Pt);
  Pts=newPts;
  return;
}
  
void
Convex2D::write(std::ostream& OX) const
  /*!
    Write out the convex hull
    \param OX :: Output stream
  */
{
  OX<<"----------   POINTS: ----------------"<<std::endl;
  std::vector<Vert2D>::const_iterator vc;
  for(vc=VList.begin();vc!=VList.end();vc++)
    {
      vc->write(OX);
      OX<<std::endl;
    }
  return;
}

} // NAMESPACE Geometry
  
