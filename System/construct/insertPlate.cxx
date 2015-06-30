/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/insertPlate.cxx
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
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "insertPlate.h"

namespace ModelSupport
{

insertPlate::insertPlate(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,6),
  ptIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(ptIndex+31),populated(0),
  zAngle(0.0),xyAngle(0.0),defMat(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

insertPlate::insertPlate(const insertPlate& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  ptIndex(A.ptIndex),cellIndex(A.cellIndex),
  populated(A.populated),zAngle(A.zAngle),xyAngle(A.xyAngle),
  width(A.width),height(A.height),depth(A.depth),defMat(A.defMat)
  /*!
    Copy constructor
    \param A :: insertPlate to copy
  */
{}

insertPlate&
insertPlate::operator=(const insertPlate& A)
  /*!
    Assignment operator
    \param A :: insertPlate to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      populated=A.populated;
      zAngle=A.zAngle;
      xyAngle=A.xyAngle;
      width=A.width;
      height=A.height;
      depth=A.depth;
      defMat=A.defMat;
    }
  return *this;
}

insertPlate::~insertPlate() 
  /*!
    Destructor
  */
{}

void
insertPlate::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Data Basex
  */
{
  ELog::RegMethod RegA("insertPlate","populate");
  
  zAngle=Control.EvalVar<double>(keyName+"ZAngle");
  xyAngle=Control.EvalVar<double>(keyName+"XYAngle");
  
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  defMat=ModelSupport::EvalMat<int>(Control,keyName+"DefMat");
  populated=1;
  return;
}

void
insertPlate::createUnitVector(const Geometry::Vec3D& OG,
			      const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param OG :: Origin
    \param LC :: LinearComponent to attach to
  */
{
  ELog::RegMethod RegA("insertPlate","createUnitVector");


  FixedComp::createUnitVector(FC);
  createUnitVector(OG,X,Y,Z);
  return;
}

void
insertPlate::createUnitVector(const Geometry::Vec3D& OG,
			      const Geometry::Vec3D& XUnit,
			      const Geometry::Vec3D& YUnit,
			      const Geometry::Vec3D& ZUnit)
  /*!
    Create the unit vectors
    \param OG :: Origin
    \param XUnit :: Xdirection
    \param YUnit :: Xdirection
    \param ZUnit :: Xdirection
  */
{
  ELog::RegMethod RegA("insertPlate","createUnitVector<Vec>");

  X=XUnit.unit();
  Y=YUnit.unit();
  Z=ZUnit.unit();
  
  const Geometry::Quaternion Qz=
    Geometry::Quaternion::calcQRotDeg(zAngle,X);
  const Geometry::Quaternion Qxy=
    Geometry::Quaternion::calcQRotDeg(xyAngle,Z);
  Qz.rotate(Y);
  Qz.rotate(Z);
  Qxy.rotate(Y);
  Qxy.rotate(X);
  Qxy.rotate(Z);

  Origin=OG;

  return;
}

void
insertPlate::createSurfaces()
  /*!
    Create all the surfaces
  */
{
  ELog::RegMethod RegA("insertPlate","createSurface");

  ModelSupport::buildPlane(SMap,ptIndex+1,Origin-Y*depth/2.0,Y);
  ModelSupport::buildPlane(SMap,ptIndex+2,Origin+Y*depth/2.0,Y);
  ModelSupport::buildPlane(SMap,ptIndex+3,Origin-X*width/2.0,X);
  ModelSupport::buildPlane(SMap,ptIndex+4,Origin+X*width/2.0,X);
  ModelSupport::buildPlane(SMap,ptIndex+5,Origin-Z*height/2.0,Z);
  ModelSupport::buildPlane(SMap,ptIndex+6,Origin+Z*height/2.0,Z);

  return;
}

void
insertPlate::createLinks()
  /*!
    Create link pointsx
   */
{
  ELog::RegMethod RegA("insertPlate","createLinks");

  const double T[3]={depth,width,height};
  const Geometry::Vec3D Dir[3]={Y,X,Z};

  for(size_t i=0;i<6;i++)
    {
      const double SN((i%2) ? 1.0 : -1.0);
      FixedComp::setConnect(i,Origin+Dir[i/2]*T[i/2],Dir[i/2]*SN);
      FixedComp::setLinkSurf(i,SMap.realSurf(ptIndex+1+static_cast<int>(i)));
    }
  return;
}

void
insertPlate::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("insertPlate","createObjects");
  
  std::string Out=
    ModelSupport::getComposite(SMap,ptIndex,"1 -2 3 -4 5 -6");
  addOuterSurf(Out);
  if (defMat<0)
    System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out)); 
  else
    System.addCell(MonteCarlo::Qhull(cellIndex++,defMat,0.0,Out)); 
  return;
}

void
insertPlate::findObjects(const Simulation& System)
  /*!
    Insert the objects into the main simulation. It is separated
    from creation since we need to determine those object that 
    need to have an exclude item added to them.
    \param System :: Simulation to add object to
  */
{
  ELog::RegMethod RegA("insertPlate","findObjects");

  std::set<int> ICells;
  // Process all the corners
  MonteCarlo::Object* OPtr(System.findCell(Origin,0));
  if (OPtr)
    ICells.insert(OPtr->getName());
  for(int i=0;i<8;i++)
    {
      const double mX((i%2) ? -1.0 : 1.0);
      const double mY(((i>>1)%2) ? -1.0 : 1.0);
      const double mZ(((i>>2)%2) ? -1.0 : 1.0);

      Geometry::Vec3D TP(Origin);
      TP+=X*(mX*depth/2.0);
      TP+=Y*(mY*depth/2.0);
      TP+=Z*(mZ*depth/2.0);
      OPtr=System.findCell(TP,OPtr);
      if (OPtr)
	ICells.insert(OPtr->getName());
    }

  for(const int IC : ICells)
    attachSystem::ContainedComp::addInsertCell(IC);

  return;
}

void
insertPlate::setValues(const double XS,const double YS,
		       const double ZS,const int Mat)
  /*!
    Set the values and populate flag
    \param XS :: X-size [width]
    \param YS :: Y-size [depth] 
    \param ZS :: Z-size [height]
    \param Mat :: Material number
   */
{
  width=XS;
  depth=YS;
  height=ZS;
  defMat=Mat;
  populated=1;
  return;
}

void
insertPlate::mainAll(Simulation& System)
  /*!
    Common part to createAll
    \param System :: Simulation
   */
{
  ELog::RegMethod RegA("insertPlate","mainAll");
  
  createSurfaces();
  createObjects(System);
  createLinks();
  findObjects(System);
  insertObjects(System);
  return;
}


void
insertPlate::createAll(Simulation& System,const Geometry::Vec3D& OG,
		       const attachSystem::FixedComp& FC)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Linear component to set axis etc
  */
{
  ELog::RegMethod RegA("insertPlate","createAll");
  if (!populated) 
    populate(System.getDataBase());  
  createUnitVector(OG,FC);
  mainAll(System);
  return;
}
  

void
insertPlate::createAll(Simulation& System,const Geometry::Vec3D& OG,
		       const Geometry::Vec3D& Xunit,
		       const Geometry::Vec3D& Yunit,
		       const Geometry::Vec3D& Zunit)

  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param OG :: Origin
    \param XUnit :: X-direction
    \param YUnit :: Y-direction
    \param ZUnit :: Z-direction
  */
{
  ELog::RegMethod RegA("insertPlate","createAll<vec>");
  
  if (!populated)
    populate(System.getDataBase());  
  createUnitVector(OG,Xunit,Yunit,Zunit);
  mainAll(System);

  return;
}
  
}  // NAMESPACE shutterSystem
