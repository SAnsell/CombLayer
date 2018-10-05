/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   balder/LeadBox.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include <array>

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
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
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
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "SpaceCut.h"
#include "ContainedSpace.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ExternalCut.h"

#include "LeadBox.h"

namespace xraySystem
{

LeadBox::LeadBox(const std::string& Key) :
  attachSystem::FixedOffset(Key,6),
  attachSystem::ContainedGroup("Main","Walls","MainWall",
			       "FrontWall","BackWall"),
  attachSystem::CellMap(),
  attachSystem::ExternalCut(),
  voidActive(1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

  
LeadBox::~LeadBox() 
  /*!
    Destructor
  */
{}

void
LeadBox::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("LeadBox","populate");
  
  FixedOffset::populate(Control);

  // Void + Fe special:
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  width=Control.EvalVar<double>(keyName+"Width");
  length=Control.EvalVar<double>(keyName+"Length");

  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  portGap=Control.EvalVar<double>(keyName+"PortGap");

  voidMat=ModelSupport::EvalDefMat<int>(Control,keyName+"VoidMat",0);
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  return;
}

void
LeadBox::createUnitVector(const attachSystem::FixedComp& FC,
			    const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("LeadBox","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  
  return;
}


void
LeadBox::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("LeadBox","createSurfaces");

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*height,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*depth,Z);

  ModelSupport::buildPlane(SMap,buildIndex+11,
			   Origin-Y*(wallThick+length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,
			   Origin+Y*(wallThick+length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+13,
			   Origin-X*(wallThick+width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(wallThick+width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(wallThick+height),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(wallThick+depth),Z);

  if (portGap>Geometry::zeroTol)
    {
      if(isActive("portCut"))
	ExternalCut::makeExpandedSurf
	  (SMap,"portCut",buildIndex+17,Origin,portGap);
      
      else if( isActive("portCutA")  && isActive("portCutB"))
	{
	  ExternalCut::makeExpandedSurf
	    (SMap,"portCutA",buildIndex+117,Origin,portGap);
	  ExternalCut::makeExpandedSurf
	    (SMap,"portCutB",buildIndex+217,Origin,portGap);
	}
      else
	{
	  ELog::EM<<"Niether portCut or portCutA/portCutB set"<<ELog::endDiag;
	}
    }
  return;
}

void
LeadBox::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("LeadBox","createObjects");

  const std::string excludeObj=getContainer("Main");
  const std::string portSurfA=getRuleStr("portCut")+getRuleStr("portCutA");
  const std::string portSurfB=getRuleStr("portCut")+getRuleStr("portCutB");;

  std::string Out;
  // Main Void
  if (voidActive)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 3 -4 5 -6 ");
      CellMap::makeCell("Void",System,cellIndex++,voidMat,0.0,Out+excludeObj);
    }
  
  // port Void

  // only 17 or 117 used
  Out=ModelSupport::getSetComposite(SMap,buildIndex,"11 -1 -117 -17 ");
  CellMap::makeCell("FrontPortVoid",System,
		    cellIndex++,voidMat,0.0,Out+portSurfA);

  // only 17 or 217 used
  Out=ModelSupport::getSetComposite(SMap,buildIndex,"2 -12 -217 -17");
  CellMap::makeCell("BackPortVoid",System,
		    cellIndex++,voidMat,0.0,Out+portSurfB);

  Out=ModelSupport::getSetComposite(SMap,buildIndex,
				 "1 -2 13 -14 15 -16 (-3:4:-5:6) " );
  CellMap::makeCell("MainWall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getSetComposite(SMap,buildIndex,
				 "11 -1  13 -14 15 -16 17 117 " );
  CellMap::makeCell("FrontWall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getSetComposite(SMap,buildIndex,
				 "2 -12  13 -14 15 -16 17 217 " );
  CellMap::makeCell("BackWall",System,cellIndex++,wallMat,0.0,Out);


  Out=ModelSupport::getComposite(SMap,buildIndex," (2 -12 ) : (11 -1 ) ");
  addOuterSurf("Walls",Out);      

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -1 13 -14 15 -16 ");
  addOuterSurf("FrontWall",Out);      

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "1 -2 13 -14 15 -16 (-3:4:-5:6) ");
  addOuterSurf("MainWall",Out);      

  Out=ModelSupport::getComposite(SMap,buildIndex," 2 -12 13 -14 15 -16 ");
  addOuterSurf("BackWall",Out);      

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 13 -14 15 -16 ");
  addOuterSurf("Main",Out);      
  return;
}

void
LeadBox::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane.
    Port position are used for first two link points
    Note that 0/1 are the flange surfaces
  */
{
  ELog::RegMethod RegA("LeadBox","createLinks");

  FixedComp::setConnect(0,Origin-Y*(wallThick+length/2.0),-Y);
  FixedComp::setConnect(1,Origin+Y*(wallThick+length/2.0),Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+11));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+12));
  

  return;
}

void
LeadBox::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("LeadBox","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);
  
  return;
}
  
}  // NAMESPACE xraySystem
