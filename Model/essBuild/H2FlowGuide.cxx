/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/H2FlowGuide.cxx 
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include <array>
#include <algorithm>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
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
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "H2FlowGuide.h"

namespace essSystem
{

H2FlowGuide::H2FlowGuide(const std::string& baseKey,
			 const std::string& extraKey,
			 const std::string& finalKey ) :
  attachSystem::FixedComp(baseKey+extraKey+finalKey,6),
  baseName(baseKey),midName(extraKey),endName(finalKey)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param baseKey :: Butterfly main key
    \param extraKey :: H2Wing part name
    \param finalKey :: Specialized flow name
  */
{}

H2FlowGuide::H2FlowGuide(const H2FlowGuide& A) : 
  attachSystem::FixedComp(A),
  baseName(A.baseName),midName(A.midName),endName(A.endName),
  baseThick(A.baseThick),baseLen(A.baseLen),
  armThick(A.armThick), armLen(A.armLen),
  baseArmSep(A.baseArmSep),
  baseOffset(A.baseOffset),armOffset(A.armOffset),
  wallMat(A.wallMat),
  wallTemp(A.wallTemp)
  /*!
    Copy constructor
    \param A :: H2FlowGuide to copy
  */
{}

H2FlowGuide&
H2FlowGuide::operator=(const H2FlowGuide& A)
  /*!
    Assignment operator
    \param A :: H2FlowGuide to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      baseThick=A.baseThick;
      baseLen=A.baseLen;
      armThick=A.armThick;
      armLen=A.armLen;
      baseArmSep=A.baseArmSep;
      baseOffset=A.baseOffset;
      armOffset=A.armOffset;
      wallMat=A.wallMat;
      wallTemp=A.wallTemp;
    }
  return *this;
}

H2FlowGuide*
H2FlowGuide::clone() const
  /*!
    Virtual copy constructor
    \return new (this)
   */
{
  return new H2FlowGuide(*this);
}
 
H2FlowGuide::~H2FlowGuide() 
  /*!
    Destructor
  */
{}

void
H2FlowGuide::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("H2FlowGuide","populate");

  const std::string flowName(baseName+endName);
  baseThick=Control.EvalTail<double>(keyName,flowName,"BaseThick");
  baseLen=Control.EvalTail<double>(keyName,flowName,"BaseLen");
  baseArmSep=Control.EvalTail<double>(keyName,flowName,"BaseArmSep");
  baseOffset=Control.EvalTail<Geometry::Vec3D>(keyName,flowName,"BaseOffset");

  armThick=Control.EvalTail<double>(keyName,flowName,"ArmThick");
  armLen=Control.EvalTail<double>(keyName,flowName,"ArmLen");
  armOffset=Control.EvalTail<Geometry::Vec3D>(keyName,flowName,"ArmOffset");

  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat",
				     flowName+"WallMat");
  wallTemp=Control.EvalTail<double>(keyName,flowName,"WallTemp");
  
  return;
}
  
  
void
H2FlowGuide::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("H2FlowGuide","createSurface");

  // base
  ModelSupport::buildPlane(SMap,buildIndex+1,
			   Origin+Y*(baseOffset.Y()-baseThick/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,
			   Origin+Y*(baseOffset.Y()+baseThick/2.0),Y);

  ModelSupport::buildPlane(SMap,buildIndex+3,
			   Origin+X*(baseOffset.X()+armThick/2.0+baseArmSep),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,
			   Origin+X*(baseOffset.X()+armThick/2.0+baseArmSep+baseLen),X);

  ModelSupport::buildPlane(SMap,buildIndex+13,
			   Origin-X*(baseOffset.X()+armThick/2.0+baseArmSep),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,
			   Origin-X*(baseOffset.X()+armThick/2.0+baseArmSep+baseLen),X);

  // arm
  ModelSupport::buildPlane(SMap,buildIndex+101,
			   Origin+Y*(armOffset.Y()-armLen/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+102,
			   Origin+Y*(armOffset.Y()+armLen/2.0),Y);

  ModelSupport::buildPlane(SMap,buildIndex+103,
			   Origin+X*(armOffset.X()-armThick/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+104,
			   Origin+X*(armOffset.X()+armThick/2.0),X);
  

  return;
}
 
void
H2FlowGuide::createObjects(Simulation& System,
			   const attachSystem::FixedComp& HW)
  /*!
    Adds the main components
    \param System :: Simulation to create objects in
    \param HW :: H2Wing object
  */
{
  ELog::RegMethod RegA("H2FlowGuide","createObjects");

  const attachSystem::CellMap* CM=
    dynamic_cast<const attachSystem::CellMap*>(&HW);
  MonteCarlo::Object* InnerObj(0);
  int innerCell(0);
  if (CM)
    {
      innerCell=CM->getCell("Inner");
      InnerObj=System.findObject(innerCell);
    }
  if (!InnerObj)
    throw ColErr::InContainerError<int>
      (innerCell,"H2Wing inner Cell not found");
  
  std::string Out;
  const std::string topBottomStr=HW.getLinkString(13)+
    HW.getLinkString(14);
  HeadRule wallExclude;
  // base
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 3 -4 ");
  wallExclude.procString(Out); 
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,wallTemp,Out+topBottomStr));


  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 -13 14 ");
  wallExclude.addUnion(Out); 
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,wallTemp,Out+topBottomStr));

  // arm
  Out=ModelSupport::getComposite(SMap,buildIndex," 101 -102 103 -104 ");
  wallExclude.addUnion(Out);
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,wallTemp,Out+topBottomStr));

  wallExclude.makeComplement();
  InnerObj->addSurfString(wallExclude.display());
  
  return;
}
  

void
H2FlowGuide::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: The H2Wing for the flow
  */
{
  ELog::RegMethod RegA("H2FlowGuide","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System,FC);
  
  return;
}
  
}  // NAMESPACE essSystem
