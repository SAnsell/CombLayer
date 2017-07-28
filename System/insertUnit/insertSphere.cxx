/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   insertUnit/insertSphere.cxx
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
#include <array>

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
#include "FrontBackCut.h"
#include "ContainedComp.h"
#include "World.h"
#include "insertObject.h"
#include "insertSphere.h"

namespace insertSystem
{

insertSphere::insertSphere(const std::string& Key)  :
  insertObject(Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

insertSphere::insertSphere(const insertSphere& A) :
  insertObject(A),radius(A.radius)
  /*!
    Copy constructor
    \param A :: insertSphere to copy
  */
{}

insertSphere&
insertSphere::operator=(const insertSphere& A)
  /*!
    Assignment operator
    \param A :: insertSphere to copy
    \return *this
  */
{
  if (this!=&A)
    {
      insertObject::operator=(A);
      radius=A.radius;
    }
  return *this;
}


insertSphere::~insertSphere() 
  /*!
    Destructor
  */
{}

void
insertSphere::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Data Base
  */
{
  ELog::RegMethod RegA("insertSphere","populate");
  
  if (!populated)
    {
      insertObject::populate(Control);
      radius=Control.EvalVar<double>(keyName+"Radius");
    }
  return;
}

void
insertSphere::createSurfaces()
  /*!
    Create all the surfaces
  */
{
  ELog::RegMethod RegA("insertSphere","createSurface");

  ModelSupport::buildSphere(SMap,ptIndex+7,Origin,radius);

  setSurf("Surf",ptIndex+1);
  return;
}

void
insertSphere::createLinks()
  /*!
    Create link points
  */
{
  ELog::RegMethod RegA("insertSphere","createLinks");

  FixedComp::setNConnect(6);
  const Geometry::Vec3D Dir[3]={Y,X,Z};

  for(size_t i=0;i<6;i++)
    {
      const double SN((i%2) ? 1.0 : -1.0);
      FixedComp::setConnect(i,Origin+Dir[i/2]*radius,Dir[i/2]*SN);
      FixedComp::setLinkSurf(i,SMap.realSurf(ptIndex+7));
    }
  return;
}

void
insertSphere::createObjects(Simulation& System)
  /*!
    Create the spherical volume
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("insertSphere","createObjects");
  
  std::string Out=
    ModelSupport::getComposite(SMap,ptIndex," -7 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,defMat,0.0,Out));
  addCell("Main",cellIndex-1);
  addOuterSurf(Out);
  return;
}

void
insertSphere::findObjects(Simulation& System)
  /*!
    Insert the objects into the main simulation. It is separated
    from creation since we need to determine those object that 
    need to have an exclude item added to them.
    \param System :: Simulation to add object to
  */
{
  ELog::RegMethod RegA("insertSphere","findObjects");

  System.populateCells();
  System.validateObjSurfMap();

  if (getInsertCells().empty())
    {
  
      std::set<int> ICells;
      // Process all the corners
      MonteCarlo::Object* OPtr(System.findCell(Origin,0));
      if (OPtr) ICells.insert(OPtr->getName());

      // normal directions
      const std::array<Geometry::Vec3D,6> XYZ=
	{ { -X,-Y,-Z,X,Y,Z } }; 
      for(size_t i=0;i<6;i++)
        {          
          Geometry::Vec3D TP(Origin);
	  TP+=XYZ[i]*radius;
          OPtr=System.findCell(TP,OPtr);
          if (OPtr)
            ICells.insert(OPtr->getName());
        }
      // sqrt directions // and normal directions
      const double RR=radius/sqrt(2);
      for(size_t i=0;i<6;i++)
	for(size_t j=i;j<6;j++)
	  {
	    if (i!=((j+3) % 6))
	      {
		Geometry::Vec3D TP(Origin);
		TP+=(XYZ[i]+XYZ[j])*RR;
		OPtr=System.findCell(TP,OPtr);
		if (OPtr)
		  ICells.insert(OPtr->getName());
	      }
	  }
      for(const int IC : ICells)
        attachSystem::ContainedComp::addInsertCell(IC);
    }
  
  return;
}

void
insertSphere::setValues(const double R,const int Mat)
  /*!
    Set the values and populate flag
    \param R :: Radius
    \param Mat :: Material number
   */
{
  radius=R;
  defMat=Mat;
  populated=1;
  return;
}

void
insertSphere::setValues(const double R,const std::string& MatName)
  /*!
    Set the values and populate flag
    \param R :: Radius
    \param MatName :: Material number
   */
{
  ELog::RegMethod RegA("insertPlate","setValues(string)");
  
  ModelSupport::DBMaterial& DB=ModelSupport::DBMaterial::Instance();
  radius=R;
  defMat=DB.processMaterial(MatName);
  populated=1;
  return;
}

void
insertSphere::mainAll(Simulation& System)
  /*!
    Common part to createAll:
    Note: the strnage order -- create links and findObject
    before createObjects. This allows findObjects not to 
    find ourselves (and correctly to find whatever this object
    is in).
    
    \param System :: Simulation
   */
{
  ELog::RegMethod RegA("insertSphere","mainAll");
  
  createSurfaces();
  createLinks();
  findObjects(System);

  createObjects(System);
  insertObjects(System);
  return;
}


void
insertSphere::createAll(Simulation& System,
			const Geometry::Vec3D& OG,
			const attachSystem::FixedComp& FC)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param OG :: Offset origin							
    \param FC :: Linear component to set axis etc
  */
{
  ELog::RegMethod RegA("insertSphere","createAll");
  if (!populated) 
    populate(System.getDataBase());  
  createUnitVector(FC,0);
  Origin=OG;
  mainAll(System);
    
  return;
}

void
insertSphere::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int lIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Linear component to set axis etc
    \param lIndex :: link Index
  */
{
  ELog::RegMethod RegA("insertSphere","createAll");
  if (!populated) 
    populate(System.getDataBase());  
  createUnitVector(FC,lIndex);

  mainAll(System);
      
  return;
}

void
insertSphere::createAll(Simulation& System,
			const Geometry::Vec3D& Orig)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param Orig :: Centre
  */
{
  ELog::RegMethod RegA("insertSphere","createAll");
  if (!populated) 
    populate(System.getDataBase());  
  createUnitVector(World::masterOrigin(),0);
  Origin=Orig;

  mainAll(System);

  return;
}
  

  
}  // NAMESPACE insertSystem
