/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delft/BeamTube.cxx
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
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ContainedComp.h"
#include "BeamTube.h"

namespace delftSystem
{

BeamTube::BeamTube(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffsetGroup(Key,"Main",3,"Beam",3),
  attachSystem::CellMap(),
  flightIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(flightIndex+1),innerVoid(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

BeamTube::BeamTube(const BeamTube& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffsetGroup(A),
  attachSystem::CellMap(A),
  flightIndex(A.flightIndex),cellIndex(A.cellIndex),
  length(A.length),
  innerRadius(A.innerRadius),innerWall(A.innerWall),
  outerRadius(A.outerRadius),outerWall(A.outerWall),
  frontWall(A.frontWall),frontGap(A.frontGap),frontIWall(A.frontIWall),
  interThick(A.interThick),interFrontWall(A.interFrontWall),
  interWallThick(A.interWallThick),interYOffset(A.interYOffset),
  interMat(A.interMat),wallMat(A.wallMat),gapMat(A.gapMat),
  innerVoid(A.innerVoid)
  /*!
    Copy constructor
    \param A :: BeamTube to copy
  */
{}

BeamTube&
BeamTube::operator=(const BeamTube& A)
  /*!
    Assignment operator
    \param A :: BeamTube to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffsetGroup::operator=(A);
      attachSystem::CellMap::operator=(A);
      cellIndex=A.cellIndex;
      length=A.length;
      innerRadius=A.innerRadius;
      innerWall=A.innerWall;
      outerRadius=A.outerRadius;
      outerWall=A.outerWall;
      frontWall=A.frontWall;
      frontGap=A.frontGap;
      frontIWall=A.frontIWall;
      interThick=A.interThick;
      interFrontWall=A.interFrontWall;
      interWallThick=A.interWallThick;
      interYOffset=A.interYOffset;
      innerMat=A.innerMat;
      interMat=A.interMat;
      wallMat=A.wallMat;
      gapMat=A.gapMat;
      innerVoid=A.innerVoid;
    }
  return *this;
}

BeamTube::~BeamTube() 
 /*!
   Destructor
 */
{}

void
BeamTube::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: Database to used
 */
{
  ELog::RegMethod RegA("BeamTube","populate");

  attachSystem::FixedOffsetGroup::populate(Control);
  waterStep=Control.EvalVar<double>(keyName+"WaterStep");
  innerStep=Control.EvalDefVar<double>(keyName+"InnerStep",0.0);

  length=Control.EvalVar<double>(keyName+"Length");
  capRadius=Control.EvalDefVar<double>(keyName+"CapRadius",0.0);
  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");
  innerWall=Control.EvalVar<double>(keyName+"InnerWall");
  outerRadius=Control.EvalVar<double>(keyName+"OuterRadius");
  outerWall=Control.EvalVar<double>(keyName+"OuterWall");

  frontWall=Control.EvalVar<double>(keyName+"FrontWall");
  frontGap=Control.EvalVar<double>(keyName+"FrontGap");
  frontIWall=Control.EvalVar<double>(keyName+"FrontIWall");

  interThick=Control.EvalVar<double>(keyName+"InterThick");
  interWallThick=Control.EvalVar<double>(keyName+"InterWallThick");
  interYOffset=Control.EvalVar<double>(keyName+"InterYOffset");
  interFrontWall=Control.EvalVar<double>(keyName+"InterFrontWall");

  innerMat=ModelSupport::EvalMat<int>(Control,keyName+"InnerMat");
  interMat=ModelSupport::EvalMat<int>(Control,keyName+"InterMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  gapMat=ModelSupport::EvalMat<int>(Control,keyName+"GapMat");

  populatePortals(Control);

  return;
}
  
void
BeamTube::populatePortals(const FuncDataBase& Control)
  /*!
    Populate the tally potals [if needed]
    \param Control :: FuncDataBase
   */
{
  ELog::RegMethod RegA("BeamTube","populatePortals");

  const int portalActive=Control.EvalDefVar<int>(keyName+"PortalActive",0);
  if (!portalActive) return;

  portalOffset.clear();
  portalMat.clear();

  portalRadius=Control.EvalDefVar<double>(keyName+"PortalRadius",0.0);
  portalThick=Control.EvalDefVar<double>(keyName+"PortalThick",100.0);

  double pD;
  int pM;
  int index(1);
  do
    {
      const std::string PKey=keyName+"Portal"+StrFunc::makeString(index);
      pM=ModelSupport::EvalDefMat<int>(Control,PKey+"Mat",-1);
      pD=Control.EvalDefVar<double>(PKey+"Dist",0.0);
      if (pM>=0)
	{
	  portalOffset.push_back(pD);
	  portalMat.push_back(pM);
	}
      index++;
    } while(pM>=0);
  
  return;
}

void
BeamTube::createUnitVector(const attachSystem::FixedComp& FC,
			   const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: A Contained FixedComp to use as basis set
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("BeamTube","createUnitVector");

  attachSystem::FixedComp& mainFC=getKey("Main");
  attachSystem::FixedComp& beamFC=getKey("Beam");

  beamFC.createUnitVector(FC,sideIndex);
  mainFC.createUnitVector(FC,sideIndex);

  xStep+=waterStep;
  applyOffset();
   
  setDefault("Main");
  return;
}

void
BeamTube::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("BeamTube","createSurfaces");


  //  const attachSystem::FixedComp& mainFC=getKey("Main");
  const attachSystem::FixedComp& beamFC=getKey("Beam");

  const Geometry::Vec3D bY(beamFC.getY());
  // Outer layers
  if (capRadius>Geometry::zeroTol)
    {
      ModelSupport::buildPlane(SMap,flightIndex+1,Origin+bY*capRadius,Y);
      ModelSupport::buildSphere(SMap,flightIndex+8,
				Origin+bY*capRadius,capRadius);
      ModelSupport::buildSphere(SMap,flightIndex+18,
				Origin+bY*(capRadius+frontWall),
				capRadius);
      // gap layer
      ModelSupport::buildSphere(SMap,flightIndex+28,
				Origin+bY*(capRadius+frontWall+frontGap),
				capRadius);
      ModelSupport::buildSphere(SMap,flightIndex+38,Origin+
				bY*(capRadius+frontWall+frontGap+frontIWall),
				capRadius);
    }
  else
    {
      ModelSupport::buildPlane(SMap,flightIndex+1,Origin,Y);
      ModelSupport::buildPlane(SMap,flightIndex+11,Origin+Y*frontWall,Y);
      ModelSupport::buildPlane(SMap,flightIndex+21,
			       Origin+Y*frontWall+bY*frontGap,bY);
      ModelSupport::buildPlane(SMap,flightIndex+31,Origin+Y*frontWall+
			       bY*(frontGap+frontIWall),bY);
      ModelSupport::buildPlane(SMap,flightIndex+1001,Origin+Y*innerStep,Y);
    }

  
  ModelSupport::buildCylinder(SMap,flightIndex+7,
			      Origin+Y*frontWall,bY,outerRadius+outerWall);
  ModelSupport::buildPlane(SMap,flightIndex+2,Origin+bY*length,bY);


  // Outer Wall

  ModelSupport::buildCylinder(SMap,flightIndex+17,Origin+Y*frontWall,
			      bY,outerRadius);

  // PORTALTS RELATIVE TO FRONT WALL CENTRE:
  int PN(flightIndex+100);
  for(size_t i=0;i<portalMat.size();i++)
    {
      ModelSupport::buildCylinder(SMap,PN+17,Origin+Y*frontWall,
				  bY,portalRadius);
      const Geometry::Vec3D YVec((i) ? bY : Y);
      ModelSupport::buildPlane(SMap,PN+11,
			       Origin+Y*(frontWall-portalThick)/2.0+
			       bY*portalOffset[i],YVec);
      ModelSupport::buildPlane(SMap,PN+12,
			       Origin+Y*(frontWall+portalThick)/2.0+
			       bY*portalOffset[i],YVec);
      PN+=100;
    }

  // Gap layer
  ModelSupport::buildCylinder(SMap,flightIndex+27,
			      Origin+Y*frontWall,
			      bY,innerRadius+innerWall);

  // Gap layer
  ModelSupport::buildCylinder(SMap,flightIndex+37,
			      Origin+Y*frontWall,
			      bY,innerRadius);

  ModelSupport::buildPlane(SMap,flightIndex+41,
			   Origin+bY*interYOffset,bY);
  ModelSupport::buildPlane(SMap,flightIndex+51,
			   Origin+bY*(interYOffset+interFrontWall),bY);

  // Inter layer
  // Layers [wall : Mid : Outer ]
  ModelSupport::buildCylinder(SMap,flightIndex+47,
			      Origin+Y*frontWall,bY,
			      outerRadius-interWallThick);
  ModelSupport::buildCylinder(SMap,flightIndex+57,
			      Origin+Y*frontWall,bY,
			      outerRadius-(interWallThick+interThick));
  ModelSupport::buildCylinder(SMap,flightIndex+67,
			      Origin+Y*frontWall,bY,
			      outerRadius-(2.0*interWallThick+interThick));

  return;
}

void
BeamTube::createCapEndObjects(Simulation& System)
  /*!
    Adds the BeamLne components with a cap end
    \param System :: Simulation to add beamline to
  */
{
  ELog::RegMethod RegA("BeamTube","createCapEndObjects");

  std::string Out;
  Out=ModelSupport::getComposite(SMap,flightIndex," (-8:1) -2 -7 ");
  addOuterSurf(Out);

  Out+=ModelSupport::getComposite(SMap,flightIndex," ((18 -1) : 17)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));

  // Void (exclude inter wall)
  Out=ModelSupport::getComposite(SMap,flightIndex,
				 " (-18:1) -2 -17  ((28 -1) : 27) (-41:-67)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,gapMat,0.0,Out));
  // Inner wall
  
  Out=ModelSupport::getComposite(SMap,flightIndex,
				 " (-28:1) ((38 -1) : 37) -2 -27 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  
  // Inner Void
  Out=ModelSupport::getComposite(SMap,flightIndex," (-38:1)  -2 -37 ");

  // Second portal:
  // Remaining portals in inner vioid
  std::string OutComposite;
  int PN(flightIndex);
  for(size_t i=0;i<portalMat.size();i++)
    {
      PN+=100;
      if (portalOffset[i]>Geometry::zeroTol)
	{
	  const std::string OutP=
	    ModelSupport::getComposite(SMap,PN," -17 11 -12");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,portalMat[i],0.0,OutP));
	  OutComposite +=ModelSupport::getComposite(SMap,PN," (17:-11:12)");
	  addCell("Portal"+std::to_string(i),cellIndex-1);
	}
    }
  System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,0.0,Out+OutComposite));
  addCell("innerVoid",cellIndex-1);
  innerVoid=cellIndex-1;

  // Inter wall
  Out=ModelSupport::getComposite(SMap,flightIndex," -47 57 51 -2 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,interMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,flightIndex,
				 "41 -17 67 (47:-57:-51) -2 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));

  return;
}

void
BeamTube::createObjects(Simulation& System)
  /*!
    Adds the BeamLne components
    \param System :: Simulation to add beamline to
  */
{
  ELog::RegMethod RegA("BeamTube","createObjects");
  
  std::string Out;
  Out=ModelSupport::getComposite(SMap,flightIndex," 1 -2 -7 ");
  addOuterSurf(Out);

  Out=ModelSupport::getComposite(SMap,flightIndex," 1 -2 -7 (-11:17)");
  // PORTALTS RELATIVE TO FRONT WALL CENTRE:
  // First one can be in the wall that makes things difficult
  if (!portalMat.empty() && portalOffset[0]<Geometry::zeroTol)
    {
      const std::string OutP=
	ModelSupport::getComposite(SMap,flightIndex," -117 111 -112");
      System.addCell(MonteCarlo::Qhull(cellIndex++,portalMat[0],0.0,OutP));
      Out+=ModelSupport::getComposite(SMap,flightIndex," (117:-111:112)"); 
    }
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));

  // Void (exclude inter wall)
  Out=ModelSupport::getComposite(SMap,flightIndex,
				 "11 -2 -17 (-21:27) (-41:-67)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,gapMat,0.0,Out));
  addCell("FrontVoid",cellIndex-1);
  
  // Inner wall
  Out=ModelSupport::getComposite(SMap,flightIndex," 21 -2 -27 (-31:37)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  
  // Inner Void
  Out=ModelSupport::getComposite(SMap,flightIndex," 31 -2 -37 ");
  
  // Second portal:
  // Remaining portals in inner vioid
  std::string OutComposite;
  int PN(flightIndex);
  for(size_t i=0;i<portalMat.size();i++)
    {
      PN+=100;
      if (portalOffset[i]>Geometry::zeroTol)
	{
	  const std::string OutP=
	    ModelSupport::getComposite(SMap,PN," -17 11 -12");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,portalMat[i],0.0,OutP));
	  addCell("Portal"+std::to_string(i),cellIndex-1);
	  OutComposite +=ModelSupport::getComposite(SMap,PN," (17:-11:12)"); 
	}
    }

  System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,0.0,Out+OutComposite));
  addCell("innerVoid",cellIndex-1);
  innerVoid=cellIndex-1;
  // Inter wall
  Out=ModelSupport::getComposite(SMap,flightIndex," -47 57 51 -2 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,interMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,flightIndex,
				 "41 -17 67 (47:-57:-51) -2 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  
  return;
}

void
BeamTube::createLinks()
  /*!
    Create All the links:
    - 0 : First surface
    - 1 : Exit surface
    - 2 : Inner face
  */
{
  ELog::RegMethod RegA("BeamTube","createLinks");

  attachSystem::FixedComp& mainFC=getKey("Main");
  attachSystem::FixedComp& beamFC=getKey("Beam");

  const Geometry::Vec3D bY(beamFC.getY());
  beamFC.setConnect(0,Origin,bY);
  beamFC.setConnect(1,Origin+bY*length,bY);


  mainFC.setConnect(0,Origin,-Y);      // Note always to the reactor
  mainFC.setConnect(1,Origin+Y*frontWall,Y);
  mainFC.setConnect(2,Origin+Y*frontWall+bY*frontGap,bY);

  if (capRadius>Geometry::zeroTol)
    {
      mainFC.setLinkSurf(0,SMap.realSurf(flightIndex+8));
      mainFC.addBridgeSurf(0,-SMap.realSurf(flightIndex+1));
    }
  else
    mainFC.setLinkSurf(0,-SMap.realSurf(flightIndex+1));

  mainFC.setLinkSurf(1,SMap.realSurf(flightIndex+2));
  mainFC.setLinkSurf(2,SMap.realSurf(flightIndex+21));

  beamFC.setLinkSurf(0,-SMap.realSurf(flightIndex+1));
  beamFC.setLinkSurf(1,SMap.realSurf(flightIndex+2));
  beamFC.setLinkSurf(2,SMap.realSurf(flightIndex+21));

  return;
}

void
BeamTube::createAll(Simulation& System,const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Global creation of the vac-vessel
    \param System :: Simulation to add vessel to
    \param FC :: Moderator Object
    \param sideIndex :: Link point
    \param PAxis :: directional axis
  */
{
  ELog::RegMethod RegA("BeamTube","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  if (capRadius>Geometry::zeroTol)
    createCapEndObjects(System);
  else
    createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE delftSystem
