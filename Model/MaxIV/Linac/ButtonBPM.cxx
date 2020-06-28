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
#include "Quaternion.h"

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
  length(A.length),
  innerRadius(A.innerRadius),
  outerRadius(A.outerRadius),
  flangeInnerRadius(A.flangeInnerRadius),
  flangeALength(A.flangeALength),
  flangeARadius(A.flangeARadius),
  flangeBLength(A.flangeBLength),
  flangeBRadius(A.flangeBRadius),
  flangeGap(A.flangeGap),
  buttonYAngle(A.buttonYAngle),
  buttonFlangeRadius(A.buttonFlangeRadius),
  buttonFlangeLength(A.buttonFlangeLength),
  buttonCaseLength(A.buttonCaseLength),
  buttonCaseRadius(A.buttonCaseRadius),
  buttonCaseMat(A.buttonCaseMat),
  elThick(A.elThick),
  elGap(A.elGap),
  elCase(A.elCase),
  elRadius(A.elRadius),
  elMat(A.elMat),
  ceramicThick(A.ceramicThick),
  ceramicMat(A.ceramicMat),
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
      outerRadius=A.outerRadius;
      flangeInnerRadius=A.flangeInnerRadius;
      flangeARadius=A.flangeARadius;
      flangeBLength=A.flangeBLength;
      flangeBRadius=A.flangeBRadius;
      flangeGap=A.flangeGap;
      flangeALength=A.flangeALength;
      buttonYAngle=A.buttonYAngle;
      buttonFlangeRadius=A.buttonFlangeRadius;
      buttonFlangeLength=A.buttonFlangeLength;
      buttonCaseLength=A.buttonCaseLength;
      buttonCaseRadius=A.buttonCaseRadius;
      buttonCaseMat=A.buttonCaseMat;
      elThick=A.elThick;
      elGap=A.elGap;
      elCase=A.elCase;
      elRadius=A.elRadius;
      elMat=A.elMat;
      ceramicThick=A.ceramicThick;
      ceramicMat=A.ceramicMat;
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

  length=Control.EvalVar<double>(keyName+"Length");
  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");
  outerRadius=Control.EvalVar<double>(keyName+"OuterRadius");

  flangeInnerRadius=Control.EvalVar<double>(keyName+"FlangeInnerRadius");
  flangeARadius=Control.EvalHead<double>(keyName,"FlangeARadius","FlangeRadius");
  flangeALength=Control.EvalHead<double>(keyName,"FlangeALength","FlangeLength");
  flangeBRadius=Control.EvalHead<double>(keyName,"FlangeBRadius","FlangeRadius");
  flangeBLength=Control.EvalHead<double>(keyName,"FlangeBLength","FlangeLength");
  flangeGap=Control.EvalVar<double>(keyName+"FlangeGap");

  buttonYAngle=Control.EvalVar<double>(keyName+"ButtonYAngle");
  buttonFlangeRadius=Control.EvalVar<double>(keyName+"ButtonFlangeRadius");
  buttonFlangeLength=Control.EvalVar<double>(keyName+"ButtonFlangeLength");
  buttonCaseLength=Control.EvalVar<double>(keyName+"ButtonCaseLength");
  buttonCaseRadius=Control.EvalVar<double>(keyName+"ButtonCaseRadius");
  buttonCaseMat=ModelSupport::EvalMat<int>(Control,keyName+"ButtonCaseMat");

  elThick=Control.EvalVar<double>(keyName+"ElectrodeThick");
  elGap=Control.EvalVar<double>(keyName+"ElectrodeGap");
  elCase=Control.EvalVar<double>(keyName+"ElectrodeCase");
  elRadius=Control.EvalVar<double>(keyName+"ElectrodeRadius");
  elMat=ModelSupport::EvalMat<int>(Control,keyName+"ElectrodeMat");
  ceramicThick=Control.EvalVar<double>(keyName+"CeramicThick");
  ceramicMat=ModelSupport::EvalMat<int>(Control,keyName+"CeramicMat");

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

  // pipe and flanges
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,innerRadius);
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

  ModelSupport::buildPlane(SMap,buildIndex+111,
                           Origin-Y*(length/2.0-flangeALength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+212,
                           Origin+Y*(length/2.0-flangeBLength/2.0),Y);

  ModelSupport::buildPlane(SMap,buildIndex+121,
                           Origin-Y*(length/2.0-flangeALength-flangeGap),Y);
  ModelSupport::buildPlane(SMap,buildIndex+222,
                           Origin+Y*(length/2.0-flangeBLength-flangeGap),Y);

  // buttons
  const Geometry::Quaternion QV = Geometry::Quaternion::calcQRotDeg(buttonYAngle, Y);
  const Geometry::Vec3D MX=QV.rotate(Z);

  ModelSupport::buildCylinder(SMap,buildIndex+407,Origin,MX,buttonFlangeRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+408,Origin,MX,buttonCaseRadius);
  ModelSupport::buildPlane(SMap,buildIndex+405,
                           Origin+MX*(outerRadius-buttonCaseLength),MX);
  ModelSupport::buildPlane(SMap,buildIndex+406,
                           Origin+MX*(outerRadius),MX);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+416,buildIndex+406,MX,-buttonFlangeLength);

  // electrode
  ModelSupport::buildShiftedPlane(SMap,buildIndex+506,buildIndex+405,MX,elThick);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+516,buildIndex+506,MX,elGap);
  ModelSupport::buildCylinder(SMap,buildIndex+507,Origin,MX,elRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+508,Origin,MX,elRadius+elGap/2.0);
  ModelSupport::buildCylinder(SMap,buildIndex+509,Origin,MX,elRadius+elGap/2.0+elCase);

  // ceramic
  ModelSupport::buildShiftedPlane(SMap,buildIndex+525,buildIndex+516,MX,elCase);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+526,buildIndex+516,MX,ceramicThick);
  ModelSupport::buildCylinder(SMap,buildIndex+517,Origin,MX,buttonCaseRadius-elCase);

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

  // pipe and flanges
  Out=ModelSupport::getComposite(SMap,buildIndex," 407 -7 ")+frontStr+backStr;
  makeCell("InnerVoid",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 7 -27 121 -222 407 ");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 7 -17 111 -121 ");
  makeCell("WallA",System,cellIndex++,wallMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 7 -17 222 -212 ");
  makeCell("WallB",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 17 -27 101 -121 ");
  makeCell("GapA",System,cellIndex++,voidMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 17 -27 222 -202 ");
  makeCell("GapB",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 27 -107 -101 ")+frontStr;
  makeCell("FlangeA",System,cellIndex++,flangeMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 17 -27 111 -101 ");
  makeCell("FlangeA",System,cellIndex++,flangeMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 7 -27 -111 ")+frontStr;
  makeCell("FlangeAVoid",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 27 -207 202 ")+backStr;
  makeCell("FlangeB",System,cellIndex++,flangeMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 17 -27 202 -212 ");
  makeCell("FlangeB",System,cellIndex++,flangeMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 7 -27 212 ")+backStr;
  makeCell("FlangeBVoid",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 27 -307 101 -202 407 ");
  makeCell("VoidBWFlanges",System,cellIndex++,voidMat,0.0,Out);

  if (flangeBRadius+Geometry::zeroTol<flangeARadius)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex," 207 -307 202 ") + backStr;
      makeCell("FlangeBVoid",System,cellIndex++,voidMat,0.0,Out);
    }
  else if (flangeARadius+Geometry::zeroTol<flangeBRadius)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex," 107 -307 -101 ") + frontStr;
      makeCell("FlangeAVoid",System,cellIndex++,voidMat,0.0,Out);
    }

  // buttons
  Out=ModelSupport::getComposite(SMap,buildIndex," -107 -407 (-405:406) ");
  makeCell("ButtonHolder",System,cellIndex++,voidMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 416 -406 -407 ");
  makeCell("ButtonFlange",System,cellIndex++,buttonCaseMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 526 -416 -408 ");
  makeCell("ButtonCase",System,cellIndex++,buttonCaseMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 525 -416 408 -407 ");
  makeCell("ButtonCaseOut",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 516 -525 509 -407 ");
  makeCell("ButtonCaseOut",System,cellIndex++,voidMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 516 -525 408 -509 ");
  makeCell("ButtonCaseOut",System,cellIndex++,buttonCaseMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 405 -506 -507 ");
  makeCell("Electrode",System,cellIndex++,elMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 405 -506 507 -508 ");
  makeCell("ElectrodeGap",System,cellIndex++,voidMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 506 -516 -508 ");
  makeCell("ElectrodeGap",System,cellIndex++,voidMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 405 -516 508 -509 ");
  makeCell("ElectrodeCase",System,cellIndex++,buttonCaseMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 405 -516 509 -407 ");
  makeCell("ElectrodeOut",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 516 -526 -517 ");
  makeCell("Ceramic",System,cellIndex++,ceramicMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 516 -526 517 -408 ");
  makeCell("CeramicCase",System,cellIndex++,buttonCaseMat,0.0,Out);

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

  ExternalCut::createLink("front",*this,0,Origin,Y);
  ExternalCut::createLink("back", *this,1,Origin,Y);

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
