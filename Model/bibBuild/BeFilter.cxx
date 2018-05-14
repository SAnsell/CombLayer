/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   bibBuild/BeFilter.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"

#include "BeFilter.h"


namespace bibSystem
{

BeFilter::BeFilter(const std::string& Key) :
  attachSystem::ContainedComp(),attachSystem::FixedOffset(Key,6),
  beFilterIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(beFilterIndex+1)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

BeFilter::BeFilter(const BeFilter& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  beFilterIndex(A.beFilterIndex),cellIndex(A.cellIndex),
  width(A.width),height(A.height),length(A.length),
  wallThick(A.wallThick),wallMat(A.wallMat),beMat(A.beMat),
  beTemp(A.beTemp)
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

  FixedOffset::populate(Control);

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
			   const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
    \param sideIndex :: link point [signed]
  */
{
  ELog::RegMethod RegA("BeFilter","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,0);
  Origin=FC.getLinkPt(sideIndex);
  applyOffset();
  Origin+=Y*wallThick;   // accommodate wall thickness
  return;
}

void
BeFilter::createSurfaces()
  /*!
    Create surface for fileter and surround
  */
{
  ELog::RegMethod RegA("BeFilter","createSurfaces");

  ModelSupport::buildPlane(SMap,beFilterIndex+1,Origin,Y);
  ModelSupport::buildCylinder(SMap,beFilterIndex+2,Origin,Z,length); 
  
  ModelSupport::buildPlane(SMap,beFilterIndex+3,Origin-X*width/2.0,X);  
  ModelSupport::buildPlane(SMap,beFilterIndex+4,Origin+X*width/2.0,X);  
  ModelSupport::buildPlane(SMap,beFilterIndex+5,Origin-Z*height/2.0,Z);  
  ModelSupport::buildPlane(SMap,beFilterIndex+6,Origin+Z*height/2.0,Z);  
  
  ModelSupport::buildPlane(SMap,beFilterIndex+11,Origin-Y*wallThick,Y);  
  ModelSupport::buildCylinder(SMap,beFilterIndex+12,Origin,Z,
			      length+wallThick);
  
  ModelSupport::buildPlane(SMap,beFilterIndex+13,
			   Origin-X*(width/2.0+wallThick),X);  
  ModelSupport::buildPlane(SMap,beFilterIndex+14,Origin+X*(width/2.0+wallThick),X);  
  ModelSupport::buildPlane(SMap,beFilterIndex+15,Origin-Z*(height/2.0+wallThick),Z);  
  ModelSupport::buildPlane(SMap,beFilterIndex+16,Origin+Z*(height/2.0+wallThick),Z);  


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
  Out=ModelSupport::getComposite(SMap,beFilterIndex,"1 -2 3 -4 5 -6");
  System.addCell(MonteCarlo::Qhull(cellIndex++,beMat,beTemp,Out));

  Out=ModelSupport::getComposite(SMap,beFilterIndex,
				 "11 -12 13 -14 15 -16  (-1:2:-3:4:-5:6)");
  Out+=CC.getContainer();
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,beTemp,Out));
  
  
  // Hay que añadir una superficie externa
  Out=ModelSupport::getComposite(SMap,beFilterIndex,"11 -12 13 -14 15 -16" );
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
  // Es importante decir donde estan los puntos,
  // y elegir la normal a los mismos.
  FixedComp::setConnect(0,Origin-Y*wallThick,-Y);
  FixedComp::setConnect(1,Origin+Y*(length+wallThick),Y);  
  FixedComp::setConnect(2,Origin-X*(width/2.0+wallThick),-X);  
  FixedComp::setConnect(3,Origin+X*(width/2.0+wallThick),X);  
  FixedComp::setConnect(4,Origin-Z*(height/2.0+wallThick),-Z);  
  FixedComp::setConnect(5,Origin+Z*(height/2.0+wallThick),Z);  

  //Hay que respetar el numero de superficie a la que se liga el 
  // punto de vinculado (linking point)
  FixedComp::setLinkSurf(0,-SMap.realSurf(beFilterIndex+11)); 
  FixedComp::setLinkSurf(1,SMap.realSurf(beFilterIndex+12));
  FixedComp::setLinkSurf(2,-SMap.realSurf(beFilterIndex+13));
  FixedComp::setLinkSurf(3,SMap.realSurf(beFilterIndex+14));
  FixedComp::setLinkSurf(4,-SMap.realSurf(beFilterIndex+15));
  FixedComp::setLinkSurf(5,SMap.realSurf(beFilterIndex+16));
  
  return;
}

void
BeFilter::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex,
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
