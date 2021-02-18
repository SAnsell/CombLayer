/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essLinac/MBCVT.cxx
 *
 * Copyright (c) 2017-2021 by Konstantin Batkov
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
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "FixedOffset.h"

#include "MBCVT.h"

namespace essSystem
{

MBCVT::MBCVT(const std::string& Base,
	     const std::string& Key,const size_t Index) :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Base+Key+std::to_string(Index),6),
  attachSystem::CellMap(),
  baseName(Base),
  extraName(Base+Key),voidMat(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

MBCVT::MBCVT(const MBCVT& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),
  attachSystem::CellMap(A),
  baseName(A.baseName),
  extraName(A.extraName),
  engActive(A.engActive),
  length(A.length),
  pipeRad(A.pipeRad),
  voidMat(A.voidMat),
  pipeMat(A.pipeMat),
  innerMat(A.pipeMat),
  outerMat(A.pipeMat),
  thermalMat(A.pipeMat),
  heliumMat(A.heliumMat)

  /*!
    Copy constructor
    \param A :: MBCVT to copy
  */
{}

MBCVT&
MBCVT::operator=(const MBCVT& A)
  /*!
    Assignment operator
    \param A :: MBCVT to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      engActive=A.engActive;
      length=A.length;
      pipeRad=A.pipeRad;
      pipeMat=A.pipeMat;
      voidMat=A.voidMat;
      innerMat=A.innerMat;
      outerMat=A.outerMat;
      heliumMat=A.heliumMat;      
    }
  return *this;
}

MBCVT*
MBCVT::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new MBCVT(*this);
}

MBCVT::~MBCVT()
  /*!
    Destructor
  */
{}

void
MBCVT::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("MBCVT","populate");

  FixedOffset::populate(Control);
  engActive=Control.EvalTriple<int>(keyName,baseName,"","EngineeringActive");

  length=Control.EvalTail<double>(keyName,extraName,"Length");
  pipeRad=Control.EvalTail<double>(keyName,extraName,"PipeRad");
  //  voidMat=Control.EvalPair<int>(keyName,extraName,"VoidMat");
  pipeMat=ModelSupport::EvalMat<int>(Control,keyName+"PipeMat");

  innerMat=ModelSupport::EvalMat<int>(Control,keyName+"InnerMat");
  outerMat=ModelSupport::EvalMat<int>(Control,keyName+"OuterMat");
  thermalMat=ModelSupport::EvalMat<int>(Control,keyName+"ThermalMat");
  heliumMat=ModelSupport::EvalMat<int>(Control,keyName+"HeliumMat");
  //waterMat=ModelSupport::EvalMat<int>(Control,baseName+"WaterMat");

  return;
}

void
MBCVT::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("MBCVT","createSurfaces");

  // Dividers
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin,X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin,Z);

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length),Y);


  ModelSupport::buildCylinder(SMap,buildIndex+10,Origin,Y,pipeRad);
  ModelSupport::buildCylinder(SMap,buildIndex+11,Origin,Y,pipeRad+0.4);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*12.15,Y);
  ModelSupport::buildPlane(SMap,buildIndex+13,Origin+Y*(length)-Y*12.15,Y);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+Y*19.2,Y);
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin+Y*(length)-Y*19.2,Y);

  ModelSupport::buildCylinder(SMap,buildIndex+16,Origin,Y,20.3);
  
  ModelSupport::buildPlane(SMap,buildIndex+17,Origin+Y*18.7,Y);
  ModelSupport::buildPlane(SMap,buildIndex+18,Origin+Y*(length)-Y*18.7,Y);
  ModelSupport::buildCylinder(SMap,buildIndex+19,Origin,Y,20.9);

  ModelSupport::buildCylinder(SMap,buildIndex+20,Origin,Y,14.6);
  ModelSupport::buildCylinder(SMap,buildIndex+21,Origin,Y,14.6+0.4);

  ModelSupport::buildCylinder(SMap,buildIndex+30,Origin-X*(21.1623)+Z*(28.6272),Y,5.0);
  ModelSupport::buildCylinder(SMap,buildIndex+31,Origin-X*(21.1623)+Z*(28.6272),Y,5.2);

  ModelSupport::buildCylinder(SMap,buildIndex+40,Origin-X*(53.0)+Z*(0.0),Y,2.0);
  ModelSupport::buildCylinder(SMap,buildIndex+41,Origin-X*(37.4767)+Z*(37.4767),Y,2.0);
  ModelSupport::buildCylinder(SMap,buildIndex+42,Origin-X*(13.7174)+Z*(51.1941),Y,2.0);
  ModelSupport::buildCylinder(SMap,buildIndex+43,Origin-X*(-13.7174)+Z*(51.1941),Y,2.0);
  ModelSupport::buildCylinder(SMap,buildIndex+44,Origin-X*(-37.4767)+Z*(37.4767),Y,2.0);
  ModelSupport::buildCylinder(SMap,buildIndex+45,Origin-X*(-53.0)+Z*(0.0),Y,2.0);
  ModelSupport::buildCylinder(SMap,buildIndex+46,Origin-X*(-37.4767)+Z*(-37.4767),Y,2.0); 
  ModelSupport::buildCylinder(SMap,buildIndex+47,Origin-X*(37.4767)+Z*(-37.4767),Y,2.0);

  ModelSupport::buildCylinder(SMap,buildIndex+50,Origin-X*(53.0)+Z*(0.0),Y,3.0);
  ModelSupport::buildCylinder(SMap,buildIndex+51,Origin-X*(37.4767)+Z*(37.4767),Y,3.0);
  ModelSupport::buildCylinder(SMap,buildIndex+52,Origin-X*(13.7174)+Z*(51.1941),Y,3.0);
  ModelSupport::buildCylinder(SMap,buildIndex+53,Origin-X*(-13.7174)+Z*(51.1941),Y,3.0);
  ModelSupport::buildCylinder(SMap,buildIndex+54,Origin-X*(-37.4767)+Z*(37.4767),Y,3.0);
  ModelSupport::buildCylinder(SMap,buildIndex+55,Origin-X*(-53.0)+Z*(0.0),Y,3.0);
  ModelSupport::buildCylinder(SMap,buildIndex+56,Origin-X*(-37.4767)+Z*(-37.4767),Y,3.0); 
  ModelSupport::buildCylinder(SMap,buildIndex+57,Origin-X*(37.4767)+Z*(-37.4767),Y,3.0);

  ModelSupport::buildCylinder(SMap,buildIndex+60,Origin,Y,60.0);
  ModelSupport::buildCylinder(SMap,buildIndex+61,Origin,Y,61.0);

  // Bottom
  double xb = 32.4;
  double zb1 = 30.2;
  double zb2 = 30.6;

  // Top
  double zt1 = 46.0;
  double zt2 = 45.6;

  ModelSupport::buildPlane(SMap,buildIndex+100,Origin+X*xb,X);
  ModelSupport::buildPlane(SMap,buildIndex+101,Origin-X*xb,X);
  ModelSupport::buildPlane(SMap,buildIndex+102,Origin-Z*zb1,Z);
  ModelSupport::buildPlane(SMap,buildIndex+103,Origin-Z*zb2,Z);

  ModelSupport::buildPlane(SMap,buildIndex+202,Origin+Z*zt1,Z);
  ModelSupport::buildPlane(SMap,buildIndex+203,Origin+Z*zt2,Z);

  ModelSupport::buildPlane(SMap,buildIndex+300,Origin+X*(xb+0.4),X);
  ModelSupport::buildPlane(SMap,buildIndex+301,Origin-X*(xb+0.4),X);
 
  return;
}

void
MBCVT::createObjects(Simulation& System)
  /*!
    Adds the all the components

    \todo This addOuterUnionSurf is AWFUL. It COMPLETE slows
    down the MCNP run. It should be basically a wrapper cell.

    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("MBCVT","createObjects");

  HeadRule HR;


  // round pipes in start and end, STST
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -12 -10");
  makeCell("Pipe",System,cellIndex++,voidMat,0.0,HR);
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -12 10 -11");
  makeCell("Pipe",System,cellIndex++,pipeMat,0.0,HR);
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"13 -2 -10 ");
  makeCell("Pipe",System,cellIndex++,voidMat,0.0,HR);
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"13 -2 10 -11");
  makeCell("Pipe",System,cellIndex++,pipeMat,0.0,HR);
  addOuterUnionSurf(HR);
  // Nb
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"12 -14 -10");
  makeCell("Pipe",System,cellIndex++,voidMat,0.0,HR);
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"12 -14 10 -11");
  makeCell("Pipe",System,cellIndex++,innerMat,0.0,HR);
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"15 -13 -10 ");
  makeCell("Pipe",System,cellIndex++,voidMat,0.0,HR);
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"15 -13 10 -11");
  makeCell("Pipe",System,cellIndex++,innerMat,0.0,HR);
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"14 -15 -20 ");
  makeCell("Pipe",System,cellIndex++,voidMat,0.0,HR);
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"14 -15 20 -21");
  makeCell("Pipe",System,cellIndex++,innerMat,0.0,HR);
  addOuterUnionSurf(HR);

  //LHe

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"14 -15 21 -16");
  makeCell("LiqHE",System,cellIndex++,heliumMat,0.0,HR);
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -30");
  makeCell("LiqHE",System,cellIndex++,heliumMat,0.0,HR);
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 30 -31");
  makeCell("LiqHE",System,cellIndex++,pipeMat,0.0,HR);
  addOuterUnionSurf(HR);
  
  //Ti 

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"17 -14 11 -16");
  makeCell("Outer",System,cellIndex++,outerMat,0.0,HR);
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"15 -18 11 -16");
  makeCell("Outer",System,cellIndex++,outerMat,0.0,HR);
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"17 -18 16 -19");
  makeCell("Outer",System,cellIndex++,outerMat,0.0,HR);
  addOuterUnionSurf(HR);

  // frame tubes

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -40");
  makeCell("Frame",System,cellIndex++,voidMat,0.0,HR);
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -41");
  makeCell("Frame",System,cellIndex++,voidMat,0.0,HR);
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -42");
  makeCell("Frame",System,cellIndex++,voidMat,0.0,HR);
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -43");
  makeCell("Frame",System,cellIndex++,voidMat,0.0,HR);
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -44");
  makeCell("Frame",System,cellIndex++,voidMat,0.0,HR);
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -45");
  makeCell("Frame",System,cellIndex++,voidMat,0.0,HR);
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -46");
  makeCell("Frame",System,cellIndex++,voidMat,0.0,HR);
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -47");
  makeCell("Frame",System,cellIndex++,voidMat,0.0,HR);
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 40 -50");
  makeCell("Frame",System,cellIndex++,pipeMat,0.0,HR);
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 41 -51");
  makeCell("Frame",System,cellIndex++,pipeMat,0.0,HR);
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 42 -52");
  makeCell("Frame",System,cellIndex++,pipeMat,0.0,HR);
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 43 -53");
  makeCell("Frame",System,cellIndex++,pipeMat,0.0,HR);
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 44 -54");
  makeCell("Frame",System,cellIndex++,pipeMat,0.0,HR);
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 45 -55");
  makeCell("Frame",System,cellIndex++,pipeMat,0.0,HR);
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 46 -56");
  makeCell("Frame",System,cellIndex++,pipeMat,0.0,HR);
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 47 -57");
  makeCell("Frame",System,cellIndex++,pipeMat,0.0,HR);
  addOuterUnionSurf(HR);

  // Simplified thermal shielding
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -60 50 51 52 53 54 55 56 57 19 31 (-103:300:202:-301)");
  makeCell("Thermal",System,cellIndex++,voidMat,0.0,HR);
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 19 31 102 -100 -203 101");
  makeCell("Thermal",System,cellIndex++,voidMat,0.0,HR);
  addOuterUnionSurf(HR);
  
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 60 -61 ");
  makeCell("Thermal",System,cellIndex++,pipeMat,0.0,HR);
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -17 11 -19 ");
  makeCell("Thermal",System,cellIndex++,voidMat,0.0,HR);
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"18 -2 11 -19 ");
  makeCell("Thermal",System,cellIndex++,voidMat,0.0,HR);
  addOuterUnionSurf(HR);

  //Thermal Shielding


  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 103 -102 -100 101 ");
  makeCell("Shield",System,cellIndex++,thermalMat,0.0,HR);
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 203 -202 -100 101 ");
  makeCell("Shield",System,cellIndex++,thermalMat,0.0,HR);
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 103 -202 100 -300 ");
  makeCell("Shield",System,cellIndex++,thermalMat,0.0,HR);
  addOuterUnionSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 103 -202 301 -101 ");
  makeCell("Shield",System,cellIndex++,thermalMat,0.0,HR);
  addOuterUnionSurf(HR);
  
  return;
}


void
MBCVT::createLinks()

  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("MBCVT","createLinks");

  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));

  FixedComp::setConnect(1,Origin+Y*(length),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+12));


  // for (int i=6; i<8; i++)
  //   ELog::EM << keyName << " " << i << "\t" << getLinkSurf(i) << "\t" << getLinkPt(i) << "\t\t" << getLinkAxis(i) << ELog::endDiag;

  return;
}




void
MBCVT::createAll(Simulation& System,
		 const attachSystem::FixedComp& FC,
		 const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("MBCVT","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // essSystem
