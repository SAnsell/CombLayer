/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: cosaxs/cosaxsFrontEnd.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include <iterator>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "inputParam.h"
#include "Surface.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Rules.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedGroup.h"
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "FixedOffsetGroup.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "CopiedComp.h"
#include "InnerZone.h"
#include "World.h"
#include "AttachSupport.h"
#include "generateSurf.h"
#include "ModelSupport.h"

#include "VacuumPipe.h"
#include "insertObject.h"
#include "insertCylinder.h"
#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "LCollimator.h"
#include "GateValve.h"
#include "OffsetFlangePipe.h"
#include "VacuumBox.h"
#include "portItem.h"
#include "PipeTube.h"
#include "PortTube.h"
#include "CrossPipe.h"
#include "Wiggler.h"
#include "SqrCollimator.h"
#include "BeamMount.h"
#include "HeatDump.h"
#include "UTubePipe.h"
#include "Undulator.h"
#include "R3FrontEnd.h"

#include "cosaxsFrontEnd.h"

namespace xraySystem
{

// Note currently uncopied:
  
cosaxsFrontEnd::cosaxsFrontEnd(const std::string& Key) :
  R3FrontEnd(Key),
  undulatorPipe(new xraySystem::UTubePipe(newName+"UPipe")),
  undulator(new xraySystem::Undulator(newName+"Undulator"))   
  /*!
    Constructor
    \param Key :: Name of construction key
    \param Index :: Index number
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(undulatorPipe);
  OR.addObject(undulator);
}
  
cosaxsFrontEnd::~cosaxsFrontEnd()
  /*!
    Destructor
   */
{}

void
cosaxsFrontEnd::createLinks()
  /*!
    Create a front/back link
   */
{
  setLinkSignedCopy(0,*undulator,1);
  setLinkSignedCopy(1,*lastComp,2);
  return;
}


const attachSystem::FixedComp&
cosaxsFrontEnd::buildUndulator(Simulation& System,
				MonteCarlo::Object* masterCell,
				const attachSystem::FixedComp& preFC,
				const long int preSideIndex)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
    \param masterCell :: Main cell with all components in
    \param preFC :: Initial cell
    \param preSideIndex :: Initial side index
    \return link object 
  */
{
  ELog::RegMethod RegA("cosaxsFrontEnd","buildUndulator");

  int outerCell;
  undulatorPipe->createAll(System,preFC,preSideIndex);
  outerCell=buildZone.createOuterVoidUnit(System,masterCell,*undulatorPipe,2);

  CellMap::addCell("UndulatorOuter",outerCell);
  undulatorPipe->insertInCell("FFlange",System,outerCell);
  undulatorPipe->insertInCell("BFlange",System,outerCell);
  undulatorPipe->insertInCell("Pipe",System,outerCell);

  undulator->addInsertCell(outerCell);
  undulator->createAll(System,*undulatorPipe,0);
  undulatorPipe->insertInCell("Pipe",System,undulator->getCell("Void"));


  return *undulatorPipe;
}

  
}   // NAMESPACE xraySystem

