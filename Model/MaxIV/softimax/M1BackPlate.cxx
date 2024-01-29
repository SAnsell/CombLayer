/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   softimax/M1BackPlate.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Exception.h"
#include "surfRegister.h"
#include "BaseVisit.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "M1BackPlate.h"

namespace xraySystem
{

M1BackPlate::M1BackPlate(const std::string& Key) :
  attachSystem::FixedRotate(Key,8),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap()
  /*!
    Constructor
    \param Key :: Name of construction key
    \param Index :: Index number
  */
{}

M1BackPlate::~M1BackPlate()
  /*!
    Destructor
   */
{}

void
M1BackPlate::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("M1BackPlate","populate");

  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  clearGap=Control.EvalVar<double>(keyName+"ClearGap");
  backThick=Control.EvalVar<double>(keyName+"BackThick");
  mainThick=Control.EvalVar<double>(keyName+"MainThick");
  cupHeight=Control.EvalVar<double>(keyName+"CupHeight");
  topExtent=Control.EvalVar<double>(keyName+"TopExtent");
  extentThick=Control.EvalVar<double>(keyName+"ExtentThick");
  baseExtent=Control.EvalVar<double>(keyName+"BaseExtent");
  midExtent=Control.EvalVar<double>(keyName+"MidExtent");

  frontPlateGap=Control.EvalVar<double>(keyName+"FrontPlateGap");
  frontPlateWidth=Control.EvalVar<double>(keyName+"FrontPlateWidth");
  frontPlateHeight=Control.EvalVar<double>(keyName+"FrontPlateHeight");

  baseMat=ModelSupport::EvalMat<int>(Control,keyName+"BaseMat");
  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");

  frontMat=ModelSupport::EvalMat<int>(Control,keyName+"FrontMat");
  return;
}

void
M1BackPlate::createSurfaces()
  /*!
    Create planes for mirror block and support
  */
{
  ELog::RegMethod RegA("M1BackPlate","createSurfaces");

  if (!isActive("Top") || !isActive("Base") ||
      !isActive("Back") || !isActive("Mirror"))
    throw ColErr::InContainerError<std::string>
      ("Top/Base/Back","Surface not defined");

  // mid divider
  ModelSupport::buildPlane(SMap,buildIndex+100,Origin,Z);
  
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*clearGap,X);
  makeShiftedSurf(SMap,"Base",buildIndex+5,Z,-clearGap);
  makeShiftedSurf(SMap,"Top",buildIndex+6,Z,clearGap);

  ModelSupport::buildPlane
    (SMap,buildIndex+104,Origin+X*(topExtent-clearGap),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+204,Origin+X*(baseExtent-clearGap),X);

  // plate layer
  ModelSupport::buildPlane
    (SMap,buildIndex+13,Origin-X*(clearGap+backThick),X);
  makeShiftedSurf(SMap,"Base",buildIndex+15,Z,-(clearGap+mainThick));
  makeShiftedSurf(SMap,"Top",buildIndex+16,Z,clearGap+mainThick);
  SurfMap::addSurf("LowBaseAttach",SMap.realSurf(buildIndex+15));
  SurfMap::addSurf("TopBaseAttach",SMap.realSurf(buildIndex+16));
  
  // Extent
  makeShiftedSurf(SMap,"Base",buildIndex+25,Z,-(clearGap+cupHeight));
  makeShiftedSurf(SMap,"Base",buildIndex+125,Z,-(midExtent+clearGap+cupHeight));
  makeShiftedSurf(SMap,"Top",buildIndex+26,Z,clearGap+cupHeight);
  makePlane("TopSideAttach",SMap,buildIndex+124,
	    Origin+X*(topExtent-clearGap-extentThick),X);
  makePlane("LowSideAttach",SMap,buildIndex+224,
	    Origin+X*(baseExtent-clearGap-extentThick),X);


  // front plate
  makeShiftedSurf(SMap,"NearEnd",buildIndex+2001,Y,-frontPlateGap);
  ModelSupport::buildPlane
    (SMap,buildIndex+2004,Origin+X*(frontPlateWidth-clearGap),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+2005,Origin-Z*(frontPlateHeight/2.0),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+2006,Origin+Z*(frontPlateHeight/2.0),Z);

  return;
}

void
M1BackPlate::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("M1BackPlate","createObjects");


  const HeadRule topHR=getRule("Top");
  const HeadRule baseHR=getRule("Base");
  const HeadRule backHR=getRule("Back");
  const HeadRule mirrorHR=getRule("Mirror");
  const HeadRule nearHR=getRule("NearEnd");
  const HeadRule farHR=getRule("FarEnd");

  const HeadRule topCompHR=topHR.complement();
  const HeadRule baseCompHR=baseHR.complement();
  const HeadRule backCompHR=backHR.complement();
  const HeadRule mirrorCompHR=mirrorHR.complement();
  const HeadRule nearCompHR=nearHR.complement();
  
  const HeadRule tbUnionHR=topHR+backHR;
  const HeadRule bbUnionHR=baseHR+backHR;
  const HeadRule tubeHR=getRule("TubeRadius");

  HeadRule HR;

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"100 -2 3 -104 -6");
  makeCell("PlateGap",System,cellIndex++,voidMat,0.0,
	   HR*tbUnionHR*nearCompHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"100 1 -2 13 -104 -16 (6:-3)");
  makeCell("Plate",System,cellIndex++,baseMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 16 -26 124 -104");
  makeCell("Plate",System,cellIndex++,baseMat,0.0,HR);
  
  // lower section:
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-100 -2 3 -204 5");
  makeCell("PlateGap",System,cellIndex++,voidMat,0.0,HR*bbUnionHR*nearCompHR); 

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-100 1 -2 13 -204 15 (-5:-3)");
  makeCell("Plate",System,cellIndex++,baseMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -15 25 224 -204");
  makeCell("Plate",System,cellIndex++,baseMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -25 125 224 -204");
  makeCell("PlateVoid",System,cellIndex++,voidMat,0.0,HR);
  
  // OUTER VOIDS:
  // main c voids
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 13 -224 125 -15");
  makeCell("OuterVoid",System,cellIndex++,voidMat,0.0,HR*tubeHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 13 -124 16 -26");
  makeCell("OuterVoid",System,cellIndex++,voidMat,0.0,HR);  
  //  addOuterUnionSurf(HR);
  
  // crystal ends
  HR=HeadRule(SMap,buildIndex,-2);
  makeCell("OuterVoid",System,cellIndex++,voidMat,0.0,
	   HR*backCompHR*mirrorCompHR*farHR*topCompHR*baseCompHR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 2005 -2006 2004");  
  makeCell("OuterVoid",System,cellIndex++,voidMat,0.0,
	   HR*mirrorCompHR*nearHR);

  // INNER VOIDS:
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2 -26 104 ");
  makeCell("InnerVoid",System,cellIndex++,voidMat,0.0,
	   HR*mirrorCompHR*topHR*nearCompHR);
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2 125 204");
  makeCell("BaseVoid",System,cellIndex++,voidMat,0.0,
	   HR*mirrorCompHR*baseHR*nearCompHR*tubeHR);
  // small gaps between spring centres and electron plate:
  // cross support at 501/502

  // front heat shield plate
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"2001 3 -2004 2005 -2006");
  makeCell("HeatGap",System,cellIndex++,voidMat,0.0,HR*nearHR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2001 3 -2004 2005 -2006");
  makeCell("HeatShield",System,cellIndex++,frontMat,0.0,HR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 3 -104 -6 2006");
  makeCell("HeatVoid",System,cellIndex++,voidMat,0.0,HR*nearHR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 3 -204 5 -2005");
  makeCell("HeatVoid",System,cellIndex++,voidMat,0.0,HR*nearHR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2005 125 204 ");
  makeCell("HeatVoid",System,cellIndex++,voidMat,0.0,HR*nearHR*mirrorCompHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 2006 -26 104 ");
  makeCell("HeatVoid",System,cellIndex++,voidMat,0.0,HR*nearHR*mirrorCompHR);
  ELog::EM<<"Cells == "<<keyName<<" "<<cellIndex-1<<" "
	  <<nearCompHR<<" "<<baseHR<<ELog::endDiag;

  if (isActive("TubeRadius"))
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 125 -26 -13");
      makeCell("BackVoid",System,cellIndex++,voidMat,0.0,HR*tubeHR);
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 125 -26");
      addOuterSurf(HR*mirrorCompHR);
    }
  else
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 13 125 -26");
      addOuterSurf(HR*mirrorCompHR);
    }
  
  return;
}

void
M1BackPlate::joinRing(Simulation& System,
		      const HeadRule& fbHR,
		      const HeadRule& ringCyl)
  /*!
    Construct join to ring
    \param fbHR :: front/back of the ring system
    \param fbHR :: ringCyl :: inner ring of the systemx
   */
{
  ELog::RegMethod RegA("M1BackPlate","joinRing");

  HeadRule HR;
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-224 -15 25 13");
  makeCell("LowRingJoin",System,cellIndex++,
	   baseMat,0.0,HR*fbHR*ringCyl);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-224 -25 125");
  makeCell("LowRingJoinVoid",System,cellIndex++,
	   voidMat,0.0,HR*fbHR*ringCyl);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-124 16 -26 13");

  makeCell("TopRingJoin",System,cellIndex++,
	   baseMat,0.0,HR*fbHR*ringCyl);

  insertComponent(System,"OuterVoid",0,fbHR.complement());
  insertComponent(System,"OuterVoid",1,fbHR.complement());

  HR=fbHR.complement()+ringCyl;

  CellMap::insertComponent(System,"PlateVoid",HR);
  CellMap::insertComponent(System,"BaseVoid",HR);

  return;  
}

void
M1BackPlate::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("M1BackPlate","createLinks");

  FixedComp::setConnect(0,Origin-Y*(length/2.0),-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));

  FixedComp::setConnect(1,Origin+Y*(length/2.0),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));

  const HeadRule mirrorHR=getRule("Mirror");
  FixedComp::setConnect(2,Origin,X);
  FixedComp::setLinkSurf(2,mirrorHR);

  FixedComp::setConnect(3,Origin,X);
  FixedComp::setLinkSurf(3,mirrorHR);

  FixedComp::setLinkSurf(4,SMap.realSurf(buildIndex+5));
  FixedComp::setLineConnect(4,Origin,Z);

  FixedComp::setLinkSurf(5,-SMap.realSurf(buildIndex+6));
  FixedComp::setLineConnect(5,Origin,-Z);

  FixedComp::setLinkSurf(6,SMap.realSurf(buildIndex+3));
  FixedComp::setLineConnect(6,Origin,X);

  nameSideIndex(2,"elecShieldIn");
  nameSideIndex(3,"elecShieldOut");
  nameSideIndex(4,"innerBase");
  nameSideIndex(5,"innerTop");
  nameSideIndex(6,"innerSide");



  
  // link points are defined in the end of createSurfaces

  return;
}

void
M1BackPlate::createAll(Simulation& System,
		  const attachSystem::FixedComp& FC,
		  const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp to construct from
    \param sideIndex :: Side point
   */
{
  ELog::RegMethod RegA("M1BackPlate","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE xraySystem
