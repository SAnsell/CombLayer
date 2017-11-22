/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Engineering/FishGillVessel.cxx
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
#include "FishGillVessel.h"

namespace ts1System
{

FishGillVessel::FishGillVessel(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,12),
  pvIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(pvIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

FishGillVessel::FishGillVessel(const FishGillVessel& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  pvIndex(A.pvIndex),cellIndex(A.cellIndex),frontClear(A.frontClear),
  frontThick(A.frontThick),frontWaterThick(A.frontWaterThick),
  mainRadius(A.mainRadius),radii(A.radii),length(A.length),
  waterHeight(A.waterHeight),taVertThick(A.taVertThick),
  taRadialThick(A.taRadialThick),clearVertThick(A.clearVertThick),
  clearRadialThick(A.clearRadialThick),waterMat(A.waterMat),
  wallMat(A.wallMat),innerCells(A.innerCells)
  /*!
    Copy constructor
    \param A :: FishGillVessel to copy
  */
{}

FishGillVessel&
FishGillVessel::operator=(const FishGillVessel& A)
  /*!
    Assignment operator
    \param A :: FishGillVessel to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
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

  
FishGillVessel::~FishGillVessel() 
 /*!
   Destructor
 */
{}

void
FishGillVessel::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: Variable database
 */
{
  ELog::RegMethod RegA("FishGillVessel","populate");
  
  frontClear=Control.EvalVar<double>(keyName+"FrontClear");
  frontThick=Control.EvalVar<double>(keyName+"FrontThick");
  frontWaterThick=Control.EvalVar<double>(keyName+"FrontWaterThick");

  const size_t nRadii=
    Control.EvalVar<size_t>(keyName+"NRadii");

  for(size_t i=0;i<nRadii;i++)
    {
      const std::string Rkey=StrFunc::makeString(i);
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
FishGillVessel::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    - Y Down the beamline
    \param FC :: FixedComp for origin and axis
  */
{
  ELog::RegMethod RegA("FishGillVessel","createUnitVector");

  attachSystem::FixedComp::createUnitVector(FC);
  return;
}

void
FishGillVessel::createSurfaces()
  /*!
    Create All the surfaces
    \todo remove surface 8 with a linkSurf from inner target
  */
{
  ELog::RegMethod RegA("FishGillVessel","createSurface");

  // First layer [Bulk]
  ModelSupport::buildPlane(SMap,pvIndex+1,
			   Origin-Y*(frontClear+frontThick+frontWaterThick),Y);
  ModelSupport::buildPlane(SMap,pvIndex+11,
			   Origin-Y*(frontThick+frontWaterThick),Y);
  ModelSupport::buildPlane(SMap,pvIndex+21,
			   Origin-Y*frontWaterThick,Y);

  // This could be a copy of the exterior of Plate target
  ModelSupport::buildCylinder(SMap,pvIndex+8,Origin,Y,mainRadius);
  // clearance gap
  ModelSupport::buildCylinder(SMap,pvIndex+18,Origin,Y,
			      mainRadius+clearRadialThick);
  
  // Dividing plane
  ModelSupport::buildPlane(SMap,pvIndex+3,Origin,X);

  int PV(pvIndex);
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
  
  ModelSupport::buildPlane(SMap,pvIndex+5,
			   Origin-Z*(waterHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,pvIndex+6,
			   Origin+Z*(waterHeight/2.0),Z);

  ModelSupport::buildPlane(SMap,pvIndex+15,
			   Origin-Z*(waterHeight/2.0+taVertThick),Z);
  ModelSupport::buildPlane(SMap,pvIndex+16,
			   Origin+Z*(waterHeight/2.0+taVertThick),Z);


  ModelSupport::buildPlane(SMap,pvIndex+25,
     Origin-Z*(waterHeight/2.0+taVertThick+clearVertThick),Z);
  ModelSupport::buildPlane(SMap,pvIndex+26,
     Origin+Z*(waterHeight/2.0+taVertThick+clearVertThick),Z);

  return;
}

void
FishGillVessel::createObjects(Simulation& System,
			      const attachSystem::FixedComp& TarObj,
			      const long int frontIndex)
  /*!
    Creates the target vessel objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("FishGillVessel","createObjects");
  
  std::string Out;
  
  const std::string FFace=TarObj.getLinkString(frontIndex);
  // Front water space
  Out=ModelSupport::getComposite(SMap,pvIndex,"21 5 -6 -8 ")+FFace;
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,0.0,Out));
  
  // Void
  Out=ModelSupport::getComposite(SMap,pvIndex,"1 -11 ((-27 25 -26) : -18) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  // Wal
  Out=ModelSupport::getComposite(SMap,pvIndex,"11 -21 -17 15 -16 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,pvIndex,"21 -8 6 -16 ")+FFace;
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,pvIndex,"21 -8 -5 15 ")+FFace;
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,pvIndex,"11 -8 16 -26 ")+FFace;
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  Out=ModelSupport::getComposite(SMap,pvIndex,"11 -8 -15 25 ")+FFace;
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  Out=ModelSupport::getComposite(SMap,pvIndex,"1 -18 ")+FFace;
  addOuterUnionSurf(Out);
  
  int PV(pvIndex);

  // Note special first surface Special for first contact:
  std::string frontSurf=ModelSupport::getComposite(SMap,pvIndex," 21 ");
  const std::string overLap=ModelSupport::getComposite(SMap,pvIndex," 1 ");
  const std::string IRadius=ModelSupport::getComposite(SMap,pvIndex," 8 ");

  for(size_t i=0;i<length.size();i++)
    {
      // WATER
      Out=ModelSupport::getComposite(SMap,PV,pvIndex,"-3M -2 -7 5M -6M 8M ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,0.0,Out+frontSurf));
      innerCells.push_back(cellIndex-1);

      Out=ModelSupport::getComposite(SMap,PV,pvIndex,"3M -2 -7 5M -6M 8M ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,0.0,Out+frontSurf));
      innerCells.push_back(cellIndex-1);

      // TA layer
      Out=ModelSupport::getComposite(SMap,PV,pvIndex,
				     "-3M -2 -17 15M -16M (7: -5M :6M)  8M ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out+frontSurf));

      Out=ModelSupport::getComposite(SMap,PV,pvIndex,
				     "3M -2 -17 15M -16M (7: -5M :6M)  8M ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out+frontSurf));
      
      // Clear
      if (!i)
	frontSurf=ModelSupport::getComposite(SMap,pvIndex," 11 ");	


      Out=ModelSupport::getComposite(SMap,PV,pvIndex,
				     "-3M -2 -27 25M -26M (17: -15M :16M) 8M ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out+frontSurf));

      Out=ModelSupport::getComposite(SMap,PV,pvIndex,
				     "3M -2 -27 25M -26M (17: -15M : 16M) 8M ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out+frontSurf));

      Out=ModelSupport::getComposite(SMap,PV,pvIndex,"-2 -27 25M -26M ");
      if (i)
	addOuterUnionSurf(Out+frontSurf);
      else
	addOuterUnionSurf(Out+overLap);   // front overlap

      frontSurf=ModelSupport::getComposite(SMap,PV," 2 ");
      PV+=100;
    }

  // WRAP ROUND CLEARANCE:
  PV-=100;
  Out=ModelSupport::getComposite(SMap,pvIndex,PV,"11 -18 (-25:26) -2M");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out+IRadius));
  addOuterUnionSurf(Out);

  Out=ModelSupport::getComposite(SMap,pvIndex," 11 -8 (-25 : 26) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out+FFace));


  // BACKPLATE:
  const std::string BFace=TarObj.getLinkString(2);
  Out=ModelSupport::getComposite(SMap,pvIndex,PV," -8 -2M ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out+BFace));
  
  
  return;
}


void
FishGillVessel::createLinks()
  /*!
    Creates a full attachment set [Internal]
  */
{
  ELog::RegMethod RegA("FishGillVessel","createLinks");

  FixedComp::setConnect(0,Origin-Y*(frontClear+frontThick+frontWaterThick),-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(pvIndex+1));

  // set Links :: Inner links:
  
  FixedComp::setConnect(7,Origin-Z*(waterHeight/2.0),Z);
  FixedComp::setLinkSurf(7,SMap.realSurf(pvIndex+5));

  FixedComp::setConnect(8,Origin+Z*(waterHeight/2.0),-Z);
  FixedComp::setLinkSurf(8,-SMap.realSurf(pvIndex+6));
  return;
}



void
FishGillVessel::createAll(Simulation& System,
			  const attachSystem::FixedComp& FC,
  			  const attachSystem::FixedComp& TarObj,
			  const long int frontIndex)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: FixedComp for origin
    \param TarObj :: FixedComp for front face object
    \param frontIndex :: Front face index
  */
{
  ELog::RegMethod RegA("FishGillVessel","createAll");
  populate(System.getDataBase());

  createUnitVector(FC);
  createSurfaces();
  createObjects(System,TarObj,frontIndex);
  createLinks();
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE ts1System
