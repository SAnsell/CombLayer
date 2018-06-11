/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/StubWall.cxx
 *
 * Copyright (c) 2018 by Konstantin Batkov
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
#include "surfDBase.h"
#include "surfDIter.h"
#include "surfDivide.h"
#include "SurInter.h"
#include "mergeTemplate.h"

#include "CellMap.h"

#include "StubWall.h"

namespace essSystem
{

  StubWall::StubWall(const std::string& baseKey,const std::string& extraKey,
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

StubWall::StubWall(const StubWall& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),
  attachSystem::CellMap(A),
  baseName(A.baseName),
  Index(A.Index),
  surfIndex(A.surfIndex),cellIndex(A.cellIndex),
  length(A.length),width(A.width),
  height(A.height),
  nLayers(A.nLayers),
  mat(A.mat),
  gapActive(A.gapActive),
  gapMat(A.gapMat),
  gapOffset(A.gapOffset),
  gapWidth(A.gapWidth),
  gapHeight(A.gapHeight),
  gapDist(A.gapDist)
  /*!
    Copy constructor
    \param A :: StubWall to copy
  */
{}

StubWall&
StubWall::operator=(const StubWall& A)
  /*!
    Assignment operator
    \param A :: StubWall to copy
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
      height=A.height;
      nLayers=A.nLayers;
      mat=A.mat;
      gapActive=A.gapActive;
      gapMat=A.gapMat;
      gapOffset=A.gapOffset;
      gapWidth=A.gapWidth;
      gapHeight=A.gapHeight;
      gapDist=A.gapDist;
    }
  return *this;
}

StubWall*
StubWall::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new StubWall(*this);
}

StubWall::~StubWall()
  /*!
    Destructor
  */
{}

void
StubWall::layerProcess(Simulation& System, const std::string& cellName,
		       const long int& lpS, const long int& lsS,
		       const std::vector<double>& frac,
		       const std::vector<int>& fracMat)
  /*!
    Processes the splitting of the surfaces into a multilayer system
    \param System :: Simulation to work on
    \param cellName :: StubWall wall cell name
    \param lpS :: link pont of primary surface
    \param lsS :: link point of secondary surface
    \param frac :: vector of fractions
    \param fracMat :: vector of materials
  */
{
    ELog::RegMethod RegA("StubWall","layerProcess");

    const size_t N(frac.size()+1);

    if (N!=fracMat.size())
      throw ColErr::SizeError<size_t>(N,fracMat.size(),
				   "size of fracMat must be equal to the size of frac+1");

    if (N<=1)
      return;

    const int pS(getLinkSurf(lpS));
    const int sS(getLinkSurf(lsS));

    const attachSystem::CellMap* CM = dynamic_cast<const attachSystem::CellMap*>(this);
    MonteCarlo::Object* wallObj(0);
    int wallCell(0);

    if (CM)
      {
	wallCell=CM->getCell(cellName);
	wallObj=System.findQhull(wallCell);
      }

    if (!wallObj)
      throw ColErr::InContainerError<int>(wallCell,
					  "Cell '" + cellName + "' not found");

    double baseFrac = frac[0];
    ModelSupport::surfDivide DA;
    for(size_t i=1;i<N;i++)
      {
	DA.addFrac(baseFrac);
	DA.addMaterial(fracMat[i-1]);
	baseFrac += frac[i];
      }
    DA.addMaterial(fracMat.back());

    DA.setCellN(wallCell);
    DA.setOutNum(cellIndex, surfIndex+10000);

    ModelSupport::mergeTemplate<Geometry::Plane,
				Geometry::Plane> surroundRule;

    surroundRule.setSurfPair(SMap.realSurf(pS),SMap.realSurf(sS));

    std::string OutA(getLinkString(lpS));
    std::string OutB(getLinkString(-lsS));

    surroundRule.setInnerRule(OutA);
    surroundRule.setOuterRule(OutB);

    DA.addRule(&surroundRule);
    DA.activeDivideTemplate(System);

    cellIndex=DA.getCellNum();

    return;
}

void
StubWall::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("StubWall","populate");

  FixedOffset::populate(Control);
  length=Control.EvalVar<double>(keyName+"Length");
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  nLayers=Control.EvalDefVar<size_t>(keyName+"NLayers",1);
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  gapActive=Control.EvalDefVar<int>(keyName+"GapActive",0);
  if (gapActive)
    {
      gapMat=ModelSupport::EvalMat<int>(Control,keyName+"GapMat");
      gapOffset=Control.EvalVar<double>(keyName+"GapOffset");
      gapWidth=Control.EvalVar<double>(keyName+"GapWidth");
      gapHeight=Control.EvalVar<double>(keyName+"GapHeight");
      gapDist=Control.EvalVar<double>(keyName+"GapDist");
    }
  else
    {
      gapMat = 0;
      gapOffset = 0.0;
      gapWidth = 0.0;
      gapHeight = 0.0;
      gapDist = 0.0;
    }

  return;
}

void
StubWall::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: object for origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("StubWall","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
StubWall::createSurfaces(const attachSystem::FixedComp& FC,const long int floor)
  /*!
    Create All the surfaces
    \param FC :: Central origin
  */
{
  ELog::RegMethod RegA("StubWall","createSurfaces");

  ModelSupport::buildPlane(SMap,surfIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,surfIndex+2,Origin+Y*(width),Y);

  ModelSupport::buildPlane(SMap,surfIndex+3,Origin-X*(length/2.0),X);
  ModelSupport::buildPlane(SMap,surfIndex+4,Origin+X*(length/2.0),X);

  ModelSupport::buildShiftedPlane(SMap,surfIndex+6,
				  SMap.realPtr<Geometry::Plane>(FC.getLinkSurf(floor)),
				  height);

  return;
}

void
StubWall::createObjects(Simulation& System,const attachSystem::FixedComp& FC,
		   const long int floor,const long int roof)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
    \param floor :: link point for floor
    \param roof  :: link point for roof
  */
{
  ELog::RegMethod RegA("StubWall","createObjects");

  std::string Out=ModelSupport::getComposite(SMap,surfIndex," 1 -2 3 -4 -6");
  Out += FC.getLinkString(floor);
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));
  setCell("wall", cellIndex-1);

  if (gapActive)
    {
      std::vector<double> frac;
      frac.push_back(gapOffset/height);
      frac.push_back(gapHeight/height);

      std::vector<int> fracMat;
      for (int i=0; i<3; i++)
	fracMat.push_back(mat);

      layerProcess(System, "wall", -5,6,frac,fracMat);
      setCell("GapLevel", cellIndex-2);

      frac.clear();
      frac.push_back((length-2*gapWidth-gapDist)/length/2.0);
      frac.push_back(gapWidth/length);
      frac.push_back(gapDist/length);
      frac.push_back(gapWidth/length);

      fracMat.clear();
      for (int i=0; i<5; i++)
	fracMat.push_back(i%2 ? gapMat : mat);

      layerProcess(System, "GapLevel", -3,4,frac,fracMat);
    }

  addOuterSurf(Out);

  return;
}


void
StubWall::createLinks(const attachSystem::FixedComp& FC,
		 const long int floor,const long int roof)
/*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("StubWall","createLinks");

  FixedComp::setConnect(0,Origin,Y);
  FixedComp::setLinkSurf(0,SMap.realSurf(surfIndex+1));

  FixedComp::setConnect(1,Origin+Y*(width),-Y);
  FixedComp::setLinkSurf(1,-SMap.realSurf(surfIndex+2));

  FixedComp::setConnect(2,Origin-X*(length/2.0),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(surfIndex+3));

  FixedComp::setConnect(3,Origin+X*(length/2.0),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(surfIndex+4));

  FixedComp::setConnect(4,Origin+Z*(FC.getLinkPt(8).Z())+
			Y*(width/2.0),-Z);
  FixedComp::setLinkSurf(4,-FC.getLinkSurf(floor));

  FixedComp::setConnect(5,getLinkPt(5)+Z*height,Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(surfIndex+6));

  //   for (int i=0; i<=6; i++)
  //     ELog::EM << "lp " << i << ":\t" << getLinkSurf(i) << " " << getLinkPt(i) << ELog::endDiag;

  return;
}




void
StubWall::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex,
		    const attachSystem::FixedComp& FC1,
		    const long int floor,
		    const long int roof)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
    \param FC1 :: roof/floor FixedComponent
    \param floor :: link point for floor
    \param roof  :: link point for roof
  */
{
  ELog::RegMethod RegA("StubWall","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces(FC1,floor);
  createLinks(FC1,floor,roof);
  createObjects(System,FC1,floor,roof);
  insertObjects(System);

  return;
}

}  // essSystem
