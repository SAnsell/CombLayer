/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/Aperture.cxx
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
#include <numeric>
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
#include "stringCombine.h"
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
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "neutron.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "FrontBackCut.h"
#include "Aperture.h"

namespace constructSystem
{

Aperture::Aperture(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,14),
  attachSystem::FrontBackCut(),
  appIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(appIndex+1)

  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

Aperture::Aperture(const Aperture& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  attachSystem::FrontBackCut(A),
  appIndex(A.appIndex),cellIndex(A.cellIndex),
  innerWidth(A.innerWidth),innerHeight(A.innerHeight),
  width(A.width),height(A.height),thick(A.thick),
  nLayers(A.nLayers),voidMat(A.voidMat),defMat(A.defMat)
  /*!
    Copy constructor
    \param A :: Aperture to copy
  */
{}

Aperture&
Aperture::operator=(const Aperture& A)
  /*!
    Assignment operator
    \param A :: Aperture to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::FrontBackCut::operator=(A);
      cellIndex=A.cellIndex;
      innerWidth=A.innerWidth;
      innerHeight=A.innerHeight;
      width=A.width;
      height=A.height;
      thick=A.thick;
      nLayers=A.nLayers;
      voidMat=A.voidMat;
      defMat=A.defMat;
    }
  return *this;
}

Aperture::~Aperture() 
  /*!
    Destructor
  */
{}

void
Aperture::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("VacuumBox","populate");
   
  FixedOffset::populate(Control);

  // Void + Fe special:
  innerWidth=Control.EvalVar<double>(keyName+"InnerWidth");
  innerHeight=Control.EvalVar<double>(keyName+"InnerHeight");
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  thick=Control.EvalVar<double>(keyName+"Thick");

  if (height-innerHeight<Geometry::zeroTol)
    throw ColErr::OrderError<double>(innerHeight/2.0,height,
				     "innerHeight/Height");
  if (width-innerWidth<Geometry::zeroTol)
    throw ColErr::OrderError<double>(innerWidth,width,"innerWidth/Width");
  
  voidMat=ModelSupport::EvalDefMat<int>(Control,keyName+"VoidMat",0);
  defMat=ModelSupport::EvalMat<int>(Control,keyName+"DefMat");

  return;
}
  
void
Aperture::createUnitVector(const attachSystem::FixedComp& FC,
                             const long int sideIndex)
  /*!
    Create the unit vectors
    - Y Down the beamline
    \param FC :: Linked object
    \param sideIndex :: Link point
  */
{
  ELog::RegMethod RegA("Aperture","createUnitVector");
  
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  return;
}
  
void
Aperture::createSurfaces()
  /*!
    Create all the surfaces
  */
{
  ELog::RegMethod RegA("Aperture","createSurfaces");
  

  ModelSupport::buildPlane(SMap,appIndex+1,Origin-Y*thick/2.0,Y);
  ModelSupport::buildPlane(SMap,appIndex+2,Origin+Y*thick/2.0,Y);

  ModelSupport::buildPlane(SMap,appIndex+3,Origin-X*width/2.0,X);
  ModelSupport::buildPlane(SMap,appIndex+4,Origin+X*width/2.0,X);
  
  ModelSupport::buildPlane(SMap,appIndex+5,Origin-Z*height/2.0,Z);
  ModelSupport::buildPlane(SMap,appIndex+6,Origin+Z*height/2.0,Z);
  
  ModelSupport::buildPlane(SMap,appIndex+13,Origin-X*innerWidth/2.0,X);
  ModelSupport::buildPlane(SMap,appIndex+14,Origin+X*innerWidth/2.0,X);
  
  ModelSupport::buildPlane(SMap,appIndex+15,Origin-Z*innerHeight/2.0,Z);
  ModelSupport::buildPlane(SMap,appIndex+16,Origin+Z*innerHeight/2.0,Z);

  
  return;
}

void 
Aperture::createObjects(Simulation& System)
  /*!
    Create a window object
    \param System :: Simulation
  */
{
  ELog::RegMethod RegA("Aperture","createObjects");
  std::string Out;

  Out=ModelSupport::getComposite(SMap,appIndex,"1 -2 13 -14 15 -16 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,voidMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,appIndex,
                                 "1 -2 3 -4 5 -6 (-13:14:-15:16) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,defMat,0.0,Out));


  Out=ModelSupport::getComposite(SMap,appIndex,"1 -2 3 -4 5 -6 ");
  addOuterSurf(Out);
  return;
}

void
Aperture::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("VacuumBox","createLinks");


  FixedComp::setConnect(0,Origin-Y*(thick/2.0),-Y);
  FixedComp::setConnect(1,Origin+Y*(thick/2.0),Y);
  FixedComp::setConnect(2,Origin-X*(width/2.0),-X);
  FixedComp::setConnect(3,Origin+X*(width/2.0),X);
  FixedComp::setConnect(4,Origin-Z*(height/2.0),-Z);
  FixedComp::setConnect(5,Origin+Z*(height/2.0),Z);  

  FixedComp::setLinkSurf(0,-SMap.realSurf(appIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(appIndex+2));
  FixedComp::setLinkSurf(2,-SMap.realSurf(appIndex+3));
  FixedComp::setLinkSurf(3,SMap.realSurf(appIndex+4));
  FixedComp::setLinkSurf(4,-SMap.realSurf(appIndex+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(appIndex+6));

  // 8 corners [FRONT/BACK]
  for(size_t i=0;i<4;i++)
    {
      FixedComp::setLinkSurf(i+6,-SMap.realSurf(appIndex+1));
      FixedComp::setLinkSurf(i+10,SMap.realSurf(appIndex+2));
    }
  Geometry::Vec3D platePt(Origin-Y*(thick/2.0));
  FixedComp::setConnect(6,platePt-X*(width/2.0),-X);
  FixedComp::setConnect(7,platePt+X*(width/2.0),X);
  FixedComp::setConnect(8,platePt-Z*(height/2.0),-Z);
  FixedComp::setConnect(9,platePt+Z*(height/2.0),Z);

  platePt=Origin+Y*(thick/2.0);
  FixedComp::setConnect(10,platePt-X*(width/2.0),-X);
  FixedComp::setConnect(11,platePt+X*(width/2.0),X);
  FixedComp::setConnect(12,platePt-Z*(height/2.0),-Z);
  FixedComp::setConnect(13,platePt+Z*(height/2.0),Z);
  
  
  return;
}


void
Aperture::createAll(Simulation& System,
                      const attachSystem::FixedComp& FC,
                      const long int sideIndex)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: Fixed Component to place object within
    \param sideIndex :: sideIndex
  */
{
  ELog::RegMethod RegA("Aperture","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
      
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE constructSystem

