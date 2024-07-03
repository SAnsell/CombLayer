/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   tally/TallyBuilder.cxx
 *
 * Copyright (c) 2004-2024 by Stuart Ansell / Konstantin Batkov
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "inputParam.h"

#include "gridConstruct.h"
#include "pointConstruct.h"
#include "meshConstruct.h"
#include "tmeshConstruct.h"
#include "fmeshConstruct.h"
#include "fluxConstruct.h"
#include "heatConstruct.h"
#include "fissionConstruct.h"
#include "itemConstruct.h"
#include "surfaceConstruct.h"
#include "sswConstruct.h"

#include "TallyBuilder.h"

namespace tallySystem
{

void
tallySelection(SimMCNP& System,
	       const mainSystem::inputParam& IParam)
  /*!
    An amalgumation of values to determine what sort of tallies to put
    in the system.
    \param System :: SimMCNP to add tallies
    \param IP :: InputParam
  */
{
  ELog::RegMethod RegA("TallyBuilder","tallySelection(basic)");

  const size_t ntallies = IParam.setCnt("tally");
  for(size_t i=0;i<ntallies;i++)
    {
      const size_t NItems=IParam.itemCnt("tally",i);
      if (NItems==0)
	ELog::EM << "-T needs arguments. Call '-T help' for help." << ELog::endErr;

      const std::string tallyName=IParam.getValue<std::string>("tally",i,0);
      const std::string TType = NItems > 1 ?  IParam.getValue<std::string>("tally",i,1) : "";
      const std::string HType=(NItems==1) ? tallyName : TType;

      if (tallyName=="help")
	helpTallyType(HType);
      else if (TType=="grid")
	gridConstruct::processGrid(System,IParam,i);
      else if (TType=="point")
	pointConstruct::processPoint(System,IParam,i);
      else if (TType=="tmesh")
	tmeshConstruct::processMesh(System,IParam,i);
      else if (TType=="fmesh")
	fmeshConstruct::processMesh(System,IParam,i);
      else if (TType=="flux")
	fluxConstruct::processFlux(System,IParam,i);
      else if (TType=="fission")
	fissionConstruct::processPower(System,IParam,i);
      else if (TType=="heat")
	heatConstruct::processHeat(System,IParam,i);
      else if (TType=="item")
	itemConstruct::processItem(System,IParam,i);
      else if (TType=="surface" || TType=="surfCurrent")
	surfaceConstruct::processSurfaceCurrent(System,IParam,i);
      else if (TType=="surfFlux" || TType=="surfaceFlux")
	surfaceConstruct::processSurfaceFlux(System,IParam,i);
      else if (TType=="SSW" || TType=="ssw")
	sswConstruct::processSSW(System,IParam,i);
      else
	ELog::EM<<"Unable to understand tally type: "<<TType<<
	  ". Call '-T help' for help." << ELog::endErr;
    }

  //if (IParam.flag("Txml"))
    //   tallySystem::addXMLtally(System,IParam.getValue<std::string>("Txml"));

  return;
}

void
helpTallyType(const std::string& HType)
  /*!
    Simple help for types
    \param HType :: specialization if present that help is required for
  */
{
  ELog::RegMethod("TallyConstructor","helpTallyType");

  if (HType=="grid")
    gridConstruct::writeHelp(ELog::EM.Estream());
  else if (HType=="tmesh")
    tmeshConstruct::writeHelp(ELog::EM.Estream());
  else if (HType=="fmesh")
    fmeshConstruct::writeHelp(ELog::EM.Estream());
  else if (HType=="point")
    pointConstruct::writeHelp(ELog::EM.Estream());
  else if (HType=="surface")
    surfaceConstruct::writeHelp(ELog::EM.Estream());
  else if (HType=="flux")
    fluxConstruct::writeHelp(ELog::EM.Estream());
  else if (HType=="heat")
    heatConstruct::writeHelp(ELog::EM.Estream());
  else if (HType=="fission")
    fissionConstruct::writeHelp(ELog::EM.Estream());
  else if (HType=="ssw")
    sswConstruct::writeHelp(ELog::EM.Estream());
  else
    {
      ELog::EM<<"The '-T' flag adds a tally into the model." << ELog::endBasic;
      ELog::EM<<"Usage: -T tallyName tallyType parameters" << ELog::endBasic;
      ELog::EM<<"       run: '-T help tallyType' to display the type-based help\n" << ELog::endBasic;
      ELog::EM<<"Tally Types:\n\n";
      ELog::EM<<"-- grid : \n";
      ELog::EM<<"-- mesh (tmesh): \n";
      ELog::EM<<"-- point (f5): \n";
      ELog::EM<<"-- surface (f1): \n";
      ELog::EM<<"-- flux (f4): \n";
      ELog::EM<<"-- heat (f6): \n";
      ELog::EM<<"-- fission : \n";
      ELog::EM<<"-- SSW : \n";
    }
  ELog::EM<<ELog::endBasic;

  return;
}


} // NAMESPACE tallySystem
