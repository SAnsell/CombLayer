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
#include "WWGControl.h"

namespace WeightSystem
{

WWGControl::WWGControl() :
  WeightControl(),
  nMarkov(0),sourceFlux(0),adjointFlux(0)
  /*!
    Constructor
  */
{}

  
WWGControl::WWGControl(const WWGControl& A) :
  WeightControl(A),nMarkov(A.nMarkov),
  sourceFlux((A.sourceFlux) ? new WWGWeight(*A.sourceFlux) : 0),
  adjointFlux((A.adjointFlux) ? new WWGWeight(*A.adjointFlux) : 0)
  /*!
    Copy Constructor
    \param A :: WWGControl item to copy
  */
{}

WWGControl&
WWGControl::operator=(const WWGControl& A)
  /*!
    Assignment operator
    \param A :: WWGControl to copy
    \return this
  */
{
  if (this!=&A)
    {
      WeightControl::operator=(A);
      nMarkov=A.nMarkov;
      *sourceFlux= *A.sourceFlux;
      *adjointFlux= *A.adjointFlux;
    }
  return *this;
}

WWGControl::~WWGControl()
  /*!
    Destructor
   */
{
  delete sourceFlux;
  delete adjointFlux;
}
  
  
void
WWGControl::wwgMesh(const mainSystem::inputParam& IParam)
  /*!
    Process wwg Mesh - constructs the 3D mesh boundary
    \todo Modify to remove ability to create complex boundaries
    \param IParam :: Input parameters
  */
{
  ELog::RegMethod RegA("WWGControl","wwgMesh");

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

      if (NXM!=3)
	throw ColErr::IndexError<size_t>
	  (NXM,3,"Insufficient items for "+itemName+
	   ": X_0 : N_0 : X1 ");

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
WWGControl::wwgInitWeight()
  /*!
    Initialize the WWGWeight arrays
   */
{
  ELog::RegMethod RegA("WWGControl","wwgInitWeight");
  
  WeightSystem::weightManager& WM=
    WeightSystem::weightManager::Instance();
  WWG& wwg=WM.getWWG();
  wwg.setEnergyBin(EBand,WT);

  // 99% of the time these are either not set of the wrong size
  delete sourceFlux;
  delete adjointFlux;

  // built and zeroed
  sourceFlux=new WWGWeight(EBand.size(),wwg.getGrid());
  adjointFlux=new WWGWeight(EBand.size(),wwg.getGrid());

  return;
}
  
void
WWGControl::wwgCreate(const Simulation& System,
                      const mainSystem::inputParam& IParam)
  /*!
    Calculate a WWG weights based 
    \param System :: Simulation
    \param IParam :: Input deck
   */
{
  ELog::RegMethod RegA("WWGControl","wwgCreate");

  WeightSystem::weightManager& WM=
    WeightSystem::weightManager::Instance();
  WWG& wwg=WM.getWWG();

  int wwgFlag(0);
  
  const std::vector<Geometry::Vec3D>& GridMidPt=wwg.getMidPoints();
  const size_t NSetCnt=IParam.setCnt("wwgCalc");
  
  for(size_t iSet=0;iSet<NSetCnt;iSet++)
    {
      std::string ptType;
      size_t ptIndex;
      bool adjointFlag;
      
      const std::string SourceStr=
        IParam.getValueError<std::string>
        ("wwgCalc",iSet,0,"wwgCalc without source term");

      processPtString(SourceStr,ptType,ptIndex,adjointFlag);
      
      // get values [ecut / density / minWeight/ r2Length / r2Power ]
      procParam(IParam,"wwgCalc",iSet,1);      
      WWGWeight& wSet=(adjointFlag) ? *adjointFlux : *sourceFlux;

      if (ptType=="Plane")   
	{
          if (ptIndex>=planePt.size())
            throw ColErr::IndexError<size_t>(ptIndex,planePt.size(),
                                             "planePt.size() < ptIndex");
          
	  wSet.wTrack(System,planePt[ptIndex],GridMidPt,
		      density,r2Length,r2Power);
	}
      else if (ptType=="Source")
        {
	  ELog::EM<<"Here "<<ptIndex<<" "<<sourcePt.size()<<ELog::endDiag;
	  if (ptIndex>=sourcePt.size())
            throw ColErr::IndexError<size_t>(ptIndex,sourcePt.size(),
                                             "sourcePt.size() < ptIndex");

          wSet.wTrack(System,sourcePt[ptIndex],GridMidPt,
		      density,r2Length,r2Power);
        }
      else 
	throw ColErr::InContainerError<std::string>
	  (ptType,"SourceType not known");

      wSet.controlMinValue(minWeight);
      wwgFlag|= (adjointFlag) ? 2 : 1;
    }
  
  if ((wwgFlag & 1)==0)
    ELog::EM<<"Failed to calculate : Source for WWG"<<ELog::endCrit;
  if ((wwgFlag & 2)==0)
    ELog::EM<<"Failed to calculate : Adjoint for WWG"<<ELog::endCrit;

  return;
}
  

void
WWGControl::wwgMarkov(const Simulation& System,
			 const mainSystem::inputParam& IParam)
  /*!
    Process a Markov chain iteration for the system
    \param System :: Simulation
    \param IParam :: Input deck
   */
{
  ELog::RegMethod RegA("WWGControl","wwgMarkov");

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
WWGControl::wwgNormalize(const mainSystem::inputParam& IParam)
  /*!
    Normalize the main weight system
    \param IParam :: Input parameter
  */
{
  ELog::RegMethod RegA("WWGControl","wwgNormalize");

  WeightSystem::weightManager& WM=
    WeightSystem::weightManager::Instance();
  WWG& wwg=WM.getWWG();

  wwg.updateWM(*sourceFlux,1.0);
  if (IParam.flag("wwgNorm"))
    {
      const double minWeight=
        IParam.getDefValue<double>(-100.0,"wwgNorm",0,0);
      if (minWeight<-90)
	{
	  ELog::EM<<"Norm == "<<ELog::endDiag;
	  wwg.normalize();
	}
      else
	{
	  const double powerWeight=
	    IParam.getDefValue<double>(1.0,"wwgNorm",0,1);
	  ELog::EM<<"Scale Range == "<<std::pow(10.0,-minWeight)<<ELog::endDiag;
	  wwg.scaleRange(std::pow(10.0,-minWeight),1.0);
	  wwg.powerRange(powerWeight);

	      
	}
    }
  else
    ELog::EM<<"Warning : No WWG normalization step"<<ELog::endWarn;
  return;
}
  

void
WWGControl::wwgVTK(const mainSystem::inputParam& IParam)
  /*!
    Write out an vkt file
    \param IParam :: Data for point
  */
{
  ELog::RegMethod RegA("WWGControl","wwgvTK");
  
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

void
WWGControl::procMarkov(const mainSystem::inputParam&,
                       const std::string&,
                       const size_t)
  /*!
    Markov update -- 
   */
{
  ELog::RegMethod RegA("WWGControl","procMarkov");
  ELog::EM<<"Calling placeholder"<<ELog::endDiag;
  return;
}

void
WWGControl::wwgCombine(const Simulation& System,
		       const mainSystem::inputParam& IParam)
  /*!
    Co-joint the adjoint/source terms
    \param System :: Model simulation
    \param IParam :: input data
   */
{
  ELog::RegMethod RegA("WWGControl","wwgCombine");

  if (IParam.flag("wwgCADIS"))
    {
      WeightSystem::weightManager& WM=
	WeightSystem::weightManager::Instance();
      WWG& wwg=WM.getWWG();
	
      size_t itemCnt(0);

      const Geometry::Vec3D SPoint=
	IParam.getCntVec3D("wwgCADIS",0,itemCnt,"CADIS Source Point");
      const std::vector<Geometry::Vec3D>& GridMidPt=wwg.getMidPoints();

      //      sourceFlux->CADISnorm(System,*adjointFlux,GridMidPt,SPoint);
      sourceFlux->CADISnorm(System,*adjointFlux,GridMidPt,SPoint);      
    }
  else
    ELog::EM<<"Warning : No WWG CADIS step"<<ELog::endWarn;


  
  
  return;
}
  

void
WWGControl::processWeights(Simulation& System,
			   const mainSystem::inputParam& IParam)
  /*!
    External control sequence for WWG:
    Controls full build of WWG mesh/ calculation / and final setting
    \param System :: Simulation
    \param IParam :: input stream
  */
{
  ELog::RegMethod RegA("WWGControl","processWeights");

  WeightSystem::weightManager& WM=
    WeightSystem::weightManager::Instance();  

  if (IParam.flag("wWWG"))
    {
      WeightControl::processWeights(System,IParam);
      
      procParam(IParam,"wWWG",0,0);
      wwgMesh(IParam);               // create mesh [wwgXMesh etc]
      wwgInitWeight();               // Zero arrays etc
      wwgCreate(System,IParam);      // LOG space
      wwgMarkov(System,IParam);
      
      wwgCombine(System,IParam);                 
      wwgNormalize(IParam); 
      wwgVTK(IParam);
      
      WM.getParticle('n')->setActiveWWP(0);
      setWWGImp(System);
    }

  return;
}


  
}  // NAMESPACE weightSystem

