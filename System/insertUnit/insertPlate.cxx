/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   insertUnit/insertPlate.cxx
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
#include "LayerDivide3D.h"
#include "insertObject.h"
#include "insertPlate.h"

namespace insertSystem
{

insertPlate::insertPlate(const std::string& Key)  :
  insertObject(Key),nGrid(1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

insertPlate::insertPlate(const insertPlate& A) : 
  insertObject(A),
  width(A.width),height(A.height),depth(A.depth),
  nGrid(A.nGrid)
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
      nGrid=A.nGrid;
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
      nGrid=Control.EvalDefVar<size_t>(keyName+"NGrid",1);
    }
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
    {
      ModelSupport::buildPlane(SMap,ptIndex+1,Origin-Y*(depth/2.0),Y);
      setSurf("Front",ptIndex+1);
    }
  else
    setSurf("Front",getFrontRule().getPrimarySurface());
  
  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,ptIndex+2,Origin+Y*(depth/2.0),Y);
      setSurf("Back",SMap.realSurf(ptIndex+2));
    }
  else
    setSurf("Back",getBackRule().getPrimarySurface());


  ModelSupport::buildPlane(SMap,ptIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,ptIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,ptIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,ptIndex+6,Origin+Z*(height/2.0),Z);

  

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

  FixedComp::setNConnect(14);
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
      FixedComp::setConnect(1,Origin+Y*(depth/2.0),Y);
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
  const Geometry::Vec3D frontPt=getLinkPt(1);
  FixedComp::setConnect(6,frontPt-X*(width/2.0)-Z*(height/2.0),-X-Z);
  FixedComp::setConnect(7,frontPt+X*(width/2.0)-Z*(height/2.0),X-Z);
  FixedComp::setConnect(8,frontPt-X*(width/2.0)+Z*(height/2.0),-X+Z);
  FixedComp::setConnect(9,frontPt+X*(width/2.0)+Z*(height/2.0),X+Z);

  // Back corner:
  const Geometry::Vec3D backPt=getLinkPt(2);
  FixedComp::setConnect(10,backPt-X*(width/2.0)-Z*(height/2.0),-X-Z);
  FixedComp::setConnect(11,backPt+X*(width/2.0)-Z*(height/2.0),X-Z);
  FixedComp::setConnect(12,backPt-X*(width/2.0)+Z*(height/2.0),-X+Z);
  FixedComp::setConnect(13,backPt+X*(width/2.0)+Z*(height/2.0),X+Z);

  FixedComp::setLinkSurf(6,-SMap.realSurf(ptIndex+3));
  FixedComp::setLinkSurf(7,SMap.realSurf(ptIndex+4));
  FixedComp::setLinkSurf(8,-SMap.realSurf(ptIndex+3));
  FixedComp::setLinkSurf(9,SMap.realSurf(ptIndex+4));

  FixedComp::addLinkSurf(6,-SMap.realSurf(ptIndex+5));
  FixedComp::addLinkSurf(7,-SMap.realSurf(ptIndex+5));
  FixedComp::addLinkSurf(8,SMap.realSurf(ptIndex+6));
  FixedComp::addLinkSurf(9,SMap.realSurf(ptIndex+6));

  FixedComp::setLinkSurf(10,-SMap.realSurf(ptIndex+3));
  FixedComp::setLinkSurf(11,SMap.realSurf(ptIndex+4));
  FixedComp::setLinkSurf(12,-SMap.realSurf(ptIndex+3));
  FixedComp::setLinkSurf(13,SMap.realSurf(ptIndex+4));

  FixedComp::addLinkSurf(10,-SMap.realSurf(ptIndex+5));
  FixedComp::addLinkSurf(11,-SMap.realSurf(ptIndex+5));
  FixedComp::addLinkSurf(12,SMap.realSurf(ptIndex+6));
  FixedComp::addLinkSurf(13,SMap.realSurf(ptIndex+6));

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
insertPlate::setGrid(const size_t NG)
  /*!
    Set the plate to be a grid plate
    \param NG :: Number of division in XZ
   */
{
  nGrid=NG;
  return;
}
  
void
insertPlate::createDivision(Simulation& System)
  /*!
    Create a divided plate
    \param System :: Simulation to use
   */
{
  ELog::RegMethod RegA("insertPlate","createDivision");
  
  ModelSupport::LayerDivide3D LD3(keyName+"DivXZ");

  if (nGrid>1)
    {
      // Front/back??
      LD3.setSurfPair(0,getSurf("Front"),getSurf("Back"));
      LD3.setSurfPair(1,SMap.realSurf(ptIndex+3),SMap.realSurf(ptIndex+4));
      LD3.setSurfPair(2,SMap.realSurf(ptIndex+5),SMap.realSurf(ptIndex+6));

      LD3.setFractions(0,1);
      LD3.setFractions(1,nGrid);	    
      LD3.setFractions(2,nGrid);

      const int cellN=getCell("Main");
      
      LD3.divideCell(System,cellN);
      addCells("Grid",LD3.getCells());
    }

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
   
}  // NAMESPACE insertSystem
