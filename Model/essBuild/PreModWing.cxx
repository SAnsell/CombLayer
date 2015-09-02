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
  modIndex(A.modIndex),cellIndex(A.cellIndex)
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
  ELog::EM << Origin << ELog::endCrit;
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

  // Divide plane
  ModelSupport::buildPlane(SMap,modIndex+1,Origin,X);  
  ModelSupport::buildPlane(SMap,modIndex+2,Origin,Y);  

  //  ModelSupport::buildPlane(SMap,modIndex+5,Origin+Z*(thick),Z);  
  //  ModelSupport::buildPlane(SMap,modIndex+6,Origin+Z*(thick+wallThick),Z);  

  ModelSupport::buildCylinder(SMap,modIndex+8,Origin,Z,tiltRadius);  
  ModelSupport::buildCone(SMap, modIndex+5, Origin+Z*(thick), Z, 90-tiltAngle, -1);
  ModelSupport::buildCone(SMap, modIndex+6, Origin+Z*(thick+wallThick), Z, 90-tiltAngle, -1);

  return; 
}

void
PreModWing::createObjects(Simulation& System, const attachSystem::FixedComp& Pre, const long int linkPoint, 
			  const attachSystem::FixedComp& Mod)

  /*!
    Create the disc component
    \param System :: Simulation to add results
    \param Mod :: butterfly moderator (to get it's side surface)
    \param Pre :: attachment top/bottom point
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
      (ambientCell,"ButterflyModerator ambient void cell not found");
  
  const ButterflyModerator *BM = dynamic_cast<const ButterflyModerator*>(&Mod);
  BM->getSideSurface();
  const std::string Exclude = BM->getSideSurface();;//BM->getExcludeStr(); 

  // BM outer cylinder side surface
  HeadRule HR;
  HR.procString(BM->getLinkString(2));
  HR.makeComplement();
  const std::string BMouterCyl = HR.display();

  std::string Out;
  HeadRule wingExclude;

  std::string PreString = Pre.getLinkString(linkPoint);
  HR.procString(Pre.getLinkString(linkPoint));
  HR.makeComplement();
  PreString = HR.display();
  ELog::EM << "PreString: " << PreString << ELog::endCrit;
  
  Out=ModelSupport::getComposite(SMap,modIndex," -5 ") + PreString;
  wingExclude.procString(Out);
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out+Exclude + BMouterCyl));

  Out=ModelSupport::getComposite(SMap,modIndex," 5 -6 ");
  wingExclude.addUnion(Out);
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out+Exclude + BMouterCyl));

  wingExclude.makeComplement();
  AmbientVoid->addSurfString(wingExclude.display());

  Out=ModelSupport::getComposite(SMap,modIndex," -6 ") + PreString+Exclude+BMouterCyl;
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
		      const attachSystem::FixedComp& Pre, const long int linkPoint,
		      const bool zRotate,
		      const attachSystem::FixedComp& Mod)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param Pre :: Attachment point
    \param linkPoint :: z-surface of Pre
    \param zRotate :: true if must be flipped
    \param Mod :: Butterfly moderator
   */
{
  ELog::RegMethod RegA("PreModWing","createAll");


  populate(System.getDataBase());
  createUnitVector(Pre, linkPoint, zRotate);

  createSurfaces();
  createObjects(System, Pre, linkPoint, Mod);
  createLinks();



  insertObjects(System);

  //  if (engActive) 
  //    InnerComp->createAll(System,*this,7);


  return;
}

}  // NAMESPACE essSystem 
