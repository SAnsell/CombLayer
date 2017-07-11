/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   d4cModel/DetectorBank.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include "stringCombine.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Tally.h"
#include "TallyCreate.h"
#include "inputParam.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "DetectorBank.h"

#include "TallySelector.h" 
#include "SpecialSurf.h"
#include "pointConstruct.h"
#include "Detector.h" 
#include "PointDetector.h" 
#include "DetGroup.h" 

namespace d4cSystem
{

DetectorBank::DetectorBank(const size_t BN,const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key+StrFunc::makeString(BN),6),
  bankNumber(BN),baseName(Key),
  detIndex(ModelSupport::objectRegister::Instance().cell(keyName)),
  cellIndex(detIndex+1)
  /*!
    Constructor
    \param BN :: Bank number
    \param Key :: Name of construction key
  */
{}

DetectorBank::DetectorBank(const DetectorBank& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  bankNumber(A.bankNumber),baseName(A.baseName),
  detIndex(A.detIndex),cellIndex(A.cellIndex),
  centreOffset(A.centreOffset),
  centreAngle(A.centreAngle),detDepth(A.detDepth),
  detLength(A.detLength),detHeight(A.detHeight),
  wallThick(A.wallThick),wallMat(A.wallMat),detMat(A.detMat),
  nDet(A.nDet)
  /*!
    Copy constructor
    \param A :: DetectorBank to copy
  */
{}

DetectorBank&
DetectorBank::operator=(const DetectorBank& A)
  /*!
    Assignment operator
    \param A :: DetectorBank to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      cellIndex=A.cellIndex;
      centreOffset=A.centreOffset;
      centreAngle=A.centreAngle;
      detDepth=A.detDepth;
      detLength=A.detLength;
      detHeight=A.detHeight;
      wallThick=A.wallThick;
      wallMat=A.wallMat;
      detMat=A.detMat;
      nDet=A.nDet;
    }
  return *this;
}

DetectorBank::~DetectorBank()
  /*!
    Destructor
   */
{}

void
DetectorBank::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("DetectorBank","populate");

  FixedOffset::populate(Control);
    // Master values
  xStep=Control.EvalPair<double>(keyName,baseName,"XStep");
  yStep=Control.EvalPair<double>(keyName,baseName,"YStep");
  zStep=Control.EvalPair<double>(keyName,baseName,"ZStep");
  xyAngle=Control.EvalPair<double>(keyName,baseName,"XYangle");
  zAngle=Control.EvalPair<double>(keyName,baseName,"Zangle");

  centreOffset=Control.EvalPair<double>(keyName,baseName,"CentreOffset");
  centreAngle=Control.EvalPair<double>(keyName,baseName,"CentreAngle");
  xyAngle-=centreAngle;
  centreAngle*=M_PI/180.0;

  detHeight=Control.EvalPair<double>(keyName,baseName,"DetHeight");
  detLength=Control.EvalPair<double>(keyName,baseName,"DetLength");
  detDepth=Control.EvalPair<double>(keyName,baseName,"DetDepth");

  wallThick=Control.EvalPair<double>(keyName,baseName,"WallThick");

  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat",
				     baseName+"WallMat");
  detMat=ModelSupport::EvalMat<int>(Control,keyName+"DetMat",
				    baseName+"DetMat");

  // trick to get def var of a pair.
  nDet=Control.EvalDefVar<size_t>(baseName+"NDet",0);
  nDet=Control.EvalDefVar<size_t>(keyName+"NDet",nDet);
  return;
}

void
DetectorBank::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: FixedComp for origin
  */
{
  ELog::RegMethod RegA("DetectorBank","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC);
  
  Origin=Origin+(X*sin(centreAngle)+Y*cos(centreAngle))*centreOffset;
  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,zAngle);

  return;
}

void
DetectorBank::createSurfaces()
  /*!
    Create planes for the silicon and Polyethene layers
  */
{
  ELog::RegMethod RegA("DetectorBank","createSurfaces");

  ModelSupport::buildPlane(SMap,detIndex+1,
			   Origin-Y*(detDepth/2.0),Y);    
  ModelSupport::buildPlane(SMap,detIndex+2,
			   Origin+Y*(detDepth/2.0),Y);    

  ModelSupport::buildPlane(SMap,detIndex+3,
			   Origin-X*(detLength/2.0),X);    
  ModelSupport::buildPlane(SMap,detIndex+4,
			   Origin+X*(detLength/2.0),X);    
  ModelSupport::buildPlane(SMap,detIndex+5,
			   Origin-Z*(detHeight/2.0),Z);    
  ModelSupport::buildPlane(SMap,detIndex+6,
			   Origin+Z*(detHeight/2.0),Z);    

  ModelSupport::buildPlane(SMap,detIndex+11,
			   Origin-Y*(detDepth/2.0+wallThick),Y);    
  ModelSupport::buildPlane(SMap,detIndex+12,
			   Origin+Y*(detDepth/2.0+wallThick),Y);    
  ModelSupport::buildPlane(SMap,detIndex+13,
			   Origin-X*(detLength/2.0+wallThick),X);    
  ModelSupport::buildPlane(SMap,detIndex+14,
			   Origin+X*(detLength/2.0+wallThick),X);    
  ModelSupport::buildPlane(SMap,detIndex+15,
			   Origin-Z*(detHeight/2.0+wallThick),Z);    
  ModelSupport::buildPlane(SMap,detIndex+16,
			   Origin+Z*(detHeight/2.0+wallThick),Z);    

  return; 
}

void
DetectorBank::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("DetectorBank","createObjects");

  std::string Out;
  // First make inner/outer void/wall and top/base

  Out=ModelSupport::getComposite(SMap,detIndex,"11 -12 13 -14 15 -16");	
  addOuterSurf(Out);  

  Out=ModelSupport::getComposite(SMap,detIndex," 1 -2 3 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,detMat,0.0,Out));	
  Out=ModelSupport::getComposite(SMap,detIndex,
				 " 11 -12 13 -14 15 -16 (-1:2:-3:4:-5:6)");	
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  return; 
}

void
DetectorBank::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("DetectorBank","createLinks");
  int sign(-1);
  int surfN(11);
  const double T[]={detDepth/2.0,detLength/2.0,detHeight/2.0};
  const Geometry::Vec3D XYZ[]={-Y,Y,-X,X,-Z,Z};
  for(size_t i=0;i<6;i++)
    {
      FixedComp::setLinkSurf(i,sign*SMap.realSurf(detIndex+surfN));
      FixedComp::setConnect(i,Origin+XYZ[i]*(T[i/2]+wallThick),XYZ[i]); 
      sign*=-1;
      surfN++;
    }
  return;
}

void
DetectorBank::createTally(Simulation& System) const
  /*!
    Extrenal build everything
    \param System :: Simulation
   */
{
  ELog::RegMethod RegA("DetectorBank","createTally");
  if (!nDet) return;

  int tNum(5);
  const double XStep=detLength/static_cast<double>(nDet+1);
  double XPos=(-detLength/2.0);
  for(size_t i=0;i<nDet;i++)
    {
      tNum=System.nextTallyNum(tNum);
      const Geometry::Vec3D DPt=Origin+X*XPos;
      tallySystem::addF5Tally(System,tNum);      
      tallySystem::setF5Position(System,tNum,DPt);
      tallySystem::setTallyTime(System,tNum,"");
      tallySystem::setEnergy(System,tNum,"");
      XPos+=XStep;
    }

  return;
}

void
DetectorBank::createTally(Transport::DetGroup& DU) const
  /*!
    Extrenal build everything
    \param DU :: Detector Unit
   */
{
  ELog::RegMethod RegA("DetectorBank","createTally(DetGroup)");
  if (!nDet) return;

  const double XStep=detLength/static_cast<double>(nDet+1);
  double XPos=(-detLength/2.0);
  for(size_t i=0;i<nDet;i++)
    {
      const Geometry::Vec3D DPt=Origin+X*XPos;
      XPos+=XStep;
      Transport::PointDetector* DPtr=
	new Transport::PointDetector
	(bankNumber*100+i,DPt);

      Geometry::Vec3D CX(DPt.unit());
      const double cosA=CX.dotProd(Geometry::Vec3D(0,1,0));
      DPtr->setAngle(180.0*acos(cosA)/M_PI);
      DU.manageDetector(DPtr);
    }

  return;
}

void
DetectorBank::createAll(Simulation& System,const attachSystem::FixedComp& FC)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComp to add
   */
{
  ELog::RegMethod RegA("DetectorBank","createAll");
  populate(System.getDataBase());

  createUnitVector(FC);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE instrumentSystem
