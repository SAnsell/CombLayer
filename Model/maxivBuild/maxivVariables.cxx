/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   maxivBuild/maxivVariables.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNMAXIV FOR A PARTICULAR PURPOSE.  See the
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
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "variableSetup.h"

#include "maxivVariables.h"

namespace setVariable
{

void
MaxIVVariables(FuncDataBase& Control,
	       const std::set<std::string>& beamNames)
  /*!
    Function to set the control variables and constants
    -- This version is for MAXIV 
    \param Control :: Function data base to add constants too
    \param beamName :: Set of beamline names
  */
{
  ELog::RegMethod RegA("setVariable","maxivVariables");

// -----------
// GLOBAL stuff
// -----------

  Control.addVariable("zero",0.0);     // Zero
  Control.addVariable("one",1.0);      // one

  maxivInstrumentVariables(beamNames,Control);
  
  Control.addVariable("sdefEnergy",3000.0);
  // FINAL:
  Control.resetActive();
  return;
}

void
maxivInstrumentVariables(const std::set<std::string>& BL,
			 FuncDataBase& Control)
  /*!
    Construct the variables for the beamlines if required
    for MaxIV
    \param BL :: Set for the beamlines
    \param Control :: Database for variables
   */
{
  ELog::RegMethod RegA("maxivVariables[F]",
                       "maxivInstrumentVariables");

  const std::set<std::string> R1Beam(
     {"RING1","MAXPEEM","SPECIES"});

  
  typedef void (*VariableFunction)(FuncDataBase&);
  typedef std::multimap<std::string,VariableFunction> VMap;
  
  const VMap VarInit({
     {"BALDER",    &BALDERvariables},
     {"COSAXS",    &COSAXSvariables},
     {"FORMAX",    &FORMAXvariables},
     {"MAXPEEM",   &MAXPEEMvariables},
     {"SPECIES",   &SPECIESvariables}
   });

  bool r1Flag(0);
  for(const std::string& beam : BL)
    {
      
      if (!r1Flag && (R1Beam.find(beam)!=R1Beam.end()))
	{
	  R1RingVariables(Control);
	  r1Flag=1;
	}
	  
      // std::pair<VMap::const_iterator,VMap::const_iterator>
      VMap::const_iterator mc;
      decltype(VarInit.equal_range("")) rangePair
	= VarInit.equal_range(beam);
      for(mc=rangePair.first;mc!=rangePair.second;mc++)
	{
	  mc->second(Control);
	}
    }
  return;
}  
  
}  // NAMESPACE setVariable
