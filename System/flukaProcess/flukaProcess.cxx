/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flukaProcess/flukaProcess.cxx
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "inputParam.h"
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
#include "Zaid.h"
#include "MXcards.h"
#include "Material.h"
#include "DBMaterial.h"

#include "flukaGenParticle.h"
#include "SimFLUKA.h"
#include "strValueSet.h"
#include "cellValueSet.h"
#include "flukaPhysics.h"
#include "flukaImpConstructor.h"
#include "flukaProcess.h"

namespace flukaSystem
{

std::set<int>
getActiveUnit(const int typeFlag,const std::string& cellM)
  /*!
    Based on the typeFlag get teh cell/material/particle set
    \param typeFlag :: -1 : partilce / cell /material 
    \param cellM :: string to use as id
    \return set of index(s0
  */
{
  ELog::RegMethod RegA("flukaProcess[F]","getActiveUnit");
  
  switch (typeFlag)
    {
    case -1:
      return getActiveParticle(cellM);
    case 0:
      return getActiveCell(cellM);
    }
  return getActiveMaterial(cellM);
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

std::set<int>
getActiveMaterial(std::string material)
  /*!
    Given a material find the active cells
    \param material : material name to use
    \return set of active components
  */
{
  ELog::RegMethod RegA("flukaProcess[F]","getActiveMaterial");

  const ModelSupport::DBMaterial& DB=
    ModelSupport::DBMaterial::Instance();
    
  const std::set<int>& activeMat=DB.getActive();
  
  if (material=="All" || material=="all")
    return activeMat;
  
  bool negKey(0);
  if (material.size()>1 && material.back()=='-')
    {
      negKey=1;
      material.pop_back();
    }
  
  if (!DB.hasKey(material))
    throw ColErr::InContainerError<std::string>
      (material,"Material no present");

  const int MatNum=DB.getIndex(material);

  if (negKey)
    {
      std::set<int> activeOut(activeMat);
      activeOut.erase(MatNum);
      return activeOut;
    }
  std::set<int> activeOut;
  activeOut.insert(MatNum);
  return activeOut;
}

std::set<int>
getActiveCell(const std::string& cell)
  /*!
    Given a cell find the active cells
    \param cell : cell0 name to use
    \return set of active components
  */
{
  ELog::RegMethod RegA("flukaProcess[F]","getActiveCell");

  const ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  const std::vector<int> Cells=OR.getObjectRange(cell);
  if (Cells.empty())
    throw ColErr::InContainerError<std::string>(cell,"Empty cell");
  std::set<int> activeCell(Cells.begin(),Cells.end());

  activeCell.erase(1);
  return activeCell;
}
  
void 
setDefaultPhysics(SimFLUKA& System,
		  const mainSystem::inputParam& IParam)
  /*!
    Set the default Physics
    \param System :: Simulation
    \param IParam :: Input parameter
  */
{
  ELog::RegMethod RegA("DefPhysics[F]","setDefaultPhysics");
  
  System.setNPS(IParam.getValue<size_t>("nps"));
  System.setRND(IParam.getValue<long int>("random"));


  flukaPhysics* PC=System.getPhysics();
  if (!PC) return;

  size_t nSet=IParam.setCnt("wIMP");
  if (nSet)
    {
      flukaSystem::flukaImpConstructor A;
      for(size_t index=0;index<nSet;index++)
	A.processUnit(*PC,IParam,index);
    }
  
  nSet=IParam.setCnt("wCUT");    
  if (nSet)
    {
      flukaSystem::flukaImpConstructor A;
      for(size_t index=0;index<nSet;index++)
	A.processCUT(*PC,IParam,index);
    }
  
  nSet=IParam.setCnt("wEMF");
  if (nSet)
    {
      flukaSystem::flukaImpConstructor A;
      for(size_t index=0;index<nSet;index++)
	A.processEMF(*PC,IParam,index);
    }
  
  nSet=IParam.setCnt("wEXP");
  if (nSet)
    {
      flukaSystem::flukaImpConstructor A;
      for(size_t index=0;index<nSet;index++)
	A.processEXP(*PC,IParam,index);
    }
  
  return; 
}


} // NAMESPACE flukaSystem
