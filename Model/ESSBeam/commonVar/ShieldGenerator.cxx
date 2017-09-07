/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonVar/ShieldGenerator.cxx
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
#include <stack>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <numeric>
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
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
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
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "surfExpand.h"
#include "ShieldGenerator.h"

namespace setVariable
{

ShieldGenerator::ShieldGenerator() :
  leftAngle(0.0),rightAngle(0.0),endThick(0.0),
  endRadius(0.0),nWall(0),nRoof(0),nFloor(0),
  defMat("Stainless304")
  /*!
    Constructor and defaults
  */
{}

ShieldGenerator::ShieldGenerator(const ShieldGenerator& A) :
  leftAngle(A.leftAngle),rightAngle(A.rightAngle),
  endThick(A.endThick),endRadius(A.endRadius),
  nWall(A.nWall),nRoof(A.nRoof),nFloor(A.nFloor),
  defMat(A.defMat),wallLen(A.wallLen),roofLen(A.roofLen),
  floorLen(A.floorLen),wallMat(A.wallMat),roofMat(A.roofMat),
  floorMat(A.floorMat)
  /*!
    Copy constructor
    \param A :: ShieldGenerator to copy
  */
{}

ShieldGenerator&
ShieldGenerator::operator=(const ShieldGenerator& A)
  /*!
    Assignment operator
    \param A :: ShieldGenerator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      nWall=A.nWall;
      nRoof=A.nRoof;
      nFloor=A.nFloor;
      defMat=A.defMat;
      wallLen=A.wallLen;
      roofLen=A.roofLen;
      floorLen=A.floorLen;
      wallMat=A.wallMat;
      roofMat=A.roofMat;
      floorMat=A.floorMat;
    }
  return *this;
}

  
ShieldGenerator::~ShieldGenerator() 
 /*!
   Destructor
 */
{}

void
ShieldGenerator::setAngle(const double LA,const double RA)
  /*!
    Set the wall angles
    \param LA :: Left angle
    \param RA :: Right angle
   */
{
  ELog::RegMethod RegA("ShieldGenerator","setAngle");
  leftAngle=LA;
  rightAngle=RA;
  return;
}

void
ShieldGenerator::setEndWall(const double T,const double R)
  /*!
    Set the end wall 
    \param T :: Thickness of end wall
    \param R :: Radius of end wall void
    \todo add additional features for mat/layers etc
  */
{
  endThick=T;
  endRadius=R;
  return;
}

  
void
ShieldGenerator::setLayers(MLTYPE& lenMap,MSTYPE& matMap,
                           double& primThick,size_t& nLayer,
                           const size_t NL,
                           const double voidThick,
                           const std::vector<double>& thick,
                           const std::vector<std::string>& matName)
 /*!
   Given a set of values set the wall
   \param NL :: Number of layers 
   \param voidThick :: Thickness of inner void [half]
   \param thick :: Layer thickness
   \param matName :: layer materials
 */
{
  ELog::RegMethod RegA("ShieldGenerator","setLayers");

  if (thick.size()!=matName.size())
    throw ColErr::MisMatch<size_t>(thick.size(),matName.size(),"thick/matName");
  
  primThick=std::accumulate(thick.begin(),thick.end(),0.0);
  
  lenMap.erase(lenMap.begin(),lenMap.end());
  matMap.erase(matMap.begin(),matMap.end());

  lenMap.emplace(1,voidThick);
  matMap.emplace(1,"Void");

  // set each layer to be approximately equal.
  const double meanDist(primThick/static_cast<double>(NL));

  size_t index(0);
  double TNext(thick[index]);
  double primDist(meanDist);
  for(size_t i=0;i <NL;i++)
    {
      if (TNext>=primDist)
	{
	  lenMap.emplace(i+2,primDist+voidThick);
	  matMap.emplace(i+2,matName[index]);
	}
      else
	{
	  lenMap.emplace(i+2,TNext);
          matMap.emplace(i+2,matName[index]);
	  if (index<thick.size()) index++;
	  TNext+=thick[index];
	}
      primDist+=meanDist;
    }
  primThick+=voidThick;
  nLayer=NL+1;
  return;
}

void
ShieldGenerator::clearLayers()
  /*!
    Clear layers
  */
{
  ELog::RegMethod RegA("ShieldGenerator","clearLayers");

  wallLen.clear();
  floorLen.clear();
  roofLen.clear();
  wallMat.clear();
  floorMat.clear();
  roofMat.clear();
  return;
}
  
void
ShieldGenerator::addWall(const size_t index,const double Len,
                         const std::string& matName)
  /*!
    Add a layer to the wall
    \param index :: index values
    \param Len :: length
    \param matName :: Material name
  */
{
  ELog::RegMethod RegA("ShieldGenerator","addWall");
  wallLen.emplace(index,Len);
  wallMat.emplace(index,matName);
  return;
}

void
ShieldGenerator::addRoof(const size_t index,const double Len,
                         const std::string& matName)
  /*!
    Add a layer to the roof
    \param index :: index values
    \param Len :: length
    \param matName :: Material name
  */
{
  ELog::RegMethod RegA("ShieldGenerator","addRoof");
  roofLen.emplace(index,Len);
  roofMat.emplace(index,matName);
  return;
}

void
ShieldGenerator::addFloor(const size_t index,const double Len,
			  const std::string& matName)
  /*!
    Add a layer to the floor
    \param index :: index values
    \param Len :: length
    \param matName :: Material name
  */
{
  ELog::RegMethod RegA("ShieldGenerator","addFloor");
  floorLen.emplace(index,Len);
  floorMat.emplace(index,matName);
  return;
}

void
ShieldGenerator::addWallLen(const size_t index,const double Len)
  /*!
    Add a layer to the wall
    \param index :: index values
    \param Len :: length
  */
{
  ELog::RegMethod RegA("ShieldGenerator","addWallLen");
  wallLen.emplace(index,Len);
  return;
}

void
ShieldGenerator::addRoofLen(const size_t index,const double Len)
  /*!
    Add a layer to the roof
    \param index :: index values
    \param Len :: length
  */
{
  ELog::RegMethod RegA("ShieldGenerator","addRoofLen");
  roofLen.emplace(index,Len);
  return;
}

void
ShieldGenerator::addFloorLen(const size_t index,const double Len)
  /*!
    Add a layer to the floor
    \param index :: index values
    \param Len :: length
  */
{
  ELog::RegMethod RegA("ShieldGenerator","addFloorLen");
  floorLen.emplace(index,Len);
  return;
}

  
void
ShieldGenerator::addWallMat(const size_t index,
                             const std::string& matName)
  /*!
    Add a mat layer to the wall
    \param index :: index values
    \param matName :: Material name
  */
{
  ELog::RegMethod RegA("ShieldGenerator","addWallMat");
  wallMat.emplace(index,matName);
  return;
}
  
void
ShieldGenerator::addRoofMat(const size_t index,
                             const std::string& matName)
  /*!
    Add a mat layer to the roof
    \param index :: index values
    \param matName :: Material name
  */
{
  ELog::RegMethod RegA("ShieldGenerator","addRoofMat");
  roofMat.emplace(index,matName);
  return;
}

void
ShieldGenerator::addFloorMat(const size_t index,
                             const std::string& matName)
  /*!
    Add a mat layer to the floor
    \param index :: index values
    \param matName :: Material name
  */
{
  ELog::RegMethod RegA("ShieldGenerator","addFloorMat");
  floorMat.emplace(index,matName);
  return;
}

  

void
ShieldGenerator::processLayers
(FuncDataBase& Control,const std::string& keyName) const
  /*!
    Set the individual layers
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
  */
{
  ELog::RegMethod RegA("ShieldGenerator","processLayers");
  
  for(const MLTYPE::value_type& LItem : wallLen)
    {
      Control.addVariable
        (keyName+"WallLen"+StrFunc::makeString(LItem.first),LItem.second);
    }
  for(const MLTYPE::value_type& LItem : floorLen)
    {
      Control.addVariable
        (keyName+"FloorLen"+StrFunc::makeString(LItem.first),LItem.second);
    }
  for(const MLTYPE::value_type& LItem : roofLen)
    {
      Control.addVariable
        (keyName+"RoofLen"+StrFunc::makeString(LItem.first),LItem.second);
    }
  

  for(const MSTYPE::value_type& LItem : wallMat)
    {
      Control.addVariable
        (keyName+"WallMat"+StrFunc::makeString(LItem.first),LItem.second);
    }
  for(const MSTYPE::value_type& LItem : floorMat)
    {
      Control.addVariable
        (keyName+"FloorMat"+StrFunc::makeString(LItem.first),LItem.second);
    }
  for(const MSTYPE::value_type& LItem : roofMat)
    {
      Control.addVariable
        (keyName+"RoofMat"+StrFunc::makeString(LItem.first),LItem.second);
    }
  return;
}
void
ShieldGenerator::setRFLayers(const size_t nF,const size_t nR)
  /*!
    Set the number of roof/floor layers
    \param nF :: floor Number [0 use NLayers value]
    \param nR :: roof Number [0 use NLayers value]
   */
{
  nRoof=nR;
  nFloor=nF;
  return;
}
  
void
ShieldGenerator::generateShield
( FuncDataBase& Control,const std::string& keyName,
  const double length,const double side,const double height,
  const double depth,const size_t NSeg,const size_t NLayer)  const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param length :: overall length
    \param side :: full extent at sides
    \param height :: Full height
    \param depth :: Full depth
    \param NSeg :: number of segments
    \param NLayer :: number of layers
  */
{
  ELog::RegMethod RegA("ShieldGenerator","generatorShield");

  const size_t NWall(nWall ? nWall : NLayer);
  const size_t NRoof(nRoof ? nRoof : NLayer);
  const size_t NFloor(nFloor ? nFloor : NLayer);
  
  Control.addVariable(keyName+"Length",length);
  Control.addVariable(keyName+"Left",side);
  Control.addVariable(keyName+"Right",side);
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"Depth",depth);
  Control.addVariable(keyName+"DefMat",defMat);
  Control.addVariable(keyName+"NSeg",NSeg);
  Control.addVariable(keyName+"NWallLayers",NWall);
  Control.addVariable(keyName+"NFloorLayers",NFloor);
  Control.addVariable(keyName+"NRoofLayers",NRoof);
  
  processLayers(Control,keyName);
  
  return;

}

void
ShieldGenerator::generateTriShield
( FuncDataBase& Control,const std::string& keyName,
  const double length,const double side,const double height,
  const double depth,const size_t NSeg,const size_t NLayer)  const
  /*!
    Primary funciton for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param length :: overall length
    \param side :: full extent at sides
    \param height :: Full height
    \param depth :: Full depth
    \param NSeg :: number of segments
    \param NLayer :: number of layers
  */
{
  ELog::RegMethod RegA("ShieldGenerator","generatorShield");

  const size_t NWall(nWall ? nWall : NLayer);
  const size_t NRoof(nRoof ? nRoof : NLayer);
  const size_t NFloor(nFloor ? nFloor : NLayer);
  
  Control.addVariable(keyName+"Length",length);  
  Control.addVariable(keyName+"LeftAngle",leftAngle);
  Control.addVariable(keyName+"RightAngle",rightAngle);
  Control.addVariable(keyName+"EndWall",endThick);
  Control.addVariable(keyName+"EndVoid",endRadius);
  Control.addVariable(keyName+"Left",side);
  Control.addVariable(keyName+"Right",side);
  Control.addVariable(keyName+"Height",height);
  Control.addVariable(keyName+"Depth",depth);
  
  Control.addVariable(keyName+"DefMat",defMat);
  Control.addVariable(keyName+"NSeg",NSeg);
  Control.addVariable(keyName+"NWallLayers",NWall);
  Control.addVariable(keyName+"NFloorLayers",NFloor);
  Control.addVariable(keyName+"NRoofLayers",NRoof);
  
  processLayers(Control,keyName);
  
  return;

}

}  // NAMESPACE setVariable
