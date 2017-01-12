/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/CompBInsert.cxx
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
#include <numeric>

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
#include "SurInter.h"
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
#include "ReadFunctions.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "FrontBackCut.h"
#include "Bunker.h"
#include "CompBInsert.h"

namespace essSystem
{

CompBInsert::CompBInsert(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedOffset(Key,7),
  attachSystem::CellMap(),attachSystem::FrontBackCut(),
  insIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(insIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}


CompBInsert::~CompBInsert() 
  /*!
    Destructor
  */
{}

void
CompBInsert::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("CompBInsert","populate");
  FixedOffset::populate(Control);

  NBox=Control.EvalVar<size_t>(keyName+"NBox");

  double H,W,L;
  int M;
  for(size_t index=0;index<NBox;index++)
    {
      const std::string SNum=StrFunc::makeString(index);
      H=Control.EvalPair<double>(keyName+"Height"+SNum,keyName+"Height");
      W=Control.EvalPair<double>(keyName+"Width"+SNum,keyName+"Width");
      L=Control.EvalPair<double>(keyName+"Length"+SNum,keyName+"Length");
      M=ModelSupport::EvalMat<int>(Control,keyName+"Mat"+SNum,keyName+"Mat");

      height.push_back(H);
      width.push_back(W);
      length.push_back(L);
      mat.push_back(M);
    }
  
  NWall=Control.EvalVar<size_t>(keyName+"NWall");
  double T;
  for(size_t index=0;index<NWall;index++)
    {
      const std::string SNum=StrFunc::makeString(index);                                                                
      T=Control.EvalPair<double>(keyName+"WallThick"+SNum,keyName+"WallThick");
      M=ModelSupport::EvalMat<int>(Control,keyName+"WallMat"+SNum,keyName+"WallMat");
      wallThick.push_back(T);
      wallMat.push_back(M);
    }
  wallThick.push_back(0.0);
  
  return;
}
  
void
CompBInsert::createUnitVector(const attachSystem::FixedComp& FC,
                              const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Linked object
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("CompBInsert","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  return;
}
  
void
CompBInsert::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("CompBInsert","createSurface");

  int SI(insIndex);
  double L(0.0);
  for(size_t index=0;index<NBox;index++)
    {
      double T(0.0);
      int SWI(SI);
      for(size_t wallIndex=0;wallIndex<=NWall;wallIndex++)
        {
	  ModelSupport::buildPlane(SMap,SWI+1,Origin+Y*(T+L),Y);
          ModelSupport::buildPlane(SMap,SWI+3,Origin-X*(T+width[index]/2.0),X);
          ModelSupport::buildPlane(SMap,SWI+4,Origin+X*(T+width[index]/2.0),X);
          ModelSupport::buildPlane(SMap,SWI+5,Origin-Z*(T+height[index]/2.0),Z);
          ModelSupport::buildPlane(SMap,SWI+6,Origin+Z*(T+height[index]/2.0),Z);
          SWI+=10;
          T+=wallThick[wallIndex];
        }
      L+=length[index];
      SI+=100;
    }
	    
  ModelSupport::buildPlane(SMap,SI+1,Origin+Y*L,Y);
  if (!frontActive())
    setFront(SMap.realSurf(insIndex+1));
  if (!backActive())
    setBack(-SMap.realSurf(SI+1));

  return;
}
  
void
CompBInsert::createObjects(Simulation& System)
/*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("CompBInsert","createObjects");
  
  std::string Out;
  int SI(insIndex);

  std::string frontBack;
  for(size_t index=0;index<NBox;index++)
    {
      HeadRule Inner;
      int SWI(SI);
      frontBack= (index) ?
	ModelSupport::getComposite(SMap,SI," 1 ") :
	frontRule();
      frontBack+= ((index+1)!=NBox) ?
	ModelSupport::getComposite(SMap,SI," -101 ") :
	backRule();

      for(size_t wallIndex=0;wallIndex<=NWall;wallIndex++)
        {
          Out=ModelSupport::getComposite(SMap,SWI," 3 -4 5 -6 ");
          const int M=(!wallIndex) ? mat[index] : wallMat[wallIndex-1];
          System.addCell(MonteCarlo::Qhull(cellIndex++,M,0.0,
					   frontBack+Out+Inner.display()));

          if (!wallIndex)
            addCell("Item",cellIndex-1);
          else
            addCell("Wall",cellIndex-1);
          
          Inner.procString(Out);
          Inner.makeComplement();
          SWI+=10;
        }
      addOuterUnionSurf(Out+frontBack);
      SI+=100;
    }
  return;
}
  
void
CompBInsert::createLinks()
  /*!
    Create all the linkes [OutGoing]
  */
{
  ELog::RegMethod RegA("CompBInsert","createLinks");

  setNConnect(NBox*6+1);

  FrontBackCut::createLinks(*this,Origin,Y);
  if (!frontActive())
    {
      FixedComp::setConnect(0,Origin,-Y);
      FixedComp::setLinkSurf(0,-SMap.realSurf(insIndex+1));
    }
  
  size_t LOffset(0);
  double L(0.0);
  const int WIOffset(static_cast<int>(NWall)*10);
  int SI(insIndex);

  const double T=std::accumulate(wallThick.begin(),wallThick.end(),0.0);

  for(size_t index=0;index<NBox;index++)
    {
      FixedComp::setConnect(LOffset+2,Origin-X*(T+width[index]/2.0),-X);
      FixedComp::setConnect(LOffset+3,Origin+X*(T+width[index]/2.0),X);
      FixedComp::setConnect(LOffset+4,Origin-Z*(T+height[index]/2.0),-Z);
      FixedComp::setConnect(LOffset+5,Origin+Z*(T+height[index]/2.0),Z);

      FixedComp::setLinkSurf(2,SMap.realSurf(SI+WIOffset+3));
      FixedComp::setLinkSurf(3,-SMap.realSurf(SI+WIOffset+4));
      FixedComp::setLinkSurf(4,SMap.realSurf(SI+WIOffset+5));
      FixedComp::setLinkSurf(5,-SMap.realSurf(SI+WIOffset+6));

      L+=length[index];
      LOffset+=6;
      SI+=100;
    }
  
  if (!backActive())
    {
      FixedComp::setConnect(1,Origin+Y*L,Y);
      FixedComp::setLinkSurf(1,SMap.realSurf(SI+1));
    }
  // Mid point [useful for guides etc]
  FixedComp::setConnect(6,Origin+Y*(L/2.0),Y);
  
  return;
}


void
CompBInsert::createAll(Simulation& System,
                       const attachSystem::FixedComp& FC,
                       const long int sideIndex,
                       const attachSystem::FixedComp&)
  
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link for origin
  */
{
  ELog::RegMethod RegA("CompBInsert","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();

  createObjects(System);
  createLinks();
  
  insertObjects(System);              

  return;
}

}  // NAMESPACE essSystem
