/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   lensModel/lensFL.cxx
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
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <iterator>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Tensor.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "HeadRule.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "SurInter.h"
#include "ContainedComp.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "lensFL.h"

namespace lensSystem
{

lensFL::lensFL() : 
  attachSystem::ContainedComp(),attachSystem::FixedComp("lensFL",4),
  Axis(Geometry::Vec3D(0,1,0)),Z(Geometry::Vec3D(0,0,1)),
  X(Geometry::Vec3D(1,0,0)),Width(0.0),
  Height(0.0),liner(0.0)
  /*!
    Constructor
  */
{}

lensFL::lensFL(const Geometry::Vec3D& C,const Geometry::Vec3D& A,
		       const double W,const double H,const double L) :
  attachSystem::ContainedComp(),attachSystem::FixedComp("lensFL",4),
  Axis(A.unit()),Z(Geometry::Vec3D(0,0,1)),
  X(Axis*Z),Centre(C),Width(W),Height(H),liner(L)
  /*!
    Constructor
    \param A :: Axis :: Axis for centre line
    \param C :: Centre point
    \param W :: Width
    \param H :: Height     
    \param L :: Liner thickness
  */
{}


lensFL::lensFL(const lensFL& A) :
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  Axis(A.Axis),Z(A.Z),
  X(A.X),Centre(A.Centre),
  Width(A.Width),Height(A.Height),
  liner(A.liner),Sides(A.Sides)
  /*!
    Copy Constructor
    \param A :: lensFL to copy
  */
{}


lensFL&
lensFL::operator=(const lensFL& A) 
  /*!
    Assignment operator
    \param A :: lensFL to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      Axis=A.Axis;
      Z=A.Z;
      X=A.X;
      Centre=A.Centre;
      Width=A.Width;
      Height=A.Height;
      liner=A.liner;
      Sides=A.Sides;
    }
  return *this;
}


void
lensFL::generateSurfaces(ModelSupport::surfRegister& SMap,
			     const int surIndex) 
  /*!
    Create the surfaces for the flight line + store
    \param SMap :: Surface register to use
    \param surIndex : surface index
  */		  
{
  ELog::RegMethod RegA("lensFL","generateSurfaces");

  ModelSupport::buildPlane(SMap,surIndex+3,Centre-X*Width,X);
  ModelSupport::buildPlane(SMap,surIndex+4,Centre+X*Width,X);
  ModelSupport::buildPlane(SMap,surIndex+5,Centre-Z*Height,Z);
  ModelSupport::buildPlane(SMap,surIndex+6,Centre+Z*Height,Z);

  Sides.push_back(SMap.realPtr<Geometry::Plane>(surIndex+3));
  Sides.push_back(SMap.realPtr<Geometry::Plane>(surIndex+4));
  Sides.push_back(SMap.realPtr<Geometry::Plane>(surIndex+5));
  Sides.push_back(SMap.realPtr<Geometry::Plane>(surIndex+6));

  if (liner>0.0)
    {
      ModelSupport::buildPlane(SMap,surIndex+13,Centre-X*(Width-liner),X);
      ModelSupport::buildPlane(SMap,surIndex+14,Centre+X*(Width-liner),X);
      ModelSupport::buildPlane(SMap,surIndex+15,Centre-Z*(Height-liner),Z);
      ModelSupport::buildPlane(SMap,surIndex+16,Centre+Z*(Height-liner),Z);
      Sides.push_back(SMap.realPtr<Geometry::Plane>(surIndex+13));
      Sides.push_back(SMap.realPtr<Geometry::Plane>(surIndex+14));
      Sides.push_back(SMap.realPtr<Geometry::Plane>(surIndex+15));            
      Sides.push_back(SMap.realPtr<Geometry::Plane>(surIndex+16));
    }

  return;
}

int
lensFL::hasInterceptLeft(const lensFL &A,
			     const Geometry::Surface* SPtr,
			     const int sign, 
			     std::pair<int,int>& Out) const
  /*!
    Determine if the surface A will intercept before 
    the distance in this. 
    - this :: LEFT object
    - A :: RIGHT object
    \param A :: surface to compare
    \param Out :: Pair<leftWall,rightWall>
    \param SPtr :: surface test pointer
    \param sign :: sign on surface
    \return Number of sides
  */
{
  ELog::RegMethod RegA("lensFL","hasInterceptLeft");

  Out.first=0;
  Out.second=0;
  if (A.Sides.size()<4 || Sides.size()<4)
    return 0;
  
  if (A.Z != Z) 
    {
      Out.first=A.Sides[0]->getName();
      Out.second=A.Sides[1]->getName();
      return 2;
    }

  // Surfaces must be parallel
  int N(0);
  Geometry::Plane XP(1,0,Geometry::Vec3D(0,0,0),Z);
  std::vector<Geometry::Vec3D> Pt=
    SurInter::makePoint(&XP,Sides[1],A.Sides[0]);
  ELog::EM<<"Point[0][1] == "<<Pt.front()<<ELog::endErr;
  if (!Pt.empty() && (!SPtr || 
		      (SPtr->side(Pt.front())*sign > 0)))
    {
      Out.first=A.Sides[0]->getName();
      N++;
    }

  Pt=SurInter::makePoint(&XP,Sides[1],A.Sides[1]);
  ELog::EM<<"Point[1][1] == "<<Pt.front()<<ELog::endErr;
  if (!Pt.empty() && (!SPtr || 
		      (SPtr->side(Pt.front())*sign > 0)))
    {
      Out.second=A.Sides[1]->getName();
      N++;
    }
  return N;
}

int
lensFL::hasInterceptRight(const lensFL &A,
			      const Geometry::Surface* SPtr,
			      const int sign, 
			      std::pair<int,int>& Out) const
  /*!
    Determine if the surface A will intercept before 
    the distance in this. 
    - this :: RIGHT object
    - A :: LEFT object
    \param A :: surface to compare
    \param SPtr :: surface test pointer
    \param sign :: sign on surface
    \param Out :: Pair<leftWall,rightWall>
    \return Number of sides
  */
{
  ELog::RegMethod RegA("lensFL","hasInterceptRight");

  Out.first=0;
  Out.second=0;
  if (A.Sides.size()<4 || Sides.size()<4)
    return 0;
  
  if (A.Z != Z) 
    {
      Out.first=A.Sides[1]->getName();
      Out.second=A.Sides[0]->getName();
      return 2;
    }

  // Surfaces must be parallel
  int N(0);
  Geometry::Plane XP(1,0,Geometry::Vec3D(0,0,0),Z);
  std::vector<Geometry::Vec3D> Pt=
    SurInter::makePoint(&XP,Sides[0],A.Sides[1]);
  ELog::EM<<"Point[0][1] == "<<Pt.front()<<ELog::endCrit;
  if (!Pt.empty() && (!SPtr || 
		      (SPtr->side(Pt.front())*sign > 0)))
    {
      Out.first=A.Sides[1]->getName();
      N++;
    }

  Pt=SurInter::makePoint(&XP,Sides[0],A.Sides[0]);
  ELog::EM<<"Point[0][0] == "<<Pt.front()<<ELog::endCrit;
  if (!Pt.empty() && (!SPtr || 
		      (SPtr->side(Pt.front())*sign > 0)))
    {
      Out.second=A.Sides[0]->getName();
      N++;
    }
    
  return N;
}

std::string
lensFL::getLine() const
  /*!
    Gets the line to build : signed flight line
    \return string of part-object 
  */
{
  std::ostringstream cx;
  if (Sides.size()>3)
    {
      cx<<Sides[0]->getName()<<" ";
      cx<<-Sides[1]->getName()<<" ";
      cx<<Sides[2]->getName()<<" ";
      cx<<-Sides[3]->getName()<<" ";
    }
  return cx.str();  
}

std::string
lensFL::getVirtual() const
  /*!
    Get an outside object
    \return # of flightline
  */
{
  std::ostringstream cx;
  if (Sides.size()>3)
    {
      cx<<"( "<<-Sides[0]->getName()<<" : ";
      cx<<Sides[1]->getName()<<" : ";
      cx<<-Sides[2]->getName()<<" : ";
      cx<<Sides[3]->getName()<<" )";
    }
  return cx.str();

}
  



} // NAMESPACE LensSystem

