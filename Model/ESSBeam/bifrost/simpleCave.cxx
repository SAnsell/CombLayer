/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   pipeBuild/pipeTube.cxx
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
#include <array>



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
#include "simpleCave.h"



namespace simpleCaveSystem
{

simpleCave::simpleCave(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffsetGroup(Key,"Inner",6,"Outer",6)
  //  buildIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  //  cellIndex(buildIndex+1)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

simpleCave::simpleCave(const simpleCave& A) : 
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffsetGroup(A),attachSystem::CellMap(A),
  //  buildIndex(A.buildIndex),cellIndex(A.cellIndex),
  //cutRadius(A.cutRadius),
  innerLengthFront(A.innerLengthFront),innerLengthBack(A.innerLengthBack),
  innerHeight(A.innerHeight),innerDepth(A.innerDepth),
  innerWidthRight(A.innerWidthRight), innerWidthLeft(A.innerWidthLeft),
  /* hatchFront(A.hatchFront),hatchBack(A.hatchBack),hatchLeft(A.hatchLeft),
  hatchRight(A.hatchRight),
  lockThick(A.lockThick),lockCapThick(A.lockCapThick),lockCapNLayers(A.lockCapNLayers),
  */
  wallThick(A.wallThick), innerMat (A.innerMat), //beamstopMat (A.beamstopMat),
  wallMat(A.wallMat),nWallLayers(A.nWallLayers),
  wallFracList(A.wallFracList),wallMatList(A.wallMatList)
  /*!
    Copy constructor
    \param A :: simpleCave to copy
  */
{}

simpleCave&
simpleCave::operator=(const simpleCave& A)
  /*!
    Assignment operator
    \param A :: simpleCave to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffsetGroup::operator=(A);
      attachSystem::CellMap::operator=(A);
      //  cellIndex=A.cellIndex;
      innerLengthFront=A.innerLengthFront;
      innerLengthBack=A.innerLengthBack;
      innerHeight=A.innerHeight;
      innerDepth=A.innerDepth;
      innerHeight=A.innerHeight;
      innerWidthRight=A.innerWidthRight;
      innerWidthLeft=A.innerWidthLeft;
      /*      hatchFront=A.hatchFront;
      hatchLeft=A.hatchLeft;
      hatchRight=A.hatchRight;
      hatchBack=A.hatchBack;
      lockThick=A.lockThick;
      lockCapThick=A.lockCapThick;
      lockCapNLayers=A.lockCapNLayers;
      */ wallMat=A.wallMat;
      // cutRadius=A.cutRadius;
      innerMat=A.innerMat;
      // beamstopMat=A.beamstopMat;
      nWallLayers=A.nWallLayers;
      wallFracList=A.wallFracList;
      wallMatList=A.wallMatList;
    }
  return *this;
}

simpleCave::~simpleCave()
  /*!
    Destructor
  */
{}
  

void
simpleCave::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("simpleCave","populate");

  FixedOffsetGroup::populate(Control);

    iLayerFlag=Control.EvalDefVar<int>("LayerFlag",1);

  innerLengthFront=Control.EvalVar<double>(keyName+"LengthFront");
  innerLengthBack=Control.EvalVar<double>(keyName+"LengthBack");
  innerHeight=Control.EvalVar<double>(keyName+"Height");
  innerWidthRight=Control.EvalVar<double>(keyName+"WidthRight");
  innerDepth=Control.EvalVar<double>(keyName+"Depth");
  innerWidthLeft=Control.EvalVar<double>(keyName+"WidthLeft");
  /*  doglegWidth=Control.EvalVar<double>(keyName+"DoglegWidth");
  doglegDepth=Control.EvalVar<double>(keyName+"DoglegDepth");
  doglegLength=Control.EvalVar<double>(keyName+"DoglegLength");
  */
  /*
  lockThick=Control.EvalVar<double>(keyName+"LockThick");
  lockMat=ModelSupport::EvalMat<int>(Control,keyName+"LockMat");
  lockCapThick=Control.EvalVar<double>(keyName+"LockCapThick");
  lockCapMat=ModelSupport::EvalMat<int>(Control,keyName+"LockCapMat");
  lockCapNLayers=Control.EvalVar<size_t>(keyName+"LockCapNLayers");
  ModelSupport::populateDivide(Control,lockCapNLayers,
			       keyName+"CapLayerMat",
			       lockCapMat,capMatList);
  ModelSupport::populateDivideLen(Control,lockCapNLayers,
				  keyName+"CapLayerThick",
				  lockCapThick,
  				  capFracList);
  */

  roofExtraThick=Control.EvalVar<double>(keyName+"RoofExtraThick");
	wallThick=Control.EvalVar<double>(keyName+"WallThick");
	roofThick=Control.EvalVar<double>(keyName+"RoofThick");
	floorThick=Control.EvalVar<double>(keyName+"FloorThick");
	//  cutRadius=Control.EvalVar<double>(keyName+"CutRadius");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  innerMat=ModelSupport::EvalMat<int>(Control,keyName+"InnerMat");
  // beamstopMat=ModelSupport::EvalMat<int>(Control,keyName+"BeamstopMat");

  nWallLayers=Control.EvalVar<size_t>(keyName+"NWallLayers");
  ModelSupport::populateDivide(Control,nWallLayers,
			       keyName+"WLayerMat",
			       wallMat,wallMatList);
  ModelSupport::populateDivideLen(Control,nWallLayers,
				  keyName+"WLayerThick",
				  wallThick,
  				  wallFracList);
  

  nRoofLayers=Control.EvalVar<size_t>(keyName+"NRoofLayers");
  ModelSupport::populateDivide(Control,nRoofLayers,
			       keyName+"WLayerMat",
			       wallMat,roofMatList);
  ModelSupport::populateDivideLen(Control,nWallLayers,
				  keyName+"WLayerThick",
				  wallThick,
  				  roofFracList);
  
  return;
}

void
simpleCave::createUnitVector(const attachSystem::FixedComp& FC,
			   const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("simpleCave","createUnitVector");

  attachSystem::FixedComp& Outer=getKey("Outer");
  attachSystem::FixedComp& Inner=getKey("Inner");

  Outer.createUnitVector(FC,sideIndex);
  Inner.createUnitVector(FC,sideIndex);
  applyOffset();
  setDefault("Outer");
  
  //attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  //  yStep+=innerLengthFront+wallThick;
  //  FixedOffset::applyOffset();

  return;
}

void
simpleCave::createSurfaces()
  /*!
    Create planes for system
  */
{
  ELog::RegMethod RegA("simpleCave","createSurfaces");

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(innerLengthFront+wallThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(innerLengthBack+wallThick),Y);  
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(innerWidthRight+wallThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(innerWidthLeft+wallThick),X);  
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(innerDepth+floorThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(innerHeight+roofThick),Z);  


  ModelSupport::buildPlane(SMap,buildIndex+11,Origin-Y*(innerLengthFront),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(innerLengthBack),Y);  
  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(innerWidthRight),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(innerWidthLeft),X);  
  //For corners:
  ModelSupport::buildPlane(SMap,buildIndex+101,Origin-Y*(innerLengthFront),Y);
  ModelSupport::buildPlane(SMap,buildIndex+102,Origin+Y*(innerLengthBack),Y);  
  ModelSupport::buildPlane(SMap,buildIndex+103,Origin-X*(innerWidthRight),X);
  ModelSupport::buildPlane(SMap,buildIndex+104,Origin+X*(innerWidthLeft),X);  

  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(innerDepth),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(innerHeight),Z);

  if (roofExtraThick>0.1) {ModelSupport::buildPlane(SMap,buildIndex+216,
				    Origin+Z*(innerHeight+wallThick+roofExtraThick),Z);
    /*    //dogleg
    ModelSupport::buildPlane(SMap,buildIndex+226,
		 Origin+Z*(innerHeight+wallThick+roofExtraThick-doglegDepth),Z);
    //dogleg lining
    ModelSupport::buildPlane(SMap,buildIndex+326,
		 Origin+Z*(innerHeight+wallThick+roofExtraThick-doglegDepth-0.3),Z);
    */}
  else    {/*
    //dogleg
      ModelSupport::buildPlane(SMap,buildIndex+226,
		  Origin+Z*(innerHeight+wallThick+roofExtraThick-doglegDepth),Z);
    //dogleg lining
      ModelSupport::buildPlane(SMap,buildIndex+326,
		  Origin+Z*(innerHeight+wallThick+roofExtraThick-doglegDepth-0.3),Z);
	   */
  }
  /*
  //Hatch
  ModelSupport::buildPlane(SMap,buildIndex+21,Origin-Y*(hatchFront),Y);
  ModelSupport::buildPlane(SMap,buildIndex+22,Origin+Y*(hatchBack),Y);
  //dogleg
  ModelSupport::buildPlane(SMap,buildIndex+222,Origin+Y*(hatchBack+doglegLength),Y);
  //dogleg lining
  ModelSupport::buildPlane(SMap,buildIndex+322,Origin+Y*(hatchBack+doglegLength+0.3),Y);
  
  ModelSupport::buildPlane(SMap,buildIndex+23,Origin-X*(hatchRight),X);
  ModelSupport::buildPlane(SMap,buildIndex+24,Origin+X*(hatchLeft),X);  

  ModelSupport::buildPlane(SMap,buildIndex+31,Origin-Y*(hatchFront+10),Y);
  ModelSupport::buildPlane(SMap,buildIndex+32,Origin+Y*(hatchBack+10),Y);  
  ModelSupport::buildPlane(SMap,buildIndex+33,Origin-X*(hatchRight+10),X);
  ModelSupport::buildPlane(SMap,buildIndex+34,Origin+X*(hatchLeft+10),X);  
  ModelSupport::buildPlane(SMap,buildIndex+233,Origin-X*(0.5*doglegWidth),X);
  ModelSupport::buildPlane(SMap,buildIndex+234,Origin+X*(0.5*doglegWidth),X);  
  //dogleg lining
  ModelSupport::buildPlane(SMap,buildIndex+333,Origin-X*(0.5*doglegWidth+0.3),X);
  ModelSupport::buildPlane(SMap,buildIndex+334,Origin+X*(0.5*doglegWidth+0.3),X);  

  ModelSupport::buildPlane(SMap,buildIndex+401,Origin-Y*(hatchFront+40-lockCapThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+402,Origin+Y*(hatchBack+40-lockCapThick),Y);  
  ModelSupport::buildPlane(SMap,buildIndex+403,Origin-X*(hatchRight+40-lockCapThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+404,Origin+X*(hatchLeft+40-lockCapThick),X);

  ModelSupport::buildPlane(SMap,buildIndex+41,Origin-Y*(hatchFront+40),Y);
  ModelSupport::buildPlane(SMap,buildIndex+42,Origin+Y*(hatchBack+40),Y);  
  ModelSupport::buildPlane(SMap,buildIndex+43,Origin-X*(hatchRight+40),X);
  ModelSupport::buildPlane(SMap,buildIndex+44,Origin+X*(hatchLeft+40),X);
  //Hatch lining
  ModelSupport::buildPlane(SMap,buildIndex+201,Origin-Y*(hatchFront+0.3),Y);
  ModelSupport::buildPlane(SMap,buildIndex+202,Origin+Y*(hatchBack+0.3),Y);  
  ModelSupport::buildPlane(SMap,buildIndex+203,Origin-X*(hatchRight+0.3),X);
  ModelSupport::buildPlane(SMap,buildIndex+204,Origin+X*(hatchLeft+0.3),X);  
  
  
  //HatchLock
  ModelSupport::buildPlane(SMap,buildIndex+206,
       Origin+Z*(innerHeight+wallThick+lockThick +roofExtraThick-lockCapThick),Z);
  //Hatch lock cap
  ModelSupport::buildPlane(SMap,buildIndex+26,Origin+Z*(innerHeight+wallThick+lockThick+roofExtraThick),Z);
*/

  //CutOut for insert and beamstop  
  //  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,cutRadius);
 
  
  return; 
}

void
simpleCave::createObjects(Simulation& System)
  /*
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("simpleCave","createObjects");

  std::string Out;

  // Inner 
  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 13 -14 15 -16 ");
  System.addCell(MonteCarlo::Object(cellIndex++,innerMat,0.0,Out));

  CellMap::setCell("InnerSpace",cellIndex-1);

  
  //Hatch
  
  
int splane=6;
  
  Out=ModelSupport::getComposite(SMap,buildIndex,
 " 1 -2 3 -4  -6  16 (-11:12:-13:14:16) (-11:12:-13:14) "); 
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));
  CellMap::setCell("RoofOuter",cellIndex-1);
  

  Out=ModelSupport::getComposite(SMap,buildIndex,
 " 11 -12 13 -14 16 -6 ");
  
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));
  CellMap::setCell("RoofInner",cellIndex-1);


  Out=ModelSupport::getComposite(SMap,buildIndex,
 " 1 -2 3 -4 5 -15 "); //exclude dogleg
  
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));
  CellMap::setCell("Floor",cellIndex-1);

  

  Out=ModelSupport::getComposite(SMap,buildIndex,
 " 1 -11 13 -14 15 -16 "); //rear wall
  
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));
  CellMap::setCell("RearWall",cellIndex-1);
  
  Out=ModelSupport::getComposite(SMap,buildIndex,
 " 12 -2 13 -14 15 -16   "); //front wall
  
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));
  CellMap::setCell("FrontWall",cellIndex-1);
  
  
  Out=ModelSupport::getComposite(SMap,buildIndex,
 " 11 -12 -13 3 15 -16  "); //exclude dogleg
  
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));
  CellMap::setCell("LeftWall",cellIndex-1);

  
  Out=ModelSupport::getComposite(SMap,buildIndex,
 " 11 -12 14 -4 15 -16 "); //exclude dogleg
  
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));
  CellMap::setCell("RightWall",cellIndex-1);


  //Corners cave  
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 // "( 14 : -11 )  (14 1 -4 15 -16 -11 )");
    " (-11:12:-13:14) 1 -2 3 -4 15 -16 -11 14"); 
  
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));
  CellMap::setCell("CornerA",cellIndex-1);
  
  Out=ModelSupport::getComposite(SMap,buildIndex,
 "  (-11:12:-13:14) 1 -2 3 -4  12 14  15 -16 "); //exclude dogleg
  
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));
  CellMap::setCell("CornerB",cellIndex-1);



  Out=ModelSupport::getComposite(SMap,buildIndex,
 "   (-11:12:-13:14) 1 -2 3 -4 -11  -13 15 -16  "); //exclude dogleg
  
  
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));
  CellMap::setCell("CornerC",cellIndex-1);
 
  

  Out=ModelSupport::getComposite(SMap,buildIndex,
 "  (-11:12:-13:14) 1 -2 3 -4 12 -13  15 -16  "); //exclude dogleg
   
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));
  CellMap::setCell("CornerD",cellIndex-1);
 

  
  
  
  Out=
  ModelSupport::getComposite(SMap,buildIndex," 1 -2 3 -4 5 "+std::to_string(-splane));
			       //+" :  41 -42 43 -44 -26 "+std::to_string(splane));
  addOuterSurf(Out);
  return; 
}

void
simpleCave::createLinks()
  /*!
    Creates a full attachment set
    First two are in the -/+Y direction and have a divider
    Last two are in the -/+X direction and have a divider
    The mid two are -/+Z direction
  */
{  
  ELog::RegMethod RegA("simpleCave","createLinks");

  attachSystem::FixedComp& innerFC=FixedGroup::getKey("Inner");
  attachSystem::FixedComp& outerFC=FixedGroup::getKey("Outer");


  //Outer surfaces
  outerFC.setConnect(0,Origin-Y*(innerLengthFront+wallThick),-Y);
  outerFC.setLinkSurf(0,-SMap.realSurf(buildIndex+1));

  outerFC.setConnect(1,Origin+Y*(innerLengthBack+wallThick),Y);
  outerFC.setLinkSurf(1,SMap.realSurf(buildIndex+2));
  
  outerFC.setConnect(2,Origin-X*(innerWidthRight+wallThick),-X);
  outerFC.setLinkSurf(2,-SMap.realSurf(buildIndex+3));
  
  outerFC.setConnect(3,Origin+X*(innerWidthLeft+wallThick),X);
  outerFC.setLinkSurf(3,SMap.realSurf(buildIndex+4));
  
  outerFC.setConnect(4,Origin-Z*(innerDepth+floorThick),-Z);
  outerFC.setLinkSurf(4,-SMap.realSurf(buildIndex+5));

   int splane=6;
   //   if(roofExtraThick>0.0) splane=216; else splane=6;
   
  
  outerFC.setConnect(5,Origin+Z*(innerHeight+roofThick),Z);
  outerFC.setLinkSurf(5,SMap.realSurf(buildIndex+splane));

  //Inner surfaces
  innerFC.setConnect(0,Origin-Y*(innerLengthFront),-Y);
  innerFC.setLinkSurf(0,-SMap.realSurf(buildIndex+11));

  innerFC.setConnect(1,Origin+Y*(innerLengthBack),Y);
  innerFC.setLinkSurf(1,SMap.realSurf(buildIndex+12));
  
  innerFC.setConnect(2,Origin-X*(innerWidthRight),-X);
  innerFC.setLinkSurf(2,-SMap.realSurf(buildIndex+13));
  
  innerFC.setConnect(3,Origin+X*(innerWidthLeft),X);
  innerFC.setLinkSurf(3,SMap.realSurf(buildIndex+14));
  
  innerFC.setConnect(4,Origin-Z*(innerDepth),-Z);
  innerFC.setLinkSurf(4,-SMap.realSurf(buildIndex+15));
  
  innerFC.setConnect(5,Origin+Z*(innerHeight),Z);
  innerFC.setLinkSurf(5,SMap.realSurf(buildIndex+16));

  return;
}

void 
simpleCave::layerProcess(Simulation& System)
  /*!
    Processes the splitting of the surfaces into a multilayer system
    \param System :: Simulation to work on
  */
{
  ELog::RegMethod RegA("simpleCave","layerProcess");
  // Wall layers
  //  layerSpecial(System);
  if (iLayerFlag==0) return;
  
      int splane;
   if(roofExtraThick>0.0) splane=216; else splane=6;
   
   
   if (nWallLayers>1)
    {
      std::string OutA,OutB;
      ModelSupport::surfDivide DA;
   
      //Walls
      for(size_t i=1;i<nWallLayers;i++)
	{
	  DA.addFrac(wallFracList[i-1]);
	  DA.addMaterial(wallMatList[i-1]);
	}
      DA.addMaterial(wallMatList.back());


      DA.setCellN(CellMap::getCell("RoofOuter"));
      DA.setOutNum(cellIndex,buildIndex+1001);

      ModelSupport::mergeTemplate<Geometry::Plane,
				  Geometry::Plane> surroundRule;
      // Original
      surroundRule.setSurfPair(SMap.realSurf(buildIndex+11),
			       SMap.realSurf(buildIndex+1));
      surroundRule.setSurfPair(SMap.realSurf(buildIndex+12),
			       SMap.realSurf(buildIndex+2));
      
      surroundRule.setSurfPair(SMap.realSurf(buildIndex+13),
			       SMap.realSurf(buildIndex+3));
      surroundRule.setSurfPair(SMap.realSurf(buildIndex+14),
			       SMap.realSurf(buildIndex+4));
      //Roof:
      surroundRule.setSurfPair(SMap.realSurf(buildIndex+16),
			       SMap.realSurf(buildIndex+6));
      
      OutA=ModelSupport::getComposite(SMap,buildIndex,"(-11:12:-13:14:16) ");
      OutB=ModelSupport::getComposite(SMap,buildIndex,
				      " 1 -2 3 -4 -6");

      surroundRule.setInnerRule(OutA);
      surroundRule.setOuterRule(OutB);
      DA.addRule(&surroundRule);

      
      DA.activeDivideTemplate(System);
      cellIndex=DA.getCellNum();



      
      //Corner A
     DA.setCellN(CellMap::getCell("CornerA"));
      DA.setOutNum(cellIndex,buildIndex+2001);

      OutA=ModelSupport::getComposite(SMap,buildIndex,"(-11:12:-13:14) ");
      OutB=ModelSupport::getComposite(SMap,buildIndex,
				      " 1 -2 3 -4");

      surroundRule.setInnerRule(OutA);
      surroundRule.setOuterRule(OutB);
      DA.addRule(&surroundRule);

      
      DA.addRule(&surroundRule);

      
      DA.activeDivideTemplate(System);
      cellIndex=DA.getCellNum();
      




            //Corner B
     DA.setCellN(CellMap::getCell("CornerB"));
      DA.setOutNum(cellIndex,buildIndex+2101);

      OutA=ModelSupport::getComposite(SMap,buildIndex,"(-11:12:-13:14) ");
      OutB=ModelSupport::getComposite(SMap,buildIndex,
				      " 1 -2 3 -4");

      surroundRule.setInnerRule(OutA);
      surroundRule.setOuterRule(OutB);
      DA.addRule(&surroundRule);

      
      DA.addRule(&surroundRule);

      
      DA.activeDivideTemplate(System);
      cellIndex=DA.getCellNum();

            //Corner C
      
DA.setCellN(CellMap::getCell("CornerC"));
      DA.setOutNum(cellIndex,buildIndex+2201);

      OutA=ModelSupport::getComposite(SMap,buildIndex,"(-11:12:-13:14) ");
      OutB=ModelSupport::getComposite(SMap,buildIndex,
				      " 1 -2 3 -4");

      surroundRule.setInnerRule(OutA);
      surroundRule.setOuterRule(OutB);
      DA.addRule(&surroundRule);

      
      DA.addRule(&surroundRule);

      
      DA.activeDivideTemplate(System);
      cellIndex=DA.getCellNum();
      
      
            //Corner D
      
DA.setCellN(CellMap::getCell("CornerD"));
      DA.setOutNum(cellIndex,buildIndex+2301);

      OutA=ModelSupport::getComposite(SMap,buildIndex,"(-11:12:-13:14) ");
      OutB=ModelSupport::getComposite(SMap,buildIndex,
				      " 1 -2 3 -4");

      surroundRule.setInnerRule(OutA);
      surroundRule.setOuterRule(OutB);
      DA.addRule(&surroundRule);

      
      DA.addRule(&surroundRule);

      
      DA.activeDivideTemplate(System);
      cellIndex=DA.getCellNum();

      

      
      // Cell Specific:
      DA.setCellN(CellMap::getCell("RoofInner"));
      DA.setOutNum(cellIndex,buildIndex+1501);
      //Roof:
      OutA=ModelSupport::getComposite(SMap,buildIndex," 16");
      OutB=ModelSupport::getComposite(SMap,buildIndex,
				      " -6");

      surroundRule.setInnerRule(OutA);
      surroundRule.setOuterRule(OutB);
      DA.addRule(&surroundRule);
      DA.activeDivideTemplate(System);
      cellIndex=DA.getCellNum();

      
      //RearWall
      DA.setCellN(CellMap::getCell("RearWall"));
      DA.setOutNum(cellIndex,buildIndex+1601);
      OutA=ModelSupport::getComposite(SMap,buildIndex," -11");
      OutB=ModelSupport::getComposite(SMap,buildIndex," 1");

      surroundRule.setInnerRule(OutA);
      surroundRule.setOuterRule(OutB);
      DA.addRule(&surroundRule);
      DA.activeDivideTemplate(System);
      cellIndex=DA.getCellNum();
      

      
      //FrontWall
      DA.setCellN(CellMap::getCell("FrontWall"));
      DA.setOutNum(cellIndex,buildIndex+1701);
      OutA=ModelSupport::getComposite(SMap,buildIndex," 12");
      OutB=ModelSupport::getComposite(SMap,buildIndex,
				      " -2");

      surroundRule.setInnerRule(OutA);
      surroundRule.setOuterRule(OutB);
      DA.addRule(&surroundRule);
      DA.activeDivideTemplate(System);
      cellIndex=DA.getCellNum();

      
      //LeftWall
      DA.setCellN(CellMap::getCell("LeftWall"));
      DA.setOutNum(cellIndex,buildIndex+1801);
      OutA=ModelSupport::getComposite(SMap,buildIndex," -13");
      OutB=ModelSupport::getComposite(SMap,buildIndex,
				      " 3");

      surroundRule.setInnerRule(OutA);
      surroundRule.setOuterRule(OutB);
      DA.addRule(&surroundRule);
      DA.activeDivideTemplate(System);
      cellIndex=DA.getCellNum();

      
      
      //RightWall
      DA.setCellN(CellMap::getCell("RightWall"));
      DA.setOutNum(cellIndex,buildIndex+1901);
      OutA=ModelSupport::getComposite(SMap,buildIndex," 14");
      OutB=ModelSupport::getComposite(SMap,buildIndex," -4");

      surroundRule.setInnerRule(OutA);
      surroundRule.setOuterRule(OutB);
      DA.addRule(&surroundRule);
      DA.activeDivideTemplate(System);
      cellIndex=DA.getCellNum();

      /*
      //CornerA
      //" (14 : -11) 1 -4 15 -16 -101 104"); 
  

      DA.setCellN(CellMap::getCell("CornerA"));
      DA.setOutNum(cellIndex,buildIndex+2001);

      ModelSupport::mergeTemplate<Geometry::Plane,
				  Geometry::Plane> surroundRuleCorners;
      surroundRuleCorners.setSurfPair(SMap.realSurf(buildIndex+101),
			       SMap.realSurf(buildIndex+1));
      surroundRuleCorners.setSurfPair(SMap.realSurf(buildIndex+102),
			       SMap.realSurf(buildIndex+2));
      
      surroundRuleCorners.setSurfPair(SMap.realSurf(buildIndex+103),
			       SMap.realSurf(buildIndex+3));
      surroundRuleCorners.setSurfPair(SMap.realSurf(buildIndex+104),
			       SMap.realSurf(buildIndex+4));

      OutA=ModelSupport::getComposite(SMap,buildIndex,"( 104 : -101 )   ");
      OutB=ModelSupport::getComposite(SMap,buildIndex," -4 1 ");

      surroundRuleCorners.setInnerRule(OutA);
      surroundRuleCorners.setOuterRule(OutB);
      DA.addRule(&surroundRuleCorners);
      DA.activeDivideTemplate(System);
      cellIndex=DA.getCellNum();
      */

      // Cell Specific, hatch walls
      /*
      DA.setCellN(CellMap::getCell("HatchWalls"));
      DA.setOutNum(cellIndex,buildIndex+3001);

      ModelSupport::mergeTemplate<Geometry::Plane,
				  Geometry::Plane> surroundRule1;
      surroundRule1.setSurfPair(SMap.realSurf(buildIndex+16),
			       SMap.realSurf(buildIndex+splane));
      OutA=ModelSupport::getComposite(SMap,buildIndex," 16 ");
      OutB=ModelSupport::getComposite(SMap,buildIndex," "+std::to_string(-splane)+" ");

      surroundRule1.setInnerRule(OutA);
      surroundRule1.setOuterRule(OutB);
      DA.addRule(&surroundRule1);
      DA.activeDivideTemplate(System);

      cellIndex=DA.getCellNum();

      // Cell Specific, hatch walls
      DA.setCellN(CellMap::getCell("HatchWalls1"));
      DA.setOutNum(cellIndex,buildIndex+2001);
      DA.addRule(&surroundRule1);
      DA.activeDivideTemplate(System);

      cellIndex=DA.getCellNum();
      */
    }



   
   /*
  if(lockCapNLayers>1){

      std::string OutA,OutB;
      ModelSupport::surfDivide DA;
      
      //Walls
      for(size_t i=1;i<lockCapNLayers;i++)
	{
	  DA.addFrac(capFracList[i-1]);
	  DA.addMaterial(capMatList[i-1]);
	}
      DA.addMaterial(capMatList.back());
        //Cell specific, hatch lock cap
      DA.setCellN(CellMap::getCell("HatchLockCap"));
      DA.setOutNum(cellIndex,buildIndex+4001);

      ModelSupport::mergeTemplate<Geometry::Plane,
				  Geometry::Plane> surroundRule;
      surroundRule.setSurfPair(SMap.realSurf(buildIndex+401),
			       SMap.realSurf(buildIndex+41));
      surroundRule.setSurfPair(SMap.realSurf(buildIndex+402),
			       SMap.realSurf(buildIndex+42));
      surroundRule.setSurfPair(SMap.realSurf(buildIndex+403),
			       SMap.realSurf(buildIndex+43));
      surroundRule.setSurfPair(SMap.realSurf(buildIndex+404),
			       SMap.realSurf(buildIndex+44));
      surroundRule.setSurfPair(SMap.realSurf(buildIndex+206),
			       SMap.realSurf(buildIndex+26));
      OutA=ModelSupport::getComposite(SMap,buildIndex," (-401:402:-403:404:206) ");
      OutB=ModelSupport::getComposite(SMap,buildIndex," 41 -42 43 -44 -26 ");

      surroundRule.setInnerRule(OutA);
      surroundRule.setOuterRule(OutB);
      DA.addRule(&surroundRule);
      DA.activeDivideTemplate(System);

      cellIndex=DA.getCellNum();
      }*/
  return;
}


  
void
simpleCave::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: Attachment point
    \param sideIndex :: sideIndex for link point
   */
{
  ELog::RegMethod RegA("simpleCave","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  //  std::cout << "Cave, creating surfaces" << std::endl;
  createSurfaces();
  //    std::cout << "Cave, creating objects"<< std::endl;
  createObjects(System);
  //  std::cout << "Cave, creating objects, done"<< std::endl;
  createLinks();
  insertObjects(System);
  //    std::cout << "Processing layers"<< std::endl;
  //  layerProcess(System); // dividing surfaces into layers, will be called later
  //  std::cout << "Done" << std::endl;
  return;
}

}  // NAMESPACE pipeSystem
