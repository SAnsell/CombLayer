/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/Window.cxx
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
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "pairBase.h"
#include "pairItem.h"
#include "pairFactory.h"
#include "Window.h"

namespace constructSystem
{

Window::Window(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedComp(Key,2),
  winIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(winIndex+1),baseCell(0),FSurf(0),BSurf(0),
  nLayers(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

Window::Window(const Window& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  winIndex(A.winIndex),cellIndex(A.cellIndex),
  baseCell(A.baseCell),Centre(A.Centre),WAxis(A.WAxis),
  fSign(A.fSign),bSign(A.bSign),FSurf(A.FSurf),BSurf(A.BSurf),
  divideFlag(A.divideFlag),width(A.width),height(A.height),
  nLayers(A.nLayers),layerThick(A.layerThick),
  layerMat(A.layerMat)
  /*!
    Copy constructor
    \param A :: Window to copy
  */
{}

Window&
Window::operator=(const Window& A)
  /*!
    Assignment operator
    \param A :: Window to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      baseCell=A.baseCell;
      Centre=A.Centre;
      WAxis=A.WAxis;
      fSign=A.fSign;
      bSign=A.bSign;
      divideFlag=A.divideFlag;
      width=A.width;
      height=A.height;
      nLayers=A.nLayers;
      layerThick=A.layerThick;
      layerMat=A.layerMat;
    }
  return *this;
}


Window::~Window() 
  /*!
    Destructor
  */
{}

void
Window::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    - Y Down the beamline
    \param FC :: Linked object
  */
{
  ELog::RegMethod RegA("Window","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC);

   return;
 }

void
Window::createCentre(Simulation& System) 
/*!
  Calculates the window centre position based on line
  track intercept with object 
  \param System :: Simulation to obtain cell from 
*/
{
  ELog::RegMethod RegA("Window","createCentre");
  
  MonteCarlo::Qhull* QHptr=System.findQhull(baseCell);
  if (!QHptr)
    {
      ELog::EM<<"Unable to find window base cell : "
	      <<baseCell<<ELog::endErr;
      return;
    }
  // QHptr->populate();
  QHptr->createSurfaceList();
  std::pair<const Geometry::Surface*,double> IPt=
    QHptr->forwardInterceptInit(Centre,WAxis);
  
  if (!IPt.first)
    {
      ELog::EM<<"Unable to find intercept track with line:"
	      <<baseCell<<ELog::endErr;
      return;
    }
  FSurf=IPt.first;
  const MonteCarlo::neutron N(1,Origin,WAxis);
  fSign=-FSurf->side(Centre);
   
  Origin=Centre+WAxis*IPt.second;
  Y=WAxis;
  X=Y*Z;
  
  IPt=QHptr->forwardIntercept(Origin,WAxis);
  
  if (!IPt.first)
    {
      ELog::EM<<"Unable to find second intercept track with line:"
	      <<baseCell<<ELog::endDiag;
      ELog::EM<<"Cell = "<<*QHptr<<ELog::endDiag;
      ELog::EM<<"B = "<<Origin<<":"<<WAxis<<ELog::endDiag;
      ELog::EM<<ELog::endErr;
      return;
    }
  BSurf=IPt.first;
  bSign=-BSurf->side(Centre);
  
  return;
}
  
void
Window::createSurfaces()
  /*!
    Create all the surfaces
  */
{
  ELog::RegMethod RegA("Window","createSurfaces");
  
  if (FSurf->className()!="Plane" ||
      BSurf->className()!="Plane" )
    {
      ModelSupport::buildPlane(SMap,winIndex+1,Centre,Y);
      divideFlag=1;
    }
  
  ModelSupport::buildPlane(SMap,winIndex+3,Origin-X*width/2.0,X);
  ModelSupport::buildPlane(SMap,winIndex+4,Origin+X*width/2.0,X);
  
  ModelSupport::buildPlane(SMap,winIndex+5,Origin-Z*height/2.0,Z);
  ModelSupport::buildPlane(SMap,winIndex+6,Origin+Z*height/2.0,Z);
  
  
  if (nLayers>1)
    {
      ModelSupport::pairBase* pBase=
	ModelSupport::pairFactory::createPair(FSurf,BSurf);
      
      int divSurf(winIndex+11);
      for(size_t i=0;i<nLayers-1;i++)
	{
	  const int sNum=pBase->
	    createSurface(layerThick[i],divSurf++);
	  layerSurf.push_back(sNum);
	}
      delete pBase;
    }
  return;
}

void 
Window::createObjects(Simulation& System)
  /*!
    Create a window object
    \param System :: Simulation
  */
{
  ELog::RegMethod RegA("Window","createObjects");
  std::string Out;

  std::string WOut=
    ModelSupport::getComposite(SMap,winIndex," 3 -4 5 -6 ");

  std::ostringstream cx;

  if (divideFlag)
    {
      cx<<SMap.realSurf(winIndex+1)<<" ";
      WOut+=cx.str();
    }
  cx<<fSign*FSurf->getName()<<" "<<-bSign*BSurf->getName()<<" ";
  addOuterSurf(WOut+cx.str());

  if (nLayers>1)
    {
    }
  else
    {
      Out=WOut+cx.str();
      System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
      addOuterSurf(Out);
    }

  return;
}


std::string
Window::getComposite(const std::string& surfList) const
  /*!
    Exposes local version of getComposite
    \param surfList :: surface list
    \return Composite string
  */
{
  return ModelSupport::getComposite(SMap,winIndex,surfList);
}

void
Window::setCentre(const Geometry::Vec3D& C,
		   const Geometry::Vec3D& A)
  /*!
    Set the centre and axis of the port
    \param C :: Centre position
    \param A :: Axis direction
  */
{
  Centre=C;
  WAxis=A.unit();
  return;
}

void
Window::setSize(const double H,const double W)
  /*!
    Set the centre and axis of the port
    \param H :: Height
    \param W :: Width
  */
{
  height=H;
  width=W;
  return;
}

void
Window::setBaseCell(const int BNumber)
  /*!
    Define the included cell (for inclusion and link points)
    \param BNumber :: Cell number
  */
{
  baseCell=BNumber;
  return;
}

void
Window::createAll(Simulation& System,
		  const attachSystem::FixedComp& FC)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: Fixed Component to place object within
  */
{
  ELog::RegMethod RegA("Window","createAll");

  createUnitVector(FC);
  createCentre(System);
  createSurfaces();
  createObjects(System);
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE ts1System

