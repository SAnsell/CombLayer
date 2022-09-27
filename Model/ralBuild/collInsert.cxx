/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   ralBuild/collInsert.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <complex>
#include <set>
#include <map>
#include <list>
#include <vector>
#include <string>
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "Exception.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "collInsert.h"

namespace shutterSystem
{

collInsert::collInsert(const std::string& key) :
  attachSystem::FixedRotate(key,2),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

double
collInsert::calcDrop(const double R) const
  /*!
    Calculate the effective drop of the beam
    so the shutter fall in the correct place
    \param R :: Radial distance to calculate
  */
{
  const double drop=R*tan(M_PI*xAngle/180.0);
  return drop-zStep;
}
  
void
collInsert::populate(const FuncDataBase& Control)
  /*!
    Populate main variabe
    \param Control :: DataBase to use
   */
{
  ELog::RegMethod RegA("collInsert","populate");

  FixedRotate::populate(Control);

  nB4C=Control.EvalVar<size_t>(keyName+"NB4C");
  nSteel=Control.EvalVar<size_t>(keyName+"NSteel");
  fStep=Control.EvalVar<double>(keyName+"FStep");
  length=Control.EvalVar<double>(keyName+"Length");
  hGap=Control.EvalVar<double>(keyName+"HGap");
  vGap=Control.EvalVar<double>(keyName+"VGap");
  hGapRAngle=Control.EvalVar<double>(keyName+"HGapRAngle");
  vGapRAngle=Control.EvalVar<double>(keyName+"VGapRAngle");

  b4cThick=Control.EvalVar<double>(keyName+"B4CThick");
  b4cSpace=Control.EvalVar<double>(keyName+"B4CSpace");

  steelOffset=Control.EvalVar<double>(keyName+"SteelOffset");
  steelAWidth=Control.EvalVar<double>(keyName+"SteelAWidth");
  steelBWidth=Control.EvalVar<double>(keyName+"SteelBWidth");

  b4cMat=ModelSupport::EvalMat<int>(Control,keyName+"B4CMat");
  steelMat=ModelSupport::EvalMat<int>(Control,keyName+"SteelMat");
  if (nB4C<3)
    throw ColErr::IndexError<size_t>(nB4C,3,"nB4C too small");

  if (nSteel<1)
    throw ColErr::IndexError<size_t>(nB4C,3,"nB4C too small");


  return;
}


  
void
collInsert::createSurfaces()
  /*!
    Constrcut inner surfaces
   */
{
  ELog::RegMethod RegA("collInsert","createSurfaces");

  
  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+3,Origin-X*(hGap/2.0),X,
				  Z,-hGapRAngle);
  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+4,Origin+X*(hGap/2.0),X,
				  Z,hGapRAngle);
  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+5,Origin-Z*(vGap/2.0),Z,
				  X,vGapRAngle);
  ModelSupport::buildPlaneRotAxis(SMap,buildIndex+6,Origin+Z*(vGap/2.0),Z,
				  X,-vGapRAngle);

  // Steel layer
  ModelSupport::buildPlaneRotAxis
    (SMap,buildIndex+13,Origin-X*(steelOffset+hGap/2.0),X,Z,-hGapRAngle);
  ModelSupport::buildPlaneRotAxis
    (SMap,buildIndex+14,Origin+X*(steelOffset+hGap/2.0),X,Z,hGapRAngle);
  ModelSupport::buildPlaneRotAxis
    (SMap,buildIndex+15,Origin-Z*(steelOffset+vGap/2.0),Z,X,vGapRAngle);
  ModelSupport::buildPlaneRotAxis
    (SMap,buildIndex+16,Origin+Z*(steelOffset+vGap/2.0),Z,X,-vGapRAngle);

    // Steel layer
  ModelSupport::buildPlaneRotAxis
    (SMap,buildIndex+23,Origin-X*(steelAWidth+hGap/2.0),X,Z,-hGapRAngle);
  ModelSupport::buildPlaneRotAxis
    (SMap,buildIndex+24,Origin+X*(steelAWidth+hGap/2.0),X,Z,hGapRAngle);
  ModelSupport::buildPlaneRotAxis
    (SMap,buildIndex+25,Origin-Z*(steelAWidth+vGap/2.0),Z,X,vGapRAngle);
  ModelSupport::buildPlaneRotAxis
    (SMap,buildIndex+26,Origin+Z*(steelAWidth+vGap/2.0),Z,X,-vGapRAngle);

    // Steel layer
  ModelSupport::buildPlaneRotAxis
    (SMap,buildIndex+33,Origin-X*(steelBWidth+hGap/2.0),X,Z,-hGapRAngle);
  ModelSupport::buildPlaneRotAxis
    (SMap,buildIndex+34,Origin+X*(steelBWidth+hGap/2.0),X,Z,hGapRAngle);
  ModelSupport::buildPlaneRotAxis
    (SMap,buildIndex+35,Origin-Z*(steelBWidth+vGap/2.0),Z,X,vGapRAngle);
  ModelSupport::buildPlaneRotAxis
    (SMap,buildIndex+36,Origin+Z*(steelBWidth+vGap/2.0),Z,X,-vGapRAngle);


  // calculate two length : gap for a B4C pair  and a steel block
  const double b4cUnit=b4cSpace*3.0+b4cThick*2.0;
  const double steelUnit=(length-b4cUnit*static_cast<double>(nB4C))/
    static_cast<double>((nB4C-1)*nSteel);

  int BI(buildIndex+100);
  Geometry::Vec3D Org(Origin+Y*fStep);
  for(size_t i=0;i<nB4C;i++)
    {
      // B4C block
      ModelSupport::buildPlane(SMap,BI+1,Org,Y);
      Org+=Y*b4cSpace;
      ModelSupport::buildPlane(SMap,BI+11,Org,Y);
      Org+=Y*b4cThick;
      ModelSupport::buildPlane(SMap,BI+12,Org,Y);
      Org+=Y*b4cSpace;
      ModelSupport::buildPlane(SMap,BI+21,Org,Y);
      Org+=Y*b4cThick;
      ModelSupport::buildPlane(SMap,BI+22,Org,Y);
      Org+=Y*b4cSpace;

      if (i!=nB4C-1)
	{
	  int SI(BI);
	  for(size_t j=0;j<nSteel;j++)
	    {
	      ModelSupport::buildPlane(SMap,SI+31,Org,Y);
	      Org+=Y*steelUnit;
	      SI++;
	    }
	  BI+=100;
	}
    }
      // last surface
  ModelSupport::buildPlane(SMap,BI+31,Org,Y);
  return;
}

  
void
collInsert::createObjects(Simulation& System)
  /*!
    Construc int
   */
{
  ELog::RegMethod RegA("collInsert","createObject");

  const HeadRule& RInnerComp=ExternalCut::getComplementRule("RInner");
  const HeadRule& ROuterHR=ExternalCut::getRule("ROuter");
  const HeadRule& RDivider=ExternalCut::getRule("Divider");

  // inner void
  
  // inner steel layer
  const HeadRule innerHR=
    ModelSupport::getHeadRule(SMap,buildIndex,"23 -24 25 -26");
  // outer steel layer
  const HeadRule outerHR=
    ModelSupport::getHeadRule(SMap,buildIndex,"33 -34 35 -36");

  const HeadRule steelHR=
    ModelSupport::getHeadRule(SMap,buildIndex,"13 -14 15 -16");
  // mid divider
  const HeadRule midHR=
    HeadRule(SMap,buildIndex+100*(1+nB4C/2),1);

  HeadRule HR;
  // front space
  HR=HeadRule(SMap,buildIndex+100,-1);
  System.addCell(cellIndex++,0,0.0,HR*RInnerComp*innerHR*RDivider);

  const HeadRule voidHR=
    ModelSupport::getHeadRule(SMap,buildIndex,"-3:4:-5:6");

  
  int BI(buildIndex+100);
  int b4cI(buildIndex);
  HeadRule limitHR(innerHR);
  for(size_t i=0;i<nB4C;i++)
    {
      if (i==nB4C/2)
	{
	  limitHR=outerHR;
	  b4cI+=10;
	}

      HR=ModelSupport::getHeadRule(SMap,BI,"1 -11");
      System.addCell(cellIndex++,0,0.0,HR*limitHR*voidHR);

      HR=ModelSupport::getHeadRule(SMap,BI,buildIndex,"11 -12 (-3M:4M)");
      System.addCell(cellIndex++,b4cMat,0.0,HR*limitHR);
      HR=ModelSupport::getHeadRule
	(SMap,BI,buildIndex,b4cI,"11 -12 3M -4M -5M 25N");
      System.addCell(cellIndex++,0,0.0,HR*limitHR);
      HR=ModelSupport::getHeadRule
	(SMap,BI,buildIndex,b4cI,"11 -12 3M -4M 6M -26N");
      System.addCell(cellIndex++,0,0.0,HR*limitHR);

      HR=ModelSupport::getHeadRule(SMap,BI,"12 -21");
      System.addCell(cellIndex++,0,0.0,HR*limitHR*voidHR);

      HR=ModelSupport::getHeadRule(SMap,BI,buildIndex,"21 -22 (-5M:6M)");
      System.addCell(cellIndex++,b4cMat,0.0,HR*limitHR);
      HR=ModelSupport::getHeadRule
	(SMap,BI,buildIndex,b4cI,"21 -22 5M -6M -3M 23N");
      System.addCell(cellIndex++,0,0.0,HR);
      HR=ModelSupport::getHeadRule
	(SMap,BI,buildIndex,b4cI,"21 -22 5M -6M 4M -24N");
      System.addCell(cellIndex++,0,0.0,HR);
      
      HR=ModelSupport::getHeadRule(SMap,BI,"22 -31");
      System.addCell(cellIndex++,0,0.0,HR*limitHR*voidHR);
      
      if (i!=nB4C-1)
	{
	  int SI(BI);
	  for(size_t j=0;j<nSteel;j++)
	    {
	      HR=ModelSupport::getAltHeadRule(SMap,SI,BI,"31 -32A -101MB");
	      
	      System.addCell(cellIndex++,0,0.0,HR*steelHR*voidHR);
	      System.addCell(cellIndex++,steelMat,0.0,
			     HR*steelHR.complement()*limitHR);
	      SI++;
	    }
	  BI+=100;
	}
    }
  // last surface
  HR=HeadRule(SMap,BI,31);
  System.addCell(cellIndex++,0,0.0,HR*ROuterHR*outerHR*RDivider);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,BI,"101 3 -4 5 -6 -31M");
  System.addCell(cellIndex++,0,0.0,HR);

  addOuterSurf(innerHR*midHR.complement());
  addOuterUnionSurf(outerHR*midHR);
  return;
}
  
void
collInsert::createLinks()
  /*!
    Creates a full attachment set
    0 - 1 standard points
    2 - 3 beamaxis points
  */
{
  ELog::RegMethod RegA("collInsert","createLinks");
  /*
  attachSystem::FixedComp& mainFC=getKey("Main");
  attachSystem::FixedComp& beamFC=getKey("Beam");
  
  mainFC.setConnect(0,Origin,-Y);
  mainFC.setConnect(1,Origin+Y*length,Y);
 
  mainFC.setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  mainFC.setLinkSurf(1,SMap.realSurf(buildIndex+2));

  const Geometry::Vec3D beamOrigin(beamFC.getCentre());
  const Geometry::Vec3D bY(beamFC.getY());

  beamFC.setConnect(0,beamOrigin,-bY);
  beamFC.setConnect(1,beamOrigin+bY*length,bY);
 
  beamFC.setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  beamFC.setLinkSurf(1,SMap.realSurf(buildIndex+2));
  */
  return;
}


Geometry::Vec3D
collInsert::getWindowCentre() const
  /*!
    Calculate the effective window in the void
    \return Centre 
  */
{
  ELog::RegMethod RegA("collInsert","getWindowCentre");

  /*
  const attachSystem::FixedComp& beamFC=getKey("Beam");
  const Geometry::Vec3D beamOrigin(beamFC.getCentre());
  */
  return Origin;
}
  
  
void
collInsert::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int sideIndex)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: FixedComp (shutter)
    \param sideIndex :: side poitn
  */
{
  ELog::RegMethod RegA("collInsert","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

} // NAMESPACE shutterSystem
