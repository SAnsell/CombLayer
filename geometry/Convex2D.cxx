/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   geometry/Convex2D.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include <vector>
#include <map>
#include <list>
#include <string>
#include <algorithm>
#include <iterator>
#include <functional>
#include <boost/bind.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
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

const double PTolerance(1e-8);   ///< Tolerance for a point

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
  
  std::vector<Vec3D>::iterator vc=
    max_element(Pts.begin(),Pts.end(),
		boost::bind(std::greater<double>(),
			    boost::bind(&Vec3D::Distance,_2,centroid),
			    boost::bind(&Vec3D::Distance,_1,centroid) ));
  distIndex=static_cast<size_t>(distance(Pts.begin(),vc));
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

  sort(VList.begin(),VList.end(),
       boost::bind(std::less<double>(),
		   boost::bind(&Vert2D::getAngle,_1),
		   boost::bind(&Vert2D::getAngle,_2)));
  // Add first two points
  std::list<Vert2D> cList;
  std::list<Vert2D>::const_iterator lc;
  cList.push_front(Vert2D(distIndex,Pts[distIndex]));
  cList.push_front(VList.front());
  
  VTYPE::const_iterator vc=VList.begin();
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
	    throw ColErr::IndexError<int>(listSize,2,"listSize == 2");
	  
	  cList.pop_front();
	  listSize--;
	}
    }

  VList.clear();
  for(lc=cList.begin();lc!=cList.end();lc++)
    VList.push_back(*lc);
  
  return;
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
	    boost::bind(&Vert2D::getV,_1));
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
  
