/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   insertUnit/insertPencil.cxx
 *
 * Copyright (c) 2004-2025 by Stuart Ansell and Konstantin Batkov
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "DBMaterial.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "insertObject.h"
#include "insertPencil.h"

namespace insertSystem
{

insertPencil::insertPencil(const std::string& Key)  :
  insertObject(Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

insertPencil::insertPencil(const insertPencil& A) :
  insertObject(A),
  radius(A.radius),length(A.length)
  /*!
    Copy constructor
    \param A :: insertPencil to copy
  */
{}

insertPencil&
insertPencil::operator=(const insertPencil& A)
  /*!
    Assignment operator
    \param A :: insertPencil to copy
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

insertPencil::~insertPencil()
  /*!
    Destructor
  */
{}

void
insertPencil::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Data Base
  */
{
  ELog::RegMethod RegA("insertPencil","populate");

  if (!populated)
    {
      insertObject::populate(Control);
      radius=Control.EvalVar<double>(keyName+"Radius");
      length=Control.EvalVar<double>(keyName+"Length");
      angle=Control.EvalVar<double>(keyName+"Angle");
    }
  return;
}

void
insertPencil::createSurfaces()
  /*!
    Create all the surfaces
  */
{
  ELog::RegMethod RegA("insertPencil","createSurface");

  const double tipLength = radius / tan(angle*M_PI/360.0);

  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0+tipLength),Y);
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length/2.0,Y);
      ExternalCut::setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }

  ModelSupport::buildPlane(SMap,buildIndex+11,Origin-Y*(length/2.0),Y);

  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);
  ModelSupport::buildCone(SMap,buildIndex+8,Origin-Y*(length/2.0+tipLength),Y,angle/2.0);

  setSurf("Front",getFrontRule().getPrimarySurface());
  setSurf("Back",getBackRule().getPrimarySurface());

  setSurf("Radius",SMap.realSurf(buildIndex+7));
  return;
}

void
insertPencil::createLinks()
  /*!
    Create link points
  */
{
  ELog::RegMethod RegA("insertPencil","createLinks");

  FixedComp::setNConnect(10);
  FrontBackCut::createLinks(*this,Origin,Y);

  FixedComp::setConnect(2,Origin-X*radius,-X);
  FixedComp::setConnect(3,Origin+X*radius,X);
  FixedComp::setConnect(4,Origin-Z*radius,-Z);
  FixedComp::setConnect(5,Origin+Z*radius,Z);

  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(4,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+7));

  // corners
  const double r2=sqrt(radius);
  FixedComp::setConnect(6,Origin-X*r2-Z*r2,-X-Z);
  FixedComp::setConnect(7,Origin+X*r2-Z*r2,X-Z);
  FixedComp::setConnect(8,Origin-X*r2+Z*r2,-X+Z);
  FixedComp::setConnect(9,Origin+X*r2+Z*r2,X+Z);

  FixedComp::setLinkSurf(6,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(7,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(8,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(9,SMap.realSurf(buildIndex+7));

  FixedComp::addLinkSurf(6,SMap.realSurf(buildIndex+7));
  FixedComp::addLinkSurf(7,SMap.realSurf(buildIndex+7));
  FixedComp::addLinkSurf(8,SMap.realSurf(buildIndex+7));
  FixedComp::addLinkSurf(9,SMap.realSurf(buildIndex+7));

  return;
}

void
insertPencil::createObjects(Simulation& System)
  /*!
    Create the main volume
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("insertPencil","createObjects");

  HeadRule HR;

  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"-7 -8 -11");
  HR*=getFrontRule();
  makeCell("Tip",System,cellIndex++,defMat,0.0,HR);

  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"-7 8 -11");
  HR*=getFrontRule();
  makeCell("TipVoid",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"-7 11");
  HR*=getBackRule();
  makeCell("Main",System,cellIndex++,defMat,0.0,HR);

  HR=ModelSupport::getSetHeadRule(SMap,buildIndex,"-7");
  HR*=getFrontRule();
  HR*=getBackRule();
  addOuterSurf(HR);

  return;
}


void
insertPencil::setValues(const double R,const double L,const double A,const int Mat, const int VM)
  /*!
    Set the values and populate flag
    \param R :: Radius
    \param L :: length
    \param Mat :: Material number
   */
{
  radius=R;
  length=L;
  angle=A,
  defMat=Mat;
  voidMat=VM;
  populated=1;
  return;
}

void
insertPencil::setValues(const double R,const double L,const double A,
			const std::string& Mat, const std::string& VM)
  /*!
    Set the values and populate flag
    \param R :: Radius
    \param L :: length
    \param Mat :: Material name
   */
{
  ELog::RegMethod RegA("insertPencil","setValues(string)");

  ModelSupport::DBMaterial& DB=ModelSupport::DBMaterial::Instance();
  setValues(R,L,A,DB.processMaterial(Mat),DB.processMaterial(VM));
  return;
}

void
insertPencil::mainAll(Simulation& System)
  /*!
    Common part to createAll:
    Note: the strnage order -- create links and findObject
    before createObjects. This allows findObjects not to
    find ourselves (and correctly to find whatever this object
    is in).

    \param System :: Simulation
   */
{
  ELog::RegMethod RegA("insertPencil","mainAll");

  createSurfaces();
  createLinks();

  if (!delayInsert)
    findObjects(System);
  createObjects(System);

  insertObjects(System);
  return;
}


void
insertPencil::createAll(Simulation& System,
			  const Geometry::Vec3D& OG,
                          const attachSystem::FixedComp& FC)
/*!
    Generic function to create everything
    \param System :: Simulation item
    \param OG :: Offset origin
    \param FC :: Linear component to set axis etc
  */
{
  ELog::RegMethod RegA("insertPencil","createAll(Vec,FC)");
  if (!populated)
    populate(System.getDataBase());
  createUnitVector(FC,OG);
  mainAll(System);
  return;
}

void
insertPencil::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int lIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Linear component to set axis etc
    \param lIndex :: link Index
  */
{
  ELog::RegMethod RegA("insertPencil","createAll(FC,index)");

  if (!populated)
    populate(System.getDataBase());
  createUnitVector(FC,lIndex);
  mainAll(System);

  return;
}

void
insertPencil::createAll(Simulation& System,
                          const Geometry::Vec3D& Orig,
                          const Geometry::Vec3D& Axis)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param Orig :: Origin point
    \param Axis :: Main axis
  */
{
  ELog::RegMethod RegA("insertPencil","createAll");
  if (!populated)
    populate(System.getDataBase());
  insertObject::createUnitVector(Orig,Axis);
  mainAll(System);

  return;
}

}  // NAMESPACE insertSystem
