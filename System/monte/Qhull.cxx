/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monte/Qhull.cxx
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
#include <deque>
#include <stack>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <memory>
#include <functional>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "OutputLog.h"
#include "Transform.h"
#include "Surface.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Object.h"
#include "SurfVertex.h"
#include "Line.h"
#include "SurInter.h"
#include "Qhull.h"

namespace MonteCarlo
{

Qhull::Qhull() : Object()
  /*!
    Default Constructor
  */
{}

Qhull::Qhull(const int N,const int M,
	     const double T,const std::string& Line) :
  Object(N,M,T,Line)
  /*!
    Constuctor, sets number/material and temperature 
   \param N :: number
   \param M :: material
   \param T :: temperature
   \param Line :: Line to use
  */
{}

Qhull::Qhull(const Qhull& A) : Object(A),
  VList(A.VList),CofM(A.CofM)
  /*!
    Copy constructor
    \param A :: Qhull to copy
  */
{}

Qhull&
Qhull::operator=(const Qhull& A)
  /*!
    Assignment operator=
    \param A :: Qhull to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Object::operator=(A);
      VList=A.VList;
      CofM=A.CofM;
    }
  return *this;
}

Qhull::~Qhull()
  /*!
    Destructor
  */
{}


Qhull*
Qhull::clone() const 
  /*!
    Virtual copy constructor
    \return copy ( *this )
   */
{
  return new Qhull(*this);
}

int
Qhull::calcIntersections()
  /*! 
    Loops over all the surfaces and calculates the appropiate
    intersection
    \return number of items intersection points.
  */
{
  ELog::RegMethod RegA("Qhull","calcIntersections");

  VList.clear();                       // clear list of Vertex
  std::vector<const Geometry::Surface*>::const_iterator ic,jc,kc;
  int cnt(0);
  for(ic=SurList.begin();ic!=SurList.end();ic++)
    for(jc=ic+1;jc!=SurList.end();jc++)
      for(kc=jc+1;kc!=SurList.end();kc++)
        {
	  // This adds intersections to the VList
	  cnt+=getIntersect(*ic,*jc,*kc);
	}
  // return number of item found
  return cnt;
}

int
Qhull::getIntersect(const Geometry::Surface* SurfX,
		    const Geometry::Surface* SurfY,
		    const Geometry::Surface* SurfZ)
  /*!
    Calculates the intersection between three surfaces.
    Adds the point to the vertex list if the point is valid 
    and is on a side.
    
    \param SurfX :: Surface pointer
    \param SurfY :: Surface pointer
    \param SurfZ :: Surface pointer
    \returns Number intersections found
  */
{
  ELog::RegMethod RegA("Qhull","getIntersect");

  const int AS=SurfX->getName();
  const int BS=SurfY->getName();
  const int CS=SurfZ->getName();

  std::vector<Geometry::Vec3D> PntOut=
    SurInter::processPoint(SurfX,SurfY,SurfZ);

  int Ncnt(0);
  std::set<int> ExSN={AS,BS,CS};
	
  for(const Geometry::Vec3D& VC : PntOut)
    {
      if (isValid(VC,ExSN) &&         // Is point in/on the object
	  VC.abs()<1e8)          // Tracked onto the very large points
	{
	  SurfVertex tmp;
	  tmp.addSurface(const_cast<Geometry::Surface*>(SurfX));
	  tmp.addSurface(const_cast<Geometry::Surface*>(SurfY));
	  tmp.addSurface(const_cast<Geometry::Surface*>(SurfZ));
	  tmp.setPoint(VC);
	  VList.push_back(tmp);
	  Ncnt++;
	}
    }      
  return Ncnt;
}

void
Qhull::printVertex() const
  /*!
    Print out the Vlist  
  */
{
  std::vector<SurfVertex>::const_iterator vc;
  for(vc=VList.begin();vc!=VList.end();vc++)
    {
      std::cout<<"SurfVertex["<<1+static_cast<int>(vc-VList.begin())<<"] == ";
      std::cout<<(*vc)<<std::endl;
    }
  return;
}

void
Qhull::printVertex(const Geometry::Vec3D& Dis,
		   const Geometry::Matrix<double>& Rot) const
  /*!
   Print out each vertex after applying the displacement Dis and
   the rotation Rot to the point
   \param Dis :: displacement to add to the vertex point
   \param Rot :: Rotation to apply to the vertex point.
  */
{
  // Print out the Vlist
  std::vector<SurfVertex>::const_iterator vc;
  for(vc=VList.begin();vc!=VList.end();vc++)
    {
      std::cout<<"Vertex["<<1+static_cast<int>(vc-VList.begin())<<"] == ";
      vc->print(std::cout,Dis,Rot);
      std::cout<<std::endl;
    }
  return;
}

void
Qhull::calcCentreOfMass() 
  /*!
    Calculate the centre of mass of the cell
  */
{
  ELog::RegMethod RegA("Qhull","calcCentreOfMass");

  if (!VList.empty())
    {
      CofM=Geometry::Vec3D(0,0,0);
      std::vector<SurfVertex>::const_iterator vc;
      for(vc=VList.begin();vc!=VList.end();vc++)
	{
	  // if (debug)
	  //   ELog::EM<<"Point == "<<vc->getPoint()<<ELog::endTrace;
	  CofM+=vc->getPoint();
	}
      CofM/=static_cast<double>(VList.size());
    }
  return;
}

int
Qhull::calcMidVertex()
  /*!
    Calculate the mid point from a number of object
    that do not have vertex systems.
    \return Number of mid points
  */
{
  ELog::RegMethod RegA("Qhull","calcMidVertex");

  int cnt(0);
  if (VList.empty())
    {
      std::vector<const Geometry::Surface*>::const_iterator ic,jc,kc;

      Geometry::Vec3D Pt;
      Geometry::Vec3D CM; 
      for(ic=SurList.begin();ic!=SurList.end();ic++)
	for(jc=ic+1;jc!=SurList.end();jc++)
	  for(kc=jc+1;kc!=SurList.end();kc++)
	    {
	      // This adds intersections to the VList
	      if (SurInter::getMidPoint(*ic,*jc,*kc,Pt) &&
		  isValid(Pt))
		{
		  CM+=Pt;
		  cnt++;
		}
	    }
      if (cnt)
	CofM=CM/cnt;
    }
  return cnt;
}

std::vector<Geometry::Vec3D>   
Qhull::getVertex() const 
  /*!
    Get a list of the vertex 
    \return vector of the Vec3D pts
  */
{
  std::vector<Geometry::Vec3D> outV(VList.size());
  transform(VList.begin(),VList.end(),outV.begin(),
	    std::bind(&SurfVertex::getPoint,std::placeholders::_1));
  return outV;
}

int
Qhull::calcVertex()
  /*!
    Loop over all the surfaces, determine if the 
    plane intersection exists and calculate
    the vertii
    \return Number of intersection vertexs found.
  */
{
  ELog::RegMethod RegA("Qhull","calcVertex");
  
  createSurfaceList();
  populate();
  calcIntersections();
  if (!VList.empty())  
    calcCentreOfMass();
    
  return static_cast<int>(VList.size());
}


  
Geometry::Matrix<double>
Qhull::getRotation(const unsigned int M,const unsigned int A,
		   const unsigned int B,const unsigned int C) const
  /*! 
     This takes three points + Origin (first point)
     and generates the rotation matrix
     Note:: it is likely that the displacement vector
     has to be APPLIED first 
     \param M :: Vertex point in object for zero
     \param A :: Vertex point for x-axis (A-M)
     \param B :: Vertex point for y-axis (B-M)
     \param C :: Vertex point for z-axis (C-M)
     \return Rotation matrix
  */
{
  ELog::RegMethod RegA("Qhull","getRotation");

  if (VList.size()<=M || VList.size()<=A || 
      VList.size()<=B || VList.size()<=C)
    {
      ELog::EM<<"Out of range ("<<
	VList.size()<<")"<<ELog::endErr;
      Geometry::Matrix<double> Ident(3,3);
      Ident.identityMatrix();
      return Ident;
    }

  const Geometry::Vec3D Origin=VList[M].getPoint();
  Geometry::Vec3D Ax=VList[A].getPoint()-Origin;
  Geometry::Vec3D By=VList[B].getPoint()-Origin;
  Geometry::Vec3D Cz=VList[C].getPoint()-Origin;
  Ax.makeUnit();
  By.makeUnit();
  Cz.makeUnit();
  Geometry::Matrix<double> Out(3,3);
  for(int i=0;i<3;i++)
    {
      Out[0][i]=Ax[i];
      Out[1][i]=By[i];
      Out[2][i]=Cz[i];
    }
//  Out.Invert();
  return Out;
}

int 
Qhull::isConnected(const int) const
  /*! 
     Checks if the points connect between themselves and other points 
     \return  **Currently returns 1
     \todo Not written
  */
{
  return 1;
}


Geometry::Vec3D
Qhull::getDisplace(const unsigned int A) const
  /*! 
     Get point on the vertex list given by A
     \param A :: Point Index 
     \retval Origin point if point not found.
  */
{
  if (VList.size()>=A)
    return VList[A].getPoint();
  return Geometry::Vec3D(0,0,0);
}


std::string
Qhull::cellStr(const std::map<int,Qhull*>& MList) const
  /*!
    Returns just the cell string object. Processes complement
    and self include.
    \param MList :: List of indexable Hulls
    \return Cell String (from TopRule)
    \todo Break infinite recusion
  */
{
  ELog::RegMethod RegA("Qhull","cellStr");

  const char compUnit[]="%#";
  std::string TopStr=this->topRule()->display();
  std::string::size_type pos=TopStr.find_first_of(compUnit);
  std::ostringstream cx;
  while(pos!=std::string::npos)
    {
      const int compFlag(TopStr[pos]=='%' ? 0 : 1); 
      pos++;
      cx<<TopStr.substr(0,pos);            // Everything including the #
      int cN(0);
      const size_t nLen=StrFunc::convPartNum(TopStr.substr(pos),cN);
      if (nLen>0)
        {
	  std::map<int,MonteCarlo::Qhull*>::const_iterator vc=MList.find(cN);
	  if (vc==MList.end() || cN==this->getName())
	    {
	      ELog::EM<<"Cell:"<<getName()<<" comp unit:"
		      <<cN<<ELog::endCrit;
	      ELog::EM<<"full string == "
		      <<topRule()->display()<<ELog::endCrit;
	      cx<<compUnit[compFlag]<<cN;
	      throw ColErr::InContainerError<int>
		(cN,"Qhull::cellStr unknown complementary unit");
	    }
	  else
	    {
	      if (compFlag) cx<<"(";
	      // Not the recusion :: This will cause no end of problems 
	      // if there is an infinite loop.
	      cx<<vc->second->cellStr(MList);
	      if (compFlag) cx<<")";
	    }
	  cx<<" ";
	  pos+=nLen;
	}
      TopStr.erase(0,pos);
      pos=TopStr.find_first_of(compUnit);
    }

  cx<<TopStr;
  return cx.str();
}

void
Qhull::displace(const Geometry::Vec3D& DVec)
  /*!
    Applies a displacement to the SurfVertex objects
    \param DVec :: displacement
  */
{
  for(SurfVertex& vc :  VList)
    vc.displace(DVec);
  return;
}


void
Qhull::rotate(const Geometry::Matrix<double>& MRot)
  /*!
    Applies a displacement to the Vertex objects
    \param MRot :: rotation matrix
  */
{
  for(SurfVertex& vc :  VList)
    vc.rotate(MRot);
  return;
}


void
Qhull::mirror(const Geometry::Plane& PObj)
  /*!
    Applies a displacement to the Vertex objects
    \param PObj :: Plane to mirror around
  */
{
  for(SurfVertex& vc :  VList)
    vc.mirror(PObj);
  return;
}

}  // NAMESPACE MonteCarlo
