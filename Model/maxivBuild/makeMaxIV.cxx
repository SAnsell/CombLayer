/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   maxivBuild/makeMaxIV.cxx
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
 * MERCHANTABILITY or FITNMaxIV FOR A PARTICULAR PURPOSE.  See the
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
#include <array>
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
#include "FixedOffset.h"
#include "FixedGroup.h"
#include "ContainedComp.h"
#include "SpaceCut.h"
#include "ContainedSpace.h"
#include "ContainedGroup.h"
#include "FrontBackCut.h"
#include "CopiedComp.h"
#include "LayerComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "World.h"
#include "AttachSupport.h"
#include "LinkSupport.h"

#include "BALDER.h"
#include "COSAXS.h"

#include "makeMaxIV.h"

namespace xraySystem
{

makeMaxIV::makeMaxIV() 
 /*!
    Constructor
 */
{
  // Require registration of THIS world
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  std::shared_ptr<attachSystem::FixedComp> worldPtr=
    std::make_shared<attachSystem::FixedComp>(World::masterOrigin());
  OR.addObject(worldPtr);
}


makeMaxIV::~makeMaxIV()
  /*!
    Destructor
  */
{}

void
makeMaxIV::makeBeamLine(Simulation& System,
		      const mainSystem::inputParam& IParam)
  /*!
    Build a beamline based on LineType
     -- to construct a beamline the name of the guide Item 
     and the beamline typename is required
    \param System :: Simulation 
    \param IParam :: Input paramters
  */
{
  ELog::RegMethod RegA("makeMaxIV","makeBeamLine");

  const ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  const size_t NSet=IParam.setCnt("beamlines");
  for(size_t j=0;j<NSet;j++)
    {
      const size_t NItems=IParam.itemCnt("beamlines",j);
      size_t index=0;
      while(index<NItems)  // min of one name
	{
	  const std::string BL=
	    IParam.getValue<std::string>("beamlines",j,index);
	  const std::string FCName=
	    IParam.getValue<std::string>("beamlines",j,index+1);
	  const long int linkIndex=
	    IParam.getValue<long int>("beamlines",j,index+2);
	  index+=3;

	  // ORIGIN current not done as no ring:
          const attachSystem::FixedComp* FCOrigin=
	    OR.getObjectThrow<attachSystem::FixedComp>
	    (FCName,"FixedComp not found for origin");

	  if (BL=="BALDER")
	    {
	      BALDER BL("Balder");
	      BL.build(System,*FCOrigin,linkIndex);
	    }
	  else if (BL=="COSAXS")
	    {
	      COSAXS BL("Cosaxs");
	      BL.build(System,*FCOrigin,linkIndex);
	    }
	}
    }

  return;
}

void 
makeMaxIV::build(Simulation& System,
	       const mainSystem::inputParam& IParam)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param IParam :: Input parameters
   */
{
  // For output stream
  ELog::RegMethod RegA("makeMaxIV","build");

  const FuncDataBase& Control=System.getDataBase();
  int voidCell(74123);


  makeBeamLine(System,IParam);

  ELog::EM<<"=Finished beamlines="<<ELog::endDiag;
  return;
}


}   // NAMESPACE essSystem

