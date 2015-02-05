/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   attachComp/InsertObject.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include <functional>
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Tensor.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
#include "surfDIter.h"
#include "Quaternion.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "SimProcess.h"
#include "Simulation.h"
#include "ReadFunctions.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "InsertObject.h"

namespace attachSystem
{

InsertObject::InsertObject(const std::string& Key)  :
  ContainedComp(),FixedComp(Key,0),
  surfIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(surfIndex+1),populated(0),RBase(3,3)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param N :: Index value
    \param Key :: Name for item in search
  */
{}

InsertObject::InsertObject(const InsertObject& A) : 
  ContainedComp(A),FixedComp(A),
  surfIndex(A.surfIndex),cellIndex(A.cellIndex),
  populated(A.populated),xStep(A.xStep),yStep(A.yStep),zStep(A.zStep),
  xyAngle(A.xyAngle),zAngle(A.zAngle),xMinus(A.xMinus),xPlus(A.xPlus),
  yMinus(A.yMinus),yPlus(A.yPlus),zMinus(A.zMinus),zPlus(A.zPlus),
  RBase(A.RBase)
  /*!
    Copy constructor
    \param A :: InsertObject to copy
  */
{}

InsertObject&
InsertObject::operator=(const InsertObject& A)
  /*!
    Assignment operator
    \param A :: InsertObject to copy
    \return *this
  */
{
  if (this!=&A)
    {
      ContainedComp::operator=(A);
      FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      populated=A.populated;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
      xMinus=A.xMinus;
      xPlus=A.xPlus;
      yMinus=A.yMinus;
      yPlus=A.yPlus;
      zMinus=A.zMinus;
      zPlus=A.zPlus;
      RBase=A.RBase;
    }
  return *this;
}

InsertObject::~InsertObject() 
  /*!
    Destructor
  */
{}

void
InsertObject::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("InsertObject","populate");

  const FuncDataBase& Control=System.getDataBase();

  // Master values
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYangle");
  zAngle=Control.EvalVar<double>(keyName+"Zangle");
  
  populated |= 1;  
  return;
}


void
InsertObject::createUnitVector(const FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component to use as master origin
  */
{
  ELog::RegMethod RegA("InsertObject","createUnitVector");

  attachSystem::FixedComp::createUnitVector(FC);

  Origin+=X*xStep+Y*yStep+Z*zStep;
  const Geometry::Quaternion Qz=
    Geometry::Quaternion::calcQRotDeg(zAngle,X);
  const Geometry::Quaternion Qxy=
    Geometry::Quaternion::calcQRotDeg(xyAngle,Z);
  Qz.rotate(Y);
  Qz.rotate(Z);
  Qxy.rotate(Y);
  Qxy.rotate(X);
  Qxy.rotate(Z);
  
  for(size_t i=0;i<3;i++)
    {
      RBase[i][0]=X[i];
      RBase[i][1]=Y[i];
      RBase[i][2]=Z[i];
    }

  return;
}

void
InsertObject::createSurfaces()
  /*!
    Create All the surfaces
    Goes throught the insertCell to get the outside surfaces
    registered as 1-6 using normal convension on X-Y-Z
  */
{
  ELog::RegMethod RegA("InsertObject","createSurface");
  
  ModelSupport::buildPlane(SMap,surfIndex+1,Origin-Y*yMinus,Y);
  ModelSupport::buildPlane(SMap,surfIndex+2,Origin-Y*yMinus,Y);
  ModelSupport::buildPlane(SMap,surfIndex+3,Origin-X*xMinus,X);
  ModelSupport::buildPlane(SMap,surfIndex+4,Origin+X*xMinus,X);
  ModelSupport::buildPlane(SMap,surfIndex+5,Origin-Z*zMinus,Z);
  ModelSupport::buildPlane(SMap,surfIndex+6,Origin+Z*zMinus,Z);

  return;
}

void
InsertObject::createObjects()
  /*!
    Adds the Chip guide components : only called if 
  */
{
  ELog::RegMethod RegA("InsertObject","createObjects");
  std::string Out;
  Out=ModelSupport::getComposite(SMap,surfIndex,"1 -2 3 -4 5 -6");
  addOuterSurf(Out);

  return;
}


int
InsertObject::readFile(Simulation& System,const std::string& FName)
  /*!
    Process the reading of an MCNPX insert file
    \param System :: Simulation to update
    \param FName :: file to read
    \return 0 on success / -ve on failure
  */
{
  ELog::RegMethod RegA("InsertObject","readFile");

  ReadFunc::OTYPE OMap;
  std::ifstream IX;
  if (!FName.empty())
    IX.open(FName.c_str());
  if (FName.empty() || !IX.good())
    {
      ELog::EM<<"Failed to open file:"<<FName<<ELog::endErr;
      return -1;
    }
  FuncDataBase& DB=System.getDataBase();
  const int cellCnt=ReadFunc::readCells(DB,IX,cellIndex,OMap);
  
  ELog::EM<<"Read "<<cellCnt<<" extra cells for "<<keyName<<ELog::endDiag;
  if (cellCnt<=0)
    return -2;
  cellIndex+=cellCnt;
  
  const int surfCnt=ReadFunc::readSurfaces(DB,IX,surfIndex);
  ELog::EM<<"Read "<<surfCnt<<" extra surfaces for "<<keyName<<ELog::endDebug;
 
  // REBASE and recentre
  const ModelSupport::surfIndex::STYPE& SMap
    =ModelSupport::surfIndex::Instance().surMap();
  ModelSupport::surfIndex::STYPE::const_iterator sc;
  for(sc=SMap.begin();sc!=SMap.end();sc++)
    {
      if (sc->first>surfIndex && sc->first<surfIndex+10000)
	{
	  ELog::EM<<"Displacing :"<<sc->first<<" by "<<Origin<<ELog::endWarn;
	  sc->second->rotate(RBase);
	  sc->second->displace(Origin);
	}
    }
  reMapSurf(OMap);

  // RE-ADJUST 
  System.setMaterialDensity(OMap);
  ReadFunc::OTYPE& SysOMap=System.getCells();
  ReadFunc::mapInsert(OMap,SysOMap);
  
  return 0; 
}

void
InsertObject::reMapSurf(ReadFunc::OTYPE& ObjMap) const
  /*!
    Given a set of Objects, remap the surfaces so they are 
    displaced by stopIndex.
    \param ObjMap :: Map of objects
  */
{
  ELog::RegMethod RegA("InsertObject","reMapSurf");
  ReadFunc::OTYPE::iterator mc;
  
  for(mc=ObjMap.begin();mc!=ObjMap.end();mc++)
    {
      std::string cellStr=mc->second->cellCompStr();
      cellStr=ModelSupport::getComposite(SMap,surfIndex,cellStr);
      mc->second->procString(cellStr);
    }
  return;
}
  
void
InsertObject::createAll(Simulation& System,
		    const FixedComp& FC,
		    const std::string& FName)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed Component
    \param FName :: InsertObject file to be readin
  */
{
  ELog::RegMethod RegA("InsertObject","createAll");
  populate(System);
  createUnitVector(FC);
  createSurfaces();  
  readFile(System,FName);
  insertObjects(System);
 
  return;
}
  
}  // NAMESPACE shutterSystem
