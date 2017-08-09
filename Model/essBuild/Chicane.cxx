/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/Chicane.cxx
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "inputParam.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h"
#include "FixedOffset.h"
#include "FrontBackCut.h"
#include "Bunker.h"
#include "Chicane.h"

namespace essSystem
{

Chicane::Chicane(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,6),
  chicaneIndex(ModelSupport::objectRegister::Instance().cell(keyName)),
  cellIndex(chicaneIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

Chicane::Chicane(const Chicane& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  attachSystem::FrontBackCut(A),
  chicaneIndex(A.chicaneIndex),
  cellIndex(A.cellIndex),nBlock(A.nBlock),CUnits(A.CUnits)
  /*!
    Copy constructor
    \param A :: Chicane to copy
  */
{}

Chicane&
Chicane::operator=(const Chicane& A)
  /*!
    Assignment operator
    \param A :: Chicane to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::FrontBackCut::operator=(A);
      cellIndex=A.cellIndex;
      nBlock=A.nBlock;
      CUnits=A.CUnits;
    }
  return *this;
}


Chicane::~Chicane() 
  /*!
    Destructor
  */
{}

void
Chicane::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("Chicane","populate");

  FixedOffset::populate(Control);
  
  nBlock=Control.EvalVar<size_t>(keyName+"NBlocks");
  for (size_t i=0;i<nBlock;i++)
    {
      const std::string nStr(std::to_string(i));
      const double l=Control.EvalPair<double>
        (keyName+"Length"+nStr,keyName+"Length");
      const double w=Control.EvalPair<double>
        (keyName+"Width"+nStr,keyName+"Width");
      const double h=Control.EvalPair<double>
        (keyName+"Height"+nStr,keyName+"Height");
      const int matN=ModelSupport::EvalMat<int>
        (Control,keyName+"Mat"+nStr,keyName+"Mat");
      CUnits.push_back(chicaneUnit(l,w,h,matN));
    }

  
  return;
}

  
void
Chicane::createUnitVector(const Bunker& BUnit,
                          const size_t segIndex)
  /*!
    Create the unit vectors
    \param BUnit :: Bunker unit
    \param segIndex :: Segment number
  */
{
  ELog::RegMethod RegA("Chicane","createUnitVector(Bunker)");

  Geometry::Vec3D DPos,DX,DY,DZ;
  BUnit.calcSegPosition(segIndex,DPos,DX,DY,DZ);
  attachSystem::FixedComp::createUnitVector(DPos,DX,DY,DZ);
  applyOffset();
  return;
}

  
void
Chicane::createUnitVector(const attachSystem::FixedComp& FC,
                          const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: object for origin
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("Chicane","createUnitVector(FC)");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  return;
}
  
void
Chicane::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("Chicane","createSurfaces");
  
  int CIndex(chicaneIndex);
  Geometry::Vec3D blockOrg(Origin);
  for(const chicaneUnit& CU : CUnits)
    {
      ModelSupport::buildPlane(SMap,CIndex+1,blockOrg,Y);
      ModelSupport::buildPlane(SMap,CIndex+2,blockOrg+Y*CU.length,Y);
      
      ModelSupport::buildPlane(SMap,CIndex+3,blockOrg-X*(CU.width/2.0),X);
      ModelSupport::buildPlane(SMap,CIndex+4,blockOrg+X*(CU.width/2.0),X);
  
      ModelSupport::buildPlane(SMap,CIndex+5,blockOrg-Z*(CU.height/2.0),Z);
      ModelSupport::buildPlane(SMap,CIndex+6,blockOrg+Z*(CU.height/2.0),Z);

      blockOrg+=Y*CU.length;
      CIndex+=100;
    }

  return;
}
  
void
Chicane::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("Chicane","createObjects");


  std::string Out;
  int CIndex(chicaneIndex);
  for(const chicaneUnit& CU : CUnits)
    {
      Out=ModelSupport::getComposite(SMap,CIndex," 1 -2 3 -4 5 -6 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,CU.mat,0.0,Out));
      addOuterUnionSurf(Out);
      CIndex+=100;
    }
  return;
}

  
void
Chicane::createLinks()

  /*!
    Create all the links
  */
{
  ELog::RegMethod RegA("Chicane","createLinks");

  attachSystem::FixedComp::setNConnect(10*CUnits.size()+4);
  
  Geometry::Vec3D blockOrg(Origin);
  int CIndex(chicaneIndex);
  size_t linkOffset(0);
  std::vector<Geometry::Vec3D> Delta;
  for(const chicaneUnit& CU : CUnits)
    {
      Delta=std::vector<Geometry::Vec3D>(
        { -X*(CU.width/2.0),X*(CU.width/2.0),
            -Z*(CU.height/2.0),Z*(CU.height/2.0)
            }
        );
      FixedComp::setConnect(linkOffset+0,blockOrg,-Y);
      FixedComp::setLinkSurf(linkOffset+0,-SMap.realSurf(CIndex+1));

      FixedComp::setConnect(linkOffset+1,blockOrg+Y*CU.length,Y);
      FixedComp::setLinkSurf(linkOffset+1,SMap.realSurf(CIndex+2));

      for(size_t i=0;i<4;i++)
        {
          FixedComp::setConnect(linkOffset+6+i,
                                blockOrg+Delta[i % 2]+Delta[2+i/2],-Y);
          FixedComp::setLinkSurf(linkOffset+6+i,-SMap.realSurf(CIndex+1));
        }          
      // Go to mid point
      blockOrg+=Y*(CU.length/2.0);
      FixedComp::setConnect(linkOffset+2,blockOrg+Delta[0],-X);
      FixedComp::setLinkSurf(linkOffset+2,-SMap.realSurf(CIndex+3));

      FixedComp::setConnect(linkOffset+3,blockOrg+Delta[1],X);
      FixedComp::setLinkSurf(linkOffset+3,SMap.realSurf(CIndex+4));

      FixedComp::setConnect(linkOffset+4,blockOrg+Delta[2],-Z);
      FixedComp::setLinkSurf(linkOffset+4,-SMap.realSurf(CIndex+5));

      FixedComp::setConnect(linkOffset+5,blockOrg+Delta[3],Z);
      FixedComp::setLinkSurf(linkOffset+5,SMap.realSurf(CIndex+6));

      linkOffset+=10;
      blockOrg+=Y*(CU.length/2.0);      
      CIndex+=100;
    }
  // Put in last points
  if (!CUnits.empty())
    {
      CIndex-=100;
      for(size_t i=0;i<4;i++)
        {
          FixedComp::setConnect(linkOffset+i,
                                blockOrg+Delta[i % 2]+Delta[2+i/2],Y);
          FixedComp::setLinkSurf(linkOffset+i,-SMap.realSurf(CIndex+1));
        }
    }
  return;
}
  
  

  
void
Chicane::createAll(Simulation& System,
		   const attachSystem::FixedComp& FC,
		   const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedPoint for centre
    \param sideIndex  :: link point to the inner roof of Bunker
  */
{
  ELog::RegMethod RegA("Chicane","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);              

  return;
}
  
void
Chicane::createAll(Simulation& System,
		   const Bunker& BUnit,
		   const size_t segIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param BUnit :: Bunker component
    \param segIndex :: Segment
  */
{
  ELog::RegMethod RegA("Chicane","createAll");

  populate(System.getDataBase());
  createUnitVector(BUnit,segIndex);

  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);              

  return;
}

}  // essSystem essSystem
