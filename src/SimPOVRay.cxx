/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   src/SimPOVRay.cxx
 *
 * Copyright (c) 2004-2019 by Konstantin Batkov/Stuart Ansell
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
#include <cmath>
#include <complex>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <string>
#include <algorithm>
#include <functional>
#include <numeric>
#include <iterator>
#include <memory>
#include <array>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Zaid.h"
#include "MXcards.h"
#include "Material.h"
#include "Vec3D.h"
#include "Surface.h"
#include "surfIndex.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"

#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimPOVRay.h"


SimPOVRay::SimPOVRay() : Simulation()
  /*!
    Constructor
  */
{}

SimPOVRay::SimPOVRay(const SimPOVRay& A) : Simulation(A)
 /*!
   Copy constructor
   \param A :: Simulation to copy
 */
{}

SimPOVRay&
SimPOVRay::operator=(const SimPOVRay& A)
  /*!
    Assignment operator
    \param A :: SimPOVRay to copy
    \return *this
   */
{
  if (this!=&A)
    {
      Simulation::operator=(A);
    }
  return *this;
}

void
SimPOVRay::writeCells(std::ostream& OX) const
  /*!
    Write all the cells in standard POVRay output
    type.
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("SimPOVRay","writeCells");

  OTYPE::const_iterator mp;
  for(mp=OList.begin();mp!=OList.end();mp++)
    mp->second->writePOVRay(OX);
  return;
}

void
SimPOVRay::writeSurfaces(std::ostream& OX) const
  /*!
    Write all the surfaces in standard MCNPX output
    type.
    \param OX :: Output stream
  */
{
  const ModelSupport::surfIndex::STYPE& SurMap =
    ModelSupport::surfIndex::Instance().surMap();

  for(const ModelSupport::surfIndex::STYPE::value_type& sm : SurMap)
    sm.second->writePOVRay(OX);

  OX<<std::endl;
  return;
}

void
SimPOVRay::writeMaterial(std::ostream& OX) const
  /*!
    Write all the used Materials in standard MCNPX output
    type.
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("SimPOVRay","writeMaterial");

  std::set<int> writtenMat;      ///< set of written materials
  for(const auto& [cellNum,objPtr]  : OList)
    {
      (void) cellNum;        // avoid warning -- fixed c++20
      const MonteCarlo::Material* mPtr = objPtr->getMatPtr();
      const int ID=mPtr->getID();
      if (ID && writtenMat.find(ID)==writtenMat.end())
	{
	  mPtr->writePOVRay(OX);
	  writtenMat.emplace(ID);
	}
    }

  OX << "#if (file_exists(\"povray/materials.inc\"))" << std::endl;
  OX << "#include \"povray/materials.inc\"" << std::endl;
  OX << "#end"  << std::endl;

  OX << "#if (file_exists(\"materials.inc\"))" << std::endl;
  OX << "#include \"materials.inc\"" << std::endl;
  OX << "#end"  << std::endl;

  return;
}

void
SimPOVRay::write(const std::string& Fname) const
  /*!
    Write out all the system for povray
    \param Fname :: Output file
  */
{
  ELog::RegMethod RegA("SimPOVRay","write");
  ELog::EM<<"WRITE"<<ELog::endDiag;
  std::ofstream OX(Fname.c_str());
  OX << "// POV-Ray model from CombLayer."<<std::endl;
  OX << "// This file contains only geomety." << std::endl;
  OX << "// It is supposed to be included from a .pov file with defined camera and light source" << std::endl;
  OX << std::endl;
  OX << "// Material" << std::endl;
  writeMaterial(OX);
  OX << "// Surfaces" << std::endl;
  writeSurfaces(OX);
  OX << "// Cells" << std::endl;
  writeCells(OX);
  OX.close();
  return;
}
