/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   construct/GeneralPipe.cxx
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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
  radius(A.radius),length(A.length),
  pipeThick(A.pipeThick),
  flangeA(A.flangeA),flangeB(A.flangeB),
  windowA(A.windowA),windowB(A.windowB),
  voidMat(A.voidMat),pipeMat(A.pipeMat),
  outerVoid(A.outerVoid),activeFlag(A.activeFlag)
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
      flangeA=A.flangeA;
      flangeB=A.flangeB;
      windowA=A.windowA;
      windowB=A.windowB;
      voidMat=A.voidMat;
      pipeMat=A.pipeMat;
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

  voidMat=ModelSupport::EvalDefMat(Control,keyName+"VoidMat",0);
  pipeMat=ModelSupport::EvalMat<int>(Control,keyName+"PipeMat");

  outerVoid = Control.EvalDefVar<int>(keyName+"OuterVoid",0);

  // if flanges are good:
  populateUnit(Control,"Flange","A",flangeA);
  populateUnit(Control,"Flange","B",flangeB);
  populateUnit(Control,"Window","A",windowA);
  populateUnit(Control,"Window","B",windowB);

  return;
}

void
GeneralPipe::populateUnit(const FuncDataBase& Control,
			  const std::string& partName,
			  const std::string& indexName,
			  windowInfo& WI) const
  /*!
    Populate a variable system with windowInfo.
    \param Control :: Database
    \param partNAme :: the flange/window general name
    \param indexName :: front/back as A or B
    \param WI :: windowInfo to populate
  */
{
  ELog::RegMethod RegA("GeneralPipe","populateUnit");

  const std::string baseName(keyName+partName);
  const std::string AName(baseName+indexName);
  WI.type=Control.EvalDefTail<int>(AName,baseName,"Type",0);
  WI.thick=0.0;       // in case flange/window is not present

  if (WI.type)
    {
      WI.mat=ModelSupport::EvalMat<int>
	(Control,AName+"Mat",baseName+"Mat");
      // possiblity of two names : length or thick
      WI.thick=
	Control.EvalDefTail<double>(AName,baseName,"Thick",0.0);
      WI.thick=
	Control.EvalDefTail<double>(AName,baseName,"Length",WI.thick);
      if (WI.type==1) {
	WI.radius=Control.EvalTail<double>(AName,baseName,"Radius");
	WI.innerRadius=Control.EvalDefTail<double>(AName,baseName,"InnerRadius", WI.radius);
	if (WI.radius<WI.innerRadius-Geometry::zeroTol)
	  throw ColErr::OrderError<double>(WI.radius,WI.innerRadius,"InnerRadius must be <= Radius");
      } else
	{
	  WI.height=Control.EvalTail<double>(AName,baseName,"Height");
	  WI.width=Control.EvalTail<double>(AName,baseName,"Width");
	}
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
  return;
}

void
GeneralPipe::createWindowSurfaces()
  /*!
    General the surfaces for the windows based on
    the window type for the first or last window
    \todo Add rotation to window
   */
{
  ELog::RegMethod RegA("GeneralPipe","createWindowSurfaces");


  if (windowA.type)
    {
      getShiftedFront(SMap,buildIndex+1001,Y,
		      (flangeA.thick-windowA.thick)/2.0);
      getShiftedFront(SMap,buildIndex+1002,Y,
		      (flangeA.thick+windowA.thick)/2.0);
      // add data to surface
      addSurf("FrontWindow",SMap.realSurf(buildIndex+1001));
      addSurf("FrontWindow",SMap.realSurf(buildIndex+1002));

      // FRONT WINDOW SURFACES:
      if (windowA.type==1)
	ModelSupport::buildCylinder(SMap,buildIndex+1007,Origin,Y,
				    windowA.radius);
      else
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
    }

  // BACK WINDOW SURFACES:
  if (windowB.type)
    {
      getShiftedBack(SMap,buildIndex+1101,Y,
		     -(flangeB.thick+windowB.thick)/2.0);
      getShiftedBack(SMap,buildIndex+1102,Y,
		     -(flangeB.thick-windowB.thick)/2.0);
      // add data to surface
      addSurf("BackWindow",buildIndex+1101);
      addSurf("BackWindow",buildIndex+1102);


      if (windowB.type==1)
	ModelSupport::buildCylinder(SMap,buildIndex+1107,Origin,Y,
				    windowB.radius);
      else
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
    }
  return;
}

void
GeneralPipe::createFlangeSurfaces()
  /*!
    Create the flange surfaces either rectangle or cylindrical
  */
{
  ELog::RegMethod RegA("GeneralPipe","createFlangeSurfaces");


  if (flangeA.type)
    {
      getShiftedFront(SMap,buildIndex+101,Y,flangeA.thick);
      addSurf("FrontFlange",SMap.realSurf(buildIndex+1011));

      // FRONT FLANGE SURFACES:
      if (flangeA.type==1) {
	if (flangeA.radius>flangeA.innerRadius+Geometry::zeroTol)
	  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,flangeA.innerRadius);
	ModelSupport::buildCylinder(SMap,buildIndex+117,Origin,Y,
				    flangeA.radius);
      } else
	{
	  ModelSupport::buildPlane(SMap,buildIndex+103,
				   Origin-X*(flangeA.width/2.0),X);
	  ModelSupport::buildPlane(SMap,buildIndex+104,
				   Origin+X*(flangeA.width/2.0),X);
	  ModelSupport::buildPlane(SMap,buildIndex+105,
				   Origin-Z*(flangeA.height/2.0),Z);
	  ModelSupport::buildPlane(SMap,buildIndex+106,
				   Origin+Z*(flangeA.height/2.0),Z);
	}
    }

  // BACK FLANGE SURFACES:
  if (flangeB.type)
    {
      getShiftedBack(SMap,buildIndex+201,Y,-flangeB.thick);
      addSurf("BackFlange",buildIndex+201);

      if (flangeB.type==1) {
	if (flangeB.radius>flangeB.innerRadius+Geometry::zeroTol)
	  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,Y,flangeB.innerRadius);
	ModelSupport::buildCylinder(SMap,buildIndex+217,Origin,Y,
				    flangeB.radius);
      } else
	{
	  ModelSupport::buildPlane(SMap,buildIndex+203,
				   Origin-X*(flangeB.width/2.0),X);
	  ModelSupport::buildPlane(SMap,buildIndex+204,
				   Origin+X*(flangeB.width/2.0),X);
	  ModelSupport::buildPlane(SMap,buildIndex+205,
				   Origin-Z*(flangeB.height/2.0),Z);
	  ModelSupport::buildPlane(SMap,buildIndex+206,
				   Origin+Z*(flangeB.height/2.0),Z);
	}
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
  createFlangeSurfaces();
  createWindowSurfaces();

  // MAIN SURFACES:
  makeCylinder("InnerCyl",SMap,buildIndex+7,Origin,Y,radius);
  makeCylinder("PipeCyl",SMap,buildIndex+17,Origin,Y,radius+pipeThick);
  addSurf("OuterRadius",SMap.realSurf(buildIndex+17));

  return;
}

void
GeneralPipe::createRectangleSurfaces(const double width,
				     const double height)
  /*!
    Create the surfaces for a rectanglular system
    This is only here because of the mess of working with
    round/cylindrical pipe and it is possibliy better to have
    two types...
    \param width :: Width of pipe (inner)
    \param height :: Heigh of pipe (inner)
  */
{
  ELog::RegMethod RegA("GeneralPipe","createRectangularSurfaces");

  createCommonSurfaces();
  createFlangeSurfaces();
  createWindowSurfaces();

  double H(height/2.0);
  double W(width/2.0);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*W,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*W,X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*H,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*H,Z);

  ELog::EM<<"Pip thick == "<<pipeThick<<ELog::endDiag;
  H+=pipeThick;
  W+=pipeThick;
  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*W,X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*W,X);
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*H,Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*H,Z);

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

  if (flangeA.type)  // no flange - no window
    {
      // create window
      HeadRule windowAExclude;
      if (windowA.type)      // FRONT
	{
	  HR=ModelSupport::getSetHeadRule
	    (SMap,buildIndex,"-1007 1003 -1004 1005 -1006 1001 -1002");
	  makeCell("Window",System,cellIndex++,windowA.mat,0.0,
		   HR*getDivider("front"));

	  ExternalCut::setCutSurf("AWindow",HR);
	  windowAExclude=HR.complement();
	}

      if (flangeA.radius>flangeA.innerRadius+Geometry::zeroTol) {
	HR=ModelSupport::getHeadRule(SMap,buildIndex,"-101 -107");
	makeCell("FlangeAInnerVoid",System,cellIndex++,0,0.0,HR*frontHR*windowAExclude);
	HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"-101 103 -104 105 -106 107 -117");
	makeCell("FlangeA",System,cellIndex++,flangeA.mat,0.0,HR*frontHR);
      } else {
	HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"-101 103 -104 105 -106 -117");
	makeCell("FlangeA",System,cellIndex++,flangeA.mat,0.0,
		 HR*frontHR*outerHR*windowAExclude);
      }
    }
  if (flangeB.type)  // no flange - no window
    {
      // create window
      HeadRule windowBExclude;
      if (windowB.type)      // FRONT
	{
	  HR=ModelSupport::getSetHeadRule
	    (SMap,buildIndex,"-1107 1103 -1104 1105 -1106 1101 -1102");
	  makeCell("Window",System,cellIndex++,windowB.mat,0.0,
		   HR*getDivider("back"));

	  ExternalCut::setCutSurf("BWindow",HR);
	  windowBExclude=HR.complement();
	}

      if (flangeB.radius>flangeB.innerRadius+Geometry::zeroTol) {
	HR=ModelSupport::getHeadRule(SMap,buildIndex,"201 -207");
	makeCell("FlangeBInnerVoid",System,cellIndex++,0,0.0,HR*backHR*windowBExclude);
	HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"201 203 -204 205 -206 207 -217");
	makeCell("FlangeB",System,cellIndex++,flangeA.mat,0.0,HR*backHR);
      } else {
	HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"201 203 -204 205 -206 -217");
	makeCell("FlangeB",System,cellIndex++,flangeB.mat,0.0,
		 HR*backHR*outerHR*windowBExclude);
      }
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
    \param outerHR :: outer surface of main pipe (outward looking)
  */
{
  ELog::RegMethod RegA("GeneralPipe","createOuterVoid");

  const HeadRule& frontHR=getRule("front");
  const HeadRule& backHR=getRule("back");
  HeadRule HR;
  if (outerVoid)
    {

      if (flangeA.type==1 && flangeB.type==1) // both cylinders
	{
	  if (std::abs(flangeA.radius-flangeB.radius)<Geometry::zeroTol)
	    {
	      HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -201 -117");
	      makeCell("outerVoid",System,cellIndex++,0,0.0,HR*outerHR);
	      HR=HeadRule(SMap,buildIndex,-117);
	      addOuterSurf("Main",HR*frontHR*backHR);
	    }
	  else if (flangeA.radius>flangeB.radius)
	    {
	      HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -201 -117");
	      makeCell("outerVoid",System,cellIndex++,0,0.0,HR*outerHR);
	      HR=ModelSupport::getHeadRule(SMap,buildIndex,"201 217 -117");
	      makeCell("outerVoid",System,cellIndex++,0,0.0,HR*backHR);
	      HR=HeadRule(SMap,buildIndex,-117);
	      addOuterSurf("Main",HR*frontHR*backHR);
	    }
	  else
	    {
	      HR=ModelSupport::getHeadRule(SMap,buildIndex," 101 -201 -217 ");
	      makeCell("outerVoid",System,cellIndex++,0,0.0,HR*outerHR);
	      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-101 117 -217");
	      makeCell("outerVoid",System,cellIndex++,0,0.0,HR*frontHR);
	      HR=HeadRule(SMap,buildIndex,-217);
	      addOuterSurf("Main",HR*frontHR*backHR);
	    }
	  return;
	}

      if (flangeA.type==2 && flangeB.type==2)
	{
	  if (std::abs(flangeA.width-flangeB.width)<Geometry::zeroTol &&
	      std::abs(flangeA.height-flangeB.height)<Geometry::zeroTol)
	    {
	      HR=ModelSupport::getHeadRule
		(SMap,buildIndex,"101 -201 103 -104 105 -106");
	      makeCell("outerVoid",System,cellIndex++,0,0.0,HR*outerHR);
	      HR=ModelSupport::getHeadRule
		(SMap,buildIndex,"103 -104 105 -106");
	      addOuterSurf("Main",HR*frontHR*backHR);
	      return;
	    }
	  HeadRule midHR;  // part we inclue
	  unsigned int fIndex(0),bIndex(0);
	  if (flangeA.width>=flangeB.width-Geometry::zeroTol)
	    {
	      midHR=ModelSupport::getHeadRule(SMap,buildIndex,"103 -104");
	      bIndex=1;
	    }
	  else
	    {
	      midHR=ModelSupport::getHeadRule(SMap,buildIndex,"203 -204");
	      fIndex=1;
	    }
	  if (flangeA.height>=flangeB.height-Geometry::zeroTol)
	    {
	      midHR*=ModelSupport::getHeadRule(SMap,buildIndex,"105 -106");
	      bIndex |=2;
	    }
	  else
	    {
	      midHR*=ModelSupport::getHeadRule(SMap,buildIndex,"205 -206");
	      fIndex |=2;
	    }
	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -201");
	  makeCell("outerVoid",System,cellIndex++,0,0.0,HR*outerHR*midHR);
	  // This could be done with offset numbers... but
	  // but it just get confusing
	  const std::vector<std::string> options=
	    {"-103:104",
	     "-105:106",
	     "-103:104:-105:106"};
	  if (fIndex)
	    {
	      HR=HeadRule(SMap,buildIndex,-101);
	      HR*=ModelSupport::getHeadRule(SMap,buildIndex,options[fIndex-1]);
	      makeCell("outerVoid",System,cellIndex++,0,0.0,HR*midHR*frontHR);
	    }
	  if (bIndex)
	    {
	      HR=HeadRule(SMap,buildIndex,201);
	      HR*=ModelSupport::getHeadRule(SMap,buildIndex+100,
					    options[bIndex-1]);
	      makeCell("outerVoid",System,cellIndex++,0,0.0,HR*midHR*backHR);
	    }
	  addOuterSurf("Main",midHR*frontHR*backHR);
	  return;
	}
    }

  // NO OUTER VOID:
  else
    {
      // outer boundary [flange front]
      HR= (flangeA.type==1) ?
	ModelSupport::getHeadRule(SMap,buildIndex,"-101 -117") :
	ModelSupport::getHeadRule(SMap,buildIndex,"-101 103 -104 105 -106");
      addOuterSurf("FlangeA",HR*frontHR);

      // outer boundary [flange back]
      HR= (flangeB.type==1) ?
	ModelSupport::getSetHeadRule(SMap,buildIndex,"201 -217") :
	ModelSupport::getHeadRule(SMap,buildIndex,"201 203 -204 205 -206");
      addOuterSurf("FlangeB",HR*backHR);

      // outer boundary mid tube
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -201");
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
