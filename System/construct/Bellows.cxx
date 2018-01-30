/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/Bellows.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "FrontBackCut.h"
#include "SurfMap.h"

#include "Bellows.h"

namespace constructSystem
{

Bellows::Bellows(const std::string& Key) : 
  attachSystem::FixedOffset(Key,11),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  attachSystem::SurfMap(),attachSystem::FrontBackCut(),
  vacIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(vacIndex+1),frontJoin(0),backJoin(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

Bellows::~Bellows() 
  /*!
    Destructor
  */
{}

void
Bellows::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("Bellows","populate");
  
  FixedOffset::populate(Control);

  // Void + Fe special:
  radius=Control.EvalVar<double>(keyName+"Radius");  
  length=Control.EvalVar<double>(keyName+"Length");

  feThick=Control.EvalVar<double>(keyName+"FeThick");
  bellowThick=Control.EvalDefVar<double>(keyName+"BellowThick",0.0);
  bellowStep=Control.EvalDefVar<double>(keyName+"BellowStep",0.0);

  flangeARadius=Control.EvalPair<double>(keyName+"FlangeFrontRadius",
					   keyName+"FlangeRadius");
  flangeBRadius=Control.EvalPair<double>(keyName+"FlangeBackRadius",
					 keyName+"FlangeRadius");

  flangeALength=Control.EvalPair<double>(keyName+"FlangeFrontLength",
					 keyName+"FlangeLength");
  flangeBLength=Control.EvalPair<double>(keyName+"FlangeBackLength",
					 keyName+"FlangeLength");

  
  voidMat=ModelSupport::EvalDefMat<int>(Control,keyName+"VoidMat",0);
  feMat=ModelSupport::EvalMat<int>(Control,keyName+"FeMat");
  bellowMat=ModelSupport::EvalDefMat<int>(Control,keyName+"BellowMat",feMat);

  return;
}

void
Bellows::createUnitVector(const attachSystem::FixedComp& FC,
                             const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("Bellows","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  // after rotation
  applyActiveFrontBack();

  return;
}


void
Bellows::applyActiveFrontBack()
  /*!
    Apply the active front/back point to re-calcuate Origin
    It applies the rotation of Y to Y' to both X/Z to preserve
    orthogonality.
   */
{
  ELog::RegMethod RegA("Bellows","applyActiveFrontBack");


  const Geometry::Vec3D curFP=(frontJoin) ? FPt : Origin;
  const Geometry::Vec3D curBP=(backJoin) ? BPt : Origin+Y*length;

  Origin=(curFP+curBP)/2.0;
  const Geometry::Vec3D YAxis=(curBP-curFP).unit();
  Geometry::Vec3D RotAxis=(YAxis*Y).unit();   // need unit for numerical acc.
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
  return;
}
  

void
Bellows::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("Bellows","createSurfaces");
  
  // Inner void
  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,vacIndex+1,Origin-Y*(length/2.0),Y);    
      FrontBackCut::setFront(SMap.realSurf(vacIndex+1));
    }
  getShiftedFront(SMap,vacIndex+11,1,Y,flangeALength);
  getShiftedFront(SMap,vacIndex+21,1,Y,flangeALength+bellowStep);
  
  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,vacIndex+2,Origin+Y*(length/2.0),Y);
      FrontBackCut::setBack(-SMap.realSurf(vacIndex+2));
    }

  FrontBackCut::getShiftedBack(SMap,vacIndex+12,-1,Y,flangeBLength);
  FrontBackCut::getShiftedBack(SMap,vacIndex+22,-1,Y,
			       flangeBLength+bellowStep);
  
  ModelSupport::buildCylinder(SMap,vacIndex+7,Origin,Y,radius);
  ModelSupport::buildCylinder(SMap,vacIndex+17,Origin,Y,radius+feThick);
  ModelSupport::buildCylinder(SMap,vacIndex+27,Origin,Y,radius+feThick+bellowThick);

  ELog::EM<<"radids "<<radius<<" "<<feThick<<" "<<bellowThick<<ELog::endDiag;
  ELog::EM<<"at "<<bellowMat<<ELog::endDiag;
  // FLANGE SURFACES FRONT/BACK:
  ModelSupport::buildCylinder(SMap,vacIndex+107,Origin,Y,flangeARadius);
  ModelSupport::buildCylinder(SMap,vacIndex+207,Origin,Y,flangeBRadius);
  
  return;
}

void
Bellows::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("Bellows","createObjects");

  std::string Out;
  
  const std::string frontStr=frontRule();
  const std::string backStr=backRule();
  
  // Void
  Out=ModelSupport::getComposite(SMap,vacIndex," -7 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out+frontStr+backStr);

  // FLANGE Front: 
  Out=ModelSupport::getComposite(SMap,vacIndex," -11 -107 7 ");
  makeCell("FrontFlange",System,cellIndex++,feMat,0.0,Out+frontStr);

  // FLANGE Back: 
  Out=ModelSupport::getComposite(SMap,vacIndex," 12 -207 7 ");
  makeCell("BackFlange",System,cellIndex++,feMat,0.0,Out+backStr);

  // Inner clip if present
  if (bellowStep>Geometry::zeroTol)
    {
      Out=ModelSupport::getComposite(SMap,vacIndex," 11 -21 -17 7 ");
      makeCell("FrontClip",System,cellIndex++,feMat,0.0,Out);

      Out=ModelSupport::getComposite(SMap,vacIndex," -12 22 -17 7 ");
      makeCell("BackClip",System,cellIndex++,feMat,0.0,Out);

      Out=ModelSupport::getComposite(SMap,vacIndex," 21 -22 -27 7");
      makeCell("Bellow",System,cellIndex++,bellowMat,0.0,Out);

      Out=ModelSupport::getComposite(SMap,vacIndex," 11 -21 -27 17");
      makeCell("FrontBellowVoid",System,cellIndex++,0,0.0,Out);

      Out=ModelSupport::getComposite(SMap,vacIndex," -12 22 -27 17");
      makeCell("BackBellowVoid",System,cellIndex++,0,0.0,Out);
    }
  else
    {
      Out=ModelSupport::getComposite(SMap,vacIndex," 11 -22 -27 7 ");
      makeCell("Bellow",System,cellIndex++,bellowMat,0.0,Out);
    }
  
  
  // outer boundary [flange front]
  Out=ModelSupport::getSetComposite(SMap,vacIndex," -11 -107 ");
  addOuterSurf(Out+frontStr);
  Out=ModelSupport::getSetComposite(SMap,vacIndex," 12 -207 ");
  addOuterUnionSurf(Out+backStr);

  
  // outer boundary mid tube
  Out=ModelSupport::getSetComposite(SMap,vacIndex," 11 -12 -27");
  addOuterUnionSurf(Out);

  return;
}


  
void
Bellows::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("Bellows","createLinks");

  //stufff for intersection


  FrontBackCut::createLinks(*this,Origin,Y);  //front and back
  FixedComp::setConnect(2,Origin-X*radius,-X);
  FixedComp::setConnect(3,Origin+X*radius,X);
  FixedComp::setConnect(4,Origin-Z*radius,-Z);
  FixedComp::setConnect(5,Origin+Z*radius,Z);
  FixedComp::setLinkSurf(2,SMap.realSurf(vacIndex+7));
  FixedComp::setLinkSurf(3,SMap.realSurf(vacIndex+7));
  FixedComp::setLinkSurf(4,SMap.realSurf(vacIndex+7));
  FixedComp::setLinkSurf(5,SMap.realSurf(vacIndex+7));
  
  FixedComp::setConnect(7,Origin-Z*(radius+bellowThick),-Z);
  FixedComp::setConnect(8,Origin+Z*(radius+bellowThick),Z);
  FixedComp::setLinkSurf(7,SMap.realSurf(vacIndex+27));
  FixedComp::setLinkSurf(8,SMap.realSurf(vacIndex+27));
  

  return;
}
  
void
Bellows::setFront(const attachSystem::FixedComp& FC,
		     const long int sideIndex,
		     const bool joinFlag)
  /*!
    Set front surface
    \param FC :: FixedComponent 
    \param sideIndex ::  Direction to link
    \param joinFlag :: joint front to link object 
   */
{
  ELog::RegMethod RegA("Bellows","setFront");

  
  FrontBackCut::setFront(FC,sideIndex);
  if (joinFlag)
    {
      frontJoin=1;
      FPt=FC.getLinkPt(sideIndex);
      FAxis=FC.getLinkAxis(sideIndex);
    }
    
  return;
}
  
void
Bellows::setBack(const attachSystem::FixedComp& FC,
		    const long int sideIndex,
		    const bool joinFlag)
  /*!
    Set Back surface
    \param FC :: FixedComponent 
    \param sideIndex ::  Direction to link
    \param joinFlag :: joint front to link object 
   */
{
  ELog::RegMethod RegA("Bellows","setBack");
  
  FrontBackCut::setBack(FC,sideIndex);
  if (joinFlag)
    {
      backJoin=1;
      BPt=FC.getLinkPt(sideIndex);
      BAxis=FC.getLinkAxis(sideIndex);
    }
  return;
}
  
void
Bellows::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int FIndex)
 /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("Bellows","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();    
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE constructSystem
