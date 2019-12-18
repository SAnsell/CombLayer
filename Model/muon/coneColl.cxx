/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   muon/coneColl.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include "HeadRule.h"
#include "Object.h"
#include "SimProcess.h"
#include "SurInter.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "ContainedComp.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "coneColl.h"

namespace muSystem
{

coneColl::coneColl(const std::string& Key)  : 
  attachSystem::FixedRotate(Key,2),
  attachSystem::ContainedComp()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Key to use
  */
{}


coneColl::~coneColl() 
  /*!
    Destructor
  */
{}

void
coneColl::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase
  */
{
  ELog::RegMethod RegA("coneColl","populate");

  FixedRotate::populate(Control);
  
  outRadius=Control.EvalVar<double>(keyName+"OutRadius");
  inRadius=Control.EvalVar<double>(keyName+"InRadius");   
  radiusStartCone=Control.EvalVar<double>(keyName+"RadiusStartCone");       
  radiusStopCone=Control.EvalVar<double>(keyName+"RadiusStopCone");  
  length=Control.EvalVar<double>(keyName+"Length");     

  tubeMat=ModelSupport::EvalMat<int>(Control,keyName+"TubeMat");
  innerMat=ModelSupport::EvalMat<int>(Control,keyName+"InnerMat");
         
  return;
}

void
coneColl::createSurfaces()
  /*!
    Create all the surfaces
  */
{
  ELog::RegMethod RegA("coneColl","createSurface");

  const double angleC=180.0*atan((radiusStopCone-radiusStartCone)/length)/M_PI; 
  const double offset=radiusStartCone/tan(angleC*M_PI/180.0);

  //  :
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length,Y);
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,outRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,inRadius);  
  ModelSupport::buildCone(SMap,buildIndex+27,Origin-Y*offset,Y,angleC);   


  return;
}

void
coneColl::createObjects(Simulation& System)
  /*!
    Adds the Cone colimator components
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("coneColl","createObjects");
  
  std::string Out;

     // tube material
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 -7 ");
  addOuterSurf(Out);
  addBoundarySurf(Out);
  Out+=ModelSupport::getComposite(SMap,buildIndex,"17 ");  
  System.addCell(MonteCarlo::Object(cellIndex++,tubeMat,0.0,Out));

     // collimator material
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 -17 27 ");
  System.addCell(MonteCarlo::Object(cellIndex++,innerMat,0.0,Out));

    // hole
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 -27 ");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));
  
  return;
}


void
coneColl::createLinks()
  /*!
    Create links
   */
{
  ELog::RegMethod RegA("coneColl","createLinks");

  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setConnect(1,Origin+Y*length,Y);

  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));  

  return;
}

void
coneColl::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Create the shutter
    \param System :: Simulation to process
    \param FC :: Origin for system
    \param sideIndex :: Link point
  */
{
  ELog::RegMethod RegA("coneColl","createAll");
  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);
  return;
}
  
}  // NAMESPACE muSystem
