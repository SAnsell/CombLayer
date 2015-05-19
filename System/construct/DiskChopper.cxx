/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/DiskChopper.cxx
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
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"  
#include "FixedComp.h" 
#include "SecondTrack.h"
#include "TwinComp.h"
#include "ContainedComp.h"

#include "DiskChopper.h"

namespace constructSystem
{

DiskChopper::DiskChopper(const std::string& Key) : 
  attachSystem::FixedComp(Key,6),attachSystem::ContainedComp(),
  chpIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(chpIndex+1),centreFlag(0),nDisk(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

DiskChopper::DiskChopper(const DiskChopper& A) : 
  attachSystem::FixedComp(A),
  chpIndex(A.chpIndex),cellIndex(A.cellIndex),
  centreFlag(A.centreFlag),xStep(A.xStep),yStep(A.yStep),
  zStep(A.zStep),xyAngle(A.xyAngle),zAngle(A.zAngle),
  innerRadius(A.innerRadius),outerRadius(A.outerRadius),
  diskGap(A.diskGap),nDisk(A.nDisk),DInfo(A.DInfo)
  /*!
    Copy constructor
    \param A :: DiskChopper to copy
  */
{}
  
DiskChopper&
DiskChopper::operator=(const DiskChopper& A)
  /*!
    Assignment operator
    \param A :: DiskChopper to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      centreFlag=A.centreFlag;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
      innerRadius=A.innerRadius;
      outerRadius=A.outerRadius;
      diskGap=A.diskGap;
      nDisk=A.nDisk;
      DInfo=A.DInfo;
    }
  return *this;
}


DiskChopper::~DiskChopper() 
  /*!
    Destructor
  */
{}


void
DiskChopper::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("DiskChopper","populate");
  
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYangle");
  zAngle=Control.EvalVar<double>(keyName+"Zangle");

  diskGap=Control.EvalVar<double>(keyName+"Gap");
  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");
  outerRadius=Control.EvalVar<double>(keyName+"OuterRadius");

  nDisk=Control.EvalVar<size_t>(keyName+"NDisk");
  for(size_t i=0;i<nDisk;i++)
    {
      DiskInfo DItem;
      const std::string kN=keyName+StrFunc::makeString(i);
      DItem.thick=Control.EvalPair<double>(kN,keyName,"Thick");
      DItem.phaseAngle=Control.EvalPair<double>(kN,keyName,"PhaseAngle");
      DItem.openAngle=Control.EvalPair<double>(kN,keyName,"OpenAngle");
      DItem.innerMat=ModelSupport::EvalMat<int>
	(Control,kN+"InnerMat",keyName+"InnerMat");
      DItem.outerMat=ModelSupport::EvalMat<int>
	(Control,kN+"OuterMat",keyName+"OuterMat");
      DInfo.push_back(DItem);
    }
  return;
}

void
DiskChopper::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("DiskChopper","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,zAngle);
  if (!centreFlag)
    Origin-=Z*outerRadius;
  return;
}

void
DiskChopper::createUnitVector(const attachSystem::TwinComp& TC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param TC :: TwinComponent to link to 
    \param sideIndex :: 0 / 1 for bEnter/bExit
  */
{
  ELog::RegMethod RegA("DiskChopper","createUnitVector(TwinComp)");

  if (sideIndex>1)
    throw ColErr::IndexError<size_t>(sideIndex,1,"sideIndex");

  Origin=(!sideIndex) ? TC.getBeamStart() : TC.getBeamExit();
  X=TC.getBX();
  Y=TC.getBY();
  Z=TC.getBZ();

  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,zAngle);
  if (!centreFlag)
    Origin-=Z*outerRadius;
  return;
}

void
DiskChopper::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("DiskChopper","createSurfaces");

  ModelSupport::buildCylinder(SMap,chpIndex+7,Origin,Y,innerRadius);
  ModelSupport::buildCylinder(SMap,chpIndex+17,Origin,Y,outerRadius);

  int CI(chpIndex);
  Geometry::Vec3D DCent(Origin);
  for(const DiskInfo& DRef : DInfo)
    {
      ModelSupport::buildPlane(SMap,CI+1,DCent,Y);
      ModelSupport::buildPlane(SMap,CI+2,DCent+Y*DRef.thick,Y);
      ModelSupport::buildPlaneRotAxis(SMap,CI+13,DCent,X,
				      Y,DRef.phaseAngle);
      ModelSupport::buildPlaneRotAxis(SMap,CI+14,DCent,X,
				      Y,DRef.phaseAngle+DRef.openAngle);
      // Need to be careful here:
      // -- divider surface that
      ModelSupport::buildPlaneRotAxis(SMap,CI+19,DCent,X,Y,
				      (90.0+DRef.phaseAngle+
				       DRef.openAngle/2.0));

      DCent+=Y*(DRef.thick+diskGap);
      CI+=50;
    }

  return;
}

void
DiskChopper::createObjects(Simulation& System)
  /*!
    Adds the zoom chopper box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("DiskChopper","createObjects");

  std::string Out;
  int CI(chpIndex);
  for(const DiskInfo& DRef : DInfo)
    {
      // Gap if there is previous item
      if (CI!=chpIndex)
	{
	  Out=ModelSupport::getComposite(SMap,chpIndex,CI-50,"2M -51M -17");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
	}
      
      Out=ModelSupport::getComposite(SMap,chpIndex,CI,"1M -2M -7");
      System.addCell(MonteCarlo::Qhull(cellIndex++,DRef.innerMat,0.0,Out));

      Out=ModelSupport::getComposite(SMap,chpIndex,CI,
				     "1M -2M 13M -14M -19M 7 -17");
      System.addCell(MonteCarlo::Qhull(cellIndex++,0.0,0.0,Out));

      Out=ModelSupport::getComposite(SMap,chpIndex,CI,
				     "1M -2M (-13M:14M:19M) 7 -17");
      System.addCell(MonteCarlo::Qhull(cellIndex++,DRef.outerMat,0.0,Out));


      CI+=50;
    }

  Out=ModelSupport::getComposite(SMap,chpIndex,CI-50,"1 -17 -2M");
  addOuterSurf(Out);      

  return;
}

void
DiskChopper::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("DiskChopper","createLinks");

  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(chpIndex+1));

  const int CLast(static_cast<int>(nDisk-1)*50);
  double L(-diskGap);  // one less gap !
  for(const DiskInfo& DRef : DInfo)
    L+=DRef.thick+diskGap;
  
  FixedComp::setConnect(1,Origin+Y*L,Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(CLast+2));
  
  FixedComp::setConnect(2,Origin+Y*(L/2.0)-X*outerRadius,-X);
  FixedComp::setLinkSurf(2,SMap.realSurf(chpIndex+17));

  FixedComp::setConnect(3,Origin+Y*(L/2.0)-Z*outerRadius,-Z);
  FixedComp::setLinkSurf(3,SMap.realSurf(chpIndex+17));

  FixedComp::setConnect(4,Origin+Y*(L/2.0)+Z*outerRadius,Z);
  FixedComp::setLinkSurf(4,SMap.realSurf(chpIndex+17));

  FixedComp::setConnect(5,Origin+Y*(L/2.0)+X*outerRadius,X);
  FixedComp::setLinkSurf(5,SMap.realSurf(chpIndex+17));

  
  return;
}

void
DiskChopper::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("DiskChopper","createAll(FC)");
  
  populate(System.getDataBase());
  createUnitVector(FC,FIndex);

  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);   
  
  return;
}

void
DiskChopper::createAllBeam(Simulation& System,
			   const attachSystem::TwinComp& TC,
			   const long int FIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param TC :: Twincomp if using beam direction
    \param FIndex :: Side Index [0 entry/1 exit]
  */
{
  ELog::RegMethod RegA("DiskChopper","createAllBeam(TW)");
  
  populate(System.getDataBase());
  createUnitVector(TC,FIndex);

  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);   
  
  return;
}

  
}  // NAMESPACE constructSystem
