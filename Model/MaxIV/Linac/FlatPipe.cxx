/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/FlatPipe.cxx
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
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"

#include "FlatPipe.h"

namespace tdcSystem
{

FlatPipe::FlatPipe(const std::string& Key) : 
  attachSystem::FixedRotate(Key,8),
  attachSystem::ContainedGroup("Pipe","FlangeA","FlangeB"),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::FrontBackCut()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}


FlatPipe::~FlatPipe() 
  /*!
    Destructor
  */
{}

void
FlatPipe::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("FlatPipe","populate");
  
  FixedRotate::populate(Control);

  // Void + Wall:
  frontWidth=Control.EvalVar<double>(keyName+"FrontWidth");
  backWidth=Control.EvalVar<double>(keyName+"BackWidth");  
  frontHeight=Control.EvalVar<double>(keyName+"FrontHeight");
  backHeight=Control.EvalVar<double>(keyName+"BackHeight");

  length=Control.EvalVar<double>(keyName+"Length");
  
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  flangeARadius=Control.EvalPair<double>(keyName+"FlangeARadius",
					 keyName+"FlangeRadius");
  flangeBRadius=Control.EvalPair<double>(keyName+"FlangeBRadius",
					 keyName+"FlangeRadius");

  flangeALength=Control.EvalPair<double>(keyName+"FlangeALength",
					 keyName+"FlangeLength");
  flangeBLength=Control.EvalPair<double>(keyName+"FlangeBLength",
					 keyName+"FlangeLength");


  voidMat=ModelSupport::EvalDefMat<int>(Control,keyName+"VoidMat",0);
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  return;
}
  
void
FlatPipe::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("FlatPipe","createSurfaces");
  
  // Inner void
  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,
			       Origin-Y*(length/2.0),Y); 
      FrontBackCut::setFront(SMap.realSurf(buildIndex+1));
    }
  // use this so angled fronts correctly make
  FrontBackCut::getShiftedFront(SMap,buildIndex+11,1,Y,flangeALength);

  
  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,
			       Origin+Y*(length/2.0),Y);
      FrontBackCut::setBack(-SMap.realSurf(buildIndex+2));
    }
  FrontBackCut::getShiftedBack(SMap,buildIndex+12,-1,Y,flangeBLength);

  // main pipe
  ModelSupport::buildPlane(SMap,buildIndex+3,
			   Origin-X*(frontWidth/2.0),
			   Origin-X*(backWidth/2.0),
			   Origin-X*(backWidth/2.0)+Z,
			   X);
  ModelSupport::buildPlane(SMap,buildIndex+4,
			   Origin+X*(frontWidth/2.0),
			   Origin+X*(backWidth/2.0),
			   Origin+X*(backWidth/2.0)+Z,
			   X);
  ModelSupport::buildPlane(SMap,buildIndex+5,
			   Origin-Z*(frontHeight/2.0),
			   Origin-Z*(backHeight/2.0),
			   Origin-Z*(backHeight/2.0)+X,
			   Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,
			   Origin+Z*(frontHeight/2.0),
			   Origin+Z*(backHeight/2.0),
			   Origin+Z*(backHeight/2.0)+X,
			   Z);


  // two inner
  if (std::abs(frontHeight-backHeight)<Geometry::zeroTol)
    {
      const double R(frontHeight/2.0);
      const Geometry::Vec3D LAxis
	((Y*length-X*((backWidth-frontWidth)/2.0)).unit());
      const Geometry::Vec3D RAxis
	((Y*length+X*((backWidth-frontWidth)/2.0)).unit());

      ModelSupport::buildCylinder
	(SMap,buildIndex+7,Origin-X*(frontWidth/2.0),LAxis,R);

      ModelSupport::buildCylinder
	(SMap,buildIndex+8,Origin+X*(frontWidth/2.0),RAxis,R);

      ModelSupport::buildCylinder
	(SMap,buildIndex+17,Origin-X*(wallThick+frontWidth/2.0),LAxis,R);

      ModelSupport::buildCylinder
	(SMap,buildIndex+18,Origin+X*(wallThick+frontWidth/2.0),RAxis,R);
      
    }
  else // CONE VERSION
    {
      // buildCone takes 4 vectors:
      // Centre point [point on central axis ]
      // axis
      // Point A / Point B :: on cone boundary
      /*
      Geometry::Vec3D LeftMid(Origin+
      ModelSupport::buildCone(SMap,buildIndex+7,
			      Origin-X*(width/2.0),Y,height/2.0);
      ModelSupport::buildCone(SMap,buildIndex+8,Origin+X*(width/2.0),Y,height/2.0);
      
      ModelSupport::buildCone(SMap,buildIndex+17,
				  Origin-X*(width/2.0),Y,height/2.0+wallThick);
      ModelSupport::buildCone(SMap,buildIndex+18,
				  Origin+X*(width/2.0),Y,height/2.0+wallThick);
      */
    }
  

  // main pipe walls
  // main pipe
  ModelSupport::buildPlane(SMap,buildIndex+13,
			   Origin-X*(wallThick+frontWidth/2.0),
			   Origin-X*(wallThick+backWidth/2.0),
			   Origin-X*(wallThick+backWidth/2.0)+Z,
			   X);
  ModelSupport::buildPlane(SMap,buildIndex+14,
			   Origin+X*(wallThick+frontWidth/2.0),
			   Origin+X*(wallThick+backWidth/2.0),
			   Origin+X*(wallThick+backWidth/2.0)+Z,
			   X);
  ModelSupport::buildPlane(SMap,buildIndex+15,
			   Origin-Z*(wallThick+frontHeight/2.0),
			   Origin-Z*(wallThick+backHeight/2.0),
			   Origin-Z*(wallThick+backHeight/2.0)+X,
			   Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,
			   Origin+Z*(wallThick+frontHeight/2.0),
			   Origin+Z*(wallThick+backHeight/2.0),
			   Origin+Z*(wallThick+backHeight/2.0)+X,
			   Z);
  
  // FLANGE SURFACES FRONT/BACK:
  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,flangeARadius);
  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,Y,flangeBRadius);

  return;
}

void
FlatPipe::createObjects(Simulation& System)
  /*!
    Adds the vacuum system
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("FlatPipe","createObjects");

  std::string Out;

  const std::string frontStr=frontRule();
  const std::string backStr=backRule();
  
  // Void
  Out=ModelSupport::getComposite(SMap,buildIndex,"(3:-7) (-4:-8) 5 -6 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out+frontStr+backStr);

  Out=ModelSupport::getComposite(SMap,buildIndex,"11 -12 3 -4 6 -16");
  makeCell("TopPipe",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"11 -12 3 -4 -5 15");
  makeCell("BasePipe",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"11 -12 -3 7 -17 ");
  makeCell("LeftPipe",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"11 -12 4 8 -18 ");
  makeCell("RightPipe",System,cellIndex++,wallMat,0.0,Out);

  // FLANGE Front: 
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " -11 -107 ((7 -3) : (8 4) : -5 : 6) ");
  makeCell("FrontFlange",System,cellIndex++,wallMat,0.0,Out+frontStr);

  // FLANGE Back:
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " 12 -207 ((7 -3) : (8 4) : -5 : 6) ");
  makeCell("BackFlange",System,cellIndex++,wallMat,0.0,Out+backStr);

  // outer boundary [flange front/back]
  Out=ModelSupport::getSetComposite(SMap,buildIndex," -11 -107 ");
  addOuterSurf("FlangeA",Out+frontStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 12 -207 ");
  addOuterSurf("FlangeB",Out+backStr);

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " 11 -12 15 -16 (3:-17) (-4:-18)");


  
  addOuterSurf("Pipe",Out);
  return;
}
  
void
FlatPipe::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("FlatPipe","createLinks");
  
  //stuff for intersection
  FrontBackCut::createLinks(*this,Origin,Y);  //front and back

  const double H((frontHeight+backHeight)/2.0);
  const double W((frontWidth+backWidth)/2.0);
  FixedComp::setConnect(2,Origin-X*(wallThick+H+W),-X);
  FixedComp::setConnect(3,Origin-X*(wallThick+H+W),X);
  FixedComp::setConnect(4,Origin-Z*(wallThick+H+W),-Z);
  FixedComp::setConnect(5,Origin+Z*(wallThick+H+W),Z);
  
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+17));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+18));
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+15));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+16));


  // top lift point : Out is an complemnt of the volume
  std::string Out;
  FixedComp::setConnect(7,Origin+Z*(wallThick+H),Z);
  Out=ModelSupport::getComposite
    (SMap,buildIndex," (-15 : 16 : (-3 17) : (4 18))");
  FixedComp::setLinkSurf(7,Out);

  FixedComp::nameSideIndex(7,"outerPipe");
  
  return;
}
  
  
void
FlatPipe::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int FIndex)
 /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("FlatPipe","createAll");

  populate(System.getDataBase());
  createCentredUnitVector(FC,FIndex,length);
  createSurfaces();    
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE xraySystem
