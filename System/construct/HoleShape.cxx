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
#include "LinkUnit.h"
#include "TwinComp.h"
#include "ContainedComp.h"
#include "HoleShape.h"

namespace constructSystem
{

HoleShape::HoleShape(const std::string& Key,
		     const int HIndex) :
  attachSystem::ContainedComp(),
  attachSystem::FixedComp(Key,2),
  holeIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(holeIndex+1),shapeType(0),
  angleOffset(0),radialOffset(0.0),radius(0.0)
  /*!
    Default constructor
    \param SR :: Register to use
    \param Key :: Key name for variables
    \param HIndex :: Index offset 
  */
{}

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
    throw ColErr::IndexError<int>(ST,4,"Shape not definde : ST"); 

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
  
  angleOffset=Control.EvalVar<double>(keyName+"Angle");
  radialOffset=Control.EvalVar<double>(keyName+"ROffset");
  radius=Control.EvalVar<double>(keyName+"Radius");
  return;
}

void
HoleShape::createUnitVector(const attachSystem::FixedComp& FC,
			    const long int sideIndex)
  /*!
    Create the unit vectors:
    LC gives the origin for the wheel
    \param rotAngle :: Rotation angle
    \param FC :: Rotational origin 
    \param sideIndex :: Side index
  */
{
  ELog::RegMethod RegA("HoleShape","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  FixedComp::applyShift(0,0,radialOffset);
  FixedComp::applyAngleRotate(rotAngle,0.0);
  
  

  apply
  Origin=FC.getExit();
  // Now rotate:
  const Geometry::Quaternion QAxisR=
    Geometry::Quaternion::calcQRotDeg(rotAngle+AngleOffset,Y);
 
  QAxisR.rotate(X);
  QAxisR.rotate(Z);

  Centre=Origin+X*radialOffset;
  setExit(Origin+X*radialOffset+Y*depth,Y);
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
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  
  // inner cyclinder:
  Geometry::Cylinder* CX;
  CX=SurI.createUniqSurf<Geometry::Cylinder>(holeIndex+31);  
  CX->setCylinder(Centre,Y,radius);
  HMap.registerSurf(holeIndex+31,CX);
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
  frontFace=F;
  backFace=B;
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
  std::ostringstream cx;
  cx<<ModelSupport::getComposite(HMap,holeIndex,"-31");
  addOuterSurf(cx.str());
  cx<<" "<<frontFace<<" "<<-backFace;   
  
  return cx.str();
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
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  
  // inner square
  Geometry::Plane* PX;
  PX=SurI.createUniqSurf<Geometry::Plane>(holeIndex+33);  
  PX->setPlane(Centre-X*radius,X);
  HMap.registerSurf(holeIndex+33,PX);

  PX=SurI.createUniqSurf<Geometry::Plane>(holeIndex+34);  
  PX->setPlane(Centre+X*radius,X);
  HMap.registerSurf(holeIndex+34,PX);

  PX=SurI.createUniqSurf<Geometry::Plane>(holeIndex+35);  
  PX->setPlane(Centre-Z*radius,Z);
  HMap.registerSurf(holeIndex+35,PX);

  PX=SurI.createUniqSurf<Geometry::Plane>(holeIndex+36);  
  PX->setPlane(Centre+Z*radius,Z);
  HMap.registerSurf(holeIndex+36,PX);
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
  
  std::ostringstream cx;
  cx<<ModelSupport::getComposite(HMap,holeIndex," 33 -34 35 -36");
  addOuterSurf(cx.str());
  cx<<" "<<frontFace<<" "<<-backFace;   

  return cx.str();
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
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  
  // inner hexagon
  Geometry::Plane* PX;
  double theta=0.0;
  for(int i=0;i<6;i++)
    {
      PX=SurI.createUniqSurf<Geometry::Plane>(holeIndex+31+i);  
      const Geometry::Vec3D Norm=X*sin(theta)+Z*cos(theta);
      PX->setPlane(Centre+Norm*radius,Norm);
      HMap.registerSurf(holeIndex+31+i,PX);
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
  
  std::ostringstream cx;
  cx<<ModelSupport::getComposite(HMap,holeIndex," -31 -32 -33 -34 -35 -36");
  addOuterSurf(cx.str());
  cx<<" "<<frontFace<<" "<<-backFace;   
    
  return cx.str();
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
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  
  // inner Octagon
  Geometry::Plane* PX;
  double theta=0.0;
  for(int i=0;i<8;i++)
    {
      PX=SurI.createUniqSurf<Geometry::Plane>(holeIndex+31+i);  
      const Geometry::Vec3D Norm=X*sin(theta)+Z*cos(theta);
      PX->setPlane(Centre+Norm*radius,Norm);
      HMap.registerSurf(holeIndex+31+i,PX);
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

  std::ostringstream cx;
  cx<<ModelSupport::getComposite(HMap,holeIndex,
				 " -31 -32 -33 -34 -35 -36 -37 -38");
  addOuterSurf(cx.str());
  cx<<" "<<frontFace<<" "<<-backFace;   
  return cx.str();
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
      break;
    default:
      ELog::EM<<"Unwritten bit"<<ELog::endErr;
    }

  return;
}

std::string 
HoleShape::createObjects() 
  /*!
    Adds the Chip guide components
    \return String of the created object
  */
{
  ELog::RegMethod RegA("HoleShape","createObjects");

  switch (shapeType)
    {
    case 0:  // No shape
      return " ";
      break;
    case 1:  // circle
      return createCircleObj();
      break;
    case 2:  // square
      return createSquareObj();
      break;
    case 3:  // hexagon
      return createHexagonObj();
      break;
    case 4:  // octagon
      return createOctagonObj();
      break;
    default:
      ELog::EM<<"Unwritten object component"<<ELog::endErr;
    }
  return " ";
}

int
HoleShape::exitWindow(const double Dist,
		    std::vector<int>& window,
		    Geometry::Vec3D& Pt) const
  /*!
    Outputs a window
    \param Dist :: Dist from exit point
    \param window :: window vector of paired planes
    \param Pt :: Output point for tally
    \return Master Plane
  */
{
  window.clear();
  window.push_back(HMap.realSurf(holeIndex+3));
  window.push_back(HMap.realSurf(holeIndex+4));
  window.push_back(HMap.realSurf(holeIndex+5));
  window.push_back(HMap.realSurf(holeIndex+6));
  Pt=Origin+Y*(depth+Dist);  
  return HMap.realSurf(holeIndex+2);
}
  

void
HoleShape::createAll(const double rotAngle,
		    const attachSystem::FixedComp& FC)
  /*!
    Generic function to create everything
    \param rotAngle :: Angle offset
    \param FC :: Fixed component to set axis etc
  */
{
  ELog::RegMethod RegA("HoleShape","createAll");

  createUnitVector(rotAngle,FC);
  createSurfaces();
  
  return;
}

  
}  // NAMESPACE shutterSystem
