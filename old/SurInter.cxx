/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/SurInter.cxx
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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <complex>
#include <vector>
#include <deque>
#include <map>
#include <string>
#include <algorithm>
#include <boost/regex.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "mathSupport.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "PolyFunction.h"
#include "PolyVar.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Transform.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Sphere.h"
#include "Cylinder.h"
#include "Cone.h"
#include "General.h"
#include "Line.h"
#include "SurInter.h"


const double STolerance(1e-6);

namespace SurInter
{

 

std::vector<Geometry::Vec3D> 
processPoint(const std::vector<const Geometry::Surface*>& SInter)
  /*! 
     Since Surface is abstract so the vector is 
     of derived classes. This determines the derived class
     name and case the surface (dynamic_cast) and uses this
     object to dispatch to the intersection.
     \param SInter :: Intersection surface 
     \returns vector of Intersections
  */
{
  const std::vector<std::pair<const Geometry::Surface*,int> > SRules;
  // Call with no-additional rules
  return processPoint(SInter,SRules);
}

std::vector<Geometry::Vec3D> 
processPoint(const Geometry::Surface* A,const Geometry::Surface* B,
		       const Geometry::Surface* C)
  /*! 
     This determines the derived class name and case the surface 
     (dynamic_cast) and uses this object to dispatch to the intersection.
     \param A :: Intersection surface 
     \param B :: Intersection surface 
     \param C :: Intersection surface 
     \returns vector of Intersections
  */
{
  // Null Vector for rules
  const std::vector<std::pair<const Geometry::Surface*,int> > SRules;
  std::vector<const Geometry::Surface*> SInter;
  SInter.push_back(A);
  SInter.push_back(B);
  SInter.push_back(C);
  return processPoint(SInter,SRules);
}

std::vector<Geometry::Vec3D> 
processPoint(const std::vector<const Geometry::Surface*>& SInter)
  /*! 
     Since Surface is abstract so the vector is 
     of derived classes. This determines the derived class
     name and case the surface (dynamic_cast) and uses this
     object to dispatch to the intersection.
     \param SInter :: Intersection surface 
     \returns vector of Intersections
  */
{
  static SurInter::IntMap XX;

  std::vector<const Geometry::Surface*> SVec(SInter);
  std::vector<Geometry::Vec3D> Out;          // output vector
  std::vector<int> Index;
  std::vector<const Geometry::Surface*>::const_iterator vc;
  for(vc=SVec.begin();vc!=SVec.end();vc++)
    {
      const std::string A=(*vc)->className();
      int flag=XX.find(A);
      if (!flag)
	return Out;
      Index.push_back(flag);
    }
  mathFunc::crossSort(Index,SVec);
  std::vector<int>::const_iterator ic;
  int type(0);
  for(ic=Index.begin();ic!=Index.end();ic++)
    {
      type*=10;
      type+=*ic;
    }
  switch (type)
    {
    case 111:
      Out=makePoint(dynamic_cast<const Geometry::Plane*>(SVec[0]),
		    dynamic_cast<const Geometry::Plane*>(SVec[1]),
		    dynamic_cast<const Geometry::Plane*>(SVec[2]));
      break;
    case 113:
      Out=makePoint(dynamic_cast<const Geometry::Plane*>(SVec[0]),
		    dynamic_cast<const Geometry::Plane*>(SVec[1]),
		    dynamic_cast<const Geometry::Cylinder*>(SVec[2]));
    default:
      
      ELog::EM<<"Unwritten multi-surface intersect calc:"
	      <<type<<ELog::endWarn;
      return Out;
    }

  return Out;
}

std::vector<Geometry::Vec3D>
makePoint(const Geometry::Plane* A,const Geometry::Plane* B,const Geometry::Plane* C)
  /*!
    Calculate the intersection between three planes
    \param A :: Plane pointer 
    \param B :: Plane pointer 
    \param C :: Plane pointer 
  */
{
  ELog::RegMethod RegA("SurInter","makePoint(P,P,P)");
  std::vector<Geometry::Vec3D> Out;
  Geometry::Line Lx;
  if (Lx.setLine(*A,*B))
    {
      Lx.intersect(Out,*C);
    }
  return Out;  
}


std::vector<Geometry::Vec3D>
makePoint(const Geometry::Plane* A,const Geometry::Plane* B,
		    const Geometry::Cylinder* C)
  /*!
    Calculate the intersection between three planes
    \param A :: Plane pointer 
    \param B :: Plane pointer 
    \param C :: Cylinder pointer 
  */
{
  ELog::RegMethod RegA("SurInter","makePoint(P,P,C)");
  std::vector<Geometry::Vec3D> Out;
  Geometry::Line Lx;
  if (Lx.setLine(*A,*B))
    {
      Lx.intersect(Out,*C);  // this could be tested 
    }
  return Out;  
}

std::vector<Geometry::Vec3D>
makePoint(const Geometry::Plane* A,const Geometry::Plane* B,
		    const Geometry::Quadratic* C)
  /*!
    Calculate the intersection between three planes
    \param A :: Plane pointer 
    \param B :: Plane pointer 
    \param C :: Quadratic
  */
{
  ELog::RegMethod RegA("SurInter","makePoint(P,P,C)");
  std::vector<Geometry::Vec3D> Out;
  Geometry::Line Lx;
  if (Lx.setLine(*A,*B))
    {
      Lx.intersect(Out,*C);  // this could be tested 
    }
  return Out;  
}

std::vector<Geometry::Vec3D>
makePoint(const Geometry::Quadratic* A,const Geometry::Quadratic* B,
		    const Geometry::Quadratic* C)
  /*!
    Calculate the intersection between three quadratic surfaces
    \param A :: Quadratic pointer
    \param B :: Quadratic pointer
    \param C :: Quadratic pointer 
    \return the points found
  */
{
  ELog::RegMethod RegA("SurInter","makePoint(Q,Q,Q)");
  std::vector<Geometry::Vec3D> Out;
  if (!A || !B || !C || A==B || C==B || A==C)
    {
      ELog::EM<<"Quadratics not valid"<<ELog::endErr;
      return Out;
    }
  
  mathLevel::PolyVar<3> FXYZ;
  mathLevel::PolyVar<3> GXYZ;
  mathLevel::PolyVar<3> HXYZ;
  // Addition to avoid missing varaibles 

  FXYZ.makeTriplet(A->copyBaseEqn());
  GXYZ.makeTriplet(B->copyBaseEqn());
  HXYZ.makeTriplet(C->copyBaseEqn());
  mathLevel::PolyVar<3> masterXYZ(FXYZ);

  if (masterXYZ.expand(GXYZ) && masterXYZ.expand(HXYZ))
    return Out;

//  FXYZ+=GXYZ+HXYZ;
  mathLevel::PolyVar<2> OutX=FXYZ.reduce(GXYZ);
  mathLevel::PolyVar<2> OutY=FXYZ.reduce(HXYZ);
  // Need a master with 
  mathLevel::PolyVar<2> masterXY(OutX);
  masterXY.expand(OutY);

  mathLevel::PolyVar<1> OutFinal=OutX.reduce(OutY);
  const std::vector<double> rootX=OutFinal.realRoots();
  std::vector<double>::const_iterator xv,yv,zv;
  for(xv=rootX.begin();xv!=rootX.end();xv++)
    {
      mathLevel::PolyVar<1> YA=masterXY.substitute(*xv);
      const std::vector<double> rootY=YA.realRoots();
      for(yv=rootY.begin();yv!=rootY.end();yv++)
        {
	  mathLevel::PolyVar<1> ZA=masterXYZ.substitute<1>(*xv,*yv);
	  const std::vector<double> rootZ=ZA.realRoots();
	  for(zv=rootZ.begin();zv!=rootZ.end();zv++)
	    {
	      Out.push_back(Geometry::Vec3D(*xv,*yv,*zv));
	    }
	}
    }
  // Remove identical solutions
  std::sort(Out.begin(),Out.end());
  std::vector<Geometry::Vec3D>::iterator cutPt=
    std::unique(Out.begin(),Out.end());
  Out.erase(cutPt,Out.end());

  return Out;
}


