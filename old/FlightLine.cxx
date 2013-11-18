/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/FlightLine.cxx
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
#include <boost/shared_ptr.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "InputControl.h"
#include "Element.h"
#include "Material.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Tensor.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
#include "Plane.h"
#include "SurInter.h"
#include "FlightLine.h"

namespace LensSystem
{

FlightLine::FlightLine(const Geometry::Vec3D& A,const Geometry::Vec3D& C,
		       const double D,const double W,const double H) :
  Axis(A.unit()),Z(Geometry::Vec3D(0,0,1)),
  X(Axis*Z),Centre(C),FDist(D),EDist(-1),
  Width(W),Height(H),StopSurf(0)
  /*!
    Constructor
    \param A :: Axis :: Axis for centre line
    \param C :: Centre point
    \param W :: Width
    \param H :: Height    
    \param D :: Height
  */
{}

FlightLine::FlightLine() :
    Axis(Geometry::Vec3D(0,1,0)),
    Z(Geometry::Vec3D(0,0,1)),
    X(Geometry::Vec3D(1,0,0)),
    FDist(0),EDist(-1),Width(0.0),Height(0.0),
    StopSurf(0)
  /*!
    Constructor
  */
{}

FlightLine::FlightLine(const FlightLine& A) :
    Axis(A.Axis),Z(A.Z),
    X(A.X),Centre(A.Centre),
    FDist(A.FDist),EDist(A.EDist),
    Width(A.Width),Height(A.Height)
  /*!
    Copy Constructor
    \param A :: FlightLine to copy
  */
{}


FlightLine&
FlightLine::operator=(const FlightLine& A) 
  /*!
    Assignment operator
    \param A :: FlightLine to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Axis=A.Axis;
      Z=A.Z;
      X=A.X;
      Centre=A.Centre;
      FDist=A.FDist;
      EDist=A.EDist;
      Width=A.Width;
      Height=A.Height;
      Sides=A.Sides;
    }
  return *this;
}


void
FlightLine::generateSurfaces(const int surIndex) 
  /*!
    Create the surfaces for the flight line + store
    \param surIndex : surface index
  */		  
{
  ELog::RegMethod RegA("FlightLine","generateSurfaces");
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();

  Geometry::Plane* PPtr=
    SurI.createUniqSurf<Geometry::Plane>(surIndex+1);
  PPtr->setPlane(Centre-(X*Width),-X);
  Sides.push_back(SurI.addTypeSurface(PPtr));

  PPtr=SurI.createUniqSurf<Geometry::Plane>(surIndex+2);
  PPtr->setPlane(Centre+X*Width,X);
  Sides.push_back(SurI.addTypeSurface(PPtr));

  PPtr=SurI.createUniqSurf<Geometry::Plane>(surIndex+5);
  PPtr->setPlane(Centre-Z*Height,-Z);
  Sides.push_back(SurI.addTypeSurface(PPtr));
  

  PPtr=SurI.createUniqSurf<Geometry::Plane>(surIndex+6);
  PPtr->setPlane(Centre+Z*Height,Z);  
  Sides.push_back(SurI.addTypeSurface(PPtr));

  return;
}

int
FlightLine::hasInterceptLeft(const FlightLine &A,std::pair<int,int>& Out) const
  /*!
    Determine if the surface A will intercept before 
    the distance in this. 
    - this :: LEFT object
    - A :: RIGHT object
    \param A :: surface to compare
    \param Out :: Pair<leftWall,rightWall>
    \return Number of sides
  */
{
  ELog::RegMethod RegA("FlightLine","hasInterceptLeft");

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
  ELog::EM<<"Point == "<<Pt.front()<<ELog::endErr;
  if (!Pt.empty() && Pt.front().Distance(Centre)>FDist)
    {
      Out.first=A.Sides[0]->getName();
      N++;
    }

  Pt=SurInter::makePoint(&XP,Sides[1],A.Sides[1]);
  ELog::EM<<"Point == "<<Pt.front()<<ELog::endErr;
  if (!Pt.empty() && Pt.front().Distance(Centre)>FDist)
    {
      Out.second=A.Sides[1]->getName();
      N++;
    }
    
  return N;
}

int
FlightLine::hasInterceptRight(const FlightLine &A,std::pair<int,int>& Out) const
  /*!
    Determine if the surface A will intercept before 
    the distance in this. 
    - this :: RIGHT object
    - A :: LEFT object
    \param A :: surface to compare
    \param Out :: Pair<leftWall,rightWall>
    \return Number of sides
  */
{
  ELog::RegMethod RegA("FlightLine","hasInterceptRight");

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
  if (!Pt.empty() && Pt.front().Distance(Centre)>FDist)
    {
      Out.first=A.Sides[1]->getName();
      N++;
    }

  Pt=SurInter::makePoint(&XP,Sides[0],A.Sides[0]);
  if (!Pt.empty() && Pt.front().Distance(Centre)>FDist)
    {
      Out.second=A.Sides[0]->getName();
      N++;
    }
    
  return N;
}

std::pair<int,int>
FlightLine::zMatch(const FlightLine& A) const
  /*!
    Determine if extra Z fragments have to be
    added to the side in this direction
    \param A :: FlightLine adjacent
    \return A[3],A[4] names if required 
  */
{
  ELog::RegMethod RegA("FlightLine","zMatch");

  std::pair<int,int> Out(0,0);
  if (A.Sides.size()<4) return Out;

  // Z [low]
  if (A.Sides[2]->getName()!=Sides[2]->getName())
    {
      if (A.Z!=Z || A.Sides[2]->getDistance()<Sides[2]->getDistance())
	Out.first=A.Sides[2]->getName();
    }
  // Z [high]
  if (A.Sides[3]->getName()!=Sides[3]->getName())
    {
      if (A.Z!=Z || A.Sides[3]->getDistance()<Sides[3]->getDistance())
	Out.second=A.Sides[3]->getName();
    }
  return Out;
}

std::string
FlightLine::createCell(const FlightLine* Left,
		       const FlightLine* Right) const
  /*!
    Create the cell + / - 
    \param Left :: Left cell
    \param Right :: Right cell 
    \return cell string including left/right intersects.
  */
{
  ELog::RegMethod RegA("FlightLine","createCell");

  std::ostringstream cx;
  // Stop line:
  if (StopSurf) cx<<StopSurf;

  std::vector<Geometry::Plane*>::const_iterator vc;
  for(vc=Sides.begin();vc!=Sides.end();vc++)
    {
      cx<<" "<<-(*vc)->getName();
    }

  // Check Left:
  if (Left)
    {
      std::pair<int,int> wall;
      const int N=hasInterceptLeft(*Left,wall);
      if (N)
	{
	  std::pair<int,int> roof=this->zMatch(*Left);
	  std::vector<int> xS;
	  if (roof.first) xS.push_back(roof.first);
	  if (roof.second) xS.push_back(roof.second);
	  if (wall.first) xS.push_back(wall.first);
	  if (wall.second) xS.push_back(wall.second);
	  if (xS.size()==1)
	    cx<<" "<<xS.front();
	  else if (!xS.empty())
	    {
	      cx<<" (";
	      for(size_t i=0;i<xS.size()-1;i++)
		cx<<xS[i]<<" : ";
	      cx<<xS.back()<<" )";
	    }
	}
    }

  // Check Right:
  if (Right)
    {
      std::pair<int,int> wall;
      const int N=hasInterceptRight(*Right,wall);
      if (N)
	{
	  std::pair<int,int> roof=this->zMatch(*Right);
	  std::vector<int> xS;
	  if (roof.first) xS.push_back(roof.first);
	  if (roof.second) xS.push_back(roof.second);
	  if (wall.first) xS.push_back(wall.first);
	  if (wall.second) xS.push_back(wall.second);
	  if (xS.size()==1)
	    cx<<" "<<xS.front();
	  else if (!xS.empty())
	    {
	      cx<<" (";
	      for(size_t i=0;i<xS.size()-1;i++)
		cx<<xS[i]<<" : ";
	      cx<<xS.back()<<" )";
	    }
	}
    }
  return cx.str();
}



} // NAMESPACE LensSystem

