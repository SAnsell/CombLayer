/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
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
#include "CellWeight.h"
#include "Simulation.h"
#include "objectRegister.h"
#include "inputParam.h"
#include "TallyCreate.h"
#include "PointWeights.h"
#include "TempWeights.h"
#include "WWGconstruct.h"

#include "LineTrack.h"
#include "ObjectTrackAct.h"
#include "BasicWWE.h"


namespace WeightSystem
{

basicWeightState::basicWeightState() 
{
  setHighEBand();
}

void

  
void
basicWeightState::simulationWeights(Simulation& System,
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

  // WEIGHTS:
  if (IParam.flag("wWWG") )
    {
      WWGconstruct WConstruct;
      WConstruct.createWWG(System,IParam);
    }
  else
    {
      setWeightType(System,IParam);
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
	scaleTempWeights(System,10.0);
    }
  if (IParam.flag("tallyWeight"))
    tallySystem::addPointPD(System);

  return;
}

void
basicWeightState::setWeightType(Simulation& System,
				const mainSystem::inputParam& IParam)
  /*!
    Set the basic weight either for low energy to high energy
    \param Type :: simulation type (basic/high/mid)
    \param IParam Input point
  */
{
  ELog::RegMethod RegA("BasicWeightState","setWeightsType");


  const std::string Type=IParam.flag("weightType") ?
    IParam.getValue<std::string>("weightType") : "basic";

  const std::string CellType=IParam.flag("weightCell") ?
    IParam.getValue<std::string>("weightCell") : "flat";
  
  
  if (Type=="basic" || Type=="low")
    setLowEBand(System);
  else if (Type=="high")
    setHighEBand(System);
  else if (Type=="mid")
    setMidEBand(System);
  else if (Type=="help")
    {
      ELog::EM<<"Basic weight energy types == \n"
	"object [name Vec3D] -- HighEnergy Object name and source point \n"
	"high -- High energy\n"
	"mid -- Mid/old point energy\n"
	"basic -- Cold spectrum energy"<<ELog::endBasic;
      throw ColErr::ExitAbort("End Help");
    }
    
  if (CellType=="object")
    {
      const size_t itemCnt=IParam.itemCnt("weightCell",0);
      if (itemCnt<3)
	throw ColErr::IndexError<size_t>(itemCnt,2,"weightCell incomplete");
      const std::string OName=
	IParam.getValue<std::string>("weightCell",0,1);
      size_t itemIndex(2);
      const Geometry::Vec3D sPoint=
	IParam.getCntVec3D("weightCell",0,itemIndex,"Source Point");
      setWeightsObject(System,OName,sPoint);
    }
  else if (CellType=="flat")
    {
      setWeights(System);
    }
  else
    throw ColErr::InContainerError<std::string>(Type,"Unknown weight type");

  return;
}

void
basicWeightState::setWeights(Simulation& System)
  /*!
    Function to set up the weights system.
    It replaces the old file read system.
    \param System :: Simulation component
  */
{
  ELog::RegMethod RegA("BasicWeightState","setWeights(Simulation)");

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
  std::set<std::string> EmptySet;
  setWeights(System,Eval,WT,EmptySet);

  return;
}

void
basicWeightState::setLowEBand()
  /*!
    Function to set up the weights system.
  */
{
  ELog::RegMethod RegA("basicWeightState","setLowEBand");

  energyBand={1.4678e-10,3.1622e-10,6.8130e-10;
	      1.4678e-9,3.1622e-9,5e7}
  WT={0.02,0.07,0.1,0.1,0.2,0.7};
  return;
}

void
basicWeightState::setMidEBand();
  /*!
    Function to set up the weights system.
    It replaces the old file read system.
    \param System :: Simulation component
  */
{
  ELog::RegMethod RegA("BasicWWE","setMidEBand");
  
  energyBand={1e-9,1e-5,1e03,1e-1,1,1000};
  WT={0.9,0.8,0.5,0,5,0,7};
  return;
}

void
basicWeightState::setHighEBand();
  /*!
    Function to set up the weights system.
    It replaces the old file read system.
    \param System :: Simulation component
  */
{
  ELog::RegMethod RegA("BasicWWE","setHihgEBand");
  
  energyBand={1e-4,0.01,1.0,10,100,5e7};
  WT={0.02,0.07,0.2,0.7,0.7};
  
  return;
}

void
basicWeightState::setWeightsObject(Simulation& System,
				   const std::string& Key,
				   const Geometry::Vec3D& sourcePoint)
  /*!
    Function to set up the weights system.
    This is for the bunker shielding at 
    \param System :: Simulation component
    \param sourcePoint :: point to calculate tracks from
  */
{
  ELog::RegMethod RegA("BasicWWE","setWeightsObject");
  
  setWeights(System,Eval,WT);

  // In case not calculated
  System.calcAllVertex();

  const ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  CellWeight CTrack;
  
  const int BStart=OR.getRenumberCell(Key);
  const int BRange=OR.getRenumberRange(Key);
  if (BStart==0)
    throw ColErr::InContainerError<std::string>(Key,"Object name not found");

  ModelSupport::ObjectTrackAct OTrack(sourcePoint);
  for(int i=BStart;i<=BRange;i++)
    {
      const MonteCarlo::Qhull* CellPtr=System.findQhull(i);
      if (CellPtr)
	{
	  std::vector<int> cellN;
	  std::vector<double> attnN;
	  const int cN=CellPtr->getName();  // this should be i !!
	  OTrack.addUnit(System,cN,CellPtr->getCofM());
	  // either this :
	  CTrack.addTracks(cN,OTrack.getAttnSum(cN));
	  // or
	  //	  OTrack.createAttenPath(cellN,attnN);
	  //	  for(size_t i=0;i<cellN.size();i++)
	  //	    CTrack.addTracks(cellN[i],attnN[i]);
	  CTrack.updateWM();
	  ELog::EM<<CTrack<<ELog::endDiag;
		  
	}
      
    }
  return;
}


  
}  // NAMESPACE weightSystem
