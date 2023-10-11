/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   construct/GeneralPipe.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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

#include "Exception.h"
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
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "SurfMap.h"

#include "GeneralPipe.h"
#include "GeneralPipe.h"

namespace constructSystem
{

GeneralPipe::GeneralPipe(const std::string& Key) :
  attachSystem::FixedRotate(Key,12),
  attachSystem::ContainedGroup("Main","FlangeA","FlangeB"),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::FrontBackCut(),
  windowType(0),activeFlag(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  FixedComp::nameSideIndex(0,"front");
  FixedComp::nameSideIndex(1,"back");
  FixedComp::nameSideIndex(6,"midPoint");
}

GeneralPipe::GeneralPipe(const std::string& Key,
			   const size_t nLink) :
  attachSystem::FixedRotate(Key,nLink),
  attachSystem::ContainedGroup("Main","FlangeA","FlangeB"),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::FrontBackCut(),
  windowType(0),activeFlag(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  FixedComp::nameSideIndex(0,"front");
  FixedComp::nameSideIndex(1,"back");
  FixedComp::nameSideIndex(6,"midPoint");
}

GeneralPipe::GeneralPipe(const GeneralPipe& A) : 
  attachSystem::FixedRotate(A),
  attachSystem::ContainedGroup(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),
  radius(A.radius),length(A.length),pipeThick(A.pipeThick),
  flangeARadius(A.flangeARadius),flangeALength(A.flangeALength),
  flangeBRadius(A.flangeBRadius),flangeBLength(A.flangeBLength),
  windowType(A.windowType),windowA(A.windowA),
  windowB(A.windowB),voidMat(A.voidMat),pipeMat(A.pipeMat),
  flangeMat(A.flangeMat),outerVoid(A.outerVoid),
  activeFlag(A.activeFlag)
  /*!
    Copy constructor
    \param A :: GeneralPipe to copy
  */
{}

GeneralPipe&
GeneralPipe::operator=(const GeneralPipe& A)
  /*!
    Assignment operator
    \param A :: GeneralPipe to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedGroup::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      attachSystem::FrontBackCut::operator=(A);
      radius=A.radius;
      length=A.length;
      pipeThick=A.pipeThick;
      flangeARadius=A.flangeARadius;
      flangeALength=A.flangeALength;
      flangeBRadius=A.flangeBRadius;
      flangeBLength=A.flangeBLength;
      windowType=A.windowType;
      windowA=A.windowA;
      windowB=A.windowB;
      voidMat=A.voidMat;
      pipeMat=A.pipeMat;
      flangeMat=A.flangeMat;
      outerVoid=A.outerVoid;
      activeFlag=A.activeFlag;
    }
  return *this;
}

void
GeneralPipe::applyActiveFrontBack(const double length)
  /*!
    Apply the active front/back point to re-calcuate Origin
    It applies the rotation of Y to Y' to both X/Z to preserve
    orthogonality.
    \param length :: Full length [can be removed?]
   */
{
  ELog::RegMethod RegA("GeneralPipe","applyActiveFrontBack");
  const Geometry::Vec3D curFP=((activeFlag & 1) && frontPointActive()) ?
    getFrontPoint() : Origin;
  const Geometry::Vec3D curBP=((activeFlag & 2) && backPointActive()) ?
    getBackPoint() : Origin+Y*length;
  
  Origin=(curFP+curBP)/2.0;

  if (activeFlag)
    {
      const Geometry::Vec3D YAxis=(curBP-curFP).unit();
      // need unit for numerical acc.
      Geometry::Vec3D RotAxis=(YAxis*Y).unit();
      
      if (!RotAxis.nullVector())
	{
	  const Geometry::Quaternion QR=
	    Geometry::Quaternion::calcQVRot(Y,YAxis,RotAxis);
	  Y=YAxis;
	  QR.rotate(X);
	  QR.rotate(Z);
	}
      else if (Y.dotProd(YAxis) < -0.5) // (reversed
	{
	  Y=YAxis;
	  X*=-1.0;
	  Z*=-1.0;
	}
    }
  return;
}

void
GeneralPipe::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("GeneralPipe","populate");
  
  FixedRotate::populate(Control);
  
  radius=Control.EvalDefVar<double>(keyName+"Radius",-1.0);
  length=Control.EvalVar<double>(keyName+"Length");

  pipeThick=Control.EvalVar<double>(keyName+"PipeThick");

  const double fR=Control.EvalDefVar<double>(keyName+"FlangeRadius",-1.0);
  flangeARadius=Control.EvalDefVar<double>(keyName+"FlangeARadius",fR);
  flangeBRadius=Control.EvalDefVar<double>(keyName+"FlangeBRadius",fR);

  const double fL=Control.EvalDefVar<double>(keyName+"FlangeLength",-1.0);
  flangeALength=Control.EvalDefVar<double>(keyName+"FlangeALength",fL);
  flangeBLength=Control.EvalDefVar<double>(keyName+"FlangeBLength",fL);

  voidMat=ModelSupport::EvalDefMat(Control,keyName+"VoidMat",0);
  pipeMat=ModelSupport::EvalMat<int>(Control,keyName+"PipeMat");
  flangeMat=ModelSupport::EvalDefMat(Control,keyName+"FlangeMat",pipeMat);

  outerVoid = Control.EvalDefVar<int>(keyName+"OuterVoid",0);

  // if flanges are good:
  populateWindows(Control);
    
  return;
}

void
GeneralPipe::populateWindows(const FuncDataBase& Control)
  /*!
    Gets the window data if the variables are set
    \param Control :: Database
  */
{
  ELog::RegMethod RegA("GeneralPipe","populateWindows");

  windowType=Control.EvalDefVar<int>(keyName+"WindowType",0);

  if ((windowType & 5) == 5 || (windowType & 10) ==10)
    throw ColErr::InContainerError<int>
      (windowType,
       "Window active requests both rectangular and circular window");
  
  if (windowType & 5)  // front windows 
    {
      windowA.thick=Control.EvalPair<double>
	(keyName+"WindowAThick",keyName+"WindowThick");
      if (windowType & 1)  //radial window
	windowA.radius=Control.EvalPair<double>
	  (keyName+"WindowARadius",keyName+"WindowRadius");
      else
	{
	  windowA.height=Control.EvalPair<double>
	    (keyName+"WindowAHeight",keyName+"WindowHeight");
	  windowA.width=Control.EvalPair<double>
	    (keyName+"WindowAWidth",keyName+"WindowWidth");
	}
      windowA.mat=ModelSupport::EvalMat<int>
	(Control,keyName+"WindowAMat",keyName+"WindowMat");
    }
  if (windowType & 10)  // back windows  (2 | 8)
    {
      windowB.thick=Control.EvalPair<double>
	(keyName+"WindowBThick",keyName+"WindowThick");
      if (windowType & 1)  //radial window
	windowB.radius=Control.EvalPair<double>
	  (keyName+"WindowBRadius",keyName+"WindowRadius");
      else
	{
	  windowB.height=Control.EvalPair<double>
	    (keyName+"WindowBHeight",keyName+"WindowHeight");
	  windowB.width=Control.EvalPair<double>
	    (keyName+"WindowBWidth",keyName+"WindowWidth");
	}
      windowB.mat=ModelSupport::EvalMat<int>
	(Control,keyName+"WindowBMat",keyName+"WindowMat");
    }

  if ( ((windowType & 1) && windowA.radius<Geometry::zeroTol) ||
       ((windowType & 4) &&
	(windowA.width<Geometry::zeroTol ||
	 windowA.height<Geometry::zeroTol)))
    {
      throw ColErr::EmptyContainer("Pipe:["+keyName+"] has neither "
				   "windowA:Radius or Height/Width");
    }
  if ( ((windowType & 2) && windowB.radius<Geometry::zeroTol) ||
       ((windowType & 8) &&
	(windowB.width<Geometry::zeroTol ||
	 windowB.height<Geometry::zeroTol)))
    {
      throw ColErr::EmptyContainer("Pipe:["+keyName+"] has neither "
				   "windowB:Radius or Height/Width");
    }
  return;
}

void
GeneralPipe::createUnitVector(const attachSystem::FixedComp& FC,
                             const long int sideIndex)
/*!
    Create the unit vectors
    Note that the active:front/back is applied afterwards.
    This corrects the mid point and Y based on front/back surfaces.
    
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("GeneralPipe","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  // after rotation
  applyActiveFrontBack(length);
  return;
}


void
GeneralPipe::createCommonSurfaces()
  /*!
    Constructor of common surfaces for both rectangular and cyclindrical
    pipes.
  */
{
  ELog::RegMethod RegA("GeneralPipe","createCommonSurfaces");
  
  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  if (!isActive("back"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);
      ExternalCut::setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }
  
  // Front Inner void
  FrontBackCut::getShiftedFront(SMap,buildIndex+101,Y,flangeALength);
  FrontBackCut::getShiftedBack(SMap,buildIndex+102,Y,-flangeBLength);
  return;
}

void
GeneralPipe::createWindowSurfaces()
{
  ELog::RegMethod RegA("GeneralPipe","createWindowSurfaces");

  if (!windowType) return;
  
  if (windowType & 5)
    {
      getShiftedFront(SMap,buildIndex+1001,Y,
		      (flangeALength-windowA.thick)/2.0);
      getShiftedFront(SMap,buildIndex+1002,Y,
		      (flangeALength+windowA.thick)/2.0);
      // add data to surface
      addSurf("FrontWindow",SMap.realSurf(buildIndex+1001));
      addSurf("FrontWindow",SMap.realSurf(buildIndex+1002));
    }

  // Back Inner void
  if (windowType & 10)
    {
      getShiftedBack(SMap,buildIndex+1101,Y,
		     -(flangeBLength-windowB.thick)/2.0);
      getShiftedBack(SMap,buildIndex+1102,Y,
		     -(flangeBLength+windowB.thick)/2.0);
      // add data to surface
      addSurf("BackWindow",buildIndex+1101);
      addSurf("BackWindow",buildIndex+1102);
    }

  // FRONT WINDOW SURFACES:
  if (windowType & 1)
    {
      ModelSupport::buildCylinder(SMap,buildIndex+1007,Origin,Y,
				  windowA.radius);
    }
  else if (windowType & 4) 
    {
      ModelSupport::buildPlane(SMap,buildIndex+1003,
			       Origin-X*(windowA.width/2.0),X);
      ModelSupport::buildPlane(SMap,buildIndex+1004,
			       Origin+X*(windowA.width/2.0),X);
      ModelSupport::buildPlane(SMap,buildIndex+1005,
			       Origin-Z*(windowA.height/2.0),Z);
      ModelSupport::buildPlane(SMap,buildIndex+1006,
			       Origin+Z*(windowA.height/2.0),Z);
    }
  // BACK WINDOW SURFACES:
  if (windowType & 2)
    {
      ModelSupport::buildCylinder(SMap,buildIndex+1107,Origin,Y,
				  windowB.radius);
    }
  else if (windowType & 8)
    {
      ModelSupport::buildPlane(SMap,buildIndex+1103,
			       Origin-X*(windowB.width/2.0),X);
      ModelSupport::buildPlane(SMap,buildIndex+1104,
			       Origin+X*(windowB.width/2.0),X);
      ModelSupport::buildPlane(SMap,buildIndex+1105,
			       Origin-Z*(windowB.height/2.0),Z);
      ModelSupport::buildPlane(SMap,buildIndex+1106,
			       Origin+Z*(windowB.height/2.0),Z);
    }
  
  return;
}

void
GeneralPipe::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("GeneralPipe","createSurfaces");

  createCommonSurfaces();
  createWindowSurfaces();
  
  // MAIN SURFACES:
  makeCylinder("InnerCyl",SMap,buildIndex+7,Origin,Y,radius);
  makeCylinder("PipeCyl",SMap,buildIndex+17,Origin,Y,radius+pipeThick);
  addSurf("OuterRadius",SMap.realSurf(buildIndex+17));

  // FLANGE SURFACES :
  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,flangeARadius);
  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,Y,flangeBRadius);

  return;
}

void
GeneralPipe::createRectangleSurfaces(const double width,
				     const double height)
  /*!
    Create the surfaces for a rectanglular system
    \param width :: Width of pipe (inner)
    \param height :: Heigh of pipe (inner)
  */
{
  ELog::RegMethod RegA("VacuumPipe","createRectangularSurfaces");

  createCommonSurfaces();
  createWindowSurfaces();

  double H(height/2.0);
  double W(width/2.0);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*W,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*W,X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*H,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*H,Z);
  
  H+=pipeThick;
  W+=pipeThick;
  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*W,X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*W,X);
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*H,Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*H,Z);
    
  // FLANGE SURFACES FRONT:
  ELog::EM<<"Flange Size == "<<keyName<<" "<<flangeARadius<<ELog::endDiag;
  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,flangeARadius);
  // FLANGE SURFACES BACK:
  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,Y,flangeBRadius);

  return;
}

void
GeneralPipe::createFlange(Simulation& System,
			  const HeadRule& outerHR)
  /*!
    Create window in flange (and flange)
    Requires that front/back have been set in ExternalCut
    \param System :: simulation
    \param outerHR :: Pipe void inner
   */
{
  HeadRule HR;
  const HeadRule& frontHR=getRule("front");
  const HeadRule& backHR=getRule("back");

  if (flangeARadius>Geometry::zeroTol)
    {
      HeadRule windowAExclude;
      if (windowType & 5)      // FRONT
	{
	  HR=ModelSupport::getSetHeadRule
	    (SMap,buildIndex,"-1007 1003 -1004 1005 -1006 1001 -1002");
	  makeCell("Window",System,cellIndex++,windowA.mat,0.0,
		   HR*getDivider("front"));
	  ExternalCut::setCutSurf("AWindow",HR);
	  windowAExclude=HR.complement();
	}

      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-101 -107");
      makeCell("FlangeA",System,cellIndex++,flangeMat,0.0,
	       HR*frontHR*windowAExclude*outerHR);
    }
  if (flangeBRadius>Geometry::zeroTol)
    {
      HeadRule windowBExclude;
      if (windowType & 10)
	{
	  HR=ModelSupport::getSetHeadRule
	    (SMap,buildIndex,"-1107 1103 -1104 1105 -1106 1102 -1101");
	  
	  makeCell("Window",System,cellIndex++,windowB.mat,0.0,
		   HR*getDivider("back"));
	  ExternalCut::setCutSurf("BWindow",HR);
	  windowBExclude=HR.complement();
	}
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"102 -207");
      makeCell("FlangeB",System,cellIndex++,flangeMat,0.0,
	       HR*backHR*windowBExclude*outerHR);
    }

  
  
  return;
}

void
GeneralPipe::createOuterVoid(Simulation& System,
			     const HeadRule& outerHR)
  /*!
    Construct the outer void of the pipe assuming normal pipe
    with round flanges (and will update for HeadRule innerWall)
    \param System :: Simulation
  */
{
  ELog::RegMethod RegA("GeneralPipe","createOuterVoid");
  
  const HeadRule& frontHR=getRule("front");
  const HeadRule& backHR=getRule("back");
  HeadRule HR;

  if (outerVoid)
    {
      if (std::abs(flangeARadius-flangeBRadius)<Geometry::zeroTol)
	{
	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -102 -107");
	  makeCell("outerVoid",System,cellIndex++,0,0.0,HR*outerHR);
	  HR=HeadRule(SMap,buildIndex,-107);
	  addOuterSurf("Main",HR*frontHR*backHR);
	}
      else if (flangeARadius>flangeBRadius)
	{
	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -102 -107");
	  makeCell("outerVoid",System,cellIndex++,0,0.0,HR*outerHR);
	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"102 207 -107");
	  makeCell("outerVoid",System,cellIndex++,0,0.0,HR*backHR);
	  HR=HeadRule(SMap,buildIndex,-107);
	  addOuterSurf("Main",HR*frontHR*backHR);
	}
      else if (flangeBRadius>flangeARadius)
	{
	  HR=ModelSupport::getHeadRule(SMap,buildIndex," 101 -102 -207 ");
	  makeCell("outerVoid",System,cellIndex++,0,0.0,HR*outerHR);
	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-101 107 -207");
	  makeCell("outerVoid",System,cellIndex++,0,0.0,HR*frontHR);
	  HR=HeadRule(SMap,buildIndex,-207);
	  addOuterSurf("Main",HR*frontHR*backHR);
	}
    }
  else
    {
      // outer boundary [flange front]
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-101 -107");
      addOuterSurf("FlangeA",HR*frontHR);

      // outer boundary [flange back]
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"102 -207");
      addOuterSurf("FlangeB",HR*backHR);

      // outer boundary mid tube
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -102");
      addOuterSurf("Main",HR*outerHR.complement());
    }
  
  return;
}

void
GeneralPipe::setJoinFront(const attachSystem::FixedComp& FC,
			  const long int sideIndex)
  /*!
    Set front and allow half merge for angle and position
    \param FC :: FixedComponent
    \param sideIndex :: Link point
   */
{
  FrontBackCut::setFront(FC,sideIndex);
  activeFlag |= 1;
  return;
}

void
GeneralPipe::setJoinBack(const attachSystem::FixedComp& FC,
			  const long int sideIndex)
  /*!
    Set back and allow half merge for angle and position
    \param FC :: FixedComponent
    \param sideIndex :: Link point
   */
{
  FrontBackCut::setBack(FC,sideIndex);
  activeFlag |= 2;
  return;
}
  
  
}  // NAMESPACE constructSystem
