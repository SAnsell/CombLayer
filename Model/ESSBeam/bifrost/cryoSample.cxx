/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   cryoBuild/cryoSample.cxx
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
#include "surfDIter.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
#include "Quadratic.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "support.h"
#include "SurInter.h"
#include "stringCombine.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "LayerComp.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "surfDBase.h"
#include "mergeTemplate.h"
#include "SurfMap.h"

#include "cryoSample.h"


namespace cryoSystem
{

      
cryoSample::cryoSample(const std::string& Key) :
  attachSystem::FixedOffset(Key,6),attachSystem::ContainedComp(),
  attachSystem::CellMap(),attachSystem::SurfMap()
  //  buildIndex(ModelSupport::objectRegister::Instance().cell(Key)), 
  //  cellIndex(buildIndex+1)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}



cryoSample::cryoSample(const cryoSample& A) : 
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),attachSystem::CellMap(A),
  //  buildIndex(A.buildIndex),cellIndex(A.cellIndex),
  sampleRad(A.sampleRad), sampleHei(A.sampleHei),sTemp(A.sTemp),smat(A.smat),
  holderThickness(A.holderThickness),holderOuterRadius(A.holderOuterRadius),
  holderTop(A.holderTop),holderBott(A.holderBott),hTemp(A.hTemp),hmat(A.hmat)
  /*!
    Copy constructor
    \param A :: cryoTube to copy
  */
{}

cryoSample&
cryoSample::operator=(const cryoSample& A)
  /*!
    Assignment operator
    \param A :: cryoSample to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      cellIndex=A.cellIndex;
      sampleRad=A.sampleRad;
      sampleHei=A.sampleHei;
      sTemp=A.sTemp;
      smat=A.smat;
      holderThickness=A.holderThickness;
      holderOuterRadius=A.holderOuterRadius;
      holderTop=A.holderTop;
      holderBott=A.holderBott;
      hTemp=A.hTemp;
      hmat=A.hmat;
    }
  return *this;
}


  
  
cryoSample::~cryoSample()
  /*!
    Destructor
  */
{}


void
cryoSample::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("cryoSample","populate");

  FixedOffset::populate(Control);

  ELog::EM << keyName << ELog::endDiag;
// Master values
      sampleRad=Control.EvalVar<double>(keyName+"SampleRadius");
      sampleHei=Control.EvalVar<double>(keyName+"SampleHeight");
      sTemp=Control.EvalVar<double>(keyName+"SampleTemp");
      holderThickness=Control.EvalVar<double>(keyName+"HolderThickness");
      holderOuterRadius=Control.EvalVar<double>(keyName+"HolderOuterRadius");
      hTemp=Control.EvalVar<double>(keyName+"HolderTemp");
      holderTop=Control.EvalVar<double>(keyName+"HolderTop");
      holderBott=Control.EvalVar<double>(keyName+"HolderBottom");
      
     hmat=ModelSupport::EvalMat<int>(Control,keyName+"HolderMat");
     smat=ModelSupport::EvalMat<int>(Control,keyName+"SampleMat");
  
  
  return;
}

void
cryoSample::createUnitVector(const attachSystem::FixedComp& FC,
			   const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("cryoSample","createUnitVector");

  const attachSystem::LinkUnit& LU=FC.getLU(sideIndex);
  /* std::cout << LU.getAxis()<< std::endl;
   std::cout << FC.getX()<< std::endl;
   std::cout << FC.getY()<< std::endl;
   std::cout << FC.getZ()<< std::endl;
  */  
    attachSystem::FixedComp::createUnitVector(FC,sideIndex);
    /* std::cout << FC.getX()<< std::endl;
   std::cout << FC.getY()<< std::endl;
   std::cout << FC.getZ()<< std::endl;*/
    applyOffset();
  /* if ((sideIndex!=0)&&(sideIndex<=4)) yStep+=holderOuterRadius;
     applyOffset();*/
  if ((sideIndex!=0)&&(sideIndex<=4))
    attachSystem::FixedComp::applyShift(0.0,holderOuterRadius,0.0);
 
  return;
}

void
cryoSample::createSurfaces()
  /*!
    Create cylinders and planes for the sample holder and the sample
  */
{
  ELog::RegMethod RegA("cryoSample","createSurfaces");
  //Holder Outer surface
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Z,holderOuterRadius);
  //Holder Inner surface
  ModelSupport::buildCylinder(SMap,buildIndex+8,Origin,Z,holderOuterRadius-holderThickness);
  //Holder top and bottom
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(holderTop),Z);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(holderBott),Z);  
  
  //Cylindrical sample
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Z,sampleRad);
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(0.5*sampleHei),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(0.5*sampleHei),Z);

  //Horizontal plane at sample position for linking
  ModelSupport::buildPlane(SMap,buildIndex+400,Origin,Z);
  
  return; 
}

void
cryoSample::createObjects(Simulation& System)
  /*!
    Create sample holder and sample cells
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("cryoSample","createObjects");

  std::string Out;
  //Sample
  
  Out=ModelSupport::getComposite(SMap,buildIndex,"15 -16 -17 ");
    System.addCell(MonteCarlo::Object(cellIndex++,smat,0.0,Out));

  //Void in the sample holder around the sample
  Out=ModelSupport::getComposite(SMap,buildIndex," 5 -6 -8 ( -15: 16 : 17 )");
    System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));

  //Sample holder
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " 5 -6 -7 8");
   
  System.addCell(MonteCarlo::Object(cellIndex++,hmat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,buildIndex," 5 -6 -7 "); //cylinder, outside

      
  addOuterSurf(Out); // border will be what is out of cylinder above


  return; 
}

void
cryoSample::createLinks()
  /*!
    Creates a full attachment set
   Link points and link surfaces
 */
{  
  ELog::RegMethod RegA("cryoSample","createLinks");
  //Link point at the sample position
  //Along the beam
  FixedComp::setConnect(0,Origin-Y*(holderOuterRadius),-Y);
  FixedComp::setLinkSurf(0,SMap.realSurf(buildIndex+7));
  FixedComp::setConnect(1,Origin+Y*(holderOuterRadius),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+7));
  //Perp to beam
  FixedComp::setConnect(2,Origin-X*(holderOuterRadius),-X);
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+7));
  FixedComp::setConnect(3,Origin+X*(holderOuterRadius),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+7));
  //Vertical dimensions of sample holder
  FixedComp::setConnect(4,Origin+Z*(holderTop),-Z);
  FixedComp::setLinkSurf(4,SMap.realSurf(buildIndex+5));
  FixedComp::setConnect(5,Origin-Z*(holderBott),Z);
  FixedComp::setLinkSurf(5,-SMap.realSurf(buildIndex+6));


  
  return;
}

void
cryoSample::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
    \param sideIndex :: surface offset
   */
{
  ELog::RegMethod RegA("cryoSample","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE constructSystem
