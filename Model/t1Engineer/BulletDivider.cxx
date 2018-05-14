/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Engineer/BulletDivider.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell/Goran Skoro
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
#include <numeric>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "SimProcess.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "BulletDivider.h"

namespace ts1System
{

BulletDivider::BulletDivider(const std::string& Key,const int Index,
			       const int sideDir)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedComp(Key+StrFunc::makeString(Index),6),
  ID(Index),baseName(Key),
  divIndex(ModelSupport::objectRegister::Instance().cell(keyName)),
  cellIndex(divIndex+1),divDirection(sideDir)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
    \param Index :: ID number of divider
    \param sideDir :: Direction of curve
  */
{}

BulletDivider::BulletDivider(const BulletDivider& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  ID(A.ID),baseName(A.baseName),divIndex(A.divIndex),
  cellIndex(A.cellIndex),divDirection(A.divDirection),
  YStep(A.YStep),radii(A.radii),length(A.length),
  endPts(A.endPts),wallThick(A.wallThick),wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: BulletDivider to copy
  */
{}

BulletDivider&
BulletDivider::operator=(const BulletDivider& A)
  /*!
    Assignment operator
    \param A :: BulletDivider to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      YStep=A.YStep;
      radii=A.radii;
      length=A.length;
      endPts=A.endPts;
      wallThick=A.wallThick;
      wallMat=A.wallMat;
    }
  return *this;
}



BulletDivider::~BulletDivider() 
 /*!
   Destructor
  */
{}


void
BulletDivider::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: Variable database
 */
{
  ELog::RegMethod RegA("BulletDivider","populate");

  YStep=Control.EvalPair<double>(keyName,baseName,"YStep");

  const size_t nRadii=
    Control.EvalPair<size_t>(keyName,baseName,"NRadii");

  double R,L;
  radii.push_back(0.0);  // set later
  for(size_t i=0;i<nRadii;i++)
    {
      const std::string Rkey=StrFunc::makeString(i);
      
      R=Control.EvalDefPair<double>(keyName+"Radius"+Rkey,
				    keyName+"Radius",-1.0);
      if (R<0)
	R=Control.EvalPair<double>(baseName+"Radius"+Rkey,
				   baseName+"Radius");

      radii.push_back(R);
      L=Control.EvalDefPair<double>(keyName+"Length"+Rkey,
				    keyName+"Length",-1.0);
      if (L<0)
	L=Control.EvalPair<double>(baseName+"Length"+Rkey,
				   baseName+"Length");
      length.push_back(L);
    }
  
  wallThick=Control.EvalPair<double>(keyName,baseName,"WallThick");
  // Material
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat",
				     baseName+"WallMat");

  return;
}

void
BulletDivider::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    - Y Down the beamline
    \param FC :: FixedComp for origin and axis
  */
{
  ELog::RegMethod RegA("BulletDivider","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC);
  applyShift(0,YStep,0);
  return;
}

void
BulletDivider::setInnerRadius(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Calculate the inner radius of the main target
    \param FC :: PlateTarget inner
    \param sideIndex :: outer radial point
   */
{
  ELog::RegMethod RegA("BulletDivider","setInnerRadius");

  const Geometry::Vec3D Pt=FC.getLinkPt(sideIndex);
  radii[0]=std::abs(X.dotProd(Pt-Origin));
  
  return;
}
  

void
BulletDivider::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("BulletVessel","createSurface");

  
  // Dividing plane
  const Geometry::Vec3D XDir= (divDirection>0) ? X : -X;
  ModelSupport::buildPlane(SMap,divIndex+3,Origin,XDir);

    // First layer [Bulk]
  ModelSupport::buildPlane(SMap,divIndex+1,Origin,Y);

  int DV(divIndex);
  Geometry::Vec3D CPt(Origin);
  double tAngle(0.0);
  for(size_t i=0;i<length.size();i++)
    {
      tAngle=0.0;
      if (fabs(radii[i]-radii[i+1])<Geometry::zeroTol)
	{
	  ModelSupport::buildCylinder(SMap,DV+7,Origin,Y,radii[i]);
	  ModelSupport::buildCylinder(SMap,DV+17,Origin,Y,
				   radii[i]+wallThick);
	}
      else
	{
	  // using similar triangles:
	  const double x=radii[i]*length[i]/(radii[i+1]-radii[i]);
	  tAngle=std::abs(radii[i+1]-radii[i])/length[i];
	  const double theta=180.0/M_PI*atan(tAngle);	  	  
	  // Inner
	  ModelSupport::buildCone(SMap,DV+7,CPt-Y*x,Y,theta);
	  // Wall
	  ModelSupport::buildCone(SMap,DV+17,
				  CPt-Y*(x+wallThick/tAngle),Y,theta);
	}
      endPts.push_back(CPt+XDir*(radii[i]+(wallThick/2.0)));

      CPt+=Y*length[i];
      ModelSupport::buildPlane(SMap,DV+2,CPt,Y);
      DV+=100;
    }
  
  if (tAngle>Geometry::zeroTol)
    endPts.push_back(CPt+XDir*(radii.back()+(wallThick/tAngle/2.0)));
  else
    endPts.push_back(CPt+XDir*(radii.back()+(wallThick/2.0)));

  return;
}

void
BulletDivider::createObjects(Simulation& System,
			     const attachSystem::FixedComp& TarObj,
			     const long int radialSide,
			     const attachSystem::FixedComp& VesselObj,
			     const long int topSide,const long int baseSide)
  /*!
    Creates the target vessel objects
    \param System :: Simulation to create objects in
    \param TarObj :: Inner link surface for radius
    \param radialSide :: link point for inner radius
    \param vesselObj :: Inner link surface for top/base
    \param topSide :: Link point for top
    \param baseSide :: Link point for bas
  */
{
  ELog::RegMethod RegA("BulletVessel","createObjects");
  
  std::string Out;

  const std::string innerRadii=TarObj.getLinkString(radialSide);
  const std::string vertCut=VesselObj.getLinkString(topSide)+
    VesselObj.getLinkString(baseSide);
  int DV(divIndex);

  // Note special first surface Special for first contact:
  std::string frontSurf=
    ModelSupport::getComposite(SMap,divIndex," 1 ")+innerRadii;

  for(size_t i=0;i<length.size();i++)
    {
      // TA 
      Out=ModelSupport::getComposite(SMap,DV,divIndex,"-2 3M -17 7");
      Out+=frontSurf+vertCut;
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  
      Out=ModelSupport::getComposite(SMap,DV,divIndex," -2 -17 7 ");
      addOuterUnionSurf(Out+frontSurf+vertCut);
      
      frontSurf=ModelSupport::getComposite(SMap,DV," 2 ");
      DV+=100;
    }

  return;
}

void
BulletDivider::excludeInsertCells(Simulation& System)
  /*!
    Determine those exluded cells that are +/- the divide surface
    \param System :: Simulation for all cell objects
  */
{
  ELog::RegMethod RegA("BulletDivide","excludeInsertCells");

  if (radii.size()<2) return;

  const Geometry::Vec3D XDir=(divDirection) ? X : -X;
  
  std::vector<int> active;
  for(const int CN : getInsertCells())
    {
      MonteCarlo::Object* OPtr=System.findQhull(CN);
      if (OPtr)
	{
	  OPtr->populate();
	  for(const Geometry::Vec3D& testPt : endPts)
	    {
	      if( OPtr->isValid(testPt))
		{
		  active.push_back(CN);
		  break;
		}
	    }
	}
    }
  setInsertCell(active);
  return;
}

void
BulletDivider::createLinks()
  /*!
    Create links
   */
{
}
  
void
BulletDivider::createAll(Simulation& System,
			 const attachSystem::FixedComp& Inner,
			 const attachSystem::FixedComp& Vessel)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param Inner :: Plate target inner part
    \param Vessel :: Container vessel
  */
{
  ELog::RegMethod RegA("BulletDivider","createAll");

  populate(System.getDataBase());

  createUnitVector(Inner);
  setInnerRadius(Inner,3);
  createSurfaces();
  createObjects(System,Inner,3,Vessel,8,9);
  excludeInsertCells(System);
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE ts1System
