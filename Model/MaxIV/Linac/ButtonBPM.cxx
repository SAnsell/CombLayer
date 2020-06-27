/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/Linac/ButtonBPM.cxx
 *
 * Copyright (c) 2004-2020 by Konstantin Batkov
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
#include "Vec3D.h"
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
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"

#include "ButtonBPM.h"

namespace tdcSystem
{

ButtonBPM::ButtonBPM(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,6),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::FrontBackCut()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

ButtonBPM::ButtonBPM(const ButtonBPM& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),
  radius(A.radius),length(A.length),outerRadius(A.outerRadius),
  innerRadius(A.innerRadius),
  flangeInnerRadius(A.flangeInnerRadius),
  flangeARadius(A.flangeARadius),
  flangeBLength(A.flangeBLength),
  flangeBRadius(A.flangeBRadius),
  flangeALength(A.flangeALength),
  voidMat(A.voidMat),wallMat(A.wallMat),
  flangeMat(A.flangeMat)
  /*!
    Copy constructor
    \param A :: ButtonBPM to copy
  */
{}

ButtonBPM&
ButtonBPM::operator=(const ButtonBPM& A)
  /*!
    Assignment operator
    \param A :: ButtonBPM to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      attachSystem::FrontBackCut::operator=(A);
      length=A.length;
      innerRadius=A.innerRadius;
      radius=A.radius;
      outerRadius=A.outerRadius;
      flangeInnerRadius=A.flangeInnerRadius;
      flangeARadius=A.flangeARadius;
      flangeBLength=A.flangeBLength;
      flangeBRadius=A.flangeBRadius;
      flangeALength=A.flangeALength;
      voidMat=A.voidMat;
      wallMat=A.wallMat;
      flangeMat=A.flangeMat;
    }
  return *this;
}

ButtonBPM*
ButtonBPM::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new ButtonBPM(*this);
}

ButtonBPM::~ButtonBPM()
  /*!
    Destructor
  */
{}

void
ButtonBPM::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("ButtonBPM","populate");

  FixedOffset::populate(Control);

  radius=Control.EvalVar<double>(keyName+"Radius");
  length=Control.EvalVar<double>(keyName+"Length");
  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");
  outerRadius=Control.EvalVar<double>(keyName+"OuterRadius");

  flangeInnerRadius=Control.EvalVar<double>(keyName+"FlangeInnerRadius");
  flangeARadius=Control.EvalHead<double>(keyName,"FlangeARadius","FlangeRadius");
  flangeALength=Control.EvalHead<double>(keyName,"FlangeALength","FlangeLength");
  flangeBRadius=Control.EvalHead<double>(keyName,"FlangeBRadius","FlangeRadius");
  flangeBLength=Control.EvalHead<double>(keyName,"FlangeBLength","FlangeLength");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  flangeMat=ModelSupport::EvalMat<int>(Control,keyName+"FlangeMat");

  return;
}

void
ButtonBPM::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: object for origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("ButtonBPM","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
ButtonBPM::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("ButtonBPM","createSurfaces");

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

  // main pipe and thicness
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,flangeInnerRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,Y,outerRadius);

  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,flangeARadius);
  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,Y,flangeBRadius);

  const double flangeMaxRadius = std::max(flangeARadius,flangeBRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+307,Origin,Y,flangeMaxRadius);

  ModelSupport::buildPlane(SMap,buildIndex+101,
                           Origin-Y*(length/2.0-flangeALength),Y);
  ModelSupport::buildPlane(SMap,buildIndex+202,
                           Origin+Y*(length/2.0-flangeBLength),Y);

  return;
}

void
ButtonBPM::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("ButtonBPM","createObjects");

  std::string Out;
  const std::string frontStr(frontRule());
  const std::string backStr(backRule());

  Out=ModelSupport::getComposite(SMap,buildIndex," -7 ")+frontStr+backStr;
  makeCell("InnerVoid",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 7 -27 ")+frontStr+backStr;
  makeCell("Wall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 27 -107 -101 ")+frontStr;
  makeCell("FlangeA",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 27 -207 202 ")+backStr;
  makeCell("FlangeB",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 27 -307 101 -202 ");
  makeCell("VoidBWFlanges",System,cellIndex++,voidMat,0.0,Out);

  if (flangeARadius>flangeBRadius+Geometry::zeroTol)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex," 207 -307 202 ") + backStr;
      makeCell("FlangeBVoid",System,cellIndex++,voidMat,0.0,Out);
    }
  else if (flangeARadius+Geometry::zeroTol<flangeBRadius)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex," 107 -307 -101 ") + frontStr;
      makeCell("FlangeAVoid",System,cellIndex++,voidMat,0.0,Out);
    }

  Out=ModelSupport::getComposite(SMap,buildIndex," -307 ")+frontStr+backStr;
  addOuterSurf(Out);

  return;
}


void
ButtonBPM::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("ButtonBPM","createLinks");

  FrontBackCut::createLinks(*this,Origin,Y);

  FixedComp::setConnect(2,Origin-X*(radius/2.0),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));

  FixedComp::setConnect(3,Origin+X*(radius/2.0),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));

  FixedComp::setConnect(4,Origin-Z*(outerRadius/2.0),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(5,Origin+Z*(outerRadius/2.0),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));

  return;
}

void
ButtonBPM::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("ButtonBPM","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // tdcSystem
