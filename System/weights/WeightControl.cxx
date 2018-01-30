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
  scaleFactor(1.0),weightPower(0.5),
  density(1.0),r2Length(1.0),r2Power(2.0)
  /*
    Constructor
  */
{
  setHighEBand();
}

WeightControl::WeightControl(const WeightControl& A) : 
  activeParticles(A.activeParticles),
  energyCut(A.energyCut),scaleFactor(A.scaleFactor),
  weightPower(A.weightPower),
  density(A.density),r2Length(A.r2Length),r2Power(A.r2Power),
  EBand(A.EBand),WT(A.WT),conePt(A.conePt),planePt(A.planePt),
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
      activeParticles=A.activeParticles;
      energyCut=A.energyCut;
      scaleFactor=A.scaleFactor;
      weightPower=A.weightPower;
      density=A.density;
      r2Length=A.r2Length;
      r2Power=A.r2Power;
      EBand=A.EBand;
      WT=A.WT;
      conePt=A.conePt;
      planePt=A.planePt;
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
  ELog::EM<<"NSource == "<<NSource<<ELog::endDiag;
  for(size_t index=0;index<NSource;index++)
    {
      const size_t NItem=IParam.itemCnt(wKey,index);
      size_t itemCnt(0);
      while(NItem>itemCnt)
        {
          const Geometry::Vec3D TPoint=
	    IParam.getCntVec3D(wKey,index,itemCnt,"Source Point");
          ELog::EM<<"Source Point["<<sourcePt.size()
                  <<"] == "<<TPoint<<ELog::endDiag;
          sourcePt.push_back(TPoint);
        }
    }  
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
	  const Geometry::Vec3D PPoint=
	    IParam.getCntVec3D(wKey,index,itemCnt,wKey+":PlanePoint");

          //          IParam.getCntVec3D(wKey,index,itemCnt,
          //			       wKey+" Vec3D");
	  const Geometry::Vec3D Norm=
	    IParam.getCntVec3D(wKey,index,itemCnt,wKey+"PlaneNorm");
          planePt.push_back(Geometry::Plane(0,0,PPoint,Norm));
        }
    }

  return;
}
  
void
WeightControl::processPtString(std::string ptStr,
			       std::string& ptType,
			       size_t& ptIndex,
			       bool& adjointFlag)
  /*!
    Process a point with PtStr 
    -- Note that this is a check of the string.

    The input string is of the form
    [TS]{P}Index
    - T/S designated source / tally
    - P [optional] indicates that a plane is used not a point
    - [index] : number of source/plane/tally point

    Example TS2 --> Adjoint type : Source 2 
    \param ptStr :: String to process
    \param ptType :: Source/Plane/Cone
    \param ptIndex :: Index to SPC vector unit
    \param adjointFlag :: use Adjoint source
  */
{
  ELog::RegMethod RegA("WeightControl","processPtString");

  const static std::map<char,std::string> TypeMap
    ({ { 'S',"Source" }, {'P',"Plane"}, {'C',"Cone"} });

  if (ptStr.size()<2)
    throw ColErr::InvalidLine
      (ptStr,"PtStr[0] expected:: [ST] [SPC] number");  
  
  const std::string Input(ptStr);
  const char SP=static_cast<char>(std::toupper(ptStr[0]));
  const char TP=static_cast<char>(std::toupper(ptStr[1]));
  if (SP!='T' && SP!='S')  // fail
    throw ColErr::InvalidLine(Input,"PtStr[0] expected:: [ST] [SPC] number");
  
  std::map<char,std::string>::const_iterator mc=TypeMap.find(TP);
  if (mc==TypeMap.end())
    throw ColErr::InvalidLine
      (Input,"PtStr[1] expected:: [ST] [SPC] number");
  
  adjointFlag= (SP=='T') ? 1 : 0;  
  ptType=mc->second;
  
  ptStr[0]=' ';
  ptStr[1]=' ';
  if (!StrFunc::sectPartNum(ptStr,ptIndex))
    throw ColErr::InvalidLine(Input,"PtStr Index not found");

  if (ptType=="Plane" && ptIndex>=planePt.size())
    throw ColErr::IndexError<size_t>(ptIndex,planePt.size(),
				     "planePt.size() < ptIndex");
  else if (ptType=="Source" && ptIndex>=sourcePt.size())
    throw ColErr::IndexError<size_t>(ptIndex,sourcePt.size(),
				     "sourcePt.size() < ptIndex");

  return;
}

  
void
WeightControl::procParticles(const mainSystem::inputParam& IParam)
  /*!
    Extract the particles used for the weight system
    \param IParam :: input param
  */
{
  ELog::RegMethod RegA("weightControl","procParticles");

  activeParticles.clear();

  const size_t nItem=IParam.itemCnt("weightParticles",0);
  if (!nItem) activeParticles.insert("n");
  
  std::string PList;
  for(size_t index=0;index<nItem;index++)
    {
      PList=IParam.getValue<std::string>("weightParticles",0,index);
      std::string P;
      while(StrFunc::section(PList,P))
	{
	  activeParticles.insert(P);
	}
    }
  return;
}

void
WeightControl::procEnergyType(const mainSystem::inputParam& IParam)
  /*!
    Extract hte type from the input to process of energy system 
    \param IParam :: input param
  */
{
  ELog::RegMethod RegA("weightControl","procEnergyType");
  const std::string Type=IParam.getValue<std::string>("weightEnergyType");
  
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
      const size_t itemCnt=IParam.itemCnt("weightEnergyType",0);
      std::vector<double> E;
      std::vector<double> W;
      for(size_t i=1;i<itemCnt;i+=2)
	{
	  E.push_back(IParam.getValue<double>("weightEnergyType",i));
	  W.push_back(IParam.getValue<double>("weightEnergyType",i+1));
	}
      EBand=E;
      WT=W;
      
    }
  else if (Type=="help")
    {
      procEnergyTypeHelp();
    }
  return;
}

    
void
WeightControl::procParam(const mainSystem::inputParam& IParam,
                         const std::string& unitName,
			 const size_t iSet,
                         const size_t iOffset)
  /*!
    Set the global constants based on a unit and the offset numbers
    Numbers are in log space [nat log]
    \param IParam :: Input parameters
    \param unitName :: unit string
    \param iSet :: group number [normally 0]
    \param iOffset :: offset number 
   */
{
  ELog::RegMethod RegA("WeightControl","procParam");

  const size_t nItem=IParam.setCnt(unitName);


  if (iSet>nItem)
    throw ColErr::IndexError<size_t>(iSet,nItem,"iSet/nItem:"+unitName);
    
  size_t index(iOffset);  

  energyCut=IParam.getDefValue<double>(0.0,unitName,iSet,index++);
  scaleFactor=IParam.getDefValue<double>(1.0,unitName,iSet,index++);
  density=IParam.getDefValue<double>(1.0,unitName,iSet,index++);
  r2Length=IParam.getDefValue<double>(1.0,unitName,iSet,index++);
  r2Power=IParam.getDefValue<double>(2.0,unitName,iSet,index++);

  const size_t nIndex=IParam.itemCnt(unitName,iSet);
  const size_t nIndexB=IParam.itemCnt("WObject",iSet);
  
  ELog::EM<<"Param("<<unitName<<")["<<iSet<<"] eC:"<<energyCut
	  <<" sF:"<<scaleFactor
    	  <<" rho:"<<density
    	  <<" r2Len:"<<r2Length
	  <<" r2Pow:"<<r2Power<<ELog::endDiag;
  if (r2Power>0.1)
    weightPower=1.0/r2Power;
  if (scaleFactor>1.0)
    ELog::EM<<"density scale factor > 1.0 "<<ELog::endWarn;
  return;
}
    


void
WeightControl::normWeights(Simulation& System,
                           const mainSystem::inputParam& IParam)
  /*!
    Normalize the weights after the main processing event
    \param System :: simulation to use
    \param IParam :: Parameter
  */
    
{
  ELog::RegMethod RegA("WeightControl","normWeights");
  
  // This shoudl be elsewhere
  if (IParam.flag("tallyWeight"))
    tallySystem::addPointPD(System);

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

  procParticles(IParam);
  
  if (IParam.flag("weightEnergyType"))
    procEnergyType(IParam);


  if (IParam.flag("weightSource"))
    procSourcePoint(IParam);
  if (IParam.flag("weightPlane"))
    procPlanePoint(IParam);

  return;
}


}  // NAMESPACE weightSystem

