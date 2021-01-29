/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flukaProcess/flukaProcess.cxx
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
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cmath>
#include <complex>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <functional>
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "inputParam.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "Zaid.h"
#include "MXcards.h"
#include "Material.h"
#include "DBMaterial.h"
#include "Process.h"

#include "flukaGenParticle.h"
#include "SimFLUKA.h"
#include "cellValueSet.h"
#include "pairValueSet.h"
#include "flukaPhysics.h"
#include "flukaDefPhysics.h"
#include "flukaImpConstructor.h"
#include "flukaProcess.h"

namespace flukaSystem
{

std::set<int>
getActiveUnit(const Simulation& System,
	      const int typeFlag,
	      const std::string& cellM)
  /*!
    Based on the typeFlag get the cell/material/particle set
    \param System :: Active Group
    \param typeFlag :: -1 : particle / cell /material 
    \param cellM :: string to use as id
    \return set of type units
  */
{
  ELog::RegMethod RegA("flukaProcess[F]","getActiveUnit");


  switch (typeFlag)
    {
    case -1:
      return getActiveParticle(cellM);
    case 0:
      return ModelSupport::getActiveCell(System,cellM);
    default:
      return ModelSupport::getActiveMaterial(System,cellM);
    }

}
  
std::set<int>
getActiveParticle(const std::string& particle)
  /*!
    Given a material find the active particle
    \param particle : name of particle to use
    \return set of active components
  */
{
  ELog::RegMethod RegA("flukaProcess[F]","getActiveParticle");

  const flukaGenParticle& GP=flukaGenParticle::Instance();

  std::set<int> activeOut;
  activeOut.emplace(GP.flukaITYP(particle));
  return activeOut;
}

} // NAMESPACE flukaSystem
