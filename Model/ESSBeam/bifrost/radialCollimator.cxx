/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   simpleCave/radialCollimator.cxx
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
#include "SurInter.h"
#include "SurfMap.h"
#include "surfDIter.h"


#include "support.h"


#include "FixedGroup.h"
#include "FixedOffsetGroup.h"
#include "LayerComp.h"
#include "ContainedComp.h"
#include "SpaceCut.h"
#include "surfDivide.h"
//#include "ContainedSpace.h"
#include "ContainedGroup.h"
#include "surfDBase.h"
#include "mergeTemplate.h"
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
#include "Qhull.h"
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
#include "radialCollimator.h"

namespace simpleCaveSystem
{

radialCollimator::radialCollimator(const std::string& Key) :
  attachSystem::FixedOffset(Key,6),attachSystem::ContainedComp(),
  attachSystem::CellMap(),attachSystem::SurfMap()
  //  layerIndex(ModelSupport::objectRegister::Instance().cell(Key)), 
  //  cellIndex(layerIndex+1)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

  

 radialCollimator::radialCollimator(const radialCollimator& A) : 
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),attachSystem::CellMap(A),
  //  buildIndex(A.buildIndex),cellIndex(A.cellIndex),
  innerRad(A.innerRad),
  middleRad(A.middleRad),
  outerRad(A.outerRad),
  height(A.height),
  bladeAngle(A.bladeAngle),
  bladeThick(A.bladeThick),
  startAngle(A.startAngle),
  endAngle(A.endAngle),
  bladeMat(A.bladeMat),
  filterMat(A.filterMat)
  
{}

radialCollimator&
radialCollimator::operator=(const radialCollimator& A)
  /*!
    Assignment operator
    \param A :: radialCollimator to copy
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
      middleRad=A.middleRad;
      outerRad=A.outerRad;
      height=A.height;
      bladeAngle=A.bladeAngle;
      bladeThick=A.bladeThick;
      startAngle=A.startAngle;
      endAngle=A.endAngle;
      bladeMat=A.bladeMat;
      filterMat=A.filterMat;
    }
  return *this;
}
  

  
  
radialCollimator::~radialCollimator()
  /*!
    Destructor
  */
{}


void
radialCollimator::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("radialCollimator","populate");

  FixedOffset::populate(Control);

  ELog::EM << keyName << ELog::endDiag;

      innerRad=Control.EvalVar<double>(keyName+"InnerRadius");
      middleRad=Control.EvalVar<double>(keyName+"MiddleRadius");
      outerRad=Control.EvalVar<double>(keyName+"OuterRadius");
      height=Control.EvalVar<double>(keyName+"Height");
      bladeAngle=3.14159256/180.0*Control.EvalVar<double>(keyName+"BladeAngle");
      bladeThick=Control.EvalVar<double>(keyName+"BladeThick");
      startAngle=3.14159256/180.0*Control.EvalVar<double>(keyName+"StartAngle");
      endAngle=3.14159256/180.0*Control.EvalVar<double>(keyName+"EndAngle");
     bladeMat=ModelSupport::EvalMat<int>(Control,keyName+"BladeMat");
     filterMat=ModelSupport::EvalMat<int>(Control,keyName+"FilterMat");
  
  return;
}

void
radialCollimator::createUnitVector(const attachSystem::FixedComp& FC,
			   const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("radialCollimator","createUnitVector");

  //  const attachSystem::LinkUnit& LU=FC.getLU(sideIndex);
      attachSystem::FixedComp::createUnitVector(FC,sideIndex);
      applyOffset();
  /* if ((sideIndex!=0)&&(sideIndex<=4)) yStep+=holderOuterRadius;
     applyOffset();*/
  
 
  return;
}

void
radialCollimator::createSurfaces()
  /*!
    Create cylinders and planes for the sample holder and the sample
  */
{
  ELog::RegMethod RegA("radialCollimator","createSurfaces");
  //Cylindrical part
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Z,innerRad);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Z,middleRad);
  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,Z,outerRad);
  //Starting angle
  ModelSupport::buildPlane
    (SMap,buildIndex+1,
     Origin, Y*cos(startAngle)+X*sin(startAngle));
  ModelSupport::buildPlane
    (SMap,buildIndex+2,
     Origin, Y*cos(endAngle)+X*sin(endAngle));  
  //Blade planes
  int bladeOffset = 100;
  //  int isign = (startAngle>0)?1:-1;
  int bladeCount=0;
  for(int i=(int) (startAngle/bladeAngle); i<(endAngle/bladeAngle); i++){
    double angle=i*bladeAngle;
    //    std::cout << angle << std::endl;
    ModelSupport::buildPlane
    (SMap,buildIndex+bladeOffset+2*bladeCount,
     Origin-(Y*cos(angle)+X*sin(angle))*0.5*bladeThick,
     Y*cos(angle)+X*sin(angle));
  ModelSupport::buildPlane
    (SMap,buildIndex+bladeOffset+2*bladeCount+1,
     Origin+(Y*cos(angle)+X*sin(angle))*0.5*bladeThick,
     Y*cos(angle)+X*sin(angle));
  bladeCount++;
  }
  //Top and bottom
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height*0.5),Z);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height*0.5),Z);
  //planes along the beam
  ModelSupport::buildPlane(SMap,buildIndex+99,Origin,X);
    return; 
}

void
radialCollimator::createObjects(Simulation& System)
  /*!
    Create sample holder and sample cells
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("radialCollimator","createObjects");

  std::string Out;
 //Blades and sides
  int bladeOffset=100;
  Out=ModelSupport::getComposite(SMap,buildIndex,"-"+std::to_string(bladeOffset)+" 1 -6 5 7 -27");
  System.addCell(MonteCarlo::Object(cellIndex++,bladeMat,0.0,Out));
    CellMap::setCell("Void",cellIndex-1);

  //blades
  
  int bladeCount=0;
  
for(int i=(int) (startAngle/bladeAngle); i<(endAngle/bladeAngle); i++){
      Out=
    ModelSupport::getComposite(SMap,buildIndex,
  "-99 -6 5 -27 7 "+std::to_string(bladeOffset+2*bladeCount)+" -"
			       +std::to_string(bladeOffset+2*bladeCount+1));
  System.addCell(MonteCarlo::Object(cellIndex++,bladeMat,0.0,Out));
    CellMap::setCell("Blade",cellIndex-1);
    bladeCount++;
  }
  
 
  Out=ModelSupport::getComposite(SMap,buildIndex,
	      std::to_string(bladeOffset+2*bladeCount-1)+" -2 -6 5 7 -27");
  System.addCell(MonteCarlo::Object(cellIndex++,bladeMat,0.0,Out));
    CellMap::setCell("Void",cellIndex-1);

    //voids between
 bladeCount=0;
  for(int i=(int) (startAngle/bladeAngle); i<(endAngle/bladeAngle)-1; i++){
      Out=
    ModelSupport::getComposite(SMap,buildIndex,
  "-99 -6 5 -17 7 "+std::to_string(bladeOffset+2*bladeCount+1)+" -"
			       +std::to_string(bladeOffset+2*bladeCount+2));
  System.addCell(MonteCarlo::Object(cellIndex++,0.0,0.0,Out));
    CellMap::setCell("Void",cellIndex-1);
      Out=
    ModelSupport::getComposite(SMap,buildIndex,
  "-99 -6 5 -27 17 "+std::to_string(bladeOffset+2*bladeCount+1)+" -"
			       +std::to_string(bladeOffset+2*bladeCount+2));
  System.addCell(MonteCarlo::Object(cellIndex++,filterMat,0.0,Out));
    CellMap::setCell("BeFilter",cellIndex-1);
    bladeCount++;
  }
 


  Out=ModelSupport::getComposite(SMap,buildIndex,"-99 -27 7 1 -2 -6 5");
  
      
  addOuterSurf(Out); // border will be what is out of cylinder above
    

  return; 
}

void
radialCollimator::createLinks()
  /*!
    Creates a full attachment set
   Link points and link surfaces
 */
{  
  ELog::RegMethod RegA("radialCollimator","createLinks");
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

  /*!
    Processes the splitting of the surfaces into a multilayer system
    \param System :: Simulation to work on
  */
  /*
void 
radialCollimator::layerProcess(Simulation& System)
{
  ELog::RegMethod RegA("radialCollimator","layerProcess");
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
  return;
}
  */


  
void
radialCollimator::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
    \param sideIndex :: surface offset
   */
{
  ELog::RegMethod RegA("radialCollimator","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       
  //  layerProcess(System);
  return;
}

}  // NAMESPACE constructSystem
