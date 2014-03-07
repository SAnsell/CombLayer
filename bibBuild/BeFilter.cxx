/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   bibBuild/BeFilter.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <boost/shared_ptr.hpp>

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
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
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
#include "ContainedComp.h"
#include "ContainedGroup.h"

#include "BeFilter.h"


namespace bibSystem
{

BeFilter::BeFilter(const std::string& Key) :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,6),
  befilterindex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(befilterindex+1)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

BeFilter::BeFilter(const BeFilter& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  befilterindex(A.befilterindex),cellIndex(A.cellIndex),
  xStep(A.xStep),yStep(A.yStep),zStep(A.zStep),
  xyAngle(A.xyAngle),zAngle(A.zAngle),width(A.width),
  height(A.height),length(A.length),wallThick(A.wallThick),
  wallMat(A.wallMat),beMat(A.beMat),beTemp(A.beTemp)
  /*!
    Copy constructor
    \param A :: BeFilter to copy
  */
{}

BeFilter&
BeFilter::operator=(const BeFilter& A)
  /*!
    Assignment operator
    \param A :: BeFilter to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
      width=A.width;
      height=A.height;
      length=A.length;
      wallThick=A.wallThick;
      wallMat=A.wallMat;
      beMat=A.beMat;
      beTemp=A.beTemp;
    }
  return *this;
}


BeFilter::~BeFilter()
  /*!
    Destructor
  */
{}

void
BeFilter::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("BeFilter","populate");

  // Master values
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYangle");
  zAngle=Control.EvalVar<double>(keyName+"Zangle");

  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  length=Control.EvalVar<double>(keyName+"Length");
  
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");  
  beMat=ModelSupport::EvalMat<int>(Control,keyName+"BeMat");  
  beTemp=Control.EvalVar<double>(keyName+"BeTemp");
      
  return;
}


void
BeFilter::createUnitVector(const attachSystem::FixedComp& FC,
			   const size_t sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
  */
{
  ELog::RegMethod RegA("BeFilter","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC);
  Origin=FC.getLinkPt(sideIndex); /** Aqui pide el punto*/
  
  // Note shift by wall thickness to allow wall creation
  applyShift(xStep,wallThick,zStep);
  applyAngleRotate(xyAngle,zAngle);

  return;
}

void
BeFilter::createSurfaces()
  /*!
    Create planes for the silicon and Polyethene layers
  */
  
  /** Creamos las superficies de cada cuerpo*/
{
  ELog::RegMethod RegA("BeFilter","createSurfaces");

  ModelSupport::buildPlane(SMap,befilterindex+1,Origin,Y);  
  ModelSupport::buildCylinder(SMap,befilterindex+2,Origin,Z,length); /** Superficie cilindrica. Hay que indicar le eje normal*/
  
  ModelSupport::buildPlane(SMap,befilterindex+3,Origin-X*width/2.0,X);  
  ModelSupport::buildPlane(SMap,befilterindex+4,Origin+X*width/2.0,X);  
  ModelSupport::buildPlane(SMap,befilterindex+5,Origin-Z*height/2.0,Z);  
  ModelSupport::buildPlane(SMap,befilterindex+6,Origin+Z*height/2.0,Z);  
  
  ModelSupport::buildPlane(SMap,befilterindex+11,Origin-Y*wallThick,Y);  
  ModelSupport::buildCylinder(SMap,befilterindex+12,Origin,Z,(length+wallThick)); /** Superficie cilindrica. Hay que indicar el eje normal*/
  
  ModelSupport::buildPlane(SMap,befilterindex+13,Origin-X*(width/2.0+wallThick),X);  
  ModelSupport::buildPlane(SMap,befilterindex+14,Origin+X*(width/2.0+wallThick),X);  
  ModelSupport::buildPlane(SMap,befilterindex+15,Origin-Z*(height/2.0+wallThick),Z);  
  ModelSupport::buildPlane(SMap,befilterindex+16,Origin+Z*(height/2.0+wallThick),Z);  


  return; 
}

void
BeFilter::createObjects(Simulation& System,
			const attachSystem::ContainedComp& CC)
  /*!
    Create the simple moderator
    \param System :: Simulation to add results
    \param CC :: Moderator for containement [overkill]
   */
{
  ELog::RegMethod RegA("BeFilter","createObjects");
  
  std::string Out;

  // BeFilter
  Out=ModelSupport::getComposite(SMap,befilterindex,"1 -2 3 -4 5 -6");
  System.addCell(MonteCarlo::Qhull(cellIndex++,beMat,beTemp,Out));

  Out=ModelSupport::getComposite(SMap,befilterindex,
				 "11 -12 13 -14 15 -16  (-1:2:-3:4:-5:6)");
  Out+=CC.getContainer();
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,beTemp,Out));
  
  
  // Hay que añadir una superficie externa
  Out=ModelSupport::getComposite(SMap,befilterindex,"11 -12 13 -14 15 -16" );
  addOuterSurf(Out);

  return; 
}

void
BeFilter::createLinks()
  /*!
    Creates a full attachment set
    Surfaces pointing outwards
  */
{
  ELog::RegMethod RegA("BeFilter","createLinks");
  // set Links :: Inner links:
  // Wrapper layer
  // Index : Point :: Normal
  FixedComp::setConnect(0,Origin-Y*wallThick,-Y); /** Es importante decir donde estan los puntos, y elegir la normal a los mismos.*/
  FixedComp::setConnect(1,Origin+Y*(length+wallThick),Y);  
  FixedComp::setConnect(2,Origin-X*(width/2.0+wallThick),-X);  
  FixedComp::setConnect(3,Origin+X*(width/2.0+wallThick),X);  
  FixedComp::setConnect(4,Origin-Z*(height/2.0+wallThick),-Z);  
  FixedComp::setConnect(5,Origin+Z*(height/2.0+wallThick),Z);  

  FixedComp::setLinkSurf(0,-SMap.realSurf(befilterindex+11)); /**Hay que respetar el numero de superficie a la que se liga el punto de vinculado (linking point)*/
  FixedComp::setLinkSurf(1,SMap.realSurf(befilterindex+12));
  FixedComp::setLinkSurf(2,-SMap.realSurf(befilterindex+13));
  FixedComp::setLinkSurf(3,SMap.realSurf(befilterindex+14));
  FixedComp::setLinkSurf(4,-SMap.realSurf(befilterindex+15));
  FixedComp::setLinkSurf(5,SMap.realSurf(befilterindex+16));
  
  return;
}

void
BeFilter::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const size_t sideIndex,
		    const attachSystem::ContainedComp& CC)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
    \param sideIndex :: Side index
    \param CC :: Moderator for containement [overkill]
   */
{
  ELog::RegMethod RegA("BeFilter","createAll");

  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System,CC);

  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE bibSystem
