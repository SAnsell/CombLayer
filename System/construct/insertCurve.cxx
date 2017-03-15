/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/insertCurve.cxx
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
#include "SurInter.h"
#include "AttachSupport.h"
#include "insertCurve.h"


namespace constructSystem
{
  
insertCurve::insertCurve(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedOffset(Key,10),
  attachSystem::CellMap(),attachSystem::SurfMap(),
  ptIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(ptIndex+1),populated(0),
  defMat(0),delayInsert(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

insertCurve::insertCurve(const insertCurve& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  attachSystem::CellMap(A),attachSystem::SurfMap(A),
  ptIndex(A.ptIndex),cellIndex(A.cellIndex),
  populated(A.populated),radius(A.radius),
  width(A.width),height(A.height),
  defMat(A.defMat),delayInsert(A.delayInsert)
  /*!
    Copy constructor
    \param A :: insertCurve to copy
  */
{}

insertCurve&
insertCurve::operator=(const insertCurve& A)
  /*!
    Assignment operator
    \param A :: insertCurve to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      cellIndex=A.cellIndex;
      populated=A.populated;
      radius=A.radius;
      width=A.width;
      height=A.height;

      defMat=A.defMat;
      delayInsert=A.delayInsert;
    }
  return *this;
}

insertCurve::~insertCurve() 
  /*!
    Destructor
  */
{}

void
insertCurve::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Data Base
  */
{
  ELog::RegMethod RegA("insertCurve","populate");
  
  if (!populated)
    {
      FixedOffset::populate(Control);      
      width=Control.EvalVar<double>(keyName+"Width");
      height=Control.EvalVar<double>(keyName+"Height");
      radius=Control.EvalVar<double>(keyName+"Radius");
      defMat=ModelSupport::EvalMat<int>(Control,keyName+"DefMat");
      populated=1;
    }
  return;
}

void
insertCurve::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed coordinate system
    \param lIndex :: link index
  */
{
  ELog::RegMethod RegA("insertCurve","createUnitVector(FC,index)");


  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  return;
}

void
insertCurve::createUnitVector(const Geometry::Vec3D& OG,
			      const attachSystem::FixedComp& FC,
                              const long int sideIndex)
  /*!
    Create the unit vectors
    \param OG :: Origin
    \param FC :: FixedComp to attach to
    \param sideIndex :: linkPoint
  */
{
  ELog::RegMethod RegA("insertCurve","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  Origin=OG;
  applyOffset();
  return;
}


void
insertCurve::createSurfaces()
  /*!
    Create all the surfaces
  */
{
  ELog::RegMethod RegA("insertCurve","createSurface");

  // calculate the normal from the start ppoint

  const Geometry::Vec3D norm=(BPt-APt).unit();
  ModelSupport::buildPlane(SMap,ptIndex+1,APt,Centre,Centre+Z,norm);
  ModelSupport::buildPlane(SMap,ptIndex+2,BPt,Centre,Centre+Z,norm);


  //  setSurf("Left",SMap.realSurf(ptIndex+3));
  //  setSurf("Right",SMap.realSurf(ptIndex+4));
  //  setSurf("Base",SMap.realSurf(ptIndex+5));
  //  setSurf("Top",SMap.realSurf(ptIndex+6));
  return;
}

void
insertCurve::createLinks()
  /*!
    Create link points
  */
{
  ELog::RegMethod RegA("insertCurve","createLinks");
  /*
  if (frontActive)
    {
      FixedComp::setLinkSurf(0,frontSurf);
      FixedComp::setBridgeSurf(0,frontBridge);
      FixedComp::setConnect
        (0,SurInter::getLinePoint(Origin,Y,frontSurf,frontBridge),-Y);
    }
  else
    {
      FixedComp::setConnect(0,Origin-Y*(depth/2.0),-Y);
      FixedComp::setLinkSurf(0,-SMap.realSurf(ptIndex+1));
    }

  if (backActive)
    {
      FixedComp::setLinkSurf(1,backSurf);
      FixedComp::setBridgeSurf(1,backBridge);
      FixedComp::setConnect
        (1,SurInter::getLinePoint(Origin,Y,backSurf,backBridge),Y);
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
  */
  return;
}

void
insertCurve::createObjects(Simulation& System)
  /*!
    Create the main volume
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("insertCurve","createObjects");
  
  std::string Out=
    ModelSupport::getSetComposite(SMap,ptIndex,"1 -2 3 -4 5 -6");
  //  if (frontActive) Out+=frontSurf.display()+frontBridge.display();
  //  if (backActive) Out+=backSurf.display()+backBridge.display();
  System.addCell(MonteCarlo::Qhull(cellIndex++,defMat,0.0,Out));
  addCell("Main",cellIndex-1);
  addOuterSurf(Out);
  return;
}
  
void
insertCurve::findObjects(Simulation& System)
  /*!
    Insert the objects into the main simulation. It is separated
    from creation since we need to determine those object that 
    need to have an exclude item added to them.
    \param System :: Simulation to add object to
  */
{
  ELog::RegMethod RegA("insertCurve","findObjects");

  typedef std::map<int,MonteCarlo::Object*> MTYPE;
  
  System.populateCells();
  System.validateObjSurfMap();

  MTYPE OMap;
  attachSystem::lineIntersect(System,*this,OMap);

  // Add exclude string
  MTYPE::const_iterator ac;
  for(ac=OMap.begin();ac!=OMap.end();ac++)
    attachSystem::ContainedComp::addInsertCell(ac->first);
  
  
  return;
}

void
insertCurve::setStep(const double XS,const double YS,
		       const double ZS)
  /*!
    Set the values but NOT the populate flag
    \param XS :: X-step [width]
    \param YS :: Y-step [depth] 
    \param ZS :: Z-step [height]
   */
{
  xStep=XS;
  yStep=YS;
  zStep=ZS;
  return;
}

void
insertCurve::setStep(const Geometry::Vec3D& XYZ)
  /*!
    Set the values but NOT the populate flag
    \param XYZ :: X/Y/Z
   */
{
  xStep=XYZ[0];
  yStep=XYZ[1];
  zStep=XYZ[2];
  return;
}

void
insertCurve::setAngles(const double XS,const double ZA)
  /*!
    Set the values but NOT the populate flag
    \param XY :: XY angle
    \param ZA :: Z angle
   */
{
  xyAngle=XS;
  zAngle=ZA;
  return;
}

void
insertCurve::setValues(const double R,const double XW,
                       const double ZH,const int Mat)
  /*!
    Set the values and populate flag
    \param R :: radius
    \param XW :: Y-size [depth] 
    \param ZH :: Z-size [height]
    \param Mat :: Material number
   */
{
  radius=R;
  width=XW;
  height=ZH;
  defMat=Mat;
  populated=1;
  return;
}

void
insertCurve::setValues(const double R,const double XW,
                       const double ZH,const std::string& Mat)
  /*!
    Set the values and populate flag
    \param R :: radius
    \param XW :: Y-size [depth] 
    \param ZH :: Z-size [height]
    \param Mat :: Material number
   */
{
  ELog::RegMethod RegA("insertCurve","setValues");
  
  ModelSupport::DBMaterial& DB=ModelSupport::DBMaterial::Instance();
  setValues(R,XW,ZH,DB.processMaterial(Mat));
  return;
}

void
insertCurve::mainAll(Simulation& System)
  /*!
    Common part to createAll:
    Note: the strnage order -- create links and findObject
    before createObjects. This allows findObjects not to 
    find ourselves (and correctly to find whatever this object
    is in).
    \param System :: Simulation
   */
{
  ELog::RegMethod RegA("insertCurve","mainAll");

  
  createSurfaces();
  createLinks();

  if (!delayInsert)
    findObjects(System);
  createObjects(System);
  insertObjects(System);
  return;
}



void
insertCurve::calcCentre()
  /*!
    Simple method to caculate the centre [3d] of 
    the circle that intersect the points A,B and the plane
    defined by the normal ZA
   */
{
  ELog::RegMethod RegA("insertCurve","calcCentre");

  const Geometry::Vec3D MidPt=(APt+BPt)/2.0;
  const Geometry::Vec3D u=(BPt-APt).unit();
  const Geometry::Vec3D v=(u*Z).unit();   // sign issued gives two solution.

  const double l=APt.Distance(BPt)/2.0;
  const double x=sqrt(radius*radius-l*l);
  const std::vector<Geometry::Vec3D> Out({MidPt-v*x,MidPt+v*x});
  
  Centre= SurInter::nearPoint(Out,Origin);
  return;
}

  
void
insertCurve::createAll(Simulation& System,
                       const attachSystem::FixedComp& FC,
                       const long int sideIndex,
		       const Geometry::Vec3D& PtA,
                       const Geometry::Vec3D& PtB)
                       
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param Orig :: Point to define the sence of the curve
    \param PtA :: Point on the curve
    \param PtB :: Point on the curve
  */
{
  ELog::RegMethod RegA("insertCurve","createAll");
  if (!populated) 
    populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  // calc points
  APt=PtA;
  BPt=PtB;

  calcCentre();
  
  
  mainAll(System);
  
  return;
}
 

}  // NAMESPACE constructSystem

