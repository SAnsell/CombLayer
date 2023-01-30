/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/PreModWing.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell/Konstantin Batkov
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
#include <array>
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "surfRegister.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "PreModWing.h"


namespace essSystem
{

PreModWing::PreModWing(const std::string& Key) :
  attachSystem::FixedOffset(Key,3),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap()
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

PreModWing::PreModWing(const PreModWing& A) : 
  attachSystem::FixedOffset(A),
  attachSystem::ContainedComp(A),
  attachSystem::ExternalCut(A),
  attachSystem::CellMap(A),
  innerHeight(A.innerHeight),outerHeight(A.outerHeight),
  innerDepth(A.innerDepth),outerDepth(A.outerDepth),
  wallThick(A.wallThick),innerRadius(A.innerRadius),
  outerRadius(A.outerRadius),innerYCut(A.innerYCut),
  mat(A.mat),wallMat(A.wallMat)
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
      attachSystem::FixedOffset::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      attachSystem::CellMap::operator=(A);
      innerHeight=A.innerHeight;
      outerHeight=A.outerHeight;
      innerDepth=A.innerDepth;
      outerDepth=A.outerDepth;
      wallThick=A.wallThick;
      innerRadius=A.innerRadius;
      outerRadius=A.outerRadius;
      innerYCut=A.innerYCut;
      mat=A.mat;
      wallMat=A.wallMat;
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

  FixedOffset::populate(Control);
  
  innerHeight=Control.EvalVar<double>(keyName+"InnerHeight");
  outerHeight=Control.EvalVar<double>(keyName+"OuterHeight");
  innerDepth=Control.EvalVar<double>(keyName+"InnerDepth");
  outerDepth=Control.EvalVar<double>(keyName+"OuterDepth");

  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");
  outerRadius=Control.EvalVar<double>(keyName+"OuterRadius");
  innerYCut=Control.EvalVar<double>(keyName+"InnerYCut");

  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  nLayers=Control.EvalDefVar<size_t>(keyName+"NLayers",1);
  if (nLayers<1) nLayers=1;
  
  innerMat.push_back(mat);
  surfMat.push_back(wallMat);
  for(size_t i=1;i<nLayers;i++)
    {
      const std::string sNum=std::to_string(i);
      const double RL=Control.EvalVar<double>
        (keyName+"LayerRadius"+sNum);
      const int iMat=ModelSupport::EvalDefMat
        (Control,keyName+"InnerMat"+sNum,mat);
      const int sMat=ModelSupport::EvalDefMat
        (Control,keyName+"SurfMat"+sNum,wallMat);
      layerRadii.push_back(RL);
      innerMat.push_back(iMat);
      surfMat.push_back(sMat);

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


  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Z,innerRadius);
  if (!isActive("Outer"))
    {
      ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Z,outerRadius);
      setCutSurf("Outer",-SMap.realSurf(buildIndex+17));
    }

  // make height cone if given:
  const double IR(innerRadius>Geometry::zeroTol  ? innerRadius : 0.0);
  if (outerHeight>innerHeight+Geometry::zeroTol)
    {
      const double effHeight=
	  outerHeight-(outerHeight-innerHeight)*outerRadius/(outerRadius-IR);
      const double tanHTheta=(outerRadius-IR)/(outerHeight-innerHeight);
      const double thetaH=atan(tanHTheta)*180.0/M_PI;

      ModelSupport::buildCone(SMap,buildIndex+9,Origin+Z*effHeight,-Z,thetaH);
      ModelSupport::buildCone(SMap,buildIndex+19,
                              Origin+Z*(effHeight-wallThick),-Z,thetaH);
      ModelSupport::buildPlane(SMap,buildIndex+1006,Origin+Z*effHeight,Z);
      ModelSupport::buildPlane(SMap,buildIndex+1016,Origin+Z*(effHeight-wallThick),Z);

    }
        
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*innerHeight,Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(innerHeight-wallThick),Z);

  // make height cone if given:
  // calculate relative to outer zero [and correct to centre]
  if (outerDepth>innerDepth+Geometry::zeroTol)
    {
      const double effDepth=
	  outerDepth-(outerDepth-innerDepth)*outerRadius/(outerRadius-IR);

      const double tanDTheta=(outerRadius-IR)/(outerDepth-innerDepth);
      const double thetaD=atan(tanDTheta)*180.0/M_PI;

      ModelSupport::buildCone(SMap,buildIndex+8,Origin-Z*effDepth,Z,thetaD);
      ModelSupport::buildCone(SMap,buildIndex+18,Origin-Z*(effDepth-wallThick),Z,thetaD);
      // cutters for cone
      // not at lifted positoin
      ModelSupport::buildPlane(SMap,buildIndex+1005,Origin-Z*effDepth,Z);
      ModelSupport::buildPlane(SMap,buildIndex+1015,Origin-Z*(effDepth-wallThick),Z);
    }
  // flat section
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*innerDepth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(innerDepth-wallThick),Z);

  // Build layres for different material structure
  int CL(buildIndex+100);
  for(size_t i=1;i<nLayers;i++)
    {
      ModelSupport::buildCylinder(SMap,CL+7,Origin,Z,layerRadii[i-1]);
      CL+=100;
    }
  
  return; 
}

HeadRule
PreModWing::getLayerZone(const size_t layerIndex) const
  /*!
    Generate the inner zone for a given index
    \param layerIndex :: zone index
    \return HeadRule for surfaces in zone
  */
{
  ELog::RegMethod RegA("PreModWing","getLayerZone");

  HeadRule HR;
  
  const int CL(buildIndex+static_cast<int>(layerIndex)*100);
  if (!layerIndex)
    HR=getRule("Inner");
  else
    HR=getRule("Divider");

  HR*=HeadRule(SMap,CL,7);
  
  if (layerIndex+1==nLayers)
    HR*=getRule("Outer");
  else
    HR*=HeadRule(SMap,CL,-107);

  return HR;
}

  
void
PreModWing::createObjects(Simulation& System)

  /*!
    Create the disc component
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("PreModWing","createObjects");

  const HeadRule innerSurf=getRule("Inner");
  const HeadRule mainDivider=getRule("Divider");
  const HeadRule midSurf=getRule("Mid");
  const HeadRule baseSurf=getRule("Base");
  const HeadRule topSurf=getRule("Top");
  const HeadRule outerSurf=getRule("Outer");
  HeadRule HR;

  // BASE
  // inner first
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-5 -7");
  System.addCell(cellIndex++,mat,0.0,HR*baseSurf*innerSurf);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-15 5 -7");
  System.addCell(cellIndex++,wallMat,0.0,HR*innerSurf);

  // cone section
  for(size_t i=0;i<nLayers;i++)
    {
      const HeadRule Zone=getLayerZone(i);
      // base layer
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-8 -1005");
      System.addCell(cellIndex++,innerMat[i],0.0,HR*baseSurf*midSurf*Zone);

      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-18 -1015 (8:1005)");
      System.addCell(cellIndex++,surfMat[i],0.0,HR*baseSurf*midSurf*Zone);
      
      // Top layer
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-9 1006");
      System.addCell(cellIndex++,innerMat[i],0.0,
		     HR*midSurf*Zone*topSurf*getLayerZone(i));

      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-19 1016 (9:-1006)");
      System.addCell(cellIndex++,surfMat[i],0.0,HR*Zone*topSurf*midSurf);
    }
  
  // TOP
  // inner first
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"6 -7");
  System.addCell(cellIndex++,mat,0.0,HR*topSurf*innerSurf);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"16 -6 -7");
  System.addCell(cellIndex++,wallMat,0.0,HR*innerSurf);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"((-19 16 ) : (-18 -15))");
  addOuterSurf(HR*outerSurf*mainDivider);

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
		      const attachSystem::FixedComp& ModFC,
                      const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param ModFC :: Attachment point
    \param sideIndex :: link point
   */
{
  ELog::RegMethod RegA("PreModWing","createAll");


  populate(System.getDataBase());
  createUnitVector(ModFC,sideIndex);

  createSurfaces();
  createObjects(System);
  createLinks();

  insertObjects(System);

  //  if (engActive) 
  //    InnerComp->createAll(System,*this,7);


  return;
}

}  // NAMESPACE essSystem 
