/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/HeatDump.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include "surfRegister.h"
#include "objectRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "support.h"
#include "stringCombine.h"
#include "inputParam.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "HeatDump.h"


namespace xraySystem
{

HeatDump::HeatDump(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,8),
  attachSystem::ExternalCut(),
  attachSystem::CellMap()
  /*!
    Constructor
    \param Key :: Name of construction key
    \param Index :: Index number
  */
{}


HeatDump::~HeatDump()
  /*!
    Destructor
   */
{}

void
HeatDump::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("HeatDump","populate");

  FixedOffset::populate(Control);
  
  radius=Control.EvalVar<double>(keyName+"Radius");
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  thick=Control.EvalVar<double>(keyName+"Thick");

  cutHeight=Control.EvalVar<double>(keyName+"CutHeight");
  cutAngle=Control.EvalVar<double>(keyName+"CutAngle");
  cutDepth=Control.EvalVar<double>(keyName+"CutDepth");

  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  return;
}

void
HeatDump::createUnitVector(const attachSystem::FixedComp& FC,
                               const long int sideIndex)
  /*!
    Create the unit vectors.
    Note that it also set the view point that neutrons come from
    \param FC :: FixedComp for origin
    \param sideIndex :: direction for link
  */
{
  ELog::RegMethod RegA("HeatDump","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  ELog::EM<<"Oring == "<<Origin<<ELog::endDiag;
  ELog::EM<<"Z == "<<Z<<ELog::endDiag;
  return;
}

void
HeatDump::createSurfaces()
  /*!
    Create planes for mirror block and support
  */
{
  ELog::RegMethod RegA("HeatDump","createSurfaces");
  
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Z,radius);
  
  if (!isActive("mountSurf"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);
      setCutSurf("mountSurf",-SMap.realSurf(buildIndex+6));
    }

  // cut surfaces

  // base durface
  const Geometry::Vec3D ZCut
    (Origin-Z*(height/2-cutHeight)+Y*cutDepth);
  ModelSupport::buildPlane(SMap,buildIndex+15,ZCut,Z);
  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+16,ZCut,Z,X,-cutAngle);
			   
  return; 
}

void
HeatDump::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("HeatDump","createObjects");

  const std::string mountSurf(ExternalCut::getRuleStr("mountSurf"));

  std::string Out;
  Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 -7 5 (-15:16) " );
  makeCell("Dump",System,cellIndex++,mat,0.0,Out+mountSurf);

  Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 -7 15 -16 " );
  makeCell("Cut",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 -7 5 " );
  addOuterSurf(Out+mountSurf);
  return; 
}

std::vector<Geometry::Vec3D>
HeatDump::calcEdgePoints() const
  /*!
    Get points to test for ContainedComp
    \return Points
   */
{
  ELog::RegMethod RegA("HeatDump","calcEdgePoints");
  
  std::vector<Geometry::Vec3D> Out;
  Out.push_back(Origin);
  Out.push_back(Origin-Z*(height/2.0));
  Out.push_back(Origin+Z*(height/2.0));
  return Out;
}
  
void
HeatDump::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("HeatDump","createLinks");
  
  return;
}

void
HeatDump::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp to construct from
    \param sideIndex :: Side point
   */
{
  ELog::RegMethod RegA("HeatDump","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System,calcEdgePoints());       

  return;
}

}  // NAMESPACE xraySystem
