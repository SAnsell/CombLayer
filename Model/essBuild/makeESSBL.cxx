/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/makeESSBL.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#include <utility>
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
#include "surfRegister.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "CopiedComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "FixedRotateUnit.h"
#include "FixedGroup.h"
#include "FixedRotateGroup.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "Bunker.h"
#include "GuideItem.h"

#include "BEER.h"
#include "BIFROST.h"
#include "CSPEC.h"
#include "DREAM.h"
#include "ESTIA.h"
#include "FREIA.h"
#include "HEIMDAL.h"
#include "LOKI.h"
#include "MAGIC.h"
#include "MIRACLES.h"
#include "NMX.h"
#include "NNBAR.h"
#include "ODIN.h"
#include "TESTBEAM.h"
#include "TREX.h"
#include "VESPA.h"
#include "VOR.h"
#include "SKADI.h"

#include "simpleITEM.h"

#include "beamlineConstructor.h"
#include "makeESSBL.h"

namespace essSystem
{

makeESSBL::makeESSBL(std::string  SN,
		     std::string  BName) : 
  beamlineSystem::beamlineConstructor(),
  shutterName(std::move(SN)),beamName(std::move(BName))
 /*!
    Constructor
    \param SN :: Shutter name
    \param BName :: Beamline
 */
{
}

makeESSBL::makeESSBL(const makeESSBL& A) :
  beamlineSystem::beamlineConstructor(A),
  shutterName(A.shutterName),beamName(A.beamName)
  /*!
    Copy constructor
    \param A :: makeESSBL to copy
  */
{}

makeESSBL&
makeESSBL::operator=(const makeESSBL& A)
  /*!
    Assignment operator
    \param A :: makeESSBL to copy
    \return *this
  */
{
  if (this!=&A)
    {
      beamlineSystem::beamlineConstructor::operator=(A);
    }
  return *this;
}


makeESSBL::~makeESSBL()
  /*!
    Destructor
  */
{}

std::pair<int,int>
makeESSBL::getBeamNum(const std::string& Name)
  /*!
    Process to determine the range of the beamline
    \param Name :: beamline name in form GxBLineyy
    \return pair of sector/Index
   */
{
  ELog::RegMethod RegA("makeESSBL","getBeamNum");
  
  if (Name.length()<11)
    throw ColErr::InvalidLine
      (Name,"Name not in form : GxBLineTopjj/GxBLineLowjj");
  
  std::pair<int,int> Out(0,0);
  std::string BN(Name);
  BN[0]=' ';
  BN.replace(2,8,"     ");
  if (!StrFunc::section(BN,Out.first) ||
      !StrFunc::section(BN,Out.second))
    throw ColErr::InvalidLine(Name,"Name processable in from : GxBLineyy");

  return Out;
}
  
void 
makeESSBL::build(Simulation& System,const Bunker& bunkerObj)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param bunkerObj :: Bunker cell system
   */
{
  // For output stream
  ELog::RegMethod RegA("makeESSBL","build");

  const int voidCell(74123);

  const attachSystem::FixedComp* mainFCPtr=
    System.getObject<attachSystem::FixedComp>(shutterName);
  const GuideItem* mainGIPtr=dynamic_cast<const GuideItem*>(mainFCPtr);
  if (!mainGIPtr)
    throw ColErr::InContainerError<std::string>(shutterName,"GuideItem");

  if (beamName=="BEER")
    {
      BEER beerBL("beer");
      // beer cuts (sometimes the main wheel void!!!!
      if (mainGIPtr->isActive("wheel"))
	{
	  beerBL.copyCutSurf("wheel",*mainGIPtr,"wheel");
	  ELog::EM<<"Bl added"<<ELog::endDiag;
	}
      beerBL.build(System,*mainGIPtr,bunkerObj,voidCell);      
    }  
  else if (beamName=="BIFROST")
    {
      BIFROST bifrostBL("bifrost");
      bifrostBL.build(System,*mainGIPtr,bunkerObj,voidCell);
    }  
  else if (beamName=="CSPEC")
    {
      CSPEC cspecBL("cspec");
      cspecBL.build(System,*mainGIPtr,bunkerObj,voidCell);
    }
  else if (beamName=="DREAM")
    {
      DREAM dreamBL("dream");
      dreamBL.build(System,*mainGIPtr,bunkerObj,voidCell);
    }
  else if (beamName=="ESTIA")
    {
      ESTIA estiaBL("estia");
      estiaBL.build(System,*mainGIPtr,bunkerObj,voidCell);
    }
  else if (beamName=="FREIA")
    {
      FREIA freiaBL("freia");
      freiaBL.build(System,*mainGIPtr,bunkerObj,voidCell);
    }  
  else if (beamName=="HEIMDAL")
    {
      // DREAM beamline
      HEIMDAL heimdalBL("heimdal");
      heimdalBL.build(System,*mainGIPtr,bunkerObj,voidCell);
    }
  else if (beamName=="LOKI")
    {
      // LOKI beamline
      LOKI lokiBL("loki");
      lokiBL.build(System,*mainGIPtr,bunkerObj,voidCell);
    }
  else if (beamName=="MAGIC")
    {
      MAGIC magicBL("magic");
      magicBL.build(System,*mainGIPtr,bunkerObj,voidCell);
    }  

  else if (beamName=="MIRACLES")
    {
      // NMX beamline
      MIRACLES miraclesBL("miracles");
      miraclesBL.build(System,*mainGIPtr,bunkerObj,voidCell);
    }
  else if (beamName=="NNBAR")
    {
      // DREAM beamline
      NNBAR nnbarBL("nnbar");
      nnbarBL.build(System,*mainGIPtr,bunkerObj,voidCell);
    }
  else if (beamName=="NMX")
    {
      // NMX beamline
      NMX nmxBL("nmx");
      nmxBL.build(System,*mainGIPtr,bunkerObj,voidCell);
    }
  else if (beamName=="ODIN")
    {
      // Odin beamline
      ODIN OdinBL("odin");
      OdinBL.build(System,*mainGIPtr,bunkerObj,voidCell);
    }
  else if (beamName=="TESTBEAM")
    {
      // TEST beamline
      TESTBEAM testBL("testBeam");
      testBL.build(System,*mainGIPtr,bunkerObj,voidCell);
    }
 else if (beamName=="TREX")
    {
      // TREX beamline
      TREX TrexBL("trex");
      TrexBL.build(System,*mainGIPtr,bunkerObj,voidCell);
    }
  else if (beamName=="SKADI")
    {
      //SKADI beamline
      SKADI SkadiBL("skadi");
      SkadiBL.build(System,*mainGIPtr,bunkerObj,voidCell);
    }
  else if (beamName=="VESPA")
    {
      // vespa beamline
      VESPA vespaBL("vespa");
      vespaBL.build(System,*mainGIPtr,bunkerObj,voidCell);
    }
  else if (beamName=="VOR")
    {
      VOR vorBL("vor");
      vorBL.build(System,*mainGIPtr,bunkerObj,voidCell);
    }
  else if (beamName=="SIMPLE")
    {
      // Simple beamline
      simpleITEM simpleBL("simple");
      simpleBL.build(System,*mainGIPtr,bunkerObj,voidCell);      
    }
  else
    {
      ELog::EM<<"NON-UNDERSTOOD BEAMLINE : "<<beamName<<ELog::endErr;
    }

  return;
}


}   // NAMESPACE essSystem


