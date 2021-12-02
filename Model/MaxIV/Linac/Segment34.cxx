/*********************************************************************
  Comb-Layer : MCNP(X) Input builder

 * File: Linac/Segment34.cxx
 *
 * Copyright (c) 2004-2020 by Konstantin Batkov
 *
 * This program is free software: you can redstribute it and/or modify
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
#include "surfRegister.h"
#include "objectRegister.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "BlockZone.h"
#include "generalConstruct.h"
#include "LObjectSupport.h"
#include "VacuumPipe.h"
#include "SplitFlangePipe.h"
#include "Bellows.h"
#include "FlatPipe.h"
#include "DipoleDIBMag.h"

#include "TDCsegment.h"
#include "Segment34.h"

namespace tdcSystem
{

// Note currently uncopied:

Segment34::Segment34(const std::string& Key) :
  TDCsegment(Key,2),

  flatA(new tdcSystem::FlatPipe(keyName+"FlatA")),
  dipoleA(new tdcSystem::DipoleDIBMag(keyName+"DipoleA")),
  pipeA(new constructSystem::VacuumPipe(keyName+"PipeA")),
  flatB(new tdcSystem::FlatPipe(keyName+"FlatB")),
  dipoleB(new tdcSystem::DipoleDIBMag(keyName+"DipoleB")),
  bellow(new constructSystem::Bellows(keyName+"Bellow"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(flatA);
  OR.addObject(dipoleA);
  OR.addObject(pipeA);
  OR.addObject(flatB);
  OR.addObject(dipoleB);
  OR.addObject(bellow);

  setFirstItems(flatA);
}

Segment34::~Segment34()
  /*!
    Destructor
   */
{}


void
Segment34::buildObjects(Simulation& System)
  /*!
    Build all the objects relative to the main FC
    point.
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Segment34","buildObjects");

  if (isActive("front"))
    flatA->copyCutSurf("front",*this,"front");

  flatA->createAll(System,*this,0);
  pipeMagGroup(System,*buildZone,flatA,
     {"FlangeA","Pipe"},"Origin","outerPipe",dipoleA);
  pipeTerminateGroup(System,*buildZone,flatA,{"FlangeB","Pipe"});

  pipeA->setFront(*flatA,"back");
  constructSystem::constructUnit
    (System,*buildZone,*flatA,"back",*pipeA);

  flatB->setFront(*pipeA,"back");
  flatB->createAll(System,*pipeA,"back");
  pipeMagGroup(System,*buildZone,flatB,
     {"FlangeA","Pipe"},"Origin","outerPipe",dipoleB);
  pipeTerminateGroup(System,*buildZone,flatB,{"FlangeB","Pipe"});

  constructSystem::constructUnit
    (System,*buildZone,*flatB,"back",*bellow);

  return;
}

void
Segment34::createLinks()
  /*!
    Create a front/back link
   */
{
  ELog::RegMethod RegA("Segment34","createLinks");

  setLinkCopy(0,*flatA,1);
  setLinkCopy(1,*bellow,2);

  joinItems.push_back(FixedComp::getFullRule(2));

  return;
}

void
Segment34::writePoints() const
  /*!
    Writes out points to allow tracking through magnets
  */
{
  ELog::RegMethod RegA("Segment34","writePoints");


  const Geometry::Vec3D Org(flatA->getLinkPt(1)*10.0);
  const Geometry::Vec3D ptFlatA(flatA->getLinkPt(2)*10.0);
  const Geometry::Vec3D ptPipeA(pipeA->getLinkPt(2)*10.0);
  const Geometry::Vec3D ptFlatB(flatB->getLinkPt(2)*10.0);
  const Geometry::Vec3D ptBellow(bellow->getLinkPt(2)*10.0);

  ELog::EM<<"Orig == "<<Org<<ELog::endDiag;
  ELog::EM<<"Flat A= "<<ptFlatA-Org<<ELog::endDiag;
  ELog::EM<<"Pipe A= "<<ptPipeA-Org<<ELog::endDiag;
  ELog::EM<<"Flat B= "<<ptFlatB-Org<<ELog::endDiag;
  ELog::EM<<"Bellow= "<<ptBellow-Org<<ELog::endDiag;


  ELog::EM<<"Length A == "<<ptFlatA.Distance(Org)<<ELog::endDiag;
  ELog::EM<<"Length C == "<<ptPipeA.Distance(ptFlatA)<<ELog::endDiag;
  ELog::EM<<"Length B == "<<ptFlatB.Distance(ptPipeA)<<ELog::endDiag;
  ELog::EM<<"Length D == "<<ptBellow.Distance(ptFlatB)<<ELog::endDiag;

  const Geometry::Vec3D bAxis=bellow->getLinkAxis(2);
  ELog::EM<<"Exit track == "<<bAxis<<ELog::endDiag;
  ELog::EM<<"Exit track == "<<
    acos(bAxis.Y())*180.0/M_PI<<ELog::endDiag;
  return;
}


void
Segment34::createAll(Simulation& System,
			  const attachSystem::FixedComp& FC,
			  const long int sideIndex)
  /*!
    Carry out the full build
    \param System :: Simulation system
    \param FC :: Fixed component
    \param sideIndex :: link point
   */
{
  // For output stream
  ELog::RegMethod RControl("Segment34","build");

  FixedRotate::populate(System.getDataBase());
  createUnitVector(FC,sideIndex);

  buildObjects(System);
  createLinks();
  //  writePoints();
  return;
}


}   // NAMESPACE tdcSystem
