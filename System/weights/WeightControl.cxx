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
#include "WeightControl.h"


namespace WeightSystem
{
  
WeightControl::WeightControl()
  /*
    Constructor
   */
{
  setHighEBand();
}

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
weightControl::procType(const mainSystem::inputParam& IParam)
  /*!
    Process the type of weight system
    \param IParam :: I
  */
{
  ELog::RegMethod RegA("weightControl","procType");
  const std::string Type=IParam.getValue<std::string>("weightType");
  
  if (Type=="basic")
    setWeightsBasic(System);
  else if (Type=="high")
    setWeightsHighE(System);
  else if (Type=="mid")
    setWeightsMidE(System);
  else if (Type=="energy")
    {
      const size_t itemCnt=IParam.itemCnt("weightType",0);
      std::vector<double> E;
      std::vector<double> W;
      for(i=1;i<itemCnt;i+=2)
	{
	  E.push_back(IParam.getValue<double>("weightType",i));
	  W.push_back(IParam.getValue<double>("weightType",i));
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
	"energy [E1 W1 E2 W2 ...] : Set energy bands" 
	ELog::endDiag;
    }
  return;
}


void
weightControl::procSource(const mainSystem::inputParam& IParam)
  /*!
    Process the source weight point
    \param IParam :: Input parame
  */
{
  ELog::RegMethod RegA("weightControl","procSource");

  size_t itemCnt(1);
  sourcePt=IParam.getCntVec3D("weightSource",0,itemCnt,"weightSource Vec3D");
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
  
  for(size_t iSet=0;iSet<nSet;iSet++)
    {
      const std::string key=
	IParam.getValue<std::string>(weightObject,iSet,0);

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

  if (IParam.flag("weightType"))
    procType(IParam);
  if (IParam.flag("weightSource"))
    procSource(IParam);
  
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






		       
}  // NAMESPACE weightSystem

