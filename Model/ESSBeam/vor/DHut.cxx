/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   vor/DHut.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include <array>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "surfRegister.h"
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
#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"

#include "DHut.h"

namespace essSystem
{

DHut::DHut(const std::string& Key) : 
  attachSystem::FixedOffsetGroup(Key,"Inner",6,"Mid",6,"Outer",6),
  attachSystem::ContainedComp(),attachSystem::CellMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

DHut::~DHut() 
  /*!
    Destructor
  */
{}

void
DHut::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("DHut","populate");
  
  FixedOffsetGroup::populate(Control);

  // Void + Fe special:
  voidHeight=Control.EvalVar<double>(keyName+"VoidHeight");
  voidWidth=Control.EvalVar<double>(keyName+"VoidWidth");
  voidDepth=Control.EvalVar<double>(keyName+"VoidDepth");
  voidLength=Control.EvalVar<double>(keyName+"VoidLength");
  voidFrontCut=Control.EvalVar<double>(keyName+"VoidFrontCut");
  voidFrontStep=Control.EvalVar<double>(keyName+"VoidFrontStep");
  voidBackCut=Control.EvalVar<double>(keyName+"VoidBackCut");
  voidBackStep=Control.EvalVar<double>(keyName+"VoidBackStep");

  feThick=Control.EvalVar<double>(keyName+"FeThick");
  concThick=Control.EvalVar<double>(keyName+"ConcThick");

  feMat=ModelSupport::EvalMat<int>(Control,keyName+"FeMat");
  concMat=ModelSupport::EvalMat<int>(Control,keyName+"ConcMat");

  return;
}

void
DHut::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("DHut","createUnitVector");

  // add nosecone + half centre
  yStep+=voidLength/2.0+concThick+feThick;

  attachSystem::FixedComp& Outer=getKey("Outer");
  attachSystem::FixedComp& Mid=getKey("Mid");
  attachSystem::FixedComp& Inner=getKey("Inner");

  Outer.createUnitVector(FC,sideIndex);
  Mid.createUnitVector(FC,sideIndex);
  Inner.createUnitVector(FC,sideIndex);
  applyOffset();
  setDefault("Inner");
  return;
}

void
DHut::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("DHut","createSurfaces");

  // Inner void
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(voidLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(voidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*voidDepth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*voidHeight,Z);  

  
  // DCut:
  Geometry::Vec3D APt(Origin-Y*(voidLength/2.0)+X*voidFrontCut);
  Geometry::Vec3D ASidePt(Origin+X*(voidWidth/2.0)-Y*voidFrontStep);
  Geometry::Vec3D BPt(Origin+Y*voidLength/2.0+X*voidBackCut);
  Geometry::Vec3D BSidePt(Origin+X*(voidWidth/2.0)+Y*voidBackStep);

  ModelSupport::buildPlane(SMap,buildIndex+11,APt,ASidePt,APt+Z,X);
  ModelSupport::buildPlane(SMap,buildIndex+12,BPt,BSidePt,BPt+Z,X);


  // FE WALLS
  ModelSupport::buildPlane(SMap,buildIndex+101,
			   Origin-Y*(voidLength/2.0+feThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+102,
			   Origin+Y*(voidLength/2.0+feThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+103,
			   Origin-X*(voidWidth/2.0+feThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+104,
			   Origin+X*(voidWidth/2.0+feThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+105,
			   Origin-Z*(voidDepth+feThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+106,
			   Origin+Z*(voidHeight+feThick),Z);  

  

  const Geometry::Plane* PA=
    SMap.realPtr<Geometry::Plane>(buildIndex+11);
  const Geometry::Plane* PB=
    SMap.realPtr<Geometry::Plane>(buildIndex+12);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+111,PA,feThick);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+112,PB,feThick);

  
  // CONC WALLS
  ModelSupport::buildPlane(SMap,buildIndex+201,
			   Origin-Y*(voidLength/2.0+feThick+concThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+202,
			   Origin+Y*(voidLength/2.0+feThick+concThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+203,
			   Origin-X*(voidWidth/2.0+feThick+concThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+204,
			   Origin+X*(voidWidth/2.0+feThick+concThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+205,
			   Origin-Z*(voidDepth+feThick+concThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+206,
			   Origin+Z*(voidHeight+feThick+concThick),Z);  

  

  ModelSupport::buildShiftedPlane(SMap,buildIndex+211,PA,concThick+feThick);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+212,PB,concThick+feThick);
  
  return;
}

void
DHut::createObjects(Simulation& System)
  /*!
    Adds the main objects
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("DHut","createObjects");

  std::string Out;
  
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 3 -4 5 -6 -11 -12 ");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));
  setCell("Void",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "101 -102 103 -104 105 -106  -111 -112 "
  				 "(-1:2:-3:4:-5: 6 :11 : 12)");
  System.addCell(MonteCarlo::Object(cellIndex++,feMat,0.0,Out));

  setCell("Steel",cellIndex-1);
    
  Out=ModelSupport::getComposite(SMap,buildIndex,
				   "201 -202 203 -204 205 -206 -211 -212 "
				 "(-101:102:-103:104:-105: 106 :111 :112)");
  System.addCell(MonteCarlo::Object(cellIndex++,concMat,0.0,Out));
  setCell("Concrete",cellIndex-1);
  
  
  // Exclude:
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "201 -202 203 -204 205 -206 -211 -212 ");
  addOuterSurf(Out);      

  return;
}

void
DHut::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("DHut","createLinks");

  attachSystem::FixedComp& innerFC=FixedGroup::getKey("Inner");
  attachSystem::FixedComp& midFC=FixedGroup::getKey("Mid");
  attachSystem::FixedComp& outerFC=FixedGroup::getKey("Outer");

  // INNER VOID
  innerFC.setConnect(0,Origin-Y*(voidLength/2.0),-Y);
  innerFC.setConnect(1,Origin+Y*(voidLength/2.0),Y);
  innerFC.setConnect(2,Origin-X*(voidWidth/2.0),-X);
  innerFC.setConnect(3,Origin+X*(voidWidth/2.0),X);
  innerFC.setConnect(4,Origin-Z*voidDepth,-Z);
  innerFC.setConnect(5,Origin+Z*voidHeight,Z);  

  innerFC.setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  innerFC.setLinkSurf(1,SMap.realSurf(buildIndex+2));
  innerFC.setLinkSurf(2,-SMap.realSurf(buildIndex+3));
  innerFC.setLinkSurf(3,SMap.realSurf(buildIndex+4));
  innerFC.setLinkSurf(4,-SMap.realSurf(buildIndex+5));
  innerFC.setLinkSurf(5,SMap.realSurf(buildIndex+6));

  
    // OUTER VOID
  double TThick(feThick);
  midFC.setConnect(0,Origin-Y*(TThick+voidLength/2.0),-Y);
  midFC.setConnect(1,Origin+Y*(TThick+voidLength/2.0),Y);
  midFC.setConnect(2,Origin-X*(TThick+voidWidth/2.0),-X);
  midFC.setConnect(3,Origin+X*(TThick+voidWidth/2.0),X);
  midFC.setConnect(4,Origin-Z*(TThick+voidDepth),-Z);
  midFC.setConnect(5,Origin+Z*(TThick+voidHeight),Z);  

  midFC.setLinkSurf(0,-SMap.realSurf(buildIndex+101));
  midFC.setLinkSurf(1,SMap.realSurf(buildIndex+102));
  midFC.setLinkSurf(2,-SMap.realSurf(buildIndex+103));
  midFC.setLinkSurf(3,SMap.realSurf(buildIndex+104));
  midFC.setLinkSurf(4,-SMap.realSurf(buildIndex+105));
  midFC.setLinkSurf(5,SMap.realSurf(buildIndex+106));

  // OUTER VOID
  TThick+=concThick;
  outerFC.setConnect(0,Origin-Y*(TThick+voidLength/2.0),-Y);
  outerFC.setConnect(1,Origin+Y*(TThick+voidLength/2.0),Y);
  outerFC.setConnect(2,Origin-X*(TThick+voidWidth/2.0),-X);
  outerFC.setConnect(3,Origin+X*(TThick+voidWidth/2.0),X);
  outerFC.setConnect(4,Origin-Z*(TThick+voidDepth),-Z);
  outerFC.setConnect(5,Origin+Z*(TThick+voidHeight),Z);  

  outerFC.setLinkSurf(0,-SMap.realSurf(buildIndex+201));
  outerFC.setLinkSurf(1,SMap.realSurf(buildIndex+202));
  outerFC.setLinkSurf(2,-SMap.realSurf(buildIndex+203));
  outerFC.setLinkSurf(3,SMap.realSurf(buildIndex+204));
  outerFC.setLinkSurf(4,-SMap.realSurf(buildIndex+205));
  outerFC.setLinkSurf(5,SMap.realSurf(buildIndex+206));

  
  return;
}

void
DHut::createAll(Simulation& System,
		const attachSystem::FixedComp& FC,
		const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("DHut","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE essSystem
