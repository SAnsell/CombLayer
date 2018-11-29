/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flukaTally/resnucConstruct.cxx
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
#include "surfRegister.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h"
#include "LinkSupport.h"
#include "inputParam.h"

#include "Object.h"
#include "SimFLUKA.h"
#include "particleConv.h"
#include "flukaGenParticle.h"
#include "TallySelector.h"
#include "flukaTally.h"
#include "userTrack.h"
#include "resnucConstruct.h" 


namespace flukaSystem
{

void 
resnucConstruct::createTally(SimFLUKA& System,
				const std::string& PType,const int fortranTape,
				const int cellA,
				const bool eLog,const double Emin,
				const double Emax,const size_t nE)
/*!
    An amalgamation of values to determine what sort of mesh to put
    in the system.
    \param System :: SimFLUKA to add tallies
    \param fortranTape :: output stream
    \param CellA :: initial region
    \param eLog :: energy in log bins
    \param aLog :: angle in log bins
    \param Emin :: Min energy 
    \param Emax :: Max energy 
  */
{
  ELog::RegMethod RegA("resnucConstruct","createTally");

  const flukaGenParticle& FG=flukaGenParticle::Instance();
    
  userTrack UD(fortranTape);
  UD.setParticle(FG.nameToFLUKA(PType));

  UD.setCell(cellA);
  UD.setEnergy(eLog,Emin,Emax,nE);
  
  System.addTally(UD);

  return;
}


void
resnucConstruct::processTrack(SimFLUKA& System,
			     const mainSystem::inputParam& IParam,
			     const size_t Index) 
  /*!
    Add TRACK tally (s) as needed
    - Input:
    -- particle FixedComp index
    -- particle cellA  
    -- particle SurfMap name
    \param System :: SimFLUKA to add tallies
    \param IParam :: Main input parameters
    \param Index :: index of the -T card
  */
{
  ELog::RegMethod RegA("resnucConstruct","processTrack");

  
  const std::string particleType=
    IParam.getValueError<std::string>("tally",Index,1,"tally:ParticleType");
  const std::string FCname=
    IParam.getValueError<std::string>("tally",Index,2,"tally:Object/Cell");
  const std::string FCindex=
    IParam.getValueError<std::string>("tally",Index,3,"tally:linkPt/Cell");

  size_t itemIndex(4);
  int cellA(0);
  if (!StrFunc::convert(FCname,cellA))
    {
      throw ColErr::InContainerError<std::string>
	(FCname+":"+FCindex,"No regions");
    }
  
  ELog::EM<<"Regions connected from "<<cellA<<ELog::endDiag;  

  // This needs to be more sophisticated
  const int nextId=System.getNextFTape();
  
  const double EA=IParam.getDefValue<double>(1e-9,"tally",Index,itemIndex++);
  const double EB=IParam.getDefValue<double>(1000,"tally",Index,itemIndex++);
  const size_t NE=IParam.getDefValue<size_t>(200,"tally",Index,itemIndex++); 
  
  resnucConstruct::createTally(System,particleType,nextId,
				  cellA,1,EA,EB,NE);
  
  return;      
}  
  
void
resnucConstruct::writeHelp(std::ostream& OX) 
  /*!
    Write out help
    \param OX :: Output stream
  */
{
  OX<<
    "recordType filename \n"
    "  --recordType avaiable:\n"
    "    source : trajectory : local : continuous\n"
    "    sourceLoss : trajLoss : user";

  return;
}

}  // NAMESPACE flukaSystem
