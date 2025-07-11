/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   construct/CornerPipe.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
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
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "SurfMap.h"

#include "CornerPipe.h"

namespace constructSystem
{

CornerPipe::CornerPipe(const std::string& Key) :
  attachSystem::FixedRotate(Key,11),
  attachSystem::ContainedGroup("Main","FlangeA","FlangeB","Tube"),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),attachSystem::FrontBackCut()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  FixedComp::nameSideIndex(0,"front");
  FixedComp::nameSideIndex(1,"back");
  FixedComp::nameSideIndex(6,"midPoint");
}

CornerPipe::~CornerPipe()
  /*!
    Destructor
  */
{}

void
CornerPipe::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("CornerPipe","populate");

  FixedRotate::populate(Control);

  // Void + Fe special:
  cornerRadius=Control.EvalVar<double>(keyName+"CornerRadius");
  height=Control.EvalVar<double>(keyName+"Height");
  width=Control.EvalVar<double>(keyName+"Width");

  length=Control.EvalVar<double>(keyName+"Length");

  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  const double fR=Control.EvalDefVar<double>(keyName+"FlangeRadius",-1.0);

  flangeARadius=Control.EvalDefVar<double>(keyName+"FlangeARadius",fR);

  flangeBRadius=Control.EvalDefVar<double>(keyName+"FlangeBRadius",fR);

  const double fL=Control.EvalDefVar<double>(keyName+"FlangeLength",-1.0);
  flangeALength=Control.EvalDefVar<double>(keyName+"FlangeALength",fL);
  flangeBLength=Control.EvalDefVar<double>(keyName+"FlangeBLength",fL);


  voidMat=ModelSupport::EvalDefMat(Control,keyName+"VoidMat",0);
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  flangeMat=ModelSupport::EvalDefMat(Control,keyName+"FlangeMat",wallMat);

  outerVoid = Control.EvalDefVar<int>(keyName+"OuterVoid",0);

  return;
}

void
CornerPipe::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("CornerPipe","createSurfaces");

  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  if (!isActive("back"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
      ExternalCut::setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }

  // Front Inner void
  getShiftedFront(SMap,buildIndex+101,Y,flangeALength);
  getShiftedBack(SMap,buildIndex+102,Y,-flangeBLength);

  // mid point corner (cyclic)
  const Geometry::Vec3D midA(Origin-X*(width/2.0-cornerRadius)-
			     Z*(height/2.0-cornerRadius));
  const Geometry::Vec3D midB(Origin-X*(width/2.0-cornerRadius)+
			     Z*(height/2.0-cornerRadius));
  const Geometry::Vec3D midC(Origin+X*(width/2.0-cornerRadius)+
			     Z*(height/2.0-cornerRadius));
  const Geometry::Vec3D midD(Origin+X*(width/2.0-cornerRadius)-
			     Z*(height/2.0-cornerRadius));

  // inner points
  ModelSupport::buildPlane(SMap,buildIndex+3,midA,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,midC,X);
  ModelSupport::buildPlane(SMap,buildIndex+5,midA,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,midB,Z);

  // inner walls
  ModelSupport::buildPlane(SMap,buildIndex+13,midA-X*cornerRadius,X);
  ModelSupport::buildPlane(SMap,buildIndex+14,midC+X*cornerRadius,X);
  ModelSupport::buildPlane(SMap,buildIndex+15,midA-Z*cornerRadius,Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,midB+Z*cornerRadius,Z);

  // outer walls
  const double WT(cornerRadius+wallThick);
  ModelSupport::buildPlane(SMap,buildIndex+23,midA-X*WT,X);
  ModelSupport::buildPlane(SMap,buildIndex+24,midC+X*WT,X);
  ModelSupport::buildPlane(SMap,buildIndex+25,midA-Z*WT,Z);
  ModelSupport::buildPlane(SMap,buildIndex+26,midB+Z*WT,Z);

  // corners (inner):
  ModelSupport::buildCylinder(SMap,buildIndex+107,midA,Y,cornerRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+117,midB,Y,cornerRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+127,midC,Y,cornerRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+137,midD,Y,cornerRadius);

  // corners (outer):
  ModelSupport::buildCylinder(SMap,buildIndex+207,midA,Y,WT);
  ModelSupport::buildCylinder(SMap,buildIndex+217,midB,Y,WT);
  ModelSupport::buildCylinder(SMap,buildIndex+227,midC,Y,WT);
  ModelSupport::buildCylinder(SMap,buildIndex+237,midD,Y,WT);

  // FLANGE SURFACES FRONT:
  ModelSupport::buildCylinder(SMap,buildIndex+1007,Origin,Y,flangeARadius);
  ModelSupport::buildCylinder(SMap,buildIndex+2007,Origin,Y,flangeBRadius);

  return;
}

void
CornerPipe::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("CornerPipe","createObjects");

  HeadRule HR;

  const HeadRule frontHR=getRule("front");
  const HeadRule backHR=getRule("back");

  // mid void (cross) + top/bottom corner
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"13 -14 5 -6");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*frontHR*backHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-5 -3 -107");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*frontHR*backHR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-5 15 3 -4");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*frontHR*backHR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-5 4 -137");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*frontHR*backHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"6 -16 3 -4");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*frontHR*backHR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"6 -3 -117");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*frontHR*backHR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"6 4 -127");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*frontHR*backHR);


  // Walls:

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"23 -13 5 -6 101 -102");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-24 14 5 -6 101 -102");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"25 -15 3 -4 101 -102");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-26 16 3 -4 101 -102");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-3 -5 107 -207 101 -102 23 25");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-3 6 117 -217 101 -102 23 -26");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"4 6 127 -227 101 -102 -24 -26");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"4 -5 137 -237 101 -102 -24 25");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,HR);

  // Flange A lower part
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-101 -3 -5 107 -1007");
  makeCell("FlangeA",System,cellIndex++,wallMat,0.0,HR*frontHR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-101 3 -4 -15 -1007");
  makeCell("FlangeA",System,cellIndex++,wallMat,0.0,HR*frontHR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-101 4 -5 137 -1007");
  makeCell("FlangeA",System,cellIndex++,wallMat,0.0,HR*frontHR);

  // Flange A mid part
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1007 -101 (-13:14) 5 -6 ");
  makeCell("FlangeA",System,cellIndex++,wallMat,0.0,HR*frontHR);

  // Flange A upper part
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-101 -3 6 117 -1007");
  makeCell("FlangeA",System,cellIndex++,wallMat,0.0,HR*frontHR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-101 3 -4 16 -1007");
  makeCell("FlangeA",System,cellIndex++,wallMat,0.0,HR*frontHR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-101 4 6 127 -1007");
  makeCell("FlangeA",System,cellIndex++,wallMat,0.0,HR*frontHR);

  // Flange B lower part
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"102 -3 -5 107 -2007 ");
  makeCell("FlangeB",System,cellIndex++,wallMat,0.0,HR*backHR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"102 3 -4 -15 -2007 ");
  makeCell("FlangeB",System,cellIndex++,wallMat,0.0,HR*backHR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"102 4 -5 137 -2007 ");
  makeCell("FlangeB",System,cellIndex++,wallMat,0.0,HR*backHR);

  // Flange B mid part
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2007 102 (-13:14) 5 -6 ");
  makeCell("FlangeB",System,cellIndex++,wallMat,0.0,HR*backHR);

  // Flange B lower part
  //  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2007 102 (16:-3:4) 117 127 6");
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"102 -3 6 117 -2007");
  makeCell("FlangeB",System,cellIndex++,wallMat,0.0,HR*backHR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"102 3 -4 16 -2007");
  makeCell("FlangeB",System,cellIndex++,wallMat,0.0,HR*backHR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"102 4 6 127 -2007");
  makeCell("FlangeB",System,cellIndex++,wallMat,0.0,HR*backHR);



  // outer void corners:

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-3 -5 23 25 207 101 -102");
  makeCell("OutCorner",System,cellIndex++,voidMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-3 6 23 -26 217 101 -102");
  makeCell("OutCorner",System,cellIndex++,voidMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"4 6 -24 -26 227 101 -102");
  makeCell("OutCorner",System,cellIndex++,voidMat,0.0,HR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"4 -5 -24 25 237 101 -102");
  makeCell("OutCorner",System,cellIndex++,voidMat,0.0,HR);

  // currently as
  if (outerVoid)
    {
      HR=ModelSupport::getHeadRule
	(SMap,buildIndex,"-1007 (-23:24:-25:26) 101 -102");
      makeCell("OutVoid",System,cellIndex++,voidMat,0.0,HR);
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1007");
      addOuterSurf("Main",HR*frontHR*backHR);
    }
  else
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1007 -101");
      addOuterSurf("FlangeA",HR*frontHR);
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2007 102");
      addOuterSurf("FlangeB",HR*backHR);
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -102 23 -24 25 -26");
      addOuterSurf("Main",HR);
    }

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"23 -24 25 -26");
  addOuterSurf("Tube",HR*frontHR*backHR);

  return;
}


void
CornerPipe::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("CornerPipe","createLinks");
  FrontBackCut::createLinks(*this,Origin,Y);  //front and back

  FixedComp::setConnect(2,Origin-X*(width/2.0+wallThick),-X);
  FixedComp::setConnect(3,Origin+X*(width/2.0+wallThick),X);
  FixedComp::setConnect(4,Origin-Z*(height/2.0+wallThick),-Z);
  FixedComp::setConnect(5,Origin+Z*(height/2.0+wallThick),Z);

  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+23));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+24));
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+25));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+26));

  FixedComp::nameSideIndex(4,"outerPipe");
  FixedComp::nameSideIndex(4,"pipeOuterBase");
  FixedComp::nameSideIndex(5,"pipeOuterTop");

  // corner tracks

  // mid point corner (cyclic)
  const Geometry::Vec3D ptA(Origin-X*(width/2.0+wallThick)-
			     Z*(height/2.0+wallThick));
  const Geometry::Vec3D ptB(Origin-X*(width/2.0+wallThick)+
			     Z*(height/2.0+wallThick));
  const Geometry::Vec3D ptC(Origin+X*(width/2.0+wallThick)+
			     Z*(height/2.0+wallThick));
  const Geometry::Vec3D ptD(Origin+X*(width/2.0+wallThick)-
			     Z*(height/2.0+wallThick));

  FixedComp::setConnect(6,ptA,Y);
  FixedComp::setConnect(7,ptB,Y);
  FixedComp::setConnect(8,ptC,Y);
  FixedComp::setConnect(9,ptD,Y);

  FixedComp::nameSideIndex(6,"cornerA");
  FixedComp::nameSideIndex(7,"cornerB");
  FixedComp::nameSideIndex(8,"cornerC");
  FixedComp::nameSideIndex(9,"cornerD");

  return;
}

void
CornerPipe::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int FIndex)
 /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("CornerPipe","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();
  createObjects(System);
  createLinks();

  insertObjects(System);

  return;
}

}  // NAMESPACE constructSystem
