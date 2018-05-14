/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/vor/DetectorTank.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include <numeric>
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
#include "surfDivide.h"
#include "surfDIter.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
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
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "DetectorTank.h"

namespace essSystem
{

DetectorTank::DetectorTank(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,6),attachSystem::CellMap(),
  tankIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(tankIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

DetectorTank::DetectorTank(const DetectorTank& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  attachSystem::CellMap(A),
  tankIndex(A.tankIndex),cellIndex(A.cellIndex),
  innerRadius(A.innerRadius),outerRadius(A.outerRadius),
  midAngle(A.midAngle),height(A.height),innerThick(A.innerThick),
  frontThick(A.frontThick),backThick(A.backThick),
  roofThick(A.roofThick),wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: DetectorTank to copy
  */
{}

DetectorTank&
DetectorTank::operator=(const DetectorTank& A)
  /*!
    Assignment operator
    \param A :: DetectorTank to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      cellIndex=A.cellIndex;
      innerRadius=A.innerRadius;
      outerRadius=A.outerRadius;
      midAngle=A.midAngle;
      height=A.height;
      innerThick=A.innerThick;
      frontThick=A.frontThick;
      backThick=A.backThick;
      roofThick=A.roofThick;
      wallMat=A.wallMat;
    }
  return *this;
}


DetectorTank::~DetectorTank() 
 /*!
   Destructor
 */
{}

void
DetectorTank::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: Database of variable
 */
{
  ELog::RegMethod RegA("DetectorTank","populate");
  
  FixedOffset::populate(Control);

  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");
  outerRadius=Control.EvalVar<double>(keyName+"OuterRadius");

  height=Control.EvalVar<double>(keyName+"Height");
  midAngle=Control.EvalVar<double>(keyName+"MidAngle");

  innerThick=Control.EvalVar<double>(keyName+"InnerThick");
  frontThick=Control.EvalVar<double>(keyName+"FrontThick");
  backThick=Control.EvalVar<double>(keyName+"BackThick");
  frontThick=Control.EvalVar<double>(keyName+"FrontThick");
  roofThick=Control.EvalVar<double>(keyName+"RoofThick");

  // Material
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  return;
}
  
void
DetectorTank::createUnitVector(const attachSystem::FixedComp& FC,
			       const long int sideIndex)
  /*!
    Create the unit vectors
    - Y Down the beamline
    \param FC :: Linked object
    \param sideIndex :: sinde track
  */
{
  ELog::RegMethod RegA("DetectorTank","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);

  applyOffset();
  return;
}
  
void
DetectorTank::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("DetectorTank","createSurface");

  ModelSupport::buildPlaneRotAxis(SMap,tankIndex+1,
				  Origin,Y,Z,midAngle);
  const Geometry::Plane* PX=SMap.realPtr<Geometry::Plane>(tankIndex+1);
  ModelSupport::buildShiftedPlane(SMap,tankIndex+11,PX,-frontThick);
  

  ModelSupport::buildPlane(SMap,tankIndex+5,
			   Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,tankIndex+6,
			   Origin+Z*(height/2.0),Z);

  ModelSupport::buildPlane(SMap,tankIndex+15,
			   Origin-Z*(height/2.0+roofThick),Z);
  ModelSupport::buildPlane(SMap,tankIndex+16,
			   Origin+Z*(height/2.0+roofThick),Z);


  ModelSupport::buildCylinder(SMap,tankIndex+7,Origin,Z,innerRadius);
  ModelSupport::buildCylinder(SMap,tankIndex+8,Origin,Z,outerRadius);

  ModelSupport::buildCylinder(SMap,tankIndex+17,Origin,
			      Z,innerRadius+innerThick);
  ModelSupport::buildCylinder(SMap,tankIndex+18,Origin,
			      Z,outerRadius+backThick);

  
  return;
}

void
DetectorTank::createObjects(Simulation& System)
  /*!
    Adds the vacuum object
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("DetectorTank","createObjects");
  
  std::string Out;

  // Main voids
  Out=ModelSupport::getComposite(SMap,tankIndex,"-7 5 -6");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  addCell("SampleVoid",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,tankIndex,"1 5 -6 7 -8");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  addCell("DetVoid",cellIndex-1);

  // WALLS:
  // sample
  Out=ModelSupport::getComposite(SMap,tankIndex,"-17 7 5 -6 -1");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  addCell("Steel",cellIndex-1);
  // front wall
  Out=ModelSupport::getComposite(SMap,tankIndex,"-8 17 5 -6 -1 11");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  addCell("Steel",cellIndex-1);
  // Detector wall
  Out=ModelSupport::getComposite(SMap,tankIndex,"5 -6 11 8 -18");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  addCell("Steel",cellIndex-1);

  // Roof wall
  Out=ModelSupport::getComposite(SMap,tankIndex,"6 -16 11 -18 17");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat+1,0.0,Out));
  addCell("RoofSteel",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,tankIndex,"6 -16 -17");
  //  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  //  addCell("RoofSteel",cellIndex-1);

    // Detector wall
  Out=ModelSupport::getComposite(SMap,tankIndex,"5 -6 11 8 -18");
  //  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  //  addCell("Steel",cellIndex-1);
  

  Out=ModelSupport::getComposite(SMap,tankIndex,"5 -16 ((11 -18) : -17)");
  addOuterSurf(Out);
  
  return;
}

void
DetectorTank::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("DetectorTank","createLinks");
  
  return;
}

void
DetectorTank::createAll(Simulation& System,
		   const attachSystem::FixedComp& FC,
		   const long int sideIndex)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: Fixed Component to place object within
    \param sideIndex :: Link point
  */
{
  ELog::RegMethod RegA("DetectorTank","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE essSystem
