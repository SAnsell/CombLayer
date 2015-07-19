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
#include "FixedGroup.h" 
#include "SecondTrack.h"
#include "TwinComp.h"
#include "ContainedComp.h"

#include "DiskBlades.h"
#include "DiskChopper.h"

namespace constructSystem
{

DiskChopper::DiskChopper(const std::string& Key) : 
  attachSystem::FixedGroup(Key,"Main",6,"Beam",2),
  attachSystem::ContainedComp(),
  chpIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(chpIndex+1),centreFlag(0),nDisk(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

DiskChopper::DiskChopper(const DiskChopper& A) : 
  attachSystem::FixedGroup(A),
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
      attachSystem::FixedGroup::operator=(A);
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


  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");
  outerRadius=Control.EvalVar<double>(keyName+"OuterRadius");

  nDisk=Control.EvalVar<size_t>(keyName+"NDisk");
  diskGap= (nDisk>1) ?
    Control.EvalVar<double>(keyName+"Gap") : 0.0;
  
  for(size_t i=0;i<nDisk;i++)
    {
      DiskBlades DItem;
      const std::string kN=keyName+StrFunc::makeString(i);
      DItem.thick=Control.EvalPair<double>(kN,keyName,"Thick");
      DItem.innerMat=ModelSupport::EvalMat<int>
	(Control,kN+"InnerMat",keyName+"InnerMat");
      DItem.outerMat=ModelSupport::EvalMat<int>
	(Control,kN+"OuterMat",keyName+"OuterMat");

      const size_t NB=Control.EvalPair<size_t>(kN,keyName,"NBlades");
      for(size_t j=0;j<NB;j++)
	{
	  const std::string PAStr("PhaseAngle"+StrFunc::makeString(j));
	  const std::string OAStr("OpenAngle"+StrFunc::makeString(j));
	  const double pA=Control.EvalPair<double>(kN,keyName,PAStr);
	  const double oA=Control.EvalPair<double>(kN,keyName,OAStr);
	  DItem.addOpen(pA,oA);
	}
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

  attachSystem::FixedComp& mainFC=FixedGroup::getKey("Main");
  attachSystem::FixedComp& beamFC=FixedGroup::getKey("Beam");

  mainFC.createUnitVector(FC,sideIndex);
  mainFC.applyShift(xStep,yStep,zStep);
  beamFC=mainFC;          // rotation not applied to beamFC
  mainFC.applyAngleRotate(xyAngle,zAngle);

  beamOrigin=beamFC.getCentre();
  beamAxis=beamFC.getY();

  if (centreFlag && centreFlag<4 && centreFlag>-4)
    {
      double XYZ[3]={0,0,0};
      const size_t index=static_cast<size_t>(std::abs(centreFlag)-1);
      if (centreFlag<0)
	XYZ[index]=-((outerRadius+innerRadius)/2.0);
      else
	XYZ[index]=((outerRadius+innerRadius)/2.0);

      mainFC.applyShift(XYZ[0],XYZ[1],XYZ[2]);
    }
  setDefault("Main");  
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
    throw ColErr::IndexError<long int>(sideIndex,1,"sideIndex");

  Origin=(!sideIndex) ? TC.getBeamStart() : TC.getBeamExit();
  X=TC.getBX();
  Y=TC.getBY();
  Z=TC.getBZ();

  beamOrigin=Origin;
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
  for(const DiskBlades& DRef : DInfo)
    {
      ModelSupport::buildPlane(SMap,CI+1,DCent,Y);
      ModelSupport::buildPlane(SMap,CI+2,DCent+Y*DRef.getThick(),Y);
      // make inner blades
      int PI(CI);
      for(size_t index=0; index<DRef.getNPhase();index++)
	{
	  const double oA=DRef.getOpenAngle(index);
	  const double cA=DRef.getCloseAngle(index);
	  ModelSupport::buildPlaneRotAxis(SMap,PI+3,DCent,X,Y,oA);
	  ModelSupport::buildPlaneRotAxis(SMap,PI+4,DCent,X,Y,cA);
	  
	  PI+=10;
	}
      DCent+=Y*(DRef.thick+diskGap);
      CI+=500;
    }

  return;
}

void
DiskChopper::createObjects(Simulation& System)
  /*!
    Adds the chopper blades
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("DiskChopper","createObjects");

  std::string Out;
  int CI(chpIndex);
  for(const DiskBlades& DRef : DInfo)
    {
      // Gap if there is previous item
      if (CI!=chpIndex)
	{
	  Out=ModelSupport::getComposite(SMap,chpIndex,CI-500,"2M -501M -17");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
	}
      // inner layer
      Out=ModelSupport::getComposite(SMap,chpIndex,CI,"1M -2M -7");
      System.addCell(MonteCarlo::Qhull(cellIndex++,
				       DRef.getInnerMat(),0.0,Out));

      // Chopper opening
      const size_t NPhase=DRef.getNPhase();
        const std::string Main=
	ModelSupport::getComposite(SMap,chpIndex,CI,"1M -2M  7 -17");
      
      int PI(CI);       // current 
      int PN(CI+10);    // next
      for(size_t index=0;index<NPhase;index++)
	{
	  if (index==NPhase-1)
	    PN=CI;
	  // OPENING
	  double oA=DRef.getOpenAngle(index/2);
	  double cA=DRef.getCloseAngle(index/2);
	  if (cA-oA<180.0)
	    Out=ModelSupport::getComposite(SMap,PI," 3 -4 ");
	  else
	    Out=ModelSupport::getComposite(SMap,PI," (3 : -4) ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out+Main));
	  // CLOSING
	  oA=DRef.getCloseAngle(index/2);
	  cA=DRef.getOpenAngle(1+(index/2));
	  if (cA-oA<180.0)
	    Out=ModelSupport::getComposite(SMap,PI,PN," 4 -3M ");
	  else
	    Out=ModelSupport::getComposite(SMap,PI,PN," (4 : -3M) ");
	  System.addCell(MonteCarlo::Qhull
			 (cellIndex++,DRef.getOuterMat(),0.0,Out+Main));
	  PI+=10;
	  PN+=10;
	}
      CI+=500;
    }

  Out=ModelSupport::getComposite(SMap,chpIndex,CI-500,"1 -17 -2M");
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

  attachSystem::FixedComp& mainFC=FixedGroup::getKey("Main");
  attachSystem::FixedComp& beamFC=FixedGroup::getKey("Beam");
  
  mainFC.setConnect(0,Origin,-Y);
  mainFC.setLinkSurf(0,-SMap.realSurf(chpIndex+1));

  const int CLast(chpIndex+static_cast<int>(nDisk-1)*500);
  double L(-diskGap);  // one less gap !
  for(const DiskBlades& DRef : DInfo)
    L+=DRef.getThick()+diskGap;

  mainFC.setConnect(1,Origin+Y*L,Y);
  mainFC.setLinkSurf(1,SMap.realSurf(CLast+2));
  
  mainFC.setConnect(2,Origin+Y*(L/2.0)-X*outerRadius,-X);
  mainFC.setLinkSurf(2,SMap.realSurf(chpIndex+17));

  mainFC.setConnect(3,Origin+Y*(L/2.0)-Z*outerRadius,-Z);
  mainFC.setLinkSurf(3,SMap.realSurf(chpIndex+17));

  mainFC.setConnect(4,Origin+Y*(L/2.0)+Z*outerRadius,Z);
  mainFC.setLinkSurf(4,SMap.realSurf(chpIndex+17));

  mainFC.setConnect(5,Origin+Y*(L/2.0)+X*outerRadius,X);
  mainFC.setLinkSurf(5,SMap.realSurf(chpIndex+17));

  // MAIN BEAM:
  beamFC.setConnect(0,beamOrigin,-beamAxis);
  beamFC.setLinkSurf(0,-SMap.realSurf(chpIndex+1));

  beamFC.setConnect(1,beamOrigin+Y*L,beamAxis);
  beamFC.setLinkSurf(1,SMap.realSurf(CLast+2));
  
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
