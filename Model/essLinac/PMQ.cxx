/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essLinac/PMQ.cxx
 *
 * Copyright (c) 2017-2021 by Konstantin Batkov
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
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Rules.h"
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
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"

#include "PMQ.h"

namespace essSystem
{

  PMQ::PMQ(const std::string& Base,const std::string& Key,const size_t Index) :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Base+Key+std::to_string(Index),6),
  attachSystem::CellMap(),
  baseName(Base),
  extraName(Base+Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

PMQ::PMQ(const PMQ& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),attachSystem::CellMap(A),
  baseName(A.baseName),
  extraName(A.extraName),
  length(A.length),
  gapLength(A.gapLength),
  nLayers(A.nLayers),radius(A.radius),
  mat(A.mat),
  airMat(A.airMat),
  nBars(A.nBars),
  barHeight(A.barHeight),
  barThick(A.barThick),
  barMat(A.barMat)
  /*!
    Copy constructor
    \param A :: PMQ to copy
  */
{}

PMQ&
PMQ::operator=(const PMQ& A)
  /*!
    Assignment operator
    \param A :: PMQ to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      length=A.length;
      gapLength=A.gapLength;
      nLayers=A.nLayers;
      radius=A.radius;
      mat=A.mat;
      airMat=A.airMat;
      nBars=A.nBars;
      barHeight=A.barHeight;
      barThick=A.barThick;
      barMat=A.barMat;
    }
  return *this;
}

PMQ*
PMQ::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new PMQ(*this);
}

PMQ::~PMQ()
  /*!
    Destructor
  */
{}

void
PMQ::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("PMQ","populate");

  FixedOffset::populate(Control);

  length=Control.EvalTail<double>(keyName,extraName,"Length");
  gapLength=Control.EvalVar<double>(keyName+"GapLength");
  nLayers=Control.EvalTail<size_t>(keyName,baseName,"NLayers");

  double R;
  int m;
  std::string strmat;
  for (size_t i=0; i<nLayers; i++)
    {
      R = Control.EvalTail<double>(keyName,baseName,
				   "Radius"+std::to_string(i+1));
      radius.push_back(R);

      strmat = "Mat"+std::to_string(i+1);
      m = ModelSupport::EvalMat<int>(Control,keyName+strmat,baseName+strmat);
      mat.push_back(m);
    }

  airMat = ModelSupport::EvalMat<int>
    (Control,extraName+"AirMat",baseName+"AirMat");

  nBars = Control.EvalTail<size_t>(keyName,extraName,"NBars");
  if (nBars % 2)
    throw ColErr::NumericalAbort(keyName+"NBars must be even");
  
  barHeight = Control.EvalTail<double>(keyName,extraName,"BarHeight");
  barThick = Control.EvalTail<double>(keyName,extraName,"BarThick");
  barMat = ModelSupport::EvalMat<int>(Control,keyName+"BarMat",extraName+"BarMat");

  return;
}

void
PMQ::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("PMQ","createSurfaces");

  // dividers
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin,X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin,Z);

  //  SMap.addMatch(buildIndex+1,FC.getSignedLinkSurf(sideIndex));
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(length+gapLength),Y);

  int SI(buildIndex);
  for (size_t i=0; i<nLayers; i++)
    {
      ModelSupport::buildCylinder(SMap,SI+7,Origin,Y,radius[i]);
      SI += 10;
    }

  // magnet bars
  int SJ(buildIndex+static_cast<int>(nLayers-1)*10);
  double theta(0.0);
  const double dTheta = 360.0/static_cast<double>(nBars);
  // +1 in order to repeat 1st bar surfaces for simpler definition of the last bar void
  for (size_t j=0; j<nBars/2+1; j++)
    {
      Geometry::Vec3D dirX(X);
      Geometry::Vec3D dirZ(Z);
      Geometry::Quaternion::calcQRotDeg(theta,Y).rotate(dirX);
      Geometry::Quaternion::calcQRotDeg(theta,Y).rotate(dirZ);
      ModelSupport::buildPlane(SMap, SJ+2, dirX,-barThick/2.0);
      ModelSupport::buildPlane(SMap, SJ+3, dirX, barThick/2.0);
      ModelSupport::buildPlane(SMap, SJ+5, Origin, dirZ);
      SJ += 10;
      theta += dTheta;
    }
  return;
}

void
PMQ::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("PMQ","createObjects");

  HeadRule HR,sideHR;

  int SI(buildIndex);
  int SJ(buildIndex+static_cast<int>(nLayers-1)*10); // magnet bars

  for (size_t i=0; i<nLayers; i++)
    {
      if (i==0)
	{
	  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -7");
	  makeCell("Magnet",System,cellIndex++,mat[i],0.0,HR);
	}
      else if (i==2) // magnet goes there
	{
	  sideHR=ModelSupport::getHeadRule(SMap,buildIndex,SI-10,"1 -2 -17M 7M");
	  for (size_t j=0; j<nBars/2;j++)
	    {
	      HR=ModelSupport::getHeadRule(SMap,SJ,"2 -3");
	      makeCell("Magnet",System,cellIndex++,barMat,0.0,HR*sideHR);
	      HR=ModelSupport::getHeadRule(SMap,SJ,"5 3 -2M");
	      makeCell("Magnet",System,cellIndex++,mat[i],0.0,HR*sideHR);
	      HR=ModelSupport::getHeadRule(SMap,SJ,"-5 -2 3M");
	      makeCell("Magnet",System,cellIndex++,mat[i],0.0,HR*sideHR);
	      SJ += 10;
	    }
	}
      else
	{
	  HR=ModelSupport::getHeadRule(SMap,buildIndex,SI-10,"1 -2 -17M 7M");
	  makeCell("Bar",System,cellIndex++,mat[i],0.0,HR);
	}
      SI += 10;
    }

  // gap
  const size_t ldtl(3); // number of layers in the gap
  SI = buildIndex+static_cast<int>(nLayers-ldtl)*10;
  for (size_t i=0; i<ldtl; i++)
    {
      if (i==0)
	HR=ModelSupport::getHeadRule(SMap,buildIndex,SI,"2 -12 -7M");
      else
	HR=ModelSupport::getHeadRule(SMap,buildIndex,SI-10,"2 -12 17M -7M");

      const size_t matIndex(i ? nLayers-ldtl+i : 0);
      makeCell("Gap",System,cellIndex++,mat[matIndex],0.0,HR);
      SI += 10;
    }

  HR=ModelSupport::getHeadRule(SMap,buildIndex,SI-10,"1 -12 -7M");
  addOuterSurf(HR);

  return;
}


void
PMQ::createLinks()

  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("PMQ","createLinks");

  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));

  FixedComp::setConnect(1,Origin+Y*(length+gapLength),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+12));

  const int SI(buildIndex+static_cast<int>(nLayers-1)*10);
  const double hl = (length+gapLength)/2.0;

  FixedComp::setConnect(2,Origin+Y*(hl)-Z*radius.back(),-Z);
  FixedComp::setLinkSurf(2,SMap.realSurf(SI+7));
  FixedComp::addBridgeSurf(2,-SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(3,Origin+Y*(hl)+Z*radius.back(),Z);
  FixedComp::setLinkSurf(3,SMap.realSurf(SI+7));
  FixedComp::addBridgeSurf(3,SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(4,Origin+Y*(hl)-X*radius.back(),-X);
  FixedComp::setLinkSurf(4,SMap.realSurf(SI+7));
  FixedComp::addBridgeSurf(4,-SMap.realSurf(buildIndex+3));

  FixedComp::setConnect(5,Origin+Y*(hl)+X*radius.back(),X);
  FixedComp::setLinkSurf(5,SMap.realSurf(SI+7));
  FixedComp::addBridgeSurf(5,SMap.realSurf(buildIndex+3));

  // for (int i=6; i<8; i++)
  //   ELog::EM << keyName << " " << i << "\t" << getLinkSurf(i) << "\t" << getLinkPt(i) << "\t\t" << getLinkAxis(i) << ELog::endDiag;

  return;
}




void
PMQ::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("PMQ","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // essSystem
