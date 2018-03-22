/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physics/flukaImpConstructor.cxx
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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <cmath>
#include <complex> 
#include <vector>
#include <list>
#include <set>
#include <map> 
#include <string>
#include <algorithm>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
#include "NList.h"
#include "NRange.h"
#include "support.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "objectRegister.h"
#include "Zaid.h"
#include "MXcards.h"
#include "Material.h"
#include "DBMaterial.h"
#include "inputParam.h"
#include "cellValueSet.h"
#include "flukaPhysics.h"
#include "flukaImpConstructor.h"

namespace flukaSystem
{
  
void
flukaImpConstructor::processUnit(flukaPhysics& PC,
				 const mainSystem::inputParam& IParam,
				 const size_t setIndex)
/*!
    Set individual IMP based on Iparam
    \param PC :: PhysicsCards
    \param System :: Simulation
    \param IParam :: input stream
    \param setIndex :: index for the importance set
  */
{
  ELog::RegMethod RegA("flukaImpConstructor","processUnit");

  const ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  double value;
  std::string particle=IParam.getValueError<std::string>
    ("wIMP",setIndex,0,"No particle for wIMP ");

  if (particle=="help" || particle=="Help")
    {
      writeHelp(ELog::EM.Estream());
      ELog::EM<<ELog::endBasic;
      return;
    }
  
  size_t index(1);
  if (StrFunc::convert(particle,value))
    particle="all";
  else
    {
      value=IParam.getValueError<double>
	("wIMP",setIndex,index,"No value for wIMP");
      index++;
    }

  const std::string objName=IParam.getValueError<std::string>
    ("wIMP",setIndex,index,"No objName for wIMP");
  const std::vector<int> Cells=OR.getObjectRange(objName);
  if (Cells.empty())
    throw ColErr::InContainerError<std::string>(objName,"Empty cell");

  //
  for(const int CN : Cells)
    if (CN!=1)
      PC.setImp(particle,CN,value);

  return;
}

void
flukaImpConstructor::processEMF(flukaPhysics& PC,
				const mainSystem::inputParam& IParam,
				const size_t setIndex)
/*!
    Set individual EMFCUT based on IParam
    \param PC :: PhysicsCards
    \param IParam :: input stream
    \param setIndex :: index for the importance set
  */
{
  ELog::RegMethod RegA("flukaImpConstructor","processEMF");

  const ModelSupport::DBMaterial& DB=
    ModelSupport::DBMaterial::Instance();

  // must have size
  std::string material=IParam.getValueError<std::string>
    ("wEMF",setIndex,0,"No material for wEMF ");
  
  if (material=="help" || material=="Help")
    {
      writeEMFHelp(ELog::EM.Estream());
      ELog::EM<<ELog::endBasic;
      return;
    }

  const std::set<int>& activeMat=DB.getActive();

  const double V1=IParam.getValueError<double>
    ("wEMF",setIndex,1,"No value[1] for wEMF ");
  const double V2=IParam.getValueError<double>
    ("wEMF",setIndex,2,"No value[2] for wEMF ");
  
  
  if (material=="All" || material=="all")
    {
      for(const int MN : activeMat)
	PC.setEMF("emfcut",MN,V1,V2);
      return;
    }

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
      for(const int MN : activeMat)
	if (MN!=MatNum)
	  PC.setEMF("emfcut",MN,V1,V2);
      return;
    }
  PC.setEMF("emfcut",MatNum,V1,V2);
  return;
}

void
flukaImpConstructor::writeHelp(std::ostream& OX) const
  /*!
    Write out the help
    \param OX :: Output stream
  */
{
  OX<<"wIMP help :: \n";

  OX<<"-wIMP type value[double] object/range/cell  -- ::\n\n";
  
  OX<<"  particle : optional [default all]\n"
      "    -- all hadron electron low \n"
      "  value : value for importance \n"
      "  object : object name  \n"
      "         : object name:cellname\n"
      "         : cell number range\n"
      "         : cell number\n"
      "         : all\n";
  return;
}

void
flukaImpConstructor::writeEMFHelp(std::ostream& OX) const
  /*!
    Write out the help
    \param OX :: Output stream
  */
{
  OX<<"wEMF help :: \n";

  OX<<"-wEMF material eCut[double] gammaCut[double]  -- ::\n\n";
  
  OX<<"  material : "
      "    -- material- :: All without material\n"
      "    -- all- :: All materials\n"
      "    -- material :: Only material\n"
      "  eCut : value for electron cut [MeV] \n"
      "  gammaCut : value for gamma [MeV]\n";

  return;
}

} // Namespace flukaSystem
