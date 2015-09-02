/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/makeESSBL.cxx
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
#include "stringCombine.h"
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
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "CellMap.h"
#include "LayerComp.h"
#include "FixedGroup.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "ShapeUnit.h"
#include "Bunker.h"
#include "GuideLine.h"
#include "GuideItem.h"

#include "ODIN.h"
#include "LOKI.h"
#include "NMX.h"
#include "VOR.h"
#include "beamlineConstructor.h"
#include "makeESSBL.h"

namespace essSystem
{

makeESSBL::makeESSBL(const std::string& SN,
		     const std::string& BName) : 
  beamlineSystem::beamlineConstructor(),
  shutterName(SN),beamName(BName)
 /*!
    Constructor
    \param SN :: Shutter name
    \param BName :: Beamline
 */
{
}

makeESSBL::makeESSBL(const makeESSBL& A) : 
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
  
  if (Name.length()<8)
    throw ColErr::InvalidLine(Name,"Name not in from : GxBLineyy");
  std::pair<int,int> Out(0,0);
  std::string BN(Name);
  BN[0]=' ';
  BN.replace(2,5,"     ");
  if (!StrFunc::section(BN,Out.first) ||
      !StrFunc::section(BN,Out.second))
    {

      throw ColErr::InvalidLine(Name,"Name processable in from : GxBLineyy");
    }
  return Out;
}
  
  
void 
makeESSBL::build(Simulation& System,
		 const Bunker& bunkerObj)
  /*!
    Carry out the full build
    \param SimPtr :: Simulation system
    \param bunkerObj :: Bunker cell system
   */
{
  // For output stream
  ELog::RegMethod RegA("makeESSBL","build");

  const int voidCell(74123);
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  const attachSystem::FixedComp* mainFCPtr=
    OR.getObject<attachSystem::FixedComp>(shutterName);
  const GuideItem* mainGIPtr=
    dynamic_cast<const GuideItem*>(mainFCPtr);
    
  if (!mainGIPtr)
    throw ColErr::InContainerError<std::string>(shutterName,"shutterObject");

  if (beamName=="ODIN")
    {
      // Odin beamline
      ODIN OdinBL;
      OdinBL.build(System,*mainGIPtr,bunkerObj,voidCell);
    }
  else if (beamName=="LOKI")
    {
      // LOKI beamline
      LOKI LokiBL;
      LokiBL.build(System,*mainGIPtr,bunkerObj,voidCell);
    }
  else if (beamName=="NMX")
    {
      // NMX beamline
      ELog::EM<<"Building NMW"<<ELog::endDiag;
      NMX nmxBL;
      nmxBL.build(System,*mainGIPtr,bunkerObj,voidCell);
    }
  else if (beamName=="VOR")
    {
      // NMX beamline
      ELog::EM<<"Building VOR"<<ELog::endDiag;
      VOR vorBL;
      vorBL.build(System,*mainGIPtr,bunkerObj,voidCell);
    }
  else if (beamName=="JSANS" || beamName=="JRef")
    {
      ///< Guide line [refl]
      std::shared_ptr<beamlineSystem::GuideLine>
	RefA(new beamlineSystem::GuideLine(beamName));
      OR.addObject(RefA);
      RefA->addInsertCell(voidCell);
      RefA->createAll(System,*mainFCPtr,2,*mainFCPtr,2);
    }

  return;
}


}   // NAMESPACE essSystem


