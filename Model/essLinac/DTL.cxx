/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/DTL.cxx
 *
 * Copyright (c) 2004-2017 by Konstantin Batkov
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
#include "FixedOffset.h"
#include "surfDBase.h"
#include "surfDIter.h"
#include "surfDivide.h"
#include "SurInter.h"
#include "mergeTemplate.h"
#include "CellMap.h"
#include "DTL.h"

namespace essSystem
{

  DTL::DTL(const std::string& Base,const std::string& Key,const size_t Index) :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Base+Key+StrFunc::makeString(Index),6),
  attachSystem::CellMap(),
  baseName(Base),
  extraName(Base+Key),
  surfIndex(ModelSupport::objectRegister::Instance().cell(keyName)),
  cellIndex(surfIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

DTL::DTL(const DTL& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),attachSystem::CellMap(A),
  baseName(A.baseName),
  extraName(A.extraName),
  surfIndex(A.surfIndex),cellIndex(A.cellIndex),
  engActive(A.engActive),
  length(A.length),
  itLength(A.itLength),
  itRadius(A.itRadius),
  itWallThick(A.itWallThick),
  nLayers(A.nLayers),radius(A.radius),coverThick(A.coverThick),
  mat(A.mat),
  airMat(A.airMat)
  /*!
    Copy constructor
    \param A :: DTL to copy
  */
{}

DTL&
DTL::operator=(const DTL& A)
  /*!
    Assignment operator
    \param A :: DTL to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      cellIndex=A.cellIndex;
      engActive=A.engActive;
      length=A.length;
      itLength=A.itLength;
      itRadius=A.itRadius;
      itWallThick=A.itWallThick;
      nLayers=A.nLayers;
      radius=A.radius;
      mat=A.mat;
      airMat=A.airMat;
      coverThick=A.coverThick;
    }
  return *this;
}

DTL*
DTL::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new DTL(*this);
}

DTL::~DTL()
  /*!
    Destructor
  */
{}

void
DTL::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("DTL","populate");

  FixedOffset::populate(Control);
  engActive=Control.EvalTriple<int>(keyName,baseName,"","EngineeringActive");

  length=Control.EvalVar<double>(keyName+"Length");
  itLength=Control.EvalVar<double>(keyName+"IntertankLength");
  itRadius=Control.EvalPair<double>(keyName,extraName,"IntertankRadius");
  itWallThick=Control.EvalPair<double>(keyName,extraName,"IntertankWallThick");
  nLayers=Control.EvalPair<size_t>(keyName,extraName,"NLayers");
  double R;
  int m;
  std::string strmat;
  for (size_t i=0; i<nLayers; i++)
    {
      R = Control.EvalPair<double>(keyName,extraName,
				   "Radius"+std::to_string(i+1));
      radius.push_back(R);

      strmat = "Mat"+std::to_string(i+1);
      m = ModelSupport::EvalMat<int>(Control,keyName+strmat,extraName+strmat);
      mat.push_back(m);
    }

  coverThick=Control.EvalPair<double>(keyName,extraName,"CoverThick");
  airMat = ModelSupport::EvalMat<int>(Control,extraName+"AirMat",baseName+"AirMat");

  return;
}

void
DTL::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: object for origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("DTL","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
DTL::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("DTL","createSurfaces");

  // dividers
  ModelSupport::buildPlane(SMap,surfIndex+3,Origin,X);
  ModelSupport::buildPlane(SMap,surfIndex+5,Origin,Z);

  //  SMap.addMatch(surfIndex+1,FC.getSignedLinkSurf(sideIndex));
  ModelSupport::buildPlane(SMap,surfIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,surfIndex+2,Origin+Y*(length),Y);

  // cover
  ModelSupport::buildPlane(SMap,surfIndex+11,Origin+Y*(coverThick),Y);
  ModelSupport::buildPlane(SMap,surfIndex+12,Origin+Y*(length-coverThick),Y);

  // intertank
  ModelSupport::buildPlane(SMap,surfIndex+22,Origin+Y*(length+itLength),Y);
  ModelSupport::buildCylinder(SMap,surfIndex+8,Origin,Y,itRadius);
  ModelSupport::buildCylinder(SMap,surfIndex+9,Origin,Y,itRadius+itWallThick);

  int SI(surfIndex);
  for (size_t i=0; i<nLayers; i++)
    {
      ModelSupport::buildCylinder(SMap,SI+7,Origin,Y,radius[i]);
      SI += 10;
    }

  return;
}

void
DTL::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("DTL","createObjects");

  std::string Out;

  int SI(surfIndex);
  for (size_t i=0; i<nLayers; i++)
    {
      if (i==0)
	{
	  Out=ModelSupport::getComposite(SMap,surfIndex," 11 -12 -7 ");
	} else
	{
	  Out=ModelSupport::getComposite(SMap,surfIndex,SI,SI-10, " 11 -12 -7M 7N ");
	}
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat[i],0.0,Out));
      SI += 10;
    }

  // covers
  Out=ModelSupport::getComposite(SMap,surfIndex,SI-10," 1 -11 -8 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex,SI-10," 1 -11 8 -7M ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat.back(),0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex,SI-10," 12 -2 -8 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex,SI-10," 12 -2 8 -7M ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat.back(),0.0,Out));

  // intertank
  if (itLength>0.0)
    {
      Out=ModelSupport::getComposite(SMap,surfIndex," 2 -22 -8 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

      Out=ModelSupport::getComposite(SMap,surfIndex,SI-10," 2 -22 8 -9 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat.back(),0.0,Out));

      Out=ModelSupport::getComposite(SMap,surfIndex,SI-10," 2 -22 9 -7M ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,airMat,0.0,Out));
    }

  Out=ModelSupport::getComposite(SMap,surfIndex,SI-10," 1 -22 -7M ");
  addOuterSurf(Out);

  return;
}


void
DTL::createLinks()

  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("DTL","createLinks");

  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(surfIndex+1));

  FixedComp::setConnect(1,Origin+Y*(length+itLength),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(surfIndex+22));

  const int SI(surfIndex+static_cast<int>(nLayers-1)*10);
  const double hl = (length+itLength)/2.0;

  FixedComp::setConnect(2,Origin+Y*(hl)-Z*radius.back(),-Z);
  FixedComp::setLinkSurf(2,SMap.realSurf(SI+7));
  FixedComp::addLinkSurf(2,-SMap.realSurf(surfIndex+5));
  ELog::EM << "addLinkSurf or addBridgeSurf ?" << ELog::endDiag;

  FixedComp::setConnect(3,Origin+Y*(hl)+Z*radius.back(),Z);
  FixedComp::setLinkSurf(3,SMap.realSurf(SI+7));
  FixedComp::addLinkSurf(3,SMap.realSurf(surfIndex+5));

  FixedComp::setConnect(4,Origin+Y*(hl)-X*radius.back(),-X);
  FixedComp::setLinkSurf(4,SMap.realSurf(SI+7));
  FixedComp::addLinkSurf(4,-SMap.realSurf(surfIndex+3));

  FixedComp::setConnect(5,Origin+Y*(hl)+X*radius.back(),X);
  FixedComp::setLinkSurf(5,SMap.realSurf(SI+7));
  FixedComp::addLinkSurf(5,SMap.realSurf(surfIndex+3));

  //  for (int i=0; i<6; i++)
  //    ELog::EM << keyName << " " << i << "\t" << getLinkSurf(i) << "\t" << getLinkPt(i) << "\t\t" << getLinkAxis(i) << ELog::endDiag;

  return;
}




void
DTL::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("DTL","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // essSystem
