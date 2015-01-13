/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   delft/BeFullBlock.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
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
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "BeFullBlock.h"

namespace delftSystem
{

BeFullBlock::BeFullBlock(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,3),
  insertIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(insertIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

BeFullBlock::~BeFullBlock() 
 /*!
   Destructor
 */
{}

void
BeFullBlock::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: DataBase
 */
{
  ELog::RegMethod RegA("BeFullBlock","populate");


  // First get inner widths:
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");


  length=Control.EvalVar<double>(keyName+"Length");
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");


  return;
}
  
void
BeFullBlock::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    - Y Points towards the beamline
    - X Across the Face
    - Z up (towards the target)
    \param FC :: A Contained FixedComp to use as basis set
  */
{
  ELog::RegMethod RegA("BeFullBlock","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyShift(xStep,yStep,zStep);

  return;
}

void
BeFullBlock::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("BeFullBlock","createSurfaces");

  // Outer layers
  ModelSupport::buildPlane(SMap,insertIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,insertIndex+2,Origin+Y*length,Y);

  ModelSupport::buildPlane(SMap,insertIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,insertIndex+4,Origin+X*(width/2.0),X);

  ModelSupport::buildPlane(SMap,insertIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,insertIndex+6,Origin+Z*(height/2.0),Z);

  return;
}

void
BeFullBlock::createObjects(Simulation& System)
  /*!
    Adds the BeamLne components
    \param System :: Simulation to add beamline to
  */
{
  ELog::RegMethod RegA("BeFullBlock","createObjects");
  
  std::string Out;
  Out=ModelSupport::getComposite(SMap,insertIndex," 1 -2 3 -4 5 -6 ");
  addOuterSurf(Out);
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));
  
  return;
}

void
BeFullBlock::createLinks()
  /*!
    Create All the links:
    - 0 : First surface
    - 1 : Exit surface
    - 2 : Inner face
  */
{
  ELog::RegMethod RegA("BeFullBlock","createLinks");

  // FixedComp::setConnect(0,Origin,-Y);      
  // FixedComp::setConnect(1,Origin+Y*length,Y);
  // FixedComp::setConnect(2,Origin+X*outerRadius+Y*length/2.0,X);

  // FixedComp::setLinkSurf(0,SMap.realSurf(insertIndex+1));
  // FixedComp::setLinkSurf(1,SMap.realSurf(insertIndex+2));
  // FixedComp::setLinkSurf(2,SMap.realSurf(insertIndex+17));

  return;
}

void
BeFullBlock::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Global creation of the vac-vessel
    \param System :: Simulation to add vessel to
    \param FC :: Moderator Object
    \param sideIndex :: Side index
  */
{
  ELog::RegMethod RegA("BeFullBlock","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       
  return;
}

  
}  // NAMESPACE delftSystem
