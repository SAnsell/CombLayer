/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/CylGateTube.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell 
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
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "SurfMap.h"
#include "SurInter.h"

#include "CylGateTube.h" 

namespace constructSystem
{

CylGateTube::CylGateTube(const std::string& Key) : 
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::ExternalCut(),
  closed(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}



CylGateTube::~CylGateTube() 
  /*!
    Destructor
  */
{}

void
CylGateTube::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("CylGateTube","populate");
  
  FixedRotate::populate(Control);

  radius=Control.EvalVar<double>(keyName+"Radius");
  depth=Control.EvalVar<double>(keyName+"Depth");
  height=Control.EvalVar<double>(keyName+"Height");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  portZStep=Control.EvalVar<double>(keyName+"PortZStep");
  portRadius=Control.EvalVar<double>(keyName+"PortRadius");
  portFlangeRadius=Control.EvalVar<double>(keyName+"PortFlangeRadius");
  portInner=Control.EvalVar<double>(keyName+"PortInner");
  portThick=Control.EvalVar<double>(keyName+"PortThick");

  topRadius=Control.EvalVar<double>(keyName+"TopRadius");
  topHoleRadius=Control.EvalVar<double>(keyName+"TopHoleRadius");
  topThick=Control.EvalVar<double>(keyName+"TopThick");

  liftHeight=Control.EvalVar<double>(keyName+"LiftHeight");
  liftRadius=Control.EvalVar<double>(keyName+"LiftRadius");
  liftThick=Control.EvalVar<double>(keyName+"LiftThick");
  liftPlate=Control.EvalVar<double>(keyName+"LiftPlate");

  driveRadius=Control.EvalVar<double>(keyName+"DriveRadius");
  bladeLift=Control.EvalVar<double>(keyName+"BladeLift");
  bladeThick=Control.EvalVar<double>(keyName+"BladeThick");
  bladeRadius=Control.EvalVar<double>(keyName+"BladeRadius");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  bladeMat=ModelSupport::EvalMat<int>(Control,keyName+"BladeMat");
  driveMat=ModelSupport::EvalMat<int>(Control,keyName+"DriveMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  return;
}


void
CylGateTube::createSurfaces()
  /*!
    Create the surfaces
    Note that it is created in the vertical direction for the 
    main cylinder and the port view is in the Y directions.
  */
{
  ELog::RegMethod RegA("CylGateTube","createSurfaces");

  const double bladeZStep((closed) ? bladeLift : 0.0);
    
  // front/back planes
  if (!isActive("front"))
    {
      ModelSupport::buildPlane
	(SMap,buildIndex+1,Origin-Y*(radius+wallThick+portThick),Y);
      setCutSurf("front",SMap.realSurf(buildIndex+1));
    }	
  if (!isActive("back"))
    {
      ModelSupport::buildPlane
	(SMap,buildIndex+2,Origin+Y*(radius+wallThick+portThick),Y);
      setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }	
  // flanges on front / back
  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin.Z,portRadius);
  // inner flange limits:
  ModelSupport::buildPlane
    (SMap,buildIndex+101,Origin-Y*(radius+wallThick-portInner),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+102,Origin+Y*(radius+wallThick-portInner),Y);

  
  // Main tube
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Z,radius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Z,radius+wallThick);

  ModelSupport::buildCylinder(SMap,buildIndex+5,Origin-Z*depth,Z);
  ModelSupport::buildCylinder(SMap,buildIndex+15,Origin-Z*(depth+wallThick),Z);

  ModelSupport::buildCylinder(SMap,buildIndex+6,Origin+Z*height,Z);

  // Flange at top
  ModelSupport::buildCylinder
    (SMap,buildIndex+106,Origin+Z*(height+topThick),Z);
  ModelSupport::buildCylinder(SMap,buildIndex+207,Origin,Z,topRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+217,Origin,Z,topHoleRadius);
  
  // lift port
  ModelSupport::buildCylinder
    (SMap,buildIndex+206,Origin+Z*(height+topThick+liftHeight),Z);
  ModelSupport::buildCylinder
    (SMap,buildIndex+216,Origin+Z*(height+topThick+liftHeight+liftPlate),Z);

  ModelSupport::buildCylinder(SMap,buildIndex+307,Origin,Z,liftRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+317,Origin,Z,liftRadius+liftThick);
  // blade support:
  const Geometry::Vec3D bladeOrg(Origin+Z*bladeZStep);
  ModelSupport::buildPlane(SMap,buildIndex+405,bladeOrg,Z);
  ModelSupport::buildCylinder(SMap,buildIndex+407,bladeOrg,Y,bladeRadius);
  ModelSupport::buildPlane(SMap,buildIndex+401,bladeOrg-Y*(bladeThick/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+402,bladeOrg+Y*(bladeThick/2.0),Y);

  return;
}

void
CylGateTube::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("CylGateTube","createObjects");

  std::string Out;


  const std::string frontStr=frontRule();  //  1
  const std::string backStr=backRule();    // -2


  // Main Void [exclude flange cylinder/ blade and blade tube]
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 5 -6 -7 107 (407 : 401 : -402) (307 : -405)");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 5 -6 7 -17 107 ");
  makeCell("Wall",System,cellIndex++,wallMat,0.0,Out);

  // Main body
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " 1 -2 7 -17 (-3:4:-5) ");
  makeCell("Body",System,cellIndex++,wallMat,0.0,Out);
  // Top body
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 11 -12 17 13 -14 -16 5 (-1:2:-3:4:6) ");
  makeCell("Body",System,cellIndex++,wallMat,0.0,Out);

  // blade
  Out=ModelSupport::getComposite(SMap,buildIndex," -307 301 -302 ");
  makeCell("Blade",System,cellIndex++,bladeMat,0.0,Out);

  // front plate
  Out=ModelSupport::getComposite(SMap,buildIndex," -1 11 -17 117 ");
  makeCell("FrontPlate",System,cellIndex++,wallMat,0.0,Out);
  // seal ring
  Out=ModelSupport::getComposite(SMap,buildIndex," -1 107 -117 ");
  makeCell("FrontSeal",System,cellIndex++,wallMat,0.0,Out+frontStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," -1 -107 ");
  makeCell("FrontVoid",System,cellIndex++,voidMat,0.0,Out+frontStr);
  
  if (!portAExtends)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex," 11 -117 ");
      makeCell("FrontVoidExtra",System,cellIndex++,voidMat,0.0,Out+frontComp);
    }
       
  // back plate
  Out=ModelSupport::getComposite(SMap,buildIndex," 2 -12 -17 217 ");
  makeCell("BackPlate",System,cellIndex++,wallMat,0.0,Out);
  // seal ring
  Out=ModelSupport::getComposite(SMap,buildIndex," 2 107 -217 ");
  makeCell("BackSeal",System,cellIndex++,wallMat,0.0,Out+backStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 2 -207 ");
  makeCell("BackVoid",System,cellIndex++,voidMat,0.0,Out+backStr);
  
  if (!portBExtends)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex," -12 -217 ");
      makeCell("BackVoidExtra",System,cellIndex++,voidMat,0.0,Out+backComp);
    }
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 11 -12 (-17 : (13 -14 5 -16))");

  if (portAExtends || portBExtends)
    {
      if (!portAExtends)
	{
	  addOuterSurf(Out);
	  Out=ModelSupport::getComposite(SMap,buildIndex," 12 -217 ");
	}
      else if (!portBExtends)
	{
	  addOuterSurf(Out);
	  Out=ModelSupport::getComposite(SMap,buildIndex," -11 -117 ");
	}
      else
	{
	  Out=ModelSupport::getComposite(SMap,buildIndex," -11 -17 117 ");
	  makeCell("PortOuterVoid",System,cellIndex++,0,0.0,Out+frontStr);
	  Out=ModelSupport::getComposite(SMap,buildIndex," 12 -17 217 ");
	  makeCell("PortOuterVoid",System,cellIndex++,0,0.0,Out+backStr);
	  Out=ModelSupport::getComposite
	    (SMap,buildIndex," -11 -16 13 -14 5 17 ");
	  makeCell("PortOuterVoid",System,cellIndex++,0,0.0,Out+frontStr);
	  Out=ModelSupport::getComposite
	    (SMap,buildIndex," 12 -16 13 -14 5 17 ");
	  makeCell("PortOuterVoid",System,cellIndex++,0,0.0,Out+backStr);

	  Out=ModelSupport::getComposite(SMap,buildIndex," ( (13 -14 5 -16):-17 )");
	}
      addOuterUnionSurf(Out+frontStr+backStr);

    }

  //  addOuterUnionSurf(Out+frontStr+backStr);

  return;
}
  
void
CylGateTube::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("CylGateTube","createLinks");

  //stuff for intersection


  FrontBackCut::createLinks(*this,Origin,Y);  //front and back

  return;
}
  
  
void
CylGateTube::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int FIndex)
 /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("CylGateTube","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();    
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE constructSystem
