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

namespace essSystem
{

MidWaterDivider::MidWaterDivider(const std::string& baseKey,
				 const std::string& extraKey) :
  attachSystem::ContainedComp(),
  attachSystem::LayerComp(0,0),
  attachSystem::FixedComp(baseKey+extraKey,8),
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
  modTemp(A.modTemp)
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
MidWaterDivider::createLinks()
  /*!
    Construct links for the triangle moderator
    The normal 1-3 and 5-6 are plane,
    7,8,9 are 
  */
{
  ELog::RegMethod RegA("MidWaterDivider","createLinks");

  // Loop over corners that are bound by convex

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

  // +Y section
  ModelSupport::buildPlaneRotAxis
    (SMap,divIndex+3,Origin+Y*midYStep,X,-Z,-midAngle/2.0);
  ModelSupport::buildPlaneRotAxis
    (SMap,divIndex+4,Origin+Y*midYStep,X,-Z,midAngle/2.0);

  // -Y section
  ModelSupport::buildPlaneRotAxis
    (SMap,divIndex+23,Origin-Y*midYStep,-X,-Z,-midAngle/2.0);
  ModelSupport::buildPlaneRotAxis
    (SMap,divIndex+24,Origin-Y*midYStep,-X,-Z,midAngle/2.0);

  // Make lengths:

  Geometry::Vec3D leftNorm(Y);
  Geometry::Quaternion::calcQRotDeg(-midAngle/2.0,Z).rotate(leftNorm);  
  Geometry::Vec3D rightNorm(Y);
  Geometry::Quaternion::calcQRotDeg(midAngle/2.0,Z).rotate(rightNorm);  
  
  ModelSupport::buildPlane(SMap,divIndex+11,Origin+leftNorm*length,leftNorm);
  ModelSupport::buildPlane(SMap,divIndex+12,Origin+rightNorm*length,rightNorm);

  // Length below [note reverse of normals]
  ModelSupport::buildPlane(SMap,divIndex+31,Origin-rightNorm*length,-rightNorm);
  ModelSupport::buildPlane(SMap,divIndex+32,Origin-leftNorm*length,-leftNorm);

  
  // Aluminum layers [+100]
  // +Y section
  const double LStep(midYStep+wallThick/sin(midAngle/2.0));
  ModelSupport::buildPlaneRotAxis
    (SMap,divIndex+103,Origin+Y*LStep,X,-Z,-midAngle/2.0);
  ModelSupport::buildPlaneRotAxis
    (SMap,divIndex+104,Origin+Y*LStep,X,-Z,midAngle/2.0);

  // -Y section

  ModelSupport::buildPlaneRotAxis
    (SMap,divIndex+123,Origin-Y*LStep,-X,-Z,-midAngle/2.0);
  ModelSupport::buildPlaneRotAxis
    (SMap,divIndex+124,Origin-Y*LStep,-X,-Z,midAngle/2.0);

  
  ModelSupport::buildPlane(SMap,divIndex+111,
			   Origin+leftNorm*(length+wallThick),leftNorm);
  ModelSupport::buildPlane(SMap,divIndex+112,
			   Origin+rightNorm*(length+wallThick),rightNorm);

  // Length below [note reverse of normals]
  ModelSupport::buildPlane(SMap,divIndex+131,
			   Origin-rightNorm*(wallThick+length),-rightNorm);
  ModelSupport::buildPlane(SMap,divIndex+132,
			   Origin-leftNorm*(wallThick+length),-leftNorm);


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

  Out=ModelSupport::getComposite(SMap,divIndex,"100 (-3 : 4) -11 -12 ");
  Out+=LCut.display()+RCut.display()+Base;
  System.addCell(MonteCarlo::Qhull(cellIndex++,modMat,modTemp,Out));

  // Aluminium
  Out=ModelSupport::getComposite(SMap,divIndex,
				 "100 (-103 : 104) -111 -112 "
				 " ( (3  -4) : 11 : 12 ) ");
				 
  Out+=LCut.display()+RCut.display()+Base;
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,modTemp,Out));

  Out=ModelSupport::getComposite(SMap,divIndex,
				 "100 (-103 : 104)  -111 -112 ");
  addOuterSurf(Out);
  // Reverse layer
  Out=ModelSupport::getComposite(SMap,divIndex,"-100 (-23 : 24) -31 -32 ");
  Out+=LCut.display()+RCut.display()+Base;
  System.addCell(MonteCarlo::Qhull(cellIndex++,modMat,modTemp,Out));
  Out=ModelSupport::getComposite(SMap,divIndex,
				 "-100 (-123 : 124)  -131 -132 "
				 "((23  -24) : 31 : 32 )");
  Out+=LCut.display()+RCut.display()+Base;
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,modTemp,Out));
  Out=ModelSupport::getComposite(SMap,divIndex,
				 "-100 (-123 : 124) -131 -132 ");
  addOuterUnionSurf(Out);
  
  
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
  createLinks();
  insertObjects(System);       
  return;
}
  
}  // NAMESPACE essSystem
