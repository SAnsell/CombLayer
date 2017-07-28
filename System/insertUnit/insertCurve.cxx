/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   insertUnit/insertCurve.cxx
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
#include "insertCurve.h"

namespace insertSystem
{

insertCurve::insertCurve(const std::string& Key)  :
  insertObject(Key),yFlag(1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

insertCurve::insertCurve(const insertCurve& A) : 
  insertSystem::insertObject(A),
  yFlag(A.yFlag),radius(A.radius),width(A.width),
  height(A.height),length(A.length),Centre(A.Centre)
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
      insertSystem::insertObject::operator=(A);
      yFlag=A.yFlag;
      radius=A.radius;
      width=A.width;
      height=A.height;
      length=A.length;
      Centre=A.Centre;
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
      insertObject::populate(Control);
      yFlag=Control.EvalDefVar<int>(keyName+"YFlag",1);
      radius=Control.EvalVar<double>(keyName+"Radius");
      width=Control.EvalVar<double>(keyName+"Width");
      // radial length
      length=Control.EvalVar<double>(keyName+"Length");
    }
  return;
}

void
insertCurve::createUnitVector(const attachSystem::FixedComp& FC,
                              const long int lIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed coordinate system
    \param lIndex :: link index
  */
{
  ELog::RegMethod RegA("insertCurve","createUnitVector(FC,index)");

  FixedComp::createUnitVector(FC,lIndex);
  applyOffset();
  Centre= Origin+Y*(radius*yFlag);
  return;
}

void
insertCurve::createUnitVector(const Geometry::Vec3D& OG,
                                 const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param OG :: Origin
    \param FC :: LinearComponent to attach to
  */
{
  ELog::RegMethod RegA("insertCurve","createUnitVector");

  FixedComp::createUnitVector(FC);
  Origin=OG;
  applyOffset();

  Centre = Origin+Y*(radius*yFlag);
  return;
}

void
insertCurve::createUnitVector(const Geometry::Vec3D& OG,
                              const Geometry::Vec3D& YAxis,
                              const Geometry::Vec3D& ZAxis)
  /*!
    Create the unit vectors
    \param OG :: Origin
    \param YAxis :: Y-direction 
    \param ZAxis :: Z-direction 
  */
{
  ELog::RegMethod RegA("insertCurve","createUnitVector<Vec,Vec,Vec>");
  
  Y=YAxis.unit();
  Z=ZAxis.unit();
  X=Y*Z;
  FixedComp::computeZOffPlane(Y,Z,X);  // note applied to X
  Origin=OG;
  applyOffset();

  Centre= Origin-YAxis*(radius*yFlag);
  return;
}

void
insertCurve::createSurfaces()
  /*!
    Create all the surfaces
  */
{
  ELog::RegMethod RegA("insertCurve","createSurface");

  const double theta=length/(2*radius);  // angle in radians

  const Geometry::Quaternion QFront=
    Geometry::Quaternion::calcQRot(theta,Z*yFlag);
  
  if (!frontActive())
    {
      Geometry::Vec3D AAxis(X);
      QFront.rotate(AAxis);
      ModelSupport::buildPlane(SMap,ptIndex+1,Centre,AAxis);
    }
  if (!backActive())
    {
      Geometry::Vec3D BAxis(X);
      QFront.invRotate(BAxis);
      ModelSupport::buildPlane(SMap,ptIndex+2,Centre,BAxis);
    }

  
  ModelSupport::buildCylinder(SMap,ptIndex+7,Centre,Z,radius-width/2.0);
  ModelSupport::buildCylinder(SMap,ptIndex+17,Centre,Z,radius+width/2.0);

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

  setSurf("InnerRadius",SMap.realSurf(ptIndex+7));
  setSurf("OuterRadius",SMap.realSurf(ptIndex+17));
  return;
}

void
insertCurve::createLinks()
  /*!
    Create link points
  */
{
  ELog::RegMethod RegA("insertCurve","createLinks");

  FixedComp::setNConnect(10);

  Geometry::Vec3D APt,BPt;
  const double theta=length/(2*radius);  // angle in radians
  FrontBackCut::createLinks(*this,Origin,Y);
  if (!frontActive())
    {
      APt=Centre+(Y*cos(theta)-X*sin(theta)) * (yFlag*radius);
      FixedComp::setConnect(0,APt,-X*yFlag);
      FixedComp::setLinkSurf(0,-SMap.realSurf(ptIndex+1));
    }

  if (!backActive())
    {
      BPt=Centre+(Y*cos(theta)+X*sin(theta)) * (yFlag*radius);
      FixedComp::setConnect(1,BPt,X*yFlag);
      FixedComp::setLinkSurf(1,SMap.realSurf(ptIndex+2));
    }
  
  FixedComp::setConnect(2,Origin-Y*(width/2.0),-Y);
  FixedComp::setConnect(3,Origin+Y*(width/2.0),Y);
  FixedComp::setConnect(4,Origin-Z*(height/2.0),-Z);
  FixedComp::setConnect(5,Origin+Z*(height/2.0),Z);

  FixedComp::setLinkSurf(2,-SMap.realSurf(ptIndex+7));
  FixedComp::setLinkSurf(3,SMap.realSurf(ptIndex+17));
  FixedComp::setLinkSurf(4,-SMap.realSurf(ptIndex+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(ptIndex+6));

  FixedComp::setConnect(6,APt-Z*(height/2.0),-X*yFlag);
  FixedComp::setConnect(7,BPt-Z*(height/2.0),X*yFlag);
  FixedComp::setLinkSurf(6,-SMap.realSurf(ptIndex+1));
  FixedComp::setLinkSurf(7,SMap.realSurf(ptIndex+2));

  FixedComp::setConnect(8,APt+Z*(height/2.0),-X*yFlag);
  FixedComp::setConnect(9,BPt+Z*(height/2.0),X*yFlag);
  FixedComp::setLinkSurf(8,-SMap.realSurf(ptIndex+1));
  FixedComp::setLinkSurf(9,SMap.realSurf(ptIndex+2));


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
    ModelSupport::getSetComposite(SMap,ptIndex," 1 -2 7 -17 5 -6 ");
  Out+=frontRule();
  Out+=backRule();
  System.addCell(MonteCarlo::Qhull(cellIndex++,defMat,0.0,Out));
  addCell("Main",cellIndex-1);
  addOuterSurf(Out);
  return;
}

void
insertCurve::setValues(const double R,const double L,
                       const double W,const double H,
                       const int Mat)
  /*!
    Set the values and populate flag
    \param R :: Radius
    \param L :: length
    \param W :: Width 
    \param H :: height
    \param Mat :: Material number
   */
{
  radius=R;
  length=L;
  width=W;
  height=H;
  defMat=Mat;
  populated=1;
  return;
}

void
insertCurve::setValues(const double R,const double L,
                       const double W,const double H,
                       const std::string& Mat)
  /*!
    Set the values and populate flag
    \param R :: Radius
    \param L :: length
    \param W :: width
    \param H :: height
    \param Mat :: Material name
   */
{
  ELog::RegMethod RegA("insertCurve","setValues(string)");
  
  ModelSupport::DBMaterial& DB=ModelSupport::DBMaterial::Instance();
  setValues(R,L,W,H,DB.processMaterial(Mat));
  return;
}

void
insertCurve::mainAll(Simulation& System)
  /*!
    Common part to createAll:
    Note: the strange order -- create links and findObject
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
insertCurve::createAll(Simulation& System,const Geometry::Vec3D& OG,
                          const attachSystem::FixedComp& FC)
/*!
    Generic function to create everything
    \param System :: Simulation item
    \param OG :: Offset origin							
    \param FC :: Linear component to set axis etc
  */
{
  ELog::RegMethod RegA("insertCurve","createAll(Vec,FC)");
  if (!populated) 
    populate(System.getDataBase());  
  createUnitVector(OG,FC);
  mainAll(System);
  return;
}

void
insertCurve::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int lIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Linear component to set axis etc
    \param lIndex :: link Index
  */
{
  ELog::RegMethod RegA("insertCurve","createAll(FC,index)");
  
  if (!populated) 
    populate(System.getDataBase());  
  createUnitVector(FC,lIndex);
  mainAll(System);
  
  return;
}

void
insertCurve::createAll(Simulation& System,
                       const Geometry::Vec3D& Orig,
                       const Geometry::Vec3D& YAxis,
                       const Geometry::Vec3D& ZAxis)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param Orig :: Origin point 
    \param YAxis :: Main axis
    \param ZAxis :: Z axis
  */
{
  ELog::RegMethod RegA("insertCurve","createAll");
  if (!populated) 
    populate(System.getDataBase());  
  createUnitVector(Orig,YAxis,ZAxis);
  mainAll(System);
  
  return;
}
   
}  // NAMESPACE insertSystem
