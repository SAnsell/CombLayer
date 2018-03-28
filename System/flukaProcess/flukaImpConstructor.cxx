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
#include "inputParam.h"
#include "cellValueSet.h"
#include "flukaPhysics.h"
#include "flukaProcess.h"
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
    \param IParam :: input stream
    \param setIndex :: index for the importance set
  */
{
  ELog::RegMethod RegA("flukaImpConstructor","processUnit");

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
  const std::set<int> activeCells=getActiveCell(objName);
  if (activeCells.empty())
    throw ColErr::InContainerError<std::string>(objName,"Empty cell");

  for(const int CN : activeCells)
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


  // cell/mat : tag name /  scale V1 / scale V2 [if used]
  typedef std::tuple<size_t,bool,std::string,double,double,double> emfTYPE;
  ELog::EM<<"EMF == "<<ELog::endDiag;
  static const std::map<std::string,emfTYPE> EMap
    ({
      { "cut",emfTYPE(2,0,"emfcut",-0.001,0.001,0) },   // cell: S2 : -GeV : GeV

      { "prodcut",emfTYPE(2,1,"prodcut",-0.001,0.001,0) }, 
      { "elpothr",emfTYPE(3,1,"elpothr",0.001,0.001,0.001) },
      { "pair",emfTYPE(2,1,"pairbrem",0.001,0.001,0) }, // mat   GeV : GeV
      { "photonuc",emfTYPE(2,1,"photonuc",0,0,0) },     // mat 
    });
  
  // must have size
  std::string type=IParam.getValueError<std::string>
    ("wEMF",setIndex,0,"No type for wEMF ");

  if (type=="help" || type=="Help")
    {
      writeEMFHelp(ELog::EM.Estream());
      ELog::EM<<ELog::endBasic;
      return;
    }

  std::map<std::string,emfTYPE>::const_iterator mc=EMap.find(type);
  if (mc==EMap.end())
    throw ColErr::InContainerError<std::string>(type,"emf type unknown");

  const std::string cellM=IParam.getValueError<std::string>
    ("wEMF",setIndex,1,"No cell/material for wEMF ");


  const size_t cellSize(std::get<0>(mc->second));
  const bool materialFlag(std::get<1>(mc->second));
  const std::string keyName(std::get<2>(mc->second));
  const double scaleA(std::get<3>(mc->second));
  const double scaleB(std::get<4>(mc->second));
  const double scaleC(std::get<5>(mc->second));


  double VV[3];
  for(size_t i=0;i<cellSize;i++)
    VV[i]=IParam.getValueError<double>
      ("wEMF",setIndex,2+i,
       "No value["+std::to_string(i+1)+"] for wEMF ");      
  
  std::set<int> activeCell=
    (!materialFlag) ? getActiveCell(cellM) : getActiveMaterial(cellM);

  if (cellSize==2)
    for(const int MN : activeCell)
      PC.setEMF(keyName,MN,VV[0]*scaleA,VV[1]*scaleB);
  else if (cellSize==3)
    for(const int MN : activeCell)
      PC.setTHR(keyName,MN,VV[0]*scaleA,VV[1]*scaleB,VV[2]*scaleC);
  else if (cellSize==0)
    for(const int MN : activeCell)
      PC.setFlag(keyName,MN);
  else if (cellSize==1)
    for(const int MN : activeCell)
      PC.setImp(keyName,MN,VV[0]*scaleA);
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
  OX<<"wEMF help :: \n"
    " - types \n"
    " -- cut \n"
    "      Cell-Range electronCut[MeV] gammaCut[MeV]\n"
    " -- pair \n"
    "      Mat-Range  \n";
  return;
}

} // Namespace flukaSystem
