/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/HighBay.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "Bunker.h"
#include "HighBay.h"


namespace essSystem
{

HighBay::HighBay(const std::string& key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedComp(key,6),
  attachSystem::CellMap(),attachSystem::SurfMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param key :: Name of component
  */
{}

HighBay::HighBay(const HighBay& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  attachSystem::CellMap(A),attachSystem::SurfMap(A),
  baseName(A.baseName),length(A.length),height(A.height),roofThick(A.roofThick),
  wallMat(A.wallMat),roofMat(A.roofMat),curtainCut(A.curtainCut)
  /*!
    Copy constructor
    \param A :: HighBay to copy
  */
{}

HighBay&
HighBay::operator=(const HighBay& A)
  /*!
    Assignment operator
    \param A :: HighBay to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      length=A.length;
      height=A.height;
      roofThick=A.roofThick;
      wallMat=A.wallMat;
      roofMat=A.roofMat;
      curtainCut=A.curtainCut;
    }
  return *this;
}

HighBay::~HighBay() 
  /*!
    Destructor
  */
{}
  
void
HighBay::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("HighBay","populate");
  

  length=Control.EvalVar<double>(keyName+"Length");
  height=Control.EvalVar<double>(keyName+"Height");
  roofThick=Control.EvalVar<double>(keyName+"Thick");

  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  roofMat=ModelSupport::EvalMat<int>(Control,keyName+"RoofMat");
  
  return;
}
  
void
HighBay::createUnitVector(const attachSystem::FixedComp& FC,
			  const long int sideIndex)
/*!
    Create the unit vectors
    \param FC :: Linked object (bunker )
    \param sideIndex :: Side for linkage centre (wall)
  */
{
  ELog::RegMethod RegA("HighBay","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  return;
}
  
void
HighBay::createSurfaces(const Bunker& leftBunker,
			const Bunker& rightBunker)
  /*!
    Create all the surfaces
    \param leftBunker :: left bunker unit
    \param rightBunker :: right bunker unit
   */
{
  ELog::RegMethod RegA("HighBay","createSurface");

  const int leftSurfRoof=leftBunker.getSurf("roofOuter");
  
  ModelSupport::buildShiftedPlane(SMap,buildIndex+6,
				  SMap.realPtr<Geometry::Plane>(leftSurfRoof),
				  height);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+16,
				  SMap.realPtr<Geometry::Plane>(leftSurfRoof),
				  height+roofThick);
  
  const int rightSurfRoof=rightBunker.getSurf("roofOuter");
  if (rightSurfRoof!=leftSurfRoof)
    {
      ModelSupport::buildShiftedPlane
	(SMap,buildIndex+106,SMap.realPtr<Geometry::Plane>(rightSurfRoof),
	 height);
      
      ModelSupport::buildShiftedPlane
	(SMap,buildIndex+116,SMap.realPtr<Geometry::Plane>(rightSurfRoof),
	 height+roofThick);
    }

  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length,Y);  
  return;
}

void
HighBay::createObjects(Simulation& System,
		       const Bunker& leftBunker,
		       const Bunker& rightBunker)
  /*!
    Create all the objects
    \param System :: Simulation
    \param leftBunker :: left bunker unit    
    \param rightBunker :: left bunker unit
  */
{
  ELog::RegMethod RegA("HighBay","createObjects");
  std::string Out;


  const std::string frontCut=leftBunker.getLinkString(3);

  const HeadRule leftWallInner(leftBunker.getSurfRules("leftWallInner"));
  const HeadRule rightWallInner(rightBunker.getSurfRules("-rightWallInner"));
  const HeadRule leftWallOuter(leftBunker.getSurfRules("leftWallOuter"));
  const HeadRule rightWallOuter(rightBunker.getSurfRules("-rightWallOuter"));
  const HeadRule bunkerTop=leftBunker.getSurfRules("roofOuter");

  // void area
  Out=ModelSupport::getComposite(SMap,buildIndex," -2 -6 ");
  Out+=leftWallInner.display()+rightWallInner.display();
  Out+=frontCut;
  Out+=bunkerTop.display();
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,
				   Out+curtainCut.complement().display()));

  // Roof area
  Out=ModelSupport::getComposite(SMap,buildIndex," -2 6 -16 ");
  Out+=leftWallInner.display()+rightWallInner.display();
  Out+=frontCut;
  System.addCell(MonteCarlo::Object(cellIndex++,roofMat,0.0,Out));

  // Left wall
  Out=ModelSupport::getComposite(SMap,buildIndex," -2 -16 ");
  Out+=leftWallOuter.display()+leftWallInner.complement().display();
  Out+=bunkerTop.display();
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out+frontCut));
  // Right wall
  Out=ModelSupport::getComposite(SMap,buildIndex," -2 -16 ");
  Out+=rightWallOuter.display()+rightWallInner.complement().display();
  Out+=bunkerTop.display();
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out+frontCut));
		 
  
  Out=ModelSupport::getComposite(SMap,buildIndex," -2 -16 ");
  Out+=leftWallOuter.display()+rightWallOuter.display();
  Out+=frontCut;
  Out+=bunkerTop.display();
  addOuterSurf(Out);

  return;
}
  
void
HighBay::buildAll(Simulation& System,
		  const Bunker& leftBunker,
		  const Bunker& rightBunker)
  
  /*!
    Generic function to initialize everything
    \param Control :: DataBase for variables
    \param leftBunker :: Left bunker unit
    \param rightBunker :: right bunker Unit
  */
{
  ELog::RegMethod RegA("HighBay","createAll");

  populate(System.getDataBase());  
  createUnitVector(leftBunker,0);
  createSurfaces(leftBunker,rightBunker);
  createObjects(System,leftBunker,rightBunker);
  insertObjects(System);              

  return;
}

}  // NAMESPACE essSystem
