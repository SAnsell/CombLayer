/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/PipeTube.cxx
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
#include "Line.h"
#include "Plane.h"
#include "Cylinder.h"
#include "SurInter.h"
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
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"

#include "portItem.h"
#include "doublePortItem.h"
#include "VirtualTube.h"
#include "BlankTube.h"

namespace constructSystem
{

BlankTube::BlankTube(const std::string& Key) :
  VirtualTube(Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{
  nameSideIndex(2,"Flange");
  ContainedGroup::addCC("Flange");
}

  
BlankTube::~BlankTube() 
  /*!
    Destructor
  */
{}

void
BlankTube::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("BlankTube","populate");
  
  VirtualTube::populate(Control);

  blankThick=Control.EvalVar<double>(keyName+"BlankThick");

  flangeRadius=Control.EvalVar<double>(keyName+"FlangeRadius");
  flangeLength=Control.EvalVar<double>(keyName+"FlangeLength");
  flangeCapThick=Control.EvalDefVar<double>(keyName+"FlangeCapThick",0.0);
  
  return;
}

void
BlankTube::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("BlankTube","createSurfaces");

  // Do outer surfaces (vacuum ports)
  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,
			       Origin-Y*(length/2.0),Y);
      setFront(SMap.realSurf(buildIndex+1));
    }
  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,
			       Origin+Y*(length/2.0),Y);
      setBack(-SMap.realSurf(buildIndex+2));
    }
  
  
  // void space:
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);
  SurfMap::addSurf("VoidCyl",-SMap.realSurf(buildIndex+7));
    
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Y,radius+wallThick);
  SurfMap::addSurf("OuterCyl",SMap.realSurf(buildIndex+17));

  ModelSupport::buildPlane
    (SMap,buildIndex+101,
     Origin-Y*(length/2.0-(flangeLength+flangeCapThick)),Y);
  
  ModelSupport::buildPlane(SMap,buildIndex+102,
			   Origin+Y*(length/2.0-blankThick),Y);

  ModelSupport::buildPlane(SMap,buildIndex+201,
			   Origin-Y*(length/2.0-flangeCapThick),Y);

  // flange:
  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,flangeRadius);
  
  return;
}

std::string
BlankTube::makeOuterVoid(Simulation& System)
  /*!
    Build outer void and return the outer volume
    \param System :: Simulation to build in
    \return Outer exclude volume
  */
{
  ELog::RegMethod RegA("BlankTube","makeOuterVoid");
  
  std::string Out;
  const std::string frontSurf(frontRule());
  const std::string backSurf(backRule());

  Out=ModelSupport::getComposite(SMap,buildIndex," 17 -107 101 ");
  makeCell("OuterVoid",System,cellIndex++,0,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex,"  -107 ")+
    frontSurf+backSurf;
  return Out;
}

void
BlankTube::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("BlankTube","createObjects");

  const std::string frontSurf(frontRule());
  const std::string backSurf(backRule());

  const std::string frontVoidSurf=
    (flangeCapThick<Geometry::zeroTol) ? frontSurf :
    ModelSupport::getComposite(SMap,buildIndex," 201 ");
 
  std::string Out;
  
  Out=ModelSupport::getComposite(SMap,buildIndex," -7 -102 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out+
	   frontVoidSurf);
  // main walls
  Out=ModelSupport::getComposite(SMap,buildIndex," -17 7 ");
  makeCell("MainTube",System,cellIndex++,wallMat,0.0,
	   Out+frontVoidSurf+backSurf);

  Out=ModelSupport::getComposite(SMap,buildIndex," 17 -107 -101 ");
  makeCell("FrontFlange",System,cellIndex++,wallMat,0.0,Out+frontVoidSurf);

  Out=ModelSupport::getComposite(SMap,buildIndex," -7 102 ");
  makeCell("BackFlange",System,cellIndex++,wallMat,0.0,Out+backSurf);

  if (flangeCapThick>Geometry::zeroTol)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex," -201 -107 ");
      makeCell("FrontCap",System,cellIndex++,capMat,0.0,Out+frontSurf);	    
    }
  if (outerVoid)
    {
      Out=makeOuterVoid(System);
      addOuterSurf("Main",Out);
    }
  else
    {
      Out=ModelSupport::getComposite(SMap,buildIndex," 101 -2 -17 ");
      addOuterSurf("Main",Out);
      
      Out=ModelSupport::getComposite(SMap,buildIndex," -107 -101 ");
      addOuterSurf("Flange",Out+frontSurf);
      
    }
  return;
}

void
BlankTube::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane.
    Port position are used for first two link points
    Note that 0/1 are the flange surfaces
  */
{
  ELog::RegMethod RegA("BlankTube","createLinks");

  // port centre
  FrontBackCut::createFrontLinks(*this,Origin,Y); 
  FrontBackCut::createBackLinks(*this,Origin,Y);  
  // get link points
  FixedComp::setConnect(2,FixedComp::getLinkPt(1),-Y);
  FixedComp::setConnect(3,FixedComp::getLinkPt(2),Y);

  // make a composite flange
  std::string Out;
  const std::string frontSurf(frontRule());
  const std::string backSurf(backRule());
  Out=ModelSupport::getComposite(SMap,buildIndex," -101 -107 ");
  FixedComp::setLinkComp(2,Out+frontSurf);


  // inner links
  int innerFrontSurf;
  Geometry::Vec3D innerFrontVec;
  if (flangeCapThick<Geometry::zeroTol)
    {
      innerFrontSurf = getFrontRule().getPrimarySurface();
      innerFrontVec = Origin-Y*(length/2.0);
    }
  else
    {
      innerFrontSurf = buildIndex+201;
      innerFrontVec = Origin-Y*(length/2.0-flangeCapThick);
    }
  
  FixedComp::setConnect(4,innerFrontVec,Y);
  FixedComp::setLinkSurf(4,innerFrontSurf);
  nameSideIndex(4,"InnerFront");

  FixedComp::setConnect(5,Origin+Y*(length/2.0-blankThick),Y);
  FixedComp::setLinkSurf(5,-SMap.realSurf(buildIndex+102));
  nameSideIndex(5,"InnerBack");

  FixedComp::setLinkSurf(6,-SMap.realSurf(buildIndex+7));
  nameSideIndex(6,"InnerSide");

  
  return;
}

  

  
}  // NAMESPACE constructSystem
