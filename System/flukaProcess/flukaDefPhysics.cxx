/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flukaProcess/flukaDefPhysics.cxx
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
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimMCNP.h"
#include "SimFLUKA.h"

#include "flukaImpConstructor.h"
#include "flukaProcess.h"
#include "cellValueSet.h"
#include "pairValueSet.h"
#include "flukaPhysics.h"
#include "flukaDefPhysics.h"

namespace flukaSystem
{  

void
setModelPhysics(SimFLUKA& System,
		const mainSystem::inputParam& IParam)
  /*!
    Set the physics that needs a model
    \param System :: Simulation
    \param IParam :: Input parameter
  */
{
  ELog::RegMethod Rega("flukaDefPhysics","setModelPhysics");
  
  setXrayPhysics(System,IParam);
  
  size_t nSet=IParam.setCnt("wMAT");
  if (nSet)
    {
      flukaSystem::flukaImpConstructor A;
      for(size_t index=0;index<nSet;index++)
	A.processMAT(System,IParam,index);
    }

  nSet=IParam.setCnt("wIMP");
  if (nSet)
    {
      flukaSystem::flukaImpConstructor A;
      for(size_t index=0;index<nSet;index++)
	A.processUnit(System,IParam,index);
    }
  
  nSet=IParam.setCnt("wCUT");    
  if (nSet)
    {
      flukaSystem::flukaImpConstructor A;
      for(size_t index=0;index<nSet;index++)
	A.processCUT(System,IParam,index);
    }
  
  nSet=IParam.setCnt("wEMF");
  if (nSet)
    {
      flukaSystem::flukaImpConstructor A;
      for(size_t index=0;index<nSet;index++)
	A.processEMF(System,IParam,index);
    }
  
  nSet=IParam.setCnt("wEXP");
  if (nSet)
    {
      flukaSystem::flukaImpConstructor A;
      for(size_t index=0;index<nSet;index++)
	A.processEXP(System,IParam,index);
    }

  nSet=IParam.setCnt("wLAM");    
  if (nSet)
    {
      flukaSystem::flukaImpConstructor A;
      for(size_t index=0;index<nSet;index++)
	A.processLAM(System,IParam,index);
    }
  
  return; 
}

  
void 
setXrayPhysics(SimFLUKA& System,
	       const mainSystem::inputParam& IParam)
  /*!
    Set the neutron Physics for FLUKA run on a reactor
    \param System :: Fluke sim
    \param IParam :: Input stream
  */
{
  ELog::RegMethod RegA("DefPhysics","setXrayPhysics");

  flukaPhysics& PC= *System.getPhysics();
  const std::string PModel=IParam.getValue<std::string>("physModel");
  // typedef std::tuple<size_t,std::string,std::string,
  // 		     std::string,std::string> unitTYPE;

  // CELL emfs
  const std::set<int> activeCell=getActiveUnit(System,0,"all");
  const std::set<int> activeMat=getActiveUnit(System,1,"all");
  for(const int MN : activeMat)
    {
      //Turn pair-bremstrauhlung on 
      PC.setFlag("photonuc",MN);
      // Muon photon
      PC.setFlag("muphoton",MN);
      // electronuclear
      PC.setFlag("elecnucl",MN);
      // muon pair
      PC.setFlag("mupair",MN);
      
      // Compton PhotoElectic GammaPairProductuion      
      PC.setTHR("photthr",MN,"1e-3","1e-3","1.0");
	
      // Interaction threshold : Brem-e+/e- moller scatter photonuclear"
      PC.setTHR("elpothr",MN,"0.0","1e-3","0.0");

      // Turn off multiple scattering [not a good idea]
      //      PC.setTHR("mulsopt",MN,"0","0","3");

      // Production Cut for e/e+ photon 
      PC.setEMF("prodcut",MN,"1","1e-3");
      // Rayleigh photonuc-iteratcion
      PC.setEMF("pho2thr",MN,"1e-3","1");

      // Pairbrem
      PC.setEMF("pairbrem",MN,"0.0","0.1");
    }
  //  const std::set<int> activeMat=getActiveUnit(1,"all");
  
  for(const int CN : activeCell)
    {
      // Electrons are stopped in the EMFCut disk
      PC.setEMF("emfcut",CN,"0.035","0.015");
    }
  // SPECIAL:
  PC.setIMP("partthr","neutron","1e-9");
  return; 
}


} // NAMESPACE flukaSystem
