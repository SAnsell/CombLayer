/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flukaTally/userTrackConstruct.cxx
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
#include "Vec3D.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "inputParam.h"

#include "SimFLUKA.h"
#include "flukaTally.h"
#include "userTrack.h"
#include "userTrackConstruct.h" 


namespace flukaSystem
{

void 
userTrackConstruct::createTally(SimFLUKA& System,
				const std::string& PType,const int fortranTape,
				const int cellA,
				const bool eLog,const double Emin,
				const double Emax,const size_t nE)
/*!
    An amalgamation of values to determine what sort of mesh to put
    in the system.
    \param System :: SimFLUKA to add tallies
    \param PType :: Particle type
    \param fortranTape :: output stream
    \param cellA :: initial region
    \param eLog :: energy in log bins
    \param aLog :: angle in log bins
    \param Emin :: Min energy 
    \param Emax :: Max energy 
  */
{
  ELog::RegMethod RegA("userTrackConstruct","createTally");

    
  userTrack UD(fortranTape,fortranTape);
  UD.setParticle(PType);

  UD.setCell(cellA);
  UD.setEnergy(eLog,Emin,Emax,nE);
  
  System.addTally(UD);

  return;
}


void
userTrackConstruct::processTrack(SimFLUKA& System,
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
  ELog::RegMethod RegA("userTrackConstruct","processTrack");
  
  const std::string particleType=
    IParam.getValueError<std::string>("tally",Index,1,"tally:ParticleType");

  
  const std::string FCname=
    IParam.getValueError<std::string>("tally",Index,2,"tally:Object/Cell");

  // throws on error
  const std::vector<int> cellList=System.getObjectRange(FCname);
  
  const double EA=IParam.getDefValue<double>(1e-9,"tally",Index,3);
  const double EB=IParam.getDefValue<double>(1000,"tally",Index,4);
  const size_t NE=IParam.getDefValue<size_t>(200,"tally",Index,5); 


  // This needs to be more sophisticated

  for(const int cellA : cellList)
    {
      const int nextId=System.getNextFTape();
      userTrackConstruct::createTally(System,particleType,-nextId,
				    cellA,1,EA,EB,NE);
      
    }  
  return;      
}  
  
void
userTrackConstruct::writeHelp(std::ostream& OX) 
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
