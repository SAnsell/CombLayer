/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   maxivBuild/R1Ring.cxx
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
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedGroup.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "SpaceCut.h"
#include "ContainedSpace.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"

#include "R1Ring.h"

namespace xraySystem
{

R1Ring::R1Ring(const std::string& Key) : 
  attachSystem::FixedOffset(Key,12),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  NPoints(0),concaveNPoints(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  nameSideIndex(2,"outerWall");
  nameSideIndex(3,"ringWall");
  nameSideIndex(4,"floor");
  nameSideIndex(5,"roof");
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
  
  FixedOffset::populate(Control);

  fullOuterRadius=Control.EvalVar<double>(keyName+"FullOuterRadius");
  
  hexRadius=Control.EvalVar<double>(keyName+"HexRadius");
  hexWallThick=Control.EvalVar<double>(keyName+"HexWallThick");


  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  floorThick=Control.EvalVar<double>(keyName+"FloorThick");
  roofThick=Control.EvalVar<double>(keyName+"RoofThick");

  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  floorMat=ModelSupport::EvalMat<int>(Control,keyName+"FloorMat");
  roofMat=ModelSupport::EvalMat<int>(Control,keyName+"RoofMat");

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
    
  return;
}

void
R1Ring::createUnitVector(const attachSystem::FixedComp& FC,
			       const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("R1Ring","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
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
  
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*depth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*height,Z);

  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(depth+floorThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(height+roofThick),Z);

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
      surfN+=10;
    }

  
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

  //  const std::set<size_t> innerPts({1,4,6,8,11,13,15,17,19,21});

  std::string Out;

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " -3 -13 -23 -33 -43 -53 15 -16" );
  makeCell("InnerVoid",System,cellIndex++,0,0.0,Out);

  int prevN(buildIndex+50);
  int surfN(buildIndex);
  for(size_t i=0;i<6;i++)
    {
      Out=ModelSupport::getComposite(SMap,surfN,prevN,buildIndex,
				     " 7M -7 3 -103 15N -16N" );
      makeCell("Wall",System,cellIndex++,wallMat,0.0,Out);
      prevN=surfN;
      surfN+=10;
    }


  // Create inner voids
  std::string Unit;
  surfN=5000;
  for(size_t i=0;i<concaveNPoints;i++)
    {
      Unit+=std::to_string(surfN+9)+" ";
      surfN+=10;
    }
  Out=ModelSupport::getComposite(SMap,buildIndex,Unit+"15 -16");
  Out+=ModelSupport::getComposite
    (SMap,buildIndex,"(103:113:123:133:143:153)");
  makeCell("Void",System,cellIndex++,0,0.0,Out);  

   // loop to make individual units:
  const std::string TBase=
    ModelSupport::getComposite(SMap,buildIndex," 15 -16 ");
  size_t index=0;
  surfN=1000;
  int convexN=5000;
  Out="";
  std::string WOut;
  for(size_t i=1;i<NPoints+2;i++)
    {
      Out+=ModelSupport::getComposite(SMap,buildIndex+surfN," 3 ");
      if (i==concavePts[index])
	{
	  Out+=ModelSupport::getComposite(SMap,buildIndex+convexN," -9 ");
	  if (index)
	    {
	      makeCell("VoidTriangle",System,cellIndex++,0,0.0,Out+TBase);
	      convexN+=10;
	    }

	  index++;
	  Out="";
	}
      surfN= (i==NPoints) ? 1000 : surfN+10;
    }
  
  Out+=ModelSupport::getComposite(SMap,buildIndex+convexN," -9 ");
  makeCell("VoidTri",System,cellIndex++,0,0.0,Out+TBase);

  // WALLS:
  Out=ModelSupport::getComposite
    (SMap,buildIndex," -2003 (-1013:-1023) 1033 2013 2023 15 -16");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," -1033 -1043 1053 2023 (2033:2043) 15 -16");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," -1053 -1063 1073 2043 (2053:2063) 15 -16");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite  
    (SMap,buildIndex," -1073 -1083 2063 2073 15 -16");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," -2073 (-1083:-1093) 2083 2093 1103  15 -16");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," -1103 -1113 1123 2093 (2103:2113) 15 -16");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," -1123 -1133 1143 2113 (2123:2133) 15 -16");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," -1143 -1153 1163 2133 (2143:2153) 15 -16");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," -1163 -1173 1183 2153 (2163:2173) 15 -16");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," -1183 -1193 1203 2173 (2183:2193) 15 -16");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," -1203 -1213 1003 2193 (2203:2213) 15 -16");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,Out);

  Out=ModelSupport::getComposite  
    (SMap,buildIndex," 2213 -1013 2003 -1003 15 -16");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,Out);


  // NOW DO external sub-triangles:
  // segment 1:
  Out=ModelSupport::getComposite  
    (SMap,buildIndex," -9007 2013 -2023 15 -16");
  makeCell("OuterSegment",System,cellIndex++,0,0.0,Out);

  // segment 2:
  Out=ModelSupport::getComposite  
    (SMap,buildIndex," -9007 -2033 2023 -2043 15 -16");
  makeCell("OuterSegment",System,cellIndex++,0,0.0,Out);

  // segment 3:
  Out=ModelSupport::getComposite  
    (SMap,buildIndex," -9007 -2053 2043 -2063 15 -16");
  makeCell("OuterSegment",System,cellIndex++,0,0.0,Out);

  // segment 4:
  Out=ModelSupport::getComposite  
    (SMap,buildIndex," -9007 -2073 2063 -2083 15 -16");
  makeCell("OuterSegment",System,cellIndex++,0,0.0,Out);
  
  // segment 5 [LONG]:
  Out=ModelSupport::getComposite  
    (SMap,buildIndex," -9007 2083 -2093 15 -16");
  makeCell("OuterSegment",System,cellIndex++,0,0.0,Out);

  // segment 6:
  Out=ModelSupport::getComposite  
    (SMap,buildIndex," -9007 -2103 -2113 2093 15 -16");
  makeCell("OuterSegment",System,cellIndex++,0,0.0,Out);

  // segment 7:
  Out=ModelSupport::getComposite  
    (SMap,buildIndex," -9007 -2123 -2133 2113 15 -16");
  makeCell("OuterSegment",System,cellIndex++,0,0.0,Out);

    // segment 8:
  Out=ModelSupport::getComposite  
    (SMap,buildIndex," -9007 -2143 -2153 2133 15 -16");
  makeCell("OuterSegment",System,cellIndex++,0,0.0,Out);

  // segment 9:
  Out=ModelSupport::getComposite  
    (SMap,buildIndex," -9007 -2163 -2173 2153 15 -16");
  makeCell("OuterSegment",System,cellIndex++,0,0.0,Out);

  // segment 10:
  Out=ModelSupport::getComposite  
    (SMap,buildIndex," -9007 -2183 -2193 2173 15 -16");
  makeCell("OuterSegment",System,cellIndex++,0,0.0,Out);

  // segment 11:
  Out=ModelSupport::getComposite  
    (SMap,buildIndex," -9007 -2203 -2213 2193 15 -16");
  makeCell("OuterSegment",System,cellIndex++,0,0.0,Out);
  
  // segment 12:
  Out=ModelSupport::getComposite  
    (SMap,buildIndex," -9007 -2003 -2013 2213 15 -16");
  makeCell("OuterSegment",System,cellIndex++,0,0.0,Out);


  Out=ModelSupport::getComposite(SMap,buildIndex,"-9007 15 -16");
  addOuterSurf(Out);  
  return;
  Out=ModelSupport::getComposite
    (SMap,buildIndex," -1093 -1103 1113 2083 (2093:2103) 15 -16");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,Out);


  
  return;

  // WALLS:

  
  
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
  
  const double beamInStep(100.0);
  // Main beam start points DONT have a surface [yet]
  for(size_t i=0;i<concaveNPoints;i++)
    {
      const Geometry::Vec3D& Pt(voidTrack[concavePts[i]]);
      const Geometry::Vec3D Axis=(Origin-Pt).unit();
      const Geometry::Vec3D Beam=Axis*Z;
      ELog::EM<<"Beam == "<<Pt<<" :: "<<Beam<<ELog::endDiag;
      FixedComp::setConnect(i+2,Pt-Axis*beamInStep,Beam);
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
  return;
}
  
}  // NAMESPACE xraySystem
