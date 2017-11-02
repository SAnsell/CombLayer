/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   lensModel/makeLens.cxx
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
#include <iterator>
#include <memory>
#include <array>

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
#include "support.h"
#include "inputParam.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Rules.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
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

  SiModObj->createAll(*SimPtr);
  candleObj->createAll(*SimPtr,*SiModObj);
  //  candleObj->specialExclude(*SimPtr,74123);
  
  layerObj->createAll(*SimPtr,*candleObj);
  return;
}

const FlightCluster&
makeLens::getFC() const 
  /*!
    Access the flight cluster
    \return FlightCluster object
  */
{
  ELog::RegMethod RegA("makeLens","getFC");
  if (!layerObj)
    throw ColErr::EmptyValue<void>("LayerObject "+RegA.getFull());
 
  return layerObj->getFC();
}

void
makeLens::createTally(Simulation& System,
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
	  lensSystem::addSurfTally(System,getFC(),FL,Dist);
	}
    }

  return;
}


}   // NAMESPACE lensSystem
