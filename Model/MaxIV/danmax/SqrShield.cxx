/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/danmax/SqrShield.cxx
 *
 * Copyright (c) 2004-2026 by Stuart Ansell and Udo Friman-Gayer
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
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"

#include "SqrShield.h"

namespace xraySystem
{

SqrShield::SqrShield(const std::string& Key)  :
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::FrontBackCut(),
  voidMat(0)
 /*!
    Constructor BUT ALL variables are left unpopulated.
    \param Key :: Name for item in search
  */
{
}

SqrShield::~SqrShield()
  /*!
    Destructor
  */
{}

void
SqrShield::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("SqrShield","populate");

  FixedRotate::populate(Control);

  smallWidth=Control.EvalVar<double>(keyName+"SmallWidth");
  largeWidth=Control.EvalVar<double>(keyName+"LargeWidth");
  smallHeight=Control.EvalVar<double>(keyName+"SmallHeight");
  largeHeight=Control.EvalVar<double>(keyName+"LargeHeight");
  length=Control.EvalDefVar<double>(keyName+"Length",0.0);
  largeRegionStart=Control.EvalVar<double>(keyName+"LargeRegionStart");
  largeRegionLength=Control.EvalVar<double>(keyName+"LargeRegionLength");
  topThick=Control.EvalVar<double>(keyName+"TopThick");
  bottomThick=Control.EvalVar<double>(keyName+"BottomThick");
  leftThick=Control.EvalVar<double>(keyName+"LeftThick");
  rightThick=Control.EvalVar<double>(keyName+"RightThick");
  frontBackThick=Control.EvalVar<double>(keyName+"FrontBackThick");
  skinThick=Control.EvalVar<double>(keyName+"SkinThick");

  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  skinMat=ModelSupport::EvalMat<int>(Control,keyName+"SkinMat");
  voidMat=ModelSupport::EvalDefMat(Control,keyName+"VoidMat",0);

  return;
}

double SqrShield::dXYZ(const unsigned int n, const unsigned int i) const {
  double dY = 0.0;
  switch(n){
    case 0:
      return 0.0;
    case 1:
      return skinThick;
    case 2:
    case 3:
      dY = (n-1)*skinThick;
      switch(i){
        case 1:
        case 2:
          return dY+frontBackThick;
        case 3:
          return dY+rightThick;
        case 4:
          return dY+leftThick;
        case 5:
          return dY+bottomThick;
        case 6:
          return dY+topThick;
        default:
          return 0.0;
      }
    default:
      return 0.0;
  }
}

void
SqrShield::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("SqrShield","createSurfaces");

  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
      FrontBackCut::setFront(SMap.realSurf(buildIndex+1));
    }

  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
      FrontBackCut::setBack(-SMap.realSurf(buildIndex+2));
    }

  int BI;
  for(unsigned int n=0;n<4;++n){
    BI = n*10;
    getShiftedFront(SMap,buildIndex+11+BI,Y,largeRegionStart-dXYZ(n,1));
    getShiftedFront(SMap,buildIndex+12+BI,Y,largeRegionStart+largeRegionLength+dXYZ(n,2));

    ModelSupport::buildPlane(SMap,buildIndex+3+BI,Origin-X*(smallWidth/2.0+dXYZ(n, 3)),X);
    ModelSupport::buildPlane(SMap,buildIndex+4+BI,Origin+X*(smallWidth/2.0+dXYZ(n, 4)),X);
    ModelSupport::buildPlane(SMap,buildIndex+5+BI,Origin-Z*(smallHeight/2.0+dXYZ(n, 5)),Z);
    ModelSupport::buildPlane(SMap,buildIndex+6+BI,Origin+Z*(smallHeight/2.0+dXYZ(n, 6)),Z);

    ModelSupport::buildPlane(SMap,buildIndex+103+BI,Origin-X*(largeWidth/2.0+dXYZ(n, 3)),X);
    ModelSupport::buildPlane(SMap,buildIndex+104+BI,Origin+X*(largeWidth/2.0+dXYZ(n, 4)),X);
    ModelSupport::buildPlane(SMap,buildIndex+105+BI,Origin-Z*(largeHeight/2.0+dXYZ(n, 5)),Z);
    ModelSupport::buildPlane(SMap,buildIndex+106+BI,Origin+Z*(largeHeight/2.0+dXYZ(n, 6)),Z);
  }

  return;
}

HeadRule
SqrShield::getSurround() const
  /*!
    Get the inner void space
    \return HeadRule of inner void [minus front/back]
   */
{
  return ModelSupport::getHeadRule(SMap,buildIndex,"3 -4 5 -6");  
}  


void
SqrShield::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("SqrShield","createObjects");

  HeadRule HR;
  const HeadRule frontHR(getFrontRule());
  const HeadRule backHR(getBackRule());

  makeCell("FrontOuterVoid",System,cellIndex++,voidMat,0.0,ModelSupport::getHeadRule
    (SMap,buildIndex,"133 -134 135 -136 (-33:34:-35:36) -41")*frontHR);
  makeCell("FrontOuterSkin",System,cellIndex++,skinMat,0.0,ModelSupport::getHeadRule
    (SMap,buildIndex,"33 -34 35 -36 (-23:24:-25:26) -41")*frontHR);
  makeCell("FrontWall",System,cellIndex++,mat,0.0,ModelSupport::getHeadRule
    (SMap,buildIndex,"23 -24 25 -26 (-13:14:-15:16) -31")*frontHR);
  makeCell("FrontInnerSkin",System,cellIndex++,skinMat,0.0,ModelSupport::getHeadRule
    (SMap,buildIndex,"13 -14 15 -16 (-3:4:-5:6) -11")*frontHR);

  makeCell("LargeRegionVoid",System,cellIndex++,voidMat,0.0,ModelSupport::getHeadRule
    (SMap,buildIndex,"103 -104 105 -106 (-3:4:-5:6) 11 -12"));
  makeCell("LargeRegionOuterSkin",System,cellIndex++,skinMat,0.0,ModelSupport::getHeadRule
    (SMap,buildIndex,"133 -134 135 -136 (-123:124:-125:126) 41 -42")*frontHR);
  makeCell("LargeRegionWall",System,cellIndex++,mat,0.0,ModelSupport::getHeadRule
    (SMap,buildIndex,"123 -124 125 -126 (-113:114:-115:116) 31 -32")*frontHR);
  makeCell("LargeRegionInnerSkin",System,cellIndex++,skinMat,0.0,ModelSupport::getHeadRule
    (SMap,buildIndex,"113 -114 115 -116 (-103:104:-105:106) 11 -12")*frontHR);

  makeCell("LargeRegionFrontInnerSkin",System,cellIndex++,skinMat,0.0,ModelSupport::getHeadRule
    (SMap,buildIndex,"113 -114 115 -116 (-13:14:-15:16) -11 21")*frontHR);
  makeCell("LargeRegionFrontWall",System,cellIndex++,mat,0.0,ModelSupport::getHeadRule
    (SMap,buildIndex,"113 -114 115 -116 (-13:14:-15:16) -21 31")*frontHR);
  makeCell("LargeRegionFrontOuterSkin",System,cellIndex++,skinMat,0.0,ModelSupport::getHeadRule
    (SMap,buildIndex,"123 -124 125 -126 (-23:24:-25:26) -31 41")*frontHR);

  makeCell("LargeRegionBackInnerSkin",System,cellIndex++,skinMat,0.0,ModelSupport::getHeadRule
    (SMap,buildIndex,"113 -114 115 -116 (-13:14:-15:16) 12 -22")*backHR);
  makeCell("LargeRegionBackWall",System,cellIndex++,mat,0.0,ModelSupport::getHeadRule
    (SMap,buildIndex,"113 -114 115 -116 (-13:14:-15:16) 22 -32")*backHR);
  makeCell("LargeRegionBackOuterSkin",System,cellIndex++,skinMat,0.0,ModelSupport::getHeadRule
    (SMap,buildIndex,"123 -124 125 -126 (-23:24:-25:26) 32 -42")*backHR);

  makeCell("BackOuterVoid",System,cellIndex++,voidMat,0.0,ModelSupport::getHeadRule
    (SMap,buildIndex,"133 -134 135 -136 (-33:34:-35:36) 42")*backHR);
  makeCell("BackOuterSkin",System,cellIndex++,skinMat,0.0,ModelSupport::getHeadRule
    (SMap,buildIndex,"33 -34 35 -36 (-23:24:-25:26) 42")*backHR);
  makeCell("BackWall",System,cellIndex++,mat,0.0,ModelSupport::getHeadRule
    (SMap,buildIndex,"23 -24 25 -26 (-13:14:-15:16) 32")*backHR);
  makeCell("BackInnerSkin",System,cellIndex++,skinMat,0.0,ModelSupport::getHeadRule
    (SMap,buildIndex,"13 -14 15 -16 (-3:4:-5:6) 12")*backHR);

  addOuterSurf(ModelSupport::getHeadRule
    (SMap,buildIndex,"133 -134 135 -136")*frontHR*backHR);

  return;
}

void
SqrShield::createLinks()
  /*!
    Create all the links
  */
{
  ELog::RegMethod RegA("SqrShield","createLinks");

  FrontBackCut::createLinks(*this,Origin,Y);

  return;
}

void
SqrShield::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("SqrShield","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // xraySystem
