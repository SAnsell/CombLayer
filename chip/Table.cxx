/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   chip/Table.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

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
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Tally.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
#include "surfDIter.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "chipDataStore.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "ContainedComp.h"
#include "Table.h"

namespace hutchSystem
{

Table::Table(const int T,const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,6),
  shapeType(T),tableIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(tableIndex+1),populated(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param T :: Shape type [0 : Rectangle / 1 Triangle]
    \param Key :: Name for item in search
  */
{}

Table::Table(const Table& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  shapeType(A.shapeType),tableIndex(A.tableIndex),
  cellIndex(A.cellIndex),populated(A.populated),
  xyAngle(A.xyAngle),zAngle(A.zAngle),
  fStep(A.fStep),xStep(A.xStep),Centre(A.Centre),
  height(A.height),width(A.width),length(A.length),surThick(A.surThick),
  sideThick(A.sideThick),topMat(A.topMat),defMat(A.defMat),
  floorSurf(A.floorSurf)
  /*!
    Copy constructor
    \param A :: Table to copy
  */
{}

Table&
Table::operator=(const Table& A)
  /*!
    Assignment operator
    \param A :: Table to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      populated=A.populated;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
      fStep=A.fStep;
      xStep=A.xStep;
      Centre=A.Centre;
      height=A.height;
      width=A.width;
      length=A.length;
      surThick=A.surThick;
      sideThick=A.sideThick;
      topMat=A.topMat;
      defMat=A.defMat;
      floorSurf=A.floorSurf;
    }
  return *this;
}

Table::~Table() 
  /*!
    Destructor
  */
{}

void
Table::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Table","populate");

  const FuncDataBase& Control=System.getDataBase();

  xyAngle=Control.EvalVar<double>(keyName+"XYAngle");
  zAngle=Control.EvalVar<double>(keyName+"ZAngle");
  fStep=Control.EvalVar<double>(keyName+"FStep");
  xStep=Control.EvalVar<double>(keyName+"XStep");

  height=Control.EvalVar<double>(keyName+"Height");
  width=Control.EvalVar<double>(keyName+"Width");
  length=Control.EvalVar<double>(keyName+"Length");

  surThick=Control.EvalVar<double>(keyName+"SurThick");
  sideThick=Control.EvalVar<double>(keyName+"SideThick");

  topMat=ModelSupport::EvalMat<int>(Control,keyName+"TopMat");
  defMat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  populated |= 1;
  return;
}


void
Table::createUnitVector(const attachSystem::FixedComp& TC)
  /*!
    Create the unit vectors
    \param TC :: LinearComponent to attach to
  */
{
  ELog::RegMethod RegA("Table","createUnitVector");

  //  const masterRotate& MR=masterRotate::Instance();
  //  chipIRDatum::chipDataStore& CS=chipIRDatum::chipDataStore::Instance();

  // Origin is in the wrong place as it is at the EXIT:
  FixedComp::createUnitVector(TC);
  Origin=TC.getExit();
  Origin+=X*xStep+Y*fStep;

  const Geometry::Quaternion Qz=
    Geometry::Quaternion::calcQRotDeg(zAngle,X);
  const Geometry::Quaternion Qxy=
    Geometry::Quaternion::calcQRotDeg(xyAngle,Z);

  Centre=Origin+Y*(length/2.0);  
  return;
}

void
Table::createLinks()
   /*!
     Create link points/surface for the table
   */
{
  ELog::RegMethod RegA("Table","createLinks");

  FixedComp::setConnect(0,Centre-Y*length/2.0,-Y);
  FixedComp::setConnect(1,Centre+Y*length/2.0,Y);
  FixedComp::setConnect(2,Centre-X*width/2.0,-X);
  FixedComp::setConnect(3,Centre+X*width/2.0,X);
  FixedComp::setConnect(4,Centre+Z*height,Z);
  
  return;
}


void
Table::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("Table","createTriangleSurface");
  // REGISTER FLOOR:
  SMap.addMatch(tableIndex+5,floorSurf);
  //
  // OUTER PLANES
  // 
  Geometry::Plane* PX;
  // Front
  ModelSupport::buildPlane(SMap,tableIndex+1,
			   Centre-Y*length/2.0,Y);

  // Back
  ModelSupport::buildPlane(SMap,tableIndex+2,
			   Centre+Y*length/2.0,Y);

  // sides
  switch (shapeType) 
    {
    case 0:    // Triangle
      ModelSupport::buildPlane(SMap,tableIndex+3,
			       Centre-Y*length/2.0,
			       Centre+Y*length/2.0-X*width/2.0+Z,
			       Centre+Y*length/2.0-X*width/2.0,X);
      ModelSupport::buildPlane(SMap,tableIndex+4,
			       Centre-Y*length/2.0,
			       Centre+Y*length/2.0+X*width/2.0+Z,
			       Centre+Y*length/2.0+X*width/2.0,X);
      PX=SMap.realPtr<Geometry::Plane>(tableIndex+3);
      ModelSupport::buildPlane(SMap,tableIndex+13,
			       PX->getNormal(),
			       PX->getDistance()+sideThick);
      PX=SMap.realPtr<Geometry::Plane>(tableIndex+4);
      ModelSupport::buildPlane(SMap,tableIndex+14,
			       PX->getNormal(),
			       PX->getDistance()-sideThick);
      break;
    case 1:
      ModelSupport::buildPlane(SMap,tableIndex+3,Centre-X*width/2.0,X);
      ModelSupport::buildPlane(SMap,tableIndex+4,Centre+X*width/2.0,X);
      ModelSupport::buildPlane(SMap,tableIndex+13,
			       Centre-X*(width/2.0-sideThick),X);
      ModelSupport::buildPlane(SMap,tableIndex+14,
			       Centre-X*(width/2.0-sideThick),X);
      break;
    default:
      ELog::EM<<"Error with shapeType == "<<shapeType<<ELog::endErr;
    }

  // top
  ModelSupport::buildPlane(SMap,tableIndex+6,
			   Centre+Z*height,Z);
  
  //
  // INNER SURFACES
  // 
  ModelSupport::buildPlane(SMap,tableIndex+11,
			   Centre-Y*(length/2.0-sideThick),Y);
  // Back
  ModelSupport::buildPlane(SMap,tableIndex+12,
			   Centre+Y*(length/2.0-sideThick),Y);
  // top
  ModelSupport::buildPlane(SMap,tableIndex+16,
			   Centre+Z*(height-surThick),Z);

  return;
}

void
Table::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("Table","createObjects");

  std::string Out;
  Out=ModelSupport::getComposite(SMap,tableIndex,"1 -2 3 -4 -6 ");
  addOuterSurf(Out);

  // Base Object
  Out=ModelSupport::getComposite(SMap,tableIndex,"1 -2 3 -4 5 -16 "
				 "(-11 : 12 : -13 : 14)" );
  System.addCell(MonteCarlo::Qhull(cellIndex++,defMat,0.0,Out));
  // Inner void
  Out=ModelSupport::getComposite(SMap,tableIndex,"11 -12 13 -14 5 -16 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  // Table top
  Out=ModelSupport::getComposite(SMap,tableIndex,"1 -2 3 -4 -6 16 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,topMat,0.0,Out));

  return;
}

int
Table::exitWindow(const double Dist,
		    std::vector<int>& window,
		    Geometry::Vec3D& Pt) const
  /*!
    Outputs a window
    \param Dist :: Dist from exit point
    \param window :: window vector of paired planes
    \param Pt :: Output point for tally
    \return Master Plane
  */
{
  window.clear();
  window.push_back(SMap.realSurf(tableIndex+3));
  window.push_back(SMap.realSurf(tableIndex+4));
  window.push_back(SMap.realSurf(tableIndex+5));
  window.push_back(SMap.realSurf(tableIndex+6));
  Pt=Origin+Y*(length+Dist);  
  return SMap.realSurf(tableIndex+2);
}
  
void
Table::createAll(Simulation& System,
		 const attachSystem::FixedComp& FC)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Linear component to set axis etc
  */
{
  ELog::RegMethod RegA("Table","createAll");
  populate(System);

  createUnitVector(FC);
  createSurfaces();
  createObjects(System);
  insertObjects(System);
  createLinks();
  return;
}
  
}  // NAMESPACE shutterSystem
