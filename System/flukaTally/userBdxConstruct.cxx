/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flukaTally/userBdxConstruct.cxx
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

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "support.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "inputParam.h"

#include "SimFLUKA.h"
#include "flukaGenParticle.h"
#include "TallySelector.h"
#include "flukaTally.h"
#include "userBdx.h"
#include "userBdxConstruct.h" 


namespace flukaSystem
{


void 
userBdxConstruct::createTally(SimFLUKA& System,
			      const std::string& PType,
			      const int fortranTape,
			      const int cellA,const int cellB,
			      const bool eLog,const double Emin,
			      const double Emax,const size_t nE,
			      const bool aLog,const double Amin,
			      const double Amax,const size_t nA)
  /*!
    An amalgamation of values to determine what sort of mesh to put
    in the system.
    \param System :: SimFLUKA to add tallies
    \param PType :: particle name
    \param fortranTape :: output stream
    \param CellA :: initial region
    \param CellB :: secondary region
    \param eLog :: energy in log bins
    \param aLog :: angle in log bins
    \param Emin :: Min energy 
    \param Emax :: Max energy 
    \param Amin :: Min angle 
    \param Amax :: Max angle 
    \param nE :: Number of energy bins
    \param nA :: Number of angle bins
  */
{
  ELog::RegMethod RegA("userBdxConstruct","createTally");

  const flukaGenParticle& FG=flukaGenParticle::Instance();
    
  userBdx UD("surf",fortranTape,fortranTape);
  UD.setParticle(FG.nameToFLUKA(PType));

  UD.setCell(cellA,cellB);
  UD.setEnergy(eLog,Emin,Emax,nE);
  UD.setAngle(aLog,Amin,Amax,nA);
  
  System.addTally(UD);

  return;
}


void
userBdxConstruct::processBDX(SimFLUKA& System,
			     const mainSystem::inputParam& IParam,
			     const size_t Index) 
  /*!
    Add BDX tally (s) as needed
    - Input:
    -- particle FixedComp index
    -- particle cellA  cellB
    -- particle SurfMap name
    -- particle FixedComp:CellMapName FixedComp:CellMapName 

    \param System :: SimFLUKA to add tallies
    \param IParam :: Main input parameters
    \param Index :: index of the -T card
  */
{
  ELog::RegMethod RegA("userBdxConstruct","processBDX");

  System.createObjSurfMap();
  
  const std::string particleType=
    IParam.getValueError<std::string>("tally",Index,1,"tally:ParticleType");
  const std::string FCname=
    IParam.getValueError<std::string>("tally",Index,2,"tally:Object/Cell");
  const std::string FCindex=
    IParam.getValueError<std::string>("tally",Index,3,"tally:linkPt/Cell");

  size_t itemIndex(4);
  int cellA(0);
  int cellB(0);
  if (
      (!StrFunc::convert(FCname,cellA) ||
       !StrFunc::convert(FCindex,cellB) ||
       !checkLinkCells(System,cellA,cellB) ) &&
      !constructCellMapPair(System,FCname,FCindex,cellA,cellB) &&
      !constructLinkRegion(System,FCname,FCindex,cellA,cellB)
      )
    {
      throw ColErr::CommandError(FCname+" "+FCindex,"Surf Tally conversion");
    }
  ELog::EM<<"Regions connected from "<<cellA<<" to "<<cellB<<ELog::endDiag;  

  // This needs to be more sophisticated
  const int nextId=System.getNextFTape();
  const double EA=IParam.getDefValue<double>(1e-9,"tally",Index,itemIndex++);
  const double EB=IParam.getDefValue<double>(1000.0,"tally",Index,itemIndex++);
  const size_t NE=IParam.getDefValue<size_t>(200,"tally",Index,itemIndex++); 

  const double AA=IParam.getDefValue<double>(0.0,"tally",Index,itemIndex++);
  const double AB=IParam.getDefValue<double>(2*M_PI,"tally",Index,itemIndex++);
  const size_t NA=IParam.getDefValue<size_t>(1,"tally",Index,itemIndex++); 

  
  userBdxConstruct::createTally(System,particleType,-nextId,
				cellA,cellB,
				1,EA,EB,NE,
				0,AA,AB,NA);
  
  return;      
}  
  
void
userBdxConstruct::writeHelp(std::ostream& OX) 
  /*!
    Write out help
    \param OX :: Output stream
  */
{
  OX<<
    "-T surface \n"
    "  particleType : name of particle / energy to tally\n"
    "  Option A : \n"
    "       FixedComp + linkPoint (using % to mean -ve)\n"
    "  Option B : \n"
    "       FixedComp:CellMap:Index (index optional) \n"
    "  Emin EMax NE \n"
    "  AMin AngleMax NA (all optional) \n"
    <<std::endl;
  return;
}

}  // NAMESPACE flukaSystem
