/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   insertUnit/addInsertObj.cxx
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
#include "insertGrid.h"
#include "insertCurve.h"
#include "addInsertObj.h"


///\file addInsertObj.cxx

namespace constructSystem
{

void
addInsertCurveCell(Simulation& System,
                   const std::string& objName,
                   const Geometry::Vec3D& APt,
                   const Geometry::Vec3D& BPt,
                   const int yFlag,
                   const Geometry::Vec3D& ZAxis,
		   const double radius,
                   const double width,
		   const double height,
		   const std::string& mat)
  /*!
    Adds a void cell for tallying in the guide if required
    Note his normally leave a "hole" in the guide so 
    it is ideally not used unless absolutely needed.
    
    \param System :: Simulation to used
    \param objName :: object name
    \param APt :: Begin point
    \param BPt :: End point
    \param yFlag :: y rotation +/-
    \param ZAxis :: Axis out of plane
    \param radius :: radial size
    \param width :: Full gap
    \param height :: Full height
    \param mat :: Material 
  */
{
  ELog::RegMethod RegA("addInsertObj","addInsertCurveCell(FC)");
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  System.populateCells();
  System.validateObjSurfMap();

  std::shared_ptr<insertSystem::insertCurve>
    TCurve(new insertSystem::insertCurve(objName));

  OR.addObject(TCurve);
 
  // calc proper length
  const Geometry::Vec3D YDir((BPt-APt).unit());
  const Geometry::Vec3D XDir(ZAxis*YDir*yFlag);
  const double Dist=APt.Distance(BPt)/2.0;
  const double hplus=radius-sqrt(radius*radius-Dist*Dist);
  const Geometry::Vec3D CentPos=XDir*hplus+(BPt+APt)/2.0;
  
  const double theta=asin(Dist/radius);
  const double length=2*radius*theta;

  TCurve->setValues(radius,length,width,height,mat);
  TCurve->createAll(System,CentPos,XDir,ZAxis);

  return;
}

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
    \param radius :: radial size
    \param length :: full height
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

  std::shared_ptr<insertSystem::insertCylinder>
    TCyl(new insertSystem::insertCylinder(objName));

  OR.addObject(TCyl);
  TCyl->setStep(XYZStep);
  TCyl->setValues(radius,length,mat);
  TCyl->createAll(System,*mainFCPtr,linkIndex);
  ELog::EM<<"Cylinder["<<TCyl->getKeyName()<<"] at "<<
    TCyl->getCentre()<<ELog::endDiag;
  return;
}

void
addInsertCylinderCell(Simulation& System,
                      const std::string& objName,
                      const Geometry::Vec3D& CentPos,
                      const Geometry::Vec3D& YAxis,
                      const double radius,const double length,
                      const std::string& mat)
  /*!
    Adds a cell for tallying in 
    
    \param System :: Simulation to used
    \param objName :: new plate name
    \param CentPos :: Central positoin
    \param YAxis :: Direction along Y
    \param radius :: radius
    \param length :: length of cylinder
    \param mat :: material
  */
{
  ELog::RegMethod RegA("addInsertObj[F]","addInsertCylinderCell");
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  System.populateCells();
  System.validateObjSurfMap();

  std::shared_ptr<insertSystem::insertCylinder>
    TCyl(new insertSystem::insertCylinder(objName));

  OR.addObject(TCyl);
  TCyl->setValues(radius,length,mat);

  TCyl->createAll(System,CentPos,YAxis);

  return;
}

  
void
addInsertPlateCell(Simulation& System,const std::string& objName,
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

  std::shared_ptr<insertSystem::insertPlate>
    TPlate(new insertSystem::insertPlate(objName));

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

  std::shared_ptr<insertSystem::insertPlate>
    TPlate(new insertSystem::insertPlate(objName));

  OR.addObject(TPlate);
  TPlate->setValues(xSize,ySize,zSize,mat);

  TPlate->createAll(System,CentPos,YAxis,ZAxis);

  return;
}

void
addInsertGridCell(Simulation& System,
		  const std::string& objName,
		  const std::string& FCname,
		  const std::string& linkName,
		  const Geometry::Vec3D& XYZStep,
		  const size_t NL,
		  const double length,
		  const double thick,
		  const double gap,
		  const std::string& layerMat)
  /*!
    Adds a void cell for tallying in the guide if required
    Note his normally leave a "hole" in the guide so 
    it is ideally not used unless absolutely needed.
    
    \param System :: Simulation to used
    \param objName :: object name
    \param FCname :: FixedComp reference name
    \param linkName :: link direction
    \param XYZStep :: Extent of cell in X,Y,Z direction
    \param NL :: Number of layers
    \param length :: primary length
    \param thick :: thickness of layer
    \param gap :: gap layer (and 2xmid)
    \param layerMat :: Material 
  */
{
  ELog::RegMethod RegA("addInsertObj","addInsertGridCell(FC)");
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  const attachSystem::FixedComp* mainFCPtr=
    OR.getObjectThrow<attachSystem::FixedComp>(FCname,"FixedComp");
  const long int linkIndex=attachSystem::getLinkIndex(linkName);
  
  System.populateCells();
  System.validateObjSurfMap();

  std::shared_ptr<insertSystem::insertGrid>
    TGrid(new insertSystem::insertGrid(objName));

  OR.addObject(TGrid);
  TGrid->setStep(XYZStep);
  TGrid->setValues(gap*2.0,length,gap*2.0,layerMat);
  for(size_t i=0;i<NL;i++)
    {
      if (i % 2)
	TGrid->addLayer(gap,"Void");	  
      else
	TGrid->addLayer(thick,layerMat);
    }
  
  TGrid->createAll(System,*mainFCPtr,linkIndex);

  return;
}

void
addInsertGridCell(Simulation& System,
		  const std::string& objName,
		  const Geometry::Vec3D& Origin,
		  const Geometry::Vec3D& YAxis,
		  const Geometry::Vec3D& ZAxis,
		  const size_t NL,
		  const double length,
		  const double thick,
		  const double gap,
		  const std::string& layerMat)
  /*!
    Adds a void cell for tallying in the guide if required
    Note his normally leave a "hole" in the guide so 
    it is ideally not used unless absolutely needed.
    
    \param System :: Simulation to used
    \param objName :: object name
    \param Origin :: Centre of grid
    \param YAxis :: Main Y axis
    \param ZAxis :: Z axis [Orthoganalized if not]
    \param NL :: Number of layers
    \param length :: primary length
    \param thick :: thickness of layer
    \param gap :: gap layer (and 2xmid)
    \param layerMat :: Material 
  */
{
  ELog::RegMethod RegA("addInsertObj","addInsertGridCell(Vec)");
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  
  System.populateCells();
  System.validateObjSurfMap();

  std::shared_ptr<insertSystem::insertGrid>
    TGrid(new insertSystem::insertGrid(objName));

  OR.addObject(TGrid);
  TGrid->setValues(gap*2.0,length,gap*2.0,layerMat);
  for(size_t i=0;i<NL;i++)
    {
      if (i % 2)
	TGrid->addLayer(gap,"Void");	  
      else
	TGrid->addLayer(thick,layerMat);
    }
  
  TGrid->createAll(System,Origin,YAxis,ZAxis);

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
    \param radius :: radius of phere
    \param mat :: material
  */
{
  ELog::RegMethod RegA("addInsertObj[F]","addInsertSphereCell");
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  System.populateCells();
  System.validateObjSurfMap();

  std::shared_ptr<insertSystem::insertSphere>
    TSphere(new insertSystem::insertSphere(objName));

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
    \param FCname :: Fixedobject naem
    \param linkName :: Link point / link name
    \param XYZStep :: Central Offset
    \param radius :: radius of phere
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

  std::shared_ptr<insertSystem::insertSphere>
    TSphere(new insertSystem::insertSphere(objName));

  OR.addObject(TSphere);
  TSphere->setStep(XYZStep);
  TSphere->setValues(radius,mat);
  TSphere->createAll(System,*mainFCPtr,linkIndex);

  return;
}


  
}  // NAMESPACE insertSystem
