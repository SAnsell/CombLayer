/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/TaperedFlightLine.cxx
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
#include <stack>
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
#include "Cone.h"
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
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "surfExpand.h"
#include "TaperedFlightLine.h"

namespace moderatorSystem
{

TaperedFlightLine::TaperedFlightLine(const std::string& Key)  :
  attachSystem::ContainedGroup("inner","outer"),
  attachSystem::FixedComp(Key,12),
  flightIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(flightIndex+1),nLayer(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

TaperedFlightLine::~TaperedFlightLine() 
 /*!
   Destructor
 */
{}

void
TaperedFlightLine::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: Database to use
 */
{
  ELog::RegMethod RegA("TaperedFlightLine","populate");
  
  // First get inner widths:
  xStep=Control.EvalVar<double>(keyName+"XStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYangle");
  zAngle=Control.EvalVar<double>(keyName+"Zangle");

  anglesXY[0]=Control.EvalVar<double>(keyName+"AngleXY1");
  anglesXY[1]=Control.EvalVar<double>(keyName+"AngleXY2");

  anglesZ[0]=Control.EvalVar<double>(keyName+"AngleZTop");
  anglesZ[1]=Control.EvalVar<double>(keyName+"AngleZBase");

  if (height<Geometry::zeroTol) {
    //    ELog::EM << "height<0 - this 'if' made memory problems before commenting out - fix this" << ELog::endDiag;
    // height=Control.EvalVar<double>(keyName+"Height"); // otherwise it was set by setHeight()
  }

  height=Control.EvalVar<double>(keyName+"Height"); // otherwise it was set by setHeight()
  width=Control.EvalVar<double>(keyName+"Width");

  innerMat=ModelSupport::EvalDefMat<int>(Control,keyName+"InnerMat",0);

  nLayer=Control.EvalDefVar<size_t>(keyName+"NLiner",0);
  lThick.clear();
  lMat.clear();
  for(size_t i=0;i<nLayer;i++)
    {
      const std::string idxStr=StrFunc::makeString(i+1);
      lThick.push_back(Control.EvalVar<double>(keyName+"LinerThick"+idxStr));
      lMat.push_back(ModelSupport::EvalMat<int>
		     (Control,keyName+"LinerMat"+idxStr));
    }  
  return;
}
  
void
TaperedFlightLine::createUnitVector(const attachSystem::FixedComp& FC,
				  const long int sideIndex)
  /*!
    Create the unit vectors
    - Y Points towards the beamline
    - X Across the Face
    - Z up (towards the target)
    \param FC :: A FixedComp to use as basis set
    \param sideIndex :: Index for centre and axis
  */
{
  ELog::RegMethod RegA("TaperedFlightLine","createUnitVector");
  FixedComp::createUnitVector(FC,sideIndex);

  applyShift(xStep,0,zStep);
  applyAngleRotate(xyAngle,zAngle);
  return;
}

void
TaperedFlightLine::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("TaperedFlightLine","createSurfaces");

  // Sides: Layers:
  Geometry::Vec3D xDircA(X);   
  Geometry::Vec3D xDircB(X);
  Geometry::Vec3D zDircA(Z);   
  Geometry::Vec3D zDircB(Z);

  Geometry::Quaternion::calcQRotDeg(anglesXY[0],Z).rotate(xDircA);
  Geometry::Quaternion::calcQRotDeg(-anglesXY[1],Z).rotate(xDircB);
  Geometry::Quaternion::calcQRotDeg(-anglesZ[0],X).rotate(zDircA);
  Geometry::Quaternion::calcQRotDeg(anglesZ[1],X).rotate(zDircB);


  ModelSupport::buildPlane(SMap,flightIndex+3,Origin-X*(width/2.0),xDircA);
  ModelSupport::buildPlane(SMap,flightIndex+4,Origin+X*(width/2.0),xDircB);

  // The following ifs check whether the flight line should be tappered
  // and builds either cone or plane
  if (anglesZ[0]>Geometry::zeroTol)
    ModelSupport::buildCone(SMap,flightIndex+5,Origin-Z*(height/2.0),Z,90-anglesZ[0],Origin[2]>0 ? -1 : 1); // SA: this is weird, but I do not know a better way to do it (same applies to all cones below)
  else
    ModelSupport::buildPlane(SMap,flightIndex+5,Origin-Z*(height/2.0),zDircA);

  if (anglesZ[1]>Geometry::zeroTol)
    ModelSupport::buildCone(SMap,flightIndex+6,Origin+Z*(height/2.0),Z,90-anglesZ[1],Origin[2]>0 ? 1 : -1);
  else
    ModelSupport::buildPlane(SMap,flightIndex+6,Origin+Z*(height/2.0),zDircB);


  double layT(0.0);
  for(size_t i=0;i<nLayer;i++)
    {
      const int II(static_cast<int>(i));
      layT+=lThick[i];
	  
      ModelSupport::buildPlane(SMap,flightIndex+II*10+13,
			       Origin-X*(width/2.0)-xDircA*layT,xDircA);
      ModelSupport::buildPlane(SMap,flightIndex+II*10+14,
			       Origin+X*(width/2.0)+xDircB*layT,xDircB);

      if (anglesZ[0]>Geometry::zeroTol)
	ModelSupport::buildCone(SMap,flightIndex+II*10+15,Origin-Z*(height/2.0+layT),Z,90-anglesZ[0],Origin[2]>0 ? -1 : 1);
      else
	ModelSupport::buildPlane(SMap,flightIndex+II*10+15, Origin-Z*(height/2.0)-zDircA*layT,zDircA);

      if (anglesZ[1]>Geometry::zeroTol)
	ModelSupport::buildCone(SMap,flightIndex+II*10+16,Origin+Z*(height/2.0+layT),Z,90-anglesZ[1],Origin[2]>0 ?  1 : -1);
      else
	ModelSupport::buildPlane(SMap,flightIndex+II*10+16,Origin+Z*(height/2.0)+zDircB*layT,zDircB);

    }

  // CREATE LINKS
  int signVal(-1);
  for(size_t i=3;i<7;i++)
    {
      const int sNum(flightIndex+static_cast<int>(10*nLayer+i));
      FixedComp::setLinkSurf(i-1,signVal*SMap.realSurf(sNum));
      const int tNum(flightIndex+static_cast<int>(i));
      FixedComp::setLinkSurf(i+5,signVal*SMap.realSurf(tNum));
      signVal*=-1;
    } 


  FixedComp::setConnect(2,Origin-X*(width/2.0)-xDircA*layT,-xDircA);
  FixedComp::setConnect(3,Origin+X*(width/2.0)+xDircB*layT,xDircB);
  FixedComp::setConnect(4,Origin-Z*(height/2.0)-zDircA*layT,-zDircA);
  FixedComp::setConnect(5,Origin+Z*(height/2.0)+zDircB*layT,zDircB);

  FixedComp::setConnect(6,Origin,-Y);
  FixedComp::setConnect(7,Origin,Y);
  FixedComp::setConnect(8,Origin-X*(width/2.0),-xDircA);
  FixedComp::setConnect(9,Origin+X*(width/2.0),xDircB);
  FixedComp::setConnect(10,Origin-Z*(height/2.0),-zDircA);
  FixedComp::setConnect(11,Origin+Z*(height/2.0),zDircB);

  return;
}

void
TaperedFlightLine::createObjects(Simulation& System,
			       const attachSystem::FixedComp& innerFC,
			       const long int innerIndex,
			       const attachSystem::FixedComp& outerFC,
			       const long int outerIndex)
  /*!
    Creates the objects for the flightline
    \param System :: Simulation to create objects in
    \param innerFC :: Inner Object
    \param innerIndex :: Link point [if zero none]
    \param outerFC :: Outer Object
    \param outerIndex :: Link point [if zero none] 
  */
{
  ELog::RegMethod RegA("TaperedFlightLine","createObjects");
  


  const std::string innerCut=innerFC.getSignedLinkString(innerIndex);
  const std::string outerCut=outerFC.getSignedLinkString(outerIndex);

  const int layerIndex=flightIndex+static_cast<int>(nLayer)*10;  
  std::string Out;
  if (anglesZ[1]>Geometry::zeroTol)
    Out=ModelSupport::getComposite(SMap,layerIndex," 3 -4 5 6 ");
  else
    Out=ModelSupport::getComposite(SMap,layerIndex," 3 -4 5 -6 ");
  addOuterSurf("outer",Out);

  // Inner Void
  if (anglesZ[1]>Geometry::zeroTol)
    Out=ModelSupport::getComposite(SMap,flightIndex," 3 -4 5 6 ");
  else
    Out=ModelSupport::getComposite(SMap,flightIndex," 3 -4 5 -6 ");
  addOuterSurf("inner",Out);


  // Make inner object
  Out+=innerCut+outerCut;
  System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,0.0,Out));

  //Flight layers:
  for(size_t i=0;i<nLayer;i++)
    {
      const int II(static_cast<int>(i));
      if (anglesZ[1]>Geometry::zeroTol)
	Out=ModelSupport::getComposite(SMap,flightIndex+10*II,
				       "13 -14 15 16 (-3:4:-5:-6) ");
      else
	Out=ModelSupport::getComposite(SMap,flightIndex+10*II,
				       "13 -14 15 -16 (-3:4:-5:6) ");
      Out+=innerCut+outerCut;
      System.addCell(MonteCarlo::Qhull(cellIndex++,lMat[i],0.0,Out));
    }
  
  return;
}

void
TaperedFlightLine::createAll(Simulation& System,
			   const attachSystem::FixedComp& originFC,
			   const long int originIndex,
			   const attachSystem::FixedComp& innerFC,
			   const long int innerIndex,
			   const attachSystem::FixedComp& outerFC,
			   const long int outerIndex)
  /*!
    Global creation of the basic flight line connecting two
    objects
    \param System :: Simulation to add vessel to
    \param innerFC :: Moderator Object
    \param innerIndex :: Use side index from moderator
    \param outerFC :: Edge of bulk shield 
    \param outerIndex :: Use side index from moderator

  */
{
  ELog::RegMethod RegA("TaperedFlightLine","createAll");
  populate(System.getDataBase());
  createUnitVector(originFC,originIndex);
  createSurfaces();

  createObjects(System,innerFC,innerIndex,outerFC,outerIndex);
  insertObjects(System);       

  return;
}



  
}  // NAMESPACE moderatorSystem
