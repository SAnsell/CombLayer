/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/MidWaterDivider.cxx 
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include "surfDivide.h"
#include "surfDIter.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "RuleSupport.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "Vert2D.h"
#include "Convex2D.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "LayerComp.h"
#include "CellMap.h"
#include "AttachSupport.h"
#include "geomSupport.h"
#include "ModBase.h"
#include "H2Wing.h"
#include "MidWaterDivider.h"
#include "SurInter.h"

namespace essSystem
{

MidWaterDivider::MidWaterDivider(const std::string& baseKey,
				 const std::string& extraKey) :
  attachSystem::ContainedComp(),
  attachSystem::LayerComp(0,0),
  attachSystem::FixedComp(baseKey+extraKey,10),
  baseName(baseKey),
  divIndex(ModelSupport::objectRegister::Instance().cell(keyName)),
  cellIndex(divIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param baseKey :: Base Name for item in search
    \param extraKey :: extra [specialized] Name for item in search
  */
{}

MidWaterDivider::MidWaterDivider(const MidWaterDivider& A) : 
  attachSystem::ContainedComp(A),attachSystem::LayerComp(A),
  attachSystem::FixedComp(A), baseName(A.baseName),
  divIndex(A.divIndex),cellIndex(A.cellIndex),midYStep(A.midYStep),
  midAngle(A.midAngle),length(A.length),height(A.height),
  wallThick(A.wallThick),modMat(A.modMat),wallMat(A.wallMat),
  modTemp(A.modTemp),edgeRadius(A.edgeRadius)
  /*!
    Copy constructor
    \param A :: MidWaterDivider to copy
  */
{}

MidWaterDivider&
MidWaterDivider::operator=(const MidWaterDivider& A)
  /*!
    Assignment operator
    \param A :: MidWaterDivider to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::LayerComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      midYStep=A.midYStep;
      midAngle=A.midAngle;
      length=A.length;
      height=A.height;
      wallThick=A.wallThick;
      modMat=A.modMat;
      wallMat=A.wallMat;
      modTemp=A.modTemp;
      edgeRadius=A.edgeRadius;
    }
  return *this;
}

MidWaterDivider*
MidWaterDivider::clone() const
  /*!
    Virtual copy constructor
    \return new (this)
   */
{
  return new MidWaterDivider(*this);
}
  
MidWaterDivider::~MidWaterDivider() 
  /*!
    Destructor
  */
{}

void
MidWaterDivider::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("MidWaterDivider","populate");

  cutLayer=Control.EvalDefVar<size_t>(keyName+"CutLayer",3);
  
  midYStep=Control.EvalVar<double>(keyName+"MidYStep");
  midAngle=Control.EvalVar<double>(keyName+"MidAngle");

  length=Control.EvalVar<double>(keyName+"Length");
  height=Control.EvalDefVar<double>(keyName+"Height",-1.0);
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  
  modMat=ModelSupport::EvalMat<int>(Control,keyName+"ModMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  modTemp=Control.EvalVar<double>(keyName+"ModTemp");
  edgeRadius=Control.EvalVar<double>(keyName+"EdgeRadius");

  totalHeight=Control.EvalPair<double>(keyName,baseName,"TotalHeight");
  if (height<Geometry::zeroTol)
    height=totalHeight-2.0*wallThick;

  return;
}
  
void
MidWaterDivider::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    - Y Points down the MidWaterDivider direction
    - X Across the MidWaterDivider
    - Z up (towards the target)
    \param FC :: fixed Comp [and link comp]
  */
{
  ELog::RegMethod RegA("MidWaterDivider","createUnitVector");

  FixedComp::createUnitVector(FC);
  return;
}

void
MidWaterDivider::createLinks(const H2Wing &LA, const H2Wing &RA)
  /*!
    Construct links for the triangle moderator
    The normal 1-3 and 5-6 are plane,
    7,8,9 are 
  */
{
  ELog::RegMethod RegA("MidWaterDivider","createLinks");

  // Loop over corners that are bound by convex
  const Geometry::Plane *pz = SMap.realPtr<Geometry::Plane>(divIndex+5);

  const Geometry::Plane *p103 = SMap.realPtr<Geometry::Plane>(divIndex+103);
  const Geometry::Plane *p104 = SMap.realPtr<Geometry::Plane>(divIndex+104);
  //FixedComp::setConnect(0, Origin+X*LStep, X); // x+
  FixedComp::setConnect(0, SurInter::getPoint(p103, p104, pz), X);
  //  ELog::EM << "Why x and y are swapped when the commented line is called instead of the line above? Are the plane numbers correct?" << ELog::endCrit;
  FixedComp::setLinkSurf(0, -SMap.realSurf(divIndex+103));
  FixedComp::addLinkSurf(0, SMap.realSurf(divIndex+104));

  const Geometry::Plane *p123 = SMap.realPtr<Geometry::Plane>(divIndex+123);
  const Geometry::Plane *p124 = SMap.realPtr<Geometry::Plane>(divIndex+124);
  //  FixedComp::setConnect(1, Origin-X*LStep, -X); // x-
  FixedComp::setConnect(1, SurInter::getPoint(p123, p124, pz), -X);
  //ELog::EM << "Why x and y are swapped when the commented line is called instead of the line above? Are the plane numbers correct?" << ELog::endCrit;
  FixedComp::setLinkSurf(1, -SMap.realSurf(divIndex+123));
  FixedComp::addLinkSurf(1, SMap.realSurf(divIndex+124));

  // minor links
  const Geometry::Plane *pWater, *pWing;
  int iWater; // mid water surface index

  // === Intersections of water divider and outer/inner H2Wing surfaces ===
  //     Inner link points are used by F5Collimators
  // !!! Signs of surfaces for outer and inner link points must be different - check this!!!

  for (int i=0; i<2; i++) // 0=outer H2Wing surface, 1=inner H2Wing surface
    {
      // x+y+
      iWater = divIndex+131;
      pWater = SMap.realPtr<Geometry::Plane>(iWater);
      pWing = SMap.realPtr<Geometry::Plane>(LA.getLinkSurf(0+i*8));
      FixedComp::setConnect(2+i*4, SurInter::getPoint(pWater, pWing, pz), pWing->getNormal());
      FixedComp::setLinkSurf(2+i*4, SMap.realSurf(iWater));

      // x-y+
      iWater = divIndex+111;
      pWater = SMap.realPtr<Geometry::Plane>(iWater);
      pWing = SMap.realPtr<Geometry::Plane>(LA.getLinkSurf(2+i*8));
      FixedComp::setConnect(3+i*4, SurInter::getPoint(pWater, pWing, pz), pWing->getNormal());
      FixedComp::setLinkSurf(3+i*4, SMap.realSurf(iWater));

      // x-y-
      iWater = divIndex+112;
      pWater = SMap.realPtr<Geometry::Plane>(iWater);
      pWing = SMap.realPtr<Geometry::Plane>(RA.getLinkSurf(0+i*8));
      FixedComp::setConnect(4+i*4, SurInter::getPoint(pWater, pWing, pz), pWing->getNormal());
      FixedComp::setLinkSurf(4+i*4, -SMap.realSurf(iWater));
  
      // x+y-
      iWater = divIndex+132;
      pWater = SMap.realPtr<Geometry::Plane>(iWater);
      pWing = SMap.realPtr<Geometry::Plane>(RA.getLinkSurf(2+i*8));
      FixedComp::setConnect(5+i*4, SurInter::getPoint(pWater, pWing, pz), pWing->getNormal());
      FixedComp::setLinkSurf(5+i*4, -SMap.realSurf(iWater));
    }
  //  !!! check if surface signs in setLinkSurf are ok. Since there are always 2 surfaces, use addLinkSurf for link points 2-5  in the same way as it is done for 0-1 !!!
  // postponed until I understand surfaces in the link points 0 and 1 - I think 0 should use surfaces 123 and 124, and 1 - 103 and 104

  return;
}
  


void
MidWaterDivider::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("MidWaterDivider","createSurface");

  // Mid divider
  ModelSupport::buildPlane(SMap,divIndex+100,Origin,Y);
  ModelSupport::buildPlane(SMap,divIndex+300,Origin,X);

  // +Y section
  ModelSupport::buildPlaneRotAxis
    (SMap,divIndex+3,Origin+Y*midYStep,X,-Z,-midAngle/2.0);
  ModelSupport::buildPlaneRotAxis
    (SMap,divIndex+4,Origin+Y*midYStep,X,-Z,midAngle/2.0);

  // -Y section
  ModelSupport::buildPlaneRotAxis(SMap,divIndex+23,
				  Origin-Y*midYStep,-X,-Z,-midAngle/2.0);
  ModelSupport::buildPlaneRotAxis(SMap,divIndex+24,
				  Origin-Y*midYStep,-X,-Z,midAngle/2.0);

  // Make lengths:

  Geometry::Vec3D leftNorm(Y);
  Geometry::Quaternion::calcQRotDeg(-midAngle/2.0,Z).rotate(leftNorm);  
  Geometry::Vec3D rightNorm(Y);
  Geometry::Quaternion::calcQRotDeg(midAngle/2.0,Z).rotate(rightNorm);  
  
  ModelSupport::buildPlane(SMap,divIndex+11,Origin+leftNorm*length,leftNorm);
  ModelSupport::buildPlane(SMap,divIndex+12,Origin+rightNorm*length,rightNorm);

  // Length below [note reverse of normals]
  ModelSupport::buildPlane(SMap,divIndex+31,
			   Origin-rightNorm*length,-rightNorm);
  ModelSupport::buildPlane(SMap,divIndex+32,
			   Origin-leftNorm*length,-leftNorm);

  
  // Aluminum layers [+100]
  Geometry::Vec3D leftNormAl(X);
  Geometry::Quaternion::calcQRotDeg(midAngle/2.0,Z).rotate(leftNormAl);  
  Geometry::Vec3D rightNormAl(X);
  Geometry::Quaternion::calcQRotDeg(-midAngle/2.0,Z).rotate(rightNormAl);  

  // +Y section
  ModelSupport::buildPlane(SMap,divIndex+103,Origin+Y*midYStep+leftNormAl*wallThick, leftNormAl);
  ModelSupport::buildPlane(SMap,divIndex+104,Origin+Y*midYStep-rightNormAl*wallThick, rightNormAl);

  // -Y section
  ModelSupport::buildPlane(SMap,divIndex+123,Origin-Y*midYStep-leftNormAl*wallThick, leftNormAl);
  ModelSupport::buildPlane(SMap,divIndex+124,Origin-Y*midYStep+rightNormAl*wallThick, rightNormAl);
  
  ModelSupport::buildPlane(SMap,divIndex+111,
			   Origin+leftNorm*(length+wallThick),leftNorm); // x-y+
  ModelSupport::buildPlane(SMap,divIndex+112,
			   Origin+rightNorm*(length+wallThick),rightNorm); // x-y-

  // Length below [note reverse of normals]
  ModelSupport::buildPlane(SMap,divIndex+131,
			   Origin-rightNorm*(wallThick+length),-rightNorm); // x+y+
  ModelSupport::buildPlane(SMap,divIndex+132,
			   Origin-leftNorm*(wallThick+length),-leftNorm); // x+y-

  // Rounding of the edges
  const Geometry::Plane *pz = ModelSupport::buildPlane(SMap, divIndex+5, Origin, Z);

  int edgeOffset(divIndex+1000);
  std::array<Geometry::Vec3D,4> CPts; // water corners
  std::array<Geometry::Vec3D,4> APts; // points for Geometry::cornerCircle
  std::vector<int> side{11, 12, 32, 31};
  std::vector<int> front{4, 3,  24, 23};
  double thick=0.0;

  //  std::cout << "before" << std::endl;
  //  SMap.realPtr<Geometry::Plane>(divIndex+side[4]+100);
  //  ELog::EM << "after" << ELog::endErr;
  
  for (size_t j=0; j<2; j++) // water and Al
    {
      for (size_t i=0; i<4; i++) // four edges
	{
	  CPts[i] = SurInter::getPoint(SMap.realPtr<Geometry::Plane>(divIndex+side[i]+j*100),
				       SMap.realPtr<Geometry::Plane>(divIndex+front[i] + j*100), pz); // water corners
	  
	  if ((i==0) || (i==2))
	    APts[i] = SurInter::getPoint(SMap.realPtr<Geometry::Plane>(divIndex+side[i]+j*100),
					 SMap.realPtr<Geometry::Plane>(divIndex+side[(i+3)%4]+j*100), pz);
	  else if ((i==1) || (i==3))
	    APts[i] = SurInter::getPoint(SMap.realPtr<Geometry::Plane>(divIndex+front[i-1]+j*100), SMap.realPtr<Geometry::Plane>(divIndex+front[i]+j*100), pz);
	}
      
      for (size_t i=0; i<4; i++) // four edges
	{
	  const int ii(static_cast<int>(i)+1);

	  Geometry::Vec3D RCent;
	  RCent = Geometry::cornerCircleTouch(CPts[i], APts[i], APts[(i+1)%4], edgeRadius+thick);

	  std::pair<Geometry::Vec3D, Geometry::Vec3D> CutPair;
	  CutPair =    Geometry::cornerCircle(CPts[i], APts[i], APts[(i+1)%4], edgeRadius+thick);

	  // midNorm
	  Geometry::Vec3D a = (CPts[(i+1)%4]-CPts[i]).unit();
	  Geometry::Vec3D b = (CPts[(i+2)%4]-CPts[i]).unit();
	  Geometry::Vec3D MD = (a+b)/2.0;
	  
	  ModelSupport::buildPlane(SMap,edgeOffset+ii+20,
				   CutPair.first,CutPair.second,
				   CutPair.first+Z,MD);

	  ModelSupport::buildCylinder(SMap,edgeOffset+ii+6,
				      RCent,Z,edgeRadius+thick);
		  
	  
	}
      edgeOffset += 100;
      thick += wallThick;
    }


  return;
}
 
void
MidWaterDivider::createObjects(Simulation& System,
			       const H2Wing& leftWing,
			       const H2Wing& rightWing)
  /*!
    Adds the main components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("MidWaterDivider","createObjects");

  const std::string Base=
    leftWing.getLinkComplement(4)+leftWing.getLinkComplement(5);
  
  HeadRule LCut(leftWing.getLayerString(cutLayer,6));
  HeadRule RCut(rightWing.getLayerString(cutLayer,6));

  LCut.makeComplement();
  RCut.makeComplement();
  std::string Out;

  Out=ModelSupport::getComposite(SMap,divIndex, divIndex+1000+2,
				 "100 -300 ((-3 -12 20M) : (-20M -6M))"); // x-y-
  Out+=RCut.display()+Base;
  System.addCell(MonteCarlo::Qhull(cellIndex++,modMat,modTemp,Out));

  Out=ModelSupport::getComposite(SMap,divIndex, divIndex+1000+1, 
				 "100 300 ((4 -11 20M) : (-20M -6M))"); // x-y+
  Out+=LCut.display()+Base;
  System.addCell(MonteCarlo::Qhull(cellIndex++,modMat,modTemp,Out)); // x-

  // Aluminium
  Out=ModelSupport::getComposite(SMap,divIndex, divIndex+1000+2,
				 "100 -300 ((( 3 : 12 ) (-103 -112 20M)) : (-120M 6M -106M) : (-103 -112 6M -20M 120M))");  // \todo Is it possible to optimise it more?
  Out+=RCut.display()+Base;
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,modTemp,Out)); // x-

  Out=ModelSupport::getComposite(SMap,divIndex, divIndex+1000+1,
  				 "100 300 ((( -4 : 11 ) (104 -111 20M)) : (-120M 6M -106M) : (104 -111 6M -20M 120M))");  // \todo Is it possible to optimise it more?
  Out+=LCut.display()+Base;
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,modTemp,Out)); // x-
  
  // outer surface:
  Out=ModelSupport::getComposite(SMap,divIndex, divIndex+1000+2,
				 "100 -300 (-103 -112 120M) : -106M ");
  addOuterSurf(Out);
  Out=ModelSupport::getComposite(SMap,divIndex,divIndex+1000+1,
				 "100 300 (104 -111 120M) : -106M ");
  addOuterUnionSurf(Out); 

  // Reverse layer
  /// water
  Out=ModelSupport::getComposite(SMap,divIndex, divIndex+1000+3,
				 "-100 -300 ((24 -32 20M) : (-20M -6M))"); // x+y-
  Out+=RCut.display()+Base;
  System.addCell(MonteCarlo::Qhull(cellIndex++,modMat,modTemp,Out));

  Out=ModelSupport::getComposite(SMap,divIndex, divIndex+1000+4,
				 "-100 300 ((-23 -31 20M) : (-20M -6M))"); // x+y+
  Out+=LCut.display()+Base;
  System.addCell(MonteCarlo::Qhull(cellIndex++,modMat,modTemp,Out));

  // Al
  Out=ModelSupport::getComposite(SMap,divIndex,divIndex+1000+3,
  				 "-100 -300 ((( -24 : 32 ) (-124 -132 20M)) : (-120M 6M -106M) : (-124 -132 6M -20M 120M)) ");   // \todo Is it possible to optimise it more?
  Out+=RCut.display()+Base;
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,modTemp,Out));

  Out=ModelSupport::getComposite(SMap,divIndex, divIndex+1000+4,
				 "-100 300 (((23 : 31 ) (123 -131 20M)) : (-120M 6M -106M) : (123 -131 6M -20M 120M)) ");   // \todo Is it possible to optimise it more?
  Out+=LCut.display()+Base;
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,modTemp,Out));

  // outer surfaces
  Out=ModelSupport::getComposite(SMap,divIndex, divIndex+1000+3,
				 "-100 -300 (-124 -132 120M) : -106M");
  addOuterUnionSurf(Out);

  Out=ModelSupport::getComposite(SMap,divIndex, divIndex+1000+4,
				 "-100 300 (123 -131 120M) : -106M ");
  addOuterUnionSurf(Out);

  HeadRule HR;
  HR.procString(ContainedComp::getExclude());
  HR.makeComplement();
  sideRule = HR.display();
  return;
}

void
MidWaterDivider::cutOuterWing(Simulation& System,
			      const H2Wing& leftWing,
			      const H2Wing& rightWing) const
  /*!
    Cut the outer surface layer of the wings with the
    exclude version of the water layer
    \param System :: Simuation to extract objects rom
    \param leftWing :: Left wing object
    \param rightWing :: Right wing object
  */
{
  ELog::RegMethod RegA("MidWaterDivider","cutOuterWing");

  const size_t lWing=leftWing.getNLayers();
  const size_t rWing=rightWing.getNLayers();

  const std::string LBase=
    leftWing.getLinkComplement(4)+leftWing.getLinkComplement(5);
  const std::string RBase=
    rightWing.getLinkComplement(4)+rightWing.getLinkComplement(5);

  HeadRule cutRule;
  std::string Out;
  if (cutLayer+1<lWing)
    {
      const int cellA=leftWing.getCell("Outer");
      
      MonteCarlo::Qhull* OPtr=System.findQhull(cellA);
      if (!OPtr)
	throw ColErr::InContainerError<int>(cellA,"leftWing Cell: Outer");
      Out=ModelSupport::getComposite(SMap,divIndex,
				     " (100: -11) (-100:-31) ");
      cutRule.procString(Out);
      cutRule.makeComplement();
      OPtr->addSurfString(cutRule.display());
    }
  if (cutLayer+1<rWing)
    {
      const int cellB=rightWing.getCell("Outer");
      MonteCarlo::Qhull* OPtr=System.findQhull(cellB);
      if (!OPtr)
	throw ColErr::InContainerError<int>(cellB,"rightWing Cell: Outer");
      Out=ModelSupport::getComposite(SMap,divIndex,
				     " (100:-12) : (-100:-32) ");
      cutRule.procString(Out);
      cutRule.makeComplement();
      OPtr->addSurfString(cutRule.display());
    }
  return;
}

  
Geometry::Vec3D
MidWaterDivider::getSurfacePoint(const size_t,
			const size_t) const
  /*!
    Given a side and a layer calculate the link point
    \param layerIndex :: layer, 0 is inner moderator [0-6]
    \param sideIndex :: Side [0-3] // mid sides   
    \return Surface point
  */
{
  ELog::RegMethod RegA("MidWaterDivider","getSurfacePoint");
  throw ColErr::AbsObjMethod("Not implemented yet");
}

int
MidWaterDivider::getLayerSurf(const size_t layerIndex,
		     const size_t sideIndex) const
  /*!
    Given a side and a layer calculate the link point
    \param layerIndex :: layer, 0 is inner moderator [0-3]
    \param sideIndex :: Side [0-3] // mid sides   
    \return Surface point
  */
{
  ELog::RegMethod RegA("MidWaterDivider","getLayerSurf");
  throw ColErr::AbsObjMethod("Not implemented yet");
}

std::string
MidWaterDivider::getLayerString(const size_t layerIndex,
		       const size_t sideIndex) const
  /*!
    Given a side and a layer calculate the link point
    \param layerIndex :: layer, 0 is inner moderator [0-6]
    \param sideIndex :: Side [0-3] // mid sides   
    \return Surface point
  */
{
  ELog::RegMethod RegA("MidWaterDivider","getLayerString");

  throw ColErr::IndexError<size_t>(sideIndex,12,"sideIndex");
}


void
MidWaterDivider::createAll(Simulation& System,
			   const attachSystem::FixedComp& FC,
			   const H2Wing& LA,
			   const H2Wing& RA)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed object just for origin/axis
    \param LA :: Left node object
    \param RA :: Right node object
  */
{
  ELog::RegMethod RegA("MidWaterDivider","createAll");

  populate(System.getDataBase());
  createUnitVector(FC);
  createSurfaces();
  createObjects(System,LA,RA);
  cutOuterWing(System,LA,RA);
  createLinks(LA, RA);
  insertObjects(System);       
  return;
}
  
}  // NAMESPACE essSystem
