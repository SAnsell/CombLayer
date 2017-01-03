/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/PreModWing.cxx
 *
 * Copyright (c) 2015-2016 by Konstantin Batkov
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
#include "Cone.h"
#include "Plane.h"
#include "Cylinder.h"
#include "LayerComp.h"
#include "ModBase.h"
#include "H2Wing.h"
#include "ButterflyModerator.h"
#include "PreModWing.h"


namespace essSystem
{

PreModWing::PreModWing(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedComp(Key,3),
  attachSystem::CellMap(),  
  modIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(modIndex+1)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

PreModWing::PreModWing(const PreModWing& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  attachSystem::CellMap(A),
  modIndex(A.modIndex),cellIndex(A.cellIndex),
  engActive(A.engActive),tiltSign(A.tiltSign),thick(A.thick),
  wallThick(A.wallThick),innerRadius(A.innerRadius),
  innerYCut(A.innerYCut),tiltAngle(A.tiltAngle),
  tiltRadius(A.tiltRadius),mat(A.mat),wallMat(A.wallMat),
  baseSurf(A.baseSurf)
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
      tiltSign=A.tiltSign;
      thick=A.thick;
      wallThick=A.wallThick;
      innerRadius=A.innerRadius;
      innerYCut=A.innerYCut;
      tiltAngle=A.tiltAngle;
      tiltRadius=A.tiltRadius;
      mat=A.mat;
      wallMat=A.wallMat;
      baseSurf=A.baseSurf;
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
  */
{
  ELog::RegMethod RegA("PreModWing","populate");

  engActive=Control.EvalPair<int>(keyName,"","EngineeringActive");

  thick=Control.EvalVar<double>(keyName+"Thick");
  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");
  innerYCut=Control.EvalVar<double>(keyName+"InnerYCut");

  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  tiltAngle=Control.EvalVar<double>(keyName+"TiltAngle");
  tiltRadius=Control.EvalVar<double>(keyName+"TiltRadius");

  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  
  return;
}

void
PreModWing::createUnitVector(const attachSystem::FixedComp& FC,
                             const long int sideIndex,
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
  if (zRotate)
    {
      X*=-1;
      Z*=-1;
    }
  return;
}


void
PreModWing::createSurfaces()
  /*!
    Create Plane and surfaces 
  */
{
  ELog::RegMethod RegA("PreModWing","createSurfaces");

  // cone must be shifted for the tilting to start at Y=tiltRadius
  const double coneShift=
    tiltRadius * tan(tiltAngle*M_PI/180.0); 

  // Divide plane
  ModelSupport::buildPlane(SMap,modIndex+1,Origin+Y*innerYCut,Y);
  ModelSupport::buildCylinder(SMap,modIndex+7,Origin,Y,innerRadius);
  ModelSupport::buildCylinder(SMap,modIndex+7,Origin,Y,innerRadius+wallThick);  


  // dividing surface
  ModelSupport::buildCylinder(SMap,modIndex+17,Origin,Z,tiltRadius);
  ModelSupport::buildPlane(SMap,modIndex+5,Origin+Z*(thick*tiltSign),
                           Z*tiltSign);  
  ModelSupport::buildPlane(SMap,modIndex+15,
                           Origin+Z*(thick+wallThick*tiltSign),Z*tiltSign);  
  if (tiltAngle>Geometry::zeroTol)
    {
      ModelSupport::buildPlane(SMap,modIndex+9,Origin,Z);
      ModelSupport::buildCone(SMap,modIndex+8,
                              Origin+Z*(thick+coneShift*tiltSign),
                              Z,90.0-tiltAngle);
      ModelSupport::buildCone(SMap,modIndex+18,
                              Origin+Z*(thick+wallThick+coneShift)*tiltSign,
                              Z, 90-tiltAngle);
    }
  else  
    {
      ELog::EM<<"THIS DOESNT WORK"<<ELog::endErr;
      // ModelSupport::buildPlane(SMap,modIndex+8,Origin+Z*(thick+coneShift)*tiltSign, Z*tiltSign);
      // ModelSupport::buildPlane(SMap,modIndex+18,Origin+Z*(thick+wallThick+coneShift)*tiltSign, Z*tiltSign);
    }

  return; 
}

void
PreModWing::createObjects(Simulation& System)

  /*!
    Create the disc component
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("PreModWing","createObjects");

  std::string Out;
  
  // Inner water:
  Out=ModelSupport::getComposite(SMap,modIndex," 1  ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));


  /*  
  Out=ModelSupport::getComposite(SMap,modIndex," -5 -7 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));

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
  */
  addOuterSurf(Out);
  return; 
}

void
PreModWing::createLinks()
  /*!
    Creates a full attachment set
  */
{  
  ELog::RegMethod RegA("PreModWing","createLinks");

  return;
}


void
PreModWing::createAll(Simulation& System,
		      const attachSystem::FixedComp& Pre,
                      const long int preLinkIndex,
		      const bool zRotate,
		      const int tiltSideDirection)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param Pre :: Attachment point
    \param preLinkIndex :: z-surface of Pre-Mod 
    \param zRotate :: true if must be flipped
    \param tiltSideDirection :: tilt side
   */
{
  ELog::RegMethod RegA("PreModWing","createAll");

  tiltSign = (tiltSideDirection>0) ? 1 : -1;

  populate(System.getDataBase());
  createUnitVector(Pre,preLinkIndex,zRotate);

  createSurfaces();
  createObjects(System);
  createLinks();

  insertObjects(System);

  //  if (engActive) 
  //    InnerComp->createAll(System,*this,7);


  return;
}

}  // NAMESPACE essSystem 
