/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/HoleShape.cxx
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
#include <set>
#include <map>
#include <string>
#include <algorithm>
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
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "SurInter.h"
#include "HoleShape.h"

#include "localRotate.h"
#include "masterRotate.h"

namespace constructSystem
{

HoleShape::HoleShape(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedComp(Key,2),
  holeIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(holeIndex+1),shapeType(0),
  angleOffset(0),radialStep(0.0),radius(0.0)
  /*!
    Default constructor
    \param Key :: Key name for variables
  */
{}

HoleShape::HoleShape(const HoleShape& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  holeIndex(A.holeIndex),cellIndex(A.cellIndex),
  shapeType(A.shapeType),angleCentre(A.angleCentre),
  angleOffset(A.angleOffset),radialStep(A.radialStep),
  radius(A.radius),rotCentre(A.rotCentre),
  rotAngle(A.rotAngle),frontFace(A.frontFace),
  backFace(A.backFace)
  /*!
    Copy constructor
    \param A :: HoleShape to copy
  */
{}

HoleShape&
HoleShape::operator=(const HoleShape& A)
  /*!
    Assignment operator
    \param A :: HoleShape to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      shapeType=A.shapeType;
      angleCentre=A.angleCentre;
      angleOffset=A.angleOffset;
      radialStep=A.radialStep;
      radius=A.radius;
      rotCentre=A.rotCentre;
      rotAngle=A.rotAngle;
      frontFace=A.frontFace;
      backFace=A.backFace;
    }
  return *this;
}

void
HoleShape::setShape(const size_t ST)
  /*!
    Set the output shape:
    Options are :
    - 0 : Nothing
    - 1 : Circle
    - 2 : Square
    - 3 : Hexagon
    - 4 : Octagon
    \param ST :: Shape type
  */
{
  ELog::RegMethod RegA("HoleShape","setShape");
      
  if (ST>4)
    throw ColErr::IndexError<size_t>(ST,4,"Shape not definde : ST"); 

  shapeType=ST;
  return;
}

void
HoleShape::populate(const FuncDataBase& Control)
  /*!
    Sets the size of the object
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("HoleShape","populate");
  
  setShape(Control.EvalVar<size_t>(keyName+"Shape"));
  
  angleCentre=Control.EvalVar<double>(keyName+"AngleCentre");
  angleOffset=Control.EvalVar<double>(keyName+"AngleOffset");
  radialStep=Control.EvalVar<double>(keyName+"RadialStep");
  radius=Control.EvalVar<double>(keyName+"Radius");
  return;
}

void
HoleShape::createUnitVector(const attachSystem::FixedComp& FC,
			    const long int sideIndex)
  /*!
    Create the unit vectors:
    LC gives the origin for the wheel
    \param FC :: Rotational origin of wheel
    \param sideIndex :: Side index
  */
{
  ELog::RegMethod RegA("HoleShape","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  rotCentre=Origin;
  
  // Now need to move origin to centre of shape

  const Geometry::Quaternion Qy=
    Geometry::Quaternion::calcQRotDeg(rotAngle,Y);
  Qy.rotate(X);
  Qy.rotate(Z);
  FixedComp::applyShift(0,0,radialStep);  

  return;
}

void
HoleShape::createCircleSurfaces()
  /*!
    Construct inner shape with a circle.
    Initially a straight cyclinder cut along the axis
    later will have a cone cut.
  */
{
  ELog::RegMethod RegA("HoleShape","createCircleSurfaces");  

  ModelSupport::buildCylinder(SMap,holeIndex+31,Origin,Y,radius);
  return;
}

void
HoleShape::setFaces(const int F,const int B)
  /*!
    Set face objects
    \param F :: Front face
    \param B :: Back face
  */
{
  frontFace.reset();
  backFace.reset();
  frontFace.addIntersection(F);
  backFace.addIntersection(B);
  return;
}

std::string
HoleShape::createCircleObj() 
  /*!
    Create the void and liner objects.
    Note that the returned object will be passed via
    getComposite later.
    \return the exclusion surface object / set
  */
{
  ELog::RegMethod RegA("HoleShape","createCircleObj");

  const std::string Out=
    ModelSupport::getComposite(SMap,holeIndex," -31 ");
  return Out;
}

void
HoleShape::createSquareSurfaces()
  /*!
    Construct inner shape with a square.
    Initially a straight cyclinder cut along the axis
    later will have a cone cut.
  */
{
  ELog::RegMethod RegA("HoleShape","createSquareSurfaces");

  // inner square
  ModelSupport::buildPlane(SMap,holeIndex+33,Origin-X*radius,X);
  ModelSupport::buildPlane(SMap,holeIndex+34,Origin+X*radius,X);
  ModelSupport::buildPlane(SMap,holeIndex+35,Origin-Z*radius,Z);
  ModelSupport::buildPlane(SMap,holeIndex+36,Origin+Z*radius,Z);
  return;
}

std::string
HoleShape::createSquareObj() 
  /*!
    Create the square cutout
    \return the exclusion surface object / set
  */
{
  ELog::RegMethod RegA("HoleShape","createSquareObj");
  
  const std::string Out=
    ModelSupport::getComposite(SMap,holeIndex," 33 -34 35 -36");

  return Out;
}

void
HoleShape::createHexagonSurfaces()
  /*!
    Construct inner shape with a hexagon.
    Initially a straight Hexagon cut along the axis
    later will have a cone cut.
  */
{
  ELog::RegMethod RegA("HoleShape","createHexagonSurfaces");

  double theta(0.0);
  for(int i=0;i<6;i++)
    {
      const Geometry::Vec3D Norm=X*sin(theta)+Z*cos(theta);      
      ModelSupport::buildPlane(SMap,holeIndex+31+i,
			       Origin+Norm*radius,Norm);      
      theta+=2.0*M_PI/6.0;
    }
  return;
}

std::string
HoleShape::createHexagonObj() 
  /*!
    Create the hexagon cutout
    \return the  surface object / set
  */
{
  ELog::RegMethod RegA("HoleShape","createHexagonObj");
  
  const std::string Out=
    ModelSupport::getComposite(SMap,holeIndex," -31 -32 -33 -34 -35 -36");
  return Out;
}

void
HoleShape::createOctagonSurfaces()
  /*!
    Construct inner shape with a octagon.
    Initially a straight Octagon cut along the axis
    later will have a cone cut.
  */
{
  ELog::RegMethod RegA("HoleShape","createOctagonSurfaces");

  // inner Octagon
  double theta(0.0);
  for(int i=0;i<8;i++)
    {
      const Geometry::Vec3D Norm=X*sin(theta)+Z*cos(theta);
      ModelSupport::buildPlane(SMap,holeIndex+31+i,Origin+Norm*radius,Norm);
      theta+=2.0*M_PI/8.0;
    }
  return;
}

std::string
HoleShape::createOctagonObj() 
  /*!
    Create the octagon cutout
    \return the exclusion surface object / set
  */
{
  ELog::RegMethod RegA("HoleShape","createOctagonObj");

  const std::string Out=
    ModelSupport::getComposite(SMap,holeIndex,
				 " -31 -32 -33 -34 -35 -36 -37 -38");

  return Out;
}

void
HoleShape::createSurfaces()
  /*!
    Create All the surfaces
   */
{
  ELog::RegMethod RegA("HoleShape","createSurface");
  
  switch (shapeType)
    {
    case 0:   // No hole:
      break;
    case 1:   // Circle
      createCircleSurfaces();
      break;
    case 2:   // Square
      createSquareSurfaces();
      break;
    case 3:   // Hexagon
      createHexagonSurfaces();
      break;
    case 4:   // Octagon
      createOctagonSurfaces();
      // No way to get here at Shape is controlled
    }

  return;
}

void
HoleShape::createObjects(Simulation& System) 
  /*!
    Adds the Chip guide components
    \param System :: Simuation for object
  */
{
  ELog::RegMethod RegA("HoleShape","createObjects");
  std::string Out;
  switch (shapeType)
    {
    case 1:  // circle
      Out=createCircleObj();
      break;
    case 2:  // square
      Out=createSquareObj();
      break;
    case 3:  // hexagon
      Out=createHexagonObj();
      break;
    case 4:  // octagon
      Out=createOctagonObj();
      break;
    default:  // No work
      return;
    }
  System.addCell(MonteCarlo::Qhull
		 (cellIndex++,0,0.0,Out+frontFace.display()+
		  backFace.display()));
  addOuterSurf(Out);
  return;
}

void
HoleShape::createLinks()
  /*!
    Construct the links for the system
   */
{
  ELog::RegMethod RegA("HoleShape","createLinks");
  
  const std::pair<Geometry::Vec3D,int> Front=
    SurInter::interceptRule(frontFace,Origin,Y);
  const std::pair<Geometry::Vec3D,int> Back=
    SurInter::interceptRule(backFace,Origin,Y);

  FixedComp::setConnect(0,Front.first,-Y);
  FixedComp::setConnect(1,Back.first,-Y);
  FixedComp::setLinkSurf(0,SMap.realSurf(Front.second));
  FixedComp::setLinkSurf(1,SMap.realSurf(Back.second));
  return;
}

void
HoleShape::setMasterAngle(const double masterAngle)
  /*!
    Given the main angle for the system
    \param masterAngle :: angle for the wheel
  */
{
  ELog::RegMethod RegA("HoleShape","setMasterAngle");
  
  rotAngle=masterAngle+angleOffset+angleCentre;
  return;
}
  
void
HoleShape::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simuation 
    \param FC :: Fixed component to set axis etc
  */
{
  ELog::RegMethod RegA("HoleShape","createAll");

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  
  return;
}

  
}  // NAMESPACE shutterSystem
