/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/insertObject.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "LineIntersectVisit.h"
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
#include "Zaid.h"
#include "MXcards.h"
#include "Material.h"
#include "DBMaterial.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h"
#include "ContainedComp.h"
#include "FrontBackCut.h"
#include "SurInter.h"
#include "AttachSupport.h"
#include "insertObject.h"

namespace insertSystem
{

insertObject::insertObject(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedOffset(Key,6),
  attachSystem::CellMap(),attachSystem::SurfMap(),
  attachSystem::FrontBackCut(),
  ptIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(ptIndex+1),populated(0),defMat(0),delayInsert(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

insertObject::insertObject(const insertObject& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  attachSystem::CellMap(A),attachSystem::SurfMap(A),
  attachSystem::FrontBackCut(A),
  ptIndex(A.ptIndex),cellIndex(A.cellIndex),
  populated(A.populated),defMat(A.defMat),
  delayInsert(A.delayInsert)
  /*!
    Copy constructor
    \param A :: insertObject to copy
  */
{}

insertObject&
insertObject::operator=(const insertObject& A)
  /*!
    Assignment operator
    \param A :: insertObject to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      cellIndex=A.cellIndex;
      populated=A.populated;
      defMat=A.defMat;
      delayInsert=A.delayInsert;
    }
  return *this;
}

insertObject::~insertObject() 
  /*!
    Destructor
  */
{}

void
insertObject::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Data Base
  */
{
  ELog::RegMethod RegA("insertObject","populate");
  
  if (!populated)
    {
      FixedOffset::populate(Control);      
      defMat=ModelSupport::EvalMat<int>(Control,keyName+"DefMat");
      populated=1;
    }
  return;
}

void
insertObject::createUnitVector(const attachSystem::FixedComp& FC,
			       const long int lIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed coordinate system
    \param lIndex :: link index
  */
{
  ELog::RegMethod RegA("insertObject","createUnitVector(FC,index)");


  FixedComp::createUnitVector(FC,lIndex);
  applyOffset();
  return;
}

void
insertObject::createUnitVector(const Geometry::Vec3D& OG,
			       const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param OG :: Origin
    \param FC :: LinearComponent to attach to
  */
{
  ELog::RegMethod RegA("insertObject","createUnitVector");

  FixedComp::createUnitVector(FC);
  Origin=OG;
  applyOffset();
  return;
}

void
insertObject::createUnitVector(const Geometry::Vec3D& OG,
			       const Geometry::Vec3D& Axis)
  /*!
    Create the unit vectors
    \param OG :: Origin
    \param Axis :: Y-direction 
  */
{
  ELog::RegMethod RegA("insertObject","createUnitVector<Vec,Vec>");
  
  Y=Axis.unit();
  X=Y.crossNormal();
  Z=X*Y;
  createUnitVector(OG,Axis,Z);
  return;
}

  
void
insertObject::createUnitVector(const Geometry::Vec3D& OG,
			       const Geometry::Vec3D& YUnit,
			       const Geometry::Vec3D& ZUnit)
  /*!
    Create the unit vectors
    \param OG :: Origin
    \param YUnit :: Y-direction
    \param ZUnit :: Z-direction
  */
{
  ELog::RegMethod RegA("insertObject","createUnitVector<Vec>");


  Geometry::Vec3D xTest(YUnit.unit()*ZUnit.unit());
  Geometry::Vec3D yTest(YUnit.unit());
  Geometry::Vec3D zTest(ZUnit.unit());
  FixedComp::computeZOffPlane(xTest,yTest,zTest);

  FixedComp::createUnitVector(OG,yTest*zTest,yTest,zTest);
  Origin=OG;
  applyOffset();

  return;
}
  
void
insertObject::findObjects(Simulation& System)
  /*!
    Insert the objects into the main simulation. It is separated
    from creation since we need to determine those object that 
    need to have an exclude item added to them.
    \param System :: Simulation to add object to
  */
{
  ELog::RegMethod RegA("insertObject","findObjects");

  typedef std::map<int,MonteCarlo::Object*> MTYPE;
  
  System.populateCells();
  System.validateObjSurfMap();

  MTYPE OMap;
  attachSystem::lineIntersect(System,*this,OMap);

  // Add exclude string
  MTYPE::const_iterator ac;
  for(ac=OMap.begin();ac!=OMap.end();ac++)
    attachSystem::ContainedComp::addInsertCell(ac->first);
  
  
  return;
}

void
insertObject::setStep(const double XS,const double YS,
		     const double ZS)
  /*!
    Set the values but NOT the populate flag
    \param XS :: X-step [width]
    \param YS :: Y-step [depth] 
    \param ZS :: Z-step [height]
   */
{
  xStep=XS;
  yStep=YS;
  zStep=ZS;
  return;
}

void
insertObject::setStep(const Geometry::Vec3D& XYZ)
  /*!
    Set the values but NOT the populate flag
    \param XYZ :: X/Y/Z
   */
{
  xStep=XYZ[0];
  yStep=XYZ[1];
  zStep=XYZ[2];
  return;
}

void
insertObject::setAngles(const double XS,const double ZA)
  /*!
    Set the values but NOT the populate flag
    \param XY :: XY angle
    \param ZA :: Z angle
   */
{
  xyAngle=XS;
  zAngle=ZA;
  return;
}

 
  
}  // NAMESPACE insertSystem
