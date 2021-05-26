/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weights/WeightControl.cxx
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
#include "Cone.h"
#include "support.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "inputParam.h"

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
WeightControl::procSourcePoint(const Simulation& System,
			       const mainSystem::inputParam& IParam)
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
	    mainSystem::getNamedPoint
	    (System,IParam,wKey,index,itemCnt,"Source Point");
          const Geometry::Vec3D offsetPoint=
	    mainSystem::getDefNamedPoint
	    (System,IParam,wKey,index,itemCnt,Geometry::Vec3D(0,0,0));	  
          ELog::EM<<"Source Point["<<sourcePt.size()
                  <<"] == "<<TPoint<<" "<<itemCnt<<ELog::endDiag;
	  
          sourcePt.push_back(TPoint);
        }
    }  
  return;
}
   
void
WeightControl::procPlanePoint(const Simulation& System,
			      const mainSystem::inputParam& IParam)
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
	    mainSystem::getNamedPoint
	    (System,IParam,wKey,index,itemCnt,wKey+"Plane Point");
	  const Geometry::Vec3D Norm=
	    mainSystem::getNamedAxis
	    (System,IParam,wKey,index,itemCnt,wKey+"Plane:Norm");
          planePt.push_back(Geometry::Plane(0,0,PPoint,Norm));
        }
    }

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
    procSourcePoint(System,IParam);
  if (IParam.flag("weightPlane"))
    procPlanePoint(System,IParam);

  return;
}


}  // NAMESPACE weightSystem

