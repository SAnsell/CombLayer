/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   moderator/Reflector.cxx
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "CoolPad.h"
#include "Reflector.h"


namespace moderatorSystem
{

Reflector::Reflector(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedRotate(Key,24),
  attachSystem::SurfMap(),
  attachSystem::CellMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{
  FixedComp::nameSideIndex(10,"CornerCentre");
}

Reflector::Reflector(const Reflector& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedRotate(A),
  attachSystem::SurfMap(A),
  attachSystem::CellMap(A),
  xySize(A.xySize),zSize(A.zSize),cutSize(A.cutSize),
  defMat(A.defMat),
  Pads(A.Pads)
  /*!
    Copy constructor
    \param A :: Reflector to copy
  */
{}

Reflector&
Reflector::operator=(const Reflector& A)
  /*!
    Assignment operator
    \param A :: Reflector to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      xySize=A.xySize;
      zSize=A.zSize;
      cutSize=A.cutSize;
      defMat=A.defMat;

      Pads=A.Pads;
    }
  return *this;
}

Reflector::~Reflector() 
  /*!
    Destructor
  */
{}

void
Reflector::populate(const FuncDataBase& Control)
/*!
  Populate all the variables
  \param Control :: Data base for variables
*/
{
  ELog::RegMethod RegA("Reflector","populate");
  
  FixedRotate::populate(Control);
  
  xySize=Control.EvalVar<double>(keyName+"XYSize");
  zSize=Control.EvalVar<double>(keyName+"ZSize");
  cutSize=Control.EvalVar<double>(keyName+"CutSize");
  
  defMat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  const size_t nPads=Control.EvalVar<size_t>(keyName+"NPads");
  for(size_t i=0;i<nPads;i++)
    Pads.push_back(CoolPad("coolPad",i+1));

  flightLine.resize(4);
  for(size_t i=0;i<flightLine.size();i++)
    {
      const std::string flName=keyName+"FL"+std::to_string(i);
      flightLine[i].Org=
	Control.EvalVar<Geometry::Vec3D>(flName+"Org");
      flightLine[i].Axis=
	Control.EvalVar<Geometry::Vec3D>(flName+"Axis");
      flightLine[i].width=Control.EvalVar<double>(flName+"Width");
      flightLine[i].height=Control.EvalVar<double>(flName+"Height");
      flightLine[i].negAngle=Control.EvalVar<double>(flName+"NegAngle");
      flightLine[i].plusAngle=Control.EvalVar<double>(flName+"PlusAngle");
      flightLine[i].upAngle=Control.EvalVar<double>(flName+"UpAngle");
      flightLine[i].downAngle=Control.EvalVar<double>(flName+"DownAngle");
    }
  
  return;
}

void
Reflector::createFlightLineSurfaces()
  /*!
    Create the flight line surfaces
  */
{
  ELog::RegMethod RegA("Reflector","createFlightLineSurfaces");

  // NOTE: the reflector sides are built at 45 deg. For conveniance
  // the Axis of the FL is re-rotated back by 45 deg. So if you jmake
  // a small adjustment to the angle that is shown in the flight line

  int FIndex(buildIndex+1000);
  size_t linkIndex(14);
  // link point names
  const std::vector<std::string> fName({
      "FLgroove","FLhydro","FLnarrow","FLwish"});
  std::vector<std::string>::const_iterator vc=fName.begin();
  for(const flightInfo& FL  : flightLine)
    {
      const Geometry::Vec3D Org=Origin+FL.Org.getInBasis(X,Y,Z);
      const Geometry::Vec3D Axis=FL.Axis.getInBasis(X,Y,Z).unit();
      const Geometry::Vec3D AxisX=Z*Axis;

      const Geometry::Quaternion Qneg=
	Geometry::Quaternion::calcQRotDeg(-FL.negAngle,Z);
      const Geometry::Quaternion Qplus=
	Geometry::Quaternion::calcQRotDeg(FL.plusAngle,Z);
      const Geometry::Quaternion Qdown=
	Geometry::Quaternion::calcQRotDeg(FL.downAngle,AxisX);
      const Geometry::Quaternion Qup=
	Geometry::Quaternion::calcQRotDeg(-FL.upAngle,AxisX);
      
      makePlane("FLcut",SMap,FIndex+1,Org-Axis*2.0,Axis);
      const Geometry::Vec3D normNeg=Qneg.makeRotate(AxisX);
      makePlane("FLneg",SMap,FIndex+3,Org-AxisX*(FL.width/2.0),normNeg);
      const Geometry::Vec3D normPlus=Qplus.makeRotate(AxisX);
      makePlane("FLplus",SMap,FIndex+4,Org+AxisX*(FL.width/2.0),normPlus);

      const Geometry::Vec3D normDown=Qdown.makeRotate(Z);
      makePlane("FLdown",SMap,FIndex+5,Org-Z*(FL.height/2.0),normDown);
      const Geometry::Vec3D normUp=Qup.makeRotate(Z);
      makePlane("FLup",SMap,FIndex+6,Org+Z*(FL.height/2.0),normUp);

      // create links:
      const HeadRule FLHR=ModelSupport::getHeadRule(SMap,FIndex,"3 -4 5 -6");
      FixedComp::setLinkSurf(linkIndex,FLHR);
      FixedComp::setConnect(linkIndex,Org,Axis);
      FixedComp::nameSideIndex(linkIndex,*vc);
      linkIndex++;

      // individual links for the HWrapper:
      if (*vc=="FLhydro")
	{
	  FixedComp::setLinkSurf(10,-SMap.realSurf(FIndex+3));
	  FixedComp::setLinkSurf(11,SMap.realSurf(FIndex+4));
	  FixedComp::setLinkSurf(12,-SMap.realSurf(FIndex+5));
	  FixedComp::setLinkSurf(13,SMap.realSurf(FIndex+6));
	  FixedComp::setConnect(10,Org-AxisX*(FL.width/2.0),-normNeg);
	  FixedComp::setConnect(11,Org+AxisX*(FL.width/2.0),normPlus);
	  FixedComp::setConnect(12,Org-Z*(FL.height/2.0),-normDown);
	  FixedComp::setConnect(13,Org+Z*(FL.height/2.0),normUp);

	  FixedComp::nameSideIndex(10,"FLhydroNeg");
	  FixedComp::nameSideIndex(11,"FLhydroPlus");
	  FixedComp::nameSideIndex(12,"FLhydroDown");
	  FixedComp::nameSideIndex(13,"FLhydroUp");
	}
      vc++;
      FIndex+=100;
    }
  return;
}

void
Reflector::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("Reflector","createSurface");

  const Geometry::Quaternion Qref=
    Geometry::Quaternion::calcQRotDeg(-45.0,Z);
  const Geometry::Vec3D XX=Qref.makeRotate(X);
  const Geometry::Vec3D YY=Qref.makeRotate(Y);

  // rotation of axis:
  const Geometry::Quaternion Qxy=
    Geometry::Quaternion::calcQRotDeg(45,Z);
  Geometry::Vec3D XR(XX);
  Geometry::Vec3D YR(YY);
  Qxy.rotate(XR);
  Qxy.rotate(YR);
  
  // Simple box planes

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-YY*xySize,YY);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+YY*xySize,YY);
  SurfMap::addSurf("Front",SMap.realSurf(buildIndex+1));
  SurfMap::addSurf("Back",-SMap.realSurf(buildIndex+2));
  
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-XX*xySize,XX);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+XX*xySize,XX);
  SurfMap::addSurf("Left",SMap.realSurf(buildIndex+3));
  SurfMap::addSurf("Right",-SMap.realSurf(buildIndex+4));

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*zSize,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*zSize,Z);
  SurfMap::addSurf("Base",SMap.realSurf(buildIndex+5));
  SurfMap::addSurf("Top",-SMap.realSurf(buildIndex+6));
 
  // Corner cuts:
  ModelSupport::buildPlane(SMap,buildIndex+11,Origin-Y*cutSize,Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*cutSize,Y);
  SurfMap::addSurf("CornerA",SMap.realSurf(buildIndex+11));
  SurfMap::addSurf("CornerB",-SMap.realSurf(buildIndex+12));
    
  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*cutSize,X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*cutSize,X);
  SurfMap::addSurf("CornerC",SMap.realSurf(buildIndex+13));
  SurfMap::addSurf("CornerD",-SMap.realSurf(buildIndex+14));
  createLinks(XX,YY);

  return;
}

void
Reflector::createLinks(const Geometry::Vec3D& XX,
		       const Geometry::Vec3D& YY)
  /*!
    Build the links to the primary surfaces
    \param XR :: Side rotation direction
    \param YR :: Side rotation direction
  */
{
  ELog::RegMethod RegA("Reflector","createLinks");

  FixedComp::setConnect(0,Origin-YY*xySize,-YY);  // chipIR OPPOSITE
  FixedComp::setConnect(1,Origin+YY*xySize,YY);   // chipIR
  FixedComp::setConnect(2,Origin-XX*xySize,-XX);
  FixedComp::setConnect(3,Origin+XX*xySize,XX);
  FixedComp::setConnect(4,Origin-Z*zSize,-Z);
  FixedComp::setConnect(5,Origin+Z*zSize,Z);

  FixedComp::setConnect(6,Origin-Y*cutSize,-Y);
  FixedComp::setConnect(7,Origin+Y*cutSize,Y);
  FixedComp::setConnect(8,Origin-X*cutSize,-X);
  FixedComp::setConnect(9,Origin+X*cutSize,-X);
  FixedComp::setConnect(10,Origin,Y);   // corner centre

  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));
  FixedComp::setLinkSurf(6,-SMap.realSurf(buildIndex+11));
  FixedComp::setLinkSurf(7,SMap.realSurf(buildIndex+12));
  FixedComp::setLinkSurf(8,-SMap.realSurf(buildIndex+13));
  FixedComp::setLinkSurf(9,SMap.realSurf(buildIndex+14));
     
  return;
}

void
Reflector::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("Reflector","createObjects");

  HeadRule HR;
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 3 -4 5 -6 11 -12 13 -14");
  addOuterSurf(HR);
  
  int FIndex(buildIndex+1000);
  HeadRule FLhr;

  FLhr=ModelSupport::getHeadRule
    (SMap,buildIndex,FIndex,"1 11 1M 3M -4M 5M -6M");
  makeCell("FLGroove",System,cellIndex++,0,0.0,FLhr);
  FLhr=ModelSupport::getHeadRule(SMap,FIndex,"1 3 -4 5 -6");
  HR.addIntersection(FLhr.complement());

  FIndex+=100;
  FLhr=ModelSupport::getHeadRule
    (SMap,buildIndex,FIndex,"-2 -14 -4 1M 3M -4M 5M -6M");
  makeCell("FLHydro",System,cellIndex++,0,0.0,FLhr);
  FLhr=ModelSupport::getHeadRule(SMap,FIndex,"1 3 -4 5 -6");
  HR.addIntersection(FLhr.complement());

  FIndex+=100;
  FLhr=ModelSupport::getHeadRule
    (SMap,buildIndex,FIndex,"13 3 1M 3M -4M 5M -6M");
  makeCell("FLNarrow",System,cellIndex++,0,0.0,FLhr);
  FLhr=ModelSupport::getHeadRule(SMap,FIndex,"1 3 -4 5 -6");
  HR.addIntersection(FLhr.complement());

  FIndex+=100;
  FLhr=ModelSupport::getHeadRule
    (SMap,buildIndex,FIndex,"-4 1M 3M -4M 5M -6M");
  makeCell("FLWish",System,cellIndex++,0,0.0,FLhr);
  FLhr=ModelSupport::getHeadRule(SMap,FIndex,"1 3 -4 5 -6");
  HR.addIntersection(FLhr.complement());

  
  makeCell("Reflector",System,cellIndex++,defMat,0.0,HR);
  for(const auto& A : this->getInsertCells())
    ELog::EM<<"InsertCell s== "<<A<<ELog::endDiag;;

  for(CoolPad& PD : Pads)
    {
      PD.addInsertCell(this->getInsertCells());      
      PD.setCutSurf("HotSurf",getFullRule(3));
      PD.createAll(System,*this,3);
    }
      
  return;
}

void
Reflector::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
  */
{
  ELog::RegMethod RegA("Reflector","createAll");

  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createFlightLineSurfaces();
  createObjects(System);
	
  insertObjects(System);              

  return;
}

}  // NAMESPACE shutterSystem
