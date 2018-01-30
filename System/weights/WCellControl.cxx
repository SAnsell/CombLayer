/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weights/WCellControl.cxx
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
#include "TempWeights.h"
#include "WeightControl.h"
#include "WCellControl.h"

namespace WeightSystem
{
  
WCellControl::WCellControl() :
  WeightControl()
  /*
    Constructor
  */
{}


WCellControl::~WCellControl()
  /*!
    Destructor
   */
{}


void
WCellControl::procRebase(const Simulation& System,
			  const mainSystem::inputParam& IParam)
  /*!
    Rebase a weight syste
    \param System :: Simulation component
    \param IParam :: input stream
  */
{
  ELog::RegMethod RegA("WCellControl","procRebase");

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
	dynamic_cast<WeightSystem::WCells*>(WM.getParticle("n"));
      if (!WF)
        throw ColErr::DynamicConv("WForm","WCells","neutron particles");
      
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
WCellControl::procObject(const Simulation& System,
                          const mainSystem::inputParam& IParam)

/*!
  Function to set up the weights system.
  This is for the object tracked from a point

  weightObject lists : object / cell / 
  \param System :: Simulation component
  \param IParam :: input stream
*/
{
  ELog::RegMethod RegA("WCellControl","procObject");

  const double minWeight(1e-16);
  
  const ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  
  
  const size_t nSet=IParam.setCnt("weightObject");
  // default values:
  procParam(IParam,"weightControl",0,0);

  for(size_t iSet=0;iSet<nSet;iSet++)
    {
      const std::string objectKey=
        IParam.getValueError<std::string>
        ("weightObject",iSet,0,"Empty");
      if (objectKey=="help")
        {
          procObjectHelp();
          return;
        }

      const std::string sourceKey=
        IParam.getValueError<std::string>("weightObject",iSet,1,"SourceKey empty");
      
      // pointType / local values
      std::string ptType;
      size_t ptIndex;
      bool adjointFlag;
      processPtString(sourceKey,ptType,ptIndex,adjointFlag);
      procParam(IParam,"weightObject",iSet,2);

      objectList.insert(objectKey);      
      const std::vector<int> objCells=OR.getObjectRange(objectKey);
    
      if (objCells.empty())
        ELog::EM<<"Cell["<<objectKey<<"] empty on renumber"<<ELog::endWarn;

      std::string activePtType;
      if (ptType=="Plane")   
        {
          if (ptIndex>=planePt.size())
            throw ColErr::IndexError<size_t>(ptIndex,planePt.size(),
                                             "planePt.size() < activePtIndex");
          CellWeight CW;
          calcCellTrack(System,planePt[ptIndex],objCells,CW);
          if (!adjointFlag)
            CW.updateWM(energyCut,scaleFactor,minWeight,weightPower);
          else
            CW.invertWM(energyCut,scaleFactor,minWeight,weightPower);
        }
      else if (ptType=="Source")
        {
          if (ptIndex>=sourcePt.size())
            throw ColErr::IndexError<size_t>
              (ptIndex,sourcePt.size(),"sourcePt.size() < activePtIndex");
        
          CellWeight CW;
          calcCellTrack(System,sourcePt[ptIndex],objCells,CW);
          if (!adjointFlag)
            CW.updateWM(energyCut,scaleFactor,minWeight,weightPower);
          else
            CW.invertWM(energyCut,scaleFactor,minWeight,weightPower);
        }
      else if (ptType=="Cone")
        {
          CellWeight CW;
          calcCellTrack(System,sourcePt[ptIndex],objCells,CW);
        }
      else
        throw ColErr::InContainerError<std::string>
          (activePtType,"SourceType no known");
    }
  return;
}

void
WCellControl::scaleObject(const Simulation& System,
                           const std::string& objKey,
                           const double SW,
                           const double eCut)
  /*!
    Scale all individual object by the scale weight.
    \param System :: Simulation model
    \param objKey :: Object key name
    \param SW :: Scale weight
    \param eCut :: min energy to use
  */
{
  ELog::RegMethod RegA("WCellControl","scaleObject");

  const ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  WeightSystem::weightManager& WM=
    WeightSystem::weightManager::Instance();  

  WeightSystem::WCells* WF=
    dynamic_cast<WeightSystem::WCells*>(WM.getParticle("n"));

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
    }
  std::vector<int> cellVec=OR.getObjectRange(objKey);
  for(const int cellN : cellVec)
    {
      const MonteCarlo::Qhull* CellPtr=System.findQhull(cellN);
      if (CellPtr && CellPtr->getMat())
        WF->scaleWeights(cellN,WEng);
    }
  return;
}
  
void
WCellControl::scaleAllObjects(const Simulation& System,
                                 const double SW,const double eCut) 
  /*!
    Scale all individual object by the scale weight.
    \param System :: Simulation model
    \param SW :: Scale weight
    \param eCut :: Scale only eneries beyond value [-ve below/+ve above]
  */
{
  ELog::RegMethod RegA("WCellControl","scaleAllObjects");
  
  
  for(const std::string& objName : objectList)
    scaleObject(System,objName,SW,eCut);
  return;
}

double
WCellControl::findMax(const Simulation& System,
                       const std::string& objKey,
                       const size_t index,
                       const double eCut) const
  /*!
    Find the maximum value in a cell
    \param System :: Simulation for cell materials
    \param objKey :: Object Name
    \param index ;: index value [+1 option / 0 all]
    \param eCut :: Scale only eneries beyond value [-ve below/+ve above]
    \return value for max
  */
{
  ELog::RegMethod RegA("WCellControl","findMax");
  
  const ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  WeightSystem::weightManager& WM=
    WeightSystem::weightManager::Instance();  

  WeightSystem::WForm* WF=WM.getParticle("n");
  if (!WF)
    throw ColErr::InContainerError<std::string>("n","neutron has no WForm");
  const std::vector<double>& WEng=WF->getEnergy();
  const long int eIndex=
    std::lower_bound(WEng.begin(),WEng.end(),std::abs(eCut))-WEng.begin();

  std::vector<int> cellRange=OR.getObjectRange(objKey);
  
  
  double maxVal(0.0);
  int cellN(0);
  double M(1.0);
  size_t foundCellCnt(0);
  for(const int CN : cellRange)
    {
      const MonteCarlo::Qhull* CellPtr=System.findQhull(CN);
      if (CellPtr && CellPtr->getMat())
	{
          foundCellCnt++;
	  const std::vector<double> WVec=WF->getWeights(CN);
	  if (!index && eCut>0.0)
	    M= *std::max_element(WVec.begin()+eIndex,WVec.end());
	  else if (!index)
	    M= *std::max_element(WVec.begin(),WVec.begin()+eIndex);
	  else
	    M=WVec[index-1];
	  if (M>maxVal)
	    {
	      maxVal=M;
	      cellN=CN;
	    }
	}
    }
  
  ELog::EM<<"Max Cell["<<objKey<<"] = "
          <<cellN<<" "<<maxVal<<"=="<<foundCellCnt<<ELog::endDiag;
  
  return (cellN) ? maxVal : 1.0;
}

   
void
WCellControl::cTrack(const Simulation& System,
                      const Geometry::Vec3D& initPt,
                      const std::vector<Geometry::Vec3D>& Pts,
                      const std::vector<long int>& index,
                      CellWeight& CTrack)
  /*!
    Calculate a specific track from sourcePoint to  postion
    \param System :: Simulation to use    
    \param initPt :: point for outgoing track
    \param Pts :: Point on track
    \param index :: cellnumber / index number
    \param CTrack :: Item Weight to add tracks to
  */
{
  ELog::RegMethod RegA("WCellControl","cTrack");
  // SOURCE Point

  ModelSupport::ObjectTrackPoint OTrack(initPt);
  std::vector<double> WVec;

  long int cN(index.empty() ? 1 : index.back());
  for(size_t i=0;i<Pts.size();i++)
    {
      const long int unit(i>=index.size() ? cN++ : index[i]);
      OTrack.addUnit(System,unit,Pts[i]);
      CTrack.addTracks(unit,OTrack.getAttnSum(unit));
    } 
  return;
}

void
WCellControl::cTrack(const Simulation& System,
                      const Geometry::Plane& initPlane,
                      const std::vector<Geometry::Vec3D>& Pts,
                      const std::vector<long int>& index,
                      CellWeight& CTrack)
  /*!
    Calculate a specific trac from sourcePoint to  postion
    \param System :: Simulation to use    
    \param initPlane :: Plane for outgoing track
    \param Pts :: Point on track
    \param index :: cellnumber / index number
    \param CTrack :: Item Weight to add tracks to
  */
{
  ELog::RegMethod RegA("WCellControl","cTrack");
  // SOURCE Point

  ModelSupport::ObjectTrackPlane OTrack(initPlane);
  std::vector<double> WVec;

  long int cN(index.empty() ? 1 : index.back());
  for(size_t i=0;i<Pts.size();i++)
    {
      const long int unit(i>=index.size() ? cN++ : index[i]);
      OTrack.addUnit(System,unit,Pts[i]);
      CTrack.addTracks(unit,OTrack.getAttnSum(unit));
    } 
  return;
}

void
WCellControl::calcCellTrack(const Simulation& System,
                            const Geometry::Cone& curCone,
                            CellWeight& CTrack)
/*!
  Calculate a given cone : calculate those cells
  that the cone intersects
  \param System :: Simulation to use
  \param curPlane :: current plane
  \param cellVec :: Cells to track
  \param CTrack :: Cell Weights for output 
*/
{
  ELog::RegMethod RegA("WCellControl","calcCellTrack<Cone>");

  CTrack.clear();
  std::vector<Geometry::Vec3D> Pts;
  std::vector<long int> index;
  /*
    for(const int cellN : cellVec)
    {
    const MonteCarlo::Qhull* CellPtr=System.findQhull(cellN);
    if (CellPtr && CellPtr->getMat())
    {
    index.push_back(CellPtr->getName());  // this should be cellN ??
    Pts.push_back(CellPtr->getCofM());
    }
    }
    cTrack(System,curPlane,Pts,index,CTrack);
  */
  return;
}

void
WCellControl::calcCellTrack(const Simulation& System,
                             const Geometry::Plane& curPlane,
                             const std::vector<int>& cellVec,
                             CellWeight& CTrack)
/*!
  Calculate a given track 
  \param System :: Simulation to use
  \param curPlane :: current plane
  \param cellVec :: Cells to track
  \param CTrack :: Cell Weights for output 
*/
{
  ELog::RegMethod RegA("WCellControl","calcCellTrack<Plane>");

  CTrack.clear();
  std::vector<Geometry::Vec3D> Pts;
  std::vector<long int> index;

  for(const int cellN : cellVec)
    {
      const MonteCarlo::Qhull* CellPtr=System.findQhull(cellN);
      if (CellPtr && CellPtr->getMat())
        {
          index.push_back(CellPtr->getName());  // this should be cellN ??
          Pts.push_back(CellPtr->getCofM());
        }
    }

  cTrack(System,curPlane,Pts,index,CTrack);
  return;
}

void
WCellControl::calcCellTrack(const Simulation& System,
                             const Geometry::Vec3D& initPt,
                             const std::vector<int>& cellVec,
                             CellWeight& CTrack)
  /*!
    Calculate a given track for a point
    \param System :: Simulation to use
    \param initPt :: point for outgoing track
    \param cellVec :: Cells to track
    \param CTrack :: Cell Weights for output 
  */
{
  ELog::RegMethod RegA("WCellControl","calcCellTrack(Vec3D)");
  CTrack.clear();
  std::vector<Geometry::Vec3D> Pts;
  std::vector<long int> index;

  for(const int cellN : cellVec)
    {
      const MonteCarlo::Qhull* CellPtr=System.findQhull(cellN);
      if (CellPtr && CellPtr->getMat())
        {
          index.push_back(CellPtr->getName());  // this should be cellN ??
          Pts.push_back(CellPtr->getCofM());
          ELog::EM<<"Cell Track = "<<initPt<<" : "<<index.back()
                  <<" : [COM] "<<Pts.back()<<ELog::endDiag;
        }
    }

  cTrack(System,initPt,Pts,index,CTrack);
  return;
}
void
WCellControl::setWeights(Simulation& System,
			 const std::string& particleType)
   /*!
    Function to set up the weights system.
    It replaces the old file read system.
    \param System :: Simulation component
    \param particleType :: Particle type
  */
{
  ELog::RegMethod RegA("WCellControl","setWeights(Simulation)");

  WeightSystem::weightManager& WM=
    WeightSystem::weightManager::Instance();  

  WM.addParticle<WeightSystem::WCells>(particleType);
  WeightSystem::WCells* WF=
    dynamic_cast<WeightSystem::WCells*>(WM.getParticle(particleType));
  if (!WF)
    throw ColErr::InContainerError<std::string>
      (std::string(particleType,1),"WCell - WM");

  WF->setEnergy(EBand);
  WF->populateCells(System.getCells());
  WF->balanceScale(WT);
  WF->maskCell(1);

  // mask imp:n etc
  setWImp(System,particleType);
  
  return;
}

void
WCellControl::normWeights(Simulation& System,
                           const mainSystem::inputParam& IParam)
  /*!
    Normalize the weight after the run
    \param System :: Simualtion to use
  */
    
{
  ELog::RegMethod RegA("WCellControl","normWeights");
  
  
  if (IParam.flag("weightTemp"))
    scaleTempWeights(System,10.0);
  
  if (IParam.flag("weightRebase"))
    procRebase(System,IParam);

  return;
}

void
WCellControl::processWeights(Simulation& System,
			     const mainSystem::inputParam& IParam)
  /*!
    Set individual weights based on temperature/cell
    Call via WeightControl processWeight
    \param System :: Simulation
    \param IParam :: input stream
  */
{
  ELog::RegMethod RegA("WCellControl","processWeights");

  WeightControl::processWeights(System,IParam);
  if (IParam.flag("weight"))
    {
      for(const std::string& P : activeParticles)
	{
	  setWeights(System,P);
	}
    }

  // requirements for vertex:
  if (IParam.flag("weightObject") ||
      IParam.flag("tallyWeight") )
    System.calcAllVertex();
  
    
  if (IParam.flag("weightObject"))
    procObject(System,IParam);


  return;
}
  
}  // NAMESPACE weightSystem

