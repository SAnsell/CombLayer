/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/MultiChannel.cxx
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
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "surfDBase.h"
#include "MultiChannel.h"


namespace constructSystem
{

MultiChannel::MultiChannel(const std::string& Key) :
  attachSystem::ContainedComp(),attachSystem::FixedOffset(Key,2),
  attachSystem::CellMap(),attachSystem::SurfMap(),
  chnIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(chnIndex+1),setFlag(0)
  /*!
    Default constructor
    \param Key :: Key name for variables
  */
{}
  
MultiChannel::MultiChannel(const MultiChannel& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  attachSystem::CellMap(A),attachSystem::SurfMap(A),
  chnIndex(A.chnIndex),cellIndex(A.cellIndex),
  setFlag(A.setFlag),topRule(A.topRule),baseRule(A.baseRule),
  topSurf(A.topSurf),baseSurf(A.baseSurf),
  divider(A.divider),leftStruct(A.leftStruct),
  rightStruct(A.rightStruct),nBlades(A.nBlades),
  bladeThick(A.bladeThick),length(A.length),
  bladeMat(A.bladeMat),voidMat(A.voidMat)
  /*!
    Copy constructor
    \param A :: MultiChannel to copy
  */
{}

MultiChannel&
MultiChannel::operator=(const MultiChannel& A)
  /*!
    Assignment operator
    \param A :: MultiChannel to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      
      cellIndex=A.cellIndex;
      setFlag=A.setFlag;
      topSurf=A.topSurf;
      baseSurf=A.baseSurf;
      topRule=A.topRule;
      baseRule=A.baseRule;
      divider=A.divider;
      leftStruct=A.leftStruct;
      rightStruct=A.rightStruct;
      nBlades=A.nBlades;
      bladeThick=A.bladeThick;
      length=A.length;
      bladeMat=A.bladeMat;
      voidMat=A.voidMat;
    }
  return *this;
}


  
void
MultiChannel::populate(const FuncDataBase& Control)
  /*!
    Sets the size of the object
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("MultiChannel","populate");

  FixedOffset::populate(Control);

  nBlades=Control.EvalVar<size_t>(keyName+"NBlades");
  bladeThick=Control.EvalVar<double>(keyName+"BladeThick");
  length=Control.EvalVar<double>(keyName+"Length");

  
  bladeMat=ModelSupport::EvalMat<int>(Control,keyName+"BladeMat");
  voidMat=ModelSupport::EvalDefMat<int>(Control,keyName+"VoidMat",0);
  
  return;
}

void
MultiChannel::createUnitVector(const attachSystem::FixedComp& FC,
                               const long int sideIndex)
  /*!
    Create the unit vectors: Note only to construct front/back surf
    \param FC :: Centre point
    \param sideIndex :: Side index
  */
{
  ELog::RegMethod RegA("MultiChannel","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  
  return;
}


void
MultiChannel::processSurface(const size_t index,
                             const double aFrac,
                             const double bFrac)
  /*!
    Process the surfaces and convert them into merged layers
    \param index :: index offset 
    \param aFrac :: Frac of side A
    \param bFrac :: Frac of side B 
  */
{
  ELog::RegMethod RegA("MultiChannel","processSurface");
  //
  // Currently we only can deal with two types of surface [ plane/plane
  // and plane/cylinder
  
  const int surfN(chnIndex+static_cast<int>(index)*10+3);
  int surfNX(surfN);

  Geometry::Surface* PX=
    ModelSupport::surfDBase::generalSurf(topSurf,baseSurf,aFrac,surfNX);
  SMap.addToIndex(surfN,PX->getName());
  addSurf("bladesA",surfN);

  PX=ModelSupport::surfDBase::generalSurf(topSurf,baseSurf,bFrac,surfNX);
  SMap.addToIndex(surfN+1,PX->getName());
  addSurf("bladesB",surfN+1);
  return;
}

void
MultiChannel::createSurfaces()
  /*!
    Create All the surfaces
   */
{
  ELog::RegMethod RegA("MultiChannel","createSurface");

  ELog::EM<<"Surface == "<<Origin<<ELog::endDiag;
  ELog::EM<<"Surface == "<<length<<ELog::endDiag;
  
  ModelSupport::buildPlane(SMap,chnIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,chnIndex+2,Origin+Y*(length/2.0),Y);
  
  // first problem is to determine the build step:
  const double TotalD=topSurf->distance(Origin)+
    baseSurf->distance(Origin);

  const double BladeTotal(static_cast<double>(nBlades)*bladeThick);
  const double voidThick((TotalD-BladeTotal)/
			 static_cast<double>(nBlades+1));

  if (TotalD<BladeTotal+Geometry::zeroTol)
    throw ColErr::SizeError<double>(TotalD,BladeTotal,
       "Distance:BladeThick["+StrFunc::makeString(nBlades)+"]");
  

  double DPosA(voidThick);
  double DPosB(voidThick+bladeThick);
  for(size_t i=0;i<nBlades;i++)
    {
      processSurface(i,1.0-DPosA/TotalD,1.0-DPosB/TotalD);
      DPosA+=voidThick+bladeThick;
      DPosB+=voidThick+bladeThick;
    }
  return;
}

void
MultiChannel::createObjects(Simulation& System) 
  /*!
    Creates the colllimator block
    \param System :: Simuation for object
  */
{
  ELog::RegMethod RegA("MultiChannel","createObjects");
  std::string Out;


  std::string FB=ModelSupport::getComposite(SMap,chnIndex,"1 -2");
  FB+=leftStruct.display()+rightStruct.display();

  HeadRule BHR(baseRule);
  int SN(chnIndex);
  for(size_t i=0;i<nBlades;i++)
    {
      Out=BHR.display()+ModelSupport::getComposite(SMap,SN," -3 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,voidMat,0.0,Out+FB));

      Out=ModelSupport::getComposite(SMap,SN," 3 -4 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,bladeMat,0.0,Out+FB));
      
      Out=ModelSupport::getComposite(SMap,SN," 4 ");
      BHR.procString(Out);
      SN+=10;
    }
  // LAST Volume
  Out=BHR.display()+topRule.display();
  System.addCell(MonteCarlo::Qhull(cellIndex++,voidMat,0.0,Out+FB));
  
  Out=FB+baseRule.display()+" "+topRule.display();
  addOuterSurf(Out);
  return;
}

void
MultiChannel::createLinks()
  /*!
    Construct the links for the system
  */
{
  ELog::RegMethod RegA("MultiChannel","createLinks");

  FixedComp::setConnect(0,Origin-Y*(length/2.0),-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(chnIndex+1));
  FixedComp::setConnect(1,Origin+Y*(length/2.0),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(chnIndex+2));      
  
  return;
}

void
MultiChannel::setFaces(const int BS,const int TS)
  /*!
    Set the top/base surface numbers
    \param BS :: Base surface number
    \param TS :: Top surface number
  */
{
  ELog::RegMethod RegA("MultiChannel","setFaces");

  // DEAL WITH MIRROR PLANES:
  baseSurf=SMap.realSurfPtr(BS);
  topSurf=SMap.realSurfPtr(TS);
  if (!baseSurf)
    throw ColErr::InContainerError<int>(BS,"BaseSurf");
  if (!topSurf)
    throw ColErr::InContainerError<int>(TS,"TopSurf");

  baseRule.procSurface(baseSurf);
  topRule.procSurface(topSurf);

  // NOTE: Top rule is complement originally
  if (TS<0) topRule.makeComplement();
  if (BS<0) baseRule.makeComplement();
  setFlag |= 1;
  return;
}

void
MultiChannel::setFaces(const attachSystem::FixedComp& FC,
		       const long int BS,const long int TS)
  /*!
    Set the top/base surface numbers
    \param FC :: Fixed Component to use
    \param BS :: Base surface link point
    \param TS :: Top surface link point
  */
{
  ELog::RegMethod RegA("MultiChannel","setFaces<FC>");

  const int baseSurfN=FC.getLinkSurf(BS);
  const int topSurfN=FC.getLinkSurf(TS);
  setFaces(baseSurfN,topSurfN);

  return;
}
  
void
MultiChannel::setDivider(const HeadRule& HR)
  /*!
    Set the inner volume
    \param HR :: Headrule of inner surfaces
  */
{
  ELog::RegMethod RegA("MultiChannel","setInner");

  divider=HR;
  setFlag |= 4;
  return;
}

void
MultiChannel::setLeftRight(const HeadRule& LR,const HeadRule& RR)
  /*!
    Set the left/right sides
    \param LR :: HeadRule of left surface
    \param RR :: HeadRule of right surface
  */
{
  ELog::RegMethod RegA("MultiChannel","setLeftRight");

  leftStruct=LR;
  rightStruct=RR;
  setFlag |= 2;
  return;
}

void
MultiChannel::setLeftRight(const attachSystem::FixedComp& FCA,
                           const long int lIndex,
			   const attachSystem::FixedComp& FCB,
                           const long int rIndex)
  /*!
    Set the left/right sides
    \param FCA :: Left fixed comp
    \param lIndex :: left side index
    \param FCB :: Left fixed comp
    \param rIndex :: right side index
  */
{
  ELog::RegMethod RegA("MultiChannel","setLeftRight(FC)");

  leftStruct=FCA.getFullRule(lIndex);
  rightStruct=FCB.getFullRule(rIndex);
  setFlag |= 2;
  return;
}


  
  
void
MultiChannel::createAll(Simulation& System,
                     const attachSystem::FixedComp& FC,
                     const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation 
    \param FC :: Fixed component to set axis etc
    \param sideIndex :: position of linkpoint
  */
{
  ELog::RegMethod RegA("MultiChannel","createAll");


  ELog::EM<<"SET FLAG ="<<setFlag<<ELog::endDiag;
  if ((setFlag & 3) == 3)
    {
      populate(System.getDataBase());
      createUnitVector(FC,sideIndex);
      createSurfaces();
      createObjects(System);
      createLinks();
      insertObjects(System);
    }
  else
    ELog::EM<<"MC channel["<<keyName<<"] no configured"<<ELog::endWarn;
  return;
}

  
}  // NAMESPACE constructSystem
