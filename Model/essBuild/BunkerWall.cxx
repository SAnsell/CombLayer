/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/BunkerWall.cxx
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
#include "BunkerWall.h"


namespace essSystem
{

BunkerWall::BunkerWall(const std::string& bunkerName) :
  attachSystem::ContainedComp(),
  attachSystem::FixedComp(bunkerName+"Wall",6),
  attachSystem::CellMap(),attachSystem::SurfMap(),baseName(bunkerName),
  wallIndex(ModelSupport::objectRegister::Instance().cell(keyName,20000)),
  cellIndex(wallIndex+1),activeWall(0),frontSurf(0),backSurf(0),
  topSurf(0),baseSurf(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param bunkerName :: Name of the bunker object that is building this wall
  */
{}

BunkerWall::BunkerWall(const BunkerWall& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  attachSystem::CellMap(A),attachSystem::SurfMap(A),
  baseName(A.baseName),wallIndex(A.wallIndex),
  cellIndex(A.cellIndex),wallThick(A.wallThick),
  wallMat(A.wallMat),activeWall(A.activeWall),nVert(A.nVert),
  nRadial(A.nRadial),nMedial(A.nMedial),vert(A.vert),
  radial(A.radial),medial(A.medial),nBasic(A.nBasic),
  basic(A.basic),basicMatVec(A.basicMatVec),
  loadFile(A.loadFile),outFile(A.outFile),divider(A.divider),
  frontSurf(A.frontSurf),backSurf(A.backSurf),
  topSurf(A.topSurf),baseSurf(A.baseSurf)
  /*!
    Copy constructor
    \param A :: BunkerWall to copy
  */
{}

BunkerWall&
BunkerWall::operator=(const BunkerWall& A)
  /*!
    Assignment operator
    \param A :: BunkerWall to copy
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
      wallThick=A.wallThick;
      wallMat=A.wallMat;
      activeWall=A.activeWall;
      nVert=A.nVert;
      nRadial=A.nRadial;
      nMedial=A.nMedial;
      vert=A.vert;
      radial=A.radial;
      medial=A.medial;
      nBasic=A.nBasic;
      basic=A.basic;
      basicMatVec=A.basicMatVec;
      loadFile=A.loadFile;
      outFile=A.outFile;
      divider=A.divider;
      frontSurf=A.frontSurf;
      backSurf=A.backSurf;
      topSurf=A.topSurf;
      baseSurf=A.baseSurf;
    }
  return *this;
}

BunkerWall::~BunkerWall() 
  /*!
    Destructor
  */
{}

void
BunkerWall::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("BunkerWall","populate");

  
  wallThick=Control.EvalVar<double>(keyName+"Thick");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  // PASSIVE
  nBasic=Control.EvalVar<size_t>(keyName+"NBasic");

  ModelSupport::populateDivideLen(Control,nBasic,keyName+"Len",
				  wallThick,basic);
  ModelSupport::populateDivide(Control,nBasic,keyName+"Mat",
			       ModelSupport::EvalMatString(wallMat),
			       basicMatVec);


  // Need two sets active and passive :
  // ACTIVE:
  // BOOLEAN NUMBER!!!!!!!
  activeWall=Control.EvalDefVar<size_t>(keyName+"Active",0);
  
  if (activeWall)
    {
      nVert=Control.EvalVar<size_t>(keyName+"NVert");
      nRadial=Control.EvalDefVar<size_t>(keyName+"NRadial",0);
      nMedial=Control.EvalDefVar<size_t>(keyName+"NMedial",0);
      ModelSupport::populateDivideLen(Control,nVert,keyName+"Vert",
				      1.0,vert);
      if (!nRadial)
	radial=basic;
      else
	ModelSupport::populateDivideLen(Control,nRadial,keyName+"Radial",
					1.0,radial);
      
      ModelSupport::populateDivideLen(Control,nMedial,keyName+"Medial",
				      1.0,medial);
      
      loadFile=Control.EvalDefVar<std::string>(keyName+"LoadFile","");
      outFile=Control.EvalDefVar<std::string>(keyName+"OutFile","");
    }
  
  return;
}
  
void
BunkerWall::createUnitVector(const attachSystem::FixedComp& FC,
			     const long int sideIndex)
/*!
    Create the unit vectors
    \param FC :: Linked object (bunker )
    \param sideIndex :: Side for linkage centre (wall)
  */
{
  ELog::RegMethod RegA("BunkerWall","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  return;
}

void
BunkerWall::setVertSurf(const int BS,const int TS)
  /*!
    Set the top/base vertical surfaces
    \param BS :: base surface
    \param TS :: top surface
  */
{
  ELog::RegMethod RegA("BunkerWall","setVertSurf");
  
  baseSurf=BS;
  topSurf=TS;
  return;
}

void
BunkerWall::setRadialSurf(const int IS,const int OS)
  /*!
    Set the radial cylindrical front/back surfaces
    \param IS :: inner surface
    \param OS :: outer surface
  */
{
  ELog::RegMethod RegA("BunkerWall","setVertSurf");
  
  frontSurf=IS;
  backSurf=OS;
  return;
}

  
void
BunkerWall::createSector(Simulation& System,
			 const size_t sectNum,const int cellN,
			 const int lwIndex,const int rwIndex)

  /*!
    Create a wall segment. Either highly divided or simple
    divided depending on the state of the activeWall flag.
    \param System :: Simulation system
    \param sectNum :: sector number
    \param cellN :: Sector cell
    \param lwIndex :: left sector boundary
    \param rwIndex :: right sector boundary
  */
{
  ELog::RegMethod RegA("BunkerWall","createSector");

  const FuncDataBase& Control=System.getDataBase();
  
  std::vector<double> empty;
  
  ModelSupport::LayerDivide3D LD3(keyName+"MainWall"+
				  StrFunc::makeString(sectNum));

  // Front/back??
  LD3.setSurfPair(0,frontSurf,backSurf);
  LD3.setSurfPair(1,lwIndex,rwIndex);
  LD3.setSurfPair(2,baseSurf,topSurf);
  LD3.setDivider(divider);

  const bool AFlag (activeWall & (1 << sectNum));

  std::vector<std::string> actualMatVec;
  ModelSupport::populateVecDivide
    (Control,keyName+std::to_string(sectNum)+"Mat",
     basicMatVec,actualMatVec);

  if (AFlag)
    {
      LD3.setFractions(0,radial);
      LD3.setFractions(1,medial);	    
      LD3.setFractions(2,vert);
      LD3.setIndexNames("Radial","Medial","Vert");
      if (!LD3.setMaterialXML(keyName+"Def.xml","WallMat",keyName+".xml",
			      ModelSupport::EvalMatString(wallMat)))
	{
	  ELog::EM<<"Using Basic Material Layers: [size="
		  <<actualMatVec.size()<<"]"<<ELog::endDiag;	    
	  LD3.setMaterials(0,actualMatVec);
	}
    }
  else
    {
      LD3.setFractions(0,basic);
      LD3.setFractions(1,empty);
      LD3.setFractions(2,empty);
      LD3.setMaterials(0,actualMatVec);
    }
  LD3.divideCell(System,cellN);
  addCells("Sector"+StrFunc::makeString(sectNum),LD3.getCells());
  return;
}

 
void
BunkerWall::initialize(const FuncDataBase& Control,
		       const attachSystem::FixedComp& FC,
		       const long int linkIndex)

  /*!
    Generic function to initialize everything
    \param Control :: DataBase for variables
    \param FC :: Central origin
    \param linkIndex :: linkIndex number
  */
{
  ELog::RegMethod RegA("BunkerWall","initialize");

  populate(Control);  
  createUnitVector(FC,linkIndex);

  return;
}

}  // NAMESPACE essSystem
