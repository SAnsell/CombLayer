/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File: commonBeam/generalContruct.cxx
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
#include "surfIndex.h"
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
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "InnerZone.h"
#include "FrontBackCut.h"
#include "CopiedComp.h"
#include "World.h"
#include "AttachSupport.h"
#include "ModelSupport.h"
#include "generateSurf.h"

#include "insertObject.h"
#include "insertPlate.h"
#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "VacuumBox.h"
#include "portItem.h"
#include "PipeTube.h"
#include "PortTube.h"

#include "MonoVessel.h"
#include "GateValveCube.h"
#include "GateValveCylinder.h"
#include "FlangeMount.h"
#include "MonoBox.h"
#include "MonoShutter.h"
#include "BeamMount.h"
#include "DCMTank.h"

namespace xrayConstruct
{

template<typename T>
int
constructUnit(Simulation& System,
	      attachSystem::InnerZone& buildZone,
	      MonteCarlo::Object* masterCell,
	      const attachSystem::FixedComp& linkUnit,
	      const std::string& sideName,
	      T& buildUnit)
  /*!
    Construct a unit in a simgle component
    \param System :: Simulation to use
    \param masterCell :: main master cell
    \param linkUnit :: Previous link unit to use
    \param sideName :: Link point to use
    \param buildUnit :: New unit to construct
  */
{
  ELog::RegMethod RegA("generalConstruct[F]","constructUnit");

  buildUnit.setFront(linkUnit,sideName);
  buildUnit.createAll(System,linkUnit,"back");
  const int outerCell=
    buildZone.createOuterVoidUnit(System,masterCell,buildUnit,2);
  buildUnit.insertInCell(System,outerCell);
  return outerCell;
}

///\cond TEMPLATE

template int
constructUnit(Simulation&,attachSystem::InnerZone&,MonteCarlo::Object*,
	      const attachSystem::FixedComp&,const std::string&,
	      constructSystem::Bellows&);
template int
constructUnit(Simulation&,attachSystem::InnerZone&,MonteCarlo::Object*,
	      const attachSystem::FixedComp&,const std::string&,
	      constructSystem::GateValveCylinder&);
template int
constructUnit(Simulation&,attachSystem::InnerZone&,MonteCarlo::Object*,
	      const attachSystem::FixedComp&,const std::string&,
	      constructSystem::GateValveCube&);

template int
constructUnit(Simulation&,attachSystem::InnerZone&,MonteCarlo::Object*,
	      const attachSystem::FixedComp&,const std::string&,
	      constructSystem::VacuumPipe&);

///\endcond TEMPLATE

}   // NAMESPACE xrayConstruct
