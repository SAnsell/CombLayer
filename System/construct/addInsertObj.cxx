/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/insertPlate.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include "SurInter.h"
#include "insertPlate.h"
#include "addInsertObj.h"

namespace constructSystem
{

void
addInsertPlateCell(Simulation& System,
		   const std::string& objName,
		   const std::string& FCname,
		   const std::string& linkName,
		   const Geometry::Vec3D& XYZStep,
		   const double xSize,const double ySize,
		   const double zSize)
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
  */
{
  ELog::RegMethod RegA("addInsertObj","addTallyCell");
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  const attachSystem::FixedComp* mainFCPtr=
    OR.getObjectThrow<attachSystem::FixedComp>(FCname,"FixedComp");
  
  System.populateCells();
  System.validateObjSurfMap();

  std::shared_ptr<constructSystem::insertPlate>
    TPlate(new constructSystem::insertPlate(objName));

  const long int linkIndex=attachSystem::getLinkIndex(linkName);
  OR.addObject(TPlate);
  TPlate->setStep(XYZStep);
  TPlate->setValues(xSize,ySize,zSize,"Void");
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
		   const double zSize)
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
  */
{
  ELog::RegMethod RegA("addInsertObj","addTallyCell");
  
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  System.populateCells();
  System.validateObjSurfMap();

  std::shared_ptr<constructSystem::insertPlate>
    TPlate(new constructSystem::insertPlate(objName));

  OR.addObject(TPlate);
  TPlate->setValues(xSize,ySize,zSize,"Void");
  TPlate->createAll(System,CentPos,YAxis,ZAxis);

  return;
}
  
}  // NAMESPACE constructSystem
