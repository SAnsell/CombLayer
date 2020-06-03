/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Linac/CeramicSep.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include "surfDIter.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "SurInter.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "SimProcess.h"
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
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h"

#include "CeramicSep.h"

namespace tdcSystem
{

CeramicSep::CeramicSep(const std::string& Key) :
  attachSystem::FixedOffset(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::FrontBackCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}


CeramicSep::~CeramicSep()
  /*!
    Destructor
  */
{}

void
CeramicSep::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("CeramicSep","populate");

  FixedOffset::populate(Control);

  radius=Control.EvalVar<double>(keyName+"Radius");
  length=Control.EvalVar<double>(keyName+"Length");

  ceramicALen=Control.EvalVar<double>(keyName+"CeramicALen");
  ceramicWideLen=Control.EvalVar<double>(keyName+"CeramicWideLen");
  ceramicBLen=Control.EvalVar<double>(keyName+"CeramicBLen");
  ceramicThick=Control.EvalVar<double>(keyName+"CeramicThick");
  ceramicWideThick=Control.EvalVar<double>(keyName+"CeramicWideThick");

  pipeLen=Control.EvalVar<double>(keyName+"PipeLen");
  pipeThick=Control.EvalVar<double>(keyName+"PipeThick");

  bellowLen=Control.EvalVar<double>(keyName+"BellowLen");
  bellowThick=Control.EvalVar<double>(keyName+"BellowThick");

  flangeARadius=Control.EvalHead<double>
    (keyName,"FlangeARadius","FlangeRadius");
  flangeALength=Control.EvalHead<double>
    (keyName,"FlangeALength","FlangeLength");

  flangeBRadius=Control.EvalHead<double>
    (keyName,"FlangeBRadius","FlangeRadius");
  flangeBLength=Control.EvalHead<double>
    (keyName,"FlangeBLength","FlangeLength");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  pipeMat=ModelSupport::EvalMat<int>(Control,keyName+"PipeMat");
  ceramicMat=ModelSupport::EvalMat<int>(Control,keyName+"CeramicMat");
  bellowMat=ModelSupport::EvalMat<int>(Control,keyName+"BellowMat");
  flangeMat=ModelSupport::EvalMat<int>(Control,keyName+"FlangeMat");
  outerMat=ModelSupport::EvalMat<int>(Control,keyName+"OuterMat");
  
  return;
}


void
CeramicSep::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("CeramicSep","createSurfaces");

  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  if (!isActive("back"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
      ExternalCut::setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }

  // these can be made into reference to active front:
  Geometry::Vec3D aOrg(Origin-Y*(length/2.0));
  const Geometry::Vec3D bOrg(Origin+Y*(length/2.0));

  // flanges:
  ModelSupport::buildPlane(SMap,buildIndex+101,aOrg+Y*flangeALength,Y);
  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,flangeARadius);

  ModelSupport::buildPlane(SMap,buildIndex+201,bOrg-Y*flangeBLength,Y);
  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,Y,flangeBRadius);

  // ceramic 
  aOrg+=Y*(ceramicALen+flangeALength);
  ModelSupport::buildPlane(SMap,buildIndex+301,aOrg,Y);

  aOrg+=Y*ceramicWideLen;
  ModelSupport::buildPlane(SMap,buildIndex+311,aOrg,Y);

  aOrg+=Y*ceramicBLen;
  ModelSupport::buildPlane(SMap,buildIndex+321,aOrg,Y);

  // bellow
  aOrg+=Y*pipeLen;
  ModelSupport::buildPlane(SMap,buildIndex+501,aOrg,Y);

  // bellow
  aOrg+=Y*bellowLen;
  ModelSupport::buildPlane(SMap,buildIndex+502,aOrg,Y);

  // inner void
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);
  // close ceriamic
  ModelSupport::buildCylinder
    (SMap,buildIndex+307,Origin,Y,radius+ceramicThick);
  // wide ceriamic
  ModelSupport::buildCylinder
    (SMap,buildIndex+317,Origin,Y,radius+ceramicWideThick);

  // pipe
  ModelSupport::buildCylinder
    (SMap,buildIndex+407,Origin,Y,radius+pipeThick);

  // bellow
  ModelSupport::buildCylinder
    (SMap,buildIndex+507,Origin,Y,radius+bellowThick);

  return;
}

void
CeramicSep::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("CeramicSep","createObjects");

  std::string Out;

  const std::string frontStr=getRuleStr("front");
  const std::string backStr=getRuleStr("back");

  // inner void
  Out=ModelSupport::getComposite(SMap,buildIndex," -7 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out+frontStr+backStr);

  // front flange
  Out=ModelSupport::getComposite(SMap,buildIndex," 7 -101 -107 ");
  makeCell("FlangeA",System,cellIndex++,flangeMat,0.0,Out+frontStr);

  // Ceramic
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 7 101 -321 -317 ((301 -311) : -307 ) ");
  makeCell("Ceramic",System,cellIndex++,ceramicMat,0.0,Out);

  // pipe
  Out=ModelSupport::getComposite(SMap,buildIndex," 7 321 -501 -407 ");
  makeCell("Pipe",System,cellIndex++,pipeMat,0.0,Out);

  // bellow
  Out=ModelSupport::getComposite(SMap,buildIndex," 7 501 -502 -507 ");
  makeCell("Bellow",System,cellIndex++,bellowMat,0.0,Out);

  // final pipe
  Out=ModelSupport::getComposite(SMap,buildIndex," 7 502 -201 -407 ");
  makeCell("OutPipe",System,cellIndex++,pipeMat,0.0,Out);

  // back flange 
  Out=ModelSupport::getComposite(SMap,buildIndex," 7 201 -207 ");
  makeCell("FlangeB",System,cellIndex++,flangeMat,0.0,Out+backStr);

  // OUTER voids
  const int FIndex((flangeARadius>=flangeBRadius) ?
		   buildIndex : buildIndex+100);
	      
  Out=ModelSupport::getComposite    
    (SMap,buildIndex,FIndex," 101 -321 -107M (317:((311:-301) 307))");
  makeCell("OuterFlange",System,cellIndex++,outerMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,FIndex," -501 321 -107M 407");
  makeCell("OuterFlange",System,cellIndex++,outerMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,FIndex," 501 -502 -107M 507");
  makeCell("OuterFlange",System,cellIndex++,outerMat,0.0,Out);
  
  Out=ModelSupport::getComposite(SMap,buildIndex,FIndex," 502 -201 -107M 407");
  makeCell("OuterFlange",System,cellIndex++,outerMat,0.0,Out);
  
  Out=ModelSupport::getComposite(SMap,FIndex," -107 ");
  addOuterSurf(Out+frontStr+backStr);

  return;
}

void
CeramicSep::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("CeramicSep","createLinks");

  ExternalCut::createLink("front",*this,0,Origin,Y);  //front and back
  ExternalCut::createLink("back",*this,1,Origin,Y);  //front and back

  return;
}

void
CeramicSep::createAll(Simulation& System,
	       const attachSystem::FixedComp& FC,
	       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("CeramicSep","createAll");

  populate(System.getDataBase());
  createCentredUnitVector(FC,sideIndex,length);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE tdcSystem
