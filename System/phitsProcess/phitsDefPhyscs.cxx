/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   process/phitsDefPhysics.cxx
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
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "inputParam.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Tally.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "AttachSupport.h"
#include "LinkSupport.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "SimMCNP.h"
#include "SimPHITS.h"

#include "phitsProcess.h"
#include "phitsPhysics.h"
#include "phitsDefPhysics.h"

namespace phitsSystem
{  

void
setModelPhysics(SimPHITS& System,
		const mainSystem::inputParam& IParam)
  /*!
    Set the physics that needs a model
    \param System :: Simulation
    \param IParam :: Input parameter
  */
{
  ELog::RegMethod Rega("phitsDefPhysics","setModelPhysics");
  
  phitsSystem::phitsPhysics* PC=System.getPhysics();
  if (!PC) return;

  if (IParam.flag("icntl"))
    System.setICNTL(IParam.getValue<std::string>("icntl"));
      
  
  return; 
}

  
void 
setXrayPhysics(phitsPhysics& PC,
	       const mainSystem::inputParam& IParam)
  /*!
    Set the neutron Physics for PHITS run on a reactor
    \param PC :: Physcis cards
    \param IParam :: Input stream
  */
{
  ELog::RegMethod RegA("DefPhysics","setXrayPhysics");

  const std::string PModel=IParam.getValue<std::string>("physModel");
  typedef std::tuple<size_t,std::string,std::string,
		     std::string,std::string> unitTYPE;
  return; 
}


} // NAMESPACE phitsSystem
