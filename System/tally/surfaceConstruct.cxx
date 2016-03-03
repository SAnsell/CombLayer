/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tally/surfaceConstruct.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include <boost/format.hpp>

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
#include "Triple.h"
#include "support.h"
#include "stringCombine.h"
#include "surfRegister.h"
#include "surfIndex.h"
#include "objectRegister.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "BaseMap.h"
#include "SurfMap.h"
#include "Simulation.h"
#include "inputParam.h"
#include "NList.h"
#include "NRange.h"
#include "Tally.h"
#include "TallyCreate.h"

#include "TallySelector.h" 
#include "basicConstruct.h" 
#include "surfaceConstruct.h" 

namespace tallySystem
{

surfaceConstruct::surfaceConstruct() 
  /*!
    Constructor
  */
{}

surfaceConstruct::surfaceConstruct(const surfaceConstruct&) 
  /*! 
    Copy Constructor
  */
{}

surfaceConstruct&
surfaceConstruct::operator=(const surfaceConstruct&) 
  /*! 
    Assignment operator
  */
{
  return *this;
}

int
surfaceConstruct::processSurface(Simulation& System,
			   const mainSystem::inputParam& IParam,
			   const size_t Index) const
  /*!
    Add surface tally (s) as needed
    \param System :: Simulation to add tallies
    \param IParam :: Main input parameters
    \param Index :: index of the -T card
  */
{
  ELog::RegMethod RegA("surfaceConstruct","processSurface");

  const size_t NItems=IParam.itemCnt("tally",Index);
  if (NItems<3)
    throw ColErr::IndexError<size_t>(NItems,3,
				     "Insufficient items for tally");
  const std::string pType(IParam.getValue<std::string>("tally",Index,1)); 
  


  const std::string MType(IParam.getValue<std::string>("tally",Index,2)); 
  ELog::EM<<"Surface Tally Type == "<<pType<<ELog::endDiag;
  std::vector<std::string> excludeSurf;
  if (pType=="object")
    {
      const std::string place=
	inputItem<std::string>(IParam,Index,2,"position not given");
      const std::string snd=
	inputItem<std::string>(IParam,Index,3,"front/back/side not give");
      const long int linkNumber=getLinkIndex(snd);
      return processSurfObject(System,place,linkNumber,excludeSurf);
    }
  if (pType=="surfMap")
    {
      const std::string object=
	IParam.outputItem<std::string>
	("tally",Index,2,"Object component not given");
      const std::string surfItem=
	IParam.outputItem<std::string>
	("tally",Index,3,"SurfMap set key not give");
      // This should be a range:
      const long int surfIndex=
	IParam.outputItem<long int>
	("tally",Index,4,"position not given");
      
      Geometry::Vec3D Axis;
      size_t itemIndex(5);
      IParam.checkCntVec3D("tally",Index,itemIndex,Axis);
      
      return processSurfMap(System,object,surfItem,surfIndex);
    }

  if (pType=="viewObject")
    {
      const std::string place=
	inputItem<std::string>(IParam,Index,2,"position not given");
      const std::string snd=
	inputItem<std::string>(IParam,Index,3,"front/back/side not give");
      const long int linkNumber=getLinkIndex(snd);
      std::vector<int> surfN;
      const size_t maxIndex=IParam.itemCnt("tally",Index);
      for(size_t i=4;i<maxIndex;i++)
	{
	  // Check for surface by S/R designator at end [to do]
	  const std::string ST=
	    IParam.getValue<std::string>("tally",Index,i);
	  excludeSurf.push_back(ST);
	}
      return processSurfObject(System,place,linkNumber,excludeSurf);
    }
  ELog::EM<<"Surface Tally NOT Processed"<<ELog::endErr;
  return 0;
}


int
surfaceConstruct::processSurfObject(Simulation& System,
				    const std::string& FObject,
				    const long int linkPt,
				    const std::vector<std::string>& linkN) const
  /*!
    Process a surface tally on a registered object
    \param System :: Simulation to add tallies
    \param FObject :: Fixed/Twin name
    \param linkPt :: Link point [-ve for beam object]
    \param linkN :: surface exclude number for making a region of interest
  */
{
  ELog::RegMethod RegA("surfaceConstruct","processSurfObject");
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  const int tNum=System.nextTallyNum(1);
  if (linkPt)
    {
      const attachSystem::FixedComp* TPtr=
	OR.getObject<attachSystem::FixedComp>(FObject);
      if (!TPtr)
	throw ColErr::InContainerError<std::string>
	  (FObject,"Fixed Object not found");

      
      const size_t iLP=(linkPt>0) ?
	static_cast<size_t>(linkPt-1) : static_cast<size_t>(-1-linkPt);
      const int masterPlane=  
	TPtr->getMasterSurf(iLP);
      std::vector<int> surfN;
      for(size_t i=0;i<linkN.size();i++)
	{
	  const long int LIndex=getLinkIndex(linkN[i]);
	  surfN.push_back(TPtr->getSignedLinkSurf(LIndex));
	}
      const int signV((linkPt>0) ? 1 : -1);
      addF1Tally(System,tNum,signV*masterPlane,surfN);
      return 1;
    }
  return 0;
}

int
surfaceConstruct::processSurfMap(Simulation& System,
				 const std::string& SObject,
				 const std::string& SurfUnit,
				 const long int linkPt) const
  /*!
    Process a surface tally on a registered object
    \param System :: Simulation to add tallies
    \param FObject :: SurfMap
    \param linkPt :: Link point [-ve for beam object]
  */
{
  ELog::RegMethod RegA("surfaceConstruct","processSurfMap");
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  ModelSupport::surfIndex& SurI=
    ModelSupport::surfIndex::Instance();
  
  const int tNum=System.nextTallyNum(1);
  if (linkPt)
    {
      const attachSystem::SurfMap* SPtr=
	OR.getObject<attachSystem::SurfMap>(SObject);
      if (!SPtr)
	throw ColErr::InContainerError<std::string>
	  (SObject,"Fixed Object not found");

      const int side=(linkPt>0) ? 1 : -1;
      const size_t index=(linkPt>0) ? static_cast<size_t>(linkPt-1) :
	static_cast<size_t>(-linkPt-1);

      const int surf=SPtr->getSurf(SurfUnit,index);

      const int signV((linkPt>0) ? 1 : -1);
      addF1Tally(System,tNum,side*surf);
      return 1;
    }
  return 0;
}

void
surfaceConstruct::writeHelp(std::ostream& OX) const
  /*!
    Write out help
    \param Output stream
  */
{
  OX<<"Surface tally options:\n"
    <<"object linkName\n"
    <<"object objectName front/back \n"
    <<"viewObject objectName front/back/N {1-4 designator} \n";
  return;
}


}  // NAMESPACE tallySystem
