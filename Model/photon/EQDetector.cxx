/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   photon/EQDetector.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "surfRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
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
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "EQDetector.h"

namespace photonSystem
{
      
EQDetector::EQDetector(const std::string& Key) :
  attachSystem::ContainedComp(),attachSystem::FixedOffset(Key,6)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

EQDetector::EQDetector(const EQDetector& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  yOffset(A.yOffset),
  radius(A.radius),length(A.length),boxLength(A.boxLength),
  boxHeight(A.boxHeight),boxWidth(A.boxWidth),boxLead(A.boxLead),
  boxPlastic(A.boxPlastic),viewRadius(A.viewRadius),
  plasticMat(A.plasticMat),detMat(A.detMat),leadMat(A.leadMat)
  /*!
    Copy constructor
    \param A :: EQDetector to copy
  */
{}

EQDetector&
EQDetector::operator=(const EQDetector& A)
  /*!
    Assignment operator
    \param A :: EQDetector to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      yOffset=A.yOffset;
      radius=A.radius;
      length=A.length;
      boxLength=A.boxLength;
      boxHeight=A.boxHeight;
      boxWidth=A.boxWidth;
      boxLead=A.boxLead;
      boxPlastic=A.boxPlastic;
      viewRadius=A.viewRadius;
      plasticMat=A.plasticMat;
      detMat=A.detMat;
      leadMat=A.leadMat;
    }
  return *this;
}

EQDetector::~EQDetector()
  /*!
    Destructor
  */
{}

EQDetector*
EQDetector::clone() const
  /*!
    Clone copy constructor
    \return copy of this
  */
{
  return new EQDetector(*this);
}

void
EQDetector::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("EQDetector","populate");

  FixedOffset::populate(Control);
  yOffset=Control.EvalVar<double>(keyName+"YOffset");
  radius=Control.EvalVar<double>(keyName+"Radius");
  length=Control.EvalVar<double>(keyName+"Length");

  boxLength=Control.EvalVar<double>(keyName+"BoxLength");
  boxHeight=Control.EvalVar<double>(keyName+"BoxHeight");
  boxWidth=Control.EvalVar<double>(keyName+"BoxWidth");
  boxLead=Control.EvalVar<double>(keyName+"BoxLead");
  boxPlastic=Control.EvalVar<double>(keyName+"BoxPlastic");
  viewRadius=Control.EvalVar<double>(keyName+"ViewRadius");

  detMat=ModelSupport::EvalMat<int>(Control,keyName+"DetMat");
  leadMat=ModelSupport::EvalMat<int>(Control,keyName+"LeadMat");
  plasticMat=ModelSupport::EvalMat<int>(Control,keyName+"PlasticMat");
  
  return;
}

void
EQDetector::createUnitVector(const attachSystem::FixedComp& FC,
				const long int sideIndex)
  /*!
    Create the unit vectors
    - Y Down the beamline
    \param FC :: Linked object
    \param sideIndex :: sinde track
  */
{
  ELog::RegMethod RegA("EQDetector","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  ELog::EM<<"Origin == "<<Origin<<ELog::endDiag;
  return;
}

void
EQDetector::createSurfaces()
  /*!
    Create surface for the collimator
  */
{
  ELog::RegMethod RegA("EQDetector","createSurfaces");

  // Outer surfaces:
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin+Y*yOffset,Y);  
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(yOffset+length),Y);  
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,radius);

  // Inner void
  ModelSupport::buildPlane(SMap,buildIndex+101,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+102,Origin+Y*boxLength,Y);
  ModelSupport::buildPlane(SMap,buildIndex+103,Origin-X*(boxWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+104,Origin+X*(boxWidth/2.0),X);  
  ModelSupport::buildPlane(SMap,buildIndex+105,Origin-Z*(boxHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+106,Origin+Z*(boxHeight/2.0),Z);  

  // Lead Wall [Inner]
  ModelSupport::buildPlane(SMap,buildIndex+201,Origin-Y*boxLead,Y);
  ModelSupport::buildPlane(SMap,buildIndex+202,Origin+Y*(boxLength+boxLead),Y);
  ModelSupport::buildPlane(SMap,buildIndex+203,
			   Origin-X*(boxLead+boxWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+204,
			   Origin+X*(boxLead+boxWidth/2.0),X);  
  ModelSupport::buildPlane(SMap,buildIndex+205,
			   Origin-Z*(boxLead+boxHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+206,
			   Origin+Z*(boxLead+boxHeight/2.0),Z);  

  // Plastic Wall [Outer]

  ModelSupport::buildPlane(SMap,buildIndex+302,
			   Origin+Y*(boxLength+boxPlastic+boxLead),Y);
  ModelSupport::buildPlane(SMap,buildIndex+303,
			   Origin-X*(boxPlastic+boxLead+boxWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+304,
			   Origin+X*(boxPlastic+boxLead+boxWidth/2.0),X);  
  ModelSupport::buildPlane(SMap,buildIndex+305,
			   Origin-Z*(boxPlastic+boxLead+boxHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+306,
			   Origin+Z*(boxPlastic+boxLead+boxHeight/2.0),Z);  


  // Cut out in front
  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Y,viewRadius);  
  return; 
}

void
EQDetector::createObjects(Simulation& System)
  /*!
    Create the collimator
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("EQDetector","createObjects");

  std::string Out;
  // Detector
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 -7 ");
  System.addCell(MonteCarlo::Object(cellIndex++,detMat,0.0,Out));

  // void
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "101 -102 103 -104 105 -106 (-1:2:7) ");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));
  // lead liner
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "201 -202 203 -204 205 -206 "
				 "(102:-103:104:-105:106)");
  System.addCell(MonteCarlo::Object(cellIndex++,leadMat,0.0,Out));

  // plastic liner
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "201 -302 303 -304 305 -306 "
				 "(202:-203:204:-205:206)");
  System.addCell(MonteCarlo::Object(cellIndex++,plasticMat,0.0,Out));


  // Front wall 
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "201 -101 103 -104 105 -106 107");
  System.addCell(MonteCarlo::Object(cellIndex++,leadMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,buildIndex,"201 -101 -107");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));

  Out=ModelSupport::getComposite(SMap,buildIndex,"201 -302 303 -304 305 -306");
  addOuterSurf(Out);
  return; 
}

void
EQDetector::createLinks()
  /*!
    Creates a full attachment set
  */
{  
  ELog::RegMethod RegA("EQDetector","createLinks");
  
  FixedComp::setConnect(0,Origin-Y*boxLead,-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+201));

  FixedComp::setConnect(1,Origin-Y*(boxLength+boxPlastic+boxLead),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+302));

  FixedComp::setConnect(2,Origin-X*(boxPlastic+boxLead+boxWidth/2.0),-X);
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+303));

  FixedComp::setConnect(3,Origin+X*(boxPlastic+boxLead+boxWidth/2.0),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+304));

  FixedComp::setConnect(4,Origin-Z*(boxPlastic+boxLead+boxHeight/2.0),-Z);
  FixedComp::setLinkSurf(4,SMap.realSurf(buildIndex+305));

  FixedComp::setConnect(5,Origin-Z*(boxPlastic+boxLead+boxHeight/2.0),-Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+306));

  return;
}

void
EQDetector::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
    \param sideIndex :: surface offset
   */
{
  ELog::RegMethod RegA("EQDetector","createAll");
  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE photonSystem
