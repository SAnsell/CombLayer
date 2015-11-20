/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weights/WeightControl.cxx
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
#include "BaseMap.h"
#include "CellMap.h"
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
#include "ImportControl.h"
#include "WWGconstruct.h"

#include "LineTrack.h"
#include "ObjectTrackAct.h"
#include "WeightControl.h"


namespace WeightSystem
{
  
WeightControl::WeightControl() :
  sourceFlag(0),tallyFlag(0)
  /*
    Constructorg
   */
{
  setHighEBand();
}

WeightControl::WeightControl(const WeightControl& A) : 
  EBand(A.EBand),WT(A.WT),sourceFlag(A.sourceFlag),
  tallyFlag(A.tallyFlag),sourcePt(A.sourcePt),
  tallyPt(A.tallyPt)
  /*!
    Copy constructor
    \param A :: WeightControl to copy
  */
{}

WeightControl&
WeightControl::operator=(const WeightControl& A)
  /*!
    Assignment operator
    \param A :: WeightControl to copy
    \return *this
  */
{
  if (this!=&A)
    {
      EBand=A.EBand;
      WT=A.WT;
      sourceFlag=A.sourceFlag;
      tallyFlag=A.tallyFlag;
      sourcePt=A.sourcePt;
      tallyPt=A.tallyPt;
    }
  return *this;
}

WeightControl::~WeightControl()
  /*!
    Destructor
   */
{}

void
WeightControl::setHighEBand()
  /*!
    Function to set up the weights system.
  */
{
  EBand={ 1e-4, 0.01,   1,  10,  100, 5e7};
  WT={    0.9,  0.9, 0.8,  0.2, 0.5, 1.0};
  return;
}

void
WeightControl::setMidEBand()
  /*!
    Function to set up the weights system.
  */
{
  EBand={ 1e-9, 1e-5, 1e-1, 1.0, 1e3, 5e7};
  WT=  {  0.9,  0.7, 0.4,  0.5, 0.7, 1.0};
  return;
}

void
WeightControl::setLowEBand()
  /*!
    Function to set up the weights system.
  */
{
  EBand={ 1e-9,  1e-7, 1e-3, 1e-1, 100, 5e7};
  WT={    0.2,  0.7,  0.8,  0.9,  0.9, 1.0};
  return;
}


void
WeightControl::procType(const mainSystem::inputParam& IParam)
  /*!
    Extract hte type from the input to process of energy system 
    \param IParam :: input param
  */
{
  ELog::RegMethod RegA("weightControl","procType");
  const std::string Type=IParam.getValue<std::string>("weightType");
  
  if (Type=="basic")
    setLowEBand();
  else if (Type=="high")
    setHighEBand();
  else if (Type=="mid")
    setMidEBand();
  else if (Type=="flat")
    setMidEBand();
  else if (Type=="energy")
    {
      const size_t itemCnt=IParam.itemCnt("weightType",0);
      std::vector<double> E;
      std::vector<double> W;
      for(size_t i=1;i<itemCnt;i+=2)
	{
	  E.push_back(IParam.getValue<double>("weightType",i));
	  W.push_back(IParam.getValue<double>("weightType",i+1));
	}
      EBand=E;
      WT=W;
    }
  else if (Type=="help")
    {
      procTypeHelp();
    }
  return;
}

void
WeightControl::procTypeHelp() const
  /*!
    Write the Type help
  */
{
  ELog::EM<<"weightType ::: \n"
    "high : Set High energy default band\n"
    "mid : Set mid energy default band\n"
    "low : Set low energy default band\n"
    "energy [E1 W1 E2 W2 ...] : Set energy bands"<<
    ELog::endDiag;
  return;
}

void
WeightControl::procRebaseHelp() const
  /*!
    Write the Rebase help
  */
{
  ELog::EM<<"weightRebase ::: \n"
    "cell cellNumber {index} {eCut} : scale weightObject  \n"
    "                                : cells by WWN[cell/index] to \n"
    "                                : give value in cell.\n"
    "object Name {index} {eCut} : scale object cells by WWN [cell/index] to \n"
    "                                : give value in cell."
	  <<ELog::endDiag;
  return;
}



void
WeightControl::procSource(const mainSystem::inputParam& IParam)
  /*!
    Process the source weight point
    \param IParam :: Input parame
  */
{
  ELog::RegMethod RegA("weightControl","procSource");

  size_t itemCnt(0);
  sourcePt=IParam.getCntVec3D("weightSource",0,itemCnt,"weightSource Vec3D");
  sourceFlag=1;
  return;
}

void
WeightControl::procTallyPoint(const mainSystem::inputParam& IParam)
  /*!
    Process the source weight point
    \param IParam :: Input parame
  */
{
  ELog::RegMethod RegA("weightControl","procTally");

  size_t itemCnt(0);
  tallyPt=IParam.getCntVec3D("weightTally",0,itemCnt,"weightTally Vec3D");
  tallyFlag=1;
  return;
}

void
WeightControl::calcTrack(const Simulation& System,
			 const Geometry::Vec3D& Pt,
			 const std::vector<int>& cellVec,
			 const double eCut,const double sF,
			 const double mW)
  /*!
    Calculate a given track 
    \param System :: Simulation to use
    \param Pt :: point for outgoing track
    \param cellVec :: Cells to track
    \param eCut :: Energy cut [MeV]
    \param sF :: Scale fraction of attenuation path
    \param mW :: mininum weight for splitting
   */
{
  ELog::RegMethod RegA("WeightControl","calcTrack");
  
  CellWeight CTrack;

  // SOURCE Point
  ModelSupport::ObjectTrackAct OTrack(Pt);
  size_t step(cellVec.size()/10);
  
  int reportN=cellVec[step];
  
  for(const int cellN : cellVec)
    {
      if (cellN==reportN)
	{
	  ELog::EM<<"Cell = "<<cellN<<" Range == ["<<cellVec.front()
		  <<":"<<cellVec.back()<<"]"
		  <<ELog::endDiag;
	  step+=cellVec.size()/50;
	  reportN=cellVec[step];
	}
      const MonteCarlo::Qhull* CellPtr=System.findQhull(cellN);
      if (CellPtr && CellPtr->getMat())
	{
	  std::vector<double> attnN;
	  const int cN=CellPtr->getName();  // this should be cellN ??
	  OTrack.addUnit(System,cN,CellPtr->getCofM());
	  // either this :
	  CTrack.addTracks(cN,OTrack.getAttnSum(cN));
	}
    }
  CTrack.setScaleFactor(sF);
  CTrack.setMinWeight(mW);
  CTrack.updateWM(eCut);
  return;
}

  
void
WeightControl::procObject(const Simulation& System,
			  const mainSystem::inputParam& IParam)

  /*!
    Function to set up the weights system.
    This is for the object tracked from a point
    \param System :: Simulation component
    \param IParam :: input stream
  */
{
  ELog::RegMethod RegA("WeightControl","procObject");

    
  const size_t nSet=IParam.setCnt("weightObject");
  double eCut(0.0),sF(1.0),minW(1e-7);
  for(size_t iSet=0;iSet<nSet;iSet++)
    {
      const size_t nItem=IParam.itemCnt("weightObject",iSet);

      if (nItem>1)
	eCut=IParam.getValue<double>("weightObject",iSet,1);
      if (nItem>2)
	sF=IParam.getValue<double>("weightObject",iSet,2);
      if (nItem>3)
	minW=IParam.getValue<double>("weightObject",iSet,3);

      const std::string Key=
	IParam.getValue<std::string>("weightObject",iSet,0);
      objectList.insert(Key);
      const std::vector<int> objCells=getObjectRange(Key);
      
      // SOURCE Point
      if (sourceFlag && tallyFlag) sF/=2.0;

      if (sourceFlag)
	calcTrack(System,sourcePt,objCells,eCut,sF,minW);
      if (tallyFlag)
	calcTrack(System,tallyPt,objCells,eCut,sF,minW);

      if (!tallyFlag && !sourceFlag)
	ELog::EM<<"No source/tally set for weightObject"<<ELog::endCrit;
    }
  return;
}

std::vector<int>
WeightControl::getObjectRange(const std::string& objName) const
  /*!
    Calculate the object cells range based on the name
    Processes down to cellMap items if objName is of the 
    form objecName:cellMapName
    \param objName :: Object name
    \return vector of item
  */
{
  ELog::RegMethod RegA("WeightContorl","getObjectRange");

  const ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  std::string::size_type pos=objName.find(":");
  
  if (pos==std::string::npos || !pos)
    {
      const int BStart=OR.getRenumberCell(objName);
      const int BRange=OR.getRenumberRange(objName);
      if (BStart==0)
	throw ColErr::InContainerError<std::string>
	  (objName,"Object name not found");
      std::vector<int> Out(static_cast<size_t>(1+BRange-BStart));
      std::iota(Out.begin(),Out.end(),BStart);
      return Out;
    }
  
  const std::string itemName=objName.substr(0,pos-1);
  const std::string cellName=objName.substr(pos+1);
  const attachSystem::CellMap* CPtr=
    OR.getObject<attachSystem::CellMap>(itemName);
  if (!CPtr)
    throw ColErr::InContainerError<std::string>
      (objName,"Object name not found");
  return CPtr->getCells(cellName);
}

  
void
WeightControl::scaleObject(const Simulation& System,
			   const std::string& objKey,
			   const double SW,
			   const double eCut)
  /*!
    Scale all individual object by the scale weight.
    \param objKey :: Object key name
    \param SW :: Scale weight
    \param eCut :: min energy to use
  */
{
  ELog::RegMethod RegA("WeightControl","scaleObject");
  
  WeightSystem::weightManager& WM=
    WeightSystem::weightManager::Instance();  

  WeightSystem::WCells* WF=
    dynamic_cast<WeightSystem::WCells*>(WM.getParticle('n'));

  if (!WF)
    throw ColErr::InContainerError<std::string>("n","neutron has no WCell");

  // Note : going to use this set as energy scale values
  std::vector<double> WEng=WF->getEnergy();

  for(double& EW : WEng)
    {
      if (eCut>0.0)
	EW= (EW>eCut) ? 1.0/SW : 1.0;
      else
	EW= (EW<-eCut) ? 1.0/SW : 1.0;
      ELog::EM<<"EW == "<<EW<<ELog::endDiag;
    }
  std::vector<int> cellVec=getObjectRange(objKey);
  for(const int cellN : cellVec)
    {
      const MonteCarlo::Qhull* CellPtr=System.findQhull(cellN);
      if (CellPtr && CellPtr->getMat())
	WF->scaleWeights(cellN,WEng);
    }
  return;
}
  
void
WeightControl::scaleAllObjects(const Simulation& System,
			       const double SW,const double eCut) 
  /*!
    Scale all individual object by the scale weight.
    \param SW :: Scale weight
  */
{
  ELog::RegMethod RegA("WeightControl","scaleAllObjects");


  for(const std::string& objName : objectList)
    scaleObject(System,objName,SW,eCut);
  return;
}

double
WeightControl::findMax(const Simulation& System,
		       const std::string& objKey,
		       const size_t index,
		       const double eCut) const
  /*!
    Find the maximum value in a cell
    \param System :: Simulation for cell materials
    \param objKey :: Object Name
    \param index ;: index value [+1 option / 0 all]
    \param eCut :: energy cut value
    \return value for max
  */
{
  ELog::RegMethod RegA("WeightControl","findMax");
  
  const ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  WeightSystem::weightManager& WM=
    WeightSystem::weightManager::Instance();  

  WeightSystem::WForm* WF=WM.getParticle('n');
  if (!WF)
    throw ColErr::InContainerError<std::string>("n","neutron has no WForm");
  const std::vector<double>& WEng=WF->getEnergy();
  const long int eIndex=
    std::lower_bound(WEng.begin(),WEng.end(),std::abs(eCut))-WEng.begin();
  const int BStart=OR.getRenumberCell(objKey);
  const int BRange=OR.getRenumberRange(objKey);
  double maxVal(0.0);

	  
  if (BStart!=0)
    {
      int cellN(0);
      double M(1.0);
      for(int i=BStart;i<=BRange;i++)
	{
	  const MonteCarlo::Qhull* CellPtr=System.findQhull(i);
	  if (CellPtr && CellPtr->getMat())
	    {
	      const std::vector<double> WVec=WF->getWeights(i);
	      if (!index && eCut>0.0)
		M= *std::max_element(WVec.begin()+eIndex,WVec.end());
	      else if (!index)
		M= *std::max_element(WVec.begin(),WVec.begin()+eIndex);
	      else
		M=WVec[index-1];
	      if (M>maxVal)
		{
		  maxVal=M;
		  cellN=i;
		}
	    }
	}
      ELog::EM<<"Cell = "<<cellN<<" "<<maxVal<<ELog::endDiag;
    }

  return maxVal;
}
 
void
WeightControl::procRebase(const Simulation& System,
			  const mainSystem::inputParam& IParam)
  /*!
    Rebase a weight syste
    \param System :: Simulation component
    \param IParam :: input stream
  */
{
  ELog::RegMethod RegA("WeightControl","procRebase");

  WeightSystem::weightManager& WM=
    WeightSystem::weightManager::Instance();  

  const size_t nItem=IParam.itemCnt("weightRebase",0);
  const std::string type=IParam.getValue<std::string>("weightRebase",0,0);

  if (type=="Cell" || type=="cell")
    {
      const int cellN=IParam.getValue<int>("weightRebase",0,1);
      const size_t index=(nItem>2) ? 
	IParam.getValue<size_t>("weightRebase",0,2) : 0;
      const double eCut=(nItem>3) ? 
	IParam.getValue<double>("weightRebase",0,3) : 0.0;

      WeightSystem::WCells* WF=
	dynamic_cast<WeightSystem::WCells*>(WM.getParticle('n'));
      const std::vector<double>& baseVec=WF->getWeights(cellN);
      if (baseVec.size()<index)
	throw ColErr::IndexError<size_t>(index,baseVec.size(),"Index range");
      
      const double scaleW = (index) ? baseVec[index-1] : baseVec.back();
      scaleAllObjects(System,scaleW,eCut);
    }
  // get maximum for the cells in a range
  else if (type=="Object" || type=="object")
    {
      const std::string objName=
	IParam.getValue<std::string>("weightRebase",0,1);
      const size_t index=(nItem>2) ? 
	IParam.getValue<size_t>("weightRebase",0,2) : 0;
      const double eCut=(nItem>3) ? 
	IParam.getValue<double>("weightRebase",0,3) : 0.0;

      const double scaleW=findMax(System,objName,index,eCut);
      scaleObject(System,objName,scaleW,eCut);
    }

  return;
}

void
WeightControl::processWeights(Simulation& System,
			      const mainSystem::inputParam& IParam)
  /*!
    Set individual weights based on temperature/cell
    weightType :: 
    \param System :: Simulation
    \param IParam :: input stream
  */
{
  ELog::RegMethod RegA("WeightControl","processWeights");

  System.populateCells();
  System.createObjSurfMap();
  
  if (IParam.flag("weight"))
    {
      setWeights(System);
    }
  // requirements for vertex:
  if (IParam.flag("weightObject") ||
      IParam.flag("tallyWeight") )
    System.calcAllVertex();
  
  if (IParam.flag("weightType"))
    procType(IParam);
  if (IParam.flag("weightSource"))
    procSource(IParam);
  if (IParam.flag("weightTally"))
    procTallyPoint(IParam);
  if (IParam.flag("weightObject"))
    procObject(System,IParam);
  if (IParam.flag("wWWG"))
    {
      WWGconstruct WConstruct;
      WConstruct.createWWG(System,IParam);
      removePhysImp(System,"n");
    }
  if (IParam.flag("weightTemp"))
    scaleTempWeights(System,10.0);
  if (IParam.flag("tallyWeight"))
    tallySystem::addPointPD(System);
  if (IParam.flag("weightRebase"))
    procRebase(System,IParam);

  
  
  return;
}

void
WeightControl::setWeights(Simulation& System)
   /*!
    Function to set up the weights system.
    It replaces the old file read system.
    \param System :: Simulation component
  */
{
  ELog::RegMethod RegA("WeightControl","setWeights(Simulation)");

  WeightSystem::weightManager& WM=
    WeightSystem::weightManager::Instance();  

  WM.addParticle<WeightSystem::WCells>('n');
  WeightSystem::WCells* WF=
    dynamic_cast<WeightSystem::WCells*>(WM.getParticle('n'));
  if (!WF)
    throw ColErr::InContainerError<std::string>("n","WCell - WM");

  WF->setEnergy(EBand);
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

  // remove neutron imp:
  removePhysImp(System,"n");
  return;
}

void
WeightControl::help() const
  /*!
    Write out the help
  */
{
  ELog::RegMethod RegA("WeightControl","help");
  ELog::EM<<"Weight help :: "<<ELog::endDiag;


  ELog::EM<<"-- WeightType -- ::"<<ELog::endDiag;
  procTypeHelp();
  ELog::EM<<"-- weightSource --::"<<ELog::endDiag;
  ELog::EM<<"-- weightTally --::"<<ELog::endDiag;
  ELog::EM<<"-- weightObject --::"<<ELog::endDiag;
  ELog::EM<<"-- weightRebase --::"<<ELog::endDiag;
  procRebaseHelp();
  ELog::EM<<"-- wWWG --::"<<ELog::endDiag;
  ELog::EM<<"-- weightTemp --::"<<ELog::endDiag;
  ELog::EM<<"-- tallyWeight --::"<<ELog::endDiag;
  return;
}

		       
}  // NAMESPACE weightSystem

