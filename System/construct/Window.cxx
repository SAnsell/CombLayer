/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/Window.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Surface.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "pairBase.h"
#include "pairFactory.h"
#include "particle.h"
#include "eTrack.h"
#include "Window.h"

namespace constructSystem
{

Window::Window(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedComp(Key,2),
  baseCell(0),FSurf(0),BSurf(0),
  nLayers(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

Window::Window(const Window& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
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
Window::createCentre(Simulation& System) 
/*!
  Calculates the window centre position based on line
  track intercept with object 
  \param System :: Simulation to obtain cell from 
*/
{
  ELog::RegMethod RegA("Window","createCentre");
  
  MonteCarlo::Object* QHptr=System.findObject(baseCell);
  if (!QHptr)
    {
      ELog::EM<<"Unable to find window base cell : "
	      <<baseCell<<ELog::endErr;
      return;
    }
  // QHptr->populate();
  QHptr->createSurfaceList();

  std::tuple<int,const Geometry::Surface*,Geometry::Vec3D,double>
    result=QHptr->trackSurfIntersect(Centre,WAxis);
			    
  if (!std::get<0>(result))
    {
      ELog::EM<<"Unable to find intercept track with line:"
	      <<baseCell<<ELog::endErr;
      return;
    }

  FSurf=std::get<1>(result);
  fSign=-FSurf->side(Centre);  // could use result(00
   
  Origin=std::get<2>(result);
  Y=WAxis;
  X=Y*Z;
  

  result=QHptr->trackSurfIntersect(Origin,WAxis);
  
  if (!std::get<0>(result))
    {
      ELog::EM<<"Unable to find second intercept track with line:"
	      <<baseCell<<ELog::endDiag;
      ELog::EM<<"Cell = "<<*QHptr<<ELog::endDiag;
      ELog::EM<<"B = "<<Origin<<":"<<WAxis<<ELog::endDiag;
      ELog::EM<<ELog::endErr;
      return;
    }
  BSurf=std::get<1>(result);
  bSign=-FSurf->side(Centre);  // could use result(00
  
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
      ModelSupport::buildPlane(SMap,buildIndex+1,Centre,Y);
      divideFlag=1;
    }
  
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*width/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*width/2.0,X);
  
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*height/2.0,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*height/2.0,Z);
  
  
  if (nLayers>1)
    {
      ModelSupport::pairBase* pBase=
	ModelSupport::pairFactory::createPair(FSurf,BSurf);
      
      int divSurf(buildIndex+11);
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
    ModelSupport::getComposite(SMap,buildIndex," 3 -4 5 -6 ");

  std::ostringstream cx;

  if (divideFlag)
    {
      cx<<SMap.realSurf(buildIndex+1)<<" ";
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
      System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));
      addOuterSurf(Out);
    }

  return;
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
		  const attachSystem::FixedComp& FC,
		  const long int sideIndex)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: Fixed Component to place object within
  */
{
  ELog::RegMethod RegA("Window","createAll");

  createUnitVector(FC,sideIndex);
  createCentre(System);
  createSurfaces();
  createObjects(System);
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE ts1System

