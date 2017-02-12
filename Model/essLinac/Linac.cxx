/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/Linac.cxx
 *
 * Copyright (c) 2004-2017 by Konstantin Batkov
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
#include "stringCombine.h"
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
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "inputParam.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
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
#include "FixedOffset.h"
#include "surfDBase.h"
#include "surfDIter.h"
#include "surfDivide.h"
#include "SurInter.h"
#include "mergeTemplate.h"

#include "BeamDump.h"
#include "Linac.h"

namespace essSystem
{

Linac::Linac(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,6),
  surfIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(surfIndex+1),
  beamDump(new BeamDump(Key,"BeamDump"))
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{
  ELog::RegMethod RegA("Linac","Linac(const std::string&)");
  ModelSupport::objectRegister& OR = ModelSupport::objectRegister::Instance();
  OR.addObject(beamDump);
}

Linac::Linac(const Linac& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),
  surfIndex(A.surfIndex),cellIndex(A.cellIndex),
  length(A.length),width(A.width),height(A.height),
  depth(A.depth),
  wallThick(A.wallThick),
  roofThick(A.roofThick),
  floorThick(A.floorThick),
  floorWidth(A.floorWidth),
  airMat(A.airMat),wallMat(A.wallMat),
  beamDump(A.beamDump->clone()),
  tswLength(A.tswLength),
  tswWidth(A.tswWidth),
  tswGap(A.tswGap),
  tswOffsetY(A.tswOffsetY)
  /*!
    Copy constructor
    \param A :: Linac to copy
  */
{}

Linac&
Linac::operator=(const Linac& A)
  /*!
    Assignment operator
    \param A :: Linac to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      cellIndex=A.cellIndex;
      length=A.length;
      width=A.width;
      height=A.height;
      depth=A.depth;
      wallThick=A.wallThick;
      roofThick=A.roofThick;
      floorThick=A.floorThick;
      floorWidth=A.floorWidth;
      airMat=A.airMat;
      wallMat=A.wallMat;
      *beamDump=*A.beamDump;
      tswLength=A.tswLength;
      tswWidth=A.tswWidth;
      tswGap=A.tswGap;
      tswOffsetY=A.tswOffsetY;
    }
  return *this;
}

Linac::~Linac()
  /*!
    Destructor
  */
{}

void
Linac::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("Linac","populate");

  FixedOffset::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  roofThick=Control.EvalVar<double>(keyName+"RoofThick");
  floorThick=Control.EvalVar<double>(keyName+"FloorThick");
  floorWidth=Control.EvalVar<double>(keyName+"FloorWidth");

  airMat=ModelSupport::EvalMat<int>(Control,keyName+"AirMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  tswLength=Control.EvalVar<double>(keyName+"TSWLength");
  tswWidth=Control.EvalVar<double>(keyName+"TSWWidth");
  tswGap=Control.EvalVar<double>(keyName+"TSWGap");
  tswOffsetY=Control.EvalVar<double>(keyName+"TSWOffsetY");

  return;
}

void
Linac::createUnitVector(const attachSystem::FixedComp& FC,
                        const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: object for origin
    \param sideIndex :: sideIndex
  */
{
  ELog::RegMethod RegA("Linac","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
Linac::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("Linac","createSurfaces");

  ModelSupport::buildPlane(SMap,surfIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,surfIndex+2,Origin+Y*(length/2.0),Y);

  ModelSupport::buildPlane(SMap,surfIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,surfIndex+4,Origin+X*(width/2.0),X);

  ModelSupport::buildPlane(SMap,surfIndex+5,Origin-Z*(depth),Z);
  ModelSupport::buildPlane(SMap,surfIndex+6,Origin+Z*(height),Z);

  ModelSupport::buildPlane(SMap,surfIndex+11,Origin-Y*(length/2.0+wallThick),Y);
  ModelSupport::buildPlane(SMap,surfIndex+12,Origin+Y*(length/2.0+wallThick),Y);

  ModelSupport::buildPlane(SMap,surfIndex+13,Origin-X*(width/2.0+wallThick),X);
  ModelSupport::buildPlane(SMap,surfIndex+14,Origin+X*(width/2.0+wallThick),X);
  // floor
  ModelSupport::buildPlane(SMap,surfIndex+23,Origin-X*(floorWidth/2.0),X);
  ModelSupport::buildPlane(SMap,surfIndex+24,Origin+X*(floorWidth/2.0),X);

  ModelSupport::buildPlane(SMap,surfIndex+15,Origin-Z*(depth+floorThick),Z);
  ModelSupport::buildPlane(SMap,surfIndex+16,Origin+Z*(height+roofThick),Z);

  // Temporary shielding walls
  double tswY(tswOffsetY);
  ModelSupport::buildPlane(SMap,surfIndex+101,Origin+Y*(tswY),Y);
  ModelSupport::buildPlane(SMap,surfIndex+103,Origin-X*(width/2.0-tswLength),X);
  ModelSupport::buildPlane(SMap,surfIndex+104,Origin+X*(width/2.0-tswLength),X);
  tswY += tswWidth;
  ModelSupport::buildPlane(SMap,surfIndex+102,Origin+Y*(tswY),Y);
  tswY += tswGap;
  ModelSupport::buildPlane(SMap,surfIndex+111,Origin+Y*(tswY),Y);
  tswY += tswWidth;
  ModelSupport::buildPlane(SMap,surfIndex+112,Origin+Y*(tswY),Y);

  return;
}

void
Linac::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("Linac","createObjects");

  std::string Out;
  Out=ModelSupport::getComposite(SMap,surfIndex," 1 -101 3 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,airMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,surfIndex," 102 -111 3 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,airMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,surfIndex," 112 -2 3 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,airMat,0.0,Out));

  // side walls and roof
  Out=ModelSupport::getComposite(SMap,surfIndex,
				 " 11 -12 13 -14 5 -16 (-1:2:-3:4:6) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  // wall bottom slab
  Out=ModelSupport::getComposite(SMap,surfIndex," 11 -12 23 -24 15 -5 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,surfIndex," 11 -12 23 -13 5 -16 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  Out=ModelSupport::getComposite(SMap,surfIndex," 11 -12 14 -24 5 -16 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  // temporary shielding walls
  // 1st wall
  Out=ModelSupport::getComposite(SMap,surfIndex," 101 -102 3 -103 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,surfIndex," 101 -102 103 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,airMat,0.0,Out));

  // 2nd wall
  Out=ModelSupport::getComposite(SMap,surfIndex," 111 -112 3 -104 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,airMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,surfIndex," 111 -112 104 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex," 11 -12 23 -24 15 -16 ");
  addOuterSurf(Out);

  return;
}


void
Linac::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("Linac","createLinks");

  //  FixedComp::setConnect(0,Origin,-Y);
  //  FixedComp::setLinkSurf(0,-SMap.realSurf(surfIndex+1));

  return;
}




void
Linac::createAll(Simulation& System,
                 const attachSystem::FixedComp& FC,
                 const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("Linac","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createLinks();
  createObjects(System);
  insertObjects(System);

  beamDump->addInsertCell(surfIndex+1); // main cell
  beamDump->createAll(System,*this,0);

  return;
}

}  // essSystem essSystem
