/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/insertCylinder.cxx
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
#include "insertCylinder.h"

namespace constructSystem
{

insertCylinder::insertCylinder(const std::string& Key)  :
  insertObject(Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

insertCylinder::insertCylinder(const insertCylinder& A) : 
  insertObject(A),
  radius(A.radius),length(A.length)
  /*!
    Copy constructor
    \param A :: insertCylinder to copy
  */
{}

insertCylinder&
insertCylinder::operator=(const insertCylinder& A)
  /*!
    Assignment operator
    \param A :: insertCylinder to copy
    \return *this
  */
{
  if (this!=&A)
    {
      insertObject::operator=(A);
      radius=A.radius;
      length=A.length;
    }
  return *this;
}

insertCylinder::~insertCylinder() 
  /*!
    Destructor
  */
{}

void
insertCylinder::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Data Base
  */
{
  ELog::RegMethod RegA("insertCylinder","populate");
  
  if (!populated)
    {
      insertObject::populate(Control);
      radius=Control.EvalVar<double>(keyName+"Radius");
      length=Control.EvalVar<double>(keyName+"Length");
    }
  return;
}

void
insertCylinder::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int lIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed coordinate system
    \param lIndex :: link index
  */
{
  ELog::RegMethod RegA("insertCylinder","createUnitVector(FC,index)");


  FixedComp::createUnitVector(FC,lIndex);
  applyOffset();
  return;
}

void
insertCylinder::createUnitVector(const Geometry::Vec3D& OG,
			      const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param OG :: Origin
    \param FC :: LinearComponent to attach to
  */
{
  ELog::RegMethod RegA("insertCylinder","createUnitVector");

  FixedComp::createUnitVector(FC);
  Origin=OG;
  applyOffset();
  return;
}

void
insertCylinder::createUnitVector(const Geometry::Vec3D& OG,
                                 const Geometry::Vec3D& Axis)
  /*!
    Create the unit vectors
    \param OG :: Origin
    \param Axis :: Y-direction 
  */
{
  ELog::RegMethod RegA("insertCylinder","createUnitVector<Vec>");
  
  Y=Axis.unit();
  X=Y.crossNormal();
  Z=X*Y;
  Origin=OG;
  applyOffset();

  return;
}

void
insertCylinder::createSurfaces()
  /*!
    Create all the surfaces
  */
{
  ELog::RegMethod RegA("insertCylinder","createSurface");

  if (!frontActive())
    ModelSupport::buildPlane(SMap,ptIndex+1,Origin-Y*length/2.0,Y);
  if (!backActive())
    ModelSupport::buildPlane(SMap,ptIndex+2,Origin+Y*length/2.0,Y);

  ModelSupport::buildCylinder(SMap,ptIndex+7,Origin,Y,radius);


  if (!frontActive())
    setSurf("Front",ptIndex+1);
  else
    setSurf("Front",getFrontRule().getPrimarySurface());

  if (!backActive())
    setSurf("Back",SMap.realSurf(ptIndex+2));
  else
    setSurf("Back",getBackRule().getPrimarySurface());

  setSurf("Radius",SMap.realSurf(ptIndex+7));
  return;
}

void
insertCylinder::createLinks()
  /*!
    Create link points
  */
{
  ELog::RegMethod RegA("insertCylinder","createLinks");

  FrontBackCut::createLinks(*this,Origin,Y);
  if (!frontActive())
    {
      FixedComp::setConnect(0,Origin-Y*(length/2.0),-Y);
      FixedComp::setLinkSurf(0,-SMap.realSurf(ptIndex+1));
    }

  if (!backActive())
    {
      FixedComp::setConnect(1,Origin+Y*(length/2.0),-Y);
      FixedComp::setLinkSurf(1,SMap.realSurf(ptIndex+2));
    }
  
  FixedComp::setConnect(2,Origin-X*radius,-X);
  FixedComp::setConnect(3,Origin+X*radius,X);
  FixedComp::setConnect(4,Origin-Z*radius,-Z);
  FixedComp::setConnect(5,Origin+Z*radius,Z);

  FixedComp::setLinkSurf(2,SMap.realSurf(ptIndex+7));
  FixedComp::setLinkSurf(3,SMap.realSurf(ptIndex+7));
  FixedComp::setLinkSurf(4,SMap.realSurf(ptIndex+7));
  FixedComp::setLinkSurf(5,SMap.realSurf(ptIndex+7));

  // corners 
  FixedComp::setConnect(6,Origin-X*radius-Z*radius,-X-Z);
  FixedComp::setConnect(7,Origin+X*radius-Z*radius,X-Z);
  FixedComp::setConnect(8,Origin-X*radius+Z*radius,-X+Z);
  FixedComp::setConnect(9,Origin+X*radius+Z*radius,X+Z);

  FixedComp::setLinkSurf(6,SMap.realSurf(ptIndex+7));
  FixedComp::setLinkSurf(7,SMap.realSurf(ptIndex+7));
  FixedComp::setLinkSurf(8,SMap.realSurf(ptIndex+7));
  FixedComp::setLinkSurf(9,SMap.realSurf(ptIndex+7));

  FixedComp::addLinkSurf(6,SMap.realSurf(ptIndex+7));
  FixedComp::addLinkSurf(7,SMap.realSurf(ptIndex+7));
  FixedComp::addLinkSurf(8,SMap.realSurf(ptIndex+7));
  FixedComp::addLinkSurf(9,SMap.realSurf(ptIndex+7));

  return;
}

void
insertCylinder::createObjects(Simulation& System)
  /*!
    Create the main volume
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("insertCylinder","createObjects");
  
  std::string Out=
    ModelSupport::getSetComposite(SMap,ptIndex," 1 -2 -7 ");
  Out+=frontRule();
  Out+=backRule();
  System.addCell(MonteCarlo::Qhull(cellIndex++,defMat,0.0,Out));
  addCell("Main",cellIndex-1);
  addOuterSurf(Out);
  return;
}


void
insertCylinder::setValues(const double R,const double L,
                          const int Mat)
  /*!
    Set the values and populate flag
    \param R :: Radius
    \param L :: length
    \param Mat :: Material number
   */
{
  radius=R;
  length=L;
  defMat=Mat;
  populated=1;
  return;
}

void
insertCylinder::setValues(const double R,const double L,
                          const std::string& Mat)
  /*!
    Set the values and populate flag
    \param R :: Radius
    \param L :: length
    \param Mat :: Material name
   */
{
  ELog::RegMethod RegA("insertCylinder","setValues(string)");
  
  ModelSupport::DBMaterial& DB=ModelSupport::DBMaterial::Instance();
  setValues(R,L,DB.processMaterial(Mat));
  return;
}

void
insertCylinder::mainAll(Simulation& System)
  /*!
    Common part to createAll:
    Note: the strnage order -- create links and findObject
    before createObjects. This allows findObjects not to 
    find ourselves (and correctly to find whatever this object
    is in).
    
    \param System :: Simulation
   */
{
  ELog::RegMethod RegA("insertCylinder","mainAll");
  
  createSurfaces();
  createLinks();

  if (!delayInsert)
    findObjects(System);
  createObjects(System);

  insertObjects(System);
  return;
}


void
insertCylinder::createAll(Simulation& System,const Geometry::Vec3D& OG,
                          const attachSystem::FixedComp& FC)
/*!
    Generic function to create everything
    \param System :: Simulation item
    \param OG :: Offset origin							
    \param FC :: Linear component to set axis etc
  */
{
  ELog::RegMethod RegA("insertCylinder","createAll(Vec,FC)");
  if (!populated) 
    populate(System.getDataBase());  
  createUnitVector(OG,FC);
  mainAll(System);
  return;
}

void
insertCylinder::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int lIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Linear component to set axis etc
    \param lIndex :: link Index
  */
{
  ELog::RegMethod RegA("insertCylinder","createAll(FC,index)");
  
  if (!populated) 
    populate(System.getDataBase());  
  createUnitVector(FC,lIndex);
  mainAll(System);
  
  return;
}

void
insertCylinder::createAll(Simulation& System,
                          const Geometry::Vec3D& Orig,
                          const Geometry::Vec3D& Axis)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param Orig :: Origin point 
    \param Axis :: Main axis
  */
{
  ELog::RegMethod RegA("insertCylinder","createAll");
  if (!populated) 
    populate(System.getDataBase());  
  createUnitVector(Orig,Axis);
  mainAll(System);
  
  return;
}
   
}  // NAMESPACE constructSystem
