/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   weights/BasicWWE.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "weightManager.h"
#include "WForm.h"
#include "WItem.h"
#include "WCells.h"
#include "WeightModification.h"
#include "Simulation.h"
#include "objectRegister.h"
#include "inputParam.h"
#include "TallyCreate.h"
#include "PointWeights.h"
#include "TempWeights.h"
#include "BasicWWE.h"

namespace WeightSystem
{

void
simulationWeights(Simulation& System,
		 const mainSystem::inputParam& IParam)
  /*!
    Set individual weights based on temperature/cell
    \param System :: Simulation
    \param IParam :: input stream
   */
{
  ELog::RegMethod RegA("BasicWWE","simulationWeights");

  System.populateCells();
  System.createObjSurfMap();

  // WEIGHTS:
  if (IParam.flag("weight") || IParam.flag("tallyWeight"))
    System.calcAllVertex();

  const std::string WType=IParam.getValue<std::string>("weightType");
  setWeights(System,WType);
  if (IParam.flag("weight"))
    {
      Geometry::Vec3D AimPoint;
      if (IParam.flag("weightPt"))
	AimPoint=IParam.getValue<Geometry::Vec3D>("weightPt");
      else 
	tallySystem::getFarPoint(System,AimPoint);
      setPointWeights(System,AimPoint,IParam.getValue<double>("weight"));
    }
  if (IParam.flag("weightTemp"))
    {
      scaleTempWeights(System,10.0);
    }

  if (IParam.flag("tallyWeight"))
    tallySystem::addPointPD(System);

  return;
}

void
setWeights(Simulation& System,const std::string& Type)
  /*!
    set the basic weight either for low energy to high energy
    \param Type :: simulation type (basic/high/mid)
  */
{
  ELog::RegMethod RegA("BasicWWE","setWeights");

  if (Type=="basic")
    setWeightsBasic(System);
  else if (Type=="high")
    setWeightsHighE(System);
  else if (Type=="mid")
    setWeightsMidE(System);
  else if (Type=="help")
    {
      ELog::EM<<"Basic weight energy types == \n"
	"High -- High energy\n"
	"Mid -- Mid/old point energy\n"
	"Basic -- Cold spectrum energy"<<ELog::endBasic;
      throw ColErr::ExitAbort("End Help");
    }
  else
    throw ColErr::InContainerError<std::string>(Type,"Unknown weight type");

  return;
}

void
setWeights(Simulation& System,const std::vector<double>& Eval,
	   const std::vector<double>& WT,
	   const std::set<std::string>& excludeBoundary)
  /*!
    Function to set up the weights system.
    It replaces the old file read system.
    \param System :: Simulation component
    \param Eval :: Energy bins
    \param WT :: Weight scale
    \param excludeBoundary :: excluded boundary
  */
{
  ELog::RegMethod RegA("BasicWWE","setWeights(Simulation,vec,vec)");


  WeightSystem::weightManager& WM=
    WeightSystem::weightManager::Instance();  

  WM.addParticle<WeightSystem::WCells>('n');
  WeightSystem::WCells* WF=
    dynamic_cast<WeightSystem::WCells*>(WM.getParticle('n'));
  if (!WF)
    throw ColErr::InContainerError<std::string>("n","WCell - WM");

  WF->setEnergy(Eval);
  System.populateWCells();
  WF->balanceScale(WT);
  const Simulation::OTYPE& Cells=System.getCells();
  Simulation::OTYPE::const_iterator oc;
  for(oc=Cells.begin();oc!=Cells.end();oc++)
    {
      if(!oc->second->getImp())
	WF->maskCell(oc->first);      
    }
  WF->maskCell(1);
  return;
}

void
setWeightsHighE(Simulation& System)
  /*!
    Function to set up the weights system.
    It replaces the old file read system.
    \param System :: Simulation component
  */
{
  ELog::RegMethod RegA("BasicWWE","setWeightsHighE");
  std::vector<double> Eval(6);
  Eval[0]=1e-4;
  Eval[1]=0.01;
  Eval[2]=1;
  Eval[3]=10;
  Eval[4]=100;
  Eval[5]=5e7;

  std::vector<double> WT(6);
  WT[0]=0.002;WT[1]=0.007;WT[2]=0.02;
  WT[3]=0.07;WT[4]= 0.2;WT[5]=0.7;
  std::set<std::string> EmptySet;
  setWeights(System,Eval,WT,EmptySet);

  return;
}

void
setWeightsMidE(Simulation& System)
  /*!
    Function to set up the weights system.
    It replaces the old file read system.
    \param System :: Simulation component
  */
{
  ELog::RegMethod RegA("BasicWWE","setWeightsMidE");
  std::vector<double> Eval(6);
  Eval[0]=1e-9;
  Eval[1]=1e-5;
  Eval[2]=1e-3;
  Eval[3]=1e-1;
  Eval[4]=1;
  Eval[5]=1000;

  std::vector<double> WT(6);
  WT[0]=0.9;WT[1]=0.8;WT[2]=0.5;
  WT[3]=0.5;WT[4]= 0.5;WT[5]=0.7;
  std::set<std::string> EmptySet;
  setWeights(System,Eval,WT,EmptySet);

  return;
}

void
setWeightsBasic(Simulation& System)
  /*!
    Function to set up the weights system.
    It replaces the old file read system.
    \param System :: Simulation component
  */
{
  ELog::RegMethod RegA("BasicWWE","setWeights(Simulation)");
  std::vector<double> Eval(6);
  Eval[0]=1.4678e-10;
  Eval[1]=3.1622e-10;
  Eval[2]=6.8130e-10;
  Eval[3]=1.4678e-9;
  Eval[4]=3.1622e-9;
  Eval[5]=5e7;

  std::vector<double> WT(6);
  WT[0]=0.02;WT[1]=0.07;WT[2]=0.1;
  WT[3]=0.1;WT[4]= 0.2;WT[5]=0.7;

  std::set<std::string> EmptySet;
  setWeights(System,Eval,WT,EmptySet);
  return;
}

}  // NAMESPACE shutterSystem
