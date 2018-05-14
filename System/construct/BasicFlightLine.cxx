/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/BasicFlightLine.cxx
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
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "BasicFlightLine.h"

namespace moderatorSystem
{

BasicFlightLine::BasicFlightLine(const std::string& Key)  :
  attachSystem::ContainedGroup("inner","outer"),
  attachSystem::FixedOffset(Key,12),
  flightIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(flightIndex+1),nLayer(0),tapFlag(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

BasicFlightLine::BasicFlightLine(const BasicFlightLine& A) : 
  attachSystem::ContainedGroup(A),attachSystem::FixedOffset(A),
  attachSystem::CellMap(A),
  flightIndex(A.flightIndex),cellIndex(A.cellIndex),
  height(A.height),width(A.width),
  innerMat(A.innerMat),nLayer(A.nLayer),lThick(A.lThick),
  lMat(A.lMat),tapFlag(A.tapFlag),attachRule(A.attachRule)
  /*!
    Copy constructor
    \param A :: BasicFlightLine to copy
  */
{
  anglesXY[0]=A.anglesXY[0];
  anglesXY[1]=A.anglesXY[1];
  anglesZ[0]=A.anglesZ[0];
  anglesZ[1]=A.anglesZ[1];

}

BasicFlightLine&
BasicFlightLine::operator=(const BasicFlightLine& A)
  /*!
    Assignment operator
    \param A :: BasicFlightLine to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedGroup::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      cellIndex=A.cellIndex;
      anglesXY[0]=A.anglesXY[0];
      anglesXY[1]=A.anglesXY[1];
      anglesZ[0]=A.anglesZ[0];
      anglesZ[1]=A.anglesZ[1];
      height=A.height;
      width=A.width;
      innerMat=A.innerMat;
      nLayer=A.nLayer;
      lThick=A.lThick;
      lMat=A.lMat;
      tapFlag=A.tapFlag;
      attachRule=A.attachRule;
    }
  return *this;
}

BasicFlightLine::~BasicFlightLine() 
 /*!
   Destructor
 */
{}

void
BasicFlightLine::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: Database to use
 */
{
  ELog::RegMethod RegA("BasicFlightLine","populate");

  FixedOffset::populate(Control);

  anglesXY[0]=Control.EvalVar<double>(keyName+"AngleXY1");
  anglesXY[1]=Control.EvalVar<double>(keyName+"AngleXY2");

  anglesZ[0]=Control.EvalVar<double>(keyName+"AngleZTop");
  anglesZ[1]=Control.EvalVar<double>(keyName+"AngleZBase");

  height=Control.EvalVar<double>(keyName+"Height");
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

  const std::string tapSurf=
    Control.EvalDefVar<std::string>(keyName+"TapSurf","plane");
  if (tapSurf!="plane" && tapSurf!="cone")
    throw ColErr::InvalidLine
      (tapSurf,keyName+":TapSurf is not 'plane' or 'cone'");

  tapFlag=(tapSurf=="cone" && anglesZ[0]>Geometry::zeroTol) ? 1 : 0;
  tapFlag|=(tapSurf=="cone" && anglesZ[1]>Geometry::zeroTol) ? 2 : 0;

  return;
}
  
void
BasicFlightLine::createUnitVector(const attachSystem::FixedComp& FC,
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
  ELog::RegMethod RegA("BasicFlightLine","createUnitVector");
  FixedComp::createUnitVector(FC,sideIndex);
  // maybe zero yStep?
  yStep=0.0;
  applyOffset();

  return;
}

void
BasicFlightLine::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("BasicFlightLine","createSurfaces");

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

  //  const int zFlag(1);
  if (tapFlag & 1)
    {
      ModelSupport::buildCone(SMap,flightIndex+5,
                              Origin-Z*(height/2.0),Z,90.0-anglesZ[0]);
      ModelSupport::buildPlane(SMap,flightIndex+505,Origin-Z*(height/2.0),Z);
    }
  else
    ModelSupport::buildPlane(SMap,flightIndex+5,Origin-Z*(height/2.0),zDircA);

  if (tapFlag & 2 )
    {
      ModelSupport::buildCone(SMap,flightIndex+6,
                              Origin+Z*(height/2.0),Z,90-anglesZ[1]);
      ModelSupport::buildPlane(SMap,flightIndex+506,Origin+Z*(height/2.0),Z);
    }
  else
    ModelSupport::buildPlane(SMap,flightIndex+6,Origin+Z*(height/2.0),-zDircB);
  
 
  double layT(0.0);
  for(size_t i=0;i<nLayer;i++)
    {
      const int II(static_cast<int>(i));
      layT+=lThick[i];
	  
      ModelSupport::buildPlane(SMap,flightIndex+II*10+13,
			       Origin-X*(width/2.0)-xDircA*layT,xDircA);
      ModelSupport::buildPlane(SMap,flightIndex+II*10+14,
			       Origin+X*(width/2.0)+xDircB*layT,xDircB);

      if (tapFlag & 1)
	{
	  ModelSupport::buildCone(SMap,flightIndex+II*10+15,
				  Origin-Z*(height/2.0+layT),Z,90.0-anglesZ[0]);
          ModelSupport::buildPlane(SMap,flightIndex+II*10+515,Origin-Z*(height/2.0+layT),Z);
         }
      else
        ModelSupport::buildPlane(SMap,flightIndex+II*10+15,
                                 Origin-Z*(height/2.0)-zDircA*layT,zDircA);

      if (tapFlag & 2)
        {
          ModelSupport::buildCone(SMap,flightIndex+II*10+16,
                                  Origin+Z*(height/2.0+layT),Z,90.0-anglesZ[1]);
          ModelSupport::buildPlane(SMap,flightIndex+II*10+516,Origin+Z*(height/2.0+layT),Z);
        }
      else
        ModelSupport::buildPlane(SMap,flightIndex+II*10+16,
                                 Origin+Z*(height/2.0)+zDircB*layT,-zDircB);
    }

  // CREATE LINKS

  const int sNum(flightIndex+static_cast<int>(10*nLayer));

  FixedComp::setLinkSurf(2,-SMap.realSurf(sNum+3));
  FixedComp::setLinkSurf(3,SMap.realSurf(sNum+4));
  FixedComp::setLinkSurf(4,-SMap.realSurf(sNum+5));
  FixedComp::setLinkSurf(5,-SMap.realSurf(sNum+6));

  FixedComp::setConnect(2,Origin-X*(width/2.0)-xDircA*layT,-xDircA);
  FixedComp::setConnect(3,Origin+X*(width/2.0)+xDircB*layT,xDircB);
  FixedComp::setConnect(4,Origin-Z*(height/2.0)-zDircA*layT,-zDircA);
  FixedComp::setConnect(5,Origin+Z*(height/2.0)+zDircB*layT,-zDircB);
  FixedComp::setConnect(6,Origin,-Y);
  FixedComp::setConnect(7,Origin,Y);

  // Inner surfaces:
  FixedComp::setConnect(8,Origin-X*(width/2.0),-xDircA);
  FixedComp::setConnect(9,Origin+X*(width/2.0),xDircB);
  FixedComp::setConnect(10,Origin-Z*(height/2.0),-zDircA);
  FixedComp::setConnect(11,Origin+Z*(height/2.0),-zDircB);

  FixedComp::setLinkSurf(8,-SMap.realSurf(flightIndex+3));
  FixedComp::setLinkSurf(9,SMap.realSurf(flightIndex+4));
  FixedComp::setLinkSurf(10,-SMap.realSurf(flightIndex+5));
  FixedComp::setLinkSurf(11,-SMap.realSurf(flightIndex+6));
  if (tapFlag & 1)
    {
      FixedComp::addLinkSurf(4,-SMap.realSurf(flightIndex+505));
      FixedComp::addLinkSurf(10,-SMap.realSurf(flightIndex+505));
    }
  if (tapFlag & 2)
    {
      FixedComp::addLinkSurf(5,SMap.realSurf(flightIndex+506));
      FixedComp::addLinkSurf(11,SMap.realSurf(flightIndex+506));
    }
  
  return;
}

void
BasicFlightLine::createObjects(Simulation& System,
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
  ELog::RegMethod RegA("BasicFlightLine","createObjects");

    
  const std::string innerCut=innerFC.getLinkString(innerIndex);
  const std::string outerCut=outerFC.getLinkString(outerIndex);
  
  setLinkSignedCopy(0,innerFC,innerIndex);
  setLinkSignedCopy(1,outerFC,outerIndex);
  
  const int layerIndex=flightIndex+static_cast<int>(nLayer)*10;  
  std::string Out;
  Out=ModelSupport::getSetComposite(SMap,layerIndex," 3 -4 (5:505) (6:-506) ");
  addOuterSurf("outer",Out);

  // Inner Void :
  Out=ModelSupport::getSetComposite(SMap,flightIndex," 3 -4 (5:505) (6:-506) ");  
  addOuterSurf("inner",Out);
  Out+=innerCut+outerCut; 

  System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,0.0,Out));
  CellMap::addCell("innerVoid",cellIndex-1);

  //Flight layers:

  for(size_t i=0;i<nLayer;i++)
    {
      const int II(static_cast<int>(i));
      HeadRule Exclude(ModelSupport::getSetComposite(SMap,flightIndex+10*II," 3 -4 (5:505) (6:-506) "));
      Exclude.makeComplement();
      Out=ModelSupport::getSetComposite(SMap,flightIndex+10*II," 13 -14 (15:505) (16:-506) ");
      Out+=innerCut+outerCut+Exclude.display();
      System.addCell(MonteCarlo::Qhull(cellIndex++,lMat[i],0.0,Out));
      CellMap::addCell("Layer"+StrFunc::makeString(i+1),cellIndex-1);
    }      
  
  return;
}

  
void
BasicFlightLine::createAll(Simulation& System,
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
  ELog::RegMethod RegA("BasicFlightLine","createAll");
  BasicFlightLine::populate(System.getDataBase());

  createUnitVector(originFC,originIndex);
  createSurfaces();

  createObjects(System,innerFC,innerIndex,outerFC,outerIndex);
  insertObjects(System);       

  return;
}



  
}  // NAMESPACE moderatorSystem
