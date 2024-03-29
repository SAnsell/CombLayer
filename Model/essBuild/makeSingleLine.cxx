/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/makeSingleLine.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "inputParam.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "CopiedComp.h"
#include "ContainedComp.h"
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

#include "BEER.h"
#include "BIFROST.h"
#include "FREIA.h"
#include "HEIMDAL.h"
#include "LOKI.h"
#include "MAGIC.h"
#include "MIRACLES.h"
#include "TESTBEAM.h"
#include "VESPA.h"

#include "makeSingleLine.h"

namespace essSystem
{

makeSingleLine::makeSingleLine() 
 /*!
    Constructor
 */
{
}

makeSingleLine::makeSingleLine(const makeSingleLine& A) : 
  beamName(A.beamName)
  /*!
    Copy constructor
    \param A :: makeSingleLine to copy
  */
{}

makeSingleLine&
makeSingleLine::operator=(const makeSingleLine& A)
  /*!
    Assignment operator
    \param A :: makeSingleLine to copy
    \return *this
  */
{
  if (this!=&A)
    {
      beamName=A.beamName;
    }
  return *this;
}


makeSingleLine::~makeSingleLine()
  /*!
    Destructor
  */
{}


  
void 
makeSingleLine::build(Simulation& System,
		      const mainSystem::inputParam& IParam)
  /*!
    Carry out the build of a single beamline
    \param System :: Simulation system
    \param IParam :: name of beamline
   */
{
  // For output stream
  ELog::RegMethod RegA("makeSingleLine","build");

  const int voidCell(74123);
  beamName=IParam.getValueError<std::string>
    ("beamlines",0,0,"Single beamline not defined");
  // GXBLineTop7 for example
  if (beamName.find("BLine")!=std::string::npos) 
    beamName=IParam.getValueError<std::string>
      ("beamlines",0,1,"Single beamline not defined");
    
  if (beamName=="BEER")
    {
      BEER beerBL("beer");
      beerBL.buildIsolated(System,voidCell);
    }
  else if (beamName=="FREIA")
    {
      FREIA freiaBL("freia");
      freiaBL.buildIsolated(System,voidCell);
    }
  else if (beamName=="HEIMDAL")
    {
      HEIMDAL heimdalBL("heimdal");
      heimdalBL.buildIsolated(System,voidCell);
    }
  else if (beamName=="LOKI")
    {
      LOKI lokiBL("loki");
      lokiBL.buildIsolated(System,voidCell);      
    }
  else if (beamName=="MAGIC")
    {
      MAGIC magicBL("magic");
      magicBL.buildIsolated(System,voidCell);      
    }
  else if (beamName=="MIRACLES")
    {
      MIRACLES miraclesBL("miracles");
      miraclesBL.buildIsolated(System,voidCell);      
    }
  else if (beamName=="VESPA")
    {
      VESPA vespaBL("vespa");
      vespaBL.buildIsolated(System,voidCell);
    }
  else if (beamName=="TEST" || beamName=="TESTBEAM")
    {
      TESTBEAM testBL("testBeam");
      testBL.buildIsolated(System,voidCell);
      
    }
  else
    {
      ELog::EM<<"BEAMLINE does not support isolated build yet : "
	      <<beamName<<ELog::endErr;
    }
  return;
}


}   // NAMESPACE essSystem


