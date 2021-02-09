/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/HoleShape.cxx
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

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurInter.h"
#include "HoleShape.h"


namespace constructSystem
{

HoleShape::HoleShape(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedComp(Key,2),
  attachSystem::CellMap(),
  shapeType(0),
  angleOffset(0),radialStep(0.0),radius(0.0),xradius(0.0),
  cutFlag(0)
  /*!
    Default constructor
    \param Key :: Key name for variables
  */
{}

HoleShape::HoleShape(const HoleShape& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  attachSystem::CellMap(A),
  shapeType(A.shapeType),angleCentre(A.angleCentre),
  angleOffset(A.angleOffset),radialStep(A.radialStep),
  radius(A.radius),xradius(A.xradius),rotCentre(A.rotCentre),
  rotAngle(A.rotAngle),cutFlag(A.cutFlag),frontFace(A.frontFace),
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
      attachSystem::CellMap::operator=(A);
      shapeType=A.shapeType;
      angleCentre=A.angleCentre;
      angleOffset=A.angleOffset;
      radialStep=A.radialStep;
      radius=A.radius;
      rotCentre=A.rotCentre;
      rotAngle=A.rotAngle;
      cutFlag=A.cutFlag;
      frontFace=A.frontFace;
      backFace=A.backFace;
    }
  return *this;
}

size_t
HoleShape::shapeIndex(const std::string& ST)
  /*!
    Accessor to shape type 
    \param ST :: name to test / of a index string
    \return shape index
   */
{
  ELog::RegMethod RegA("HoleShape","shapeIndex");
  
  static const std::map<std::string,size_t> SName =
    {
      {"Null",0},
      {"Circle",1},
      {"Square",2},
      {"Hexagon",3},
      {"Octagon",4},
      {"Rectangle",5}
    };
  std::map<std::string,size_t>::const_iterator mc=
    SName.find(ST);
  if (mc==SName.end())
    {
      size_t shapeIndex;
      if (StrFunc::convert(ST,shapeIndex) && shapeIndex<6)
	return shapeIndex;
      throw ColErr::InContainerError<std::string>(ST,"ShapeType");
    }
  
  return mc->second;
}
  
void
HoleShape::setShape(const std::string& ST)
  /*!
    Set the output shape:
    Options are :
    - 0 : Nothing
    - 1 : Circle
    - 2 : Square
    - 3 : Hexagon
    - 4 : Octagon
    - 5 : Rectangle
    \param ST :: Shape type
  */
{
  ELog::RegMethod RegA("HoleShape","setShape(string)");

  shapeType=shapeIndex(ST);
  return;
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
    - 5 : Rectangle
    \param ST :: Shape type
  */
{
  ELog::RegMethod RegA("HoleShape","setShape");
      
  if (ST>5)
    throw ColErr::IndexError<size_t>(ST,5,"Shape not defined : ST"); 

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

  const std::string shapeName=
    Control.EvalVar<std::string>(keyName+"Shape");
  setShape(shapeName);

  radialStep=Control.EvalDefVar<double>(keyName+"RadialStep",0.0);

  angleCentre=Control.EvalDefVar<double>(keyName+"AngleCentre",0.0);
  angleOffset=Control.EvalDefVar<double>(keyName+"AngleOffset",0.0);

  radius=Control.EvalVar<double>(keyName+"Radius");
  xradius=Control.EvalDefVar<double>(keyName+"XRadius",radius);

  const double masterAngle=
    Control.EvalDefVar<double>(keyName+"MasterAngle",0.0);
  setMasterAngle(masterAngle);
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

  ModelSupport::buildCylinder(SMap,buildIndex+31,Origin,Y,radius);
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

void
HoleShape::setFaces(const HeadRule& F,const HeadRule& B)
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

  const std::string Out=
    ModelSupport::getComposite(SMap,buildIndex," -31 ");
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
  ModelSupport::buildPlane(SMap,buildIndex+33,Origin-X*radius,X);
  ModelSupport::buildPlane(SMap,buildIndex+34,Origin+X*radius,X);
  ModelSupport::buildPlane(SMap,buildIndex+35,Origin-Z*radius,Z);
  ModelSupport::buildPlane(SMap,buildIndex+36,Origin+Z*radius,Z);
  return;
}

void
HoleShape::createRectangleSurfaces()
  /*!
    Construct inner shape with a square.
    Initially a straight cyclinder cut along the axis
    later will have a cone cut.
  */
{
  ELog::RegMethod RegA("HoleShape","createSquareSurfaces");

  // inner square
  ModelSupport::buildPlane(SMap,buildIndex+33,Origin-X*xradius,X);
  ModelSupport::buildPlane(SMap,buildIndex+34,Origin+X*xradius,X);
  ModelSupport::buildPlane(SMap,buildIndex+35,Origin-Z*radius,Z);
  ModelSupport::buildPlane(SMap,buildIndex+36,Origin+Z*radius,Z);
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
    ModelSupport::getComposite(SMap,buildIndex," 33 -34 35 -36");

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
      ModelSupport::buildPlane(SMap,buildIndex+31+i,
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
    ModelSupport::getComposite(SMap,buildIndex," -31 -32 -33 -34 -35 -36");
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
      ModelSupport::buildPlane(SMap,buildIndex+31+i,Origin+Norm*radius,Norm);
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
    ModelSupport::getComposite(SMap,buildIndex,
				 " -31 -32 -33 -34 -35 -36 -37 -38");

  return Out;
}

std::string
HoleShape::createRectangleObj() 
  /*!
    Create the octagon cutout
    \return the exclusion surface object / set
  */
{
  ELog::RegMethod RegA("HoleShape","createRectangleObj");

  const std::string Out=
    ModelSupport::getComposite(SMap,buildIndex,
				 " 33 -34 35 -36 ");

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
      break;
    case 5:   // Rectangel
      createRectangleSurfaces();
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
    case 5:  // rectangle
      Out=createRectangleObj();
      break;
    default:  // No work
      return;
    }

  System.addCell(MonteCarlo::Object
		 (cellIndex++,0,0.0,Out+frontFace.display()+
		  backFace.display()));
  addCell("Void",cellIndex-1);
  if (cutFlag & 1) Out+=frontFace.display();
  if (cutFlag & 2) Out+=backFace.display();
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
  FixedComp::setConnect(1,Back.first,Y);
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
HoleShape::setCutFaceFlag(const size_t F)
  /*!
    Set the face cut flag
    \param F :: Flag [1 for front / 2 for back /3 both] 
   */
{
  cutFlag=F;
  return;
}
  
void
HoleShape::createAllNoPopulate(Simulation& System,
                               const attachSystem::FixedComp& FC,
                               const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation 
    \param FC :: Fixed component to set axis etc
    \param sideIndex :: side for hole
  */
{
  ELog::RegMethod RegA("HoleShape","createAllNoPopulate");

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);
    
  return;
}

void
HoleShape::createAll(Simulation& System,
                     const attachSystem::FixedComp& FC,
                     const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation 
    \param FC :: Fixed component to set axis etc
    \param sideIndex :: side for hole
  */
{
  ELog::RegMethod RegA("HoleShape","createAllNoPopulate");

  populate(System.getDataBase());
  createAllNoPopulate(System,FC,sideIndex);
  return;
}

  
}  // NAMESPACE constructSystem
