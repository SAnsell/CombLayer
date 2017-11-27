/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   chip/ChipSample.cxx
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
#include "chipDataStore.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ChipSample.h"

namespace hutchSystem
{

ChipSample::ChipSample(const std::string& Key,const size_t Index) :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key+StrFunc::makeString(Index),2),
  ID(Index),baseName(Key),
  csIndex(ModelSupport::objectRegister::Instance().cell(keyName)),
  cellIndex(csIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
    \param Index :: Id number
  */
{}

ChipSample::ChipSample(const ChipSample& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  ID(A.ID),baseName(A.baseName),csIndex(A.csIndex),
  cellIndex(A.cellIndex),tableNum(A.tableNum),width(A.width),
  height(A.height),length(A.length),defMat(A.defMat)
  /*!
    Copy constructor
    \param A :: ChipSample to copy
  */
{}

ChipSample&
ChipSample::operator=(const ChipSample& A)
  /*!
    Assignment operator
    \param A :: ChipSample to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      cellIndex=A.cellIndex;
      tableNum=A.tableNum;
      zAngle=A.zAngle;
      width=A.width;
      height=A.height;
      length=A.length;
      defMat=A.defMat;
    }
  return *this;
}

ChipSample::~ChipSample() 
  /*!
    Destructor
  */
{}

void
ChipSample::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Funciont
  */
{
  ELog::RegMethod RegA("ChipSample","populate");

  FixedOffset::populate(baseName,Control);
  
  tableNum=Control.EvalPair<int>(keyName,baseName,"TableNum");
  
  width=Control.EvalPair<double>(keyName,baseName,"Width");
  height=Control.EvalPair<double>(keyName,baseName,"Height");
  length=Control.EvalPair<double>(keyName,baseName,"Depth");
  defMat=ModelSupport::EvalMat<int>(Control,keyName+"DefMat",
					baseName+"DefMat");
      
  return;
}

void
ChipSample::createUnitVector(const FixedComp& TC,
			     const long int sideIndex)
  /*!
    Create the unit vectors
    \param TC :: Table to attach sample to
    \param sideIndex :: Table link point
  */
{
  ELog::RegMethod RegA("ChipSample","createUnitVector");

  // Origin is in the wrong place as it is at the EXIT:
  FixedComp::createUnitVector(TC,sideIndex);
  applyOffset();

  return;
}

void
ChipSample::createSurfaces()
  /*!
    Create all the surfaces
  */
{
  ELog::RegMethod RegA("ChipSample","createSurface");
  // Sides
  ModelSupport::buildPlane(SMap,csIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,csIndex+2,Origin+Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,csIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,csIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,csIndex+5,Origin,Z);
  ModelSupport::buildPlane(SMap,csIndex+6,Origin+Z*height,Z);
  
  return;
}

void
ChipSample::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("ChipSample","createObjects");
  
  std::string Out;
  // Master box: 
  Out=ModelSupport::getComposite(SMap,csIndex,"1 -2 3 -4 5 -6");
  System.addCell(MonteCarlo::Qhull(cellIndex++,defMat,0.0,Out));

  addOuterSurf(Out);
    
  return;
}

void
ChipSample::createLinks()
  /*!
    Create links for the sample
  */
{
  ELog::RegMethod RegA("ChipSample","createLinks");
  
  FixedComp::setConnect(0,Origin-Y*(length/2.0),-Y);
  FixedComp::setConnect(1,Origin+Y*(length/2.0),Y);

  FixedComp::setLinkSurf(0,-SMap.realSurf(csIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(csIndex+2));

  return;
}
  
void
ChipSample::createAll(Simulation& System,
		      const attachSystem::FixedComp& TCA,
		      const attachSystem::FixedComp& TCB)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param TCA :: First Table 
    \param TCB :: Second Table 
  */
{
  ELog::RegMethod RegA("ChipSample","createAll");

  populate(System.getDataBase());
  if (tableNum==1)
    createUnitVector(TCA,5);
  else
    createUnitVector(TCB,5);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}
  
}  // NAMESPACE shutterSystem
