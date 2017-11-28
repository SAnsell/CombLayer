/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/TelescopicPipe.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <numeric>
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
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "SimProcess.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "AttachSupport.h"

#include "TelescopicPipe.h"

namespace essSystem
{

TelescopicPipe::TelescopicPipe(const std::string& Key) :
  attachSystem::ContainedGroup(),attachSystem::FixedComp(Key,3),
  ptIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(ptIndex+1)
  /*!
    Constructor
  */
{
}

TelescopicPipe::TelescopicPipe(const TelescopicPipe& A) :
  attachSystem::ContainedGroup(A),attachSystem::FixedComp(A),
  ptIndex(A.ptIndex),cellIndex(A.cellIndex),
  engActive(A.engActive),
  xStep(A.xStep),yStep(A.yStep),zStep(A.zStep),xyAngle(A.xyAngle),
  zAngle(A.zAngle),nSec(A.nSec),radius(A.radius),
  length(A.length),zCut(A.zCut),thick(A.thick),
  inMat(A.inMat),wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: TelescopicPipe to copy
  */
{}

TelescopicPipe&
TelescopicPipe::operator=(const TelescopicPipe& A)
  /*!
    Assignment operator
    \param A :: TelescopicPipe to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedGroup::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      engActive=A.engActive;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
      nSec=A.nSec;
      radius=A.radius;
      length=A.length;
      zCut=A.zCut;
      thick=A.thick;
      inMat=A.inMat;
      wallMat=A.wallMat;
    }
  return *this;
}

TelescopicPipe*
TelescopicPipe::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new TelescopicPipe(*this);
}

  
TelescopicPipe::~TelescopicPipe()
  /*!
    Destructor
   */
{}

void
TelescopicPipe::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("TelescopicPipe","populate");

    // Master values
  engActive=Control.EvalPair<int>(keyName,"","EngineeringActive");
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYangle");
  zAngle=Control.EvalVar<double>(keyName+"Zangle");


  nSec=Control.EvalVar<size_t>(keyName+"NSection");
  double R,L(0.0),C,T;
  int Imat,Wmat;

  for(size_t i=0;i<nSec;i++)
    {
     R=Control.EvalVar<double>
	(StrFunc::makeString(keyName+"Radius",i+1));
     L+=Control.EvalVar<double>
	(StrFunc::makeString(keyName+"Length",i+1));
     C=Control.EvalVar<double>
	(StrFunc::makeString(keyName+"Zcut",i+1));
     T=Control.EvalVar<double>
       (StrFunc::makeString(keyName+"WallThick",i+1));
     Imat=ModelSupport::EvalMat<int>
       (Control,StrFunc::makeString(keyName+"InnerMat",i+1));
     Wmat=ModelSupport::EvalMat<int>
       (Control,StrFunc::makeString(keyName+"WallMat",i+1));

      radius.push_back(R);
      length.push_back(L);
      zCut.push_back(C);
      thick.push_back(T);
      inMat.push_back(Imat);
      wallMat.push_back(Wmat);
    }

  return;
}

void
TelescopicPipe::createUnitVector(const attachSystem::FixedComp& FC,
			     const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
    \param sideIndex :: link point on target [signed]
  */
{
  ELog::RegMethod RegA("TelescopicPipe","createUnitVector");

  attachSystem::FixedComp::createUnitVector(FC,sideIndex);

  applyAngleRotate(xyAngle,zAngle);
  applyShift(xStep,yStep,zStep);

  return;
}

void
TelescopicPipe::createSurfaces()
  /*!
    Create all the surface
  */
{
  ELog::RegMethod RegA("TelescopicPipe","createSurfaces");


  int PT(ptIndex);
  for(size_t i=0;i<nSec;i++)
    {
     ModelSupport::buildCylinder(SMap,PT+7,Origin,Y,radius[i]);
     ModelSupport::buildCylinder(SMap,PT+17,Origin,Y,radius[i]+thick[i]);

     ModelSupport::buildPlane(SMap,PT+2,Origin+Y*length[i],Y);
     if (zCut[i]>0.0)
       {
	 ModelSupport::buildPlane(SMap,PT+5,Origin-Z*(radius[i]-zCut[i]),Z);
	 ModelSupport::buildPlane(SMap,PT+6,Origin+Z*(radius[i]-zCut[i]),Z);
       }
     PT+=100;
    }
  return;
}

void
TelescopicPipe::createObjects(Simulation& System,
			  const std::string& TargetSurfBoundary,
			  const std::string& outerSurfBoundary)
  /*!
    Adds the components
    \param System :: Simulation to create objects in
    \param TargetSurfBoundary :: boundary layer [expect to be target edge]
    \param outerSurfBoundary :: boundary layer [expect to be reflector/Bulk edge]
  */
{
  ELog::RegMethod RegA("TelescopicPipe","createObjects");

  std::string Out,EndCap,FrontCap;
  int PT(ptIndex);
  std::string lastEndCup(outerSurfBoundary);

  if (outerSurfBoundary.empty())
    lastEndCup =
      ModelSupport::getComposite(SMap,ptIndex+static_cast<int>((nSec-1)*100),
				 " -2 ");

  attachSystem::ContainedGroup::addCC("Full");
  for(size_t i=0;i<nSec;i++)
    {
      const std::string SName=StrFunc::makeString("Sector",i);
      FrontCap=(!i) ? TargetSurfBoundary :
	ModelSupport::getComposite(SMap,PT-100, " 2 ");
      EndCap=(i+1 == nSec) ? lastEndCup :
	ModelSupport::getComposite(SMap,PT, " -2 ");

      Out=ModelSupport::getSetComposite(SMap,PT, " -7 5 -6 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,inMat[i],0.0,
				       Out+FrontCap+EndCap));
      if (thick[i]>Geometry::zeroTol)
	{
	  Out=ModelSupport::getSetComposite(SMap,PT, " 7 -17 5 -6");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat[i],0.0,
					   Out+FrontCap+EndCap));
	}

      Out=ModelSupport::getSetComposite(SMap,PT, " -17 5 -6 ");
      attachSystem::ContainedGroup::addCC(SName);
      addOuterSurf(SName,Out+EndCap+FrontCap);
      addOuterUnionSurf("Full",Out+EndCap+FrontCap);

      PT+=100;
    }
  return;
}


void
TelescopicPipe::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("TelescopicPipe","createLinks");

  FixedComp::setNConnect(nSec+2);

  // NB: start and end links are defined in TelescopicPipe::createAll
  // 0 - start side surf
  // 1 - end side surf
  
  // radial links
  int PT(ptIndex);
  double ml; // mid length of the current segment
  for(size_t i=0;i<nSec;i++)
    {
      ml  = length[i];
      ml -= (i==0) ? length[i]/2.0 : (length[i]-length[i-1])/2.0;

      FixedComp::setConnect(i+2,Origin+Y*ml+Z*(radius[i]+thick[i]),Z);
      FixedComp::setLinkSurf(i+2,SMap.realSurf(PT+7));

      PT+=100;
    }
  return;
}

void
TelescopicPipe::createAll(Simulation& System,
		      const attachSystem::FixedComp& TargetFC,const long int tIndex,
		      const attachSystem::FixedComp& BulkFC,const long int bIndex,
		      const attachSystem::FixedComp& SB)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param TargetFC :: FixedComp for origin and target outer surf
    \param tIndex :: Target plate surface [signed]
    \param BulkFC :: FixedComp for the pipe end (not used if next arg is 0)
    \param bIndex :: BulkFC link point (not used if 0 - then the tube ends at its max length)
    \param SB :: FixedComp for Monolith Shielding (shutter bay object)
  */
{
  ELog::RegMethod RegA("TelescopicPipe","createAll");
  //  populate(System);
  populate(System.getDataBase());

  createUnitVector(TargetFC,tIndex);
  createSurfaces();
  std::string TSurf,BSurf;

  if (tIndex)
    {
      TSurf=(tIndex>0) ?
	TargetFC.getLinkString(static_cast<size_t>(tIndex)) :
	TargetFC.getCommonRule(static_cast<size_t>(-(tIndex+1+1))).display(); // is it correct instead of getBridgeComplement?
      if (tIndex<0)
	FixedComp::setLinkComponent(0,TargetFC,
				    static_cast<size_t>(-(tIndex-1)));
      else
	FixedComp::setLinkComponent(0,TargetFC,static_cast<size_t>(tIndex-1));
    }
  if (bIndex)
    {
      const size_t lIndex(static_cast<size_t>(std::abs(bIndex))-1);

      BSurf=(bIndex>0) ?
	BulkFC.getLinkString(lIndex+1) : // check this
	BulkFC.getLinkString(bIndex); // at least works fine with TSMainBuokding,-1
      FixedComp::setLinkComponent(1,BulkFC,lIndex);
    }
  createObjects(System,TSurf,BSurf);
  createLinks();
  insertObjects(System);
  
  return;
}

}  // NAMESPACE essSystem
