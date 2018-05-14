/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/BunkerRoof.cxx
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
#include "MXcards.h"
#include "Zaid.h"
#include "Material.h"
#include "DBMaterial.h"
#include "surfDIter.h"
#include "LayerDivide3D.h"
#include "BunkerRoof.h"


namespace essSystem
{

BunkerRoof::BunkerRoof(const std::string& bunkerName) :
  attachSystem::ContainedComp(),
  attachSystem::FixedComp(bunkerName+"Roof",6),
  attachSystem::CellMap(),attachSystem::SurfMap(),baseName(bunkerName),
  roofIndex(ModelSupport::objectRegister::Instance().cell(keyName,20000)),
  cellIndex(roofIndex+1),baseSurf(0),topSurf(0),innerSurf(0),outerSurf(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param bunkerName :: Name of the bunker object that is building this roof
  */
{}

BunkerRoof::BunkerRoof(const BunkerRoof& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  baseName(A.baseName),roofIndex(A.roofIndex),cellIndex(A.cellIndex),
  roofThick(A.roofThick),roofMat(A.roofMat),activeRoof(A.activeRoof),
  nVert(A.nVert),nRadial(A.nRadial),nMedial(A.nMedial),
  vert(A.vert),radial(A.radial),medial(A.medial),
  nBasicVert(A.nBasicVert),basicVert(A.basicVert),
  basicMatVec(A.basicMatVec),loadFile(A.loadFile),
  outFile(A.outFile),divider(A.divider),baseSurf(A.baseSurf),
  topSurf(A.topSurf),innerSurf(A.innerSurf),outerSurf(A.outerSurf)
  /*!
    Copy constructor
    \param A :: BunkerRoof to copy
  */
{}

BunkerRoof&
BunkerRoof::operator=(const BunkerRoof& A)
  /*!
    Assignment operator
    \param A :: BunkerRoof to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      cellIndex=A.cellIndex;
      roofThick=A.roofThick;
      roofMat=A.roofMat;
      activeRoof=A.activeRoof;
      nVert=A.nVert;
      nRadial=A.nRadial;
      nMedial=A.nMedial;
      vert=A.vert;
      radial=A.radial;
      medial=A.medial;
      nBasicVert=A.nBasicVert;
      basicVert=A.basicVert;
      basicMatVec=A.basicMatVec;
      loadFile=A.loadFile;
      outFile=A.outFile;
      divider=A.divider;
      baseSurf=A.baseSurf;
      topSurf=A.topSurf;
      innerSurf=A.innerSurf;
      outerSurf=A.outerSurf;
    }
  return *this;
}

BunkerRoof::~BunkerRoof() 
  /*!
    Destructor
  */
{}

void
BunkerRoof::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("BunkerRoof","populate");

  roofThick=Control.EvalVar<double>(keyName+"Thick");
  roofMat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  

  // Need two sets active and passive :
  // ACTIVE:
  // BOOLEAN NUMBER!!!!!!!
  activeRoof=Control.EvalDefVar<size_t>(keyName+"Active",0);
  if (activeRoof)
    {
      nVert=Control.EvalVar<size_t>(keyName+"NVert");
      nRadial=Control.EvalDefVar<size_t>(keyName+"NRadial",0);
      nMedial=Control.EvalDefVar<size_t>(keyName+"NMedial",0);
      ModelSupport::populateDivideLen(Control,nVert,keyName+"Vert",
				      roofThick,vert);
      
      ModelSupport::populateDivideLen(Control,nRadial,keyName+"Radial",
				      1.0,radial);
      
      ModelSupport::populateDivideLen(Control,nMedial,keyName+"Medial",
				      1.0,medial);

      loadFile=Control.EvalDefVar<std::string>(keyName+"LoadFile","");
      outFile=Control.EvalDefVar<std::string>(keyName+"OutFile","");
    }

  // PASSIVE
  nBasicVert=Control.EvalVar<size_t>(keyName+"NBasicVert");
  ModelSupport::populateDivideLen(Control,nBasicVert,keyName+"Vert",
				  roofThick,basicVert);
  ModelSupport::populateDivide(Control,nBasicVert,keyName+"Mat",
			       ModelSupport::EvalMatString(roofMat),
			       basicMatVec);	  

  
  return;
}
  
void
BunkerRoof::createUnitVector(const attachSystem::FixedComp& FC,
			     const long int sideIndex)
/*!
    Create the unit vectors
    \param FC :: Linked object (bunker )
    \param sideIndex :: Side for linkage centre (roof)
  */
{
  ELog::RegMethod RegA("BunkerRoof","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  return;
}

void
BunkerRoof::setVertSurf(const int IS,const int OS)
  /*!
    Set the inner/outer vertical surfaces
    \param IS :: inner surface
    \param OS :: outer surface
  */
{
  ELog::RegMethod RegA("BunkerRoof","setVertSurf");
  
  baseSurf=IS;
  topSurf=OS;
  return;
}

void
BunkerRoof::setRadialSurf(const int IS,const int OS)
  /*!
    Set the radial cylindrical vertical surfaces
    \param IS :: inner surface
    \param OS :: outer surface
  */
{
  ELog::RegMethod RegA("BunkerRoof","setVertSurf");
  
  innerSurf=IS;
  outerSurf=OS;
  return;
}

  
void
BunkerRoof::createSector(Simulation& System,
			 const size_t sectNum,const int cellN,
			 const int lwIndex,const int rwIndex)

  /*!
    Create a roof segment. Either highly divided or simple
    divided depending on the state of the activeRoof flag.
    \param System :: Simulation system
    \param sectNum :: sector number
    \param cellN :: Sector cell
    \param lwIndex :: left sector boundary
    \param rwIndex :: right sector boundary
  */
{
  ELog::RegMethod RegA("BunkerRoof","createSector");

  std::vector<double> empty;
  ModelSupport::LayerDivide3D LD3(keyName+"Main"+
				  StrFunc::makeString(sectNum));

  // Front/back??
  LD3.setSurfPair(0,innerSurf,outerSurf);
  LD3.setSurfPair(1,lwIndex,rwIndex);
  LD3.setSurfPair(2,baseSurf,topSurf);
  LD3.setDividerByExclude(System,cellN);

  const bool AFlag (activeRoof & (1 << sectNum));

  if (AFlag)
    {
      LD3.setFractions(0,radial);
      LD3.setFractions(1,medial);	    
      LD3.setFractions(2,vert);
      LD3.setIndexNames("Radial","Medial","Vert");
      LD3.setMaterialXML(keyName+"Def.xml","RoofMat",keyName+".xml",
			 ModelSupport::EvalMatString(roofMat));	  
    }
  else
    {
      LD3.setFractions(0,empty);
      LD3.setFractions(1,empty);
      LD3.setFractions(2,basicVert);
      LD3.setMaterials(2,basicMatVec);
    }
  
  LD3.divideCell(System,cellN);
  addCells("Sector"+StrFunc::makeString(sectNum),LD3.getCells());
  return;
}

 
void
BunkerRoof::initialize(const FuncDataBase& Control,
		       const attachSystem::FixedComp& FC,
		       const long int linkIndex)

  /*!
    Generic function to initialize everything
    \param Control :: DataBase for variables
    \param FC :: Central origin
    \param linkIndex :: linkIndex number
  */
{
  ELog::RegMethod RegA("BunkerRoof","initialize");

  populate(Control);  
  createUnitVector(FC,linkIndex);

  return;
}

}  // NAMESPACE essSystem
