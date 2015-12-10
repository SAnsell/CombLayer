/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ESSBeam/odin/RentrantBS.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
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
#include "FixedGroup.h"
#include "FixedOffset.h"
#include "FixedOffsetGroup.h"
#include "ContainedComp.h"
#include "CellMap.h"

#include "RentrantBS.h"

namespace essSystem
{

RentrantBS::RentrantBS(const std::string& Key) : 
  attachSystem::FixedOffset(Key,6),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  bstopIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(bstopIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

RentrantBS::~RentrantBS() 
  /*!
    Destructor
  */
{}

void
RentrantBS::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("RentrantBS","populate");
  
  FixedOffset::populate(Control);
  
  feRadius=Control.EvalVar<double>(keyName+"FeRadius");
  feLength=Control.EvalVar<double>(keyName+"FeLength");

  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  length=Control.EvalVar<double>(keyName+"Length");

  outerRadius=Control.EvalVar<double>(keyName+"OuterRadius");
  outerFeRadius=Control.EvalVar<double>(keyName+"OuterFeRadius");
  outerFeStep=Control.EvalVar<double>(keyName+"OuterFeStep");
  outerCut=Control.EvalVar<double>(keyName+"OuterCut");

  
  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");
  innerFeRadius=Control.EvalVar<double>(keyName+"InnerFeRadius");
  innerFeStep=Control.EvalVar<double>(keyName+"InnerFeStep");
  innerCut=Control.EvalVar<double>(keyName+"InnerCut");


  feMat=ModelSupport::EvalMat<int>(Control,keyName+"FeMat");
  concMat=ModelSupport::EvalMat<int>(Control,keyName+"ConcMat");

  return;
}

void
RentrantBS::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("RentrantBS","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
RentrantBS::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("RentrantBS","createSurfaces");

  // Concrete outer
  ModelSupport::buildPlane(SMap,bstopIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,bstopIndex+2,Origin+Y*length,Y);
  ModelSupport::buildPlane(SMap,bstopIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,bstopIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,bstopIndex+5,Origin-Z*depth,Z);
  ModelSupport::buildPlane(SMap,bstopIndex+6,Origin+Z*height,Z);  


  // Radii:
  ModelSupport::buildCylinder(SMap,bstopIndex+7,Origin,Y,feRadius);

  ModelSupport::buildCylinder(SMap,bstopIndex+107,Origin,Y,outerRadius);
  ModelSupport::buildCylinder(SMap,bstopIndex+117,Origin,Y,
			      outerRadius+outerFeRadius);  

  ModelSupport::buildCylinder(SMap,bstopIndex+207,Origin,Y,innerRadius);
  ModelSupport::buildCylinder(SMap,bstopIndex+217,Origin,Y,
			      innerRadius+innerFeRadius);  

  ModelSupport::buildPlane(SMap,bstopIndex+101,Origin+Y*outerCut,Y);
  ModelSupport::buildPlane(SMap,bstopIndex+111,
			   Origin+Y*(outerCut+outerFeStep),Y);
  ModelSupport::buildPlane(SMap,bstopIndex+201,Origin+Y*(innerCut+outerCut),Y);
  ModelSupport::buildPlane(SMap,bstopIndex+211,
			   Origin+Y*(innerCut+outerCut+innerFeStep),Y);
  ModelSupport::buildPlane(SMap,bstopIndex+301,
			   Origin+Y*(feLength+innerCut+outerCut),Y);

  return;
}

void
RentrantBS::createObjects(Simulation& System)
  /*!
    Adds the main objects
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("RentrantBS","createObjects");

  std::string Out;
  
  // Inner cylinder :
  Out=ModelSupport::getComposite(SMap,bstopIndex,"1 -101 -107");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));  

  Out=ModelSupport::getComposite(SMap,bstopIndex,"1 -101 -117 107 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,feMat,0.0,Out));  

  Out=ModelSupport::getComposite(SMap,bstopIndex,"1 -101 3 -4 5 -6 117 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,concMat,0.0,Out));  

  // innter void + next
  Out=ModelSupport::getComposite(SMap,bstopIndex,"101 -201 -207");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));  

  Out=ModelSupport::getComposite(SMap,bstopIndex,"101 -111 -117 207 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,feMat,0.0,Out));  

  Out=ModelSupport::getComposite(SMap,bstopIndex,"101 -111  117 3 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,concMat,0.0,Out));  

  // inner void surround
  Out=ModelSupport::getComposite(SMap,bstopIndex,"111 -211 -217 207 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,feMat,0.0,Out));  

  // extra concrete
  Out=ModelSupport::getComposite(SMap,bstopIndex,"111 -211  217 3 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,concMat,0.0,Out));  

  // === MAin
  // Steel section
  Out=ModelSupport::getComposite(SMap,bstopIndex,"201 -301 -207");
  System.addCell(MonteCarlo::Qhull(cellIndex++,feMat,0.0,Out));  


  // extra concrete
  Out=ModelSupport::getComposite(SMap,bstopIndex,"211 -301 3 -4 5 -6 207 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,concMat,0.0,Out));  

  // extra concrete
  Out=ModelSupport::getComposite(SMap,bstopIndex,"301 -2 3 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,concMat,0.0,Out));  

  Out=ModelSupport::getComposite(SMap,bstopIndex,"1 -2 3 -4 5 -6");
  addOuterSurf(Out);

  return;
}

void
RentrantBS::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("RentrantBS","createLinks");
  
  return;
}

void
RentrantBS::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("RentrantBS","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE essSystem
