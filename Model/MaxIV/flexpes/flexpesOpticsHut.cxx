/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flexpes/flexpesOpticsHut.cxx
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Surface.h"
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
#include "FixedGroup.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "SpaceCut.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "SimpleChicane.h"

#include "flexpesOpticsHut.h"

namespace xraySystem
{

flexpesOpticsHut::flexpesOpticsHut(const std::string& Key) : 
  attachSystem::FixedOffset(Key,18),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

flexpesOpticsHut::~flexpesOpticsHut() 
  /*!
    Destructor
  */
{}

void
flexpesOpticsHut::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("flexpesOpticsHut","populate");
  
  FixedOffset::populate(Control);

  height=Control.EvalVar<double>(keyName+"Height");
  length=Control.EvalVar<double>(keyName+"Length");
  outWidth=Control.EvalVar<double>(keyName+"OutWidth");
  ringShortWidth=Control.EvalVar<double>(keyName+"RingShortWidth");
  ringLongWidth=Control.EvalVar<double>(keyName+"RingLongWidth");

  extension=Control.EvalDefVar<double>(keyName+"Extension",0.0);
  shortLen=Control.EvalVar<double>(keyName+"ShortLen");
  fullLen=Control.EvalVar<double>(keyName+"FullLen");

  innerSkin=Control.EvalVar<double>(keyName+"InnerSkin");
  pbWallThick=Control.EvalVar<double>(keyName+"PbWallThick");
  pbFrontThick=Control.EvalVar<double>(keyName+"PbFrontThick");
  pbBackThick=Control.EvalVar<double>(keyName+"PbBackThick");
  pbRoofThick=Control.EvalVar<double>(keyName+"PbRoofThick");
  outerSkin=Control.EvalVar<double>(keyName+"OuterSkin");

  innerFarVoid=Control.EvalDefVar<double>(keyName+"InnerFarVoid",0.0);
  outerFarVoid=Control.EvalDefVar<double>(keyName+"OuterFarVoid",0.0);

  inletXStep=Control.EvalDefVar<double>(keyName+"InletXStep",0.0);
  inletZStep=Control.EvalDefVar<double>(keyName+"InletZStep",0.0);
  inletRadius=Control.EvalDefVar<double>(keyName+"InletRadius",0.0);

  innerMat=ModelSupport::EvalMat<int>(Control,keyName+"InnerMat");
  pbMat=ModelSupport::EvalMat<int>(Control,keyName+"PbMat");
  outerMat=ModelSupport::EvalMat<int>(Control,keyName+"OuterMat");
  
  return;
}

void
flexpesOpticsHut::createUnitVector(const attachSystem::FixedComp& FC,
				     const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("flexpesOpticsHut","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  // shift forward for back wall
  Origin+=Y*(outerSkin+innerSkin+pbFrontThick);
  return;
}
 
void
flexpesOpticsHut::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("flexpesOpticsHut","createSurfaces");

  ModelSupport::buildPlane(SMap,buildIndex+31,Origin+Y*shortLen,Y);
  ModelSupport::buildPlane(SMap,buildIndex+41,Origin+Y*fullLen,Y);
  // Inner void [large volme
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length,Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*outWidth,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*ringLongWidth,X);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*height,Z);

  ModelSupport::buildPlane(SMap,buildIndex+24,Origin+X*ringShortWidth,X);
  ModelSupport::buildPlane(SMap,buildIndex+14,
			   Origin+X*ringShortWidth+Y*shortLen,
			   Origin+X*ringLongWidth+Y*fullLen,
			   Origin+X*ringLongWidth+Y*fullLen+Z,
			   X);

  SurfMap::setSurf("InnerCorner",SMap.realSurf(buildIndex+14));
  SurfMap::setSurf("InnerShort",SMap.realSurf(buildIndex+24));
  SurfMap::setSurf("InnerRoof",SMap.realSurf(buildIndex+6));
  
  if (innerFarVoid>Geometry::zeroTol)
    ModelSupport::buildPlane
      (SMap,buildIndex+1003,Origin-X*(outWidth-innerFarVoid),X);  

  const Geometry::Plane* SPtr=
    SMap.realPtr<const Geometry::Plane>(buildIndex+14);
  
  // Inner void [large volme
  double TF(innerSkin);
  double TW(innerSkin);
  double TB(innerSkin);
  double TR(innerSkin);
  ModelSupport::buildPlane(SMap,buildIndex+101,Origin-Y*TF,Y);
  ModelSupport::buildPlane(SMap,buildIndex+102,Origin+Y*(length+TB),Y);
  ModelSupport::buildPlane(SMap,buildIndex+103,Origin-X*(outWidth+TW),X);
  ModelSupport::buildPlane(SMap,buildIndex+104,Origin+X*(ringLongWidth+TW),X);
  ModelSupport::buildPlane(SMap,buildIndex+106,Origin+Z*(height+TR),Z);
  ModelSupport::buildPlane(SMap,buildIndex+124,Origin+X*(ringShortWidth+TW),X);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+114,SPtr,TW);

  // outer lead
  TF+=pbFrontThick;
  TW+=pbWallThick;
  TR+=pbRoofThick;
  TB+=pbBackThick;


  ModelSupport::buildPlane(SMap,buildIndex+201,Origin-Y*TF,Y);
  ModelSupport::buildPlane(SMap,buildIndex+202,Origin+Y*(length+TB),Y);
  ModelSupport::buildPlane(SMap,buildIndex+203,Origin-X*(outWidth+TW),X);
  ModelSupport::buildPlane(SMap,buildIndex+204,Origin+X*(ringLongWidth+TW),X);
  ModelSupport::buildPlane(SMap,buildIndex+206,Origin+Z*(height+TR),Z);
  ModelSupport::buildPlane(SMap,buildIndex+224,Origin+X*(ringShortWidth+TW),X);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+214,SPtr,TW);

  // final outer
  TF+=outerSkin;
  TW+=outerSkin;
  TR+=outerSkin;
  TB+=outerSkin;

  // Note use front surf
  // ModelSupport::buildPlane(SMap,buildIndex+301,Origin-Y*TF,Y);
  ModelSupport::buildPlane(SMap,buildIndex+302,Origin+Y*(length+TB),Y);
  ModelSupport::buildPlane(SMap,buildIndex+303,Origin-X*(outWidth+TW),X);
  ModelSupport::buildPlane(SMap,buildIndex+304,Origin+X*(ringLongWidth+TW),X);
  ModelSupport::buildPlane(SMap,buildIndex+306,Origin+Z*(height+TR),Z);
  ModelSupport::buildPlane(SMap,buildIndex+324,Origin+X*(ringShortWidth+TW),X);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+314,SPtr,TW);


  if (inletRadius>Geometry::zeroTol)
    ModelSupport::buildCylinder
      (SMap,buildIndex+7,Origin+X*inletXStep+Z*inletZStep,Y,inletRadius);

  ModelSupport::buildPlane(SMap,buildIndex+3002,
			     Origin+Y*(length+TB+extension),Y);
  ModelSupport::buildPlane(SMap,buildIndex+1303,
			     Origin-X*(outWidth+TW+outerFarVoid),X);

  SurfMap::setSurf("OuterCorner",SMap.realSurf(buildIndex+314));
  SurfMap::setSurf("OuterBack",SMap.realSurf(buildIndex+302));
  SurfMap::setSurf("OuterRoof",SMap.realSurf(buildIndex+306));
  return;
}

void
flexpesOpticsHut::createObjects(Simulation& System)
  /*!
    Adds the main objects
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("flexpesOpticsHut","createObjects");

  const std::string floorStr=getRuleStr("Floor");
  const std::string ringWall=getRuleStr("RingWall");
  std::string Out;


  if (innerFarVoid>Geometry::zeroTol)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 3 -1003 -6 ");
      makeCell("InnerFarVoid",System,cellIndex++,0,0.0,Out+floorStr);
      Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 1003 -4 (-14:-24) -6 ");
      makeCell("Void",System,cellIndex++,0,0.0,Out+floorStr);
    }
  else
    {
      Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 3 -4 (-14:-24) -6  ");
      makeCell("Void",System,cellIndex++,0,0.0,Out+floorStr);
    }
    
    
  std::list<int> matList({innerMat,pbMat,outerMat});
  int HI(buildIndex);
  for(const std::string& layer : {"Inner","Lead","Outer"})
    {
      const int mat=matList.front();
      matList.pop_front();
      Out=ModelSupport::getComposite(SMap,HI,"1 -2 -104 (-114:-124) (4:(14 24)) -6 ");
      makeCell("Ring"+layer,System,cellIndex++,mat,0.0,Out+floorStr);
      
      Out=ModelSupport::getComposite(SMap,HI,"1 -2 -3 103 -6 ");
      makeCell("Far"+layer,System,cellIndex++,mat,0.0,Out+floorStr);
      
      Out=ModelSupport::getComposite(SMap,HI,"1 -2 103 -104 (-114:-124) 6 -106 ");
      makeCell("Roof"+layer,System,cellIndex++,mat,0.0,Out);
      
      Out=ModelSupport::getSetComposite(SMap,HI,buildIndex,"2 -102 103 -104 -106 17M");
      makeCell("Back"+layer,System,cellIndex++,mat,0.0,Out+floorStr);
      addCell("Back",cellIndex-1);

      Out=ModelSupport::getSetComposite(SMap,HI,buildIndex,"101 -1 103 -124 -106 7M ");
      if (layer=="Outer") Out+=ringWall;
      makeCell("Front"+layer,System,cellIndex++,mat,0.0,Out+floorStr);

      HI+=100;
    }

  // back wall extension
  if (extension>Geometry::zeroTol)
    {
      // note use of the 1303 surface for outerFarVoid distance
      Out=ModelSupport::getSetComposite(SMap,HI,buildIndex," 2 1303M -4 -6 -3002M ");
      makeCell("Extension",System,cellIndex++,0,0.0,Out+floorStr);
    }

  // Front/back hole
  if (inletRadius>Geometry::zeroTol)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex," -1 -7 ");
      makeCell("InletHole",System,cellIndex++,0,0.0,Out+ringWall);
    }

  // far/side wall extension
  if (outerFarVoid>Geometry::zeroTol)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex,HI,"-2M -303 1303 -6M ");
      makeCell("OuterFarVoid",System,cellIndex++,0,0.0,Out+floorStr+ringWall);
    }
  
  Out=ModelSupport::getSetComposite(SMap,buildIndex,"301 -3002 1303 -304 (-314:-324) -306 ");
  addOuterSurf(Out);  

  return;
}

void
flexpesOpticsHut::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("flexpesOpticsHut","createLinks");

  const double extraBack(innerSkin+outerSkin+pbBackThick);
  const double extraWall(innerSkin+outerSkin+pbWallThick);

  ExternalCut::createLink("RingWall",*this,0,Origin,Y);

  setConnect(1,Origin+Y*(length+extraBack),Y);
  setLinkSurf(1,SMap.realSurf(buildIndex+302));

    // outer surf
  setConnect(3,Origin-X*(extraWall+outWidth)+Y*(length/2.0),-X);
  setLinkSurf(3,-SMap.realSurf(buildIndex+303));
  nameSideIndex(3,"farWall");
  // outer surf
  setConnect(4,Origin-X*(extraWall+ringLongWidth)+Y*(length/2.0),X);
  setLinkSurf(4,SMap.realSurf(buildIndex+304));
  nameSideIndex(4,"ringWall");

  // inner surf front
  setConnect(7,Origin,Y);
  setLinkSurf(7,SMap.realSurf(buildIndex+1));
  nameSideIndex(7,"innerFront");

  // inner surf back
  setConnect(8,Origin+Y*length,-Y);
  setLinkSurf(8,-SMap.realSurf(buildIndex+2));
  nameSideIndex(8,"innerBack");

    // inner surf
  setConnect(13,Origin-X*outWidth+Y*(length/2.0),X);
  setLinkSurf(13,SMap.realSurf(buildIndex+3));
  nameSideIndex(13,"innerFarWall");

  setConnect(14,Origin-X*ringLongWidth+Y*(length/2.0),-X);
  setLinkSurf(14,-SMap.realSurf(buildIndex+4));
  nameSideIndex(14,"innerRingWall");

  return;
}

void
flexpesOpticsHut::createChicane(Simulation& System)
  /*!
    Generic function to create chicanes
    Requirements:
       - OpticsHut must provide:
       - Cells:
             -- InnerFarVoid : void cell inside of wall
             -- OuteFarVoid : void cell outside of wall
             -- FarInner : cell in wall
             -- FarLead  : cell in wall
             -- FarOuter : cell in wall
       - Link Points:
           -- innerFarWall
           -- farWall 

    Note that linkPt:farWall is used as the primary 0,0,0 centre

    \param System :: Simulation 
  */
{
  ELog::RegMethod Rega("flexpesOpticsHut","createChicane");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  const FuncDataBase& Control=System.getDataBase();

  const size_t NChicane=
    Control.EvalDefVar<size_t>(keyName+"NChicane",0);

  for(size_t i=0;i<NChicane;i++)
    {
      const std::string NStr(std::to_string(i));
      std::shared_ptr<SimpleChicane> PItem=
	std::make_shared<SimpleChicane>(keyName+"Chicane"+NStr);

      OR.addObject(PItem);

      PItem->addInsertCell("Inner",getCell("InnerFarVoid"));
      PItem->addInsertCell("Outer",getCell("OuterFarVoid"));

      PItem->addInsertCell("Middle",getCell("FarInner",0));
      PItem->addInsertCell("Middle",getCell("FarLead",0));
      PItem->addInsertCell("Middle",getCell("FarOuter",0));
      // set surfaces:

      PItem->setCutSurf("innerWall",*this,"innerFarWall");
      PItem->setCutSurf("outerWall",*this,"farWall");

      PItem->createAll(System,*this,getSideIndex("farWall"));

    }
  return;
}

void
flexpesOpticsHut::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("flexpesOpticsHut","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  createChicane(System);
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE xraySystem
