/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weights/WWGControl.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "weightManager.h"
#include "WForm.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimMCNP.h"
#include "inputParam.h"
#include "ImportControl.h"
#include "meshConstruct.h"

#include "BasicMesh3D.h"
#include "WWGWeight.h"
#include "MarkovProcess.h"
#include "WeightControl.h"
#include "WWG.h"
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
      delete sourceFlux;
      delete adjointFlux;
      sourceFlux=(A.sourceFlux) ? new WWGWeight(*A.sourceFlux) : 0;
      adjointFlux=(A.adjointFlux) ? new WWGWeight(*A.adjointFlux) : 0;
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
WWGControl::wwgSetParticles(const std::set<std::string>& actPart)
  /*!
    Process wwg Mesh - constructs the 3D mesh boundary
    \param actPart :: active paritcles
  */
{
  ELog::RegMethod RegA("WWGControl","wwgSetParticles");

  WeightSystem::weightManager& WM=
    WeightSystem::weightManager::Instance();
  WWG& wwg=WM.getWWG();

  wwg.setParticles(actPart);
  return;
}

  
void
WWGControl::wwgMesh(const Simulation& System,
		    const mainSystem::inputParam& IParam)
  /*!
    Process wwg Mesh - constructs the 3D mesh boundary.
    This uses
      -wwgMesh : object/free Vec3D(low) Vec3D(high) NPTS
    \param IParam :: Input parameters
  */
{
  ELog::RegMethod RegA("WWGControl","wwgMesh");

  WeightSystem::weightManager& WM=
    WeightSystem::weightManager::Instance();

  WWG& wwg=WM.getWWG();

  Geometry::Vec3D APt,BPt;
  std::array<size_t,3> Nxyz;

  const std::string PType=
    IParam.getValueError<std::string>("tally",0,0,"object/free/values"); 
  
  if (PType=="object")
    {
      mainSystem::meshConstruct::getObjectMesh
	(System,IParam,"wwgMesh",0,1,APt,BPt,Nxyz);
    }
  else if (PType=="free")
    {
      mainSystem::meshConstruct::getFreeMesh
	(IParam,"wwgMesh",0,1,APt,BPt,Nxyz);
    }
  else // assume just free
    {
      mainSystem::meshConstruct::getFreeMesh
	(IParam,"wwgMesh",0,0,APt,BPt,Nxyz);
    }

  const Geometry::Vec3D RefPt=
    IParam.getDefValue(Geometry::Vec3D(0.1,0.1,0.1),"wwgRPtMesh",0);


  wwg.getGrid().setMesh(APt,BPt,Nxyz);
  ELog::EM<<"REF mesh"<<RefPt<<ELog::endDiag;
  wwg.calcGridMidPoints();
  ELog::EM<<"REF mesh"<<RefPt<<ELog::endDiag;
  wwg.setRefPoint(RefPt);
  ELog::EM<<"REF mesh"<<RefPt<<ELog::endDiag;
  
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
    Calculate a WWG weights based. This calls the tracking
    system to calculate the attenuation from the source/adjoint
    to the cell in readiness for the expected Markov process [if used]
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

      WeightControl::processPtString(SourceStr,ptType,ptIndex,adjointFlag);
      
      // get values [ecut / density / r2Length / r2Power ]
      procParam(IParam,"wwgCalc",iSet,1);      
      WWGWeight& wSet=(adjointFlag) ? *adjointFlux : *sourceFlux;

      ELog::EM<<"ADJOINT == "<<adjointFlag<<ELog::endDiag;
      if (ptType=="help" || ptType=="Help")
	{
	  ELog::EM<<"wwgCalc ==> \n"
	    "       SourceType [S/T Plane/Source/Cone Index]\n"
	    "       Energy cut [MeV] \n"
	    "       ScaleFactor [default: 1.0] \n"
	    "       densityFactor [default: 1.0] \n"
	    "       r2Length factor [default: 1.0] \n"
	    "       r2Power [default: 2.0] \n"<<ELog::endCrit;
	}
      else if (ptType=="Plane")   
	{
	  wSet.wTrack(System,planePt[ptIndex],EBand,GridMidPt,
		      density,r2Length,r2Power);
	}
      else if (ptType=="Source")
        {
          wSet.wTrack(System,sourcePt[ptIndex],EBand,GridMidPt,
		      density,r2Length,r2Power);
        }
      else 
	throw ColErr::InContainerError<std::string>
	  (ptType,"SourceType not known");

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
      const size_t nMult=IParam.getValueError<size_t>
	("wwgMarkov",index,0,"Mult count not set");
      if (nMarkov)
	{
	  MarkovProcess MCalc;
	  MCalc.initializeData(wwg);
	  MCalc.computeMatrix(System,wwg,density,r2Length,r2Power);
	  MCalc.multiplyOut(nMult);
	  MCalc.rePopulateWWG();
	  ELog::EM<<"MARKOV FINISHED"<<ELog::endDiag;
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
  const size_t NE=wwg.getEBin().size();
  
  if (IParam.flag("wwgNorm"))
    {
      const std::string info=
        IParam.getDefValue<std::string>("","wwgNorm",0,0);
      if (info=="help" || info=="Help")
	{
	  ELog::EM<<"wwgNorm ==> \n"
	    "       log(weightRange) [Manditory] (typical 20) \n"
	    "       lowRange (+ve takes data range) [default 1.0]\n"
	    "       highRange (+ve takes data range) [default 1.0]\n"
	    <<ELog::endCrit;
	  return;
	}

      const double weightRange=
        IParam.getValueError<double>("wwgNorm",0,0,"Weight range not given");
      const double lowRange=
        IParam.getDefValue<double>(1.0,"wwgNorm",0,1);    // +ve means default
      const double highRange=
        IParam.getDefValue<double>(1.0,"wwgNorm",0,2);
      //      const double powerRange=
      //        IParam.getDefValue<double>(1.0,"wwgNorm",0,3);
      

      for(size_t i=0;i<NE;i++)
	wwg.scaleRange(i,lowRange,highRange,weightRange);
      //      wwg.powerRange(powerWeight);
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
      const std::string SUnit=
      	IParam.getValueError<std::string>("wwgCADIS",0,itemCnt++,
					  "CADIS Source Point");
      const std::string TUnit=
      	IParam.getDefValue<std::string>(SUnit,"wwgCADIS",0,itemCnt);
      
      std::string ptType,sndPtType;
      size_t ptIndex,sndPtIndex;
      bool adjointFlag,sndAdjointFlag;
      WeightControl::processPtString(SUnit,ptType,ptIndex,adjointFlag);
      WeightControl::processPtString(TUnit,sndPtType,sndPtIndex,sndAdjointFlag);
      const std::vector<Geometry::Vec3D>& GridMidPt=wwg.getMidPoints();
      
      if (ptType=="Plane" && sndPtType=="Plane")
	{
	  sourceFlux->CADISnorm(System,*adjointFlux,
				EBand,GridMidPt,planePt[ptIndex],
                                planePt[sndPtIndex]);      
	}
      else if (ptType=="Source" && sndPtType=="Source")
	{
	  sourceFlux->CADISnorm(System,*adjointFlux,
				EBand,GridMidPt,sourcePt[ptIndex],
                                sourcePt[sndPtIndex]);
	}
      else
	{
	  ELog::EM<<"Mixed source/plane cadis not currently supported"
		  <<ELog::endErr;
	}
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
      ELog::EM<<"SDAFASDF "<<ELog::endDiag;
      WeightControl::processWeights(System,IParam);
      wwgSetParticles(activeParticles);

      procParam(IParam,"wWWG",0,0);
      wwgMesh(System,IParam);               // create mesh [wwgXMesh etc]
      wwgInitWeight();               // Zero arrays etc
      wwgCreate(System,IParam);      // LOG space
      ELog::EM<<"SDAFASDF "<<ELog::endDiag;
      sourceFlux->writeCHECK(100);
      wwgMarkov(System,IParam);
      wwgCombine(System,IParam);
      wwgNormalize(IParam);
      
      wwgVTK(IParam);	    
      for(const std::string& P : activeParticles)
	{
	  // don't write a wwp:particle card
	  WM.getParticle(P)->setActiveWWP(0);
	  SimMCNP* SimPtr=dynamic_cast<SimMCNP*>(&System);
	  if (SimPtr)
	    clearWImp(SimPtr->getPC(),P);

	}
    }

  return;
}


  
}  // NAMESPACE weightSystem

