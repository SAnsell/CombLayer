/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/Bunker.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include "FrontBackCut.h"
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
#include "BunkerRoof.h"
#include "BunkerWall.h"
#include "LayerDivide3D.h"
#include "Chicane.h"
#include "Bunker.h"


namespace essSystem
{

Bunker::Bunker(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,12),
  attachSystem::CellMap(),attachSystem::SurfMap(),
  bnkIndex(ModelSupport::objectRegister::Instance().cell(Key,20000)),
  cellIndex(bnkIndex+1),leftWallFlag(1),rightWallFlag(1),
  roofObj(new BunkerRoof(Key)),wallObj(new BunkerWall(Key))
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(roofObj);
  OR.addObject(wallObj);
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
  wallHeight=Control.EvalDefVar<double>(keyName+"WallHeight",roofHeight);

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
  nSectors=std::max<size_t>(nSectors,1);
  ModelSupport::populateRange(Control,nSectors,keyName+"SectAngle",
			      leftPhase,rightPhase,sectPhase);

  nSegment=Control.EvalDefVar<size_t>(keyName+"NSegment",0);  
  ModelSupport::populateRange(Control,nSegment,keyName+"SegDivide",
			      0,1.0,segDivide);

  // BOOLEAN NUMBER!!!!!!!
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
  
  return;
}
  
void
Bunker::createUnitVector(const attachSystem::FixedComp& FC,
			 const long int sideIndex,
			 const bool reverseX)
  /*!
    Create the unit vectors
    \param FC :: Linked object
    \param sideIndex :: Side for linkage centre
    \param reverseX :: reverse X direction
  */
{
  ELog::RegMethod RegA("Bunker","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  if (reverseX) X*=-1;
  return;
}


void
Bunker::calcSegPosition(const size_t segIndex,
                        Geometry::Vec3D& DPosition,
                        Geometry::Vec3D& DX,
                        Geometry::Vec3D& DY,
                        Geometry::Vec3D& DZ) const
  /*!
    Calculate the position of the mid segments
    \param segIndex :: segment index
    \param DPosition :: Position of centre of wall [inner]
    \param DX :: Direction of X 
    \param DY :: Direction of Y
    \param DZ :: Direction of Z
  */
{
  ELog::RegMethod RegA("Bunker","calcSegPosition");

  const double phaseDiff(rightPhase-leftPhase);
  const double angleDiff(rightAngle-leftAngle);
  
  const double midPhaseStep((segIndex+1<nSectors) ?
                            (sectPhase[segIndex+1]-sectPhase[segIndex+1])/2.0 :
                            (sectPhase[segIndex]-sectPhase[segIndex-1])/2.0 );
                            
  const double F= (midPhaseStep-leftPhase)/phaseDiff;
  const double angle= leftAngle+F*angleDiff;
  
  DPosition=Origin-rotCentre;
  Geometry::Quaternion::calcQRotDeg(sectPhase[segIndex],-Z).rotate(DPosition);
  DPosition+=rotCentre;
      
  DX=X;
  DY=Y;
  DZ=Z;
  Geometry::Quaternion::calcQRotDeg(sectPhase[segIndex]+angle,-Z).rotate(DX);
  Geometry::Quaternion::calcQRotDeg(sectPhase[segIndex]+angle,-Z).rotate(DY);

  DPosition+=DY*(wallRadius-innerRadius);

  return;
}

  
void
Bunker::createSurfaces(const bool revX)
  /*!
    Create All the surfaces
    \param revX :: reverse rotation axis sign
  */
{
  ELog::RegMethod RegA("Bunker","createSurface");

  const Geometry::Vec3D ZRotAxis((revX) ? -Z : Z);

  Geometry::Vec3D CentAxis(Y);
  Geometry::Vec3D AWallDir(X);
  Geometry::Vec3D BWallDir(X);
  // rotation of axis:
  Geometry::Quaternion::calcQRotDeg(leftAngle+leftPhase,-ZRotAxis).rotate(AWallDir);
  Geometry::Quaternion::calcQRotDeg(rightAngle+rightPhase,-ZRotAxis).rotate(BWallDir);
  Geometry::Quaternion::calcQRotDeg
    ((leftAngle+leftPhase+rightAngle+rightPhase)/2.0,-ZRotAxis).
    rotate(CentAxis);
  // rotation of phase points:

  // Points on wall
  Geometry::Vec3D AWall(Origin-rotCentre);
  Geometry::Vec3D BWall(Origin-rotCentre);
  Geometry::Quaternion::calcQRotDeg(-leftPhase,ZRotAxis).rotate(AWall);
  Geometry::Quaternion::calcQRotDeg(-rightPhase,ZRotAxis).rotate(BWall);
  AWall+=rotCentre;
  BWall+=rotCentre;
  
  
  // Divider
  ModelSupport::buildPlane(SMap,bnkIndex+1,rotCentre,CentAxis);
  
  ModelSupport::buildCylinder(SMap,bnkIndex+7,rotCentre,Z,wallRadius);

  ModelSupport::buildPlane(SMap,bnkIndex+3,AWall,AWallDir);
  ModelSupport::buildPlane(SMap,bnkIndex+4,BWall,BWallDir);
  
  ModelSupport::buildPlane(SMap,bnkIndex+5,Origin-Z*floorDepth,Z);
  ModelSupport::buildPlane(SMap,bnkIndex+6,Origin+Z*roofHeight,Z);
  ModelSupport::buildPlane(SMap,bnkIndex+106,Origin+Z*wallHeight,Z);

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

  setSurf("leftWallInner",SMap.realSurf(bnkIndex+3));
  setSurf("rightWallInner",SMap.realSurf(bnkIndex+4));
  setSurf("leftWallOuter",SMap.realSurf(bnkIndex+13));
  setSurf("rightWallOuter",SMap.realSurf(bnkIndex+14));
  setSurf("floorInner",SMap.realSurf(bnkIndex+5));
  setSurf("wallInner",SMap.realSurf(bnkIndex+106));
  setSurf("roofInner",SMap.realSurf(bnkIndex+6));
  setSurf("roofOuter",SMap.realSurf(bnkIndex+16));
  // CREATE Sector boundary lines
  // Note negative subtraction as moving +ve to -ve


  //  double phaseAngle(leftPhase);
  //  const double phaseStep((leftPhase+rightPhase)/nSectors);

  // NEW:
  const double phaseDiff(rightPhase-leftPhase);
  const double angleDiff(rightAngle-leftAngle);

  int divIndex(bnkIndex+1001);

  double phase(leftPhase);
  const double phaseStep((rightPhase-leftPhase)/static_cast<double>(nSectors));

  SMap.addMatch(divIndex,SMap.realSurf(bnkIndex+3));
  for(size_t i=1;i<nSectors;i++)
    {
      divIndex++;
      phase+=phaseStep;  //Y
      const double F= (sectPhase[i]-leftPhase)/phaseDiff;
      const double angle= leftAngle+F*angleDiff;
            
      Geometry::Vec3D DPosition(Origin-rotCentre); // Y 
      Geometry::Quaternion::calcQRotDeg(sectPhase[i],-ZRotAxis).rotate(DPosition);
      DPosition+=rotCentre;
      
      Geometry::Vec3D DDir(X);      
      Geometry::Quaternion::calcQRotDeg(sectPhase[i]+angle,-ZRotAxis).rotate(DDir);
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
  const std::string Inner=FC.getLinkString(sideIndex);
  const int InnerSurf=FC.getLinkSurf(sideIndex);
  
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
      Out=ModelSupport::getComposite(SMap,bnkIndex," 1 -7 -3 13 5 -106 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out+Inner));
      setCell("leftWall",cellIndex-1);
      lwIndex+=10;
    }
  if (rightWallFlag)
    {
      Out=ModelSupport::getComposite(SMap,bnkIndex," 1 -7 4 -14 5 -106 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out+Inner));
      setCell("rightWall",cellIndex-1);
      rwIndex+=10;
    }
  
  Out=ModelSupport::getComposite(SMap,bnkIndex,lwIndex,rwIndex,
				 " 1 -17 3M -4N -5 15 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out+Inner));
  setCell("floor",cellIndex-1);

  // Main wall not divided
  int divIndex(bnkIndex+1000);

  for(size_t i=0;i<nSectors;i++)
    {
      // Divide the roof into sector as well
      Out=ModelSupport::getComposite(SMap,bnkIndex," 1 -17 6 (106 : -7) -16 ");
      if (i)
	Out+=ModelSupport::getComposite(SMap,divIndex," 1 ");
      else if (leftWallFlag)
	Out+=ModelSupport::getComposite(SMap,bnkIndex," 13  (106 : 3) ");
      else
	Out+=ModelSupport::getComposite(SMap,bnkIndex," 13 ");

      if (i+1!=nSectors)
	Out+=ModelSupport::getComposite(SMap,divIndex," -2 ");
      else if (rightWallFlag)
	  Out+=ModelSupport::getComposite(SMap,bnkIndex," -14  (106 : -4) ");
      else
	Out+=ModelSupport::getComposite(SMap,bnkIndex," -14 ");
      
      System.addCell(MonteCarlo::Qhull(cellIndex++,roofMat,0.0,Out+Inner));
      addCell("roof"+StrFunc::makeString(i),cellIndex-1);
      Out=ModelSupport::getComposite(SMap,bnkIndex,divIndex,
				     " 1 7 -17 1M -2M 5 -106 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
      addCell("frontWall",cellIndex-1);
      addCell("frontWall"+StrFunc::makeString(i),cellIndex-1);
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

  // Note that createSector has to convert these numbers
  // to realSurf so no need to pre-parse via SMap.
  const int outerSurf(bnkIndex+17);

  const int lwIndex((leftWallFlag) ? bnkIndex+10 : bnkIndex);
  const int rwIndex((rightWallFlag) ? bnkIndex+10 : bnkIndex);
  int divIndex(bnkIndex+1000);

  const std::string Out=ModelSupport::getComposite(SMap,bnkIndex," 1 ");
  roofObj->initialize(System.getDataBase(),*this,6);
  roofObj->setVertSurf(SMap.realSurf(bnkIndex+6),SMap.realSurf(bnkIndex+16));
  
  roofObj->setRadialSurf(SMap.realSurf(innerSurf),SMap.realSurf(outerSurf));
  roofObj->setDivider(Out);

  for(size_t i=0;i<nSectors;i++)
    {
      const std::string SectNum(StrFunc::makeString(i));
      const int LW=(i) ? divIndex+1 : lwIndex+3;
      const int RW=(i+1!=nSectors) ? divIndex+2 : rwIndex+4;
      const int cellN=getCell("roof"+SectNum);

      roofObj->createSector(System,i,cellN,
			    SMap.realSurf(LW),SMap.realSurf(RW));
      
      removeCell("roof"+SectNum);
      addCells("roof",roofObj->getCells("Sector"+SectNum));
      if (i+1==nSectors)
        addCells("roofFarEdge",roofObj->getCells("Sector"+SectNum));

      divIndex++;
    }
  return;
}
  
void
Bunker::createMainWall(Simulation& System)
  /*!
    Create a tesselated main wall
    \param System :: Simulation system
  */
{
  ELog::RegMethod RegA("Bunker","createMainWall");


  // Note that createSector has to convert these numbers
  // to realSurf so no need to pre-parse via SMap.
  const int innerSurf(bnkIndex+7);
  const int outerSurf(bnkIndex+17);

  const int lwIndex((leftWallFlag) ? bnkIndex+10 : bnkIndex);
  const int rwIndex((rightWallFlag) ? bnkIndex+10 : bnkIndex);
  int divIndex(bnkIndex+1000);

  const std::string Out=ModelSupport::getComposite(SMap,bnkIndex," 1 ");

  wallObj->initialize(System.getDataBase(),*this,0);
  wallObj->setVertSurf(SMap.realSurf(bnkIndex+5),SMap.realSurf(bnkIndex+106));  //XXX
  wallObj->setRadialSurf(SMap.realSurf(innerSurf),SMap.realSurf(outerSurf));
  wallObj->setDivider(Out);

  for(size_t i=0;i<nSectors;i++)
    {
      const std::string SectNum(StrFunc::makeString(i));
      const int LW=(i) ? divIndex+1 : lwIndex+3;
      const int RW=(i+1!=nSectors) ? divIndex+2 : rwIndex+4;
      const int cellN=getCell("frontWall"+SectNum);

      wallObj->createSector(System,i,cellN,
			    SMap.realSurf(LW),SMap.realSurf(RW));
      
      removeCell("frontWall"+SectNum);
      addCells("frontWall",wallObj->getCells("Sector"+SectNum));
      divIndex++;
    }
  return;  
}

  
void
Bunker::createLinks(const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Create all the links [OutGoing]
    \param FC :: FixedComp use for inner surf
    \param sideIndex :: link index for inner surf
  */
{
  ELog::RegMethod RegA("Bunker","createLinks");

  FixedComp::setLinkSignedCopy(2,FC,sideIndex);
  
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

  // Rotation centre:
  FixedComp::setConnect(6,rotCentre,Y);
  FixedComp::setLinkSurf(6,0);

  // Inner wall
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
    Determine the segment that a point falls in
    \param System :: Simulation System
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
		  const attachSystem::FixedComp& FC,
		  const long int linkIndex,
                  const bool reverseX)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param linkIndex :: linkIndex number
    \param reverseX :: Reverse X direction
  */
{
  ELog::RegMethod RegA("Bunker","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,linkIndex,reverseX);
    
  createSurfaces(reverseX);
  createLinks(FC,linkIndex);
  createObjects(System,FC,linkIndex);
  //  layerProcess(System);
  insertObjects(System);              

  return;
}

}  // NAMESPACE essSystem
