/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   construct/VacuumPipe.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#include "BaseVisit.h"
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

#include "GeneralPipe.h"
#include "VacuumPipe.h"

namespace constructSystem
{

VacuumPipe::VacuumPipe(const std::string& Key) :
  GeneralPipe(Key,11)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  FixedComp::nameSideIndex(0,"front");
  FixedComp::nameSideIndex(1,"back");
  FixedComp::nameSideIndex(6,"midPoint");
}

VacuumPipe::VacuumPipe(const VacuumPipe& A) :
  GeneralPipe(A),
  radius(A.radius),height(A.height),
  width(A.width),length(A.length),feThick(A.feThick),
  claddingThick(A.claddingThick),
  flangeARadius(A.flangeARadius),flangeAHeight(A.flangeAHeight),
  flangeAWidth(A.flangeAWidth),flangeALength(A.flangeALength),
  flangeBRadius(A.flangeBRadius),flangeBHeight(A.flangeBHeight),
  flangeBWidth(A.flangeBWidth),flangeBLength(A.flangeBLength),
  voidMat(A.voidMat),feMat(A.feMat),
  claddingMat(A.claddingMat),flangeMat(A.flangeMat),
  outerVoid(0)
  /*!
    Copy constructor
    \param A :: VacuumPipe to copy
  */
{}

VacuumPipe&
VacuumPipe::operator=(const VacuumPipe& A)
  /*!
    Assignment operator
    \param A :: VacuumPipe to copy
    \return *this
  */
{
  if (this!=&A)
    {
      GeneralPipe::operator=(A);
      radius=A.radius;
      height=A.height;
      width=A.width;
      length=A.length;
      feThick=A.feThick;
      claddingThick=A.claddingThick;
      flangeARadius=A.flangeARadius;
      flangeAHeight=A.flangeAHeight;
      flangeAWidth=A.flangeAWidth;
      flangeALength=A.flangeALength;
      flangeBRadius=A.flangeBRadius;
      flangeBHeight=A.flangeBHeight;
      flangeBWidth=A.flangeBWidth;
      flangeBLength=A.flangeBLength;
      voidMat=A.voidMat;
      feMat=A.feMat;
      claddingMat=A.claddingMat;
      flangeMat=A.flangeMat;
    }
  return *this;
}

VacuumPipe::~VacuumPipe()
  /*!
    Destructor
  */
{}

void
VacuumPipe::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("VacuumPipe","populate");

  FixedRotate::populate(Control);

  // Void + Fe special:
  radius=Control.EvalDefVar<double>(keyName+"Radius",-1.0);
  height=Control.EvalDefVar<double>(keyName+"Height",-1.0);
  width=Control.EvalDefVar<double>(keyName+"Width",-1.0);

  if (radius<0.0 && (width<0.0 || height<0.0))
    throw ColErr::EmptyContainer
      ("Pipe:["+keyName+"] has neither Radius or Height/Width");

  length=Control.EvalVar<double>(keyName+"Length");

  feThick=Control.EvalVar<double>(keyName+"FeThick");
  claddingThick=Control.EvalDefVar<double>(keyName+"CladdingThick",0.0);

  const double fR=Control.EvalDefVar<double>(keyName+"FlangeRadius",-1.0);
  const double fH=Control.EvalDefVar<double>(keyName+"FlangeHeight",-1.0);
  const double fW=Control.EvalDefVar<double>(keyName+"FlangeWidth",-1.0);

  flangeARadius=Control.EvalDefVar<double>(keyName+"FlangeARadius",fR);
  flangeAHeight=Control.EvalDefVar<double>(keyName+"FlangeAHeight",fH);
  flangeAWidth=Control.EvalDefVar<double>(keyName+"FlangeAWidth",fW);

  flangeBRadius=Control.EvalDefVar<double>(keyName+"FlangeBRadius",fR);
  flangeBHeight=Control.EvalDefVar<double>(keyName+"FlangeBHeight",fH);
  flangeBWidth=Control.EvalDefVar<double>(keyName+"FlangeBWidth",fW);

  if (flangeARadius<0.0 && (flangeAWidth<0.0 || flangeAHeight<0.0))
    throw ColErr::EmptyContainer
      ("Pipe:["+keyName+"][front] missing flange: Radius/Height/Width");
  if (flangeBRadius<0.0 && (flangeBWidth<0.0 || flangeBHeight<0.0))
    throw ColErr::EmptyContainer
      ("Pipe:["+keyName+"][back] missing flange: Radius/Height/Width");

  const double fL=Control.EvalDefVar<double>(keyName+"FlangeLength",-1.0);
  flangeALength=Control.EvalDefVar<double>(keyName+"FlangeALength",fL);
  flangeBLength=Control.EvalDefVar<double>(keyName+"FlangeBLength",fL);

  voidMat=ModelSupport::EvalDefMat(Control,keyName+"VoidMat",0);
  feMat=ModelSupport::EvalMat<int>(Control,keyName+"FeMat");
  claddingMat=ModelSupport::EvalDefMat(Control,keyName+"CladdingMat",0);
  flangeMat=ModelSupport::EvalDefMat(Control,keyName+"FlangeMat",feMat);

  outerVoid = Control.EvalDefVar<int>(keyName+"OuterVoid",0);

  return;
}

void
VacuumPipe::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("VacuumPipe","createSurfaces");

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
  FrontBackCut::getShiftedFront(SMap,buildIndex+101,Y,flangeALength);
  FrontBackCut::getShiftedBack(SMap,buildIndex+102,Y,-flangeBLength);


  // MAIN SURFACES:
  if (radius>Geometry::zeroTol)
    {
      ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);
      ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,radius+feThick);
      ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,
				  Y,radius+feThick+claddingThick);
      addSurf("OuterRadius",SMap.realSurf(buildIndex+27));
    }
  else
    {
      double H(height/2.0);
      double W(width/2.0);
      ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*W,X);
      ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*W,X);
      ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*H,Z);
      ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*H,Z);

      H+=feThick;
      W+=feThick;
      ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*W,X);
      ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*W,X);
      ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*H,Z);
      ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*H,Z);

      H+=claddingThick;
      W+=claddingThick;
      ModelSupport::buildPlane(SMap,buildIndex+23,Origin-X*W,X);
      ModelSupport::buildPlane(SMap,buildIndex+24,Origin+X*W,X);
      ModelSupport::buildPlane(SMap,buildIndex+25,Origin-Z*H,Z);
      ModelSupport::buildPlane(SMap,buildIndex+26,Origin+Z*H,Z);

    }

  // FLANGE SURFACES FRONT:
  if (flangeARadius>0.0)
    ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,flangeARadius);
  else
    {
      ModelSupport::buildPlane(SMap,buildIndex+103,Origin-X*(flangeAWidth/2.0),X);
      ModelSupport::buildPlane(SMap,buildIndex+104,Origin+X*(flangeAWidth/2.0),X);
      ModelSupport::buildPlane(SMap,buildIndex+105,Origin-Z*(flangeAHeight/2.0),Z);
      ModelSupport::buildPlane(SMap,buildIndex+106,Origin+Z*(flangeAHeight/2.0),Z);
    }

  // FLANGE SURFACES BACK:
  if (flangeBRadius>0.0)
    ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,Y,flangeBRadius);
  else
    {
      ModelSupport::buildPlane(SMap,buildIndex+203,Origin-X*(flangeBWidth/2.0),X);
      ModelSupport::buildPlane(SMap,buildIndex+204,Origin+X*(flangeBWidth/2.0),X);
      ModelSupport::buildPlane(SMap,buildIndex+205,Origin-Z*(flangeBHeight/2.0),Z);
      ModelSupport::buildPlane(SMap,buildIndex+206,Origin+Z*(flangeBHeight/2.0),Z);
    }

  return;
}

void
VacuumPipe::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("VacuumPipe","createObjects");

  HeadRule HR;

  const HeadRule frontHR=getRule("front");
  const HeadRule backHR=getRule("back");

  // Void
  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"-7 3 -4 5 -6");
  HeadRule InnerVoid(HR);
  InnerVoid.makeComplement();
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*frontHR*backHR);

  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"-17 13 -14 15 -16");
  HeadRule WallLayer(HR);

  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"-27 23 -24 25 -26");
  HeadRule CladdingLayer(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -102");
  makeCell("Steel",System,cellIndex++,feMat,0.0,HR*WallLayer*InnerVoid);
  addCell("MainSteel",cellIndex-1);
  // cladding
  if (claddingThick>Geometry::zeroTol)
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -102");
      HR*=WallLayer.complement()*CladdingLayer;
      makeCell("Cladding",System,cellIndex++,claddingMat,0.0,HR);
    }

  // FLANGE: 107 OR 103-106 valid
  HR=ModelSupport::getSetHeadRule
    (SMap,buildIndex,"-101 -107 103 -104 105 -106");
  makeCell("Steel",System,cellIndex++,flangeMat,0.0,HR*frontHR*InnerVoid);

  // FLANGE: 207 OR 203-206 valid
  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"102 -207 203 -204 205 -106");

  makeCell("Steel",System,cellIndex++,flangeMat,0.0,HR*backHR*InnerVoid);

  if (outerVoid && (radius>Geometry::zeroTol))
    {
      if (std::abs(flangeARadius-flangeBRadius)<Geometry::zeroTol)
	{
	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -102 27 -107");
	  makeCell("outerVoid",System,cellIndex++,0,0.0,HR);
	  HR=HeadRule(SMap,buildIndex,-107);
	  addOuterSurf("Main",HR*frontHR*backHR);
	}
      else if (flangeARadius>flangeBRadius)
	{
	  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"101 -102 27 -107");
	  makeCell("outerVoid",System,cellIndex++,0,0.0,HR);
	  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"102 207 -107");
	  makeCell("outerVoid",System,cellIndex++,0,0.0,HR*backHR);

	  HR=HeadRule(SMap,buildIndex,-107);
	  addOuterSurf("Main",HR*frontHR*backHR);
	}
      else if (flangeBRadius>flangeARadius)
	{
	  HR=ModelSupport::getSetHeadRule(SMap,buildIndex," 101 -102 27 -207");
	  makeCell("outerVoid",System,cellIndex++,0,0.0,HR);
	  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"-101 107 -207");
	  makeCell("outerVoid",System,cellIndex++,0,0.0,HR*frontHR);

	  HR=HeadRule(SMap,buildIndex,-207);
	  addOuterSurf("Main",HR*frontHR*backHR);
	}
    }
  else
    {

      // outer boundary [flange front]
      HR=ModelSupport::getSetHeadRule
	(SMap,buildIndex,"-101 -107 103 -104 105 -106");
      addOuterSurf("FlangeA",HR*frontHR);

      // outer boundary [flange back]
      HR=ModelSupport::getSetHeadRule
	(SMap,buildIndex,"102 -207 203 -204 205 -206");
      addOuterSurf("FlangeB",HR*backHR);

      // outer boundary mid tube
      HR=ModelSupport::getSetHeadRule(SMap,buildIndex," 101 -102 ");
      addOuterSurf("Main",HR*CladdingLayer);
    }

  return;
}


void
VacuumPipe::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("VacuumPipe","createLinks");

  //stuff for intersection
  FrontBackCut::createLinks(*this,Origin,Y);  //front and back

  // Round pipe
  if (radius>0.0)
    {
      FixedComp::setConnect(2,Origin-X*radius,-X);
      FixedComp::setConnect(3,Origin+X*radius,X);
      FixedComp::setConnect(4,Origin-Z*radius,-Z);
      FixedComp::setConnect(5,Origin+Z*radius,Z);
      FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+7));
      FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+7));
      FixedComp::setLinkSurf(4,SMap.realSurf(buildIndex+7));
      FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+7));

      FixedComp::setConnect(7,Origin-Z*(radius+feThick),-Z);
      FixedComp::setConnect(8,Origin+Z*(radius+feThick),Z);
      FixedComp::setLinkSurf(7,SMap.realSurf(buildIndex+17));
      FixedComp::setLinkSurf(8,SMap.realSurf(buildIndex+17));

      FixedComp::nameSideIndex(7,"outerPipe");
      FixedComp::nameSideIndex(7,"pipeOuterBase");
      FixedComp::nameSideIndex(8,"pipeOuterTop");
    }
  else // rectangular pipe
    {
      FixedComp::setConnect(2,Origin-X*(width/2.0),-X);
      FixedComp::setConnect(3,Origin+X*(width/2.0),X);
      FixedComp::setConnect(4,Origin-Z*(height/2.0),-Z);
      FixedComp::setConnect(5,Origin+Z*(height/2.0),Z);
      FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));
      FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));
      FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));
      FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));

      FixedComp::setConnect(7,Origin-Z*(height/2.0+feThick),-Z);
      FixedComp::setConnect(8,Origin+Z*(height/2.0+feThick),Z);
      FixedComp::setLinkSurf(7,-SMap.realSurf(buildIndex+15));
      FixedComp::setLinkSurf(8,SMap.realSurf(buildIndex+16));
    }

  // MID Point: [NO SURF]
  const Geometry::Vec3D midPt=
    (getLinkPt(1)+getLinkPt(2))/2.0;
  FixedComp::setConnect(6,midPt,Y);

  if (flangeARadius>0.0)
    {
      FixedComp::setConnect(9,Origin-Z*flangeARadius,-Z);
      FixedComp::setConnect(10,Origin+Z*flangeARadius,Z);

      FixedComp::setLinkSurf(9,SMap.realSurf(buildIndex+107));
      FixedComp::setLinkSurf(10,SMap.realSurf(buildIndex+107));
    }
  else
    {
      FixedComp::setConnect(9,Origin-Z*(flangeAHeight/2.0),-Z);
      FixedComp::setConnect(10,Origin+Z*(flangeAHeight/2.0),Z);
      FixedComp::setLinkSurf(9,-SMap.realSurf(buildIndex+105));
      FixedComp::setLinkSurf(10,SMap.realSurf(buildIndex+106));
    }


  return;
}


void
VacuumPipe::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int FIndex)
 /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("VacuumPipe","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  GeneralPipe::applyActiveFrontBack(length);
  createSurfaces();
  createObjects(System);
  createLinks();

  insertObjects(System);

  return;
}

}  // NAMESPACE constructSystem
