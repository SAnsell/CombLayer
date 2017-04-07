/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/addInsertObj.cxx
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
#include "LinkSupport.h"
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h"
#include "ContainedComp.h"
#include "FrontBackCut.h"
#include "SurInter.h"
#include "insertObject.h"
#include "insertPlate.h"
#include "insertSphere.h"
#include "insertCylinder.h"
#include "addInsertObj.h"


///\file addInsertObj.cxx

namespace constructSystem
{

void
addInsertCylinderCell(Simulation& System,
                      const std::string& objName,
                      const std::string& FCname,
                      const std::string& linkName,
                      const Geometry::Vec3D& XYZStep,
                      const double radius,const double length,
                      const std::string& mat)
/*!
    Adds a void cell for tallying in the guide if required
    Note his normally leave a "hole" in the guide so 
    it is ideally not used unless absolutely needed.
    
    \param System :: Simulation to used
    \param objName :: object name
    \param FCname :: FixedComp reference name
    \param linkName :: link direction
    \param XYZStep :: Step in xyz direction
    \param length :: full length
    \param radius :: radial size
    \param mat :: Material 
  */
{
  ELog::RegMethod RegA("addInsertObj","addInsertCylinderCell(FC)");
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  const attachSystem::FixedComp* mainFCPtr=
    OR.getObjectThrow<attachSystem::FixedComp>(FCname,"FixedComp");
  const long int linkIndex=attachSystem::getLinkIndex(linkName);
  
  System.populateCells();
  System.validateObjSurfMap();

  std::shared_ptr<constructSystem::insertCylinder>
    TCyl(new constructSystem::insertCylinder(objName));

  OR.addObject(TCyl);
  TCyl->setStep(XYZStep);
  TCyl->setValues(radius,length,mat);
  TCyl->createAll(System,*mainFCPtr,linkIndex);

  return;
}

void
addInsertCylinderCell(Simulation& System,
                      const std::string& objName,
                      const Geometry::Vec3D& CentPos,
                      const Geometry::Vec3D& YAxis,
                      const double radius,
                      const double length,
                      const std::string& mat)
  /*!
    Adds a cell for tallying in 
    
    \param System :: Simulation to used
    \param objName :: new plate name
    \param CentPos :: Central positoin
    \param YAxis :: Direction along Y
    \param radius :: radius
    \param length :: length of object
    \param mat :: material
  */
{
  ELog::RegMethod RegA("addInsertObj[F]","addInsertCylinderCell");
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  System.populateCells();
  System.validateObjSurfMap();

  std::shared_ptr<constructSystem::insertCylinder>
    TCyl(new constructSystem::insertCylinder(objName));

  OR.addObject(TCyl);
  TCyl->setValues(radius,length,mat);

  TCyl->createAll(System,CentPos,YAxis);

  return;
}

  
void
addInsertPlateCell(Simulation& System,
		   const std::string& objName,
		   const std::string& FCname,
		   const std::string& linkName,
		   const Geometry::Vec3D& XYZStep,
		   const double xSize,const double ySize,
		   const double zSize,const std::string& mat)
  /*!
    Adds a void cell for tallying in the guide if required
    Note his normally leave a "hole" in the guide so 
    it is ideally not used unless absolutely needed.
    
    \param System :: Simulation to used
    \param objName :: object name
    \param FCname :: FixedComp reference name
    \param linkName :: link direction
    \param XYZStep :: Step in xyz direction
    \param xSize :: x-size
    \param ySize :: y-size
    \param zSize :: zSize
    \param mat :: Material 
  */
{
  ELog::RegMethod RegA("addInsertObj","addInsertPlateCell(FC)");
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  const attachSystem::FixedComp* mainFCPtr=
    OR.getObjectThrow<attachSystem::FixedComp>(FCname,"FixedComp");
  const long int linkIndex=attachSystem::getLinkIndex(linkName);
  
  System.populateCells();
  System.validateObjSurfMap();

  std::shared_ptr<constructSystem::insertPlate>
    TPlate(new constructSystem::insertPlate(objName));

  OR.addObject(TPlate);
  TPlate->setStep(XYZStep);
  TPlate->setValues(xSize,ySize,zSize,mat);
  TPlate->createAll(System,*mainFCPtr,linkIndex);

  return;
}

void
addInsertPlateCell(Simulation& System,
		   const std::string& objName,
		   const Geometry::Vec3D& CentPos,
                   const Geometry::Vec3D& YAxis,
                   const Geometry::Vec3D& ZAxis,
		   const double xSize,
                   const double ySize,
		   const double zSize,
		   const std::string& mat)
  /*!
    Adds a void cell for tallying in the guide if required
    Note his normally leave a "hole" in the guide so 
    it is ideally not used unless absolutely needed.
    
    \param System :: Simulation to used
    \param objName :: new plate name
    \param CentPos :: Central positoin
    \param YAxis :: Direction along Y
    \param ZAxis :: Direction along Z
    \param xSize :: x-size
    \param ySize :: y-size
    \param zSize :: zSize
    \param mat :: material
  */
{
  ELog::RegMethod RegA("addInsertObj[F]","addInsertPlateCell");
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  System.populateCells();
  System.validateObjSurfMap();

  std::shared_ptr<constructSystem::insertPlate>
    TPlate(new constructSystem::insertPlate(objName));

  OR.addObject(TPlate);
  TPlate->setValues(xSize,ySize,zSize,mat);

  TPlate->createAll(System,CentPos,YAxis,ZAxis);

  return;
}

void
addInsertSphereCell(Simulation& System,
		    const std::string& objName,
		    const Geometry::Vec3D& CentPos,
		    const double radius,
		    const std::string& mat)
  /*!
    Adds a void cell for tallying in the guide if required
    Note his normally leave a "hole" in the guide so 
    it is ideally not used unless absolutely needed.
    
    \param System :: Simulation to used
    \param objName :: new plate name
    \param CentPos :: Central Position
    \param raduis :: radius of phere
    \param mat :: material
  */
{
  ELog::RegMethod RegA("addInsertObj[F]","addInsertSphereCell");
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  System.populateCells();
  System.validateObjSurfMap();

  std::shared_ptr<constructSystem::insertSphere>
    TSphere(new constructSystem::insertSphere(objName));

  OR.addObject(TSphere);
  TSphere->setValues(radius,mat);
  TSphere->createAll(System,CentPos);

  return;
}


void
addInsertSphereCell(Simulation& System,
		    const std::string& objName,
		    const std::string& FCname,
		    const std::string& linkName,
		    const Geometry::Vec3D& XYZStep,
		    const double radius,
		    const std::string& mat)
  /*!
    Adds a void cell for tallying in the guide if required
    Note his normally leave a "hole" in the guide so 
    it is ideally not used unless absolutely needed.
    
    \param System :: Simulation to used
    \param objName :: new plate name
    const std::string& FCname,
    const std::string& linkName,
    \param XYZStep :: Central Offset
    \param raduis :: radius of phere
    \param mat :: material
  */
{
  ELog::RegMethod RegA("addInsertObj[F]","addInsertSphereCell(FC)");
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  const attachSystem::FixedComp* mainFCPtr=
    OR.getObjectThrow<attachSystem::FixedComp>(FCname,"FixedComp");
  const long int linkIndex=attachSystem::getLinkIndex(linkName);
  
  System.populateCells();
  System.validateObjSurfMap();

  std::shared_ptr<constructSystem::insertSphere>
    TSphere(new constructSystem::insertSphere(objName));

  OR.addObject(TSphere);
  TSphere->setStep(XYZStep);
  TSphere->setValues(radius,mat);
  TSphere->createAll(System,*mainFCPtr,linkIndex);

  return;
}


  
}  // NAMESPACE constructSystem
