/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Engineering/BulletVessel.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell/Goran Skoro
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
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "SimProcess.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "BulletVessel.h"

namespace ts1System
{

BulletVessel::BulletVessel(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,12)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

BulletVessel::BulletVessel(const BulletVessel& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  frontClear(A.frontClear),frontThick(A.frontThick),
  frontWaterThick(A.frontWaterThick),mainRadius(A.mainRadius),
  radii(A.radii),length(A.length),waterHeight(A.waterHeight),
  taVertThick(A.taVertThick),taRadialThick(A.taRadialThick),
  clearVertThick(A.clearVertThick),clearRadialThick(A.clearRadialThick),
  waterMat(A.waterMat),wallMat(A.wallMat),innerCells(A.innerCells)
  /*!
    Copy constructor
    \param A :: BulletVessel to copy
  */
{}

BulletVessel&
BulletVessel::operator=(const BulletVessel& A)
  /*!
    Assignment operator
    \param A :: BulletVessel to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      frontClear=A.frontClear;
      frontThick=A.frontThick;
      frontWaterThick=A.frontWaterThick;
      mainRadius=A.mainRadius;
      radii=A.radii;
      length=A.length;
      waterHeight=A.waterHeight;
      taVertThick=A.taVertThick;
      taRadialThick=A.taRadialThick;
      clearVertThick=A.clearVertThick;
      clearRadialThick=A.clearRadialThick;
      waterMat=A.waterMat;
      wallMat=A.wallMat;
      innerCells=A.innerCells;
    }
  return *this;
}

  
BulletVessel::~BulletVessel() 
 /*!
   Destructor
 */
{}

void
BulletVessel::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: Variable database
 */
{
  ELog::RegMethod RegA("BulletVessel","populate");
  
  frontClear=Control.EvalVar<double>(keyName+"FrontClear");
  frontThick=Control.EvalVar<double>(keyName+"FrontThick");
  frontWaterThick=Control.EvalVar<double>(keyName+"FrontWaterThick");

  mainRadius=Control.EvalVar<double>(keyName+"MainRadius");  

  const size_t nRadii=
    Control.EvalVar<size_t>(keyName+"NRadii");

  for(size_t i=0;i<nRadii;i++)
    {
      const std::string Rkey=std::to_string(i);
      const double R=Control.EvalPair<double>(keyName+"Radius"+Rkey,
					      keyName+"Radius");
      radii.push_back(R);
      if (i+1<nRadii)
	{
	  const double L=Control.EvalPair<double>(keyName+"Length"+Rkey,
						  keyName+"Length");
	  length.push_back(L);
	}
    }
  
  waterHeight=Control.EvalVar<double>(keyName+"WaterHeight");
  taVertThick=Control.EvalVar<double>(keyName+"TaVertThick");
  taRadialThick=Control.EvalVar<double>(keyName+"TaRadialThick");

  clearVertThick=Control.EvalVar<double>(keyName+"ClearVertThick");
  clearRadialThick=Control.EvalVar<double>(keyName+"ClearRadialThick");
    // Material
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  waterMat=ModelSupport::EvalMat<int>(Control,keyName+"WaterMat");

  return;
}
  
void
BulletVessel::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    - Y Down the beamline
    \param FC :: FixedComp for origin and axis
  */
{
  ELog::RegMethod RegA("BulletVessel","createUnitVector");

  attachSystem::FixedComp::createUnitVector(FC);
  return;
}

void
BulletVessel::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("BulletVessel","createSurface");

  // First layer [Bulk]
  ModelSupport::buildPlane(SMap,buildIndex+1,
			   Origin-Y*(frontClear+frontThick+frontWaterThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+11,
			   Origin-Y*(frontThick+frontWaterThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+21,
			   Origin-Y*frontWaterThick,Y);

  ModelSupport::buildCylinder(SMap,buildIndex+8,Origin,Y,mainRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+18,Origin,Y,
			      mainRadius+taRadialThick);
  ModelSupport::buildCylinder(SMap,buildIndex+28,Origin,Y,
			      mainRadius+taRadialThick+clearRadialThick);
  
  // Dividing plane
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin,X);

  int PV(buildIndex);
  Geometry::Vec3D CPt(Origin);
  for(size_t i=0;i<length.size();i++)
    {
      if (fabs(radii[i]-radii[i+1])<Geometry::zeroTol)
	{
	  ModelSupport::buildCylinder(SMap,PV+7,Origin,Y,radii[i]);
	  ModelSupport::buildCylinder(SMap,PV+17,Origin,Y,
				   radii[i]+taRadialThick);
	  ModelSupport::buildCylinder(SMap,PV+27,Origin,Y,
				   radii[i]+taRadialThick+clearRadialThick);
	}
      else
	{
	  // using similar triangles:
	  const double x=radii[i]*length[i]/(radii[i+1]-radii[i]);
	  const double tAngle=fabs(radii[i+1]-radii[i])/length[i];
	  const double theta=180.0/M_PI*atan(tAngle);	  	  
	  ELog::EM<<"X == "<<x<<" "<<tAngle<<" "<<theta<<ELog::endDiag;
	  // Inner
	  ModelSupport::buildCone(SMap,PV+7,CPt-Y*x,Y,theta);
	  // Wall
	  ModelSupport::buildCone(SMap,PV+17,
				  CPt-Y*(x+taRadialThick/tAngle),Y,theta);
	  ModelSupport::buildCone(SMap,PV+27,
	     CPt-Y*(x+(taRadialThick+clearRadialThick)/tAngle),Y,theta);  
	}
      CPt+=Y*length[i];
      ModelSupport::buildPlane(SMap,PV+2,CPt,Y);
      PV+=100;
    }
  
  ModelSupport::buildPlane(SMap,buildIndex+5,
			   Origin-Z*(waterHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,
			   Origin+Z*(waterHeight/2.0),Z);

  ModelSupport::buildPlane(SMap,buildIndex+15,
			   Origin-Z*(waterHeight/2.0+taVertThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,
			   Origin+Z*(waterHeight/2.0+taVertThick),Z);


  ModelSupport::buildPlane(SMap,buildIndex+25,
     Origin-Z*(waterHeight/2.0+taVertThick+clearVertThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+26,
     Origin+Z*(waterHeight/2.0+taVertThick+clearVertThick),Z);

  return;
}

void
BulletVessel::createObjects(Simulation& System)
  /*!
    Creates the target vessel objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("BulletVessel","createObjects");
  
  std::string Out;

  // FRONT UNITS:
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -11 ((-27 25 -26) : -28) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  Out=ModelSupport::getComposite(SMap,buildIndex,"11 -21 ((-17 15 -16) : -18) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));

  
  int PV(buildIndex);

  // Note special first surface Special for first contact:
  std::string frontSurf=ModelSupport::getComposite(SMap,buildIndex," 21 ");
  const std::string overLap=ModelSupport::getComposite(SMap,buildIndex," 1 ");

  for(size_t i=0;i<length.size();i++)
    {
      // WATER
      Out=ModelSupport::getComposite(SMap,PV,buildIndex,
				     " -2 -3M ((-7 5M -6M) : -8M)");
      System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,0.0,Out+frontSurf));
      innerCells.push_back(cellIndex-1);
      
      Out=ModelSupport::getComposite(SMap,PV,buildIndex,
				     "-2  3M ((-7 5M -6M) : -8M)");
      System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,0.0,Out+frontSurf));
      innerCells.push_back(cellIndex-1);
      // TA 
      Out=ModelSupport::getComposite(SMap,PV,buildIndex,
			  "-2 -3M ((-17 15M -16M) : -18M) (7:-5M:6M) 8M");
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out+frontSurf));
      Out=ModelSupport::getComposite(SMap,PV,buildIndex,
			   "-2 3M ((-17 15M -16M) : -18M) (7:-5M:6M) 8M");
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out+frontSurf));
      
      // Clear
      if (!i)
	frontSurf=ModelSupport::getComposite(SMap,buildIndex," 11 ");	

      Out=ModelSupport::getComposite(SMap,PV,buildIndex,
			 "-2 -3M ((-27 25M -26M) : -28M) (17:-15M:16M) 18M");
      System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out+frontSurf));
      Out=ModelSupport::getComposite(SMap,PV,buildIndex,
     			 "-2 3M ((-27 25M -26M) : -28M) (17:-15M:16M) 18M");
      System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out+frontSurf));

      Out=ModelSupport::getComposite(SMap,PV,buildIndex,
				     "-2 ((-27 25M -26M) : -28M)");
      if (i)
	addOuterUnionSurf(Out+frontSurf);
      else
	addOuterUnionSurf(Out+overLap);   // front overlap

      frontSurf=ModelSupport::getComposite(SMap,PV," 2 ");
      PV+=100;
    }

  return;
}


void
BulletVessel::createLinks()
  /*!
    Creates a full attachment set [Internal]
  */
{
  ELog::RegMethod RegA("BulletVessel","createLinks");
  
  // set Links :: Inner links:
  
  FixedComp::setConnect(7,Origin-Z*(waterHeight/2.0),Z);
  FixedComp::setLinkSurf(7,SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(8,Origin+Z*(waterHeight/2.0),-Z);
  FixedComp::setLinkSurf(8,-SMap.realSurf(buildIndex+6));
  return;
}



void
BulletVessel::createAll(Simulation& System,
			const attachSystem::FixedComp& FC)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: FixedComp for origin
  */
{
  ELog::RegMethod RegA("BulletVessel","createAll");
  populate(System.getDataBase());

  createUnitVector(FC);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE ts1System
