/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Build/MerlinModerator.cxx
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
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "t1Reflector.h"
#include "VanePoison.h"
#include "MerlinModerator.h"

namespace ts1System
{

MerlinModerator::MerlinModerator(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,12),
  merlinIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(merlinIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

MerlinModerator::MerlinModerator(const MerlinModerator& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  merlinIndex(A.merlinIndex),cellIndex(A.cellIndex),
  xStep(A.xStep),yStep(A.yStep),zStep(A.zStep),
  xyAngle(A.xyAngle),width(A.width),depth(A.depth),
  height(A.height),innerThick(A.innerThick),vacThick(A.vacThick),
  nPoison(A.nPoison),vaneSide(A.vaneSide),poisonYStep(A.poisonYStep),
  poisonThick(A.poisonThick),alMat(A.alMat),waterMat(A.waterMat),
  poisonMat(A.poisonMat),modLayer(A.modLayer),mainCell(A.mainCell)
  /*!
    Copy constructor
    \param A :: MerlinModerator to copy
  */
{}

MerlinModerator&
MerlinModerator::operator=(const MerlinModerator& A)
  /*!
    Assignment operator
    \param A :: MerlinModerator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      xyAngle=A.xyAngle;
      width=A.width;
      depth=A.depth;
      height=A.height;
      innerThick=A.innerThick;
      vacThick=A.vacThick;
      nPoison=A.nPoison;
      vaneSide=A.vaneSide;
      poisonYStep=A.poisonYStep;
      poisonThick=A.poisonThick;
      alMat=A.alMat;
      waterMat=A.waterMat;
      poisonMat=A.poisonMat;
      modLayer=A.modLayer;
      mainCell=A.mainCell;
    }
  return *this;
}

MerlinModerator::~MerlinModerator() 
 /*!
   Destructor
 */
{}

void
MerlinModerator::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: Database to use
 */
{
  ELog::RegMethod RegA("MerlinModerator","populate");

  // Master values
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYangle");

  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");

  innerThick=Control.EvalVar<double>(keyName+"InnerThick");
  vacThick=Control.EvalVar<double>(keyName+"VacThick");

  nPoison=Control.EvalVar<size_t>(keyName+"NPoison");
  vaneSide=Control.EvalVar<long int>(keyName+"VaneSide");
  double ys,t;
  for(size_t i=0;i<nPoison;i++)
    {
      ys=Control.EvalVar<double>(keyName+
				 StrFunc::makeString("PoisonYStep",i+1));
      t=Control.EvalVar<double>(keyName+
				StrFunc::makeString("PoisonThick",i+1));
      poisonYStep.push_back(ys);
      poisonThick.push_back(t);
    } 
  
  // Materials
  alMat=ModelSupport::EvalMat<int>(Control,keyName+"AlMat");
  waterMat=ModelSupport::EvalMat<int>(Control,keyName+"WaterMat");
  poisonMat=ModelSupport::EvalMat<int>(Control,keyName+"PoisonMat");

  applyModification();
  return;
}
  
void
MerlinModerator::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    - Y Down the beamline
    \param FC :: Linked object
  */
{
  ELog::RegMethod RegA("MerlinModerator","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC);

  FixedComp::applyShift(xStep,yStep,zStep);
  FixedComp::applyAngleRotate(xyAngle,0);
  return;
}

void
MerlinModerator::applyModification() 
  /*!
    Create the modified layer set
    layers are numbered in set of 10 going from the inner all
    (no modification) to 50 at the outer. Then in -/+ (Y,X,Z).
  */
{
  return;
}

Geometry::Vec3D
MerlinModerator::getSurfacePoint(const size_t layerIndex,
				 const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link point
    \param sideIndex :: Side [0-5]
    \param layerIndex :: layer, 0 is inner moderator [0-2]
    \return Surface point
  */
{
  ELog::RegMethod RegA("MerlinModerator","getSurfacePoint");

  if (layerIndex>2) 
    throw ColErr::IndexError<size_t>(layerIndex,2,"layer");
  const size_t SI((sideIndex>0) ?
                  static_cast<size_t>(sideIndex-1) :
                  static_cast<size_t>(-1-sideIndex));

  // Modification map:
  std::map<size_t,double>::const_iterator mc;
  const double layer[]={innerThick,vacThick};
  double T(0.0);
  for(size_t i=0;i<layerIndex;i++)
    {
      mc=modLayer.find(i*10+SI+1);
      T+=(mc!=modLayer.end()) ? mc->second : layer[i];
    }


  switch(SI)
    {
    case 0:
      return Origin-Y*(depth/2.0+T);
    case 1:
      return Origin+Y*(depth/2.0+T);
    case 2:
      return Origin-X*(width/2.0+T);
    case 3:
      return Origin+X*(width/2.0+T);
    case 4:
      return Origin-Z*(height/2.0+T);
    case 5:
      return Origin+Z*(height/2.0+T);
    }
    
  throw ColErr::IndexError<long int>(sideIndex,6,"sideIndex ");
}

void
MerlinModerator::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("MerlinModerator","createSurface");

  // Multi layer -- to be generalized
  const size_t nLayer(3);
  int HI(merlinIndex);
  for(size_t i=0;i<nLayer;i++)
    {
      ModelSupport::buildPlane(SMap,HI+1,getSurfacePoint(i,1),Y);
      ModelSupport::buildPlane(SMap,HI+2,getSurfacePoint(i,2),Y);
      ModelSupport::buildPlane(SMap,HI+3,getSurfacePoint(i,3),X);
      ModelSupport::buildPlane(SMap,HI+4,getSurfacePoint(i,4),X);
      ModelSupport::buildPlane(SMap,HI+5,getSurfacePoint(i,5),Z);
      ModelSupport::buildPlane(SMap,HI+6,getSurfacePoint(i,6),Z);
      HI+=10;
    }
 

  // Poison layer:
  HI=100+merlinIndex;
  for(size_t i=0;i<nPoison;i++)
    {
      ModelSupport::buildPlane(SMap,HI+3,Origin+
			       Y*(poisonYStep[i]-poisonThick[i]/2.0),Y);
      ModelSupport::buildPlane(SMap,HI+4,Origin+
			       Y*(poisonYStep[i]+poisonThick[i]/2.0),Y);
      HI+=10;
    }
  return;
}

void
MerlinModerator::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("MerlinModerator","createObjects");

  
  std::string Out,Exclude;
  // Poison bits first:

  int PI(merlinIndex+100);
  for(size_t i=0;i<nPoison;i++)
    {
      Out=ModelSupport::getComposite(SMap,merlinIndex,PI,"3M -4M 3 -4 5 -6 ");
      Exclude+=ModelSupport::getComposite(SMap,PI,"(-3M : 4M)");
      System.addCell(MonteCarlo::Qhull(cellIndex++,poisonMat,0.0,Out));
      PI+=10;
    }      

  Out=ModelSupport::getComposite(SMap,merlinIndex,"1 -2 3 -4 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,0.0,Out+Exclude));
  mainCell=cellIndex-1;
  // Inner al
  Out=ModelSupport::getComposite(SMap,merlinIndex,"11 -12 13 -14 15 -16 "
				 " (-1:2:-3:4:-5:6) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,0.0,Out));
  // Vac/clearance layer
  Out=ModelSupport::getComposite(SMap,merlinIndex,"21 -22 23 -24 25 -26 "
				 " (-11:12:-13:14:-15:16) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  Out=ModelSupport::getComposite(SMap,merlinIndex," 21 -22 23 -24 25 -26 ");
  addOuterSurf(Out);
  addBoundarySurf(Out);

  return;
}

void
MerlinModerator::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("MerlinModerator","createLinks");

  // set Links:
  FixedComp::setConnect(0,getSurfacePoint(2,1),-Y);
  FixedComp::setConnect(1,getSurfacePoint(2,2),Y);
  FixedComp::setConnect(2,getSurfacePoint(2,3),-X);
  FixedComp::setConnect(3,getSurfacePoint(2,4),X);
  FixedComp::setConnect(4,getSurfacePoint(2,5),-Z);
  FixedComp::setConnect(5,getSurfacePoint(2,6),Z);

  FixedComp::setLinkSurf(0,-SMap.realSurf(merlinIndex+21));
  FixedComp::setLinkSurf(1,SMap.realSurf(merlinIndex+22));
  FixedComp::setLinkSurf(2,-SMap.realSurf(merlinIndex+23));
  FixedComp::setLinkSurf(3,SMap.realSurf(merlinIndex+24));
  FixedComp::setLinkSurf(4,-SMap.realSurf(merlinIndex+25));
  FixedComp::setLinkSurf(5,SMap.realSurf(merlinIndex+26));

  // -- Inner surface for vanes 
  // As inner surface needs to be reversed
  FixedComp::setLinkSurf(6,SMap.realSurf(merlinIndex+1));
  FixedComp::setLinkSurf(7,-SMap.realSurf(merlinIndex+2));
  FixedComp::setLinkSurf(8,SMap.realSurf(merlinIndex+3));
  FixedComp::setLinkSurf(9,-SMap.realSurf(merlinIndex+4));
  FixedComp::setLinkSurf(10,SMap.realSurf(merlinIndex+5));
  FixedComp::setLinkSurf(11,-SMap.realSurf(merlinIndex+6));

  FixedComp::setConnect(6,getSurfacePoint(0,1),Y);
  FixedComp::setConnect(7,getSurfacePoint(0,2),-Y);
  FixedComp::setConnect(8,getSurfacePoint(0,3),X);
  FixedComp::setConnect(9,getSurfacePoint(0,4),-X);
  FixedComp::setConnect(10,getSurfacePoint(0,5),-Z);
  FixedComp::setConnect(11,getSurfacePoint(0,6),Z);


  return;
}

void
MerlinModerator::createVanes(Simulation& System)
  /*!
    Add vane poisoning if needed
    \param System :: Simulation for cell addition
  */
{
  ELog::RegMethod RegA("MerlinModerator","createVanes");
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  std::shared_ptr<moderatorSystem::VanePoison> 
    VaneObj(new moderatorSystem::VanePoison(keyName+"Vanes"));
  OR.addObject(VaneObj);

  VaneObj->addInsertCell(mainCell);
  const long int VS(vaneSide>0 ? vaneSide+6 : vaneSide-6);
  VaneObj->createAll(System,*this,VS);

  return;
}

void
MerlinModerator::createAll(Simulation& System,
			  const attachSystem::FixedComp& FC)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: Fixed Component to place object within
  */
{
  ELog::RegMethod RegA("MerlinModerator","createAll");
  populate(System.getDataBase());

  createUnitVector(FC);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);
  if (vaneSide)
    createVanes(System);
  return;
}

  
}  // NAMESPACE ts1System
