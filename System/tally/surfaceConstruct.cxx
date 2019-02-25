/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tally/surfaceConstruct.cxx
 *
 * Copyright (c) 2004-2010 by Stuart Ansell
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
#include "LinkSupport.h"
#include "BaseMap.h"
#include "SurfMap.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimMCNP.h"
#include "inputParam.h"
#include "NList.h"
#include "NRange.h"
#include "Tally.h"
#include "TallyCreate.h"

#include "TallySelector.h" 
#include "surfaceConstruct.h" 

namespace tallySystem
{

  
void
surfaceConstruct::processSurface(SimMCNP& System,
				 const int idType,
				 const mainSystem::inputParam& IParam,
				 const size_t Index)
  /*!
    Add surface tally as needed
    \param System :: SimMCNP to add tallies
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
  // Process a surface extracted from a linkPt of an object(FC)
  if (pType=="object")
    {
      const std::string place=
	IParam.getValueError<std::string>("tally",Index,2,"position not given");
      const std::string linkName=
	IParam.getValueError<std::string>
	("tally",Index,3,"front/back/side not give");
      
      processSurfObject(System,idType,place,linkName,excludeSurf);
      return;
    }

  // Process a surface extracted from a surfMap
  if (pType=="surfMap")
    {
      const std::string object=IParam.getValueError<std::string>
	("tally",Index,2,"Object component not given");

      const std::string surfItem=IParam.getValueError<std::string>
	("tally",Index,3,"SurfMap set key not given");

      // This should be a range (?):
      const long int surfIndex=IParam.getValueError<long int>
	("tally",Index,4,"Index Offset direction");
            
      processSurfMap(System,idType,object,surfItem,surfIndex);
      return;
    }

  if (pType=="viewObject")
    {
      const std::string place=IParam.getValueError<std::string>
	("tally",Index,2,"position not given");
      const std::string linkName=IParam.getValueError<std::string>
	("tally",Index,3,"front/back/side not give");

      std::vector<int> surfN;
      const size_t maxIndex=IParam.itemCnt("tally",Index);
      for(size_t i=4;i<maxIndex;i++)
	{
	  // Check for surface by S/R designator at end [to do]
	  const std::string ST=
	    IParam.getValue<std::string>("tally",Index,i);
	  excludeSurf.push_back(ST);
	}
      processSurfObject(System,idType,place,linkName,excludeSurf);
      return;
    }
  ELog::EM<<"Surface Tally NOT Processed"<<ELog::endErr;
  return;
}


int
surfaceConstruct::processSurfObject(SimMCNP& System,
				    const int idType,
				    const std::string& FObject,
				    const std::string& linkName,
				    const std::vector<std::string>& linkN)
  /*!
    Process a surface tally on a registered object
    \param System :: SimMCNP to add tallies
    \param FObject :: Fixed/Twin name
    \param linkName :: Link point [-ve for beam object]
    \param linkN :: surface exclude number for making a region of interest
    \return 1 on success / 0 on failure to find linkPt
  */
{
  ELog::RegMethod RegA("surfaceConstruct","processSurfObject");
  
  const int tNum=System.nextTallyNum(idType);
  const attachSystem::FixedComp* TPtr=
    System.getObjectThrow<attachSystem::FixedComp>(FObject,"FixedComp");
  const long int linkPt=TPtr->getSideIndex(linkName);
  if (linkPt)
    {
      const int masterPlane= TPtr->getLinkSurf(linkPt);
      std::vector<int> surfN;
      for(size_t i=0;i<linkN.size();i++)
	surfN.push_back(TPtr->getLinkSurf(linkN[i]));

      addF1Tally(System,tNum,masterPlane,surfN);
      return 1;
    }
  return 0;
}

int
surfaceConstruct::processSurfMap(SimMCNP& System,
				 const int idType,
				 const std::string& SObject,
				 const std::string& SurfUnit,
				 const long int linkIndex) 
  /*!
    Process a surface tally on a registered object
    \param System :: SimMCNP to add tallies
    \param SObject :: SurfMap object for surfaces
    \param SurfUnit :: Object within surfMap
    \param linkIndex :: Index of surface [or -ve for all]
    \return 1 on success / 0 on failure to find linkPt
  */
{
  ELog::RegMethod RegA("surfaceConstruct","processSurfMap");
  
  const int tNum=System.nextTallyNum(idType);
  if (linkIndex)
    {
      const attachSystem::SurfMap* SPtr=
	System.getObjectThrow<attachSystem::SurfMap>(SObject,"FixedComp");

      const int side=(linkIndex>0) ? 1 : -1;
      const size_t index=(linkIndex>0) ? static_cast<size_t>(linkIndex-1) :
	static_cast<size_t>(-linkIndex-1);

      const int surf=SPtr->getSurf(SurfUnit,index);

      addF1Tally(System,tNum,side*surf);
      return 1;
    }
  return 0;
}

void
surfaceConstruct::processSurfaceCurrent(SimMCNP& System,
                                        const mainSystem::inputParam& IParam,
                                        const size_t Index) 
/*!
    Add surface tally (s) as needed
    \param System :: SimMCNP to add tallies
    \param IParam :: Main input parameters
    \param Index :: index of the -T card
    \return 1 on success / 0 on failure 
  */
{
  ELog::RegMethod RegA("surfaceConstruct","processSurfaceCurrent");
  processSurface(System,1,IParam,Index);
  return;
}

void
surfaceConstruct::processSurfaceFlux(SimMCNP& System,
                                     const mainSystem::inputParam& IParam,
                                     const size_t Index) 
/*!
    Add surface tally (s) as needed
    \param System :: SimMCNP to add tallies
    \param IParam :: Main input parameters
    \param Index :: index of the -T card
  */
{
  ELog::RegMethod RegA("surfaceConstruct","processSurfaceCurrent");
  
  processSurface(System,2,IParam,Index);
  return;
}


  
void
surfaceConstruct::writeHelp(std::ostream& OX) 
  /*!
    Write out help
    \param OX :: Output stream
  */
{
  OX<<"Surface tally options:\n"
    <<"object linkName\n"
    <<"object objectName front/back \n"
    <<"surfMap objectName surfName index-Direction \n"
    <<"  -- indexDir is +/- 1-N surfaces stored under surfName\n"
    <<"viewObject objectName front/back/N {1-4 designator} \n";
  return;
}


}  // NAMESPACE tallySystem
