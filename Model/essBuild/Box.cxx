/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/Box.cxx
 *
 * Copyright (c) 2017 by Konstantin Batkov
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
#include "LayerComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "surfDBase.h"
#include "surfDIter.h"
#include "surfDivide.h"
#include "SurInter.h"

#include "Box.h"

namespace essSystem
{

Box::Box(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,9),
  attachSystem::LayerComp(0),
  attachSystem::CellMap(),
  surfIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(surfIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

Box::Box(const Box& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),
  attachSystem::LayerComp(A),
  attachSystem::CellMap(A),
  surfIndex(A.surfIndex),cellIndex(A.cellIndex),
  engActive(A.engActive),
  nLayers(A.nLayers),
  length(A.length),width(A.width),height(A.height),
  depth(A.depth),
  mat(A.mat),
  temp(A.temp),
  sideRule(A.sideRule)
  /*!
    Copy constructor
    \param A :: Box to copy
  */
{}

Box&
Box::operator=(const Box& A)
  /*!
    Assignment operator
    \param A :: Box to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::LayerComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      cellIndex=A.cellIndex;
      engActive=A.engActive;
      nLayers=A.nLayers;
      length=A.length;
      width=A.width;
      height=A.height;
      depth=A.depth;
      mat=A.mat;
      temp=A.temp;
      sideRule=A.sideRule;
    }
  return *this;
}

Box*
Box::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new Box(*this);
}

Box::~Box()
  /*!
    Destructor
  */
{}

void
Box::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("Box","populate");

  FixedOffset::populate(Control);
  engActive=Control.EvalPair<int>(keyName,"","EngineeringActive");

  double L(0.0);
  double W(0.0);
  double H(0.0);
  double D(0.0);
  double T(0.0);
  int M;
  nLayers=Control.EvalVar<size_t>(keyName+"NLayers");
  for (size_t i=0; i<nLayers; i++)
    {
      const std::string NStr(std::to_string(i));
      L+=Control.EvalVar<double>(keyName+"Length"+NStr) * ((i==0) ? 1.0 : 2.0);
      W+=Control.EvalVar<double>(keyName+"Width"+NStr) * ((i==0) ? 1.0 : 2.0);
      H+=Control.EvalVar<double>(keyName+"Height"+NStr);
      D+=Control.EvalVar<double>(keyName+"Depth"+NStr);
      M=ModelSupport::EvalMat<int>(Control,keyName+"Mat"+NStr);
      const std::string TStr=keyName+"Temp"+NStr;
      T=(!M || !Control.hasVariable(TStr)) ? 0.0 : Control.EvalVar<double>(TStr);

      length.push_back(L);
      width.push_back(W);
      height.push_back(H);
      depth.push_back(D);
      mat.push_back(M);
      temp.push_back(T);
    }

  return;
}

void
Box::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: object for origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("Box","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  const double D=(depth.empty()) ? 0.0 : depth.back();
  applyShift(0,0,D);

  return;
}

void
Box::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("Box","createSurfaces");

  int SI(surfIndex);
  for (size_t i=0;i<nLayers;i++)
    {
      const double l = length[i]/2.0;
      const double w = width[i]/2.0;

      ModelSupport::buildPlane(SMap,SI+1,Origin-Y*l,Y);
      ModelSupport::buildPlane(SMap,SI+2,Origin+Y*l,Y);
      ModelSupport::buildPlane(SMap,SI+3,Origin-X*w,X);
      ModelSupport::buildPlane(SMap,SI+4,Origin+X*w,X);
      ModelSupport::buildPlane(SMap,SI+5,Origin-Z*(depth[i]),Z);
      ModelSupport::buildPlane(SMap,SI+6,Origin+Z*(height[i]),Z);
      SI += 10;
    }


  return;
}

void
Box::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("Box","createObjects");

  std::string Out;
  int SI(surfIndex);
  for (size_t i=0; i<nLayers; i++)
    {
      Out=ModelSupport::getComposite(SMap,SI," 1 -2 3 -4 5 -6 ");
      if (i)
	Out += ModelSupport::getComposite(SMap,SI-10," (-1:2:-3:4:-5:6) ");
      else
	CellMap::setCell("Inner", cellIndex-1);

      System.addCell(MonteCarlo::Qhull(cellIndex++,mat[i],temp[i],Out));

      SI += 10;
    }
  
  Out=ModelSupport::getComposite(SMap,SI-10," 1 -2 3 -4 5 -6 ");
  addOuterSurf(Out);

  sideRule=ModelSupport::getComposite(SMap,SI-10," 1 -2 3 -4 ");

  return;
}


void
Box::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("Box","createLinks");

  const int SI(surfIndex+static_cast<int>(nLayers-1)*10);

  const double l = length[nLayers-1]/2.0;
  const double w = width[nLayers-1]/2.0;

  FixedComp::setConnect(0,Origin-Y*l,-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(SI+1));
  FixedComp::setConnect(1,Origin+Y*l,Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(SI+2));

  FixedComp::setConnect(2,Origin-X*w,-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(SI+3));
  FixedComp::setConnect(3,Origin+X*w,X);
  FixedComp::setLinkSurf(3,SMap.realSurf(SI+4));

  FixedComp::setConnect(4,Origin-Z*(depth[nLayers-1]),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(SI+5));
  FixedComp::setConnect(5,Origin+Z*(height[nLayers-1]),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(SI+6));

  // inner link points for F5 collimators
  FixedComp::setConnect(7,Origin-Z*depth[0],Z);
  FixedComp::setLinkSurf(7,SMap.realSurf(surfIndex+5));

  FixedComp::setConnect(8,Origin+Z*height[0],-Z);
  FixedComp::setLinkSurf(8,-SMap.realSurf(surfIndex+6));

  return;
}

int
Box::getLayerSurf(const size_t layerIndex,
		  const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link surf
    \param layerIndex :: layer, 0 is inner moderator [0-4]
    \param sideIndex :: Side [1-4]
    \return Surface string
  */
{
  ELog::RegMethod RegA("Box","getLinkSurf");

  throw ColErr::AbsObjMethod("Not implemented yet - do it similar to DiskPreMod?");
}


std::string
Box::getLayerString(const size_t layerIndex,
                           const long int sideIndex) const
  /*!
    Given a side and a layer calculate the layer string
    \param layerIndex :: layer, 0 is inner moderator [0-4]
    \param sideIndex :: Side [0-3]
    \return Surface string
  */
{
  ELog::RegMethod RegA("Box","getLayerString");

  if (layerIndex>nLayers)
    throw ColErr::IndexError<size_t>(layerIndex,nLayers,"layer");

  const int SI(10*static_cast<int>(layerIndex)+surfIndex);

  std::string Out;
  const long int uSIndex(std::abs(sideIndex));
  switch(uSIndex)
    {
    case 1:
      Out=ModelSupport::getComposite(SMap,SI," -1 ");
      break;
    case 2:
      Out=ModelSupport::getComposite(SMap,SI," 2 ");
      break;
    case 3:
      Out=ModelSupport::getComposite(SMap,SI," -3 ");
      break;
    case 4:
      Out=ModelSupport::getComposite(SMap,SI," 4 ");
      break;
    case 5:
      Out=ModelSupport::getComposite(SMap,SI," -5 ");
      break;
    case 6:
      Out=ModelSupport::getComposite(SMap,SI," 6 ");
      break;
    default:
      throw ColErr::IndexError<long int>(sideIndex,6,"sideIndex");
    }
  if (sideIndex<0)
    {
      HeadRule HR(Out);
      HR.makeComplement();
      return HR.display();
    }

  return Out;
}

Geometry::Vec3D
Box::getSurfacePoint(const size_t layerIndex,
                            const long int sideIndex) const
  /*!
    Given a side and a layer calculate the surface point
    \param layerIndex :: layer, 0 is inner moderator
    \param sideIndex :: Side [0-6]
   \return Surface point
  */
{
  ELog::RegMethod RegA("Box","getSurfacePoint");

  if (layerIndex>nLayers)
    throw ColErr::IndexError<size_t>(layerIndex,nLayers,"layer");
  if (!sideIndex) return Origin;
  const size_t SI((sideIndex>0) ?
                  static_cast<size_t>(sideIndex-1) :
                  static_cast<size_t>(-1-sideIndex));
  
  const double l = length[layerIndex]/2.0;
  const double w = width[layerIndex]/2.0;

  switch(SI)
    {
    case 0:
      return Origin-Y*l;
    case 1:
      return Origin+Y*l;
    case 2:
      return Origin-X*w;
    case 3:
      return Origin+X*w;
    case 4:
      return Origin-Z*depth[layerIndex];
    case 5:
      return Origin+Z*height[layerIndex];
    }
  throw ColErr::IndexError<long int>(sideIndex,6,"sideIndex ");
}



void
Box::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("Box","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // essSystem
