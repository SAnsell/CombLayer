/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/insertPlate.cxx
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
#include "insertPlate.h"

namespace constructSystem
{

insertPlate::insertPlate(const std::string& Key)  :
  insertObject(Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

insertPlate::insertPlate(const insertPlate& A) : 
  insertObject(A),
  width(A.width),height(A.height),depth(A.depth)
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
      insertObject::operator=(A);
      width=A.width;
      height=A.height;
      depth=A.depth;
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
    \param Control :: Data Base
  */
{
  ELog::RegMethod RegA("insertPlate","populate");
  
  if (!populated)
    {
      insertObject::populate(Control);
      width=Control.EvalVar<double>(keyName+"Width");
      height=Control.EvalVar<double>(keyName+"Height");
      depth=Control.EvalVar<double>(keyName+"Depth");
    }
  return;
}

void
insertPlate::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int lIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed coordinate system
    \param lIndex :: link index
  */
{
  ELog::RegMethod RegA("insertPlate","createUnitVector(FC,index)");


  FixedComp::createUnitVector(FC,lIndex);
  applyOffset();
  return;
}

void
insertPlate::createUnitVector(const Geometry::Vec3D& OG,
			      const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param OG :: Origin
    \param FC :: LinearComponent to attach to
  */
{
  ELog::RegMethod RegA("insertPlate","createUnitVector");

  FixedComp::createUnitVector(FC);
  Origin=OG;
  applyOffset();
  return;
}

void
insertPlate::createUnitVector(const Geometry::Vec3D& OG,
			      const Geometry::Vec3D& YUnit,
			      const Geometry::Vec3D& ZUnit)
  /*!
    Create the unit vectors
    \param OG :: Origin
    \param YUnit :: Y-direction
    \param ZUnit :: Z-direction
  */
{
  ELog::RegMethod RegA("insertPlate","createUnitVector<Vec>");


  Geometry::Vec3D xTest(YUnit.unit()*ZUnit.unit());
  Geometry::Vec3D yTest(YUnit.unit());
  Geometry::Vec3D zTest(ZUnit.unit());
  FixedComp::computeZOffPlane(xTest,yTest,zTest);
  ELog::EM<<"KLey == "<<xTest<<":"<<yTest<<":"<<zTest<<ELog::endDiag;
  FixedComp::createUnitVector(OG,yTest*zTest,yTest,zTest);
  Origin=OG;
  applyOffset();

  return;
}

void
insertPlate::createSurfaces()
  /*!
    Create all the surfaces
  */
{
  ELog::RegMethod RegA("insertPlate","createSurface");

  if (!frontActive())
    ModelSupport::buildPlane(SMap,ptIndex+1,Origin-Y*depth/2.0,Y);
  if (!backActive())
    ModelSupport::buildPlane(SMap,ptIndex+2,Origin+Y*depth/2.0,Y);


  ModelSupport::buildPlane(SMap,ptIndex+3,Origin-X*width/2.0,X);
  ModelSupport::buildPlane(SMap,ptIndex+4,Origin+X*width/2.0,X);
  ModelSupport::buildPlane(SMap,ptIndex+5,Origin-Z*height/2.0,Z);
  ModelSupport::buildPlane(SMap,ptIndex+6,Origin+Z*height/2.0,Z);


  if (!frontActive())
    setSurf("Front",ptIndex+1);
  else
    setSurf("Front",getFrontRule().getPrimarySurface());

  if (!backActive())
    setSurf("Back",SMap.realSurf(ptIndex+2));
  else
    setSurf("Back",getBackRule().getPrimarySurface());

  setSurf("Left",SMap.realSurf(ptIndex+3));
  setSurf("Right",SMap.realSurf(ptIndex+4));
  setSurf("Base",SMap.realSurf(ptIndex+5));
  setSurf("Top",SMap.realSurf(ptIndex+6));
  return;
}

void
insertPlate::createLinks()
  /*!
    Create link points
  */
{
  ELog::RegMethod RegA("insertPlate","createLinks");

  if (frontActive())
    {
      setLinkSurf(0,getFrontRule());
      setLinkSurf(0,getFrontBridgeRule());
      FixedComp::setConnect
        (0,SurInter::getLinePoint(Origin,Y,getFrontRule(),
				  getFrontBridgeRule()),-Y);
    }
  else
    {
      FixedComp::setConnect(0,Origin-Y*(depth/2.0),-Y);
      FixedComp::setLinkSurf(0,-SMap.realSurf(ptIndex+1));
    }

  if (backActive())
    {
      FixedComp::setLinkSurf(1,getBackRule());
      FixedComp::setBridgeSurf(1,getBackBridgeRule());
      FixedComp::setConnect
        (1,SurInter::getLinePoint(Origin,Y,getBackRule(),
				  getBackBridgeRule()),Y);
    }
  else
    {
      FixedComp::setConnect(1,Origin+Y*(depth/2.0),-Y);
      FixedComp::setLinkSurf(1,SMap.realSurf(ptIndex+2));
    }
  
  FixedComp::setConnect(2,Origin-X*(width/2.0),-X);
  FixedComp::setConnect(3,Origin+X*(width/2.0),X);
  FixedComp::setConnect(4,Origin-Z*(height/2.0),-Z);
  FixedComp::setConnect(5,Origin+Z*(height/2.0),Z);

  FixedComp::setLinkSurf(2,-SMap.realSurf(ptIndex+3));
  FixedComp::setLinkSurf(3,SMap.realSurf(ptIndex+4));
  FixedComp::setLinkSurf(4,-SMap.realSurf(ptIndex+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(ptIndex+6));

  // corners 
  FixedComp::setConnect(6,Origin-X*(width/2.0)-Z*(height/2.0),-X-Z);
  FixedComp::setConnect(7,Origin+X*(width/2.0)-Z*(height/2.0),X-Z);
  FixedComp::setConnect(8,Origin-X*(width/2.0)+Z*(height/2.0),-X+Z);
  FixedComp::setConnect(9,Origin+X*(width/2.0)+Z*(height/2.0),X+Z);

  FixedComp::setLinkSurf(6,-SMap.realSurf(ptIndex+3));
  FixedComp::setLinkSurf(7,SMap.realSurf(ptIndex+4));
  FixedComp::setLinkSurf(8,-SMap.realSurf(ptIndex+3));
  FixedComp::setLinkSurf(9,SMap.realSurf(ptIndex+4));

  FixedComp::addLinkSurf(6,-SMap.realSurf(ptIndex+5));
  FixedComp::addLinkSurf(7,-SMap.realSurf(ptIndex+5));
  FixedComp::addLinkSurf(8,SMap.realSurf(ptIndex+6));
  FixedComp::addLinkSurf(9,SMap.realSurf(ptIndex+6));

  return;
}

void
insertPlate::createObjects(Simulation& System)
  /*!
    Create the main volume
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("insertPlate","createObjects");
  
  std::string Out=
    ModelSupport::getSetComposite(SMap,ptIndex," 1 -2 3 -4 5 -6 ");
  Out+=frontRule();
  Out+=backRule();
  System.addCell(MonteCarlo::Qhull(cellIndex++,defMat,0.0,Out));
  addCell("Main",cellIndex-1);
  addOuterSurf(Out);
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
insertPlate::setValues(const double XS,const double YS,
		       const double ZS,const std::string& Mat)
  /*!
    Set the values and populate flag
    \param XS :: X-size [width]
    \param YS :: Y-size [depth] 
    \param ZS :: Z-size [height]
    \param Mat :: Material number
   */
{
  ELog::RegMethod RegA("insertPlate","setValues");
  ModelSupport::DBMaterial& DB=ModelSupport::DBMaterial::Instance();
  setValues(XS,YS,ZS,DB.processMaterial(Mat));
  return;
}

void
insertPlate::mainAll(Simulation& System)
  /*!
    Common part to createAll:
    Note: the strnage order -- create links and findObject
    before createObjects. This allows findObjects not to 
    find ourselves (and correctly to find whatever this object
    is in).
    
    \param System :: Simulation
   */
{
  ELog::RegMethod RegA("insertPlate","mainAll");
  
  createSurfaces();
  createLinks();

  if (!delayInsert)
    findObjects(System);
  createObjects(System);

  insertObjects(System);
  return;
}


void
insertPlate::createAll(Simulation& System,const Geometry::Vec3D& OG,
		       const attachSystem::FixedComp& FC)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param OG :: Offset origin							
    \param FC :: Linear component to set axis etc
  */
{
  ELog::RegMethod RegA("insertPlate","createAll(Vec,FC)");
  if (!populated) 
    populate(System.getDataBase());  
  createUnitVector(OG,FC);
  mainAll(System);
  return;
}

void
insertPlate::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int lIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Linear component to set axis etc
    \param lIndex :: link Index
  */
{
  ELog::RegMethod RegA("insertPlate","createAll(FC,index)");
  
  if (!populated) 
    populate(System.getDataBase());  
  createUnitVector(FC,lIndex);
  mainAll(System);
  
  return;
}

void
insertPlate::createAll(Simulation& System,
		       const Geometry::Vec3D& Orig,
                       const Geometry::Vec3D& YA,
                       const Geometry::Vec3D& ZA)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param Orig :: Origin al point 
    \param YA :: Origin al point 
    \param ZA :: ZAxis
  */
{
  ELog::RegMethod RegA("insertPlate","createAll");
  if (!populated) 
    populate(System.getDataBase());  
  createUnitVector(Orig,YA,ZA);
  mainAll(System);
  
  return;
}
   
}  // NAMESPACE constructSystem
