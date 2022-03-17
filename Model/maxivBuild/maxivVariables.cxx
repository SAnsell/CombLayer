/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   maxivBuild/maxivVariables.cxx
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
#include "OutputLog.h"
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"

#include "maxivVariables.h"

namespace setVariable
{

void
MaxIVVariables(FuncDataBase& Control,
	       const std::string& magField,
	       const std::set<std::string>& beamNames)
  /*!
    Function to set the control variables and constants
    -- This version is for MAXIV 
    \param Control :: Function data base to add constants too
    \param beamName :: Set of beamline names
  */
{
  ELog::RegMethod RegA("setVariable","MaxIVVariables");

// -----------
// GLOBAL stuff
// -----------

  Control.addVariable("zero",0.0);     // Zero
  Control.addVariable("one",1.0);      // one

  maxivInstrumentVariables(beamNames,magField,Control);
  
  Control.addVariable("sdefEnergy",3000.0);
  // FINAL:
  Control.resetActive();
  return;
}

void
maxivInstrumentVariables(const std::set<std::string>& BL,
			 const std::string& defMagField,
			 FuncDataBase& Control)
  /*!
    Construct the variables for the beamlines if required
    for MaxIV
    \param BL :: Set for the beamlines
    \param defMagField :: Default arrangement for magnets
    \param Control :: Database for variables
   */
  
{
  ELog::RegMethod RegA("maxivVariables[F]",
                       "maxivInstrumentVariables");

  const std::set<std::string> magnetConfigs
    ({"TDCline","SPFline","TDClineA","TDClineB",
      "TDClineC","NONE","None"});
  
  const std::set<std::string> Linac
    ({"LINAC","SPF"});

  const std::set<std::string> R1Beam
    ({"R1RING","RING1","FLEXPES","MAXPEEM","SPECIES"});

  const std::set<std::string> R3Beam
    ({"R3RING","RING3","FORMAX","COSAXS", "SOFTIMAX",
	"DANMAX", "BALDER", "MICROMAX"});
  
  typedef void (*VariableFunction)(FuncDataBase&);
  typedef std::multimap<std::string,VariableFunction> VMap;
  
  const VMap VarInit({
     {"BALDER",    &BALDERvariables},
     {"COSAXS",    &COSAXSvariables},
     {"SOFTIMAX",  &SOFTIMAXvariables},
     {"DANMAX",    &DANMAXvariables},
     {"FLEXPES",   &FLEXPESvariables},
     {"MICROMAX",  &MICROMAXvariables},
     {"FORMAX",    &FORMAXvariables},
     {"MAXPEEM",   &MAXPEEMvariables},
     {"SPECIES",   &SPECIESvariables}
   });

  bool r1Flag(0);
  bool r3Flag(0);
  bool linacFlag(0);

  // Which magnetic configuration:
  if (!defMagField.empty() &&
      magnetConfigs.find(defMagField)==magnetConfigs.end())
    {
      throw ColErr::InContainerError<std::string>
	(defMagField,"Magnetic configurations");
    }
  const std::string magField
    (defMagField.empty() ? "TDCline" : defMagField);
  
	
  for(const std::string& beam : BL)
    {
      
      if (!r1Flag && (R1Beam.find(beam)!=R1Beam.end()))
	{
	  R1RingVariables(Control);
	  r1Flag=1;
	}

      if (!r3Flag && (R3Beam.find(beam)!=R3Beam.end()))
	{
	  R3RingVariables(Control);
	  r3Flag=1;
	}

      if (!linacFlag && (Linac.find(beam)!=Linac.end()))
	{
	  LINACvariables(Control);
	  LINACmagnetVariables(Control,magField);
	  linacFlag=1;
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
