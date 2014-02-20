/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   chip/HoleUnit.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

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
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Tally.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
#include "surfDIter.h"
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
#include "chipDataStore.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "ContainedComp.h"
#include "HoleUnit.h"

namespace hutchSystem
{

HoleUnit::HoleUnit(ModelSupport::surfRegister& SR,
		   const std::string& Key,
		   const int HIndex) :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,2),
  HMap(SR),holeIndex(HIndex),shapeType(0),
  AngleOffset(0),radialOffset(0.0),radius(0.0),
  depth(0.0),frontFace(0),backFace(0)
  /*!
    Default constructor
    \param SR :: Register to use
    \param Key :: Key name for variables
    \param HIndex :: Index offset 
  */
{}

HoleUnit::HoleUnit(const HoleUnit& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  HMap(A.HMap),holeIndex(A.holeIndex),shapeType(A.shapeType),
  AngleOffset(A.AngleOffset),radialOffset(A.radialOffset),
  radius(A.radius),depth(A.depth),frontFace(A.frontFace),
  backFace(A.backFace),Centre(A.Centre)
  /*!
    Copy constructor
    \param A :: HoleUnit to copy
  */
{}

HoleUnit&
HoleUnit::operator=(const HoleUnit& A)
  /*!
    Assignment operator
    \param A :: HoleUnit to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      shapeType=A.shapeType;
      AngleOffset=A.AngleOffset;
      radialOffset=A.radialOffset;
      radius=A.radius;
      depth=A.depth;
      frontFace=A.frontFace;
      backFace=A.backFace;
      Centre=A.Centre;
    }
  return *this;
}

void
HoleUnit::populate(const FuncDataBase& Control)
  /*!
    Sets the size of the object
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("HoleUnit","populate");

  setShape(Control.EvalVar<int>(keyName+"Shape"));
  AngleOffset=Control.EvalVar<double>(keyName+"Angle");
  radialOffset=Control.EvalVar<double>(keyName+"ROffset");
  radius=Control.EvalVar<double>(keyName+"Radius");
  return;
}

void
HoleUnit::setShape(const int ST)
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
  ELog::RegMethod RegA("HoleUnit","setShape");
      
  if (ST<0 || ST>4)
    {
      ELog::EM<<"HoleUnit only has 5 shapes types"<<ELog::endCrit;
      throw ColErr::IndexError<int>(ST,4,RegA.getBase()); 
    }
  shapeType=ST;

  return;
}


void
HoleUnit::createUnitVector(const double rotAngle,
			   const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors:
    LC gives the origin for the wheel
    \param rotAngle :: Rotation angle
    \param FC :: Fixed Component hole is within
  */
{
  ELog::RegMethod RegA("HoleUnit","createUnitVector(FixedComp)");

  //  const masterRotate& MR=masterRotate::Instance();
  //  chipIRDatum::chipDataStore& CS=chipIRDatum::chipDataStore::Instance();

  FixedComp::createUnitVector(FC);
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
HoleUnit::createCircleSurfaces()
  /*!
    Construct inner shape with a circle.
    Initially a straight cyclinder cut along the axis
    later will have a cone cut.
  */
{
  ELog::RegMethod RegA("HoleUnit","createCircleSurfaces");
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  
  // inner cyclinder:
  Geometry::Cylinder* CX;
  CX=SurI.createUniqSurf<Geometry::Cylinder>(holeIndex+31);  
  CX->setCylinder(Centre,Y,radius);
  HMap.registerSurf(holeIndex+31,CX);
  return;
}

void
HoleUnit::setFaces(const int F,const int B)
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
HoleUnit::createCircleObj() 
  /*!
    Create the void and liner objects.
    Note that the returned object will be passed via
    getComposite later.
    \return the exclusion surface object / set
  */
{
  ELog::RegMethod RegA("HoleUnit","createCircleObj");
  std::ostringstream cx;
  cx<<ModelSupport::getComposite(HMap,holeIndex,"-31");
  addOuterSurf(cx.str());
  cx<<" "<<frontFace<<" "<<-backFace;   
  
  return cx.str();
}

void
HoleUnit::createSquareSurfaces()
  /*!
    Construct inner shape with a square.
    Initially a straight cyclinder cut along the axis
    later will have a cone cut.
  */
{
  ELog::RegMethod RegA("HoleUnit","createSquareSurfaces");
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
HoleUnit::createSquareObj() 
  /*!
    Create the square cutout
    \return the exclusion surface object / set
  */
{
  ELog::RegMethod RegA("HoleUnit","createSquareObj");
  
  std::ostringstream cx;
  cx<<ModelSupport::getComposite(HMap,holeIndex," 33 -34 35 -36");
  addOuterSurf(cx.str());
  cx<<" "<<frontFace<<" "<<-backFace;   

  return cx.str();
}

void
HoleUnit::createHexagonSurfaces()
  /*!
    Construct inner shape with a hexagon.
    Initially a straight Hexagon cut along the axis
    later will have a cone cut.
  */
{
  ELog::RegMethod RegA("HoleUnit","createHexagonSurfaces");
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
HoleUnit::createHexagonObj() 
  /*!
    Create the hexagon cutout
    \return the  surface object / set
  */
{
  ELog::RegMethod RegA("HoleUnit","createHexagonObj");
  
  std::ostringstream cx;
  cx<<ModelSupport::getComposite(HMap,holeIndex," -31 -32 -33 -34 -35 -36");
  addOuterSurf(cx.str());
  cx<<" "<<frontFace<<" "<<-backFace;   
    
  return cx.str();
}

void
HoleUnit::createOctagonSurfaces()
  /*!
    Construct inner shape with a octagon.
    Initially a straight Octagon cut along the axis
    later will have a cone cut.
  */
{
  ELog::RegMethod RegA("HoleUnit","createOctagonSurfaces");
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
HoleUnit::createOctagonObj() 
  /*!
    Create the octagon cutout
    \return the exclusion surface object / set
  */
{
  ELog::RegMethod RegA("HoleUnit","createOctagonObj");

  std::ostringstream cx;
  cx<<ModelSupport::getComposite(HMap,holeIndex,
				 " -31 -32 -33 -34 -35 -36 -37 -38");
  addOuterSurf(cx.str());
  cx<<" "<<frontFace<<" "<<-backFace;   
  return cx.str();
}

void
HoleUnit::createSurfaces()
  /*!
    Create All the surfaces
   */
{
  ELog::RegMethod RegA("HoleUnit","createSurface");
  
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
HoleUnit::createObjects() 
  /*!
    Adds the Chip guide components
    \return String of the created object
  */
{
  ELog::RegMethod RegA("HoleUnit","createObjects");

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
HoleUnit::exitWindow(const double Dist,
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
HoleUnit::createAll(const double rotAngle,
		    const attachSystem::FixedComp& FC)
  /*!
    Generic function to create everything
    \param rotAngle :: Angle offset
    \param FC :: Fixed component to set axis etc
  */
{
  ELog::RegMethod RegA("HoleUnit","createAll");

  createUnitVector(rotAngle,FC);
  createSurfaces();
  
  return;
}

  
}  // NAMESPACE shutterSystem
