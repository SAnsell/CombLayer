/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   source/SourceSelector.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include <memory>
#include <array>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "doubleErr.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "Source.h"
#include "SrcItem.h"
#include "SrcData.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "LinkSupport.h"
#include "inputParam.h"
#include "Simulation.h"
#include "inputSupport.h"
#include "SourceCreate.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "objectRegister.h"
#include "particleConv.h"
#include "SourceBase.h"
#include "World.h"
#include "flukaSourceSelector.h"

namespace SDef
{
  
void 
flukaSourceSelection(Simulation& System,
		     const mainSystem::inputParam& IParam)
  /*!
    Build the source based on the input parameter table
    - sdefObj Object linkPt Distance [along y (or vector)]
    \param System :: Simulation to use
    \param IParam :: Input parameter
  */
{
  ELog::RegMethod RegA("SourceSelector[F]","flukaSourceSelection");
  
  const ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  const mainSystem::MITYPE inputMap=IParam.getMapItems("sdefMod");
  
  const std::string DObj=IParam.getDefValue<std::string>("","sdefObj",0);
  const std::string DSnd=IParam.getDefValue<std::string>("","sdefObj",1);
  const std::string Dist=IParam.getDefValue<std::string>("","sdefObj",2);

  double D;
  Geometry::Vec3D DOffsetStep(0,0,0);
  if (!StrFunc::convert(Dist,DOffsetStep) && 
      StrFunc::convert(Dist,D))
    DOffsetStep[1]=D;
  
  const attachSystem::FixedComp& FC=
    (DObj.empty()) ?  World::masterOrigin() :
    *(OR.getObjectThrow<attachSystem::FixedComp>(DObj,"Object not found"));

  const long int linkIndex=(DSnd.empty()) ?  0 :
    attachSystem::getLinkIndex(DSnd) % 1000;

  // NOTE: No return to allow active SSW systems
  const size_t NSDef(IParam.setCnt("sdefType"));
  ELog::EM<<"NSDEF == "<<NSDef<<ELog::endDiag;
  std::string sName;
  std::string eName;
  for(size_t sdefIndex=0;sdefIndex<NSDef;sdefIndex++)
    {
      const std::string sdefType=IParam.getValue<std::string>
	("sdefType",sdefIndex,0);

      ELog::EM<<"SDEF TYPE ["<<sdefIndex<<"] == "<<sdefType<<ELog::endDiag;
      
      if (sdefType=="Wiggler")                       // blader wiggler
	sName=SDef::createWigglerSource(inputMap,FC,linkIndex);
      
      else if (sdefType=="Beam" || sdefType=="beam")
	sName=SDef::createBeamSource(inputMap,"beamSource",FC,linkIndex);
      
      else if (sdefType=="external" || sdefType=="External" ||
	       sdefType=="source" || sdefType=="Source")
	eName=SDef::createFlukaSource(inputMap,"flukaSource",FC,linkIndex);
      
      else
	{
	  ELog::EM<<"sdefType :\n"
	    "Beam :: Test Beam [Radial] source \n"
	    "Wiggler :: Wiggler Source for balder \n"
	    "External :: External source from source.f \n"
		  <<ELog::endBasic;
	}
    }
  ELog::EM<<"Source name == "<<sName<<ELog::endDiag;

  if (!IParam.flag("sdefVoid") && !sName.empty())
    System.setSourceName(sName);
  if (!eName.empty())
    System.setExtraSourceName(eName);
  
  return;
}
  
  

  
} // NAMESPACE SDef
