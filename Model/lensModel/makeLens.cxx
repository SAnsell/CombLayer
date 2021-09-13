/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   lensModel/makeLens.cxx
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
#include <iterator>
#include <memory>
#include <array>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "support.h"
#include "inputParam.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimMCNP.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedUnit.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "World.h"
#include "siModerator.h"
#include "candleStick.h"
#include "ProtonFlight.h"
#include "FlightLine.h"
#include "FlightCluster.h"
#include "layers.h"

#include "LensTally.h"
#include "makeLens.h"

namespace lensSystem
{

makeLens::makeLens() :
  SiModObj(new siModerator("siMod")),
  candleObj(new candleStick("candle")),
  layerObj(new layers("layers"))

  /*!
    Constructor
  */
{
  ELog::RegMethod RegA("makeLens","Constructor");
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(SiModObj);
  OR.addObject(candleObj);
  OR.addObject(layerObj);
}

makeLens::makeLens(const makeLens& A) : 
  SiModObj(new siModerator(*A.SiModObj)),
  candleObj(new candleStick(*A.candleObj)),
  layerObj(new layers(*A.layerObj))
  /*!
    Copy constructor
    \param A :: makeLens to copy
  */
{}

makeLens&
makeLens::operator=(const makeLens& A)
  /*!
    Assignment operator
    \param A :: makeLens to copy
    \return *this
  */
{
  if (this!=&A)
    {
      *SiModObj=*A.SiModObj;
      *candleObj=*A.candleObj;
      *layerObj=*A.layerObj;
    }
  return *this;
}

makeLens::~makeLens()
  /*!
    Destructor
   */
{}

void
makeLens::setMaterials(const mainSystem::inputParam&)
  /*!
    Builds all the materials in the objects 
    \param IParam :: Table of information
  */
{
  //  const std::string PName=IParam.getValue<std::string>("polyMat");
  //  const std::string SiName=IParam.getValue<std::string>("siMat");

  //  SiModObj->setMaterial(PName,);
  return;
}


void 
makeLens::build(Simulation* SimPtr)
  /*!
    Carry out the full build
    \param SimPtr :: Simulation system
   */
{
  // For output stream
  ELog::RegMethod RControl("makeLens","build");

  layerObj->addInsertCell(74123);  

  SiModObj->createAll(*SimPtr,World::masterOrigin(),0);
  candleObj->setBasePoint(SiModObj->getLinkPt(5));
  candleObj->createAll(*SimPtr,*SiModObj,0);
  //  candleObj->specialExclude(*SimPtr,74123);
  
  layerObj->build(*SimPtr,*candleObj);
  return;
}

const FlightCluster&
makeLens::getFlightCluster() const 
  /*!
    Access the flight cluster
    \return FlightCluster object
  */
{
  ELog::RegMethod RegA("makeLens","getFC");
  if (!layerObj)
    throw ColErr::EmptyValue<void>("LayerObject ");
 
  return layerObj->getFlightCluster();
}

void
makeLens::createTally(SimMCNP& System,
		      const mainSystem::inputParam& IParam)
  /*!
    Create all the tallies for lens
    \param System :: Simulation 
    \param IParam :: Input paramter 
  */
{
  ELog::RegMethod RegA("makeLens","createTally");
  
  const size_t NSTally=IParam.setCnt("surfTally");
  //  const size_t NEng=IParam.setCnt("tallyEnergy");
  
  size_t FL;
  double Dist;
  for(size_t i=0;i<NSTally;i++)
    {
      std::string A=
	IParam.getValue<std::string>("surfTally",i,0);
      std::string B=
	IParam.getValue<std::string>("surfTally",i,1);
      if ( StrFunc::convert(A,FL) && StrFunc::convert(B,Dist) )
	{
	  ELog::EM<<"Creating tally FL"<<FL+1
		  <<" at "<<Dist<<" cm "<<ELog::endDiag;
	  lensSystem::addSurfTally(System,getFlightCluster(),FL,Dist);
	}
    }

  return;
}


}   // NAMESPACE lensSystem
