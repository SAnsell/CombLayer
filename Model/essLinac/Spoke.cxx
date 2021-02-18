/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essLinac/Spoke.cxx
 *
 * Copyright (c) 2018-2021 by Konstantin Batkov
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
#include <memory>
#include <numeric>

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
#include "Surface.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "CopiedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"

#include "VacuumPipe.h"
#include "SpokeCavity.h"
#include "QXQuad5.h"
#include "MagRoundVacuumPipe.h"

#include "Spoke.h"


namespace essSystem
{

Spoke::Spoke(const std::string& baseKey,const std::string& Key)  :
  attachSystem::CopiedComp(Key,Key),
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(newName,6),
  baseName(baseKey)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

Spoke::Spoke(const Spoke& A) : 
  attachSystem::CopiedComp(A),
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),
  baseName(A.baseName),
  vacPipe(A.vacPipe),
  spkcvt(A.spkcvt),
  qxquad(A.qxquad),
  magRoundVacuumPipe(A.magRoundVacuumPipe)
  /*!
    Copy constructor
    \param A :: Spoke to copy
  */
{}

Spoke&
Spoke::operator=(const Spoke& A)
  /*!
    Assignment operator
    \param A :: Spoke to copy
    \return *this
  */
{
  if (this!=&A)
    {
      vacPipe=A.vacPipe;
      spkcvt=A.spkcvt;
      qxquad=A.qxquad;
      magRoundVacuumPipe=A.magRoundVacuumPipe;
    }
  return *this;
}

Spoke*
Spoke::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new Spoke(*this);
}
  
Spoke::~Spoke() 
  /*!
    Destructor
  */
{}

void
Spoke::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("Spoke","populate");

  FixedOffset::populate(Control);

  return;
}

void
Spoke::createLinks()
/*!
  Construct all the linksx
 */
{
  const int N = std::accumulate(vacPipe.begin(),vacPipe.end(),0,
				[](size_t total,std::shared_ptr<constructSystem::VacuumPipe> d)
				{
				  return total+d->NConnect();
				});
  FixedComp::setNConnect(static_cast<size_t>(N));

  size_t i(0);
  for (const std::shared_ptr<constructSystem::VacuumPipe> d: vacPipe) {
    for (size_t j=0; j<d->NConnect(); j++)
      setUSLinkCopy(i++, *d, j);
  }
}
  
void
Spoke::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("Spoke","createAll");

  populate(System.getDataBase());

  //if (nDTL<1)
  //  return;

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  HeadRule HR;

  int counter = 0;
  
  std::shared_ptr<constructSystem::VacuumPipe>
    d(new constructSystem::VacuumPipe(baseName+"SpokeSection0"));
  OR.addObject(d);
  d->createAll(System, FC, sideIndex);
  HR=d->getFullRule("#front")*
    d->getFullRule("#pipeRadius")*
    d->getFullRule("#back");
  vacPipe.push_back(d);
  addOuterUnionSurf(HR);
  
  std::shared_ptr<MagRoundVacuumPipe> p
    (new MagRoundVacuumPipe(baseName,"VacuumPipe",0));
  OR.addObject(p);
  p->addInsertCell(d->getCells("Void"));
  p->createAll(System, FC, sideIndex);
  magRoundVacuumPipe.push_back(p);
  
  counter++;
  // ----  
  std::shared_ptr<constructSystem::VacuumPipe> d1
    (new constructSystem::VacuumPipe
     (baseName+"SpokeSection"+std::to_string(1)));
  OR.addObject(d1);
  d1->createAll(System, *vacPipe[counter-1], 2);
  HR=d1->getFullRule("#front")*
    d1->getFullRule("#pipeRadius")*
    d1->getFullRule("#back");
  vacPipe.push_back(d1);
  addOuterUnionSurf(HR);
  
  std::shared_ptr<QXQuad5> q1(new QXQuad5(baseName,"QXQuad5",1));
  OR.addObject(q1);
  q1->addInsertCell(d1->getCells("Void"));
  q1->createAll(System, *vacPipe[counter-1], 2);
  qxquad.push_back(q1);
  counter++;
  
  // ----  
  std::shared_ptr<constructSystem::VacuumPipe> d2
    (new constructSystem::VacuumPipe
     (baseName+"SpokeSection"+std::to_string(2)));
  OR.addObject(d2);
  d2->createAll(System, *vacPipe[counter-1],2);
  HR=d2->getFullRule("#front")*
    d2->getFullRule("#pipeRadius")*
    d2->getFullRule("#back");
  vacPipe.push_back(d2);
  addOuterUnionSurf(HR);
  
  std::shared_ptr<MagRoundVacuumPipe> p2(new MagRoundVacuumPipe(baseName,"VacuumPipe",2));
  OR.addObject(p2);
  p2->addInsertCell(d2->getCells("Void"));
  p2->createAll(System, *vacPipe[counter-1],2);
  magRoundVacuumPipe.push_back(p2);
  counter++;
  // ----
   std::shared_ptr<constructSystem::VacuumPipe> d3
     (new constructSystem::VacuumPipe
      (baseName+"SpokeSection"+std::to_string(3)));
  OR.addObject(d3);
  d3->createAll(System, *vacPipe[counter-1], 2);
  HR=d3->getFullRule("#front")*
    d3->getFullRule("#pipeRadius")*
    d3->getFullRule("#back");
  vacPipe.push_back(d3);
  addOuterUnionSurf(HR);
    
  std::shared_ptr<QXQuad5> q3(new QXQuad5(baseName,"QXQuad5",3));
  OR.addObject(q3);
  q3->addInsertCell(d3->getCells("Void"));
  q3->createAll(System, *vacPipe[counter-1], 2);
  qxquad.push_back(q3);
  counter++;
  
  // ----  
  std::shared_ptr<constructSystem::VacuumPipe> d4
    (new constructSystem::VacuumPipe
     (baseName+"SpokeSection"+std::to_string(4)));
  OR.addObject(d4);
  d4->createAll(System, *vacPipe[counter-1],2);
  HR=d4->getFullRule("#front")*
    d4->getFullRule("#pipeRadius")*
    d4->getFullRule("#back");
  vacPipe.push_back(d4);
  addOuterUnionSurf(HR);
  
  std::shared_ptr<MagRoundVacuumPipe> p4(new MagRoundVacuumPipe(baseName,"VacuumPipe",4));
  OR.addObject(p4);
  p4->addInsertCell(d4->getCells("Void"));
  p4->createAll(System, *vacPipe[counter-1],2);
  magRoundVacuumPipe.push_back(p4);
  counter++;
  // ----
  std::shared_ptr<constructSystem::VacuumPipe> d5
    (new constructSystem::VacuumPipe
     (baseName+"SpokeSection"+std::to_string(5)));
  OR.addObject(d5);
  d5->createAll(System, *vacPipe[counter-1],2);
  HR=d5->getFullRule("#front")*
    d5->getFullRule("#pipeRadius")*
    d5->getFullRule("#back");
  vacPipe.push_back(d5);
  addOuterUnionSurf(HR);
  
  std::shared_ptr<MagRoundVacuumPipe> p5
    (new MagRoundVacuumPipe(baseName,"VacuumPipe",5));
  OR.addObject(p5);
  p5->addInsertCell(d5->getCells("Void"));
  p5->createAll(System, *vacPipe[counter-1],2);
  magRoundVacuumPipe.push_back(p5);
  counter++;
  
  // ----
  
  int index = 0;
  for(int i=(1+5);i<(13+5);i++){ //13 
    //----------------
   
    std::shared_ptr<constructSystem::VacuumPipe> d6
      (new constructSystem::VacuumPipe
       (baseName+"SpokeSection"+std::to_string(i*12)));
    OR.addObject(d6);
    d6->createAll(System, *vacPipe[counter-1], 2);
    HR=d6->getFullRule("#front")*
    d6->getFullRule("#pipeRadius")*
    d6->getFullRule("#back");
    vacPipe.push_back(d6);
    addOuterUnionSurf(HR);
  
    std::shared_ptr<SpokeCavity> s6(new SpokeCavity(baseName,"SPKCVT",i*12));
    OR.addObject(s6);
    s6->addInsertCell(d6->getCells("Void"));
    s6->createAll(System, *vacPipe[counter-1], 2);
    spkcvt.push_back(s6);
    counter++;
    //----------------
     
    std::shared_ptr<constructSystem::VacuumPipe> d7
      (new constructSystem::VacuumPipe
       (baseName+"SpokeSection"+std::to_string(i*12+1)));
    OR.addObject(d7);
    d7->createAll(System, *vacPipe[counter-1], 2);
    HR=d7->getFullRule("#front")*
    d7->getFullRule("#pipeRadius")*
    d7->getFullRule("#back");
    vacPipe.push_back(d7);
    addOuterUnionSurf(HR);
  
    std::shared_ptr<MagRoundVacuumPipe> p7(new MagRoundVacuumPipe(baseName,"VacuumPipe",i*12+1));
    OR.addObject(p7);
    p7->addInsertCell(d7->getCells("Void"));
    p7->createAll(System,*vacPipe[counter-1] ,2);
    magRoundVacuumPipe.push_back(p7);
    counter++;
    // ----
    
    std::shared_ptr<constructSystem::VacuumPipe> d8
      (new constructSystem::VacuumPipe
       (baseName+"SpokeSection"+std::to_string(i*12+2)));
    OR.addObject(d8);
    d8->createAll(System, *vacPipe[counter-1], 2);
    HR=d8->getFullRule("#front")*
      d8->getFullRule("#pipeRadius")*
      d8->getFullRule("#back");
    vacPipe.push_back(d8);
    addOuterUnionSurf(HR);
  
    std::shared_ptr<SpokeCavity> s8(new SpokeCavity(baseName,"SPKCVT",i*12+2));
    OR.addObject(s8);
    s8->addInsertCell(d8->getCells("Void"));
    s8->createAll(System, *vacPipe[counter-1], 2);
    spkcvt.push_back(s8);
    counter++;
    // ----
    
    std::shared_ptr<constructSystem::VacuumPipe> d9
      (new constructSystem::VacuumPipe
       (baseName+"SpokeSection"+std::to_string(i*12+3)));
    OR.addObject(d9);
    d9->createAll(System, *vacPipe[counter-1],2);
    HR=d9->getFullRule("#front")*
      d9->getFullRule("#pipeRadius")*
      d9->getFullRule("#back");
    vacPipe.push_back(d9);
    addOuterUnionSurf(HR);
  
    std::shared_ptr<MagRoundVacuumPipe> p9(new MagRoundVacuumPipe(baseName,"VacuumPipe",i*12+3));
    OR.addObject(p9);
    p9->addInsertCell(d9->getCells("Void"));
    p9->createAll(System, *vacPipe[counter-1],2);
    magRoundVacuumPipe.push_back(p9);
    counter++;
    // ----

     
    std::shared_ptr<constructSystem::VacuumPipe> d10
      (new constructSystem::VacuumPipe
       (baseName+"SpokeSection"+std::to_string(i*12+4)));
    OR.addObject(d10);
    d10->createAll(System, *vacPipe[counter-1],2);
    HR=d10->getFullRule("#front")*
      d10->getFullRule("#pipeRadius")*
      d10->getFullRule("#back");
    vacPipe.push_back(d10);
    addOuterUnionSurf(HR);
  
    std::shared_ptr<MagRoundVacuumPipe> p10(new MagRoundVacuumPipe(baseName,"VacuumPipe",i*12+4));
    OR.addObject(p10);
    p10->addInsertCell(d10->getCells("Void"));
    p10->createAll(System, *vacPipe[counter-1],2);
    magRoundVacuumPipe.push_back(p10);
    counter++;
    // ---
    
    std::shared_ptr<constructSystem::VacuumPipe> d11
      (new constructSystem::VacuumPipe
       (baseName+"SpokeSection"+std::to_string(i*12+5)));
    OR.addObject(d11);
    d11->createAll(System, *vacPipe[counter-1], 2);
    HR=d11->getFullRule("#front")*
      d11->getFullRule("#pipeRadius")*
      d11->getFullRule("#back");
    vacPipe.push_back(d11);
    addOuterUnionSurf(HR);
    
    std::shared_ptr<QXQuad5> q11(new QXQuad5(baseName,"QXQuad5",i*12+5));
    OR.addObject(q11);
    q11->addInsertCell(d11->getCells("Void"));
    q11->createAll(System, *vacPipe[counter-1], 2);
    qxquad.push_back(q11);
    counter++;
    // ----
    
    std::shared_ptr<constructSystem::VacuumPipe> d12
      (new constructSystem::VacuumPipe
       (baseName+"SpokeSection"+std::to_string(i*12+6)));
    OR.addObject(d12);
    d12->createAll(System, *vacPipe[counter-1],2);
    HR=d11->getFullRule("#front")*
    d11->getFullRule("#pipeRadius")*
    d11->getFullRule("#back");
    vacPipe.push_back(d12);
    addOuterUnionSurf(HR);
  
    std::shared_ptr<MagRoundVacuumPipe> p12(new MagRoundVacuumPipe(baseName,"VacuumPipe",i*12+6));
    OR.addObject(p12);
    p12->addInsertCell(d12->getCells("Void"));
    p12->createAll(System, *vacPipe[counter-1],2);
    magRoundVacuumPipe.push_back(p12);
    counter++;
    // ----
      
    std::shared_ptr<constructSystem::VacuumPipe> d13
      (new constructSystem::VacuumPipe
       (baseName+"SpokeSection"+std::to_string(i*12+7)));
    OR.addObject(d13);
    d13->createAll(System, *vacPipe[counter-1], 2);
    HR=d13->getFullRule("#front")*
      d13->getFullRule("#pipeRadius")*
      d13->getFullRule("#back");
    vacPipe.push_back(d13);
    addOuterUnionSurf(HR);
    
    std::shared_ptr<QXQuad5> q13(new QXQuad5(baseName,"QXQuad5",i*12+7));
    OR.addObject(q13);
    q13->addInsertCell(d13->getCells("Void"));
    q13->createAll(System, *vacPipe[counter-1], 2);
    qxquad.push_back(q13);
    counter++;
    // ----
    
    std::shared_ptr<constructSystem::VacuumPipe> d14
      (new constructSystem::VacuumPipe
       (baseName+"SpokeSection"+std::to_string(i*12+8)));
    OR.addObject(d14);
    d14->createAll(System, *vacPipe[counter-1],2);
    HR=d14->getFullRule("#front")*
    d14->getFullRule("#pipeRadius")*
    d14->getFullRule("#back");
    vacPipe.push_back(d14);
    addOuterUnionSurf(HR);
  
    std::shared_ptr<MagRoundVacuumPipe> p14(new MagRoundVacuumPipe(baseName,"VacuumPipe",i*12+8));
    OR.addObject(p14);
    p14->addInsertCell(d14->getCells("Void"));
    p14->createAll(System, *vacPipe[counter-1],2);
    magRoundVacuumPipe.push_back(p14);
    counter++;
    // ----

    std::shared_ptr<constructSystem::VacuumPipe> d15
      (new constructSystem::VacuumPipe
       (baseName+"SpokeSection"+std::to_string(i*12+9)));
    OR.addObject(d15);
    d15->createAll(System, *vacPipe[counter-1],2);
    HR=d15->getFullRule("#front")*
    d15->getFullRule("#pipeRadius")*
    d15->getFullRule("#back");
    addOuterUnionSurf(HR);
  
    std::shared_ptr<MagRoundVacuumPipe> p15(new MagRoundVacuumPipe(baseName,"VacuumPipe",i*12+9));
    OR.addObject(p15);
    p15->addInsertCell(d15->getCells("Void"));
    p15->createAll(System, *vacPipe[counter-1],2);
    magRoundVacuumPipe.push_back(p15);
    counter++;

    index = i*12+9;
  }
  index=index+1;
  std::shared_ptr<constructSystem::VacuumPipe> d16
    (new constructSystem::VacuumPipe
     (baseName+"SpokeSection"+std::to_string(index)));
  OR.addObject(d16);
  d16->createAll(System, *vacPipe[counter-1], 2);
  HR=d16->getFullRule("#front")*
    d16->getFullRule("#pipeRadius")*
    d16->getFullRule("#back");
  vacPipe.push_back(d16);
  addOuterUnionSurf(HR);
  
  std::shared_ptr<SpokeCavity> s16(new SpokeCavity(baseName,"SPKCVT",index));
  OR.addObject(s16);
  s16->addInsertCell(d16->getCells("Void"));
  s16->createAll(System, *vacPipe[counter-1], 2);
  spkcvt.push_back(s16);
  counter++;

  //----------------
  
  std::shared_ptr<constructSystem::VacuumPipe> d17
    (new constructSystem::VacuumPipe
     (baseName+"SpokeSection"+std::to_string(index+1)));
  OR.addObject(d17);
  d17->createAll(System, *vacPipe[counter-1], 2);
  HR=d17->getFullRule("#front")*
    d17->getFullRule("#pipeRadius")*
    d17->getFullRule("#back");
  vacPipe.push_back(d17);
  addOuterUnionSurf(HR);
  
  std::shared_ptr<MagRoundVacuumPipe> p17
    (new MagRoundVacuumPipe(baseName,"VacuumPipe",index+1));
  OR.addObject(p17);
  p17->addInsertCell(d17->getCells("Void"));
  p17->createAll(System,*vacPipe[counter-1] ,2);
  magRoundVacuumPipe.push_back(p17);
  counter++;
  //----------------
  std::shared_ptr<constructSystem::VacuumPipe> d18
    (new constructSystem::VacuumPipe
     (baseName+"SpokeSection"+std::to_string(index+2)));
  OR.addObject(d18);
  d18->createAll(System, *vacPipe[counter-1], 2);
  HR=d18->getFullRule("#front")*
    d18->getFullRule("#pipeRadius")*
    d18->getFullRule("#back");
  vacPipe.push_back(d18);
  addOuterUnionSurf(HR);
  
  std::shared_ptr<SpokeCavity> s18(new SpokeCavity(baseName,"SPKCVT",index+2));
  OR.addObject(s18);
  s18->addInsertCell(d18->getCells("Void"));
  s18->createAll(System, *vacPipe[counter-1], 2);
  spkcvt.push_back(s18);
  counter++;

  //----------------
  
  std::shared_ptr<constructSystem::VacuumPipe> d19
    (new constructSystem::VacuumPipe
     (baseName+"SpokeSection"+std::to_string(index+3)));
  OR.addObject(d19);
  d19->createAll(System, *vacPipe[counter-1], 2);
  HR=d19->getFullRule("#front")*
    d19->getFullRule("#pipeRadius")*
    d19->getFullRule("#back");
  vacPipe.push_back(d19);
  addOuterUnionSurf(HR);
  
  std::shared_ptr<MagRoundVacuumPipe>
    p19(new MagRoundVacuumPipe(baseName,"VacuumPipe",index+3));
  OR.addObject(p19);
  p19->addInsertCell(d19->getCells("Void"));
  p19->createAll(System,*vacPipe[counter-1] ,2);
  magRoundVacuumPipe.push_back(p19);
  counter++;
  
  createLinks();

  return;
}

}  // essSystem
