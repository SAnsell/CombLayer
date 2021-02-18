/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/MBeta.cxx
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
#include "BaseMap.h"
#include "CellMap.h"
#include "CopiedComp.h"
#include "AttachSupport.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"

#include "VacuumPipe.h"
#include "SpokeCavity.h"
#include "QXQuad6.h"
#include "MagRoundVacuumPipe.h"
#include "MBCVT.h"

#include "MBeta.h"


namespace essSystem
{

MBeta::MBeta(const std::string& baseKey,const std::string& Key)  :
  attachSystem::CopiedComp(Key,Key),
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(newName,6),
  baseName(baseKey)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

MBeta::MBeta(const MBeta& A) : 
  attachSystem::CopiedComp(A),
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),
  baseName(A.baseName),
  vacPipe(A.vacPipe),
  spkcvt(A.spkcvt),
  qxquad(A.qxquad),
  magRoundVacuumPipe(A.magRoundVacuumPipe),
  mbcvt(A.mbcvt)
  /*!
    Copy constructor
    \param A :: MBeta to copy
  */
{}

MBeta&
MBeta::operator=(const MBeta& A)
  /*!
    Assignment operator
    \param A :: MBeta to copy
    \return *this
  */
{
  if (this!=&A)
    {
      vacPipe=A.vacPipe;
      spkcvt=A.spkcvt;
      qxquad=A.qxquad;
      magRoundVacuumPipe=A.magRoundVacuumPipe;
      mbcvt=A.mbcvt;
    }
  return *this;
}

MBeta*
MBeta::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new MBeta(*this);
}
  
MBeta::~MBeta() 
  /*!
    Destructor
  */
{}

void
MBeta::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("MBeta","populate");

  FixedOffset::populate(Control);

  return;
}

void
MBeta::createLinks()
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
MBeta::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("MBeta","createAll");

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
  int index = 0;
  int indexHEBT = 0;
  int indexHEBT2 = 0;
  
  std::shared_ptr<constructSystem::VacuumPipe> d(new constructSystem::VacuumPipe(baseName+"MBetaSection0"));
  OR.addObject(d);
  d->createAll(System, FC, sideIndex);
  Out=d->getLinkString(-1)+" "+std::to_string(d->getLinkSurf(-9))+" "+std::to_string(d->getLinkSurf(-2))+" ";
  vacPipe.push_back(d);
  addOuterUnionSurf(Out);

  std::shared_ptr<MagRoundVacuumPipe> p(new MagRoundVacuumPipe(baseName,"MBetaVacuumPipe",0));
  OR.addObject(p);
  p->addInsertCell(d->getCells("Void"));
  p->createAll(System, FC, sideIndex);
  magRoundVacuumPipe.push_back(p);
  counter++;
  //



  HeadRule HR;
  for(int i=1;i<10;i++)    //9 MBeta
    {
      //Quad
      ELog::EM<<"WARNONG THIS IS WRONG:  \n"
	"  Using just the vacuum pipe cylinder not the flanges "<<ELog::endCrit;
      std::shared_ptr<constructSystem::VacuumPipe>
	d1(new constructSystem::VacuumPipe
	   (baseName+"MBetaSection"+std::to_string(i*11)));
      OR.addObject(d1);
      d1->createAll(System,*vacPipe[counter-1],2);
      HR=d1->getFullRule("#front")*
	d1->getFullRule("#pipeRadius")*
	d1->getFullRule("#back");
      vacPipe.push_back(d1);
      addOuterUnionSurf(HR);
      
      std::shared_ptr<QXQuad6> q1(new QXQuad6(baseName,"MBetaQXQuad6",i*11));
      OR.addObject(q1);
      q1->addInsertCell(d1->getCells("Void"));
      q1->createAll(System, *vacPipe[counter-1], 2);
      qxquad.push_back(q1);
      counter++;

      //drift 
      std::shared_ptr<constructSystem::VacuumPipe>
	d2(new constructSystem::VacuumPipe
	   (baseName+"MBetaSection"+std::to_string(i*11+1)));
      OR.addObject(d2);
      d2->createAll(System, *vacPipe[counter-1], 2);
      HR=d2->getFullRule("#front")*
	d2->getFullRule("#pipeRadius")*
	d2->getFullRule("#back");
      vacPipe.push_back(d2);
      addOuterUnionSurf(HR);
  
    std::shared_ptr<MagRoundVacuumPipe> p2(new MagRoundVacuumPipe(baseName,"MBetaVacuumPipe",i*11+1));
    OR.addObject(p2);
    p2->addInsertCell(d2->getCells("Void"));
    p2->createAll(System,*vacPipe[counter-1] ,2);
    magRoundVacuumPipe.push_back(p2);
    counter++;

    //Quad
    std::shared_ptr<constructSystem::VacuumPipe> d3
      (new constructSystem::VacuumPipe(baseName+"MBetaSection"+std::to_string(i*11+2)));
    OR.addObject(d3);
    d3->createAll(System, *vacPipe[counter-1], 2);
    HR=d3->getFullRule("#front")*
      d3->getFullRule("#pipeRadius")*
      d3->getFullRule("#back");
    vacPipe.push_back(d3);
    addOuterUnionSurf(HR);
    
    std::shared_ptr<QXQuad6> q3(new QXQuad6(baseName,"MBetaQXQuad6",i*11+2));
    OR.addObject(q3);
    q3->addInsertCell(d3->getCells("Void"));
    q3->createAll(System, *vacPipe[counter-1], 2);
    qxquad.push_back(q3);
    counter++;

    //drift 
    std::shared_ptr<constructSystem::VacuumPipe> d4
      (new constructSystem::VacuumPipe(baseName+"MBetaSection"+std::to_string(i*11+3)));
    OR.addObject(d4);
    d4->createAll(System, *vacPipe[counter-1], 2);
    HR=d4->getFullRule("#front")*
      d4->getFullRule("#pipeRadius")*
      d4->getFullRule("#back");
    vacPipe.push_back(d4);
    addOuterUnionSurf(HR);
  
    std::shared_ptr<MagRoundVacuumPipe> p4(new MagRoundVacuumPipe(baseName,"MBetaVacuumPipe",i*11+3));
    OR.addObject(p4);
    p4->addInsertCell(d4->getCells("Void"));
    p4->createAll(System,*vacPipe[counter-1] ,2);
    magRoundVacuumPipe.push_back(p4);
    counter++;

    //drift 
    std::shared_ptr<constructSystem::VacuumPipe> d5
      (new constructSystem::VacuumPipe(baseName+"MBetaSection"+std::to_string(i*11+4)));
    OR.addObject(d5);
    d5->createAll(System, *vacPipe[counter-1], 2);
    HR=d5->getFullRule("#front")*
      d5->getFullRule("#pipeRadius")*
      d5->getFullRule("#back");
    vacPipe.push_back(d5);
    addOuterUnionSurf(HR);
  
    std::shared_ptr<MagRoundVacuumPipe> p5(new MagRoundVacuumPipe(baseName,"MBetaVacuumPipe",i*11+4));
    OR.addObject(p5);
    p5->addInsertCell(d5->getCells("Void"));
    p5->createAll(System,*vacPipe[counter-1] ,2);
    magRoundVacuumPipe.push_back(p5);
    counter++;

    //cvt
    std::shared_ptr<constructSystem::VacuumPipe> d6
      (new constructSystem::VacuumPipe(baseName+"MBetaSection"+std::to_string(i*11+5)));
    OR.addObject(d6);
    d6->createAll(System, *vacPipe[counter-1], 2);
    HR=d6->getFullRule("#front")*
      d6->getFullRule("#pipeRadius")*
      d6->getFullRule("#back");
    vacPipe.push_back(d6);
    addOuterUnionSurf(HR);

    std::shared_ptr<MBCVT> m6(new MBCVT(baseName,"MBCVT",i*11+5));
    OR.addObject(m6);
    m6->addInsertCell(d6->getCells("Void"));
    m6->createAll(System,*vacPipe[counter-1] ,2);
    mbcvt.push_back(m6);
    counter++;

    //cvt
    std::shared_ptr<constructSystem::VacuumPipe> d7
      (new constructSystem::VacuumPipe(baseName+"MBetaSection"+std::to_string(i*11+6)));
    OR.addObject(d7);
    d7->createAll(System, *vacPipe[counter-1], 2);
    HR=d7->getFullRule("#front")*
      d7->getFullRule("#pipeRadius")*
      d7->getFullRule("#back");
    vacPipe.push_back(d7);
    addOuterUnionSurf(HR);

    std::shared_ptr<MBCVT> m7(new MBCVT(baseName,"MBCVT",i*11+6));
    OR.addObject(m7);
    m7->addInsertCell(d7->getCells("Void"));
    m7->createAll(System,*vacPipe[counter-1] ,2);
    mbcvt.push_back(m7);
    counter++;

    //cvt
    std::shared_ptr<constructSystem::VacuumPipe> d8
      (new constructSystem::VacuumPipe(baseName+"MBetaSection"+std::to_string(i*11+7)));
    OR.addObject(d8);
    d8->createAll(System, *vacPipe[counter-1], 2);
    HR=d8->getFullRule("#front")*
      d8->getFullRule("#pipeRadius")*
      d8->getFullRule("#back");
    vacPipe.push_back(d8);
    addOuterUnionSurf(HR);

    std::shared_ptr<MBCVT> m8(new MBCVT(baseName,"MBCVT",i*11+7));
    OR.addObject(m8);
    m8->addInsertCell(d8->getCells("Void"));
    m8->createAll(System,*vacPipe[counter-1] ,2);
    mbcvt.push_back(m8);
    counter++;

    //cvt
    std::shared_ptr<constructSystem::VacuumPipe> d9
      (new constructSystem::VacuumPipe(baseName+"MBetaSection"+std::to_string(i*11+8)));
    OR.addObject(d9);
    d9->createAll(System, *vacPipe[counter-1], 2);
    HR=d9->getFullRule("#front")*
      d9->getFullRule("#pipeRadius")*
      d9->getFullRule("#back");
    vacPipe.push_back(d9);
    addOuterUnionSurf(HR);

    std::shared_ptr<MBCVT> m9(new MBCVT(baseName,"MBCVT",i*11+8));
    OR.addObject(m9);
    m9->addInsertCell(d9->getCells("Void"));
    m9->createAll(System,*vacPipe[counter-1] ,2);
    mbcvt.push_back(m9);
    counter++;

    //drift 
    std::shared_ptr<constructSystem::VacuumPipe> d10
      (new constructSystem::VacuumPipe(baseName+"MBetaSection"+std::to_string(i*11+9)));
    OR.addObject(d10);
    d10->createAll(System, *vacPipe[counter-1], 2);
    HR=d10->getFullRule("#front")*
      d10->getFullRule("#pipeRadius")*
      d10->getFullRule("#back");
    vacPipe.push_back(d10);
    addOuterUnionSurf(HR);
  
    std::shared_ptr<MagRoundVacuumPipe> p10(new MagRoundVacuumPipe(baseName,"MBetaVacuumPipe",i*11+9));
    OR.addObject(p10);
    p10->addInsertCell(d10->getCells("Void"));
    p10->createAll(System,*vacPipe[counter-1] ,2);
    magRoundVacuumPipe.push_back(p10);
    counter++;

    //drift 
    std::shared_ptr<constructSystem::VacuumPipe> d11
(new constructSystem::VacuumPipe(baseName+"MBetaSection"+std::to_string(i*11+10)));
    OR.addObject(d11);
    d11->createAll(System, *vacPipe[counter-1], 2);
    HR=d11->getFullRule("#front")*
      d11->getFullRule("#pipeRadius")*
      d11->getFullRule("#back");
    vacPipe.push_back(d11);
    addOuterUnionSurf(HR);
  
    std::shared_ptr<MagRoundVacuumPipe> p11
      (new MagRoundVacuumPipe(baseName,"MBetaVacuumPipe",i*11+10));
    OR.addObject(p11);
    p11->addInsertCell(d11->getCells("Void"));
    p11->createAll(System,*vacPipe[counter-1] ,2);
    magRoundVacuumPipe.push_back(p11);
    counter++;
    index = i*11+10;
  }
  index = index+1;
  for(int i=index;i<(index+21);i++){ //21 HBeta


    //Quad
    std::shared_ptr<constructSystem::VacuumPipe> d1
      (new constructSystem::VacuumPipe(baseName+"MBetaSection"+std::to_string(i*11)));
    OR.addObject(d1);
    d1->createAll(System, *vacPipe[counter-1], 2);
    HR=d1->getFullRule("#front")*
      d1->getFullRule("#pipeRadius")*
      d1->getFullRule("#back");
    vacPipe.push_back(d1);
    addOuterUnionSurf(HR);
    
    std::shared_ptr<QXQuad6> q1(new QXQuad6(baseName,"MBetaQXQuad6",i*11));
    OR.addObject(q1);
    q1->addInsertCell(d1->getCells("Void"));
    q1->createAll(System, *vacPipe[counter-1], 2);
    qxquad.push_back(q1);
    counter++;

    //drift 
    std::shared_ptr<constructSystem::VacuumPipe> d2
      (new constructSystem::VacuumPipe(baseName+"MBetaSection"+std::to_string(i*11+1)));
    OR.addObject(d2);
    d2->createAll(System, *vacPipe[counter-1], 2);
    HR=d2->getFullRule("#front")*
      d2->getFullRule("#pipeRadius")*
      d2->getFullRule("#back");
    vacPipe.push_back(d2);
    addOuterUnionSurf(HR);
  
    std::shared_ptr<MagRoundVacuumPipe> p2(new MagRoundVacuumPipe(baseName,"MBetaVacuumPipe",i*11+1));
    OR.addObject(p2);
    p2->addInsertCell(d2->getCells("Void"));
    p2->createAll(System,*vacPipe[counter-1] ,2);
    magRoundVacuumPipe.push_back(p2);
    counter++;

    //Quad
    std::shared_ptr<constructSystem::VacuumPipe> d3
      (new constructSystem::VacuumPipe(baseName+"MBetaSection"+std::to_string(i*11+2)));
    OR.addObject(d3);
    d3->createAll(System, *vacPipe[counter-1], 2);
    HR=d3->getFullRule("#front")*
      d3->getFullRule("#pipeRadius")*
      d3->getFullRule("#back");
    vacPipe.push_back(d3);
    addOuterUnionSurf(HR);
    
    std::shared_ptr<QXQuad6> q3(new QXQuad6(baseName,"MBetaQXQuad6",i*11+2));
    OR.addObject(q3);
    q3->addInsertCell(d3->getCells("Void"));
    q3->createAll(System, *vacPipe[counter-1], 2);
    qxquad.push_back(q3);
    counter++;

    //drift 
    std::shared_ptr<constructSystem::VacuumPipe> d4
      (new constructSystem::VacuumPipe(baseName+"MBetaSection"+std::to_string(i*11+3)));
    OR.addObject(d4);
    d4->createAll(System, *vacPipe[counter-1], 2);
    HR=d4->getFullRule("#front")*
      d4->getFullRule("#pipeRadius")*
      d4->getFullRule("#back");
    vacPipe.push_back(d4);
    addOuterUnionSurf(HR);
  
    std::shared_ptr<MagRoundVacuumPipe> p4
      (new MagRoundVacuumPipe(baseName,"MBetaVacuumPipe",i*11+3));
    OR.addObject(p4);
    p4->addInsertCell(d4->getCells("Void"));
    p4->createAll(System,*vacPipe[counter-1] ,2);
    magRoundVacuumPipe.push_back(p4);
    counter++;

    //drift 
    std::shared_ptr<constructSystem::VacuumPipe> d5
      (new constructSystem::VacuumPipe(baseName+"MBetaSection"+std::to_string(i*11+4)));
    OR.addObject(d5);
    d5->createAll(System, *vacPipe[counter-1], 2);
    HR=d5->getFullRule("#front")*
      d5->getFullRule("#pipeRadius")*
      d5->getFullRule("#back");
    vacPipe.push_back(d5);
    addOuterUnionSurf(HR);
  
    std::shared_ptr<MagRoundVacuumPipe> p5(new MagRoundVacuumPipe(baseName,"MBetaVacuumPipe",i*11+4));
    OR.addObject(p5);
    p5->addInsertCell(d5->getCells("Void"));
    p5->createAll(System,*vacPipe[counter-1] ,2);
    magRoundVacuumPipe.push_back(p5);
    counter++;

    //cvt
    std::shared_ptr<constructSystem::VacuumPipe> d6
      (new constructSystem::VacuumPipe(baseName+"MBetaSection"+std::to_string(i*11+5)));
    OR.addObject(d6);
    d6->createAll(System, *vacPipe[counter-1], 2);
    HR=d6->getFullRule("#front")*
      d6->getFullRule("#pipeRadius")*
      d6->getFullRule("#back");
    vacPipe.push_back(d6);
    addOuterUnionSurf(HR);

    std::shared_ptr<MBCVT> m6(new MBCVT(baseName,"MBCVT",i*11+5));
    OR.addObject(m6);
    m6->addInsertCell(d6->getCells("Void"));
    m6->createAll(System,*vacPipe[counter-1] ,2);
    mbcvt.push_back(m6);
    counter++;

    //cvt
    std::shared_ptr<constructSystem::VacuumPipe> d7(new constructSystem::VacuumPipe(baseName+"MBetaSection"+std::to_string(i*11+6)));
    OR.addObject(d7);
    d7->createAll(System, *vacPipe[counter-1], 2);
    HR=d7->getFullRule("#front")*
      d7->getFullRule("#pipeRadius")*
      d7->getFullRule("#back");
    vacPipe.push_back(d7);
    addOuterUnionSurf(HR);

    std::shared_ptr<MBCVT> m7(new MBCVT(baseName,"MBCVT",i*11+6));
    OR.addObject(m7);
    m7->addInsertCell(d7->getCells("Void"));
    m7->createAll(System,*vacPipe[counter-1] ,2);
    mbcvt.push_back(m7);
    counter++;

    //cvt
    std::shared_ptr<constructSystem::VacuumPipe> d8
      (new constructSystem::VacuumPipe(baseName+"MBetaSection"+std::to_string(i*11+7)));
    OR.addObject(d8);
    d8->createAll(System, *vacPipe[counter-1], 2);
    HR=d8->getFullRule("#front")*
      d8->getFullRule("#pipeRadius")*
      d8->getFullRule("#back");
    vacPipe.push_back(d8);
    addOuterUnionSurf(HR);

    std::shared_ptr<MBCVT> m8(new MBCVT(baseName,"MBCVT",i*11+7));
    OR.addObject(m8);
    m8->addInsertCell(d8->getCells("Void"));
    m8->createAll(System,*vacPipe[counter-1] ,2);
    mbcvt.push_back(m8);
    counter++;

    //cvt
    std::shared_ptr<constructSystem::VacuumPipe> d9
      (new constructSystem::VacuumPipe(baseName+"MBetaSection"+std::to_string(i*11+8)));
    OR.addObject(d9);
    d9->createAll(System, *vacPipe[counter-1], 2);
    HR=d9->getFullRule("#front")*
      d9->getFullRule("#pipeRadius")*
      d9->getFullRule("#back");
    vacPipe.push_back(d9);
    addOuterUnionSurf(HR);

    std::shared_ptr<MBCVT> m9(new MBCVT(baseName,"MBCVT",i*11+8));
    OR.addObject(m9);
    m9->addInsertCell(d9->getCells("Void"));
    m9->createAll(System,*vacPipe[counter-1] ,2);
    mbcvt.push_back(m9);
    counter++;

    //drift 
    std::shared_ptr<constructSystem::VacuumPipe> d10
      (new constructSystem::VacuumPipe(baseName+"MBetaSection"+std::to_string(i*11+9)));
    OR.addObject(d10);
    d10->createAll(System, *vacPipe[counter-1], 2);
    HR=d10->getFullRule("#front")*
      d10->getFullRule("#pipeRadius")*
      d10->getFullRule("#back");
    vacPipe.push_back(d10);
    addOuterUnionSurf(HR);
  
    std::shared_ptr<MagRoundVacuumPipe> p10(new MagRoundVacuumPipe(baseName,"MBetaVacuumPipe",i*11+9));
    OR.addObject(p10);
    p10->addInsertCell(d10->getCells("Void"));
    p10->createAll(System,*vacPipe[counter-1] ,2);
    magRoundVacuumPipe.push_back(p10);
    counter++;

    //drift 
    std::shared_ptr<constructSystem::VacuumPipe> d11
      (new constructSystem::VacuumPipe(baseName+"MBetaSection"+std::to_string(i*11+10)));
    OR.addObject(d11);
    d11->createAll(System, *vacPipe[counter-1], 2);
    HR=d11->getFullRule("#front")*
      d11->getFullRule("#pipeRadius")*
      d11->getFullRule("#back");
    vacPipe.push_back(d11);
    addOuterUnionSurf(HR);
  
    std::shared_ptr<MagRoundVacuumPipe> p11
      (new MagRoundVacuumPipe(baseName,"MBetaVacuumPipe",i*11+10));
    OR.addObject(p11);
    p11->addInsertCell(d11->getCells("Void"));
    p11->createAll(System,*vacPipe[counter-1] ,2);
    magRoundVacuumPipe.push_back(p11);
    counter++;
    indexHEBT = i*11+10;
    
  }
  indexHEBT = indexHEBT + 1;

  for(int i=indexHEBT;i<(indexHEBT+15);i++){

    //Quad
    std::shared_ptr<constructSystem::VacuumPipe> d1
      (new constructSystem::VacuumPipe(baseName+"MBetaSection"+std::to_string(i*4)));
    OR.addObject(d1);
    d1->createAll(System, *vacPipe[counter-1], 2);
    HR=d1->getFullRule("#front")*
      d1->getFullRule("#pipeRadius")*
      d1->getFullRule("#back");
    vacPipe.push_back(d1);
    addOuterUnionSurf(HR);
    
    std::shared_ptr<QXQuad6> q1(new QXQuad6(baseName,"MBetaQXQuad6",i*4));
    OR.addObject(q1);
    q1->addInsertCell(d1->getCells("Void"));
    q1->createAll(System, *vacPipe[counter-1], 2);
    qxquad.push_back(q1);
    counter++;

    //drift 
    std::shared_ptr<constructSystem::VacuumPipe> d2
      (new constructSystem::VacuumPipe(baseName+"MBetaSection"+std::to_string(i*4+1)));
    OR.addObject(d2);
    d2->createAll(System, *vacPipe[counter-1], 2);
    HR=d2->getFullRule("#front")*
      d2->getFullRule("#pipeRadius")*
      d2->getFullRule("#back");
    vacPipe.push_back(d2);
    addOuterUnionSurf(HR);
  
    std::shared_ptr<MagRoundVacuumPipe> p2(new MagRoundVacuumPipe(baseName,"MBetaVacuumPipe",i*4+1));
    OR.addObject(p2);
    p2->addInsertCell(d2->getCells("Void"));
    p2->createAll(System,*vacPipe[counter-1] ,2);
    magRoundVacuumPipe.push_back(p2);
    counter++;

    //Quad
    std::shared_ptr<constructSystem::VacuumPipe> d3
      (new constructSystem::VacuumPipe(baseName+"MBetaSection"+std::to_string(i*4+2)));
    OR.addObject(d3);
    d3->createAll(System, *vacPipe[counter-1], 2);
    HR=d3->getFullRule("#front")*
      d3->getFullRule("#pipeRadius")*
      d3->getFullRule("#back");
    vacPipe.push_back(d3);
    addOuterUnionSurf(HR);
    
    std::shared_ptr<QXQuad6> q3(new QXQuad6(baseName,"MBetaQXQuad6",i*4+2));
    OR.addObject(q3);
    q3->addInsertCell(d3->getCells("Void"));
    q3->createAll(System, *vacPipe[counter-1], 2);
    qxquad.push_back(q3);
    counter++;

    //drift 
    std::shared_ptr<constructSystem::VacuumPipe> d4
      (new constructSystem::VacuumPipe(baseName+"MBetaSection"+std::to_string(i*4+3)));
    OR.addObject(d4);
    d4->createAll(System, *vacPipe[counter-1], 2); 
    HR=d4->getFullRule("#front")*
      d4->getFullRule("#pipeRadius")*
      d4->getFullRule("#back");
    vacPipe.push_back(d4);
    addOuterUnionSurf(HR);
  
    std::shared_ptr<MagRoundVacuumPipe> p4(new MagRoundVacuumPipe(baseName,"MBetaVacuumPipe",i*4+3));
    OR.addObject(p4);
    p4->addInsertCell(d4->getCells("Void"));
    p4->createAll(System,*vacPipe[counter-1] ,2);
    magRoundVacuumPipe.push_back(p4);
    counter++;

    indexHEBT2 = i*4+3;
  }

  indexHEBT2 = indexHEBT2+1;

  //Quad
  std::shared_ptr<constructSystem::VacuumPipe> d1
    (new constructSystem::VacuumPipe(baseName+"MBetaSection"+std::to_string(indexHEBT2)));
  OR.addObject(d1);
  d1->createAll(System, *vacPipe[counter-1], 2);
  HR=d1->getFullRule("#front")*
      d1->getFullRule("#pipeRadius")*
      d1->getFullRule("#back");
  vacPipe.push_back(d1);
  addOuterUnionSurf(HR);
    
  std::shared_ptr<QXQuad6> q1(new QXQuad6(baseName,"MBetaQXQuad6",indexHEBT2));
  OR.addObject(q1);
  q1->addInsertCell(d1->getCells("Void"));
  q1->createAll(System, *vacPipe[counter-1], 2);
  qxquad.push_back(q1);
  counter++;

  //drift 
  std::shared_ptr<constructSystem::VacuumPipe> d2
    (new constructSystem::VacuumPipe(baseName+"MBetaSection"+std::to_string(indexHEBT2+1)));
  OR.addObject(d2);
  d2->createAll(System, *vacPipe[counter-1], 2);
  HR=d2->getFullRule("#front")*
      d2->getFullRule("#pipeRadius")*
      d2->getFullRule("#back");
  vacPipe.push_back(d2);
  addOuterUnionSurf(HR);
  
  std::shared_ptr<MagRoundVacuumPipe> p2
    (new MagRoundVacuumPipe(baseName,"MBetaVacuumPipe",indexHEBT2+1));
  OR.addObject(p2);
  p2->addInsertCell(d2->getCells("Void"));
  p2->createAll(System,*vacPipe[counter-1] ,2);
  magRoundVacuumPipe.push_back(p2);
  counter++;
  
  //Quad
  std::shared_ptr<constructSystem::VacuumPipe> d3
    (new constructSystem::VacuumPipe(baseName+"MBetaSection"+std::to_string(indexHEBT2+2)));
  OR.addObject(d3);
  d3->createAll(System, *vacPipe[counter-1], 2);
  HR=d3->getFullRule("#front")*
      d3->getFullRule("#pipeRadius")*
      d3->getFullRule("#back");
  vacPipe.push_back(d3);
  addOuterUnionSurf(HR);
  
  std::shared_ptr<QXQuad6> q3(new QXQuad6(baseName,"MBetaQXQuad6",indexHEBT2+2));
  OR.addObject(q3);
  q3->addInsertCell(d3->getCells("Void"));
  q3->createAll(System, *vacPipe[counter-1], 2);
  qxquad.push_back(q3);
  counter++;

  //drift 
  std::shared_ptr<constructSystem::VacuumPipe> d4
    (new constructSystem::VacuumPipe(baseName+"MBetaSection"+std::to_string(indexHEBT2+3)));
  OR.addObject(d4);
  d4->createAll(System, *vacPipe[counter-1], 2);
  HR=d4->getFullRule("#front")*
      d4->getFullRule("#pipeRadius")*
      d4->getFullRule("#back");
  vacPipe.push_back(d4);
  addOuterUnionSurf(HR);
  
  std::shared_ptr<MagRoundVacuumPipe> p4
     (new MagRoundVacuumPipe(baseName,"MBetaVacuumPipe",indexHEBT2+3));
  OR.addObject(p4);
  p4->addInsertCell(d4->getCells("Void"));
  p4->createAll(System,*vacPipe[counter-1] ,2);
  magRoundVacuumPipe.push_back(p4);
  counter++;

  
  createLinks();

  return;
}

}  // essSystem
