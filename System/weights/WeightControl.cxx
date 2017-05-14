/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weights/WeightControl.cxx
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
#include "WeightControl.h"

namespace WeightSystem
{
  
WeightControl::WeightControl() :
  scaleFactor(1.0),minWeight(1e-20),weightPower(0.5),
  density(1.0),r2Length(1.0),r2Power(2.0),nMarkov(0),
  activeAdjointFlag(0),activePtType("Void"),activePtIndex(0)
  /*
    Constructor
  */
{
  setHighEBand();
}

WeightControl::WeightControl(const WeightControl& A) :
  scaleFactor(A.scaleFactor),minWeight(A.minWeight),
  weightPower(A.weightPower),EBand(A.EBand),WT(A.WT),
  nMarkov(A.nMarkov),objectList(A.objectList),
  activeAdjointFlag(A.activeAdjointFlag),
  activePtType(A.activePtType),activePtIndex(A.activePtIndex),
  sourcePt(A.sourcePt)
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
      energyCut=A.energyCut;
      scaleFactor=A.scaleFactor;
      minWeight=A.minWeight;
      weightPower=A.weightPower;
      density=A.density;
      r2Length=A.r2Length;
      r2Power=A.r2Power;
      
      EBand=A.EBand;
      WT=A.WT;
      objectList=A.objectList;
      activeAdjointFlag=A.activeAdjointFlag;
      activePtType=A.activePtType;
      activePtIndex=A.activePtIndex;
      sourcePt=A.sourcePt;
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
  WT={ 0.2,  0.7,  0.8,  0.9,  0.9, 1.0 };
  return;
}

void
WeightControl::processPtString(std::string ptStr) 
  /*!
    Process a point with PtStr 
    -- Note that this is a check of the string.

    The input string is of the form
    [TS]{P}Index
    - T/S designated source / tally
    - P [optional] indicates that a plane is used not a point
    - [index] : number of source/plane/tally point
    - {P}power : P + power of weight [optional]

    Example TS2P0.8  --> Tally type : Source 2 Power 0.8
    \param ptStr :: String to process
  */
{
  ELog::RegMethod RegA("WeightControl","processPtString");

  const static std::map<char,std::string> TypeMap
    ({ { 'S',"Source" }, {'P',"Plane"}, {'C',"Cone"} });

  if (ptStr.size()>1)
    {
      const std::string Input(ptStr);
      const char SP=static_cast<char>(std::toupper(ptStr[0]));
      const char TP=static_cast<char>(std::toupper(ptStr[1]));
      if (SP!='T' && SP!='S')  // fail
	throw ColErr::InvalidLine(Input,"PtStr[0] expected:: [ST] [SPC] number");


      std::map<char,std::string>::const_iterator mc=TypeMap.find(TP);
      if (mc==TypeMap.end())
        throw ColErr::InvalidLine
          (Input,"PtStr[1] expected:: [ST] [SPC] number");

      activeAdjointFlag= (SP=='T') ? 1 : 0;
	      
      activePtType=mc->second;

      ptStr[0]=' ';
      ptStr[1]=' ';
      if (!StrFunc::sectPartNum(ptStr,activePtIndex))
	throw ColErr::InvalidLine(Input,"PtStr");      

      if (!ptStr.empty() && ptStr[0]=='P')
        {
          ptStr[0]=' ';
          StrFunc::section(ptStr,weightPower);
        }
    }
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
WeightControl::procMarkov(const mainSystem::inputParam& IParam,
			  const std::string& unitName,
			  const size_t iSet)
  /*!
    Process a wwgMarkov type card 
    \param IParam :: Input parameters
    \param unitName :: keyname to used 
    \param iSet :: group number [normally 0 as only one.]
  */
{
  ELog::RegMethod RegA("WeightControl","procMarkov");

  // picks up markov count first then other factors:
  size_t index(0);
  nMarkov=IParam.getValue<size_t>(unitName,iSet,index++);
  energyCut=IParam.getDefValue<double>(0.0,unitName,iSet,index++);
  scaleFactor=IParam.getDefValue<double>(1.0,unitName,iSet,index++);
  minWeight=IParam.getDefValue<double>(1e-20,unitName,iSet,index++);
  density=IParam.getDefValue<double>(1.0,unitName,iSet,index++);
  r2Length=IParam.getDefValue<double>(1.0,unitName,iSet,index++);
  r2Power=IParam.getDefValue<double>(2.0,unitName,iSet,index++);

  if (scaleFactor>1.0)
    ELog::EM<<"density scale factor > 1.0 "<<ELog::endWarn;
  
  ELog::EM<<"Param("<<unitName<<")["<<iSet<<"] eC:"<<energyCut
	  <<" sF:"<<scaleFactor
    	  <<" minW:"<<minWeight
    	  <<" rho:"<<density
    	  <<" r2Len:"<<r2Length
	  <<" r2Pow:"<<r2Power<<ELog::endDiag;
  return;
  
  return;
}
    
void
WeightControl::procParam(const mainSystem::inputParam& IParam,
                         const std::string& unitName,
			 const size_t iSet,
                         const size_t iOffset)
  /*!
    Set the global constants based on a unit and the offset numbers
    \param IParam :: Input parameters
    \param unit :: unit string
    \param iSet :: group number [normally 0]
    \param iOffset :: offset number 
   */
{
  ELog::RegMethod RegA("WeightControl","procParam");

  const size_t nItem=IParam.itemCnt(unitName,iSet);

  size_t index(iOffset);
  if (index<nItem)
    {
      const std::string PIndex=
        IParam.getValue<std::string>(unitName,iSet,index++);
      processPtString(PIndex);
    }      
  
  energyCut=IParam.getDefValue<double>(0.0,unitName,iSet,index++);
  scaleFactor=IParam.getDefValue<double>(1.0,unitName,iSet,index++);
  minWeight=IParam.getDefValue<double>(1e-20,unitName,iSet,index++);
  density=IParam.getDefValue<double>(1.0,unitName,iSet,index++);
  r2Length=IParam.getDefValue<double>(1.0,unitName,iSet,index++);
  r2Power=IParam.getDefValue<double>(2.0,unitName,iSet,index++);

  if (scaleFactor>1.0)
    ELog::EM<<"density scale factor > 1.0 "<<ELog::endWarn;
  
  ELog::EM<<"Param("<<unitName<<")["<<iSet<<"] eC:"<<energyCut
	  <<" sF:"<<scaleFactor
    	  <<" minW:"<<minWeight
    	  <<" rho:"<<density
    	  <<" r2Len:"<<r2Length
	  <<" r2Pow:"<<r2Power<<ELog::endDiag;
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
WeightControl::procCalcHelp() const
  /*!
    Write the Rebase help
  */
{
  ELog::EM<<"weightCalc ::: \n"
    " TSItem energyCut densityScalar minWeight \n"
    " -- TSItem :: String  [TS](index)P{value} \n"
    "       --- T/S :: tally/source to use \n"
    "       --- index :: index [from zero] of source/tally point\n"
    "       --- P{value} :: Optional value for the power [default 0.5]"
	  <<ELog::endDiag;
  return;
}

void
WeightControl::procObjectHelp() const
  /*!
    Write the Rebase help
  */
{
  ELog::EM<<"weightObject ::: \n"
    "objectName:{cell name} TSItem energyCut densityScalar minWeight \n"
    " -- TSItem :: String  [TS](index)P{value} \n"
    "       --- T/S :: tally/source to use \n"
    "       --- index :: index [from zero] of source/tally point\n"
    "       --- P{value} :: Optional value for the power [default 0.5]"
	  <<ELog::endDiag;
  return;
}

void
WeightControl::procPlanePoint(const mainSystem::inputParam& IParam)
  /*!
    Determine inf the next component cat be a plane
    Given as two Vec3D from inputParam
    \param IParam :: Input parameters
  */
{
  ELog::RegMethod RegA("WeightControl","procPlanePoint");

  const std::string wKey("weightPlane");
  
  planePt.clear();
  const size_t NPlane=IParam.setCnt(wKey);
  for(size_t index=0;index<NPlane;index++)
    {
      const size_t NItem=IParam.itemCnt(wKey,index);
      size_t itemCnt(0);
      while(NItem>itemCnt)
        {
	  // This use object names / FC / and normal Vec3D
	  const Geometry::Vec3D PPoint=
            attachSystem::getCntVec3D(IParam,wKey,index,itemCnt);
	  const Geometry::Vec3D Norm=
	    IParam.getCntVec3D(wKey,index,itemCnt,
			       wKey+" Vec3D");

          planePt.push_back(Geometry::Plane(0,0,PPoint,Norm));
        }
    }

  return;
}


void
WeightControl::procSourcePoint(const mainSystem::inputParam& IParam)
  /*!
    Process the source weight point
    \param IParam :: Input param
  */
{
  ELog::RegMethod RegA("weightControl","procSourcePoint");

  const std::string wKey("weightSource");
  
  sourcePt.clear();
  const size_t NSource=IParam.setCnt(wKey);
  for(size_t index=0;index<NSource;index++)
    {
      const size_t NItem=IParam.itemCnt(wKey,index);
      size_t itemCnt(0);
      while(NItem>itemCnt)
        {
          const Geometry::Vec3D TPoint=
            attachSystem::getCntVec3D(IParam,wKey,index,itemCnt);
          ELog::EM<<"Source Point["<<sourcePt.size()
                  <<"] == "<<TPoint<<ELog::endDiag;
          sourcePt.push_back(TPoint);
        }
    }  
  return;
}

  
void
WeightControl::cTrack(const Simulation& System,
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
  ELog::RegMethod RegA("WeightControl","cTrack");
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
WeightControl::cTrack(const Simulation& System,
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
  ELog::RegMethod RegA("WeightControl","cTrack");
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
WeightControl::calcCellTrack(const Simulation& System,
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
  ELog::RegMethod RegA("WeightControl","calcCellTrack<Cone>");

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
WeightControl::calcCellTrack(const Simulation& System,
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
  ELog::RegMethod RegA("WeightControl","calcCellTrack<Plane>");

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
WeightControl::calcCellTrack(const Simulation& System,
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
  ELog::RegMethod RegA("WeightControl","calcCellTrack(Vec3D)");
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
WeightControl::procObject(const Simulation& System,
                          const mainSystem::inputParam& IParam)

/*!
  Function to set up the weights system.
  This is for the object tracked from a point

  weightObject lists : object / cell / 
  \param System :: Simulation component
  \param IParam :: input stream
*/
{
  ELog::RegMethod RegA("WeightControl","procObject");
  const ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  const size_t nSet=IParam.setCnt("weightObject");
  // default values:
  procParam(IParam,"weightControl",0,0);

  for(size_t iSet=0;iSet<nSet;iSet++)
    {
      const std::string Key=
        IParam.getValue<std::string>("weightObject",iSet,0);
      if (Key=="help")
        {
          procObjectHelp();
          return;
        }
      // local values:
      procParam(IParam,"weightObject",iSet,1);

      ELog::EM<<"ObjCell== "<<Key<<ELog::endDiag;
      objectList.insert(Key);      
      const std::vector<int> objCells=OR.getObjectRange(Key);
    
      if (objCells.empty())
        ELog::EM<<"Cell["<<Key<<"] empty on renumber"<<ELog::endWarn;

    
      if (activePtType=="Plane")   
        {
          if (activePtIndex>=planePt.size())
            throw ColErr::IndexError<size_t>(activePtIndex,planePt.size(),
                                             "planePt.size() < activePtIndex");
          CellWeight CW;
          calcCellTrack(System,planePt[activePtIndex],objCells,CW);
          if (!activeAdjointFlag)
            CW.updateWM(energyCut,scaleFactor,minWeight,weightPower);
          else
            CW.invertWM(energyCut,scaleFactor,minWeight,weightPower);
        }
      else if (activePtType=="Source")
        {
          if (activePtIndex>=sourcePt.size())
            throw ColErr::IndexError<size_t>
              (activePtIndex,sourcePt.size(),"sourcePt.size() < activePtIndex");
        
          CellWeight CW;
          calcCellTrack(System,sourcePt[activePtIndex],objCells,CW);
          if (!activeAdjointFlag)
            CW.updateWM(energyCut,scaleFactor,minWeight,weightPower);
          else
            CW.invertWM(energyCut,scaleFactor,minWeight,weightPower);
        }
      else if (activePtType=="Cone")
        {
          CellWeight CW;
          calcCellTrack(System,sourcePt[activePtIndex],objCells,CW);
        }
      else
        throw ColErr::InContainerError<std::string>
          (activePtType,"SourceType no known");
    }
  return;
}



void
WeightControl::scaleObject(const Simulation& System,
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
  ELog::RegMethod RegA("WeightControl","scaleObject");

  const ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

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
WeightControl::scaleAllObjects(const Simulation& System,
                                 const double SW,const double eCut) 
  /*!
    Scale all individual object by the scale weight.
    \param System :: Simulation model
    \param SW :: Scale weight
    \param eCut :: Scale only eneries beyond value [-ve below/+ve above]
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
    \param eCut :: Scale only eneries beyond value [-ve below/+ve above]
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

  if (IParam.flag("weightType"))
    procType(IParam);
  if (IParam.flag("weight"))
    setWeights(System);
      
  // requirements for vertex:
  if (IParam.flag("weightObject") ||
      IParam.flag("tallyWeight") )
    System.calcAllVertex();
  
  if (IParam.flag("weightSource"))
    procSourcePoint(IParam);
  if (IParam.flag("weightPlane"))
    procPlanePoint(IParam);
    
  if (IParam.flag("weightObject"))
    procObject(System,IParam);
        
  if (IParam.flag("wWWG"))
    procWWGWeights(System,IParam);

  
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
  setWCellImp(System);
  //  removePhysImp(System,"n");
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
  ELog::EM<<"-- weightPlane --::"<<ELog::endDiag;
  ELog::EM<<"-- weightTally --::"<<ELog::endDiag;
  ELog::EM<<"-- weightObject --::"<<ELog::endDiag;
  procObjectHelp();
  ELog::EM<<"-- weightRebase --::"<<ELog::endDiag;
  procRebaseHelp();
  ELog::EM<<"-- wWWG --::"<<ELog::endDiag;
  ELog::EM<<"-- wwgNorm -- minWeight power :: 10^-minWeight and w=w^power "<<ELog::endDiag;
  ELog::EM<<"-- wwgCalc --::"<<ELog::endDiag;
  ELog::EM<<"-- wwgMarkov --::"<<ELog::endDiag;
  ELog::EM<<"-- wwgRPtMesh -- set hte reference point for the mesh ::"<<ELog::endDiag;
  ELog::EM<<"-- wwgVTK --::"<<ELog::endDiag;
  procCalcHelp();

  ELog::EM<<"-- wFCL --:: Set forced collision"<<ELog::endDiag;
  ELog::EM<<"-- wIMP --:: Set Importance"<<ELog::endDiag;

  ELog::EM<<"-- weightTemp --::"<<ELog::endDiag;
  ELog::EM<<"-- tallyWeight --::"<<ELog::endDiag;
  return;
}

		       
}  // NAMESPACE weightSystem

