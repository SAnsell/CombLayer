/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   photon/PlateMod.cxx
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
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "PlateMod.h"

namespace photonSystem
{
      
PlateMod::PlateMod(const std::string& Key) :
  attachSystem::ContainedComp(),attachSystem::FixedOffset(Key,6),
  plateIndex(ModelSupport::objectRegister::Instance().cell(Key)), 
  cellIndex(plateIndex+1)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

PlateMod::PlateMod(const PlateMod& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  attachSystem::CellMap(A),
  plateIndex(A.plateIndex),cellIndex(A.cellIndex),
  outerWidth(A.outerWidth),outerHeight(A.outerHeight),
  innerWidth(A.innerWidth),innerHeight(A.innerHeight),
  Layer(A.Layer),outerMat(A.outerMat)
  /*!
    Copy constructor
    \param A :: PlateMod to copy
  */
{}

PlateMod&
PlateMod::operator=(const PlateMod& A)
  /*!
    Assignment operator
    \param A :: PlateMod to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffset::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      cellIndex=A.cellIndex;
      outerWidth=A.outerWidth;
      outerHeight=A.outerHeight;
      innerWidth=A.innerWidth;
      innerHeight=A.innerHeight;
      Layer=A.Layer;
      outerMat=A.outerMat;
    }
  return *this;
}

PlateMod::~PlateMod()
  /*!
    Destructor
  */
{}


PlateMod*
PlateMod::clone() const
  /*!
    Clone copy constructor
    \return copy of this
  */
{
  return new PlateMod(*this);
}

void
PlateMod::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("PlateMod","populate");
  attachSystem::FixedOffset::populate(Control);


  outerHeight=Control.EvalVar<double>(keyName+"OuterHeight");
  outerWidth=Control.EvalVar<double>(keyName+"OuterWidth");
  innerHeight=Control.EvalVar<double>(keyName+"InnerHeight");
  innerWidth=Control.EvalVar<double>(keyName+"InnerWidth");
  outerMat=ModelSupport::EvalMat<int>(Control,keyName+"OuterMat");
  
  const size_t nLayers=Control.EvalVar<size_t>(keyName+"NLayer");
  if (!nLayers)
    throw ColErr::IndexError<size_t>(nLayers,1,"NLayers not zero");

  for(size_t i=0;i<nLayers;i++)
    {
      const std::string KN=StrFunc::makeString(i);
      plateInfo PI;
      PI.thick=Control.EvalVar<double>(keyName+"Thick"+KN);
      PI.vHeight=Control.EvalVar<double>(keyName+"VHeight"+KN);
      PI.vWidth=Control.EvalVar<double>(keyName+"VWidth"+KN);
      PI.mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat"+KN);
      PI.temp=Control.EvalDefVar<double>(keyName+"temp"+KN,0.0);
      Layer.push_back(PI);
    }
  return;
}

void
PlateMod::createUnitVector(const attachSystem::FixedComp& FC,
			   const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
  */
{
  ELog::RegMethod RegA("PlateMod","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);

  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,zAngle);

  return;
}

void
PlateMod::createSurfaces()
  /*!
    Create planes for the silicon and Polyethene layers
  */
{
  ELog::RegMethod RegA("PlateMod","createSurfaces");

  // Outer surf
  ModelSupport::buildPlane(SMap,plateIndex+3,Origin-X*(outerWidth/2.0),X);
  ModelSupport::buildPlane(SMap,plateIndex+4,Origin+X*(outerWidth/2.0),X);
  ModelSupport::buildPlane(SMap,plateIndex+5,Origin-Z*(outerHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,plateIndex+6,Origin+Z*(outerHeight/2.0),Z);

  // Inner layered boundary [easily convertable to multiple different]
  ModelSupport::buildPlane(SMap,plateIndex+13,Origin-X*(innerWidth/2.0),X);
  ModelSupport::buildPlane(SMap,plateIndex+14,Origin+X*(innerWidth/2.0),X);
  ModelSupport::buildPlane(SMap,plateIndex+15,Origin-Z*(innerHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,plateIndex+16,Origin+Z*(innerHeight/2.0),Z);

  
  int SI(plateIndex+100);
  Geometry::Vec3D OR(Origin);
  for(const plateInfo& PI : Layer)
    {
      ModelSupport::buildPlane(SMap,SI+1,OR,Y);
      // void in middle
      if (PI.vHeight>Geometry::zeroTol &&
          PI.vWidth>Geometry::zeroTol)
        {
          ModelSupport::buildPlane(SMap,SI+3,OR-X*(PI.vWidth/2.0),X);
          ModelSupport::buildPlane(SMap,SI+4,OR+X*(PI.vWidth/2.0),X);
          ModelSupport::buildPlane(SMap,SI+5,OR-Z*(PI.vHeight/2.0),Z);
          ModelSupport::buildPlane(SMap,SI+6,OR+Z*(PI.vHeight/2.0),Z);
        }
      OR+=Y*PI.thick;
      SI+=100;
    }
  // Add last plane
  ModelSupport::buildPlane(SMap,SI+1,OR,Y);  
  return; 
}

void
PlateMod::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("PlateMod","createObjects");

  std::string Out,voidCut;
  // Build outer layer

  const std::string innerContainer=
    ModelSupport::getComposite(SMap,plateIndex,"13 -14 15 -16 ");
  
  int SI(plateIndex+100);
  for(const plateInfo& PI : Layer)
    {
      // void in middle
      if (PI.vHeight>Geometry::zeroTol &&
          PI.vWidth>Geometry::zeroTol)
        {
          Out=ModelSupport::getComposite(SMap,SI,"1 -101 3 -4 5 -6 ");
          System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
          voidCut=ModelSupport::getComposite(SMap,SI,"(-3:4:-5:6)");
        }
      else
        voidCut="";
      
      Out=ModelSupport::getComposite(SMap,SI,"1 -101");
      System.addCell(MonteCarlo::Qhull(cellIndex++,PI.mat,PI.temp,
                                       Out+innerContainer+voidCut));
      SI+=100;

    }
  Out=ModelSupport::getComposite(SMap,plateIndex,SI,
                                 " 101 3 -4 5 -6 (-13:14:-15:16) -1M ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,outerMat,0.0,Out));


  Out=ModelSupport::getComposite(SMap,plateIndex,SI," 101 3 -4 5 -6 -1M ");
  addOuterSurf(Out);
  return; 
}

void
PlateMod::createLinks()
  /*!
    Creates a full attachment set
  */
{  
  ELog::RegMethod RegA("PlateMod","createLinks");

  double tThick(0.0);
  for(const plateInfo& PI : Layer)
    tThick+=PI.thick;
  const int NL(plateIndex+static_cast<int>(Layer.size()-1)*100);

  
  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(plateIndex+1));

  FixedComp::setConnect(1,Origin+Y*tThick,Y);
  FixedComp::setLinkSurf(1,-SMap.realSurf(NL+101));

  FixedComp::setConnect(2,Origin+Y*(tThick/2.0)-X*(outerWidth/2.0),-X);
  FixedComp::setLinkSurf(2,SMap.realSurf(plateIndex+3));
  
  FixedComp::setConnect(3,Origin+Y*(tThick/2.0)+X*(outerWidth/2.0),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(plateIndex+4));

  FixedComp::setConnect(4,Origin+Y*(tThick/2.0)-Z*(outerHeight/2.0),-Z);
  FixedComp::setLinkSurf(4,SMap.realSurf(plateIndex+5));

  FixedComp::setConnect(5,Origin+Y*(tThick/2.0)+Z*(outerHeight/2.0),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(plateIndex+6));

  return;
}

void
PlateMod::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
    \param sideIndex :: surface offset
   */
{
  ELog::RegMethod RegA("PlateMod","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE photonSystem
