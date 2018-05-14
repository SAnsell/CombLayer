/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/WedgeInsert.cxx
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
#include "surfRegister.h"
#include "objectRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "RuleSupport.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "support.h"
#include "stringCombine.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "LayerComp.h"

#include "WedgeInsert.h"

namespace constructSystem
{

WedgeInsert::WedgeInsert(const std::string& Key,const size_t Index) :
  attachSystem::FixedOffset(Key+StrFunc::makeString(Index),6),
  attachSystem::ContainedComp(),baseName(Key),
  wedgeIndex(ModelSupport::objectRegister::Instance().cell(keyName)),
  cellIndex(wedgeIndex+1)
  /*!
    Constructor
    \param Key :: Name of construction key
    \param Index :: Index number for component
  */
{}

WedgeInsert::WedgeInsert(const WedgeInsert& A) : 
  attachSystem::FixedOffset(A),attachSystem::ContainedComp(A),
  baseName(A.baseName),wedgeIndex(A.wedgeIndex),
  cellIndex(A.cellIndex),
  viewWidth(A.viewWidth),viewHeight(A.viewHeight),
  viewXY(A.viewXY),viewZ(A.viewZ),wall(A.wall),
  mat(A.mat),wallMat(A.wallMat),temp(A.temp)
  /*!
    Copy constructor
    \param A :: WedgeInsert to copy
  */
{}

WedgeInsert&
WedgeInsert::operator=(const WedgeInsert& A)
  /*!
    Assignment operator
    \param A :: WedgeInsert to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedOffset::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      cellIndex=A.cellIndex;
      viewWidth=A.viewWidth;
      viewHeight=A.viewHeight;
      viewXY=A.viewXY;
      viewZ=A.viewZ;
      wall=A.wall;
      mat=A.mat;
      wallMat=A.wallMat;
      temp=A.temp;
    }
  return *this;
}

WedgeInsert::~WedgeInsert()
  /*!
    Destructor
  */
{}

WedgeInsert*
WedgeInsert::clone() const
  /*!
    Clone copy constructor
    \return copy of this
  */
{
  return new WedgeInsert(*this);
}

void
WedgeInsert::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("WedgeInsert","populate");

  attachSystem::FixedOffset::populate(Control);
    // Master values
  if (Control.hasVariable(keyName+"Cent"))
    {
      const Geometry::Vec3D Cent=
	Control.EvalVar<Geometry::Vec3D>(keyName+"Cent");
      xStep=Cent.X();
      yStep=Cent.Y();
      zStep=Cent.Z();
    }

  viewWidth=Control.EvalPair<double>(keyName,baseName,"ViewWidth");   
  viewHeight=Control.EvalPair<double>(keyName,baseName,"ViewHeight");   
  viewXY=Control.EvalPair<double>(keyName,baseName,"ViewXY");   
  viewZ=Control.EvalPair<double>(keyName,baseName,"ViewZ");   
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat",baseName+"Mat");   

  wall=Control.EvalPair<double>(keyName,baseName,"Wall");   
  wallMat=(wall>Geometry::zeroTol) ? 
    ModelSupport::EvalMat<int>(Control,keyName+"WallMat"
			       ,baseName+"WallMat") : 0;   
  temp=Control.EvalPair<double>(keyName,baseName,"Temp");
  
  return;
}

void
WedgeInsert::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
  */
{
  ELog::RegMethod RegA("WedgeInsert","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC);

  applyOffset();
  return;
}

void
WedgeInsert::createSurfaces()
  /*!
    Create planes for the silicon and Polyethene layers
  */
{
  ELog::RegMethod RegA("WedgeInsert","createSurfaces");

  // Quaternions for side directions
  const Geometry::Quaternion Qxy=
    Geometry::Quaternion::calcQRotDeg(viewXY,Z);
  const Geometry::Quaternion Qz=
    Geometry::Quaternion::calcQRotDeg(viewZ,X);

  Geometry::Vec3D vXMinus(X);
  Geometry::Vec3D vXPlus(X);
  Geometry::Vec3D vZMinus(Z);
  Geometry::Vec3D vZPlus(Z);

  Qxy.rotate(vXMinus);
  Qxy.invRotate(vXPlus);
  Qz.rotate(vZMinus);
  Qz.invRotate(vZPlus);
  
  // Back plane
  ModelSupport::buildPlane(SMap,wedgeIndex+1,Origin,Y);  
  // sides:
  ModelSupport::buildPlane(SMap,wedgeIndex+3,
			   Origin-X*(viewWidth/2.0),vXMinus);  
  ModelSupport::buildPlane(SMap,wedgeIndex+4,
			   Origin+X*(viewWidth/2.0),vXPlus);  
  ModelSupport::buildPlane(SMap,wedgeIndex+5,
			   Origin-Z*(viewHeight/2.0),vZMinus);  
  ModelSupport::buildPlane(SMap,wedgeIndex+6,
			   Origin+Z*(viewHeight/2.0),vZPlus);  

  if (wall>Geometry::zeroTol)
    {
      // Back plane
      ModelSupport::buildPlane(SMap,wedgeIndex+11,Origin-Y*wall,Y);  
      // sides:
      ModelSupport::buildPlane(SMap,wedgeIndex+13,
			       Origin-X*(wall+viewWidth/2.0),vXMinus);  
      ModelSupport::buildPlane(SMap,wedgeIndex+14,
			       Origin+X*(wall+viewWidth/2.0),vXPlus);  
      ModelSupport::buildPlane(SMap,wedgeIndex+15,
			       Origin-Z*(wall+viewHeight/2.0),vZMinus);  
      ModelSupport::buildPlane(SMap,wedgeIndex+16,
			       Origin+Z*(wall+viewHeight/2.0),vZPlus);  
    }

  // Avoid recalc rotations:
  FixedComp::setConnect(2,Origin-X*(wall+viewWidth/2.0),-vXMinus);
  FixedComp::setConnect(3,Origin+X*(wall+viewWidth/2.0),vXPlus);
  FixedComp::setConnect(4,Origin-Z*(wall+viewHeight/2.0),-vZMinus);
  FixedComp::setConnect(5,Origin+Z*(wall+viewHeight/2.0),vZPlus);

  return; 
}

void
WedgeInsert::createObjects(Simulation& System,
			   const attachSystem::FixedComp& FC,
			   const size_t layerIndex,
			   const size_t sideIndex)
  /*!
    Create the wedge using the surfaces from layer 1/2 of 
    LC
    \param System :: Simulation to add results
    \param  FC :: LayerComp (passed as a FC)
    \param layerIndex :: layer leve to connect Wedge too
    \param sideIndex :: Intersecting side
  */
{
  ELog::RegMethod RegA("WedgeInsert","createObjects");

  const ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  const attachSystem::LayerComp* LCPtr=
    OR.getObjectThrow<attachSystem::LayerComp>(FC.getKeyName(),"LayerComp");

  std::string Out;
  const std::string CShape=
    MonteCarlo::getComplementShape
    (LCPtr->getLayerString(layerIndex,static_cast<long int>(sideIndex+1)));

  if (wall>Geometry::zeroTol)
    {

      Out=ModelSupport::getComposite(SMap,wedgeIndex," 1 3 -4 5 -6 ");
      Out+=CShape;
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));

      Out=ModelSupport::getComposite(SMap,wedgeIndex,
				     " 11 13 -14 15 -16 ( -1:-3:4:-5:6 )" );
      Out+=CShape;
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));

      Out=ModelSupport::getComposite(SMap,wedgeIndex," 11 13 -14 15 -16 ");
      
      addOuterSurf(Out);      
    }

  return; 
}

void
WedgeInsert::createLinks()
  /*!
    Creates a full attachment set
  */
{  
  ELog::RegMethod RegA("WedgeInsert","createLinks");


  const int WI((wall>Geometry::zeroTol) ? wedgeIndex+10 : wedgeIndex);
  FixedComp::setConnect(0,Origin-Y*wall,-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(WI+1));

  // This needs to be the fron surface of the cylmod
  FixedComp::setConnect(1,Origin+Y*wall,-Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(WI+1));


  FixedComp::setLinkSurf(2,-SMap.realSurf(WI+3));
  FixedComp::setLinkSurf(3,SMap.realSurf(WI+4));
  FixedComp::setLinkSurf(4,-SMap.realSurf(WI+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(WI+6));

  return;
}



void
WedgeInsert::createAll(Simulation& System,
		       const int mainCell,
		       const attachSystem::FixedComp& FC,
		       const size_t layerIndex,
		       const size_t layerSide)
/*!
    Extrenal build everything
    \param System :: Simulation
    \param mainCell :: Main cell
    \param FC :: FixedComponent for origin
    \param layerIndex :: depth of layer
    \param layerSide :: Layer side to use
   */
{
  ELog::RegMethod RegA("WedgeInsert","createAll");
  populate(System.getDataBase());

  createUnitVector(FC);
  createSurfaces();
  createObjects(System,FC,layerIndex,layerSide);  // layerIndex was 1
  createLinks();
  // Messy code to add insCert cells
  addInsertCell(mainCell);
  if (wall>Geometry::zeroTol)
    addInsertCell(mainCell+1);

  insertObjects(System);       

  return;
}

}  // NAMESPACE constructSystem

