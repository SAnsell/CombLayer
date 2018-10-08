/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   maxpeem/SquareFMask.cxx
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
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "SquareFMask.h"


namespace xraySystem
{

SquareFMask::SquareFMask(const std::string& Key) :
  attachSystem::ContainedSpace(),attachSystem::FixedOffset(Key,6),
  attachSystem::CellMap(),attachSystem::SurfMap()
  /*!
    Default constructor
    \param Key :: Key name for variables
  */
{}
  

void
SquareFMask::populate(const FuncDataBase& Control)
  /*!
    Sets the size of the object
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("SquareFMask","populate");

  FixedOffset::populate(Control);
  
  height=Control.EvalVar<double>(keyName+"Height");
  width=Control.EvalVar<double>(keyName+"Width");
  length=Control.EvalVar<double>(keyName+"Length");

  minLength=Control.EvalVar<double>(keyName+"MinLength");
  innerAWidth=Control.EvalVar<double>(keyName+"innerAWidth");
  innerMinWidth=Control.EvalVar<double>(keyName+"innerMinWidth");
  innerBWidth=Control.EvalVar<double>(keyName+"innerBWidth");
  innerAHeight=Control.EvalVar<double>(keyName+"innerAHeight");
  innerMinHeight=Control.EvalVar<double>(keyName+"innerMinHeight");
  innerBHeight=Control.EvalVar<double>(keyName+"innerBHeight");

  flangeAInRadius=Control.EvalVar<double>(keyName+"FlangeFrontInRadius");
  flangeAOutRadius=Control.EvalVar<double>(keyName+"FlangeFrontOutRadius");
  flangeALength=Control.EvalVar<double>(keyName+"FlangeFrontLength");

  flangeBInRadius=Control.EvalVar<double>(keyName+"FlangeBackInRadius");
  flangeBOutRadius=Control.EvalVar<double>(keyName+"FlangeBackOutRadius");
  flangeBLength=Control.EvalVar<double>(keyName+"FlangeBackLength");
  
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  voidMat=ModelSupport::EvalDefMat<int>(Control,keyName+"VoidMat",0);
  flangeMat=ModelSupport::EvalMat<int>(Control,keyName+"FlangeMat");
  
  return;
}

void
SquareFMask::createUnitVector(const attachSystem::FixedComp& FC,
				 const long int sideIndex)
  /*!
    Create the unit vectors: Note only to construct front/back surf
    \param FC :: Centre point
    \param sideIndex :: Side index
  */
{
  ELog::RegMethod RegA("SquareFMask","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  Origin+=Y*(length/2.0); 
  return;
}


void
SquareFMask::createSurfaces()
  /*!
    Create All the surfaces
   */
{
  ELog::RegMethod RegA("SquareFMask","createSurface");

  const Geometry::Vec3D APt(Origin-Y*(length/2.0));
  const Geometry::Vec3D MPt(Origin+Y*(minLength-length/2.0));
  const Geometry::Vec3D BPt(Origin+Y*(length/2.0));

  ModelSupport::buildPlane(SMap,buildIndex+1,APt,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,BPt,Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

 
  ModelSupport::buildPlane(SMap,buildIndex+11,APt+Y*flangeALength,Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,BPt-Y*flangeBLength,Y);

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,flangeAInRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,flangeAOutRadius);

  ModelSupport::buildCylinder(SMap,buildIndex+8,Origin,Y,flangeBInRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+18,Origin,Y,flangeBOutRadius);

  // Inner Structure
  ModelSupport::buildPlane(SMap,buildIndex+101,MPt,Y);
  
  const double AH2(innerAHeight/2.0);
  const double MH2(innerMinHeight/2.0);
  const double BH2(innerBHeight/2.0);
  const double AW2(innerAWidth/2.0);
  const double MW2(innerMinWidth/2.0);
  const double BW2(innerBWidth/2.0);
  
  ModelSupport::buildPlane(SMap,buildIndex+103,
			   APt-X*AW2-Z*AH2,
			   APt-X*AW2+Z*AH2,
			   MPt-X*MW2+Z*MH2,X);
  ModelSupport::buildPlane(SMap,buildIndex+104,
			   APt+X*AW2-Z*AH2,
			   APt+X*AW2+Z*AH2,
			   MPt+X*MW2+Z*MH2,X);
  ModelSupport::buildPlane(SMap,buildIndex+105,
			   APt-X*AW2-Z*AH2,
			   APt+X*AW2-Z*AH2,
			   MPt+X*MW2-Z*MH2,Z);
  ModelSupport::buildPlane(SMap,buildIndex+106,
			   APt-X*AW2+Z*AH2,
			   APt+X*AW2+Z*AH2,
			   MPt+X*MW2+Z*MH2,Z);

  ModelSupport::buildPlane(SMap,buildIndex+203,
			   BPt-X*BW2-Z*BH2,
			   BPt-X*BW2+Z*BH2,
			   MPt-X*MW2+Z*MH2,X);
  ModelSupport::buildPlane(SMap,buildIndex+204,
			   BPt+X*BW2-Z*BH2,
			   BPt+X*BW2+Z*BH2,
			   MPt+X*MW2+Z*MH2,X);
  ModelSupport::buildPlane(SMap,buildIndex+205,
			   BPt-X*BW2-Z*BH2,
			   BPt+X*BW2-Z*BH2,
			   MPt+X*MW2-Z*MH2,Z);
  ModelSupport::buildPlane(SMap,buildIndex+206,
			   BPt-X*BW2+Z*BH2,
			   BPt+X*BW2+Z*BH2,
			   MPt+X*MW2+Z*MH2,Z);
  
  return;
}

void
SquareFMask::createObjects(Simulation& System) 
  /*!
    Creates the colllimator block
    \param System :: Simuation for object
  */
{
  ELog::RegMethod RegA("SquareFMask","createObjects");
  std::string Out;

  // inner voids
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -101 103 -104 105 -106");
  CellMap::makeCell("Void",System,cellIndex++,voidMat,0.0,Out);
  
  Out=ModelSupport::getComposite(SMap,buildIndex,"101 -2 203 -204 205 -206");
  CellMap::makeCell("Void",System,cellIndex++,voidMat,0.0,Out);

  // metal [ inner section]
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 11 -101 3 -4 5 -6 (-103:104:-105:106) ");
  CellMap::makeCell("FrontColl",System,cellIndex++,mat,0.0,Out);
  
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 101 -12 3 -4 5 -6 (-203:204:-205:206) ");
  CellMap::makeCell("BackColl",System,cellIndex++,mat,0.0,Out);

  // Front flange:

  Out=ModelSupport::getComposite 
    (SMap,buildIndex,"1 -11 7 -17 ");
  CellMap::makeCell("FrontFlange",System,cellIndex++,flangeMat,0.0,Out);
  
  Out=ModelSupport::getComposite 
    (SMap,buildIndex,"1 -11  -7 (-103:104:-105:106) ");
  CellMap::makeCell("FrontFlangeMid",System,cellIndex++,mat,0.0,Out);

  Out=ModelSupport::getComposite 
    (SMap,buildIndex,"-2 12 8 -18 ");
  CellMap::makeCell("BackFlange",System,cellIndex++,flangeMat,0.0,Out);
  
  Out=ModelSupport::getComposite 
    (SMap,buildIndex,"-2 12 -8 (-203:204:-205:206) ");
  CellMap::makeCell("BackFlangeMid",System,cellIndex++,mat,0.0,Out);

  if (flangeAOutRadius>=flangeBOutRadius)
    {
      Out=ModelSupport::getComposite
	(SMap,buildIndex,"11 -12 -17 (-3:4:-5:6) ");
      CellMap::makeCell("OutVoid",System,cellIndex++,0,0.0,Out);
      if (flangeAOutRadius>flangeBOutRadius+Geometry::zeroTol)
	{
	  Out=ModelSupport::getComposite(SMap,buildIndex,"12 -2 -17 18");
	  CellMap::makeCell("OutVoid",System,cellIndex++,0,0.0,Out);
	}
      Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 -17");
    }
  else
    {
      Out=ModelSupport::getComposite
	(SMap,buildIndex,"11 -12 -18 (-3:4:-5:6) ");
      CellMap::makeCell("OutVoid",System,cellIndex++,0,0.0,Out);
      if (flangeBOutRadius>flangeAOutRadius+Geometry::zeroTol)
	{
	  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -11 -18 17");
	  CellMap::makeCell("OutVoid",System,cellIndex++,0,0.0,Out);
	}
      Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 -18");
    }
  addOuterSurf(Out);
  return;
}

void
SquareFMask::createLinks()
  /*!
    Construct the links for the system
  */
{
  ELog::RegMethod RegA("SquareFMask","createLinks");

  FixedComp::setConnect(0,Origin-Y*(length/2.0),-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  FixedComp::setConnect(1,Origin+Y*(length/2.0),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));

  const Geometry::Vec3D Axis[]={-X,X,-Z,Z};

  
  const int surfN((flangeAOutRadius>flangeBOutRadius) ? 17 : 18);
  const double R(std::max(flangeAOutRadius,flangeBOutRadius));
  for(size_t i=0;i<4;i++)
    {
      FixedComp::setConnect(i+2,Origin+Axis[i]*R,Axis[i]);
      FixedComp::setLinkSurf(i+2,SMap.realSurf(buildIndex+surfN));
    }
  return;
}
   
void
SquareFMask::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation 
    \param FC :: Fixed component to set axis etc
    \param sideIndex :: position of linkpoint
  */
{
  ELog::RegMethod RegA("SquareFMask","createAllNoPopulate");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks(); 
  insertObjects(System);
  return;
}

  
}  // NAMESPACE constructSystem
