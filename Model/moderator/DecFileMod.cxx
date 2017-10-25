/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   moderator/DecFileMod.cxx
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
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quaternion.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "Vertex.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ReadFunctions.h"
#include "ModelSupport.h"
#include "ObjSurfMap.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "Decoupled.h"
#include "DecFileMod.h"

namespace moderatorSystem
{

DecFileMod::DecFileMod(const std::string& Key)  :
  Decoupled(Key),
  incIndex(ModelSupport::objectRegister::Instance().cell(Key+"IncFile")),
  cellIndex(incIndex+1),RBase(3,3)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for base decoupled object
    \
  */
{}

DecFileMod::DecFileMod(const DecFileMod& A) : 
  Decoupled(A),
  incIndex(A.incIndex),cellIndex(A.cellIndex),RBase(A.RBase)
  /*!
    Copy constructor
    \param A :: DecFileMod to copy
  */
{}

DecFileMod&
DecFileMod::operator=(const DecFileMod& A)
  /*!
    Assignment operator
    \param A :: DecFileMod to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Decoupled::operator=(A);
      incIndex=A.incIndex;
      cellIndex=A.cellIndex;
      RBase=A.RBase;
    }
  return *this;
}

DecFileMod::~DecFileMod() 
  /*!
    Destructor
  */
{}

void
DecFileMod::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase to used
  */
{
  ELog::RegMethod RegA("DecFileMod","populate");

  FixedOffset::populate(Control);
  // Master values
  
  populated |= 1;  
  return;
}


void
DecFileMod::createUnitVector(const attachSystem::FixedComp&,
			     const long int)
  /*!
    Create the unit vectors: 
    Note that this is a modification since Decoupled::createUnitVector
    has already been called.
  */
{
  for(size_t i=0;i<3;i++)
    {
      RBase[i][0]=X[i];
      RBase[i][1]=Y[i];
      RBase[i][2]=Z[i];
    }
  return;
}


int
DecFileMod::readFile(Simulation& System,const std::string& FName)
  /*!
    Process the reading of an MCNPX insert file
    \param System :: Simulation to update
    \param FName :: file to read
    \return 0 on success / -ve on failure
  */
{
  ELog::RegMethod RegA("DecFileMod","readFile");

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
  
  const int surfCnt=ReadFunc::readSurfaces(DB,IX,incIndex);
  ELog::EM<<"Read "<<surfCnt<<" extra surfaces for "<<keyName<<ELog::endDebug;
 
  // REBASE and recentre
  ModelSupport::surfIndex& SI=
    ModelSupport::surfIndex::Instance();
  const ModelSupport::surfIndex::STYPE& SMap
    =ModelSupport::surfIndex::Instance().surMap();
  ModelSupport::surfIndex::STYPE::const_iterator sc;
  for(sc=SMap.begin();sc!=SMap.end();sc++)
    {
      if (sc->first>incIndex && sc->first<incIndex+10000)
	{
	  sc->second->rotate(RBase);
	  sc->second->displace(Origin);
	}
    }
  reMapSurf(OMap);

  // RE-ADJUST 
  System.setMaterialDensity(OMap);
  std::map<int,Geometry::Surface*> EQMap;
  if (SI.findEqualSurf(incIndex,incIndex+10000,EQMap))
    ModelSupport::ObjSurfMap::removeEqualSurf(EQMap,OMap);

  ReadFunc::OTYPE& SysOMap=System.getCells();
  ReadFunc::mapInsert(OMap,SysOMap);
  
  return 0; 
}

void
DecFileMod::reMapSurf(ReadFunc::OTYPE& ObjMap) const
  /*!
    Given a set of Objects, remap the surfaces so they are 
    displaced by incIndex.
    \param ObjMap :: Map of objects
  */
{
  ELog::RegMethod RegA("DecFileMod","reMapSurf");
  ReadFunc::OTYPE::iterator mc;
  
  for(mc=ObjMap.begin();mc!=ObjMap.end();mc++)
    {
      std::string cellStr=mc->second->cellCompStr();
      cellStr=ModelSupport::getComposite(SMap,incIndex,cellStr);
      mc->second->procString(cellStr);
    }
  return;
}
  
void
DecFileMod::createAllFromFile(Simulation& System,
			      const attachSystem::FixedComp& FC,
			      const long int sideIndex,
			      const std::string& FName)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FName :: DecFileMod file to be reading
  */
{
  ELog::RegMethod RegA("DecFileMod","createAll");

  // Check that everything from here is called in Decoupled:
  Decoupled::createAll(System,FC,sideIndex);
  createUnitVector(FC,sideIndex);
  readFile(System,FName);

  insertObjects(System);
  System.removeCell(methCell);
  System.validateObjSurfMap();
  return;
}
  
}  // NAMESPACE shutterSystem
