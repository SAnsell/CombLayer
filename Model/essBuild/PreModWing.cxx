/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/PreModWing.cxx
 *
 * Copyright (c) 2015 by Konstantin Batkov
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
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "support.h"
#include "SurInter.h"
#include "stringCombine.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ContainedComp.h"
#include "CylFlowGuide.h"
#include "PreModWing.h"
#include "Cone.h"
#include "Plane.h"
#include "Cylinder.h"
// for Butterfly
#include "LayerComp.h"
#include "ModBase.h"
#include "H2Wing.h"
#include "ButterflyModerator.h"

namespace essSystem
{

PreModWing::PreModWing(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedComp(Key,9),
  attachSystem::CellMap(),  
  modIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(modIndex+1)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
}

PreModWing::PreModWing(const PreModWing& A) : 
  attachSystem::ContainedComp(A),
  attachSystem::FixedComp(A),
  attachSystem::CellMap(A),
  modIndex(A.modIndex),cellIndex(A.cellIndex),
  engActive(A.engActive),thick(A.thick),mat(A.mat),
  wallThick(A.wallThick),wallMat(A.wallMat),
  tiltSide(A.tiltSide),tiltAngle(A.tiltAngle),
  tiltRadius(A.tiltRadius)
  /*!
    Copy constructor
    \param A :: PreModWing to copy
  */
{}

PreModWing&
PreModWing::operator=(const PreModWing& A)
  /*!
    Assignment operator
    \param A :: PreModWing to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      cellIndex=A.cellIndex;
      engActive=A.engActive;
      thick=A.thick;
      mat=A.mat;
      wallThick=A.wallThick;
      wallMat=A.wallMat;
      tiltSide=A.tiltSide;
      tiltAngle=A.tiltAngle;
      tiltRadius=A.tiltRadius;
   }
  return *this;
}

PreModWing*
PreModWing::clone() const
  /*!
    Clone self 
    \return new (this)
   */
{
  return new PreModWing(*this);
}

PreModWing::~PreModWing()
  /*!
    Destructor
  */
{}
  

void
PreModWing::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
    \param zShift :: Default offset height a
    \param outRadius :: Outer radius of reflector [for void fill]
  */
{
  ELog::RegMethod RegA("PreModWing","populate");

  ///< \todo Make this part of IParam NOT a variable
  engActive=Control.EvalPair<int>(keyName,"","EngineeringActive");

  thick=Control.EvalVar<double>(keyName+"Thick");
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  tiltAngle=Control.EvalVar<double>(keyName+"TiltAngle");
  tiltRadius=Control.EvalVar<double>(keyName+"TiltRadius");

  return;
}

void
PreModWing::createUnitVector(const attachSystem::FixedComp& FC, const long int sideIndex,
			     const bool zRotate)
  /*!
    Create the unit vectors
    \param FC :: Centre for object
    \param sideIndex :: index for link
    \param zRotate :: rotate Zaxis
  */
{
  ELog::RegMethod RegA("PreModWing","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC);
  Origin=FC.getSignedLinkPt(sideIndex);
  //  ELog::EM << Origin << ELog::endCrit;
  if (zRotate)
    {
      X*=-1;
      Z*=-1;
    }
  //  const double D=(depth.empty()) ? 0.0 : depth.back();
  //  applyShift(0,0,zStep+D);


  return;
}


void
PreModWing::createSurfaces()
  /*!
    Create planes for the silicon and Polyethene layers
    \param tiltSide :: top/bottom side to tilt
  */
{
  ELog::RegMethod RegA("PreModWing","createSurfaces");

  const double h = tiltRadius * tan(tiltAngle*M_PI/180.0); // cone must be shifted for the tilting to start at Y=tiltRadius

  // Divide plane
  ModelSupport::buildPlane(SMap,modIndex+1,Origin,X);  
  ModelSupport::buildPlane(SMap,modIndex+2,Origin,Y);  

  ModelSupport::buildCylinder(SMap,modIndex+7,Origin,Z,tiltRadius);  

  const int tiltSign = tiltSide ? 1 : -1;

  ModelSupport::buildPlane(SMap,modIndex+5,Origin+Z*(thick)*tiltSign,Z*tiltSign);  
  ModelSupport::buildPlane(SMap,modIndex+6,Origin+Z*(thick+wallThick)*tiltSign,Z*tiltSign);  
  if (tiltAngle>Geometry::zeroTol)
    {
      ModelSupport::buildCone(SMap, modIndex+8, Origin+Z*(thick+h)*tiltSign, Z, 90-tiltAngle, -tiltSign);
      ModelSupport::buildCone(SMap, modIndex+9, Origin+Z*(thick+wallThick+h)*tiltSign, Z, 90-tiltAngle, -tiltSign);
    }
  else
    {
      ModelSupport::buildPlane(SMap, modIndex+8, Origin+Z*(thick+h)*tiltSign, Z*tiltSign);
      ModelSupport::buildPlane(SMap, modIndex+9, Origin+Z*(thick+wallThick+h)*tiltSign, Z*tiltSign);
    }

  return; 
}

void
PreModWing::createObjects(Simulation& System,
			  const attachSystem::FixedComp& Pre, const long int preLP, 
			  const attachSystem::FixedComp& Mod)

  /*!
    Create the disc component
    \param System :: Simulation to add results
    \param Mod :: butterfly moderator (to get it's side rule)
    \param Pre :: attachment top/bottom object
    \param preLP :: link point of the attachemt object
    \param Mod :: Moderator
  */
{
  ELog::RegMethod RegA("PreModWing","createObjects");

  const attachSystem::CellMap* CM=
    dynamic_cast<const attachSystem::CellMap*>(&Mod);

  MonteCarlo::Object* AmbientVoid(0);
  int ambientCell(0);
  if (CM)
    {
      ambientCell=CM->getCell("ambientVoid");
      AmbientVoid=System.findQhull(ambientCell);
    }
  if (!AmbientVoid)
    throw ColErr::InContainerError<int>
      (ambientCell,"ButterflyModerator ambientVoid cell not found");
  
  const ButterflyModerator *BM = dynamic_cast<const ButterflyModerator*>(&Mod);
  const std::string excludeBM = BM->getSideRule();//BM->getExcludeStr(); 
  const std::string excludeBMLeftRightWater = BM->getLeftRightWaterSideRule();

  // BM outer cylinder side surface
  HeadRule HR;
  HR.procString(BM->getLinkString(2));
  HR.makeComplement();
  const std::string BMouterCyl = HR.display();

  std::string Out;

  std::string PreString;
  HR.procString(Pre.getLinkString(preLP));
  HR.makeComplement();
  PreString = HR.display();

  Out=ModelSupport::getComposite(SMap,modIndex," -5 -7 ") + PreString;
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out+excludeBM));

  Out=ModelSupport::getComposite(SMap,modIndex," 5 -6 -7 ");// + PreString;
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out+excludeBM));
  
  Out=ModelSupport::getComposite(SMap,modIndex," 7 -8 ") + PreString;
  // Originally I excluded all moderator by +excludeBM string, but actually this particular cell
  // only crosses its left+right water cells, so I use +BM->getLeftRightWaterSideRule()
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,
				   Out+excludeBMLeftRightWater+BMouterCyl));

  Out=ModelSupport::getComposite(SMap,modIndex," 7 8 -9 ") + PreString;
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,
				   Out + excludeBMLeftRightWater + BMouterCyl)); // same trick with excludeBMLeftRightWater as in the previous cell

  HeadRule wingExclude;
  Out=ModelSupport::getComposite(SMap,modIndex," (-6 -7):(7 -9) ") + PreString;
  wingExclude.procString(Out);
  wingExclude.makeComplement();
  AmbientVoid->addSurfString(wingExclude.display());

  Out=ModelSupport::getComposite(SMap,modIndex," -9 ") + PreString+excludeBM+BMouterCyl;
  addOuterSurf(Out);
  return; 
}

void
PreModWing::createLinks()
  /*!
    Creates a full attachment set
    First two are in the -/+Y direction and have a divider
    Last two are in the -/+X direction and have a divider
    The mid two are -/+Z direction
  */
{  
  ELog::RegMethod RegA("PreModWing","createLinks");

  return;
}


void
PreModWing::createAll(Simulation& System,
		      const attachSystem::FixedComp& Pre, const long int preLP,
		      const bool zRotate,
		      const bool ts,
		      const attachSystem::FixedComp& Mod)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param Pre :: Attachment point
    \param preLP :: z-surface of Pre
    \param zRotate :: true if must be flipped
    \param ts :: tilt side
    \param Mod :: Butterfly moderator
   */
{
  ELog::RegMethod RegA("PreModWing","createAll");

  tiltSide = ts;

  populate(System.getDataBase());
  createUnitVector(Pre, preLP, zRotate);

  createSurfaces();
  createObjects(System, Pre, preLP, Mod);
  createLinks();



  insertObjects(System);

  //  if (engActive) 
  //    InnerComp->createAll(System,*this,7);


  return;
}

}  // NAMESPACE essSystem 
