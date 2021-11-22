/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   R1Common/R1Ring.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
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
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "PointMap.h"
#include "insertObject.h"
#include "insertPlate.h"

#include "RingDoor.h"
#include "SideShield.h"
#include "R1Ring.h"

namespace xraySystem
{

R1Ring::R1Ring(const std::string& Key) : 
  attachSystem::FixedOffset(Key,24),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::PointMap(),
  NPoints(0),concaveNPoints(0),
  doorActive(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
}

R1Ring::~R1Ring() 
  /*!
    Destructor
  */
{}

void
R1Ring::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("R1Ring","populate");
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  FixedOffset::populate(Control);

  fullOuterRadius=Control.EvalVar<double>(keyName+"FullOuterRadius");
  
  hexRadius=Control.EvalVar<double>(keyName+"HexRadius");
  hexWallThick=Control.EvalVar<double>(keyName+"HexWallThick");


  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  floorThick=Control.EvalVar<double>(keyName+"FloorThick");
  roofThick=Control.EvalVar<double>(keyName+"RoofThick");
  roofExtra=Control.EvalVar<double>(keyName+"RoofExtraVoid");

  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  floorMat=ModelSupport::EvalMat<int>(Control,keyName+"FloorMat");
  roofMat=ModelSupport::EvalMat<int>(Control,keyName+"RoofMat");
  innerMat=ModelSupport::EvalDefMat(Control,keyName+"InnerMat",0);
  outerMat=ModelSupport::EvalDefMat(Control,keyName+"OuterMat",0);

  NPoints=Control.EvalVar<size_t>(keyName+"NPoints");
  for(size_t i=0;i<NPoints;i++)
    {
      const std::string NStr=std::to_string(i);
      const Geometry::Vec3D PtA=
	Control.EvalVar<Geometry::Vec3D>(keyName+"VPoint"+NStr);
      const Geometry::Vec3D PtB=
	Control.EvalVar<Geometry::Vec3D>(keyName+"OPoint"+NStr);
      voidTrack.push_back(PtA);
      outerTrack.push_back(PtB);
    }
  for(size_t i=1;i<NPoints+1;i++)
    {
      const Geometry::Vec3D& C(voidTrack[i % NPoints]);
      const Geometry::Vec3D A((voidTrack[i-1]-C).unit());
      const Geometry::Vec3D B((voidTrack[(i+1) % NPoints]-C).unit());
      Geometry::Vec3D XA=A*B;
      if (XA.dotProd(Z)<0.0)
	concavePts.push_back(i);
    }
  concaveNPoints=concavePts.size();

  // SIDE 
  const size_t nSide=Control.EvalVar<size_t>(keyName+"NSideWall");
  for(size_t i=0;i<nSide;i++)
    {
      const std::string sideKey=
	Control.EvalVar<std::string>(keyName+"SideWallName"+std::to_string(i));
      
      const size_t ID=Control.EvalVar<size_t>(sideKey+"ID");
      std::shared_ptr<SideShield>
	shieldPtr(new SideShield(keyName+"SideWall",sideKey));
      OR.addObject(shieldPtr);
      sideShields.emplace(ID,shieldPtr);
    }
  // FREE
  const size_t nFree=Control.EvalVar<size_t>(keyName+"NFreeShield");
  for(size_t i=0;i<nFree;i++)
    {
      const std::string freeKey=
	Control.EvalVar<std::string>
	(keyName+"FreeShieldName"+std::to_string(i));
      
      const size_t ID=Control.EvalVar<size_t>(freeKey+"ID");

      std::shared_ptr<insertSystem::insertPlate>
	platePtr(new insertSystem::insertPlate(keyName+"FreeShield",freeKey));
      OR.addObject(platePtr);
      plateShields.emplace(ID,platePtr);
    }

  // OUTER SHIELD
  const size_t nOuter=Control.EvalVar<size_t>(keyName+"NOutShield");
  for(size_t i=0;i<nOuter;i++)
    {
      const std::string outerKey=
	Control.EvalVar<std::string>(keyName+"OutShieldName"+std::to_string(i));
      
      const size_t ID=Control.EvalVar<size_t>(outerKey+"ID");
      std::shared_ptr<insertSystem::insertPlate>
	shieldPtr(new insertSystem::insertPlate(keyName+"OutShield",outerKey));
      OR.addObject(shieldPtr);
      outShields.emplace(ID,shieldPtr);
    }

  doorActive=Control.EvalDefVar<size_t>(keyName+"RingDoorWallID",0);
  
  return;
}
 
void
R1Ring::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("R1Ring","createSurfaces");

  ModelSupport::buildCylinder(SMap,buildIndex+9007,Origin,Z,fullOuterRadius);
  
  // quick way to rotate outgoing vector to
  // dividing vector
  const Geometry::Quaternion Qz=Geometry::Quaternion::calcQRotDeg(-120.0,Z);
  double theta(0.0);
  int surfN(buildIndex);
  
  for(size_t i=0;i<6;i++)
    {
      Geometry::Vec3D Axis(sin(theta),cos(theta),0.0);
      ModelSupport::buildPlane(SMap,surfN+3,Origin+Axis*hexRadius,Axis);
      ModelSupport::buildPlane(SMap,surfN+103,
			       Origin+Axis*(hexRadius+hexWallThick),Axis);
      // divider vector
      ModelSupport::buildPlane(SMap,surfN+7,Origin,Qz.rotate(Axis));
      theta+=M_PI/3.0;
      surfN+=10;
    }
  
  SurfMap::makePlane("Floor",SMap,buildIndex+5,Origin-Z*depth,Z);
  SurfMap::makePlane("Roof",SMap,buildIndex+6,Origin+Z*height,Z);

  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(depth+floorThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(height+roofThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+26,
			   Origin+Z*(height+roofThick+roofExtra),Z);
    
  surfN=buildIndex+1000;
  for(size_t i=0;i<NPoints;i++)
    {
      const Geometry::Vec3D AP(X*voidTrack[i].X()+Y*voidTrack[i].Y());
      const Geometry::Vec3D BP
	(X*voidTrack[(i+1)% NPoints].X()+Y*voidTrack[(i+1) % NPoints].Y());
      // note that voidTrack is about (0,0,0)
      const Geometry::Vec3D NDir=(-BP);  
      
      ModelSupport::buildPlane(SMap,surfN+3,
			       Origin+AP,
			       Origin+BP,
			       Origin+BP+Z,NDir);

      surfN+=10;
    }

  size_t cIndex(0);
  surfN=buildIndex+2000;
  for(size_t i=0;i<NPoints;i++)
    {
      const Geometry::Vec3D AP(X*outerTrack[i].X()+Y*outerTrack[i].Y());
      const Geometry::Vec3D BP
	(X*outerTrack[(i+1)% NPoints].X()+Y*outerTrack[(i+1) % NPoints].Y());
      const Geometry::Vec3D NDir=(Origin-BP);
      
      ModelSupport::buildPlane(SMap,surfN+3,
			       Origin+AP,Origin+BP,
			       Origin+BP+Z,NDir);
      // trick to get exit walls [inner /outer]
      if (concavePts[cIndex]==i+1)
	{
	  SurfMap::addSurf("BeamInner",SMap.realSurf(surfN-1000+3));
	  SurfMap::addSurf("BeamOuter",SMap.realSurf(surfN+3));
	  PointMap::addPoint("OutCorner",AP);
	  if (cIndex)
	    {
	      SurfMap::addSurf("SideInner",-SMap.realSurf(surfN-1010+3));
	      SurfMap::addSurf("SideOuter",-SMap.realSurf(surfN-10+3));

	    }
	  cIndex = (cIndex+1) % concaveNPoints;
	}
      surfN+=10;
    }
  // last wall
  SurfMap::addSurf("SideInner",SMap.realSurf(surfN-1010+3));
  SurfMap::addSurf("SideOuter",SMap.realSurf(surfN-10+3));
	  

  // Exit wall dividers
  surfN=buildIndex+3000;
  for(size_t i=0;i<concaveNPoints;i++)
    {
      const size_t aI=concavePts[i];
      const Geometry::Vec3D AP(X*voidTrack[aI].X()+Y*voidTrack[aI].Y());
      const Geometry::Vec3D BP(X*outerTrack[aI].X()+Y*outerTrack[aI].Y());
      PointMap::addPoint("OutSideWall",BP);
      // going round ring clockwize 
      const Geometry::Vec3D NDir=((BP-AP)*Z).unit();      
      ModelSupport::buildPlane(SMap,surfN+1,
			       Origin+AP,Origin+BP,
			       Origin+BP+Z,NDir);
      // Joint Plane error cylinders:
      ModelSupport::buildCylinder(SMap,surfN+7,Origin+AP,Z,0.1);
      ModelSupport::buildCylinder(SMap,surfN+8,Origin+BP,Z,0.1);


      surfN+=10;
    } 

  // Inner cut points
  surfN=buildIndex+5000;
  for(size_t i=1;i<concaveNPoints+1;i++)
    {
      const size_t aI=concavePts[i-1];
      const size_t bI=concavePts[i % concaveNPoints];
      const Geometry::Vec3D AP(X*voidTrack[aI].X()+Y*voidTrack[aI].Y());
      const Geometry::Vec3D BP(X*voidTrack[bI].X()+Y*voidTrack[bI].Y());
      const Geometry::Vec3D NDir=(-BP);
      
      ModelSupport::buildPlane(SMap,surfN+9,
			       Origin+AP,Origin+BP,
			       Origin+BP+Z,NDir);
      SurfMap::addSurf("InnerCut",SMap.realSurf(surfN+9));

      ModelSupport::buildPlane(SMap,surfN+1001,
			       Origin+AP,Origin,
			       Origin+Z,BP-AP);
      SurfMap::addSurf("DivideCut",SMap.realSurf(surfN+1001));
      surfN+=10;      
    }  
  
  return;
}

void
R1Ring::createFloor(Simulation& System)
  /*!
    Adds the main roof objects
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("R1Ring","createFloor");
  std::string Out;
  
  Out=ModelSupport::getComposite(SMap,buildIndex,"-9007 -5 15 ");
  makeCell("Floor",System,cellIndex++,floorMat,0.0,Out);
  return;
}


void
R1Ring::createRoof(Simulation& System)
  /*!
    Adds the main roof objects
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("R1Ring","createRoof");

  std::string Out;
  

  const std::string TBase=
    ModelSupport::getComposite(SMap,buildIndex," 6 -16 ");
  const std::string EBase=
    ModelSupport::getComposite(SMap,buildIndex," 16 -26 ");

    // Create inner voids
  std::ostringstream unitCX;
  int surfN=0;
  for(size_t i=0;i<concaveNPoints;i++)
    {
      unitCX<<surfN+5009<<" "<<surfN+3007<<" ";
      surfN+=10;
    }

  Out=ModelSupport::getComposite(SMap,buildIndex,unitCX.str());
  Out+=ModelSupport::getComposite
    (SMap,buildIndex,"(103:113:123:133:143:153)");
  makeCell("InnerRoof",System,cellIndex++,roofMat,0.0,Out+TBase);
  makeCell("InnerExtra",System,cellIndex++,0,0.0,Out+EBase);  
  
  return;
}

void
R1Ring::createObjects(Simulation& System)
  /*!
    Adds the main objects
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("R1Ring","createObjects");

  std::string Out;

  createRoof(System);
  createFloor(System);

  const std::string roofBase=
    ModelSupport::getComposite(SMap,buildIndex," 6 -16 ");
  const std::string wallBase=
    ModelSupport::getComposite(SMap,buildIndex," 5 -16 ");
  const std::string extraBase=
    ModelSupport::getComposite(SMap,buildIndex," 16 -26 ");
  const std::string fullBase=
    ModelSupport::getComposite(SMap,buildIndex," 5 -26 ");
  const std::string innerBase=
    ModelSupport::getComposite(SMap,buildIndex," 5 -6 ");

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " -3 -13 -23 -33 -43 -53 " );
  makeCell("InnerVoid",System,cellIndex++,0,0.0,Out+fullBase);

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " -103 -113 -123 -133 -143 -153 "
				 " (3 : 13 : 23 : 33 : 43 : 53) ");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,Out+wallBase);
  makeCell("WallExtra",System,cellIndex++,wallMat,0.0,Out+extraBase);


  // loop to make individual units:
  const std::vector<std::string> MidZone
    ({
      "5099 103 6091 -6001 3097 3007",
      "5009 (103:113) 6001 -6011 3007 3017",
      "5019 (113:123) 6011 -6021 3017 3027",
      "5029 (113:123) 6021 -6031 3027 3037",
      "5039 (123:133) 6031 -6041 3037 3047",
      "5049 (133:143) 6041 -6051 3047 3057",
      "5059       143 6051 -6061 3057 3067",
      "5069 (143:153) 6061 -6071 3067 3077",
      "5079      153  6071 -6081 3077 3087",
      "5089 (153:103) 6081 -6091 3087 3097",
	});
  for(const std::string& Item : MidZone)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex,Item);
      makeCell("Void",System,cellIndex++,innerMat,0.0,Out+innerBase);
    }
  
  // loop to make individual units:
  const std::vector<std::string> Voids
    ({
      "1013 1023 1033 -5009 3007 3017",
	"1043 1053 -5019 3017 3017 3027",
	"1063 1073 -5029 3027 3027 3037",
	"1083 1093 1103 -5039 3037 3047",
	"1113 1123 -5049 3037 3047 3057",
	"1133 1143 -5059 3047 3057 3067",
	"1153 1163 -5069 3057 3067 3077",
	"1173 1183 -5079 3067 3077 3087",
	"1193 1203 -5089 3077 3087 3097",
	"1213 1003 -5099 3087 3097 3007"
	});
  // cylinder exludes:
  int BI=buildIndex+3000;
  for(size_t i=0;i<concaveNPoints;i++)
    {
      Out=ModelSupport::getComposite(SMap,BI," -7 ");
      std::string OutB=ModelSupport::getComposite(SMap,BI," -8 ");
      makeCell("VoidCyl",System,cellIndex++,0,0.0,Out+innerBase);
      makeCell("RoofCyl",System,cellIndex++,roofMat,0.0,Out+roofBase);
      makeCell("SkyCyl",System,cellIndex++,0,0.0,Out+extraBase);
      makeCell("VoidCylB",System,cellIndex++,0,0.0,OutB+innerBase);
      makeCell("RoofCylB",System,cellIndex++,roofMat,0.0,OutB+roofBase);
      makeCell("SkyCylB",System,cellIndex++,0,0.0,OutB+extraBase);
      BI+=10;
    }
    
  for(const std::string& item : Voids)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex,item);
      makeCell("VoidTriangle",System,cellIndex++,0,0.0,Out+innerBase);
      makeCell("RoofTriangle",System,cellIndex++,roofMat,0.0,Out+roofBase);
      makeCell("RoofExtra",System,cellIndex++,0,0.0,Out+extraBase);
    }


  // WALLS:
  const std::vector<std::string> frontWalls
    ({  " 2213 -1003 -1013  2003 3007 3008",
	"-1033  2023  2033 -3011 3017 3018",
	"-1053  2043  2053 -3021 3027 3028",
	" 2063 -1073 -1083  2073 3037 3038",
	"-1103  2093  2103 -3041 3047 3048",
	"-1123  2113  2123 -3051 3057 3058",
	"-1143  2133  2143 -3061 3067 3068",
	"-1163  2153  2163 -3071 3077 3078",
	"-1183  2173  2183 -3081 3087 3088",
	"-1203  2193  2203 -3091 3097 3098"       
      });

  const std::vector<std::string> walls
    ({
      "-2003 (-1013 : -1023) 1033 2013 2023 3008 ",
      "(3011 : -2033) -1043 1053 2043 3017 3018",   
      "(3021 : -2053) -1063 1073 2063 3027 3028 ",  
      "-2073 (-1083 : -1093) 1103 2083 2093 3038",
      "(3041 : -2103) -1113 1123 2113 3047 3048",        
      "(3051 : -2123) -1133 1143 2133 3057 3058",        
      "(3061 : -2143) -1153 1163 2153 3067 3068",
      "(3071 : -2163) -1173 1183 2173 3077 3078",
      "(3081 : -2183) -1193 1203 2193 3087 3088",
      "3091 -1213 1003 2213 3097 3098"  
    });

  // Front walls:
  for(const std::string& item : frontWalls)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex,item);
      makeCell("FrontWall",System,cellIndex++,wallMat,0.0,Out+wallBase);
      makeCell("FrontExtra",System,cellIndex++,0,0.0,Out+extraBase);
    }      

  // Main walls:
  for(const std::string& item : walls)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex,item);
      makeCell("Wall",System,cellIndex++,wallMat,0.0,Out+wallBase);
      makeCell("Extra",System,cellIndex++,0,0.0,Out+extraBase);
      //      makeCell("RoofTriangle",System,cellIndex++,roofMat,0.0,Out+roofBase);
      //      makeCell("RoofExtra",System,cellIndex++,0,0.0,Out+extraBase);
    }      

  // EXTERNAL void-triangles:
  const std::vector<std::string> extTriangle
    ({
      "-9007 -2003 -2013 2213 3008",
      "-9007 -2023 2013 ",        
      "-9007 -2033 -2043 2023 3018",
      "-9007 -2053 -2063 2043 3028",
      "-9007 -2073 -2083 2063 3038",
      "-9007 -2093 2083 ",         
      "-9007 -2103 -2113 2093 3048",
      "-9007 -2123 -2133 2113 3058",
      "-9007 -2143 -2153 2133 3068",
      "-9007 -2163 -2173 2153 3078",
      "-9007 -2183 -2193 2173 3088",
      "-9007 -2203 -2213 2193 3098" 
    });
  for(const std::string& item : extTriangle)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex,item);
      makeCell("OuterSegment",System,cellIndex++,outerMat,0.0,Out+fullBase);
    }
	  
  Out=ModelSupport::getComposite(SMap,buildIndex,"-9007 15 -26");
  addOuterSurf(Out);    
  return;
}

void
R1Ring::createLinks()
  /*!
    Determines the link points for the beam direction first:
    This is special : each beamport has two coordinates (and axis)
    First is the mid triangle point [ start of straight section]
    The second is the mid point on the wall it points to
  */
{
  ELog::RegMethod RegA("R1Ring","createLinks");

  FixedComp::setNConnect(2*concaveNPoints+2);
  
  const double beamStepOut(1510.55);  // from origin
  // Main beam start points DONT have a surface [yet]

  size_t index(0);
  for(size_t i=1;i<concaveNPoints+1;i++)
    {
      // InnerWall
      const Geometry::Plane* BInner=dynamic_cast<const Geometry::Plane*>
	(SurfMap::getSurfPtr("BeamInner",((i+1) % concaveNPoints)));
      if (!BInner)
	throw ColErr::InContainerError<std::string>
	  ("BeamInner"+std::to_string(index),"Surf map no found");

      const Geometry::Vec3D Beam= -BInner->getNormal();
      const Geometry::Vec3D Axis= -Beam*Z;
      const Geometry::Vec3D PtX=Origin+Axis*beamStepOut;
      FixedComp::nameSideIndex(index+2,"OpticCentre"+std::to_string(index));
      FixedComp::setLinkSurf(index+2,-BInner->getName());
      FixedComp::setConnect(index+2,PtX,Beam);
      index++;
    }

  for(size_t i=0;i<concaveNPoints;i++)
    {

      const Geometry::Plane* SOuter=dynamic_cast<const Geometry::Plane*>
	(SurfMap::getSurfPtr("SideOuter",i));
      if (!SOuter)
	throw ColErr::InContainerError<std::string>
	  ("SideOuter"+std::to_string(index),"Surf map no found");

      const Geometry::Vec3D& APt=
	PointMap::getPoint("OutCorner",(i+1) % concaveNPoints);
      const Geometry::Vec3D& BPt(PointMap::getPoint("OutSideWall",i));
      const Geometry::Vec3D Beam= -SOuter->getNormal();

      FixedComp::nameSideIndex(index+2,"SideWall"+std::to_string(i));
      FixedComp::setLinkSurf(index+2,-SOuter->getName());
      FixedComp::setConnect(index+2,(APt+BPt)/2.0,Beam);
      index++;
    }
  return;
}

void
R1Ring::createDoor(Simulation& System)
  /*!
    Build if a ring-door is required
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("R1Ring","createDoor");
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  if (doorActive)
    {
      doorPtr=std::make_shared<xraySystem::RingDoor>(keyName+"RingDoor");
      OR.addObject(doorPtr);

      doorPtr->setCutSurf
	("innerWall",SurfMap::getSurf("SideInner",doorActive-1));
      doorPtr->setCutSurf
	("outerWall",-SurfMap::getSurf("SideOuter",doorActive-1));
      doorPtr->setCutSurf
	("floor",SurfMap::getSurf("Floor"));

      doorPtr->addAllInsertCell(getCell("Wall",doorActive % 10));
      doorPtr->createAll(System,*this,doorActive+2);
    }
  return;
}

void
R1Ring::createSideShields(Simulation& System)
  /*!
    Create side shields only: 
    \param System :: Simulation to add data 
   */
{
  ELog::RegMethod RegA("R1Ring","createSideShields");

  // int : shared_ptr<SideShield>
  for(auto& [ id , SWPtr ] : sideShields)
    {
      SWPtr->addInsertCell(CellMap::getCell("VoidTriangle",id));
      SWPtr->setCutSurf("Wall",-SurfMap::getSurf("SideInner",id));
      SWPtr->setCutSurf("Clip",-SurfMap::getSurf("InnerCut",id));
      SWPtr->createAll(System,*this,"OpticCentre"+std::to_string(id));      
    }
  return;
}

void
R1Ring::createFreeShields(Simulation& System)
  /*!
    Create free standing shields 
    \param System :: Simulation to add data 
   */
{
  ELog::RegMethod RegA("R1Ring","createFreeShields");

  // int : shared_ptr<SideShield>
  for(auto& [ id , PWPtr ] : plateShields)
    {
      PWPtr->setNoInsert();
      PWPtr->addInsertCell(CellMap::getCell("VoidTriangle",id));
      PWPtr->createAll(System,*this,"OpticCentre"+std::to_string(id-1));      
    }

  for(auto& [ id , PWPtr ] : outShields)
    {
      //      PWPtr->setNoInsert();
      //      PWPtr->addInsertCell(CellMap::getCell("VoidTriangle",id));
      PWPtr->createAll(System,*this,"SideWall"+std::to_string(id));      
    }
  return;
}


void
R1Ring::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("R1Ring","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  
  createSurfaces();    
  createObjects(System);
  createLinks();
  insertObjects(System);

  createSideShields(System);
  createFreeShields(System);
  
  createDoor(System);
  return;
}

}  // NAMESPACE xraySystem
