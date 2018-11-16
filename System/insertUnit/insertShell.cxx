/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   insertUnit/insertShell.cxx
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
#include "surfRegister.h"
#include "objectRegister.h"
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
#include "groupRange.h"
#include "objectGroups.h"
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
#include "insertShell.h"

namespace insertSystem
{

insertShell::insertShell(const std::string& Key)  :
  insertObject(Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

insertShell::insertShell(const insertShell& A) : 
  insertObject(A),
  innerRadius(A.innerRadius),outerRadius(A.outerRadius),
  innerMat(A.innerMat)
  /*!
    Copy constructor
    \param A :: insertShell to copy
  */
{}

insertShell&
insertShell::operator=(const insertShell& A)
  /*!
    Assignment operator
    \param A :: insertShell to copy
    \return *this
  */
{
  if (this!=&A)
    {
      insertObject::operator=(A);
      innerRadius=A.innerRadius;
      outerRadius=A.outerRadius;
      innerMat=A.innerMat;
    }
  return *this;
}

insertShell::~insertShell() 
  /*!
    Destructor
  */
{}

void
insertShell::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Data Base
  */
{
  ELog::RegMethod RegA("insertShell","populate");
  
  if (!populated)
    {
      insertObject::populate(Control);
      innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");
      outerRadius=Control.EvalVar<double>(keyName+"OuterRadius");
      innerMat=ModelSupport::EvalDefMat<int>(Control,keyName+"InnerMat",-1);
    }
  return;
}

void
insertShell::createSurfaces()
  /*!
    Create all the surfaces
  */
{
  ELog::RegMethod RegA("insertShell","createSurface");

  ModelSupport::buildSphere(SMap,buildIndex+7,Origin,innerRadius);
  ModelSupport::buildSphere(SMap,buildIndex+17,Origin,outerRadius);

  setSurf("InnerSurf",SMap.realSurf(buildIndex+7));
  setSurf("OuterSurf",SMap.realSurf(buildIndex+17));
  return;
}

void
insertShell::createLinks()
  /*!
    Create link points
  */
{
  ELog::RegMethod RegA("insertShell","createLinks");

  FixedComp::setNConnect(12);
  const Geometry::Vec3D Dir[3]={Y,X,Z};

  for(size_t i=0;i<6;i++)
    {
      const double SN((i%2) ? 1.0 : -1.0);
      FixedComp::setConnect(i,Origin+Dir[i/2]*outerRadius,Dir[i/2]*SN);
      FixedComp::setLinkSurf(i,SMap.realSurf(buildIndex+7));
      FixedComp::setConnect(i+6,Origin+Dir[i/2]*outerRadius,Dir[i/2]*SN);
      FixedComp::setLinkSurf(i+6,SMap.realSurf(buildIndex+17));
    }
  
  return;
}

void
insertShell::createObjects(Simulation& System)
  /*!
    Create the spherical volume
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("insertShell","createObjects");
  
  std::string Out=
    ModelSupport::getComposite(SMap,buildIndex," 7 -17 ");
  CellMap::makeCell("Main",System,cellIndex++,defMat,0.0,Out);

  if (innerMat>=0)
    {
      Out=ModelSupport::getComposite(SMap,buildIndex," -7 ");
      CellMap::makeCell("Void",System,cellIndex++,innerMat,0.0,Out);
      Out=ModelSupport::getComposite(SMap,buildIndex," -17 ");
    }

  addOuterSurf(Out);
  return;
}

void
insertShell::findObjects(Simulation& System)
  /*!
    Insert the objects into the main simulation. It is separated
    from creation since we need to determine those object that 
    need to have an exclude item added to them.
    \param System :: Simulation to add object to
  */
{
  ELog::RegMethod RegA("insertShell","findObjects");

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
      Geometry::Vec3D TP(Origin);
      for(size_t i=0;i<6;i++)
        {          
	  TP=Origin+XYZ[i]*innerRadius;
          OPtr=System.findCell(TP,OPtr);
          if (OPtr)
            ICells.insert(OPtr->getName());
	  TP=Origin+XYZ[i]*outerRadius;
          OPtr=System.findCell(TP,OPtr);
          if (OPtr)
            ICells.insert(OPtr->getName());
        }
      
      // sqrt directions // and normal directions
      const double innerRR=innerRadius/sqrt(2);
      const double outerRR=outerRadius/sqrt(2);
      for(size_t i=0;i<6;i++)
	for(size_t j=i;j<6;j++)
	  {
	    if (i!=((j+3) % 6))
	      {
		Geometry::Vec3D TP(Origin);
		TP=Origin+(XYZ[i]+XYZ[j])*innerRR;
		OPtr=System.findCell(TP,OPtr);
		if (OPtr)
		  ICells.insert(OPtr->getName());

		TP=Origin+(XYZ[i]+XYZ[j])*outerRR;
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
insertShell::setValues(const double iR,const double oR,
		       const int IMat,const int Mat)
  /*!
    Set the values and populate flag
    \param iR :: Inner Radius
    \param oR :: Outer Radius
    \param IMat :: Inner Material number [-1 : not included]
    \param Mat :: Material number (outer shell)

   */
{
  innerRadius=iR;
  outerRadius=oR;
  defMat=Mat;
  innerMat=IMat;
  populated=1;
  return;
}

void
insertShell::setValues(const double iR,const double oR,
		       const std::string& innerMatName,
		       const std::string& matName)
  /*!
    Set the values and populate flag
    \param iR :: Inner Radius
    \param oR :: Outer Radius
    \param innerMatName :: Inner Material number
    \param matName :: Material number
   */
{
  ELog::RegMethod RegA("insertPlate","setValues(string)");
  
  ModelSupport::DBMaterial& DB=ModelSupport::DBMaterial::Instance();
  innerRadius=iR;
  outerRadius=oR;
  innerMat=DB.processMaterial(innerMatName);
  defMat=DB.processMaterial(matName);
  populated=1;
  return;
}

void
insertShell::mainAll(Simulation& System)
  /*!
    Common part to createAll:
    Note: the strnage order -- create links and findObject
    before createObjects. This allows findObjects not to 
    find ourselves (and correctly to find whatever this object
    is in).
    
    \param System :: Simulation
   */
{
  ELog::RegMethod RegA("insertShell","mainAll");
  
  createSurfaces();
  createLinks();
  findObjects(System);

  createObjects(System);
  insertObjects(System);
  return;
}


void
insertShell::createAll(Simulation& System,
			const Geometry::Vec3D& OG,
			const attachSystem::FixedComp& FC)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param OG :: Offset origin							
    \param FC :: Linear component to set axis etc
  */
{
  ELog::RegMethod RegA("insertShell","createAll");
  if (!populated) 
    populate(System.getDataBase());  
  createUnitVector(FC,0);
  Origin=OG;
  mainAll(System);
    
  return;
}

void
insertShell::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int lIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Linear component to set axis etc
    \param lIndex :: link Index
  */
{
  ELog::RegMethod RegA("insertShell","createAll");
  if (!populated) 
    populate(System.getDataBase());  
  createUnitVector(FC,lIndex);

  mainAll(System);
      
  return;
}

void
insertShell::createAll(Simulation& System,
			const Geometry::Vec3D& Orig)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param Orig :: Centre
  */
{
  ELog::RegMethod RegA("insertShell","createAll");
  if (!populated) 
    populate(System.getDataBase());  
  createUnitVector(World::masterOrigin(),0);
  Origin=Orig;

  mainAll(System);

  return;
}
  

  
}  // NAMESPACE insertSystem
