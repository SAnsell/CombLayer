/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/Spoke.cxx
 *
 * Copyright (c) 2018-2019 by Konstantin Batkov
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
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "inputParam.h"
#include "HeadRule.h"
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
#include "ContainedComp.h"
#include "BaseMap.h"
#include "surfDBase.h"
#include "CellMap.h"
#include "CopiedComp.h"
#include "AttachSupport.h"

#include "SpaceCut.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"

#include "Spoke.h"

#include "VacuumPipe.h"
#include "SpokeCavity.h"
#include "QXQuad.h"
#include "MagRoundVacuumPipe.h"


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

  std::string Out;


  /*
  for (size_t i=0; i<nDTL; i++)
    {
      //std::shared_ptr<DTL> d(new DTL(baseName,"DTL",i+1));

      char numstr[21];
      sprintf(numstr, "VacuumPipe%d", i+1);
      //std::string stubY = numstr;
      
      std::shared_ptr<constructSystem::VacuumPipe> d(new constructSystem::VacuumPipe(baseName+numstr));
      OR.addObject(d);
      if (i==0)
      	{
	  d->createAll(System, FC, sideIndex);
	  Out=d->getLinkString(-1)+" "+std::to_string(d->getLinkSurf(-3))+" ";
      	} else
      	{
	  d->createAll(System, *dtl[i-1],2);
      	}
      dtl.push_back(d);
    }
  Out+=dtl.back()->getLinkString(-2);
  addOuterSurf(Out);
  */


  int counter = 0;
  
  std::shared_ptr<constructSystem::VacuumPipe> d(new constructSystem::VacuumPipe(baseName+"SpokeSection0"));
  OR.addObject(d);
  d->createAll(System, FC, sideIndex);
  Out=d->getLinkString(-1)+" "+std::to_string(d->getLinkSurf(-9))+" "+std::to_string(d->getLinkSurf(-2))+" ";
  vacPipe.push_back(d);
  addOuterUnionSurf(Out);
  
  std::shared_ptr<MagRoundVacuumPipe> p(new MagRoundVacuumPipe(baseName,"VacuumPipe",0));
  OR.addObject(p);
  p->addInsertCell(d->getCells("Void"));
  p->createAll(System, FC, sideIndex);
  magRoundVacuumPipe.push_back(p);

  counter++;
  
  for(int i=1;i<13;i++){ //13
    //----------------

    
    std::shared_ptr<constructSystem::VacuumPipe> d1(new constructSystem::VacuumPipe(baseName+"SpokeSection"+StrFunc::makeString(i*12)));
    OR.addObject(d1);
    d1->createAll(System, *vacPipe[counter-1], 2);
    Out=d1->getLinkString(-1)+" "+std::to_string(d1->getLinkSurf(-9))+" "+std::to_string(d1->getLinkSurf(-2))+" ";
    vacPipe.push_back(d1);
    addOuterUnionSurf(Out);
  
    std::shared_ptr<SpokeCavity> s1(new SpokeCavity(baseName,"SPKCVT",i*12));
    OR.addObject(s1);
    s1->addInsertCell(d1->getCells("Void"));
    s1->createAll(System, *vacPipe[counter-1], 2);
    spkcvt.push_back(s1);
    counter++;
    //----------------
     
    std::shared_ptr<constructSystem::VacuumPipe> d2(new constructSystem::VacuumPipe(baseName+"SpokeSection"+StrFunc::makeString(i*12+1)));
    OR.addObject(d2);
    d2->createAll(System, *vacPipe[counter-1], 2);
    Out=d2->getLinkString(-1)+" "+std::to_string(d2->getLinkSurf(-9))+" "+std::to_string(d2->getLinkSurf(-2))+" ";
    vacPipe.push_back(d2);
    addOuterUnionSurf(Out);
  
    std::shared_ptr<MagRoundVacuumPipe> p2(new MagRoundVacuumPipe(baseName,"VacuumPipe",i*12+1));
    OR.addObject(p2);
    p2->addInsertCell(d2->getCells("Void"));
    p2->createAll(System,*vacPipe[counter-1] ,2);
    magRoundVacuumPipe.push_back(p2);
    counter++;
    // ----
    
    std::shared_ptr<constructSystem::VacuumPipe> d3(new constructSystem::VacuumPipe(baseName+"SpokeSection"+StrFunc::makeString(i*12+2)));
    OR.addObject(d3);
    d3->createAll(System, *vacPipe[counter-1], 2);
    Out=d3->getLinkString(-1)+" "+std::to_string(d3->getLinkSurf(-9))+" "+std::to_string(d3->getLinkSurf(-2))+" ";
    vacPipe.push_back(d3);
    addOuterUnionSurf(Out);
  
    std::shared_ptr<SpokeCavity> s3(new SpokeCavity(baseName,"SPKCVT",i*12+2));
    OR.addObject(s3);
    s3->addInsertCell(d3->getCells("Void"));
    s3->createAll(System, *vacPipe[counter-1], 2);
    spkcvt.push_back(s3);
    counter++;
    // ----
    
    std::shared_ptr<constructSystem::VacuumPipe> d4(new constructSystem::VacuumPipe(baseName+"SpokeSection"+StrFunc::makeString(i*12+3)));
    OR.addObject(d4);
    d4->createAll(System, *vacPipe[counter-1],2);
    Out=d4->getLinkString(-1)+" "+std::to_string(d4->getLinkSurf(-9))+" "+std::to_string(d4->getLinkSurf(-2))+" ";
    vacPipe.push_back(d4);
    addOuterUnionSurf(Out);
  
    std::shared_ptr<MagRoundVacuumPipe> p4(new MagRoundVacuumPipe(baseName,"VacuumPipe",i*12+3));
    OR.addObject(p4);
    p4->addInsertCell(d4->getCells("Void"));
    p4->createAll(System, *vacPipe[counter-1],2);
    magRoundVacuumPipe.push_back(p4);
    counter++;
    // ----

     
    std::shared_ptr<constructSystem::VacuumPipe> d5(new constructSystem::VacuumPipe(baseName+"SpokeSection"+StrFunc::makeString(i*12+4)));
    OR.addObject(d5);
    d5->createAll(System, *vacPipe[counter-1],2);
    Out=d5->getLinkString(-1)+" "+std::to_string(d5->getLinkSurf(-9))+" "+std::to_string(d5->getLinkSurf(-2))+" ";
    vacPipe.push_back(d5);
    addOuterUnionSurf(Out);
  
    std::shared_ptr<MagRoundVacuumPipe> p5(new MagRoundVacuumPipe(baseName,"VacuumPipe",i*12+4));
    OR.addObject(p5);
    p5->addInsertCell(d5->getCells("Void"));
    p5->createAll(System, *vacPipe[counter-1],2);
    magRoundVacuumPipe.push_back(p5);
    counter++;
    // ---
    
    std::shared_ptr<constructSystem::VacuumPipe> d6(new constructSystem::VacuumPipe(baseName+"SpokeSection"+StrFunc::makeString(i*12+5)));
    OR.addObject(d6);
    d6->createAll(System, *vacPipe[counter-1], 2);
    Out=d6->getLinkString(-1)+" "+std::to_string(d6->getLinkSurf(-9))+" "+std::to_string(d6->getLinkSurf(-2))+" ";
    vacPipe.push_back(d6);
    addOuterUnionSurf(Out);
    
    std::shared_ptr<QXQuad> q6(new QXQuad(baseName,"QXQuad",i*12+5));
    OR.addObject(q6);
    q6->addInsertCell(d6->getCells("Void"));
    q6->createAll(System, *vacPipe[counter-1], 2);
    qxquad.push_back(q6);
    counter++;
    // ----
    
    std::shared_ptr<constructSystem::VacuumPipe> d7(new constructSystem::VacuumPipe(baseName+"SpokeSection"+StrFunc::makeString(i*12+6)));
    OR.addObject(d7);
    d7->createAll(System, *vacPipe[counter-1],2);
    Out=d7->getLinkString(-1)+" "+std::to_string(d7->getLinkSurf(-9))+" "+std::to_string(d7->getLinkSurf(-2))+" ";
    vacPipe.push_back(d7);
    addOuterUnionSurf(Out);
  
    std::shared_ptr<MagRoundVacuumPipe> p7(new MagRoundVacuumPipe(baseName,"VacuumPipe",i*12+6));
    OR.addObject(p7);
    p7->addInsertCell(d7->getCells("Void"));
    p7->createAll(System, *vacPipe[counter-1],2);
    magRoundVacuumPipe.push_back(p7);
    counter++;
    // ----
    
    std::shared_ptr<constructSystem::VacuumPipe> d8(new constructSystem::VacuumPipe(baseName+"SpokeSection"+StrFunc::makeString(i*12+7)));
    OR.addObject(d8);
    d8->createAll(System, *vacPipe[counter-1],2);
    Out=d8->getLinkString(-1)+" "+std::to_string(d8->getLinkSurf(-9))+" "+std::to_string(d8->getLinkSurf(-2))+" ";
    vacPipe.push_back(d8);
    addOuterUnionSurf(Out);
  
    std::shared_ptr<MagRoundVacuumPipe> p8(new MagRoundVacuumPipe(baseName,"VacuumPipe",i*12+7));
    OR.addObject(p8);
    p8->addInsertCell(d8->getCells("Void"));
    p8->createAll(System, *vacPipe[counter-1],2);
    magRoundVacuumPipe.push_back(p8);
    counter++;
    // ---
    
    std::shared_ptr<constructSystem::VacuumPipe> d9(new constructSystem::VacuumPipe(baseName+"SpokeSection"+StrFunc::makeString(i*12+8)));
    OR.addObject(d9);
    d9->createAll(System, *vacPipe[counter-1], 2);
    Out=d9->getLinkString(-1)+" "+std::to_string(d9->getLinkSurf(-9))+" "+std::to_string(d9->getLinkSurf(-2))+" ";
    vacPipe.push_back(d9);
    addOuterUnionSurf(Out);
    
    std::shared_ptr<QXQuad> q9(new QXQuad(baseName,"QXQuad",i*12+8));
    OR.addObject(q9);
    q9->addInsertCell(d9->getCells("Void"));
    q9->createAll(System, *vacPipe[counter-1], 2);
    qxquad.push_back(q9);
    counter++;
    // ----
    
    std::shared_ptr<constructSystem::VacuumPipe> d10(new constructSystem::VacuumPipe(baseName+"SpokeSection"+StrFunc::makeString(i*12+9)));
    OR.addObject(d10);
    d10->createAll(System, *vacPipe[counter-1],2);
    Out=d10->getLinkString(-1)+" "+std::to_string(d10->getLinkSurf(-9))+" "+std::to_string(d10->getLinkSurf(-2))+" ";
    vacPipe.push_back(d10);
    addOuterUnionSurf(Out);
  
    std::shared_ptr<MagRoundVacuumPipe> p10(new MagRoundVacuumPipe(baseName,"VacuumPipe",i*12+9));
    OR.addObject(p10);
    p10->addInsertCell(d10->getCells("Void"));
    p10->createAll(System, *vacPipe[counter-1],2);
    magRoundVacuumPipe.push_back(p10);
    counter++;
    // ----

    std::shared_ptr<constructSystem::VacuumPipe> d11(new constructSystem::VacuumPipe(baseName+"SpokeSection"+StrFunc::makeString(i*12+10)));
    OR.addObject(d11);
    d11->createAll(System, *vacPipe[counter-1],2);
    Out=d11->getLinkString(-1)+" "+std::to_string(d11->getLinkSurf(-9))+" "+std::to_string(d11->getLinkSurf(-2))+" ";
    vacPipe.push_back(d11);
    addOuterUnionSurf(Out);
  
    std::shared_ptr<MagRoundVacuumPipe> p11(new MagRoundVacuumPipe(baseName,"VacuumPipe",i*12+10));
    OR.addObject(p11);
    p11->addInsertCell(d11->getCells("Void"));
    p11->createAll(System, *vacPipe[counter-1],2);
    magRoundVacuumPipe.push_back(p11);
    counter++;
  }

  std::shared_ptr<constructSystem::VacuumPipe> d1(new constructSystem::VacuumPipe(baseName+"SpokeSection155"));
  OR.addObject(d1);
  d1->createAll(System, *vacPipe[counter-1], 2);
  Out=d1->getLinkString(-1)+" "+std::to_string(d1->getLinkSurf(-9))+" "+std::to_string(d1->getLinkSurf(-2))+" ";
  vacPipe.push_back(d1);
  addOuterUnionSurf(Out);
  
  std::shared_ptr<SpokeCavity> s1(new SpokeCavity(baseName,"SPKCVT",155));
  OR.addObject(s1);
  s1->addInsertCell(d1->getCells("Void"));
  s1->createAll(System, *vacPipe[counter-1], 2);
  spkcvt.push_back(s1);
  counter++;
  //----------------
  
  std::shared_ptr<constructSystem::VacuumPipe> d2(new constructSystem::VacuumPipe(baseName+"SpokeSection156"));
  OR.addObject(d2);
  d2->createAll(System, *vacPipe[counter-1], 2);
  Out=d2->getLinkString(-1)+" "+std::to_string(d2->getLinkSurf(-9))+" "+std::to_string(d2->getLinkSurf(-2))+" ";
  vacPipe.push_back(d2);
  addOuterUnionSurf(Out);
  
  std::shared_ptr<MagRoundVacuumPipe> p2(new MagRoundVacuumPipe(baseName,"VacuumPipe",156));
  OR.addObject(p2);
  p2->addInsertCell(d2->getCells("Void"));
  p2->createAll(System,*vacPipe[counter-1] ,2);
  magRoundVacuumPipe.push_back(p2);
  counter++;
  // ----
  
  std::shared_ptr<constructSystem::VacuumPipe> d3(new constructSystem::VacuumPipe(baseName+"SpokeSection157"));
  OR.addObject(d3);
  d3->createAll(System, *vacPipe[counter-1], 2);
  Out=d3->getLinkString(-1)+" "+std::to_string(d3->getLinkSurf(-9))+" "+std::to_string(d3->getLinkSurf(-2))+" ";
  vacPipe.push_back(d3);
  addOuterUnionSurf(Out);
  
  std::shared_ptr<SpokeCavity> s3(new SpokeCavity(baseName,"SPKCVT",157));
  OR.addObject(s3);
  s3->addInsertCell(d3->getCells("Void"));
  s3->createAll(System, *vacPipe[counter-1], 2);
  spkcvt.push_back(s3);
  counter++;












  
  createLinks();

  return;
}

}  // essSystem
