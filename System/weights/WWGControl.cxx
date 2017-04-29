/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weights/WWGControl.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include <boost/multi_array.hpp>

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
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cone.h"
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
#include "weightManager.h"
#include "WForm.h"
#include "WItem.h"
#include "WCells.h"
#include "CellWeight.h"
#include "Simulation.h"
#include "objectRegister.h"
#include "inputParam.h"
#include "PositionSupport.h"
#include "TallyCreate.h"
#include "TempWeights.h"
#include "ImportControl.h"

#include "LineTrack.h"
#include "ObjectTrackAct.h"
#include "ObjectTrackPoint.h"
#include "ObjectTrackPlane.h"
#include "Mesh3D.h"
#include "WWG.h"
#include "WWGItem.h"
#include "WWGWeight.h"
#include "MarkovProcess.h"
#include "WeightControl.h"

namespace WeightSystem
{

void
WeightControl::procWWGWeights(Simulation& System,
			      const mainSystem::inputParam& IParam)
  /*!
    Set individual weights based on WWG
    \param System :: Simulation
    \param IParam :: input stream
  */
{
  ELog::RegMethod RegA("WeightControl","procWWGWeights");

  WeightSystem::weightManager& WM=
    WeightSystem::weightManager::Instance();  

  procParam(IParam,"wWWG",0,0);
  wwgMesh(IParam);               // create mesh [wwgXMesh etc]
  wwgEnergy(IParam);             // set default energy grid

  wwgCreate(System,IParam);
  wwgMarkov(System,IParam);
  wwgNormalize(IParam); 
  wwgVTK(IParam);
  WM.getParticle('n')->setActiveWWP(0);
  setWWGImp(System);
  return;
}

void
WeightControl::wwgMarkov(const Simulation& System,
			 const mainSystem::inputParam& IParam)
  /*!
    Process a Markov chain iteration for the system
    \param System :: Simulation
    \param IParam :: Input deck
   */
{
  ELog::RegMethod RegA("WeightControl","wwgMarkov");

  // Find if we need to do Markov:
  const size_t NSetCnt=IParam.setCnt("wwgMarkov");
  if (!NSetCnt) return;
  
  WeightSystem::weightManager& WM=
    WeightSystem::weightManager::Instance();
  WWG& wwg=WM.getWWG();
  const std::vector<double> EBin=wwg.getEBin();
  const std::vector<Geometry::Vec3D> GridMidPt=wwg.getMidPoints();
  
  for(size_t index=0;index<NSetCnt;index++)
    {
      procMarkov(IParam,"wwgMarkov",index);
      if (nMarkov)
	{
	  MarkovProcess MCalc;
	  MCalc.initializeData(wwg);
	}
    }

  return;
}

void
WeightControl::wwgCreate(const Simulation& System,
                         const mainSystem::inputParam& IParam)
  /*!
    Calculate a WWG weights based 
    \param System :: Simulation
    \param IParam :: Input deck
   */
{
  ELog::RegMethod RegA("WeightControl","wwgCreate");

  WeightSystem::weightManager& WM=
    WeightSystem::weightManager::Instance();
  WWG& wwg=WM.getWWG();
  const std::vector<double> EBin=wwg.getEBin();
  const std::vector<Geometry::Vec3D> GridMidPt=wwg.getMidPoints();
  const size_t NSetCnt=IParam.setCnt("wwgCalc");

  for(size_t index=0;index<NSetCnt;index++)
    {
      // local mesh - zeroed
      WWGWeight wSet(EBin.size(),wwg.getGrid());   
      procParam(IParam,"wwgCalc",index,0);

      if (activePtType=="Plane")   //
	{
	  ELog::EM<<"Active type == Plane"<<ELog::endDiag;
          if (activePtIndex>=planePt.size())
            throw ColErr::IndexError<size_t>(activePtIndex,planePt.size(),
                                             "planePt.size() < activePtIndex");
	  wSet.wTrack(System,planePt[activePtIndex],EBin,GridMidPt,
		      density,r2Length,r2Power);
	}
      else if (activePtType=="Source")
        {
	  if (activePtIndex>=sourcePt.size())
            throw ColErr::IndexError<size_t>(activePtIndex,sourcePt.size(),
                                             "sourcePt.size() < activePtIndex");
          ELog::EM<<"Calling Source Point"<<ELog::endDiag;
          wSet.wTrack(System,sourcePt[activePtIndex],EBin,GridMidPt,
		      density,r2Length,r2Power);
        }
      else 
	throw ColErr::InContainerError<std::string>
	  (activePtType,"SourceType no known");

      // min weight == exp(w) (negative) 
      if (minWeight>1.0)
	minWeight*= -log(10.0);
      else if (minWeight<0.0)
	minWeight*= log(10);
      else if (minWeight>1e-30)
	minWeight= exp(minWeight);
      else
	minWeight= exp(1e-30);

      if (!activeAdjointFlag)
	{
	  wSet.makeSource(minWeight);
	}
      else
	{
	  ELog::EM<<"Call adjoint:"<<minWeight<<ELog::endDiag;
	  wSet.makeAdjoint(minWeight);
	}

      wwg.updateWM(wSet,scaleFactor);
    }
  return;
}
  
void
WeightControl::wwgNormalize(const mainSystem::inputParam& IParam)
  /*!
    Normalize the main weight system
    \param IParam :: Input parameter
  */
{
  ELog::RegMethod RegA("WeightControl","wwgNormalize");

  WeightSystem::weightManager& WM=
    WeightSystem::weightManager::Instance();
  WWG& wwg=WM.getWWG();

  if (IParam.flag("wwgNorm"))
    {
      const double minWeight=
        IParam.getDefValue<double>(-100.0,"wwgNorm",0,0);
      if (minWeight<-90)
	wwg.normalize();
      else
	{
	  const double powerWeight=
	    IParam.getDefValue<double>(1.0,"wwgNorm",0,1);
	  ELog::EM<<"Scale Range == "<<std::pow(10.0,-minWeight)<<ELog::endDiag;
	  wwg.scaleRange(std::pow(10.0,-minWeight),1.0);
	  wwg.powerRange(powerWeight);
	}
    }
  return;
}
  
void
WeightControl::wwgEnergy(const mainSystem::inputParam& IParam)
  /*!
    Modify the energy grid if explicitly given as an wwgE card.
    If not an a weightType has been set then use that -- else
    just one energy grid.
  */
{
  ELog::RegMethod RegA("WeightControl","wwgEnergy");

  WeightSystem::weightManager& WM=
    WeightSystem::weightManager::Instance();
  WWG& wwg=WM.getWWG();

  if (IParam.flag("wwgE"))
    {
      std::vector<double> EWWG;
      std::vector<double> DefVal;
      const size_t NEnergy=IParam.itemCnt("wwgE",0);
      ELog::EM<<"NEnergy = "<<NEnergy<<ELog::endDiag;
      for(size_t i=0;i<NEnergy;i++)
	{
	  EWWG.push_back(IParam.getValue<double>("wwgE",i));
	  DefVal.push_back(1.0);
	}
      if (!NEnergy)
	{
	  EWWG.push_back(1.3e5);
	  DefVal.push_back(1.0);
	}
      wwg.setEnergyBin(EWWG,DefVal);
    }
  else
    wwg.setEnergyBin(EBand,WT);

  return;
}
  
void
WeightControl::wwgMesh(const mainSystem::inputParam& IParam)
  /*!
    Process wwg Mesh
    \param IParam :: Input parameters
  */
{
  ELog::RegMethod RegA("WeightControl","wwgMesh");

  WeightSystem::weightManager& WM=
    WeightSystem::weightManager::Instance();
  WWG& wwg=WM.getWWG();

  const std::string XYZ[3]={"X","Y","Z"};
  std::vector<std::vector<double>> boundaryVal(3);
  std::vector<std::vector<size_t>> bCnt(3);
  
  for(size_t index=0;index<3;index++)
    {
      const std::string itemName("wwg"+XYZ[index]+"Mesh");
      const size_t NXM=IParam.itemCnt(itemName,0);

      if (NXM<3 || !(NXM % 2) )
	throw ColErr::IndexError<size_t>
	  (NXM,3,"Insufficient items for "+itemName+
	   ": X_0 : N_0 : X1 : N1 ...");

      for(size_t i=0;i<NXM;i++)
	{
	  if (i % 2)   // Odd : Integer
	    bCnt[index].push_back
	      (IParam.getValue<size_t>(itemName,i));
	  else
            boundaryVal[index].push_back
	      (IParam.getValue<double>(itemName,i));
	}
    }
  const Geometry::Vec3D RefPt=
    IParam.getDefValue(Geometry::Vec3D(0.1,0.1,0.1),"wwgRPtMesh",0);

  wwg.getGrid().setMesh(boundaryVal[0],bCnt[0],
			boundaryVal[1],bCnt[1],
			boundaryVal[2],bCnt[2]);
  wwg.calcGridMidPoints();
  wwg.setRefPoint(RefPt);
  
  return;
}

void
WeightControl::wwgVTK(const mainSystem::inputParam& IParam)
  /*!
    Write out an vkt file
    \param IParam :: Data for point
  */
{
  ELog::RegMethod RegA("WeightControl","wwgvTK");
  
  WeightSystem::weightManager& WM=
    WeightSystem::weightManager::Instance();

  if (IParam.flag("wwgVTK"))
    {
      WWG& wwg=WM.getWWG();
      const std::string FName=
	IParam.getValue<std::string>("wwgVTK",0);

      wwg.writeVTK(FName);
    }
  return;
}
		       
}  // NAMESPACE weightSystem

