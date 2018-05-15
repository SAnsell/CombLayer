/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   bibBuild/NiGuide.cxx
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
#include "surfRegister.h"
#include "objectRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
#include "Rules.h"
#include "HeadRule.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "support.h"
#include "stringCombine.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"

#include "NiGuide.h"


namespace bibSystem
{

NiGuide::NiGuide(const std::string& Key) :
  attachSystem::ContainedComp(),attachSystem::FixedOffset(Key,6),
  niGuideIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(niGuideIndex+1)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

NiGuide::NiGuide(const NiGuide& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  niGuideIndex(A.niGuideIndex),cellIndex(A.cellIndex),
  width(A.width),height(A.height),
  length(A.length),wallThick(A.wallThick),wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: NiGuide to copy
  */
{}

NiGuide&
NiGuide::operator=(const NiGuide& A)
  /*!
    Assignment operator
    \param A :: NiGuide to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      cellIndex=A.cellIndex;
      width=A.width;
      height=A.height;
      length=A.length;
      wallThick=A.wallThick;
      wallMat=A.wallMat;      
    }
  return *this;
}


NiGuide::~NiGuide()
  /*!
    Destructor
  */
{}

void
NiGuide::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("NiGuide","populate");

  attachSystem::FixedOffset::populate(Control);

  
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  length=Control.EvalVar<double>(keyName+"Length");
  
  wallThick=Control.EvalVar<double>(keyName+"WallThick");  
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");  

  return;
}


void
NiGuide::createUnitVector(const attachSystem::FixedComp& FC,
			   const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
    \param sideIndex :: Link Point
  */
{
  ELog::RegMethod RegA("NiGuide","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  return;
}

void
NiGuide::createSurfaces(const attachSystem::FixedComp& FCA,
			const long int sideIndexA,
			const attachSystem::FixedComp& FCB,
			const long int sideIndexB)
/*!
    Create planes for the silicon and Polyethene layers
    \param FCA : Front surface FixedComp 
    \param FCB : Back surface FixedComp
    \param sideIndexA :: Front surface link point
    \param sideIndexB :: Back surface link point
*/
{
  ELog::RegMethod RegA("NiGuide","createSurfaces");

  ModelSupport::buildPlane(SMap,niGuideIndex+2,Origin+Y*length,Y);    
  ModelSupport::buildPlane(SMap,niGuideIndex+3,Origin-X*width/2.0,X);  
  ModelSupport::buildPlane(SMap,niGuideIndex+4,Origin+X*width/2.0,X);  
  ModelSupport::buildPlane(SMap,niGuideIndex+5,Origin-Z*height/2.0,Z);  
  ModelSupport::buildPlane(SMap,niGuideIndex+6,Origin+Z*height/2.0,Z);  
  
  ModelSupport::buildPlane(SMap,niGuideIndex+11,Origin-X*(width/2.0+wallThick),X);  
  ModelSupport::buildPlane(SMap,niGuideIndex+12,Origin+X*(width/2.0+wallThick),X);  
  ModelSupport::buildPlane(SMap,niGuideIndex+13,Origin-Z*(height/2.0+wallThick),Z);  
  ModelSupport::buildPlane(SMap,niGuideIndex+14,Origin+Z*(height/2.0+wallThick),Z);  
  
  SMap.addMatch(niGuideIndex+21,FCA.getLinkSurf(sideIndexA)); 
  SMap.addMatch(niGuideIndex+22,FCB.getLinkSurf(sideIndexB)); 


  return; 
}

void
NiGuide::createObjects(Simulation& System,
		       const attachSystem::ContainedComp& CC)
  /*!
    Create the simple moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("NiGuide","createObjects");
  
  std::string Out;

  // NiGuide
  Out=ModelSupport::getComposite(SMap,niGuideIndex,"21 22 -2 3 -4 5 -6");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));


  Out=ModelSupport::getComposite(SMap,niGuideIndex,"21 22 -2 11 -12 13 -14  (-3:4:-5:6)");
  Out+=CC.getContainer(); /// Con esto se incluye tooooodo lo que hay en el interior de esta celda en la guia
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  
  
  Out=ModelSupport::getComposite(SMap,niGuideIndex,"21 22 -2 11 -12 13 -14" ); /** Hay que añadir una superficie externa*/
  addOuterSurf(Out);

  return; 
}

void
NiGuide::createLinks()
  /*!
    Creates a full attachment set
    Surfaces pointing outwards
  */
{
  ELog::RegMethod RegA("NiGuide","createLinks");
  // set Links :: Inner links:
  // Wrapper layer
  // Index : Point :: Normal
///   FixedComp::setConnect(0,Origin,-Y); /** Es importante decir donde estan los puntos, y elegir la normal a los mismos.*/
  FixedComp::setConnect(0,Origin-X*width/2.0+Y*20.0,-X);
  FixedComp::setConnect(1,Origin+Y*length,Y);  
  FixedComp::setConnect(2,Origin-X*(width/2.0+wallThick)+Y*40.0,-X);  
  FixedComp::setConnect(3,Origin+X*(width/2.0+wallThick)+Y*40.0,X);  
  FixedComp::setConnect(4,Origin-Z*(height/2.0+wallThick),-Z);  
  FixedComp::setConnect(5,Origin+Z*(height/2.0+wallThick),Z);  

  // WHY HAVENT I WRITTEN AutoLinkSurf()
  FixedComp::setLinkSurf(0,-SMap.realSurf(niGuideIndex+11)); /**Hay que respetar el numero de superficie a la que se liga el punto de vinculado (linking point)*/
  FixedComp::setLinkSurf(1,SMap.realSurf(niGuideIndex+12));
  FixedComp::setLinkSurf(2,-SMap.realSurf(niGuideIndex+13));
  FixedComp::setLinkSurf(3,SMap.realSurf(niGuideIndex+14));
  FixedComp::setLinkSurf(4,-SMap.realSurf(niGuideIndex+15));
  FixedComp::setLinkSurf(5,SMap.realSurf(niGuideIndex+16));
  
  return;
}

void
NiGuide::createAll(Simulation& System,
		   const attachSystem::FixedComp& FCA,
		   const long int sideIndexA,
		   const attachSystem::FixedComp& FCB,
		   const long int sideIndexB,
		   const attachSystem::ContainedComp& CC)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FCA :: FixedComp for origin
    \param sideIndexA :: Front Side index
    \param FCB :: FixedComp for back
    \param sideIndexB :: Back Side index
    \param CC :: Contained Componenet (cutter)
   */
{
  ELog::RegMethod RegA("NiGuide","createAll");

  populate(System.getDataBase());

  createUnitVector(FCA,sideIndexA);
  createSurfaces(FCA,sideIndexA,FCB,sideIndexB);
  createObjects(System,CC);

  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE bibSystem
