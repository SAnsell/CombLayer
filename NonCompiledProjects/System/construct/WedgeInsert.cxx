/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/WedgeInsert.cxx
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
#include <algorithm>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "surfRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "RuleSupport.h"
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
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "LayerComp.h"

#include "WedgeInsert.h"

namespace constructSystem
{

WedgeInsert::WedgeInsert(const std::string& Key,const size_t Index) :
  attachSystem::FixedOffset(Key+std::to_string(Index),6),
  attachSystem::ContainedComp(),baseName(Key)
  /*!
    Constructor
    \param Key :: Name of construction key
    \param Index :: Index number for component
  */
{}

WedgeInsert::WedgeInsert(const WedgeInsert& A) : 
  attachSystem::FixedOffset(A),attachSystem::ContainedComp(A),
  baseName(A.baseName),
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

  viewWidth=Control.EvalTail<double>(keyName,baseName,"ViewWidth");   
  viewHeight=Control.EvalTail<double>(keyName,baseName,"ViewHeight");   
  viewXY=Control.EvalTail<double>(keyName,baseName,"ViewXY");   
  viewZ=Control.EvalTail<double>(keyName,baseName,"ViewZ");   
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat",baseName+"Mat");   

  wall=Control.EvalTail<double>(keyName,baseName,"Wall");   
  wallMat=(wall>Geometry::zeroTol) ? 
    ModelSupport::EvalMat<int>(Control,keyName+"WallMat"
			       ,baseName+"WallMat") : 0;   
  temp=Control.EvalTail<double>(keyName,baseName,"Temp");
  
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
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);  
  // sides:
  ModelSupport::buildPlane(SMap,buildIndex+3,
			   Origin-X*(viewWidth/2.0),vXMinus);  
  ModelSupport::buildPlane(SMap,buildIndex+4,
			   Origin+X*(viewWidth/2.0),vXPlus);  
  ModelSupport::buildPlane(SMap,buildIndex+5,
			   Origin-Z*(viewHeight/2.0),vZMinus);  
  ModelSupport::buildPlane(SMap,buildIndex+6,
			   Origin+Z*(viewHeight/2.0),vZPlus);  

  if (wall>Geometry::zeroTol)
    {
      // Back plane
      ModelSupport::buildPlane(SMap,buildIndex+11,Origin-Y*wall,Y);  
      // sides:
      ModelSupport::buildPlane(SMap,buildIndex+13,
			       Origin-X*(wall+viewWidth/2.0),vXMinus);  
      ModelSupport::buildPlane(SMap,buildIndex+14,
			       Origin+X*(wall+viewWidth/2.0),vXPlus);  
      ModelSupport::buildPlane(SMap,buildIndex+15,
			       Origin-Z*(wall+viewHeight/2.0),vZMinus);  
      ModelSupport::buildPlane(SMap,buildIndex+16,
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
WedgeInsert::createObjects(Simulation& System)
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

  if (!LCPtr)
    throw ColErr::EmptyValue<attachSystem::LayerComp*>("LayerComp LCPtr");

  std::string Out;
  const std::string CShape=
    MonteCarlo::getComplementShape
    (LCPtr->getLayerString(layerIndex,static_cast<long int>(layerIndex+1)));

  if (wall>Geometry::zeroTol)
    {

      Out=ModelSupport::getComposite(SMap,buildIndex," 1 3 -4 5 -6 ");
      Out+=CShape;
      System.addCell(MonteCarlo::Object(cellIndex++,mat,0.0,Out));

      Out=ModelSupport::getComposite(SMap,buildIndex,
				     " 11 13 -14 15 -16 ( -1:-3:4:-5:6 )" );
      Out+=CShape;
      System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));

      Out=ModelSupport::getComposite(SMap,buildIndex," 11 13 -14 15 -16 ");
      
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


  const int WI((wall>Geometry::zeroTol) ? buildIndex+10 : buildIndex);
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
WedgeInsert::setLayer(const attachSystem::LayerComp& LC,
		      const size_t LIndex,const size_t LSide)
  /*!
    Set layer components
    \param LC :: LayerComp
    \param LIndex :: Layer index 
    \param LSide :: Layer side
   */
{
  LCPtr=&LC;
  layerIndex=LIndex;
  layerSide=LSide;
  return;
}

void
WedgeInsert::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)

/*!
    Extrenal build everything
    \param System :: Simulation
    \param mainCell :: Main cell
    \param FC :: FixedComponent for origin
    \param sideIndex :: Layer side to use
   */
{
  ELog::RegMethod RegA("WedgeInsert","createAll");
  
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);  // layerIndex was 1
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE constructSystem

