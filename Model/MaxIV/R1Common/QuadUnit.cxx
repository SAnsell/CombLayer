/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/QuadUnit.cxx
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
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ExternalCut.h" 
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h" 

#include "Quadrupole.h"
#include "QuadUnit.h"

namespace xraySystem
{

QuadUnit::QuadUnit(const std::string& Key) : 
  attachSystem::FixedOffset(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  quadX(new xraySystem::Quadrupole(Key+"QuadX")),
  quadZ(new xraySystem::Quadrupole(Key+"QuadZ"))

  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(quadX);
  OR.addObject(quadZ);

}


QuadUnit::~QuadUnit() 
  /*!
    Destructor
  */
{}

void
QuadUnit::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("QuadUnit","populate");

  FixedOffset::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  inWidth=Control.EvalVar<double>(keyName+"InWidth");
  ringWidth=Control.EvalVar<double>(keyName+"RingWidth");
  outPointWidth=Control.EvalVar<double>(keyName+"OutPointWidth");
  height=Control.EvalVar<double>(keyName+"Height");

  endGap=Control.EvalVar<double>(keyName+"EndGap");
  endLength=Control.EvalVar<double>(keyName+"EndLength");

  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  
  flangeARadius=
    Control.EvalTail<double>(keyName,"FlangeARadius","FlangeRadius");
  flangeBRadius=
    Control.EvalTail<double>(keyName,"FlangeBRadius","FlangeRadius");
  flangeALength=
    Control.EvalTail<double>(keyName,"FlangeALength","FlangeLength");
  flangeBLength=
    Control.EvalTail<double>(keyName,"FlangeBLength","FlangeLength");

  voidMat=ModelSupport::EvalDefMat<int>(Control,keyName+"VoidMat",0);
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  flangeMat=ModelSupport::EvalMat<int>(Control,keyName+"FlangeMat");

  return;
}

void
QuadUnit::createUnitVector(const attachSystem::FixedComp& FC,
    	                     const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: FixedComp to attach to
    \param sideIndex :: Link point
  */
{
  ELog::RegMethod RegA("QuadUnit","createUnitVector");
  
  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  return;
}

void
QuadUnit::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("QuadUnit","createSurface");

  if (!ExternalCut::isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
      setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  if (!ExternalCut::isActive("back"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length,Y);
      setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }


  
  // low/left counter clockwise points (starting from
  // outer point

  const Geometry::Vec3D AX(Origin-X*outPointWidth);
  const Geometry::Vec3D BX(Origin-X*inWidth+Z*(height/2.0));
  const Geometry::Vec3D CX(Origin+X*inWidth+Z*(height/2.0));
  const Geometry::Vec3D DX(Origin+X*ringWidth+Z*(endGap/2.0));
  const Geometry::Vec3D EX(Origin+X*ringWidth-Z*(endGap/2.0));
  const Geometry::Vec3D FX(Origin+X*inWidth-Z*(height/2.0));
  const Geometry::Vec3D GX(Origin-X*inWidth-Z*(height/2.0));

  
  
  // all inward
  ModelSupport::buildPlane(SMap,buildIndex+11,AX,AX+Y*length,BX,X);
  ModelSupport::buildPlane(SMap,buildIndex+12,BX,BX+Y*length,CX,-Z);
  ModelSupport::buildPlane(SMap,buildIndex+13,CX,CX+Y*length,DX,-X);
  ModelSupport::buildPlane(SMap,buildIndex+14,DX,DX+Y*length,EX,-X);
  ModelSupport::buildPlane(SMap,buildIndex+15,EX,EX+Y*length,FX,-X);
  ModelSupport::buildPlane(SMap,buildIndex+16,FX,FX+Y*length,GX,Z);
  ModelSupport::buildPlane(SMap,buildIndex+17,GX,GX+Y*length,AX,X);

  int BI(buildIndex+11);
  for(size_t i=0;i<7;i++)			     
    {
      ModelSupport::buildExpandedSurf(SMap,BI,BI+10,
				      Origin+Y*(length/2.0),wallThick);
      BI++;      
    }

  // front flange:
  ExternalCut::makeShiftedSurf(SMap,"front",buildIndex+101,1,Y,flangeALength);
  ExternalCut::makeShiftedSurf(SMap,"back",buildIndex+102,-1,Y,flangeBLength);

  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,flangeARadius);
  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,Y,flangeBRadius);

  
  return;
}

void
QuadUnit::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("QuadUnit","createObjects");

  const std::string frontStr=getRuleStr("front");
  const std::string backStr=getRuleStr("back");
  const std::string fbStr=frontStr+backStr;
  //
  std::string Out;
  Out=ModelSupport::getComposite
    (SMap,buildIndex,"  11 12 13 14 15 16 17 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out+fbStr);

  // not cutting at 14.
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 21 22 23 14 25 26 27 (-11:-12:-13:-15:-16:-17) ");
  makeCell("Outer",System,cellIndex++,wallMat,0.0,Out+fbStr);

  // Flanges
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " -101 -107 (-21:-22:-23:-14:-25:-26:-27) ");
  makeCell("FlangeA",System,cellIndex++,flangeMat,0.0,Out+frontStr);
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " 102 -207 (-21:-22:-23:-14:-25:-26:-27) ");
  makeCell("FlangeB",System,cellIndex++,flangeMat,0.0,Out+backStr);


  Out=ModelSupport::getComposite(SMap,buildIndex,"  21 22 23 14 25 26 27 ");
  addOuterUnionSurf(Out+fbStr);
  
  Out=ModelSupport::getComposite(SMap,buildIndex," -101 -107 ");
  addOuterUnionSurf(Out+frontStr);
  Out=ModelSupport::getComposite(SMap,buildIndex," 102 -207 ");
  addOuterUnionSurf(Out+backStr);


  return;
}

void 
QuadUnit::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("QuadUnit","createLinks");
  ExternalCut::createLink("front",*this,0,Origin,Y);
  ExternalCut::createLink("back",*this,1,Origin,Y);

  const Geometry::Vec3D midPt((getLinkPt(1)+getLinkPt(2))/2.0);

  // note : no surface
  FixedComp::setConnect(2,midPt,Y);
  FixedComp::nameSideIndex(2,"midPoint");
  
  return;
}

void
QuadUnit::createQuads(Simulation& System,const int cellN)
  /*!
    Separate function [will be joined later as the full 
    shell is completed.
    \param System :: Simulation :: System to insert
    \param cellN :: Cell for insertion
   */
{
  ELog::RegMethod RegA("QuadUnit","createAll");
  std::string Out;
  
  for(const std::shared_ptr<Quadrupole>& QItem : {quadX,quadZ})
    {
      QItem->addInsertCell(cellN);
      QItem->createAll(System,*this,3);

      // Insert Quad Cut into void space

      Out=ModelSupport::getComposite(SMap,buildIndex," (-26:-25:-14) "); 
      QItem->insertComponent(System,"VoidPoleA",Out);
      
      Out=ModelSupport::getComposite(SMap,buildIndex," (-26 : -27) "); 
      QItem->insertComponent(System,"VoidPoleB",Out);
      
      
      Out=ModelSupport::getComposite(SMap,buildIndex," (-22:-23:-14) "); 
      QItem->insertComponent(System,"VoidPoleC",Out);
      
      Out=ModelSupport::getComposite(SMap,buildIndex," (-21:-22) "); 
      QItem->insertComponent(System,"VoidPoleD",Out);
      
      if (QItem->CellMap::hasItem("ExtraPoleVoidA"))
	{
	  Out=ModelSupport::getComposite(SMap,buildIndex,
					 "(-21:-22:-23:-14:-25:-26:-27)");
	  QItem->insertComponent(System,"ExtraPoleVoidA",Out);
	  QItem->insertComponent(System,"ExtraPoleVoidB",Out);
	}
    }

  //
  // Split void to give vacuum cell for magnetic field
  //
  std::string frontStr=getRuleStr("front");
  std::string backStr=getRuleStr("back");
  // Delete old cell and reconstruct front/back [note make a split cell]
  CellMap::deleteCell(System,"Void");

  Out=ModelSupport::getComposite
	(SMap,buildIndex,"  11 12 13 14 15 16 17 ");
  
  for(const std::shared_ptr<Quadrupole>& QItem : {quadX,quadZ})
    {
      const HeadRule fHR=QItem->getSurfRule("FrontQuadPole");
      const HeadRule bHR=QItem->getSurfRule("BackQuadPole");
      makeCell("Void",System,cellIndex++,voidMat,0.0,Out+frontStr+
	       fHR.complement().display());
      makeCell("QuadVoid",System,cellIndex++,voidMat,0.0,Out+
	       fHR.display()+bHR.display());
      frontStr=bHR.complement().display();
    }
  // make exit cell
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out+frontStr+
	   backStr);

  return;
}


  
void
QuadUnit::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track 
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("QuadUnit","createAll");
  
  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE xraySystem
