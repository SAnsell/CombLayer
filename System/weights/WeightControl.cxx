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
    Process the type of energy system 
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
      ELog::EM<<"weightType ::: \n"
	"high : Set High energy default band\n"
	"mid : Set mid energy default band\n"
	"low : Set low energy default band\n"
	"energy [E1 W1 E2 W2 ...] : Set energy bands"<<
	ELog::endDiag;
    }
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
			 const int BStart,const int BEnd,
			 const double eCut,const double sF,
			 const double mW)
  /*!
    Calculate a given track 
    \param System :: Simulation to use
    \param Pt :: point for outgoing track
    \param BStart :: Cell to start
    \param BEnd :: Final cell to track
    \param eCut :: Energy cut [MeV]
    \param sF :: Scale fraction of attenuation path
    \param mW :: mininum weight for splitting
   */
{
  ELog::RegMethod RegA("WeightControl","calcTrack");
  
  CellWeight CTrack;

  // SOURCE Point
  ModelSupport::ObjectTrackAct OTrack(Pt);
  for(int i=BStart;i<=BEnd;i++)
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

  const ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
    
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

      const int BStart=OR.getRenumberCell(Key);
      const int BRange=OR.getRenumberRange(Key);
      if (BStart==0)
	throw ColErr::InContainerError<std::string>
	  (Key,"Object name not found");
      
      // SOURCE Point
      if (sourceFlag&& tallyFlag) sF/=2.0;
      
      if (sourceFlag)
	calcTrack(System,sourcePt,BStart,BRange,eCut,sF,minW);
      if (tallyFlag)
	calcTrack(System,tallyPt,BStart,BRange,eCut,sF,minW);
      
      if (tallyFlag)
	{
	  ELog::EM<<"Do stuff here "<<ELog::endDiag;
	}
      if (!tallyFlag && !sourceFlag)
	ELog::EM<<"No source/tally set for weightObject"<<ELog::endCrit;
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
  ELog::RegMethod RegA("WeightControl","simulationWeights");

  System.populateCells();
  System.createObjSurfMap();
  // requirements for vertex:
  if (IParam.flag("weightObject") ||
      IParam.flag("tallyWeight") )
    System.calcAllVertex();

  if (IParam.flag("weight"))
    setWeights(System);
  
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


		       
}  // NAMESPACE weightSystem

