/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/Bunker.cxx
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
#include "inputParam.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ReadFunctions.h"
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
#include "MXcards.h"
#include "Zaid.h"
#include "Material.h"
#include "DBMaterial.h"

#include "surfDBase.h"
#include "surfDIter.h"
#include "surfDivide.h"
#include "SurInter.h"
#include "mergeTemplate.h"

#include "World.h"
#include "BunkerInsert.h"
#include "Bunker.h"
#include "LayerDivide3D.h"

namespace essSystem
{

Bunker::Bunker(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,12),
  attachSystem::CellMap(),attachSystem::SurfMap(),
  bnkIndex(ModelSupport::objectRegister::Instance().cell(Key,20000)),
  cellIndex(bnkIndex+1),leftWallFlag(1),rightWallFlag(1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

Bunker::Bunker(const Bunker& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  attachSystem::CellMap(A),
  bnkIndex(A.bnkIndex),cellIndex(A.cellIndex),
  leftWallFlag(A.leftWallFlag),rightWallFlag(A.rightWallFlag),
  rotCentre(A.rotCentre),leftPhase(A.leftPhase),
  rightPhase(A.rightPhase),leftAngle(A.leftAngle),
  rightAngle(A.rightAngle),nSectors(A.nSectors),
  sectPhase(A.sectPhase),nVert(A.nVert),vertFrac(A.vertFrac),
  nLayers(A.nLayers),wallFrac(A.wallFrac),
  innerRadius(A.innerRadius),wallRadius(A.wallRadius),
  floorDepth(A.floorDepth),roofHeight(A.roofHeight),
  wallThick(A.wallThick),sideThick(A.sideThick),
  roofThick(A.roofThick),floorThick(A.floorThick),
  voidMat(A.voidMat),wallMat(A.wallMat),
  loadFile(A.loadFile),outFile(A.outFile)
  /*!
    Copy constructor
    \param A :: Bunker to copy
  */
{}

Bunker&
Bunker::operator=(const Bunker& A)
  /*!
    Assignment operator
    \param A :: Bunker to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      cellIndex=A.cellIndex;
      leftWallFlag=A.leftWallFlag;
      rightWallFlag=A.rightWallFlag;
      rotCentre=A.rotCentre;
      leftPhase=A.leftPhase;
      rightPhase=A.rightPhase;
      leftAngle=A.leftAngle;
      rightAngle=A.rightAngle;
      nSectors=A.nSectors;
      sectPhase=A.sectPhase;
      nVert=A.nVert;
      vertFrac=A.vertFrac;
      nLayers=A.nLayers;
      wallFrac=A.wallFrac;
      innerRadius=A.innerRadius;
      wallRadius=A.wallRadius;
      floorDepth=A.floorDepth;
      roofHeight=A.roofHeight;
      wallThick=A.wallThick;
      sideThick=A.sideThick;
      roofThick=A.roofThick;
      floorThick=A.floorThick;
      voidMat=A.voidMat;
      wallMat=A.wallMat;
    }
  return *this;
}

Bunker::~Bunker() 
  /*!
    Destructor
  */
{}

void
Bunker::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("Bunker","populate");

  leftPhase=Control.EvalVar<double>(keyName+"LeftPhase");
  rightPhase=Control.EvalVar<double>(keyName+"RightPhase");
  leftAngle=Control.EvalVar<double>(keyName+"LeftAngle");
  rightAngle=Control.EvalVar<double>(keyName+"RightAngle");

  wallRadius=Control.EvalVar<double>(keyName+"WallRadius");
  floorDepth=Control.EvalVar<double>(keyName+"FloorDepth");
  roofHeight=Control.EvalVar<double>(keyName+"RoofHeight");

  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  sideThick=Control.EvalVar<double>(keyName+"SideThick");
  roofThick=Control.EvalVar<double>(keyName+"RoofThick");
  floorThick=Control.EvalVar<double>(keyName+"FloorThick");

  voidMat=ModelSupport::EvalDefMat<int>(Control,keyName+"VoidMat",0);
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  roofMat=ModelSupport::EvalMat<int>(Control,keyName+"RoofMat");
  

  nLayers=Control.EvalVar<size_t>(keyName+"NLayers");
  ModelSupport::populateAddRange(Control,nLayers,keyName+"WallLen",
  			      wallRadius,wallRadius+wallThick,wallFrac);
  for(size_t i=0;i<wallFrac.size();i++)
    wallFrac[i]=(wallFrac[i]-wallRadius)/wallThick;

  nSectors=Control.EvalVar<size_t>(keyName+"NSectors");
  ModelSupport::populateRange(Control,nSectors,keyName+"SectAngle",
			      leftPhase,rightPhase,sectPhase);

  nSegment=Control.EvalDefVar<size_t>(keyName+"NSegment",0);  
  ModelSupport::populateRange(Control,nSegment,keyName+"SegDivide",
			      0,1.0,segDivide);

  // BOOLEAN NUMBER!!!!!!!
  activeSegment=Control.EvalDefVar<size_t>(keyName+"ActiveSegment",0);
  
  nVert=Control.EvalVar<size_t>(keyName+"NVert");
  midZ=Control.EvalDefVar<double>(keyName+"MidZ",0.0);
  ModelSupport::populateQuadRange(Control,nVert,keyName+"VertLen",
				  -floorDepth,midZ,roofHeight,vertFrac);
  for(size_t i=0;i<vertFrac.size();i++)
    vertFrac[i]=(vertFrac[i]+floorDepth)/(floorDepth+roofHeight);

  // SIDE LAYERS:
  
  sideFlag=Control.EvalDefVar<int>(keyName+"SideFlag",0);
  nSide=Control.EvalVar<size_t>(keyName+"NSide");
  ModelSupport::populateAddRange(Control,nSide,keyName+"SideThick",
				 0.0,sideThick,sideFrac);
  nSideVert=Control.EvalVar<size_t>(keyName+"NSideVert");
  ModelSupport::populateQuadRange(Control,nSideVert,keyName+"SideVert",
				  -floorDepth,midZ,roofHeight,sideVertFrac);
  nSideThick=Control.EvalVar<size_t>(keyName+"NSideThick");
  ModelSupport::populateAddRange(Control,nSideVert,keyName+"SideThick",
				 0,wallThick,sideThickFrac);
  

  
  nRoofVert=Control.EvalVar<size_t>(keyName+"NRoofVert");
  nRoofRadial=Control.EvalDefVar<size_t>(keyName+"NRoofRadial",0);
  nRoofSide=Control.EvalDefVar<size_t>(keyName+"NRoofSide",0);
  // BOOLEAN NUMBER!!!!!!!
  activeRoof=Control.EvalDefVar<size_t>(keyName+"ActiveRoof",0);

  ModelSupport::populateDivideLen(Control,nRoofVert,keyName+"RoofVert",
				  roofThick,roofVert);
  ModelSupport::populateDivideLen(Control,nRoofRadial,keyName+"RoofRadial",
				  1.0,roofRadial);
  ModelSupport::populateDivideLen(Control,nRoofSide,keyName+"RoofSide",
				  1.0,roofSide);
  ModelSupport::populateDivide(Control,nRoofVert,keyName+"RoofMat",
  			       roofMat,roofMatVec);

  
  loadFile=Control.EvalDefVar<std::string>(keyName+"LoadFile","");
  outFile=Control.EvalDefVar<std::string>(keyName+"OutFile","");

  return;
}
  
void
Bunker::createUnitVector(const attachSystem::FixedComp& MainCentre,
			 const attachSystem::FixedComp& FC,
			 const long int sideIndex,
			 const bool reverseZ)
  /*!
    Create the unit vectors
    \param MainCentre :: Main rotation centre
    \param FC :: Linked object
    \param sideIndex :: Side for linkage centre
    \param reverseZ :: reverse Z direction
  */
{
  ELog::RegMethod RegA("Bunker","createUnitVector");

  rotCentre=MainCentre.getCentre();
  FixedComp::createUnitVector(FC,sideIndex);
  if (reverseZ)
    {
      X*=-1;
      Z*=-1;
    }
      
  return;
}

void
Bunker::createWallSurfaces(const Geometry::Vec3D& ,
			   const Geometry::Vec3D& ) 
  /*!
    Create the wall Surface if divided
  */
{
  return;
}
  
void
Bunker::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("Bunker","createSurface");

  innerRadius=rotCentre.Distance(Origin);

  Geometry::Vec3D AWallDir(X);
  Geometry::Vec3D BWallDir(X);
  // rotation of axis:
  Geometry::Quaternion::calcQRotDeg(leftAngle+leftPhase,-Z).rotate(AWallDir);
  Geometry::Quaternion::calcQRotDeg(rightAngle+rightPhase,-Z).rotate(BWallDir);
  // rotation of phase points:

  // Points on wall
  Geometry::Vec3D AWall(Origin-rotCentre);
  Geometry::Vec3D BWall(Origin-rotCentre);
  Geometry::Quaternion::calcQRotDeg(-leftPhase,Z).rotate(AWall);
  Geometry::Quaternion::calcQRotDeg(-rightPhase,Z).rotate(BWall);
  AWall+=rotCentre;
  BWall+=rotCentre;
  // Divider
  ModelSupport::buildPlane(SMap,bnkIndex+1,rotCentre,Y);
  ModelSupport::buildCylinder(SMap,bnkIndex+7,rotCentre,Z,wallRadius);


  ModelSupport::buildPlane(SMap,bnkIndex+3,AWall,AWallDir);
  ModelSupport::buildPlane(SMap,bnkIndex+4,BWall,BWallDir);
  
  ModelSupport::buildPlane(SMap,bnkIndex+5,Origin-Z*floorDepth,Z);
  ModelSupport::buildPlane(SMap,bnkIndex+6,Origin+Z*roofHeight,Z);

  // Walls
  ModelSupport::buildCylinder(SMap,bnkIndex+17,rotCentre,
			      Z,wallRadius+wallThick);
    
  ModelSupport::buildPlane(SMap,bnkIndex+13,
			   AWall-AWallDir*sideThick,AWallDir);
  ModelSupport::buildPlane(SMap,bnkIndex+14,
			   BWall+BWallDir*sideThick,BWallDir);
  
  ModelSupport::buildPlane(SMap,bnkIndex+15,
			   Origin-Z*(floorDepth+floorThick),Z);
  ModelSupport::buildPlane(SMap,bnkIndex+16,
			   Origin+Z*(roofHeight+roofThick),Z);

  setSurf("floorInner",SMap.realSurf(bnkIndex+5));
  setSurf("roofInner",SMap.realSurf(bnkIndex+6));
  // CREATE Sector boundary lines
  // Note negative subtraction as moving +ve to -ve


  //  double phaseAngle(leftPhase);
  //  const double phaseStep((leftPhase+rightPhase)/nSectors);

  // NEW:
  const double phaseDiff(rightPhase-leftPhase);
  const double angleDiff(rightAngle-leftAngle);

  int divIndex(bnkIndex+1001);

  double phase(leftPhase);
  double angle(leftAngle);
  const double phaseStep((rightPhase-leftPhase)/nSectors);
  const double angleStep((rightAngle-leftAngle)/nSectors);

  SMap.addMatch(divIndex,SMap.realSurf(bnkIndex+3));
  for(size_t i=1;i<nSectors;i++)
    {
      divIndex++;
      phase+=phaseStep;
      angle+=angleStep;

      const double F= (sectPhase[i]-leftPhase)/phaseDiff;
      angle= leftAngle+F*angleDiff;
            
      Geometry::Vec3D DPosition(Origin-rotCentre);
      Geometry::Quaternion::calcQRotDeg(sectPhase[i],-Z).rotate(DPosition);
      DPosition+=rotCentre;
      
      Geometry::Vec3D DDir(X);      
      Geometry::Quaternion::calcQRotDeg(sectPhase[i]+angle,-Z).rotate(DDir);
      ModelSupport::buildPlane(SMap,divIndex,DPosition,DDir);
    }
  divIndex++;
  SMap.addMatch(divIndex,SMap.realSurf(bnkIndex+4));

  return;
}


  
void
Bunker::createSideLinks(const Geometry::Vec3D& AWall,
			const Geometry::Vec3D& BWall,
			const Geometry::Vec3D& AWallDir,
			const Geometry::Vec3D& BWallDir)
  /*!
    Ugly function to create side wall linkes
    \param AWall :: Left wall point
    \param BWall :: Right wall point		
    \param AWallDir :: Left wall main direction
    \param BWallDir :: Right wall main direction    
   */
{
  ELog::RegMethod RegA("Bunker","createSideLinks");
		      
  // Construct links on side walls:
  Geometry::Vec3D AWallY(AWallDir*Z);
  Geometry::Vec3D BWallY(BWallDir*Z);

  if (AWallY.dotProd(Y)<0.0)
    AWallY*=-1;
  if (BWallY.dotProd(Y)<0.0)
    BWallY*=-1;

  // Outer 
  FixedComp::setConnect(2,AWall+AWallY*wallRadius/2.0,AWallDir);
  FixedComp::setConnect(3,BWall+BWallY*wallRadius/2.0,BWallDir);

  return;
}
  
void
Bunker::createObjects(Simulation& System,
		      const attachSystem::FixedComp& FC,  
		      const long int sideIndex)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
    \param FC :: Side of bulk shield + divider(?)
    \param sideIndex :: side of link point
  */
{
  ELog::RegMethod RegA("Bunker","createObjects");
  
  std::string Out;
  const std::string Inner=FC.getSignedLinkString(sideIndex);
  const int InnerSurf=FC.getSignedLinkSurf(sideIndex);
  
  Out=ModelSupport::getComposite(SMap,bnkIndex,"1 -7 3 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,voidMat,0.0,Out+Inner));
  setCell("MainVoid",cellIndex-1);
  // process left wall:
  //  std::string leftWallStr=procLeftWall(System);

  // left:right:floor:roof:Outer
  int lwIndex(bnkIndex);  // indexs for wall 
  int rwIndex(bnkIndex);
  if (leftWallFlag)
    {
      Out=ModelSupport::getComposite(SMap,bnkIndex," 1 -17 -3 13 5 -6 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out+Inner));
      setCell("leftWall",cellIndex-1);
      lwIndex+=10;
    }
  if (rightWallFlag)
    {
      Out=ModelSupport::getComposite(SMap,bnkIndex," 1 -17 4 -14 5 -6 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out+Inner));
      setCell("rightWall",cellIndex-1);
      rwIndex+=10;
    }
  
  Out=ModelSupport::getComposite(SMap,bnkIndex,lwIndex,rwIndex,
				 " 1 -17 3M -4N -5 15 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out+Inner));
  setCell("floor",cellIndex-1);

  // Divide the roof into sector as well
  
    
  // Main wall not divided
  int divIndex(bnkIndex+1000);
  for(size_t i=0;i<nSectors;i++)
    {
      // Divide the roof into sector as well
      Out=ModelSupport::getComposite(SMap,bnkIndex," 1 -17 6 -16 ");
      if (i)
	Out+=ModelSupport::getComposite(SMap,divIndex," 1 ");
      else
	Out+=ModelSupport::getComposite(SMap,lwIndex," 3 ");

      if (i+1!=nSectors)
	Out+=ModelSupport::getComposite(SMap,divIndex," -2 ");
      else
	Out+=ModelSupport::getComposite(SMap,rwIndex," -4 ");

      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out+Inner));
      addCell("roof"+StrFunc::makeString(i),cellIndex-1);


      Out=ModelSupport::getComposite(SMap,bnkIndex,divIndex,
				     " 1 7 -17 1M -2M 5 -6 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
      addCell("frontWall",cellIndex-1);
      divIndex++;
    }
  createMainWall(System);
  createMainRoof(System,InnerSurf);

  // External
  Out=ModelSupport::getComposite(SMap,bnkIndex,lwIndex,rwIndex,
				 " 1 -17 3M -4N 15 -16 ");
  addOuterSurf(Out+Inner);
        
  return;
}

void
Bunker::createMainRoof(Simulation& System,const int innerSurf)
  /*!
    Create a tesselated main wall
    \param System :: Simulation system
    \param innerSurf :: inner roof cyclindrical surface
  */
{
  ELog::RegMethod RegA("Bunker","createMainRoof");

  size_t AS=activeRoof;  // binary system

  const int lwIndex((leftWallFlag) ? bnkIndex+10 : bnkIndex);
  const int rwIndex((rightWallFlag) ? bnkIndex+10 : bnkIndex);
  int divIndex(bnkIndex+1000);

  for(size_t i=0;AS && i<nSectors;i++)
    {
      if (AS & 1)
        {
	  ModelSupport::LayerDivide3D LD3(keyName+"mainRoof"+
					  StrFunc::makeString(i));
	  const int LW=(i) ? divIndex+1 : lwIndex+3;
	  const int RW=(i+1!=nSectors) ? divIndex+2 : rwIndex+4;	  

	  // Front/back??
	  LD3.setSurfPair(0,SMap.realSurf(innerSurf),
			  SMap.realSurf(bnkIndex+17));
	  LD3.setSurfPair(1,SMap.realSurf(bnkIndex+6),
			  SMap.realSurf(bnkIndex+16));
	  LD3.setSurfPair(2,SMap.realSurf(LW),SMap.realSurf(RW));

	  LD3.setFractions(0,roofRadial);
	  LD3.setFractions(1,roofVert);
	  LD3.setFractions(2,roofSide);
	  ELog::EM<<"Read == "<<keyName<<"Def.xml"<<ELog::endDiag;
	  LD3.setXMLdata(keyName+"Def.xml","RoofMat",keyName+".xml");

	  LD3.divideCell(System,getCell("roof"+StrFunc::makeString(i)));
	  ELog::EM<<"Read == "<<keyName<<"Def.xml"<<ELog::endDiag;
      
	  removeCell("roof"+StrFunc::makeString(i));
	  addSurfs("roof"+StrFunc::makeString(i),LD3.getSurfs());
	  addCells("roof"+StrFunc::makeString(i),LD3.getCells());

	}
      AS>>=1;
      divIndex++;
    }
  return;
}

  
void
Bunker::createMainWall(Simulation& System)
  /*!
    Create a tesselated main wall
    \param System :: Simulation syst em
  */
{
  ELog::RegMethod RegA("Bunker","createMainWall");

  size_t AS=activeSegment;  // binary system
  
  for(size_t i=0;AS && i<nSectors;i++)
    {
      const std::string CName="Sector"+StrFunc::makeString(i);
      if (AS & 1)
        {
	  const int CN=getCell("frontWall",i);
	  ModelSupport::LayerDivide3D LD3(keyName+"mainWall"+
					  StrFunc::makeString(i));
	  LD3.setSurfPair(0,SMap.realSurf(bnkIndex+1001+static_cast<int>(i)),
			  SMap.realSurf(bnkIndex+1002+static_cast<int>(i)));
	  
	  LD3.setSurfPair(1,SMap.realSurf(bnkIndex+5),
		  SMap.realSurf(bnkIndex+6));
	  LD3.setSurfPair(2,SMap.realSurf(bnkIndex+7),
			  SMap.realSurf(bnkIndex+17));
	  LD3.setFractions(0,segDivide);
	  LD3.setFractions(1,vertFrac);
	  LD3.setFractions(2,wallFrac);
	  
	  LD3.setXMLdata(keyName+"Def.xml","WallMat",keyName+".xml");
	  LD3.divideCell(System,CN);

	  removeCell("frontWall",i);
	  addSurfs(CName,LD3.getSurfs());
	  addCells(CName,LD3.getCells());
	}
      AS>>=1;
    }
  return;
}


void 
Bunker::layerProcess(Simulation& System)
  /*!
    Processes the splitting of the surfaces into a multilayer system
    \param System :: Simulation to work on
  */
{
  ELog::RegMethod RegA("Bunker","layerProcess");
  // Steel layers
  //  layerSpecial(System);

  if (nRoofVert>1)
    {
      std::string OutA,OutB;
      ModelSupport::surfDivide DA;      
      for(size_t i=1;i<nRoofVert;i++)
	{
	  DA.addFrac(roofVert[i-1]);
	  DA.addMaterial(roofMatVec[i-1]);
	}
      DA.addMaterial(roofMatVec.back());
      size_t AS=activeRoof;  // binary system
      int BNIndex(bnkIndex+5001);
      for(size_t iSector=0;iSector<nSectors;iSector++)
	{
	  if (!(AS & 1))  // only process roof sections not give
	    {
	      // Cell Specific:
	      const int firstCell(cellIndex);
	      DA.setCellN(getCell("roof"+StrFunc::makeString(iSector)));
	      DA.setOutNum(cellIndex,BNIndex);
	      ModelSupport::mergeTemplate<Geometry::Plane,
					  Geometry::Plane> surroundRule;
	      
	      surroundRule.setSurfPair(SMap.realSurf(bnkIndex+6),
				       SMap.realSurf(bnkIndex+16));
	      
	      OutA=ModelSupport::getComposite(SMap,bnkIndex," 6 ");
	      OutB=ModelSupport::getComposite(SMap,bnkIndex," -16 ");
	      
	      surroundRule.setInnerRule(OutA);
	      surroundRule.setOuterRule(OutB);
	      
	      DA.addRule(&surroundRule);
	      DA.activeDivideTemplate(System);
	      
	      cellIndex=DA.getCellNum();
	      removeCell("roof"+StrFunc::makeString(iSector));
	      setCells("roof"+StrFunc::makeString(iSector)
		       ,firstCell,cellIndex-1);
	      BNIndex+=300;
	    }
	  AS>>=1;	  
	}
    }

  for(size_t iSector=0;iSector<nSectors;iSector++)
    {
      addCells("roof",getCells("roof"+StrFunc::makeString(iSector)));
      addCells("frontWall",getCells("sector"+StrFunc::makeString(iSector)));
    }
			    
  return;
}

  
void
Bunker::createLinks()
  /*!
    Create all the linkes [OutGoing]
  */
{
  ELog::RegMethod RegA("Bunker","createLinks");

  FixedComp::setConnect(0,rotCentre+Y*(wallRadius),Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(bnkIndex+7));
  FixedComp::setBridgeSurf(0,SMap.realSurf(bnkIndex+1));
  
  // Outer
  FixedComp::setConnect(1,rotCentre+Y*(wallRadius+wallThick),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(bnkIndex+17));
  FixedComp::setBridgeSurf(1,SMap.realSurf(bnkIndex+1));
  
  FixedComp::setConnect(4,Origin-Z*(floorDepth+floorThick),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(bnkIndex+15));
  FixedComp::setConnect(5,Origin+Z*(roofHeight+roofThick),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(bnkIndex+16));

  // Inner
  FixedComp::setConnect(7,rotCentre+Y*wallRadius,-Y);
  FixedComp::setLinkSurf(7,-SMap.realSurf(bnkIndex+7));

  FixedComp::setConnect(10,Origin-Z*floorDepth,Z);
  FixedComp::setLinkSurf(10,SMap.realSurf(bnkIndex+5));
  FixedComp::setConnect(11,Origin+Z*roofHeight,-Z);
  FixedComp::setLinkSurf(11,-SMap.realSurf(bnkIndex+6));

  return;
}
  
std::string
Bunker::calcSegment(const Simulation& System,
		    const Geometry::Vec3D& TPoint,
		    const Geometry::Vec3D& Axis) const
  /*!
    Determine the segment that a point falls int
    \param TPoint :: Point to check
    \param Axis :: Direction 
    \return Segment string
  */
{
  ELog::RegMethod RegA("Bunker","calcSegment");
  for(size_t i=0;i<nSectors;i++)
    {
      const std::string SName="frontWall"+StrFunc::makeString(i);
      const int cN=getCell(SName);
      const MonteCarlo::Object* SUnit=System.findQhull(cN);
      if (SUnit)
	{
	  HeadRule HR=SUnit->getHeadRule();
	  HR.populateSurf();
	  std::pair<Geometry::Vec3D,int> LCut=
	    SurInter::interceptRuleConst(HR,TPoint,Axis);
	  
	  if (LCut.second)
	    return SName;
	}
    }
  throw ColErr::InContainerError<Geometry::Vec3D>
    (TPoint,"Not in bunker wall sectors");
}

void
Bunker::setCutWall(const bool lFlag,const bool rFlag)
  /*!
    Sets the cut rule base on an existing fixed component.
    \param lFlag :: Wall to use [0 none / 1 full ]
    \param rFlag :: Wall to use [0 none / 1 full ]
  */
{
  ELog::RegMethod RegA("Bunker","setCutWall");

  leftWallFlag=lFlag;
  rightWallFlag=rFlag;
  return;
}

  
void
Bunker::createAll(Simulation& System,
		  const attachSystem::FixedComp& MainCentre,
		  const attachSystem::FixedComp& FC,
		  const long int linkIndex,
		  const bool reverseZ)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param MainCentre :: Rotatioin Centre
    \param FC :: Central origin
    \param linkIndex :: linkIndex number
    \param reverseZ :: Reverse Z direction
  */
{
  ELog::RegMethod RegA("Bunker","createAll");

  populate(System.getDataBase());
  createUnitVector(MainCentre,FC,linkIndex,reverseZ);
  createSurfaces();
  createLinks();
  createObjects(System,FC,linkIndex);
  layerProcess(System);
  insertObjects(System);              

  return;
}

}  // NAMESPACE essSystem
