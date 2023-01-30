/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/ShutterBay.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
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
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ShutterBay.h"

namespace essSystem
{

ShutterBay::ShutterBay(const std::string& Key)  :
  attachSystem::FixedRotate(Key,8),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  attachSystem::ExternalCut()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

ShutterBay::ShutterBay(const ShutterBay& A) : 
  attachSystem::FixedRotate(A),
  attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),
  attachSystem::ExternalCut(A),
  radius(A.radius),height(A.height),depth(A.depth),
  skin(A.skin),topSkin(A.topSkin),
  mat(A.mat),skinMat(A.skinMat)
  /*!
    Copy constructor
    \param A :: ShutterBay to copy
  */
{}

ShutterBay&
ShutterBay::operator=(const ShutterBay& A)
  /*!
    Assignment operator
    \param A :: ShutterBay to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A),
      radius=A.radius;
      height=A.height;
      depth=A.depth;
      skin=A.skin;
      topSkin=A.topSkin;
      mat=A.mat;
      skinMat=A.skinMat;
    }
  return *this;
}

ShutterBay::~ShutterBay() 
  /*!
    Destructor
  */
{}

void
ShutterBay::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: Variable data base
 */
{
  ELog::RegMethod RegA("ShutterBay","populate");
  
  FixedRotate::populate(Control);
  radius=Control.EvalVar<double>(keyName+"Radius");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  skin=Control.EvalVar<double>(keyName+"Skin");
  topSkin=Control.EvalVar<double>(keyName+"TopSkin");
  cutSkin=Control.EvalVar<double>(keyName+"CutSkin");
  topCut=Control.EvalVar<double>(keyName+"TopCut");
  topRadius=Control.EvalVar<double>(keyName+"TopRadius");

  NCurtain=Control.EvalVar<size_t>(keyName+"NCurtain");
  NCurtain=std::max<size_t>(1,NCurtain);
  double thick(0.0);
  curLayer.push_back(topCut);
  for(size_t i=0;i<NCurtain;i++)
    {
      const std::string NStr=std::to_string(i);
      if (i+1!=NCurtain)
        {
          const double T=Control.EvalVar<double>(keyName+"CurtainThick"+NStr);
          thick+=T;
          if (thick>=topCut)
            throw ColErr::SizeError<double>
              (thick,topCut,"TopCut < thickness at layer:"+NStr);
          curLayer.push_back(topCut-thick);
        }
      const int cMat=
        ModelSupport::EvalMat<int>(Control,keyName+"CurtainMat"+NStr);
      curMat.push_back(cMat);
    }
  
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  skinMat=ModelSupport::EvalMat<int>(Control,keyName+"SkinMat");

  return;
}
    
void
ShutterBay::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("ShutterBay","createSurface");

  // rotation of axis:

  // Dividing planes:
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin,X);

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*depth,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*height,Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(topSkin+height),Z);
  ModelSupport::buildPlane(SMap,buildIndex+106,Origin+Z*(height-topCut),Z);


  int CL(buildIndex+200);
  for(size_t i=0;i<NCurtain;i++)
    {
      ModelSupport::buildPlane
        (SMap,CL+6,Origin+Z*(height+cutSkin-curLayer[i]),Z);
      CL+=10;
    }
  
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Z,radius);
  ModelSupport::buildCylinder(SMap,buildIndex+17,Origin,Z,radius+skin);
  ModelSupport::buildCylinder(SMap,buildIndex+107,Origin,Z,topRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+117,Origin,Z,topRadius+skin);
  
  return;
}

void
ShutterBay::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("ShutterBay","createObjects");

  HeadRule HR;
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"5 -106 -7");
  HR*=ExternalCut::getRule("Bulk");
  makeCell("MainCell",System,cellIndex++,mat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"106 -107 -6");
  makeCell("TopCell",System,cellIndex++,mat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"5 -106 7 -17");
  makeCell("Skin",System,cellIndex++,skinMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"106 -206 107 -17");
  makeCell("Skin",System,cellIndex++,skinMat,0.0,HR);
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"206 -6 107 -117");
  makeCell("Skin",System,cellIndex++,skinMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"6 -16 -117");
  makeCell("Skin",System,cellIndex++,skinMat,0.0,HR);

  int CL(buildIndex+200);
  for(size_t i=1;i<NCurtain;i++)
    {
      HR=ModelSupport::getHeadRule
        (SMap,buildIndex,CL,"117 -17 6M -16M");
      makeCell("Curtain",System,cellIndex++,curMat[i-1],0.0,HR);
      CL+=10;
    }
  HR=ModelSupport::getHeadRule(SMap,buildIndex,CL,"117 -17 6M -16");
  makeCell("Curtain",System,cellIndex++,curMat.back(),0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"5 -16 -17");
  addOuterSurf(HR);

  return;
}

void
ShutterBay::createLinks()
  /*!
    Create all the linkes [OutGoing]
  */
{
  ELog::RegMethod RegA("ShutterBay","createLinks");

  FixedComp::setConnect(0,Origin-Y*(radius+skin),-Y);   // outer point
  FixedComp::setConnect(1,Origin+Y*(radius+skin),Y);   // outer point
  FixedComp::setConnect(2,Origin-X*(radius+skin),-X);   // outer point
  FixedComp::setConnect(3,Origin+X*(radius+skin),X);   // outer point
  FixedComp::setConnect(4,Origin-Z*depth,-Z);  // base
  FixedComp::setConnect(5,Origin+Z*(height+topSkin),Z);  // 

  FixedComp::setLinkSurf(0,SMap.realSurf(buildIndex+17));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+17));
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+17));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+17));

  FixedComp::addBridgeSurf(0,-SMap.realSurf(buildIndex+1));
  FixedComp::addBridgeSurf(1,SMap.realSurf(buildIndex+1));
  FixedComp::addBridgeSurf(2,-SMap.realSurf(buildIndex+2));
  FixedComp::addBridgeSurf(3,SMap.realSurf(buildIndex+2));
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+16));

  FixedComp::setLinkSurf(6,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(7,SMap.realSurf(buildIndex+7));

  FixedComp::setConnect(6,Origin-Y*radius,-Y);   // materila point
  FixedComp::setConnect(7,Origin+Y*radius,Y);    // material point


  return;
}

void
ShutterBay::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: Link pt
  */
{
  ELog::RegMethod RegA("ShutterBay","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createLinks();
  createObjects(System);
  insertObjects(System);              

  return;
}

}  // NAMESPACE essSystem
