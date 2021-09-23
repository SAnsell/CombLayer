/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   construct/portBuilder.cxx
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
#include <array>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Line.h"
#include "Cylinder.h"
#include "SurInter.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"

#include "portItem.h"
#include "doublePortItem.h"
#include "anglePortItem.h"
#include "portBuilder.h"

namespace constructSystem
{

portItem*
makePortItem(const FuncDataBase& Control,
	     const std::string& baseName,
	     const std::string& keyName)
  /*!
    Construct the correct kind of portItem based on the
    variable keyName+"PortType" or baseName+"PortType"
    If the variable is not present make the base object :: portItem.
    \param 
  */
{
  ELog::RegMethod RegA("portBuilder[F]","makePortItem");

  const std::string typeName=
    Control.EvalDefTail<std::string>
    (keyName,baseName,"PortType","Standard");

  if (typeName=="Standard")
    return new portItem(baseName,keyName);
  else if (typeName=="Double")
    return new doublePortItem(baseName,keyName);
  else if (typeName=="Angle")
    return new anglePortItem(baseName,keyName);

  throw ColErr::InContainerError<std::string>(typeName,keyName+"PortType");

}

portItem*
makePortItem(const FuncDataBase& Control,
	     const std::string& keyName)
  /*!
    Construct the correct kind of portItem based on the
    variable keyName+"PortType"
    If the variable is not present make the base object :: portItem.
  */
{
  ELog::RegMethod RegA("portBuilder[F]","makePortItem");

  const std::string typeName=Control.EvalDefVar<std::string>
    (keyName+"PortType","Standard");

  if (typeName=="Standard")
    return new portItem(keyName);
  else if (typeName=="Double")
    return new doublePortItem(keyName);
  else if (typeName=="Angle")
    return new anglePortItem(keyName);

  throw ColErr::InContainerError<std::string>(typeName,keyName+"PortType");

}


void
populatePort(const FuncDataBase& Control,
	     const std::string& portBase,
	     std::vector<Geometry::Vec3D>& PCentre,
	     std::vector<Geometry::Vec3D>& PAxis,
	     std::vector<std::shared_ptr<portItem>>& Ports) 
  /*!
    Builds port for whichever front/main system is needed
    \param Control :: DataBase
    \param Centre :: Centre line
    \param Axis :: Main axis
    \param Ports :: Final portitem output
  */
{
  ELog::RegMethod RegA("portBuilder[F]","populatePort");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  const size_t NPorts=Control.EvalVar<size_t>(portBase+"NPorts");
  for(size_t i=0;i<NPorts;i++)
    {
      const std::string portName=portBase+"Port"+std::to_string(i);
      const Geometry::Vec3D Centre=
	Control.EvalVar<Geometry::Vec3D>(portName+"Centre");
      const Geometry::Vec3D Axis=
	Control.EvalTail<Geometry::Vec3D>(portName,portBase,"Axis");

      std::shared_ptr<portItem> portUnit
	(makePortItem(Control,portBase,portName));

      portUnit->populate(Control);

      PCentre.push_back(Centre);
      PAxis.push_back(Axis);
      Ports.push_back(portUnit);
      OR.addObject(portUnit);
    }
  return;
}
  

  
}  // NAMESPACE constructSystem
