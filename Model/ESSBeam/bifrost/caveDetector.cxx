/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   singleItem/CryoMagnetBase.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
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
#include "CellMap.h"
#include "SurfMap.h"
#include "SurInter.h"
#include "surfDIter.h"


#include "support.h"


#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "LayerComp.h"
#include "ContainedComp.h"
#include "SpaceCut.h"
//#include "ContainedSpace.h"
#include "ContainedGroup.h"
#include "surfDBase.h"
#include "mergeTemplate.h"
#include "surfDivide.h"
/*


#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "surfDivide.h"
#include "surfDIter.h"
#include "HeadRule.h"
#include "Object.h"

#include "objectGroups.h"

#include "groupRange.h"

#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "support.h"
#include "stringCombine.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h"
#include "ContainedComp.h"
#include "surfDBase.h"
#include "mergeTemplate.h"
*/
#include "cryoSample.h"
#include "CryoMagnetBase.h"
#include "caveDetector.h"

namespace simpleCaveSystem
{

      
caveDetector::caveDetector(const std::string& Key) :
  attachSystem::FixedOffset(Key,6),attachSystem::ContainedComp(),
  attachSystem::CellMap(),attachSystem::SurfMap()
  //  layerIndex(ModelSupport::objectRegister::Instance().cell(Key)), 
  //  cellIndex(layerIndex+1)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

  

caveDetector::caveDetector(const caveDetector& A) : 
  attachSystem::FixedOffset(A),attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),
  //  buildIndex(A.buildIndex),cellIndex(A.cellIndex),
  innerRad(A.innerRad), shieldThick(A.shieldThick),
  tankIntRad(A.tankIntRad),tankThick(A.tankThick),
  tankExtRad(A.tankExtRad),shelfWidth(A.shelfWidth),shelfThick(A.shelfThick),
  tankDepth(A.tankDepth),tankCutHeight(A.tankCutHeight),
  tankMat(A.tankMat),shieldMat(A.shieldMat),
  nShieldLayers(A.nShieldLayers),nTankLayers(A.nTankLayers)
{}

caveDetector&
caveDetector::operator=(const caveDetector& A)
  /*!
    Assignment operator
    \param A :: caveDetector to copy
    \return *this
  */
  {
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      cellIndex=A.cellIndex;
      innerRad=A.innerRad;
      shieldThick=A.shieldThick;
      tankIntRad=A.tankIntRad;
      tankThick=A.tankThick;
      tankExtRad=A.tankExtRad;
      shelfThick=A.shelfThick;
      shelfWidth=A.shelfWidth;
      tankDepth=A.tankDepth;
      tankCutHeight=A.tankCutHeight;
      tankMat=A.tankMat;
      shieldMat=A.shieldMat;
      nShieldLayers=A.nShieldLayers;
      nTankLayers=A.nTankLayers;
    }
  return *this;
}
  

  
  
caveDetector::~caveDetector()
  /*!
    Destructor
  */
{}


void
caveDetector::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("caveDetector","populate");

  FixedOffset::populate(Control);

  ELog::EM << keyName << ELog::endDiag;

      innerRad=Control.EvalVar<double>(keyName+"InnerRadius");
      shieldThick=Control.EvalVar<double>(keyName+"ShieldingThickness");
      tankIntRad=Control.EvalVar<double>(keyName+"TankInternalRadius");
      tankThick=Control.EvalVar<double>(keyName+"TankWallThickness");
      tankExtRad=Control.EvalVar<double>(keyName+"TankExternalRadius");

      innerRad= tankIntRad - shieldThick;
      
      shelfWidth=Control.EvalVar<double>(keyName+"ShelfWidth");
      shelfThick=Control.EvalVar<double>(keyName+"ShelfThick");
      tankDepth=Control.EvalVar<double>(keyName+"TankDepth");
      tankCutHeight=Control.EvalVar<double>(keyName+"CutoutHeight");
      tankMat=ModelSupport::EvalMat<int>(Control,keyName+"TankWallMat");
      shieldMat=ModelSupport::EvalMat<int>(Control,keyName+"ShieldMat");
  
      nShieldLayers=Control.EvalVar<size_t>(keyName+"ShieldingNLayers");
      nTankLayers=Control.EvalVar<size_t>(keyName+"TankNLayers");
  ModelSupport::populateDivide(Control,nShieldLayers,
			       keyName+"ShieldMat",
			       shieldMat,shieldMatList);
  ModelSupport::populateDivideLen(Control,nShieldLayers,
				  keyName+"ShieldingLayerThick",
				  shieldThick,
  				  shieldFracList);

  ModelSupport::populateDivide(Control,nTankLayers,
			       keyName+"TankMat",
			       tankMat,tankMatList);
  ModelSupport::populateDivideLen(Control,nTankLayers,
				  keyName+"TankLayerThick",
				  tankThick,
  				  tankFracList);

     
  return;
}

void
caveDetector::createUnitVector(const attachSystem::FixedComp& FC,
			   const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("caveDetector","createUnitVector");

  const attachSystem::LinkUnit& LU=FC.getLU(sideIndex);
      attachSystem::FixedComp::createUnitVector(FC,sideIndex);
      applyOffset();
  /* if ((sideIndex!=0)&&(sideIndex<=4)) yStep+=holderOuterRadius;
     applyOffset();*/
  
 
  return;
}

void
caveDetector::createSurfaces()
  /*!
    Create cylinders and planes for the sample holder and the sample
  */
{
  ELog::RegMethod RegA("caveDetector","createSurfaces");
  //Shielding plate surfaces
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Z,innerRad);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Z,innerRad+shieldThick);
  //Tank surface
  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,Z,tankIntRad);
  ModelSupport::buildCylinder(SMap,buildIndex+37,Origin,Z,tankIntRad+tankThick);
  ModelSupport::buildCylinder(SMap,buildIndex+47,Origin,Z,tankIntRad+shelfWidth);
  ModelSupport::buildCylinder(SMap,buildIndex+28,Origin,Z,tankExtRad);
  ModelSupport::buildCylinder(SMap,buildIndex+38,Origin,Z,tankExtRad-tankThick);
  //Tank top and bottom
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(0.5*tankCutHeight+tankThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(tankDepth),Z);
  //Tank inner dimensions
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(0.5*tankCutHeight),Z);
  ModelSupport::buildPlane(SMap,buildIndex+35,Origin-Z*(0.5*tankCutHeight+shelfThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(tankDepth-tankThick),Z);
  //Planes for cut out
  ModelSupport::buildPlane(SMap,buildIndex+26,Origin+Z*(0.5*tankCutHeight),Z);
  ModelSupport::buildPlane(SMap,buildIndex+25,Origin-Z*(0.5*tankCutHeight),Z);
  
  //Planes cutting sector
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin+(Y+X*1.50)*tankThick/sqrt(3.25),Y+X*1.50);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin-(Y-X*1.50)*tankThick/sqrt(3.25),Y-X*1.50);
  ModelSupport::buildPlane(SMap,buildIndex+11,Origin,Y+X*1.50);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin,Y-X*1.50);
  //planes along the beam
  ModelSupport::buildPlane(SMap,buildIndex+99,Origin,X);
   ModelSupport::buildPlane(SMap,buildIndex+98,Origin-X*10.0,X);

  return; 
}

void
caveDetector::createObjects(Simulation& System)
  /*!
    Create sample holder and sample cells
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("caveDetector","createObjects");

  std::string Out;
    //Inner tank void
    Out=ModelSupport::getComposite(SMap,buildIndex,
           "-38 37 -11 12 -26 15 (47: -37 : 11 : -12 : 25 : -35) ");
    //				   "-38 47 -11 12 -26 15 : -47 37 -11 12 -35 15 ");
    System.addCell(MonteCarlo::Object(cellIndex++,0.0,0.0,Out));
    CellMap::setCell("InnerVoid",cellIndex-1);
    //Shelf
    Out=ModelSupport::getComposite(SMap,buildIndex,
				   " -47 37 -11 12 -25 35");
    System.addCell(MonteCarlo::Object(cellIndex++,tankMat,0.0,Out));
    CellMap::setCell("Shelf",cellIndex-1);

    /*Tank walls*/
    Out=ModelSupport::getComposite(SMap,buildIndex,
    "-98 -28 27 -1 2 -6 5 (38: -37 : 11: -12 : 26 :-15)");
    System.addCell(MonteCarlo::Object(cellIndex++,tankMat,0.0,Out));
    CellMap::setCell("TankWalls",cellIndex-1);

     Out=ModelSupport::getComposite(SMap,buildIndex,"-98 -17 7 -1 2 -25 5");
    System.addCell(MonteCarlo::Object(cellIndex++,shieldMat,0.0,Out));
    CellMap::setCell("ShieldingPlate",cellIndex-1);
    //Cut out
    Out=ModelSupport::getComposite(SMap,buildIndex,"-17 7 -26  25 -11 12");
    System.addCell(MonteCarlo::Object(cellIndex++,0.0,0.0,Out));
    CellMap::setCell("DetectorCutOut",cellIndex-1);
    //Front cut
        Out=ModelSupport::getComposite(SMap,buildIndex,"-27 7 -6  26 -11 12 -98");
    System.addCell(MonteCarlo::Object(cellIndex++,0.0,0.0,Out));
    CellMap::setCell("DetectorFrontCut",cellIndex-1);

    Out=ModelSupport::getComposite(SMap,buildIndex,"-27 7 -6  25 11 -1 -98");
    System.addCell(MonteCarlo::Object(cellIndex++,0.0,0.0,Out));
    CellMap::setCell("DetectorFrontCut",cellIndex-1);

    
    Out=ModelSupport::getComposite(SMap,buildIndex,"-27 7 -6  25 -12 2 -98");
    System.addCell(MonteCarlo::Object(cellIndex++,0.0,0.0,Out));
    CellMap::setCell("DetectorFrontCut",cellIndex-1);

    //Outer dimensions
    Out=ModelSupport::getComposite(SMap,buildIndex,"-98 -28 7 -1 2 -6 5");
  
      
  addOuterSurf(Out); // border will be what is out of cylinder above
    

  return; 
}

void
caveDetector::createLinks()
  /*!
    Creates a full attachment set
   Link points and link surfaces
 */
{  
  ELog::RegMethod RegA("caveDetector","createLinks");
  //Link point at the sample position
  //Along the beam
  /*
  FixedComp::setConnect(0,Origin-Y*(holderOuterRadius),-Y);
  FixedComp::setLinkSurf(0,SMap.realSurf(buildIndex+7));
  FixedComp::setConnect(1,Origin+Y*(holderOuterRadius),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+7));
  //Perp to beam
  FixedComp::setConnect(2,Origin-X*(holderOuterRadius),-X);
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+7));
  FixedComp::setConnect(3,Origin+X*(holderOuterRadius),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+7));
  //Vertical dimensions of sample holder
  FixedComp::setConnect(4,Origin+Z*(holderTop),-Z);
  FixedComp::setLinkSurf(4,SMap.realSurf(buildIndex+5));
  FixedComp::setConnect(5,Origin-Z*(holderBott),Z);
  FixedComp::setLinkSurf(5,-SMap.realSurf(buildIndex+6));

  */
  
  return;
}

void 
caveDetector::layerProcess(Simulation& System)
  /*!
    Processes the splitting of the surfaces into a multilayer system
    \param System :: Simulation to work on
  */
{
  ELog::RegMethod RegA("caveDetector","layerProcess");
  // Wall layers
  //  layerSpecial(System);

  if (nShieldLayers>1)
    {
      std::string OutA,OutB;
      ModelSupport::surfDivide DA; 
            
      for(size_t i=1;i<nShieldLayers;i++)
	{
	  DA.addFrac(shieldFracList[i-1]);
	  DA.addMaterial(shieldMatList[i-1]);
	}
      DA.addMaterial(shieldMatList.back());
      // Cell Specific:
      DA.setCellN(CellMap::getCell("ShieldingPlate"));
      DA.setOutNum(cellIndex,buildIndex+1001);

      ModelSupport::mergeTemplate<Geometry::Cylinder,
				  Geometry::Cylinder> surroundRule;
      surroundRule.setSurfPair(SMap.realSurf(buildIndex+17),
			       SMap.realSurf(buildIndex+7));
      OutA=ModelSupport::getComposite(SMap,buildIndex," -17 ");
      OutB=ModelSupport::getComposite(SMap,buildIndex," 7 ");

      surroundRule.setInnerRule(OutA);
      surroundRule.setOuterRule(OutB);
      DA.addRule(&surroundRule);
      DA.activeDivideTemplate(System);

      cellIndex=DA.getCellNum();
    }

  if (nTankLayers>1)
    {
      std::string OutAC,OutBC;
      std::string OutAP,OutBP;
      ModelSupport::surfDivide DA;
                  
      for(size_t i=1;i<nTankLayers;i++)
	{
	  DA.addFrac(tankFracList[i-1]);
	  DA.addMaterial(tankMatList[i-1]);
	}
      DA.addMaterial(tankMatList.back());
      // Cell Specific:
      DA.setCellN(CellMap::getCell("TankWalls"));
   
      DA.setOutNum(cellIndex,buildIndex+1001);
    // inner and outer rules for parts of the tank defined by either cylinders or planes
    OutAC=ModelSupport::getComposite(SMap,buildIndex," (38: -37)");
    OutBC=ModelSupport::getComposite(SMap,buildIndex," -28 27  ");
    OutAP=ModelSupport::getComposite(SMap,buildIndex,"(11 : -12 :26 : -15)");
    OutBP=ModelSupport::getComposite(SMap,buildIndex,  " -1 2 -6 5 ");
    // std::cout << "Step 1" << std::endl;
      
      ModelSupport::mergeTemplate<Geometry::Plane,
				  Geometry::Plane> surroundRulePl;
      surroundRulePl.setSurfPair(SMap.realSurf(buildIndex+15),
			       SMap.realSurf(buildIndex+5));
      surroundRulePl.setSurfPair(SMap.realSurf(buildIndex+26),
			       SMap.realSurf(buildIndex+6));
      surroundRulePl.setSurfPair(SMap.realSurf(buildIndex+11),
			       SMap.realSurf(buildIndex+1));
      surroundRulePl.setSurfPair(SMap.realSurf(buildIndex+12),
			       SMap.realSurf(buildIndex+2));
    
      ModelSupport::mergeTemplate<Geometry::Cylinder,
				  Geometry::Cylinder> surroundRuleCyl;
      surroundRuleCyl.setSurfPair(SMap.realSurf(buildIndex+37),
			       SMap.realSurf(buildIndex+27));
      surroundRuleCyl.setSurfPair(SMap.realSurf(buildIndex+38),
			       SMap.realSurf(buildIndex+28));
      
      surroundRuleCyl.setInnerRule(OutAC);
      surroundRuleCyl.setOuterRule(OutBC);
      DA.addRule(&surroundRuleCyl); // registering splitting for cylindrical surfaces
      surroundRulePl.setInnerRule(OutAP);
      surroundRulePl.setOuterRule(OutBP);
      DA.addRule(&surroundRulePl); //registering splitting for planes
      DA.activeDivideTemplate(System);
      cellIndex=DA.getCellNum();
    }

  return;
}



  
void
caveDetector::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
    \param sideIndex :: surface offset
   */
{
  ELog::RegMethod RegA("caveDetector","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       
  layerProcess(System);

  return;
}

}  // NAMESPACE constructSystem
