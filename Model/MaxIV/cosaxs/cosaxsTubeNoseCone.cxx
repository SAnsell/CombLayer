/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/cosaxs/cosaxsTubeNoseCone.cxx
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

#include "cosaxsTubeNoseCone.h"

namespace xraySystem
{

cosaxsTubeNoseCone::cosaxsTubeNoseCone(const std::string& Key)  :
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

cosaxsTubeNoseCone::cosaxsTubeNoseCone(const cosaxsTubeNoseCone& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),
  length(A.length),frontPlateWidth(A.frontPlateWidth),frontPlateHeight(A.frontPlateHeight),
  frontPlateThick(A.frontPlateThick),
  backPlateWidth(A.backPlateWidth),
  backPlateHeight(A.backPlateHeight),
  backPlateThick(A.backPlateThick),
  backPlateRimThick(A.backPlateRimThick),
  flangeRadius(A.flangeRadius),
  flangeLength(A.flangeLength),
  pipeLength(A.pipeLength),
  pipeRadius(A.pipeRadius),
  pipeWallThick(A.pipeWallThick),
  wallThick(A.wallThick),wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: cosaxsTubeNoseCone to copy
  */
{}

cosaxsTubeNoseCone&
cosaxsTubeNoseCone::operator=(const cosaxsTubeNoseCone& A)
  /*!
    Assignment operator
    \param A :: cosaxsTubeNoseCone to copy
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
      frontPlateWidth=A.frontPlateWidth;
      frontPlateHeight=A.frontPlateHeight;
      frontPlateThick=A.frontPlateThick;
      backPlateWidth=A.backPlateWidth;
      backPlateHeight=A.backPlateHeight;
      backPlateThick=A.backPlateThick;
      backPlateRimThick=A.backPlateRimThick;
      flangeRadius=A.flangeRadius;
      flangeLength=A.flangeLength;
      pipeLength=A.pipeLength;
      pipeRadius=A.pipeRadius;
      pipeWallThick=A.pipeWallThick;
      wallThick=A.wallThick;
      wallMat=A.wallMat;
    }
  return *this;
}

cosaxsTubeNoseCone*
cosaxsTubeNoseCone::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new cosaxsTubeNoseCone(*this);
}

cosaxsTubeNoseCone::~cosaxsTubeNoseCone()
  /*!
    Destructor
  */
{}

void
cosaxsTubeNoseCone::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("cosaxsTubeNoseCone","populate");

  FixedOffset::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  frontPlateWidth=Control.EvalVar<double>(keyName+"FrontPlateWidth");
  frontPlateHeight=Control.EvalVar<double>(keyName+"FrontPlateHeight");
  frontPlateThick=Control.EvalVar<double>(keyName+"FrontPlateThick");
  backPlateWidth=Control.EvalVar<double>(keyName+"BackPlateWidth");
  backPlateHeight=Control.EvalVar<double>(keyName+"BackPlateHeight");
  backPlateThick=Control.EvalVar<double>(keyName+"BackPlateThick");
  backPlateRimThick=Control.EvalVar<double>(keyName+"BackPlateRimThick");
  flangeRadius=Control.EvalVar<double>(keyName+"FlangeRadius");
  flangeLength=Control.EvalVar<double>(keyName+"FlangeLength");
  pipeLength=Control.EvalVar<double>(keyName+"PipeLength");
  pipeRadius=Control.EvalVar<double>(keyName+"PipeRadius");
  pipeWallThick=Control.EvalVar<double>(keyName+"PipeWallThick");

  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  return;
}

void
cosaxsTubeNoseCone::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: object for origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("cosaxsTubeNoseCone","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
cosaxsTubeNoseCone::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("cosaxsTubeNoseCone","createSurfaces");

  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
      FrontBackCut::setFront(SMap.realSurf(buildIndex+1));
    }

  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(flangeLength+frontPlateThick+backPlateThick+length),Y);
      FrontBackCut::setBack(-SMap.realSurf(buildIndex+2));
    }

  // back plate
  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(frontPlateWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(frontPlateWidth/2.0),X);

  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(frontPlateHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(frontPlateHeight/2.0),Z);

  // front plate
  ModelSupport::buildPlane(SMap,buildIndex+23,Origin-X*(backPlateWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+24,Origin+X*(backPlateWidth/2.0),X);

  ModelSupport::buildPlane(SMap,buildIndex+25,Origin-Z*(backPlateHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+26,Origin+Z*(backPlateHeight/2.0),Z);

  // front plate rim (band)
  ModelSupport::buildShiftedPlane(SMap,buildIndex+33,
				  SMap.realPtr<Geometry::Plane>(buildIndex+23),
				  backPlateRimThick);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+34,
				  SMap.realPtr<Geometry::Plane>(buildIndex+24),
				  -backPlateRimThick);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+35,
				  SMap.realPtr<Geometry::Plane>(buildIndex+25),
				  backPlateRimThick);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+36,
				  SMap.realPtr<Geometry::Plane>(buildIndex+26),
				  -backPlateRimThick);

  // inclined walls
  const double tv(backPlateHeight-backPlateRimThick*2+wallThick*2);
  const double th(backPlateWidth-backPlateRimThick*2+wallThick*2);
  const double thetaV = std::atan((tv-frontPlateHeight)/2.0/length)*180.0/M_PI;
  const double thetaH = std::atan((th-frontPlateWidth)/2.0/length)*180.0/M_PI;
  const double dy(frontPlateThick+pipeLength); // y origin for inclined planes

  ModelSupport::buildShiftedPlane(SMap, buildIndex+41,
	      SMap.realPtr<Geometry::Plane>(getFrontRule().getPrimarySurface()),dy);

  ModelSupport::buildShiftedPlane(SMap,buildIndex+42,
				  SMap.realPtr<Geometry::Plane>(buildIndex+41),
				  length);

  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+43,
				  Origin-X*(frontPlateWidth/2.0)+Y*(dy),X,Z,thetaH);
  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+44,
				  Origin+X*(frontPlateWidth/2.0)+Y*(dy),X,Z,-thetaH);

  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+45,
				  Origin-Z*(frontPlateHeight/2.0)+Y*(dy),Z,X,-thetaV);
  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+46,
				  Origin+Z*(frontPlateHeight/2.0)+Y*(dy),Z,X,thetaV);

  ModelSupport::buildShiftedPlane(SMap,buildIndex+53,
				  SMap.realPtr<Geometry::Plane>(buildIndex+43),
				  wallThick);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+54,
				  SMap.realPtr<Geometry::Plane>(buildIndex+44),
				  -wallThick);

  ModelSupport::buildShiftedPlane(SMap,buildIndex+55,
				  SMap.realPtr<Geometry::Plane>(buildIndex+45),
				  wallThick);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+56,
				  SMap.realPtr<Geometry::Plane>(buildIndex+46),
				  -wallThick);

  // flange
  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,pipeRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+108,Origin,Y,pipeRadius+pipeWallThick);
  ModelSupport::buildCylinder(SMap,buildIndex+109,Origin,Y,flangeRadius);
  FrontBackCut::getShiftedFront(SMap,buildIndex+101,1,Y,flangeLength);
  FrontBackCut::getShiftedFront(SMap,buildIndex+102,1,Y,pipeLength);

  return;
}

void
cosaxsTubeNoseCone::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("cosaxsTubeNoseCone","createObjects");

  std::string Out;
  const std::string frontStr(frontRule()); // start
  const std::string backStr(backRule()); // end

  Out=ModelSupport::getComposite(SMap,buildIndex," 102 107 -41 13 -14 15 -16 ");
  makeCell("FrontPlate",System,cellIndex++,wallMat,0.0,Out);

  // void outside back plate
  Out=ModelSupport::getComposite(SMap,buildIndex," 102 -41 23 -24 25 -26 (-13:14:-15:16) ");
  System.addCell(cellIndex++,0,0.0,Out);

  // trapeze area
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " 41 -42 43 -44 45 -46 (-53:54:-55:56) ");
  makeCell("Trapeze",System,cellIndex++,wallMat,0.0,Out);

  // void outside trapeze
  Out=ModelSupport::getComposite(SMap,buildIndex," 41 -42 23 -24 25 -26 (-41:42:-43:44:-45:46) ");
  System.addCell(cellIndex++,0,0.0,Out);

  // void inside trapeze
  Out=ModelSupport::getComposite(SMap,buildIndex," 41 -42 53 -54 55 -56 ");
  System.addCell(cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 42 23 -24 25 -26 (-33:34:-35:36)");
  makeCell("BackPlateRim",System,cellIndex++,wallMat,0.0,Out+backStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 42 33 -34 35 -36 ");
  makeCell("BackPlateVoid",System,cellIndex++,0,0.0,Out+backStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 101 -102 107 -108 ");
  makeCell("Pipe",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -107 -41 ");
  makeCell("PipeVoidInside",System,cellIndex++,0,0.0,Out+frontStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 101 -102 108 23 -24 25 -26");
  makeCell("PipeVoidOutside",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -101 107 -109 ");
  makeCell("Flange",System,cellIndex++,wallMat,0.0,Out+frontStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," -101 109 23 -24 25 -26");
  makeCell("FlangeVoidOutside",System,cellIndex++,0,0.0,Out+frontStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 23 -24 25 -26 ");
  addOuterSurf(Out+frontStr+backStr);

  return;
}


void
cosaxsTubeNoseCone::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("cosaxsTubeNoseCone","createLinks");

  FrontBackCut::createLinks(*this,Origin,Y);

  FixedComp::setConnect(2,Origin-X*(frontPlateWidth/2.0),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));

  FixedComp::setConnect(3,Origin+X*(frontPlateWidth/2.0),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));

  FixedComp::setConnect(4,Origin-Z*(frontPlateHeight/2.0),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(5,Origin+Z*(frontPlateHeight/2.0),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));

  return;
}

void
cosaxsTubeNoseCone::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("cosaxsTubeNoseCone","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // xraySystem
