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

  frontPlateGap=Control.EvalVar<double>(keyName+"FrontPlateGap");
  frontPlateWidth=Control.EvalVar<double>(keyName+"FrontPlateWidth");
  frontPlateHeight=Control.EvalVar<double>(keyName+"FrontPlateHeight");
  
  supVOffset=Control.EvalVar<double>(keyName+"SupVOffset");
  supLength=Control.EvalVar<double>(keyName+"SupLength");
  supXOut=Control.EvalVar<double>(keyName+"SupXOut");
  supThick=Control.EvalVar<double>(keyName+"SupThick");
  supEdge=Control.EvalVar<double>(keyName+"SupEdge");
  supHoleRadius=Control.EvalVar<double>(keyName+"SupHoleRadius");
  springConnectLen=Control.EvalVar<double>(keyName+"SpringConnectLen");

  elecXOut=Control.EvalVar<double>(keyName+"ElecXOut");
  elecLength=Control.EvalVar<double>(keyName+"ElecLength");
  elecHeight=Control.EvalVar<double>(keyName+"ElecHeight");
  elecThick=Control.EvalVar<double>(keyName+"ElecThick");
  elecEdge=Control.EvalVar<double>(keyName+"ElecEdge");
  elecHoleRadius=Control.EvalVar<double>(keyName+"ElecHoleRadius");

  baseMat=ModelSupport::EvalMat<int>(Control,keyName+"BaseMat");
  supportMat=ModelSupport::EvalMat<int>(Control,keyName+"SupportMat");
  springMat=ModelSupport::EvalMat<int>(Control,keyName+"SpringMat");
  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");

  electronMat=ModelSupport::EvalMat<int>(Control,keyName+"ElectronMat");
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

  // Extent
  makeShiftedSurf(SMap,"Base",buildIndex+25,Z,-(clearGap+cupHeight));
  makeShiftedSurf(SMap,"Top",buildIndex+26,Z,1.34+clearGap+cupHeight);
  ModelSupport::buildPlane
    (SMap,buildIndex+124,Origin+X*(topExtent-clearGap-extentThick),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+224,Origin+X*(baseExtent-clearGap-extentThick),X);

  // OutSide EXTERT
  const double extra(4.0);
  makeShiftedSurf(SMap,"Base",buildIndex+35,Z,-(extra+clearGap+cupHeight));
  makeShiftedSurf(SMap,"Top",buildIndex+36,Z,extra+clearGap+cupHeight);
  ModelSupport::buildPlane
    (SMap,buildIndex+134,Origin+X*(extra+topExtent-clearGap-extentThick),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+244,Origin+X*(extra+baseExtent-clearGap-extentThick),X);


  // STOP SURFACE:
  ModelSupport::buildPlane(SMap,buildIndex+501,Origin-Y*(supLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+502,Origin+Y*(supLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+504,Origin+X*supXOut,X);
  ModelSupport::buildPlane(SMap,buildIndex+514,Origin+X*(supXOut-supThick),X);
  // top
  ModelSupport::buildPlane
    (SMap,buildIndex+511,Origin-Y*(supLength/2.0-supThick),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+512,Origin+Y*(supLength/2.0-supThick),Y);
  makeShiftedSurf(SMap,"Top",buildIndex+505,Z,supVOffset);
  makeShiftedSurf(SMap,"Top",buildIndex+506,Z,supVOffset+supEdge);
  ModelSupport::buildPlane
    (SMap,buildIndex+513,Origin+X*(supThick+topExtent-clearGap),X);
  makeShiftedSurf(SMap,"Top",buildIndex+515,Z,supVOffset+supThick);


  // cross supports
  ModelSupport::buildPlane(SMap,buildIndex+551,
			   Origin-Y*(supLength/2.0-springConnectLen),Y);
  ModelSupport::buildPlane(SMap,buildIndex+571,
			   Origin+Y*(supLength/2.0-springConnectLen),Y);

  
  // base
  makeShiftedSurf(SMap,"Base",buildIndex+606,Z,-supVOffset);
  makeShiftedSurf(SMap,"Base",buildIndex+605,Z,-(supVOffset+supEdge));
  ModelSupport::buildPlane
    (SMap,buildIndex+613,Origin+X*(supThick+baseExtent-clearGap),X);
  makeShiftedSurf(SMap,"Base",buildIndex+616,Z,-(supVOffset+supThick));

  

  // ELECTRON shield
  ModelSupport::buildPlane(SMap,buildIndex+1001,Origin-Y*(elecLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+1002,Origin+Y*(elecLength/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+1003,Origin+X*(elecXOut-elecThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+1004,Origin+X*elecXOut,X);

  ModelSupport::buildPlane(SMap,buildIndex+1005,Origin-Z*(elecHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+1006,Origin+Z*(elecHeight/2.0),Z);


  ModelSupport::buildPlane
    (SMap,buildIndex+1013,Origin+X*(elecXOut-elecEdge),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+1015,Origin-Z*(elecHeight/2.0-elecThick),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+1016,Origin+Z*(elecHeight/2.0-elecThick),Z);

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
  
  // support (Top):
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"501 -502 104 -504 505 -506 (-511:512:-513:514:-515)");
  makeCell("TopSupport",System,cellIndex++,supportMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"511 -512 513 -514 515 -506");
  makeCell("TSupportVoid",System,cellIndex++,voidMat,0.0,HR);

  // support (Base):
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"501 -502 204 -504 605 -606 (-613:514:616)");
  makeCell("BaseSupport",System,cellIndex++,supportMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"501 -502 613 -514 -616 605");
  makeCell("BSupportVoid",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"1001 -1002 1013 -1004 1005 -1006 (-1015:1016:1003)");
  makeCell("EPlate",System,cellIndex++,electronMat,0.0,HR);  
    
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"1001 -1002 1013 -1003 1015 -1016");
  makeCell("EPlate",System,cellIndex++,voidMat,0.0,HR);  
  
  // OUTER VOIDS:
  // main c voids
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 13 -224 25 -15");
  makeCell("OuterVoid",System,cellIndex++,voidMat,0.0,HR);  
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 13 -124 16 -26");
  makeCell("OuterVoid",System,cellIndex++,voidMat,0.0,HR);  
  //  addOuterUnionSurf(HR);
  // spring voids
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 104 -1004 506 -26");
  makeCell("OuterVoid",System,cellIndex++,voidMat,0.0,HR);  
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 204 -1004 25 -605");
  makeCell("OuterVoid",System,cellIndex++,voidMat,0.0,HR);  

  // top void
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 504 -1004 605 -1005");
  makeCell("OuterVoid",System,cellIndex++,voidMat,0.0,HR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 504 -1004 1006 -506");
  makeCell("OuterVoid",System,cellIndex++,voidMat,0.0,HR);  

  // void at electron plate
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -1001 1013 -1004 1005 -1006");
  makeCell("OuterVoid",System,cellIndex++,voidMat,0.0,HR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1002 -2 1013 -1004 1005 -1006");
  makeCell("OuterVoid",System,cellIndex++,voidMat,0.0,HR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -501 104 -504 505 -506");
  makeCell("OuterVoid",System,cellIndex++,voidMat,0.0,HR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"502 -2 104 -504 505 -506");
  makeCell("OuterVoid",System,cellIndex++,voidMat,0.0,HR);  

  
  // crystal ends
  HR=HeadRule(SMap,buildIndex,-2);
  makeCell("OuterVoid",System,cellIndex++,voidMat,0.0,
	   HR*backCompHR*mirrorCompHR*farHR*topCompHR*baseCompHR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 2005 -2006 2004");  
  makeCell("OuterVoid",System,cellIndex++,voidMat,0.0,
	   HR*mirrorCompHR*nearHR);

  // INNER VOIDS:

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -501 605 -606 204 -504");
  makeCell("InnerVoid",System,cellIndex++,voidMat,0.0,HR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"502 -2 605 -606 204 -504");
  makeCell("InnerVoid",System,cellIndex++,voidMat,0.0,HR);  

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"1 -2 606 -505 -1013");
  makeCell("InnerVoid",System,cellIndex++,voidMat,0.0,HR*mirrorHR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2 -505 104 ");
  makeCell("InnerVoid",System,cellIndex++,voidMat,0.0,
	   HR*mirrorCompHR*topHR*nearCompHR);  
    
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2 606 204");
  makeCell("InnerVoid",System,cellIndex++,voidMat,0.0,
	   HR*mirrorCompHR*baseHR*nearCompHR);  


  // small gaps between spring centres and electron plate:
  // cross support at 501/502

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -501 1006 -505 1013 -504");
  makeCell("InnerVoid",System,cellIndex++,voidMat,0.0,HR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"501 -551 1006 -505 1013 -504");
  makeCell("InnerVoid",System,cellIndex++,springMat,0.0,HR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"551 -571 1006 -505 1013 -504");
  makeCell("InnerVoid",System,cellIndex++,voidMat,0.0,HR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"571 -502 1006 -505 1013 -504");
  makeCell("InnerVoid",System,cellIndex++,springMat,0.0,HR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"502 -2 1006 -505 1013 -504");
  makeCell("InnerVoid",System,cellIndex++,voidMat,0.0,HR);  

  // lower cross support
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -501 606 -1005 1013 -504");
  makeCell("InnerVoid",System,cellIndex++,voidMat,0.0,HR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"501 -551 606 -1005 1013 -504");
  makeCell("InnerVoid",System,cellIndex++,springMat,0.0,HR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"551 -571 606 -1005 1013 -504");
  makeCell("InnerVoid",System,cellIndex++,voidMat,0.0,HR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"571 -502 606 -1005 1013 -504");
  makeCell("InnerVoid",System,cellIndex++,springMat,0.0,HR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"502 -2 606 -1005 1013 -504");
  makeCell("InnerVoid",System,cellIndex++,voidMat,0.0,HR);  

  // front heat shield plate
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"2001 3 -2004 2005 -2006");
  makeCell("HeatGap",System,cellIndex++,voidMat,0.0,HR*nearHR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2001 3 -2004 2005 -2006");
  makeCell("HeatShield",System,cellIndex++,frontMat,0.0,HR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 3 -104 -6 2006");
  makeCell("HeatVoid",System,cellIndex++,voidMat,0.0,HR*nearHR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 3 -204 5 -2005");
  makeCell("HeatVoid",System,cellIndex++,voidMat,0.0,HR*nearHR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 104 -505 2006");
  makeCell("InnerVoid",System,cellIndex++,voidMat,0.0,
	   HR*nearHR*mirrorCompHR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 204 606 -2005");
  makeCell("InnerVoid",System,cellIndex++,voidMat,0.0,
	   HR*nearHR*mirrorCompHR);  

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 13 -1004 25 -26");
  addOuterSurf(HR);

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
  ELog::EM<<"Oring "<<Origin-Y*(length/2.0)<<ELog::endDiag;
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));

  FixedComp::setConnect(1,Origin+Y*(length/2.0),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));

  FixedComp::setConnect(2,Origin+X*(elecXOut-elecThick),X);
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+1003));


  FixedComp::setConnect(3,Origin+X*elecXOut,X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+1004));

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
