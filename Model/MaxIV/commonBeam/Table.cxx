/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/Table.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include "surfRegister.h"
#include "BaseVisit.h"
#include "Vec3D.h"
#include "Quaternion.h"
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
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ContainedComp.h"
#include "pipeSupport.h"
#include "Table.h"

// DEBUG
#include "BaseVisit.h"
#include "BaseModVisit.h"

#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"


namespace xraySystem
{

Table::Table(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,8),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
  /*!
    Constructor
    \param Key :: Name of construction key
    \param Index :: Index number
  */
{}


Table::~Table()
  /*!
    Destructor
   */
{}

void
Table::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("Table","populate");

  FixedRotate::populate(Control);

  width=Control.EvalVar<double>(keyName+"Width");
  thick=Control.EvalVar<double>(keyName+"Thick");
  length=Control.EvalVar<double>(keyName+"Length");

  legSize=Control.EvalVar<double>(keyName+"LegSize");

  const double C=Control.EvalDefVar<double>(keyName+"Clearance",5.0);
  clearance=1.0+(C/100.0);
  
  voidMat=ModelSupport::EvalDefMat(Control,keyName+"VoidMat",0);
  plateMat=ModelSupport::EvalMat<int>(Control,keyName+"PlateMat");
  legMat=ModelSupport::EvalMat<int>(Control,keyName+"LegMat");

  return;
}

void
Table::createSurfaces()
  /*!
    Create planes for mirror block and support
  */
{
  ELog::RegMethod RegA("Table","createSurfaces");

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(thick/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(thick/2.0),Z);

  int HI(buildIndex);
  for(size_t i=0;i<holeCentre.size();i++)
    {
      ModelSupport::buildCylinder(SMap,HI+7,holeCentre[i],Z,
				  holeRadius[i]*clearance);
      HI+=10;
    }
  return;
}

void
Table::addHole(const attachSystem::FixedComp& FC,
	       const std::string& sideName,
	       const double R)
  /*!
    Add a specific hole
    \param FC :: FixedComp
    \param sideIndex :: side point
    \param R :: Radius
   */
{
  ELog::RegMethod RegA("Table","addHole(string)");

  holeCentre.push_back(FC.getLinkPt(sideName));
  holeRadius.push_back(R);
  holeExclude.push_back(FC.getFullRule(sideName));   

  return;
}

void
Table::addHole(const attachSystem::FixedComp& FC,
	       const std::string& centName,
	       const std::string& outerName)
  /*!
    Add a specific hole
    \param FC :: FixedComp
    \param centName :: side point
    \param outerName :: side point
   */
{
  ELog::RegMethod RegA("Table","addHole(string,string)");

  const double R=FC.getLinkDistance(centName,outerName);
  holeCentre.push_back(FC.getLinkPt(centName));
  holeRadius.push_back(R);
  ELog::EM<<"Hole = "<<FC.getKeyName()<<" ::: "<<
    FC.getFullRule(outerName)<<ELog::endDiag;
  holeExclude.push_back(FC.getFullRule(outerName));   
  
  return;
}
  
void
Table::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("Table","createObjects");

  HeadRule HR;
  HeadRule Holes;

  int HI(buildIndex);
  for(size_t i=0;i<holeCentre.size();i++)
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,HI,"5 -6 -7M");
      HR*=holeExclude[i];
      makeCell("Hole"+std::to_string(i),System,cellIndex++,0,0.0,HR);
      Holes*=ModelSupport::getHeadRule(SMap,HI,"7");
      HI+=10;
    }

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2  3 -4 5 -6");
  makeCell("Table",System,cellIndex++,plateMat,0.0,HR*Holes);
  
  addOuterSurf(HR);

  return;
} 

void
Table::insertInCells(Simulation& System,
		     const std::vector<int>& cellN)
  /*!
    Insert the object into the cell: But it is tested
    \param System :: Simluation
    \param cellN :: Cells
   */
{
  ELog::RegMethod RegA("Table","insertInCell");

  System.populateCells();
  System.createObjSurfMap();

  const Geometry::Vec3D APt(Origin-Y*(length/2.0));
  const Geometry::Vec3D BPt(Origin+Y*(length/2.0));
  std::map<int,MonteCarlo::Object*> OMap;
  ModelSupport::calcLineTrack(System,APt,BPt,OMap);
  for(const int CN : cellN)
    {
      if (OMap.find(CN)!=OMap.end())
	ContainedComp::insertInCell(System,CN);
    }
  return;
}
  
void
Table::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("Table","createLinks");

  // link points are defined in the end of createSurfaces

  return;
}

void
Table::createAll(Simulation& System,
		 const attachSystem::FixedComp& FC,
		 const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp to construct from
    \param sideIndex :: Side point
   */
{
  ELog::RegMethod RegA("Table","createAll");
  populate(System.getDataBase());

  createCentredUnitVector(FC,sideIndex,length/2.0);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE xraySystem
