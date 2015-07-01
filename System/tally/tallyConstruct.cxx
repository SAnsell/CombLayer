/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tally/tallyConstruct.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include "objectRegister.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "Simulation.h"
#include "inputParam.h"
#include "MeshGrid.h"
#include "NRange.h"
#include "pairRange.h"
#include "Tally.h"
#include "TallyCreate.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "meshTally.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"

#include "TallySelector.h" 
#include "basicConstruct.h" 
#include "pointConstruct.h"
#include "gridConstruct.h" 
#include "meshConstruct.h" 
#include "fluxConstruct.h" 
#include "fissionConstruct.h" 
#include "heatConstruct.h" 
#include "itemConstruct.h" 
#include "surfaceConstruct.h" 
#include "tallyConstructFactory.h" 
#include "tallyConstruct.h" 


namespace tallySystem
{

// static definitions:
  std::map<std::string,int> tallyConstruct::chipGridPos;
  
void
tallyConstruct::initStatic()
/*!
  Initialize the static members
*/
{
  typedef std::map<std::string,int> MTYPE;
  chipGridPos.insert(MTYPE::value_type("chipTable1",0));
  chipGridPos.insert(MTYPE::value_type("chipTable2",1));
  chipGridPos.insert(MTYPE::value_type("chipPreCol",2));
  chipGridPos.insert(MTYPE::value_type("chipVCol",3));
  chipGridPos.insert(MTYPE::value_type("chipHCol",4));
  chipGridPos.insert(MTYPE::value_type("chipSeparator",5));
  
  return;
}
  
tallyConstruct::tallyConstruct(const tallyConstructFactory& FC) : 
  basicConstruct(),pointPtr(FC.makePoint()),gridPtr(FC.makeGrid()),
  meshPtr(FC.makeMesh()),fluxPtr(FC.makeFlux()),
  heatPtr(FC.makeHeat()),itemPtr(FC.makeItem()),
  surfPtr(FC.makeSurf()),fissionPtr(FC.makeFission())
  /*!
    Constructor
    \param FC :: Factory object to specialize constructors
  */
{
  initStatic();
}
  
  
tallyConstruct::~tallyConstruct()
/*!
  Delete operator
*/
{
  delete pointPtr;
  delete gridPtr;
  delete meshPtr;
  delete fluxPtr;
  delete heatPtr;
  delete itemPtr;
  delete surfPtr;
  delete fissionPtr;
}
  
void
tallyConstruct::setPoint(pointConstruct* PPtr) 
/*!
  Modify/assign the pointConstructor 
  \param PPtr :: New point Ptr [MANGAGED]
*/
{
  if (PPtr)
    {
      delete pointPtr;
      pointPtr=PPtr;
    }
  return;
}
  
void
tallyConstruct::setFission(fissionConstruct* PPtr) 
/*!
  Modify/assign the pointConstructor 
  \param PPtr :: New point Ptr [MANGAGED]
*/
{
  if (PPtr)
    {
      delete fissionPtr;
      fissionPtr=PPtr;
    }
  return;
}
  
int
tallyConstruct::tallySelection(Simulation& System,
			       const mainSystem::inputParam& IParam) const
/*!
  An amalgumation of values to determine what sort of tallies to put
  in the system.
  \param System :: Simulation to add tallies
  \param IParam :: Main input parameters
  \return flag to indicate that more work is required after renumbering
*/
{
  ELog::RegMethod RegA("tallyConstruct","tallySelection");


  int workFlag(0);  
  for(size_t i=0;i<IParam.setCnt("tally");i++)
    {
      const std::string TType=
	IParam.getValue<std::string>("tally",i,0);

      const size_t NItems=IParam.itemCnt("tally",i);
      const std::string HType=(NItems>1) ?
	IParam.getValue<std::string>("tally",i,1) : "help";
      
      if (TType=="help" || TType=="?")
	helpTallyType(HType);
      else if (HType=="help" || HType=="?")
	helpTallyType(TType);
      else if (TType=="grid") 
	gridPtr->processGrid(System,IParam,i);
      else if (TType=="point")
	pointPtr->processPoint(System,IParam,i);
      else if (TType=="mesh")
	meshPtr->processMesh(System,IParam,i);
      else if (TType=="flux")
	workFlag+=fluxPtr->processFlux(System,IParam,i,0);
      else if (TType=="fission")
	workFlag+=fissionPtr->processPower(System,IParam,i,0);
      else if (TType=="heat")
	heatPtr->processHeat(System,IParam,i);
      else if (TType=="item")
	itemPtr->processItem(System,IParam,i);
      else if (TType=="surface")
	workFlag+=surfPtr->processSurface(System,IParam,i);
      else
	ELog::EM<<"Unable to understand tally type :"<<TType<<ELog::endErr;
    }
  if (IParam.flag("Txml"))
    tallySystem::addXMLtally(System,IParam.getValue<std::string>("Txml"));
      
  return workFlag;
}

int
tallyConstruct::tallyRenumber(Simulation& System,
			      const mainSystem::inputParam& IParam) const
  /*!
    An amalgumation of values to determine what sort of tallies to put
    in the system.
    \param System :: Simulation to add tallies
    \param IParam :: Main input parameters
    \return flag to indicate that more work is required after renumbering
  */
{
  ELog::RegMethod RegA("tallyConstruct","tallySelection");

  int workFlag(0);  
  for(size_t i=0;i<IParam.setCnt("tally");i++)
    {
      const std::string TType=
	IParam.getValue<std::string>("tally",i,0);

      if (TType=="flux")
	fluxPtr->processFlux(System,IParam,i,1);
      else if (TType=="heat")
	heatPtr->processHeat(System,IParam,i);
    }

  // if (IParam.flag("Txml"))
  //   tallySystem::addXMLtally(System,IParam.getValue<std::string>("Txml"));
      
  return workFlag;
}

void  
tallyConstruct::helpTallyType(const std::string& HType) const
  /*!
    Simple help for types
    \param HType :: specialization if present that help is required for
  */
{
  ELog::RegMethod("TallyConstructor","helpTallyType");

  if (HType=="grid")
    gridPtr->writeHelp(ELog::EM.Estream());
  else if (HType=="heat")
    heatPtr->writeHelp(ELog::EM.Estream());
  else if (HType=="point")
    pointPtr->writeHelp(ELog::EM.Estream());
  else if (HType=="mesh")
    meshPtr->writeHelp(ELog::EM.Estream());
  else if (HType=="surface")
    surfPtr->writeHelp(ELog::EM.Estream());
  else if (HType=="flux")
    fluxPtr->writeHelp(ELog::EM.Estream());
  else
    {
      ELog::EM<<"Tally Types:\n\n";
      ELog::EM<<"-- grid : \n";
      ELog::EM<<"-- mesh : \n";
      ELog::EM<<"-- point : \n";
      ELog::EM<<"-- surface : \n";
      ELog::EM<<"-- flux : \n";
      ELog::EM<<"-- heat : \n";
    }
  
  ELog::EM<<ELog::endBasic;
  return;
}

}  // NAMESPACE tallySystem
