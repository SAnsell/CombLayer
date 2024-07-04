/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   flukaTally/resnucConstruct.cxx
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
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "inputParam.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "MXcards.h"
#include "Zaid.h"
#include "Material.h"

#include "SimFLUKA.h"
#include "flukaTally.h"

#include "resnuclei.h"
#include "flukaTallyModification.h"
#include "resnucConstruct.h"


namespace flukaSystem
{

void
resnucConstruct::createTally(SimFLUKA& System,
			     const std::string& name,
			     const int ID,
			     const int fortranTape,
			     const int cellA)

/*!
    An amalgamation of values to determine what sort of mesh to put
    in the system.

    Note: There is a bug in fluka -- this needs each and
    every RESNUC card to have the same ZMax/AMax. So we
    update each to the groups max/min and then if we
    have a new on update all.

    \param System :: SimFLUKA to add tallies
    \param name :: estimator name
    \param ID :: estimator ID number - name postfix
                (since there might be several estimators with the same name for each cell)
    \param fortranTape :: output stream
    \param CellA :: initial region
  */
{
  ELog::RegMethod RegA("resnucConstruct","createTally");

  const MonteCarlo::Object* OPtr=
    System.findObjectThrow(cellA,"cellA");

  const std::vector<MonteCarlo::Zaid> ZVec=
    OPtr->getMatPtr()->getZaidVec();
  int ZMax(0);
  int AMax(0);
  for(const MonteCarlo::Zaid& zaid : ZVec)
    {
      const int Z=static_cast<int>(zaid.getZ());
      const int Iso=static_cast<int>(zaid.getIso());
      if (Z>ZMax) ZMax=Z;
      if (Iso>AMax) AMax=Iso;
    }
  if (ZMax>0)
    {
      resnuclei UD(name,ID,fortranTape);
      UD.setCell(cellA);
      UD.setZaid(ZMax,AMax);
      System.addTally(UD);
    }

  return;
}


void
resnucConstruct::processResNuc(SimFLUKA& System,
			       const mainSystem::inputParam& IParam,
			       const size_t Index)
  /*!
    Add TRACK tally (s) as needed
    - Input:
    -- FixedComp/CellMap name {mat}
    -- cellNumber ... cellNumber
    \param System :: SimFLUKA to add tallies
    \param IParam :: Main input parameters
    \param Index :: index of the -T card
  */
{
  ELog::RegMethod RegA("resnucConstruct","processResNuc");

  const std::string tallyName=
    IParam.getValue<std::string>("tally",Index,0);

  const std::string objectName=
    IParam.getValueError<std::string>("tally",Index,2,"tally:objectName");

  const std::string matName=
    (IParam.itemCnt("tally",Index)>3) ?
    IParam.getValue<std::string>("tally",Index,3) :
    "All";

  const std::set<int> cellVec=
    System.getObjectRangeWithMat(objectName,matName);

  const size_t ntallies = cellVec.size();
  if (ntallies == 0)
    throw ColErr::ExitAbort("No regions matching the RESNUCLEI definition found.");

  // This junk gets a good ID + fortranTape number
  int fortranTape(0);
  const std::set<resnuclei*> RSet=
    getTallyType<resnuclei>(System);
  for(const flukaTally* TPtr: RSet) {
    if (fortranTape<TPtr->getOutUnit())
      fortranTape=TPtr->getOutUnit();
  }

  if (!fortranTape) {
    fortranTape=System.getNextFTape();
  }

  int ID(0);
  for(const int CN : cellVec) {
    if (ntallies) ID++;
    resnucConstruct::createTally(System,tallyName,ID,fortranTape,CN);
  }

  return;
}

void
resnucConstruct::writeHelp(std::ostream& OX)
  /*!
    Write out help
    \param OX :: Output stream
  */
{
  OX<<"RESNUCLEI estimator options:\n";
  OX<<"* objectName[:regionName] [material]\n";
  OX<<"  if regionName is not specified, all regions of objectName are tallied;\n";
  OX<<"  if material is not specified, all materials of the specified regions are tallied.\n";
  OX<<"\n Example: -T myname resnuclei MyObject:MyRegion Concrete\n";
  return;
}

}  // NAMESPACE flukaSystem
