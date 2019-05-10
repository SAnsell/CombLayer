/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/cosaxs/cosaxsDiffPump.cxx
 *
 * Copyright (c) 2019 by Konstantin Batkov
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
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "inputParam.h"
#include "HeadRule.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ReadFunctions.h"
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
#include "surfDBase.h"
#include "surfDIter.h"
#include "surfDivide.h"
#include "SurInter.h"
#include "mergeTemplate.h"

#include "cosaxsDiffPump.h"

namespace xraySystem
{

cosaxsDiffPump::cosaxsDiffPump(const std::string& Key)  :
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

cosaxsDiffPump::cosaxsDiffPump(const cosaxsDiffPump& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),
  length(A.length),width(A.width),height(A.height),
  apertureHeight(A.apertureHeight),
  apertureWidth(A.apertureWidth),
  mat(A.mat),
  flangeRadius(A.flangeRadius),
  flangeThick(A.flangeThick),
  flangeMat(A.flangeMat),
  flangeVoidWidth(A.flangeVoidWidth),
  flangeVoidHeight(A.flangeVoidHeight),
  flangeVoidThick(A.flangeVoidThick)
  /*!
    Copy constructor
    \param A :: cosaxsDiffPump to copy
  */
{}

cosaxsDiffPump&
cosaxsDiffPump::operator=(const cosaxsDiffPump& A)
  /*!
    Assignment operator
    \param A :: cosaxsDiffPump to copy
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
      width=A.width;
      height=A.height;
      apertureHeight=A.apertureHeight;
      apertureWidth=A.apertureWidth;
      mat=A.mat;
      flangeRadius=A.flangeRadius;
      flangeThick=A.flangeThick;
      flangeMat=A.flangeMat;
      flangeVoidWidth=A.flangeVoidWidth;
      flangeVoidHeight=A.flangeVoidHeight;
      flangeVoidThick=A.flangeVoidThick;
    }
  return *this;
}

cosaxsDiffPump*
cosaxsDiffPump::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new cosaxsDiffPump(*this);
}

cosaxsDiffPump::~cosaxsDiffPump()
  /*!
    Destructor
  */
{}

void
cosaxsDiffPump::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("cosaxsDiffPump","populate");

  FixedOffset::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  apertureHeight=Control.EvalVar<double>(keyName+"ApertureHeight");
  apertureWidth=Control.EvalVar<double>(keyName+"ApertureWidth");

  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  flangeRadius=Control.EvalVar<double>(keyName+"FlangeRadius");
  flangeThick=Control.EvalVar<double>(keyName+"FlangeThick");
  flangeMat=ModelSupport::EvalMat<int>(Control,keyName+"FlangeMat");
  flangeVoidWidth=Control.EvalVar<double>(keyName+"FlangeVoidWidth");
  flangeVoidHeight=Control.EvalVar<double>(keyName+"FlangeVoidHeight");
  flangeVoidThick=Control.EvalVar<double>(keyName+"FlangeVoidThick");

  return;
}

void
cosaxsDiffPump::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: object for origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("cosaxsDiffPump","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
cosaxsDiffPump::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("cosaxsDiffPump","createSurfaces");

  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+21,Origin,Y);
      FrontBackCut::setFront(SMap.realSurf(buildIndex+21));
    }

  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+22,Origin+Y*(length),Y);
      FrontBackCut::setBack(-SMap.realSurf(buildIndex+22));

    }

  // main cell
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin+Y*(flangeThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length-flangeThick),Y);

  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

  // aperture
  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(apertureWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(apertureWidth/2.0),X);

  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(apertureHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(apertureHeight/2.0),Z);

  // flange
  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,Y,flangeRadius);
  // rectangular void inside flange
  ModelSupport::buildShiftedPlane(SMap, buildIndex+31,
	     SMap.realPtr<Geometry::Plane>(getFrontRule().getPrimarySurface()),
				  flangeVoidThick);
  ModelSupport::buildShiftedPlane(SMap, buildIndex+32,
	     SMap.realPtr<Geometry::Plane>(getBackRule().getPrimarySurface()),
				  -flangeVoidThick);
  ModelSupport::buildPlane(SMap,buildIndex+33,Origin-X*(flangeVoidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+34,Origin+X*(flangeVoidWidth/2.0),X);

  ModelSupport::buildPlane(SMap,buildIndex+35,Origin-Z*(flangeVoidHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+36,Origin+Z*(flangeVoidHeight/2.0),Z);

  return;
}

void
cosaxsDiffPump::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("cosaxsDiffPump","createObjects");

  std::string Out;
  const std::string frontStr(frontRule());
  const std::string backStr(backRule());
  //  const std::string frontCompl(frontComplement());
  //  const std::string backCompl(backComplement());
  Out=ModelSupport::getComposite(SMap,buildIndex," -1 -27 (-33:34:-35:36) ");
  makeCell("FlangeBack",System,cellIndex++,flangeMat,0.0,Out+frontStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 2 -27 (-33:34:-35:36) ");
  makeCell("FlangeFront",System,cellIndex++,flangeMat,0.0,Out+backStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," -31 33 -34 35 -36 ");
  makeCell("FlangeBackVoid",System,cellIndex++,0,0.0,Out+frontStr);

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " 1 -31 3 -4 5 -6 (-33:34:-35:36) ");
  System.addCell(cellIndex++,flangeMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 32 33 -34 35 -36 ");
  makeCell("FlangeFrontVoid",System,cellIndex++,0,0.0,Out+backStr);

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " 32 -2 3 -4 5 -6 (-33:34:-35:36) ");
  System.addCell(cellIndex++,flangeMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 -27 (-3:4:-5:6) ");
  makeCell("VoidBetweenFlanges",System,cellIndex++,0,0.0,Out+backStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 31 -32 3 -4 5 -6 (-13:14:-15:16) ");
  makeCell("MainCell",System,cellIndex++,mat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 31 -32 13 -14 15 -16 ");
  makeCell("Aperture",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -27 ");
  addOuterSurf(Out+frontStr+backStr);

  return;
}


void
cosaxsDiffPump::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("cosaxsDiffPump","createLinks");

  FrontBackCut::createLinks(*this,Origin,Y);

  FixedComp::setConnect(2,Origin-X*(width/2.0),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));

  FixedComp::setConnect(3,Origin+X*(width/2.0),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));

  FixedComp::setConnect(4,Origin-Z*(height/2.0),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(5,Origin+Z*(height/2.0),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));

  return;
}

void
cosaxsDiffPump::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("cosaxsDiffPump","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // xraySystem
