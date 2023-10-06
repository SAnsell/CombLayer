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
  activeFlag(0)
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
  activeFlag(0)
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
    // Void + Fe special:
  radius=Control.EvalDefVar<double>(keyName+"Radius",-1.0);
  length=Control.EvalVar<double>(keyName+"Length");

  feThick=Control.EvalVar<double>(keyName+"FeThick");

  const double fR=Control.EvalDefVar<double>(keyName+"FlangeRadius",-1.0);
  flangeARadius=Control.EvalDefVar<double>(keyName+"FlangeARadius",fR);
  flangeBRadius=Control.EvalDefVar<double>(keyName+"FlangeBRadius",fR);

  const double fL=Control.EvalDefVar<double>(keyName+"FlangeLength",-1.0);
  flangeALength=Control.EvalDefVar<double>(keyName+"FlangeALength",fL);
  flangeBLength=Control.EvalDefVar<double>(keyName+"FlangeBLength",fL);

  voidMat=ModelSupport::EvalDefMat(Control,keyName+"VoidMat",0);
  feMat=ModelSupport::EvalMat<int>(Control,keyName+"FeMat");
  flangeMat=ModelSupport::EvalDefMat(Control,keyName+"FlangeMat",feMat);

  outerVoid = Control.EvalDefVar<int>(keyName+"OuterVoid",0);
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
GeneralPipe::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("GeneralPipe","createSurfaces");

  createCommonSurfaces();
  
  // MAIN SURFACES:
  makeCylinder("InnerCyl",SMap,buildIndex+7,Origin,Y,radius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,radius+feThick);
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
    \param height :: Width of pipe (inner)
  */
{
  ELog::RegMethod RegA("VacuumPipe","createRectangularSurfaces");

  createCommonSurfaces();

  double H(height/2.0);
  double W(width/2.0);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*W,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*W,X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*H,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*H,Z);
  
  H+=feThick;
  W+=feThick;
  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*W,X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*W,X);
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*H,Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*H,Z);
    
  // FLANGE SURFACES FRONT:
  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,flangeARadius);
  // FLANGE SURFACES BACK:
  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,Y,flangeBRadius);

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
