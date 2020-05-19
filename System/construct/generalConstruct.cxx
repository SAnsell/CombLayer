/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File: commonBeam/generalContruct.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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

namespace constructSystem
{

int
internalUnit(Simulation& System,
	     attachSystem::InnerZone& buildZone,
	     MonteCarlo::Object* masterCell,
	     const attachSystem::FixedComp& linkUnit,
	     const std::string& sideName,
	     attachSystem::FixedComp& FC,
	     attachSystem::ExternalCut& ECut,
	     attachSystem::ContainedComp& CC)

  /*!
    Construct a unit in a simgle component
    \param System :: Simulation to use
    \param masterCell :: main master cell
    \param linkUnit :: Previous link unit to use
    \param sideName :: Link point to use
    \param buildUnit :: New unit to construct
  */
{
  ELog::RegMethod RegA("generalConstruct[F]","internalUnit");

  ECut.setCutSurf("front",linkUnit,sideName);
  FC.createAll(System,linkUnit,"back");
  const int outerCell=
    buildZone.createOuterVoidUnit(System,masterCell,FC,2);

  CC.insertInCell(System,outerCell);
  return  outerCell;
}

int
internalGroup(Simulation& System,
	     attachSystem::InnerZone& buildZone,
	     MonteCarlo::Object* masterCell,
	     const attachSystem::FixedComp& linkUnit,
	     const std::string& sideName,
	     attachSystem::FixedComp& FC,
	     attachSystem::ExternalCut& ECut,
	     attachSystem::ContainedGroup& CG,
	     const std::set<std::string>& CGunits)

  /*!
    Construct a unit in a simgle component
    \param System :: Simulation to use
    \param masterCell :: main master cell
    \param linkUnit :: Previous link unit to use
    \param sideName :: Link point to use
    \param buildUnit :: New unit to construct
  */
{
  ELog::RegMethod RegA("generalConstruct[F]","internalGroup");

  ECut.setCutSurf("front",linkUnit,sideName);
  FC.createAll(System,linkUnit,"back");
  const int outerCell=
    buildZone.createOuterVoidUnit(System,masterCell,FC,2);

  if (CGunits.empty() || CGunits.find("All")!=CGunits.end())
    CG.insertAllInCell(System,outerCell);
  else 
    {
      for(const std::string& Item : CGunits)
	CG.insertInCell(Item,System,outerCell);
    }
  return  outerCell;
}

}   // NAMESPACE constructSystem

