/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   photon/makePhoton2.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include "inputParam.h"
#include "stringCombine.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Rules.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "FrontBackCut.h"
#include "BoundOuter.h"
#include "insertObject.h"
#include "insertPlate.h"
#include "insertSphere.h"
#include "World.h"
#include "AttachSupport.h"
#include "plateInfo.h"
#include "PlateMod.h"
#include "EQDetector.h"
#include "ModContainer.h"
#include "VacuumVessel.h"
#include "CylLayer.h"
#include "TableSupport.h"
#include "He3Tubes.h"
#include "TubeDetBox.h"
#include "HeShield.h"
#include "TubeCollimator.h"
#include "insertCylinder.h"

#include "makePhoton2.h"

namespace photonSystem
{

makePhoton2::makePhoton2() :
  PModObj(new photonSystem::PlateMod("PMod")),
  Catcher(new insertSystem::insertCylinder("Catcher")),
  Chamber(new photonSystem::VacuumVessel("Chamber")),
  BaseSupport(new photonSystem::TableSupport("BaseSupport")),
  centralSupport(new photonSystem::HeShield("CentralShield")),
  centralTubes(new constructSystem::TubeDetBox("CentralDBox",1)),
  rightTubes(new photonSystem::He3Tubes("RightTubes")),
  leftColl(new photonSystem::TubeCollimator("LeftColl"))
  /*! 
    Constructor
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(PModObj);
  OR.addObject(Catcher);
  OR.addObject(Chamber);
  OR.addObject(BaseSupport);
  OR.addObject(centralSupport);
  OR.addObject(centralTubes);
  OR.addObject(rightTubes);
  OR.addObject(leftColl);
  
}

makePhoton2::~makePhoton2()
  /*!
    Destructor
   */
{}

void
makePhoton2::buildWings(Simulation& System)
  /*!
    Make the wings
    \param System :: Simulation to add wings to
   */
{
  ELog::RegMethod RegA("makePhoton2","buildWings");
    int voidCell(74123);
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  const FuncDataBase& Control=System.getDataBase();
  const double H=Control.EvalDefVar<double>("SPlateHeight",1.5);
  const double W=Control.EvalDefVar<double>("SPlateWidth",7.5);

  const Geometry::Vec3D YA=PModObj->getLinkPt(1);
  const Geometry::Vec3D YB=PModObj->getLinkPt(2);
  const Geometry::Vec3D XA=PModObj->getLinkPt(3);
  const Geometry::Vec3D XB=PModObj->getLinkPt(4);
  const Geometry::Vec3D ZA=PModObj->getLinkPt(5);
  const Geometry::Vec3D ZB=PModObj->getLinkPt(6);
  
  const double DX=XA.Distance(XB)/2.0;
  const double DY=YA.Distance(YB);   // Not dividde
  const double DZ=ZA.Distance(ZB)/2.0;
  
  for(size_t i=0;i<4;i++)
    {
      SPlate.push_back
	(std::shared_ptr<insertSystem::insertPlate>
	 (new insertSystem::insertPlate
	  ("SPlate"+StrFunc::makeString(i))));

      if (i>1)
	SPlate[i]->setValues(5.0,H,DY,"Lead");
      else
	SPlate[i]->setValues(DY,H,5.0,"Lead");
      SPlate[i]->setStep(0,H/2.0,0);
      
      OR.addObject(SPlate[i]);
      SPlate[i]->addInsertCell(voidCell);
      SPlate[i]->createAll(System,*PModObj,static_cast<long int>(i+3));
    }
  return;
}
void 
makePhoton2::build(Simulation& System,
		  const mainSystem::inputParam& IParam)
/*!
    Carry out the full build
    \param System :: Simulation system
    \param IParam :: Input parameters
   */
{
  // For output stream
  ELog::RegMethod RControl("makePhoton2","build");

  int voidCell(74123);

  Chamber->addInsertCell(voidCell);
  Chamber->createAll(System,World::masterOrigin(),0);

  
  PModObj->addInsertCell(Chamber->getCell("Void",0));
  PModObj->createAll(System,*Chamber,0);
  buildWings(System);

  Catcher->setNoInsert();  
  Catcher->addInsertCell(Chamber->getCell("Void",0));
  Catcher->createAll(System,*PModObj,1);

  BaseSupport->addInsertCell(Chamber->getCell("Void"));
  BaseSupport->createAll(System,*Chamber,0);

  centralSupport->addInsertCell(voidCell);
  centralSupport->createAll(System,*PModObj,-1);

  //  centralTubes->addInsertCell(voidCell);
  centralTubes->addInsertCell(centralSupport->getCell("Main"));
  centralTubes->createAll(System,*centralSupport,0);
  

  return;
}


}   // NAMESPACE photonSystem

