/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/TSW.cxx
 *
 * Copyright (c) 2017-2019 by Konstantin Batkov
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
#include "BaseMap.h"
#include "surfDBase.h"
#include "surfDIter.h"
#include "surfDivide.h"
#include "SurInter.h"
#include "mergeTemplate.h"

#include "CellMap.h"

#include "TSW.h"

namespace essSystem
{

TSW::TSW(const std::string& baseKey,const std::string& extraKey,
	 const size_t& index)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(baseKey+extraKey+std::to_string(index),7),
  attachSystem::CellMap(),
  baseName(baseKey),
  Index(index),
  surfIndex(ModelSupport::objectRegister::Instance().cell(keyName)),
  cellIndex(surfIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

TSW::TSW(const TSW& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),
  attachSystem::CellMap(A),
  baseName(A.baseName),
  Index(A.Index),
  surfIndex(A.surfIndex),cellIndex(A.cellIndex),
  length(A.length),width(A.width),nLayers(A.nLayers),
  wallMat(A.wallMat),
  airMat(A.airMat)
  /*!
    Copy constructor
    \param A :: TSW to copy
  */
{}

TSW&
TSW::operator=(const TSW& A)
  /*!
    Assignment operator
    \param A :: TSW to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      cellIndex=A.cellIndex;
      length=A.length;
      width=A.width;
      nLayers=A.nLayers;
      wallMat=A.wallMat;
      airMat=A.airMat;
    }
  return *this;
}

TSW*
TSW::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new TSW(*this);
}

TSW::~TSW()
  /*!
    Destructor
  */
{}

void
TSW::layerProcess(Simulation& System,const std::string& cellName,
		  const long int lpS,const long int lsS,
		  const size_t N,const int mat)
  /*!
    Processes the splitting of the surfaces into a multilayer system
    \param System :: Simulation to work on
    \param cellName :: TSW wall cell name
    \param lpS :: link pont of primary surface
    \param lsS :: link point of secondary surface
    \param N :: number of layers to divide to
    \param mat :: material
  */
{
  ELog::RegMethod RegA("TSW","layerProcess");
  
  if (N<=1)
    return;
  
  const long int pS(getLinkSurf(lpS));
  const long int sS(getLinkSurf(lsS));
  
  const attachSystem::CellMap* CM =
    dynamic_cast<const attachSystem::CellMap*>(this);
  MonteCarlo::Object* wallObj(0);
  int wallCell(0);
  
  if (CM)
    {
      wallCell=CM->getCell(cellName);
      wallObj=System.findObject(wallCell);
    }

  if (!wallObj)
    throw ColErr::InContainerError<int>(wallCell,"Cell :"+cellName);

  double baseFrac = 1.0/static_cast<double>(N);
  ModelSupport::surfDivide DA;
  for(size_t i=1;i<N;i++)
    {
      DA.addFrac(baseFrac);
      DA.addMaterial(mat);
      baseFrac += 1.0/static_cast<double>(N);
    }
  DA.addMaterial(mat);
  
  DA.setCellN(wallCell);
  DA.setOutNum(cellIndex, surfIndex+10000);
  
  ModelSupport::mergeTemplate<Geometry::Plane,
			      Geometry::Plane> surroundRule;
  
  surroundRule.setSurfPair(SMap.realSurf(static_cast<int>(pS)),
			   SMap.realSurf(static_cast<int>(sS)));
  
  const std::string OutA = getLinkString(lpS);
  const std::string OutB = getLinkString(-lsS);
  
  surroundRule.setInnerRule(OutA);
  surroundRule.setOuterRule(OutB);
  
  DA.addRule(&surroundRule);
  DA.activeDivideTemplate(System);
  
  cellIndex=DA.getCellNum();

  return;
}

void
TSW::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("TSW","populate");

  FixedOffset::populate(Control);
  length=Control.EvalVar<double>(keyName+"Length");
  width=Control.EvalVar<double>(keyName+"Width");
  nLayers=Control.EvalDefVar<size_t>(keyName+"NLayers",1);
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  airMat=ModelSupport::EvalMat<int>(Control,baseName+"AirMat");

  return;
}

void
TSW::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: object for origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("TSW","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
TSW::createSurfaces(const attachSystem::FixedComp& FC,
		    const long int wall1,
		    const long int wall2)
  /*!
    Create All the surfaces
    \param FC :: Central origin
    \param wall1 :: link point for origin
    \param wall1 :: link point for the opposite wall
  */
{
  ELog::RegMethod RegA("TSW","createSurfaces");

  ModelSupport::buildPlane(SMap,surfIndex+1,Origin,X);
  ModelSupport::buildPlane(SMap,surfIndex+2,Origin+X*(width),X);

  const int w1 = FC.getLinkSurf(wall1);
  ModelSupport::buildShiftedPlane(SMap,surfIndex+4,
				  SMap.realPtr<Geometry::Plane>(w1),
				  length);
  return;
}

void
TSW::createObjects(Simulation& System,const attachSystem::FixedComp& FC,
		   const long int wall1,const long int wall2,
		   const long int floor,const long int roof)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
    \param wall2 :: link point of the Linac wall origin
    \param wall2 :: link point of the opposite Linac wall
    \param floor :: link point for floor
    \param roof  :: link point for roof
  */
{
  ELog::RegMethod RegA("TSW","createObjects");

  const std::string tb(FC.getLinkString(floor) + FC.getLinkString(roof));
  const std::string side(ModelSupport::getComposite(SMap,surfIndex," 1 -2 "));
  const std::string common(side+tb);

  std::string Out = common+FC.getLinkString(wall1) +
    ModelSupport::getComposite(SMap,surfIndex," -4 ");
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));
  setCell("wall", cellIndex-1);

  layerProcess(System, "wall", 3, 7, nLayers, wallMat);

  const double linacWidth(FC.getLinkDistance(wall1, wall2));

  if (linacWidth-length>Geometry::zeroTol)
    {
      Out = common+FC.getLinkString(wall2) +
	ModelSupport::getComposite(SMap,surfIndex," 4 ");
      System.addCell(MonteCarlo::Object(cellIndex++,airMat,0.0,Out));
    }

  Out=common+FC.getLinkString(wall1) + FC.getLinkString(wall2);
  addOuterSurf(Out);

  return;
}


void
TSW::createLinks(const attachSystem::FixedComp& FC,
		 const long int wall1,const long int wall2,
		 const long int floor,const long int roof)
/*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("TSW","createLinks");

  FixedComp::setLinkSignedCopy(0,FC,-wall1);
  FixedComp::setLinkSignedCopy(1,FC,-wall2);

  FixedComp::setConnect(2,Origin-Y*(length/2.0),-X);
  FixedComp::setLinkSurf(2,SMap.realSurf(surfIndex+1));

  FixedComp::setConnect(3,Origin-Y*(length/2.0)+X*(width),X);
  FixedComp::setLinkSurf(3,-SMap.realSurf(surfIndex+2));

  FixedComp::setLinkSignedCopy(4,FC,-floor);
  FixedComp::setLinkSignedCopy(5,FC,-roof);

  FixedComp::setConnect(6,Origin-Y*(length/2.0)+X*(width),X);
  FixedComp::setLinkSurf(6,SMap.realSurf(surfIndex+2));

  ELog::EM << "check the links" << ELog::endCrit;

  for (int i=0; i<=6; i++)
    ELog::EM << i << ":\t" << getLinkSurf(i) << " " << getLinkPt(i) << ELog::endDiag;

  return;
}




void
TSW::createAll(Simulation& System,
	       const attachSystem::FixedComp& FC,
	       const long int wall1,
	       const long int wall2,
	       const long int floor,
	       const long int roof)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param wall1 :: link point for origin
    \param wall2 :: link point for the opposite wall
    \param floor :: link point for floor
    \param roof  :: link point for roof
  */
{
  ELog::RegMethod RegA("TSW","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,-wall1);
  createSurfaces(FC,wall1,wall2);
  createLinks(FC,wall1,wall2,floor,roof);
  createObjects(System,FC,wall1,wall2,floor,roof);
  insertObjects(System);

  return;
}

}  // essSystem
