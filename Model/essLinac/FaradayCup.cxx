/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/FaradayCup.cxx
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

#include "FaradayCup.h"

namespace essSystem
{

FaradayCup::FaradayCup(const std::string &Base,const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Base+Key,8),
  attachSystem::CellMap(),
  baseName(Base),
  surfIndex(ModelSupport::objectRegister::Instance().cell(keyName)),
  cellIndex(surfIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

FaradayCup::FaradayCup(const FaradayCup& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),
  attachSystem::CellMap(),
  baseName(A.baseName),
  surfIndex(A.surfIndex),cellIndex(A.cellIndex),
  active(A.active),
  engActive(A.engActive),
  length(A.length),outerRadius(A.outerRadius),innerRadius(A.innerRadius),
  faceLength(A.faceLength),
  faceRadius(A.faceRadius),
  absLength(A.absLength),
  absMat(A.absMat),
  baseLength(A.baseLength),
  colLength(A.colLength),
  colMat(A.colMat),wallMat(A.wallMat),
  airMat(A.airMat),
  nShieldLayers(A.nShieldLayers),
  shieldWidthLeft(A.shieldWidthLeft),
  shieldWidthRight(A.shieldWidthRight),
  shieldHeight(A.shieldHeight),
  shieldDepth(A.shieldDepth),
  shieldForwardLength(A.shieldForwardLength),
  shieldBackLength(A.shieldBackLength),
  shieldMat(A.shieldMat)
  /*!
    Copy constructor
    \param A :: FaradayCup to copy
  */
{}

FaradayCup&
FaradayCup::operator=(const FaradayCup& A)
  /*!
    Assignment operator
    \param A :: FaradayCup to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      cellIndex=A.cellIndex;
      active=A.active;
      engActive=A.engActive;
      length=A.length;
      outerRadius=A.outerRadius;
      innerRadius=A.innerRadius;
      faceLength=A.faceLength;
      faceRadius=A.faceRadius;
      absLength=A.absLength;
      absMat=A.absMat;
      baseLength=A.baseLength;
      colLength=A.colLength;
      colMat=A.colMat;
      wallMat=A.wallMat;
      airMat=A.airMat;
      nShieldLayers=A.nShieldLayers;
      shieldWidthLeft=A.shieldWidthLeft;
      shieldWidthRight=A.shieldWidthRight;
      shieldHeight=A.shieldHeight;
      shieldDepth=A.shieldDepth;
      shieldForwardLength=A.shieldForwardLength;
      shieldBackLength=A.shieldBackLength;
      shieldMat=A.shieldMat;
    }
  return *this;
}

FaradayCup*
FaradayCup::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new FaradayCup(*this);
}

FaradayCup::~FaradayCup()
  /*!
    Destructor
  */
{}

void
FaradayCup::layerProcess(Simulation& System, const std::string& cellName,
		    const size_t& lpS, const size_t& lsS,
		    const size_t& N, const int& mat)
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
  ELog::RegMethod RegA("FaradayCup","layerProcess");

    if (N<=1)
      return;

    const int pS = getLinkSurf(lpS);
    const int sS = getLinkSurf(lsS);

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

    double baseFrac = 1.0/N;
    ModelSupport::surfDivide DA;
    for(size_t i=1;i<N;i++)
      {
	DA.addFrac(baseFrac);
	DA.addMaterial(mat);
	baseFrac += 1.0/N;
      }
    DA.addMaterial(mat);

    DA.setCellN(wallCell);
    DA.setOutNum(cellIndex, surfIndex+10000);

    ModelSupport::mergeTemplate<Geometry::Plane,
				Geometry::Plane> surroundRule;

    surroundRule.setSurfPair(SMap.realSurf(pS),
			     SMap.realSurf(sS));

    std::string OutA = getLinkString(lpS);
    std::string OutB = getLinkString(-lsS);

    surroundRule.setInnerRule(OutA);
    surroundRule.setOuterRule(OutB);

    DA.addRule(&surroundRule);
    DA.activeDivideTemplate(System);

    cellIndex=DA.getCellNum();

    return;
}


void
FaradayCup::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("FaradayCup","populate");

  FixedOffset::populate(Control);
  active=Control.EvalDefVar<int>(keyName+"Active", 1);
  engActive=Control.EvalTriple<int>(keyName,baseName,"","EngineeringActive");

  length=Control.EvalVar<double>(keyName+"Length");
  outerRadius=Control.EvalVar<double>(keyName+"OuterRadius");
  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");
  faceLength=Control.EvalVar<double>(keyName+"FaceLength");
  faceRadius=Control.EvalVar<double>(keyName+"FaceRadius");
  absLength=Control.EvalVar<double>(keyName+"AbsorberLength");
  absMat=ModelSupport::EvalMat<int>(Control,keyName+"AbsorberMat");
  baseLength=Control.EvalVar<double>(keyName+"BaseLength");
  colLength=Control.EvalVar<double>(keyName+"CollectorLength");
  colMat=ModelSupport::EvalMat<int>(Control,keyName+"CollectorMat");

  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  airMat=ModelSupport::EvalMat<int>(Control,keyName+"AirMat",baseName+"AirMat");

  nShieldLayers=Control.EvalVar<size_t>(keyName+"NShieldLayers");
  shieldBackLength = Control.EvalVar<double>(keyName+"ShieldBackLength");

  for (size_t i=0; i<nShieldLayers; i++)
    {
      double WL,WR,H,D,FL;
      int mat;
      const std::string Num=std::to_string(i+1);
      WL = Control.EvalVar<double>(keyName+"ShieldWidthLeft"+Num);
      WR = Control.EvalVar<double>(keyName+"ShieldWidthRight"+Num);
      H = Control.EvalVar<double>(keyName+"ShieldHeight"+Num);
      D = Control.EvalVar<double>(keyName+"ShieldDepth"+Num);
      FL = Control.EvalVar<double>(keyName+"ShieldForwardLength"+Num);
      mat=ModelSupport::EvalMat<int>(Control,keyName+"ShieldMat"+Num);

      shieldWidthLeft.push_back(WL);
      shieldWidthRight.push_back(WR);
      shieldHeight.push_back(H);
      shieldDepth.push_back(D);
      shieldForwardLength.push_back(FL);
      shieldMat.push_back(mat);
    }

  // check if the variables are sorted
  //std::is_sorted(shieldWidthLeft.begin(),shieldWidthLeft.end());

  return;
}

void
FaradayCup::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: object for origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("FaradayCup","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
FaradayCup::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("FaradayCup","createSurfaces");

  // dividers
  ModelSupport::buildPlane(SMap,surfIndex+300,Origin,X);
  ModelSupport::buildPlane(SMap,surfIndex+500,Origin,Z);

  double dy(0.0);
  ModelSupport::buildPlane(SMap,surfIndex+1,Origin+Y*dy,Y);
  dy += faceLength;
  ModelSupport::buildPlane(SMap,surfIndex+11,Origin+Y*dy,Y);
  dy += absLength;
  ModelSupport::buildPlane(SMap,surfIndex+21,Origin+Y*dy,Y);
  dy += baseLength;
  ModelSupport::buildPlane(SMap,surfIndex+31,Origin+Y*dy,Y);
  dy += colLength;
  ModelSupport::buildPlane(SMap,surfIndex+41,Origin+Y*dy,Y);

  ModelSupport::buildPlane(SMap,surfIndex+2,Origin+Y*(length),Y);

  ModelSupport::buildCylinder(SMap,surfIndex+7,Origin,Y,innerRadius);
  ModelSupport::buildCylinder(SMap,surfIndex+17,Origin,Y,outerRadius);
  ModelSupport::buildCylinder(SMap,surfIndex+27,Origin,Y,faceRadius);

  int SI(surfIndex+100);
  for (size_t i=0; i<nShieldLayers; i++)
    {
      ModelSupport::buildPlane(SMap,SI+1,Origin-Y*(shieldBackLength),Y);
      ModelSupport::buildPlane(SMap,SI+2,Origin+Y*(shieldForwardLength[i]),Y);

      ModelSupport::buildPlane(SMap,SI+3,Origin-X*(shieldWidthLeft[i]),X);
      ModelSupport::buildPlane(SMap,SI+4,Origin+X*(shieldWidthRight[i]),X);

      ModelSupport::buildPlane(SMap,SI+5,Origin-Z*(shieldDepth[i]),Z);
      ModelSupport::buildPlane(SMap,SI+6,Origin+Z*(shieldHeight[i]),Z);

      SI += 10;
    }

  return;
}

void
FaradayCup::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("FaradayCup","createObjects");
  if (!active) return;

  std::string Out;
  // collimator
  Out=ModelSupport::getComposite(SMap,surfIndex," 1 -11 -27 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,airMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex," 1 -11 27 -17 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));

  // absorber
  Out=ModelSupport::getComposite(SMap,surfIndex," 11 -21 -17 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,absMat,0.0,Out));

  // base
  Out=ModelSupport::getComposite(SMap,surfIndex," 21 -31 -7 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  Out=ModelSupport::getComposite(SMap,surfIndex," 21 -41 7 -17 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));

  // collector
  Out=ModelSupport::getComposite(SMap,surfIndex," 31 -41 -7 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,colMat,0.0,Out));

  // back plane
  Out=ModelSupport::getComposite(SMap,surfIndex," 41 -2 -17 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));


  int SI(surfIndex+100);
  for (size_t i=0; i<nShieldLayers; i++)
    {
      if (i==0)
	{
	  Out=ModelSupport::getComposite(SMap,SI,surfIndex,
					 " 1M -2 17M 3 -4 5 -6 ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,shieldMat[0],0.0,Out));

	  Out=ModelSupport::getComposite(SMap,surfIndex,SI," 2 -2M -17 ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,shieldMat[0],0.0,Out));

	  Out=ModelSupport::getComposite(SMap,SI,surfIndex,
					 " 1 -1M 3 -4 5 -6 ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,shieldMat[0],0.0,Out));
	} else
	{
	  Out=ModelSupport::getComposite(SMap,SI,SI-10,
					 " 1 -2M 3 -4 5 -6 (-3M:4M:-5M:6M)");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,shieldMat[i],0.0,Out));
	  setCell("LateralShield" + std::to_string(i),cellIndex-1);

	  Out=ModelSupport::getComposite(SMap,SI,SI-10," 2M -2 3 -4 5 -6 ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,shieldMat[i],0.0,Out));
	  setCell("ForwardShield" + std::to_string(i),cellIndex-1);
	}
      SI += 10;
    }

  if (nShieldLayers>0)
      Out=ModelSupport::getComposite(SMap,SI-10," 1 -2 3 -4 5 -6 ");
  else
      Out=ModelSupport::getComposite(SMap,surfIndex," 1 -2 -17 ");

  addOuterSurf(Out);

  return;
}


void
FaradayCup::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("FaradayCup","createLinks");

  if (nShieldLayers>0)
    {
      const size_t n = nShieldLayers-1;
      const int SI(surfIndex+100+static_cast<int>(n)*10);

      FixedComp::setConnect(0,Origin-Y*shieldBackLength,-Y);
      FixedComp::setLinkSurf(0,-SMap.realSurf(SI+1));

      FixedComp::setConnect(1,Origin+Y*(shieldForwardLength[n]),Y);
      FixedComp::setLinkSurf(1,SMap.realSurf(SI+2));

      FixedComp::setConnect(2,Origin-X*(shieldWidthLeft[n]),-X);
      FixedComp::setLinkSurf(2,-SMap.realSurf(SI+3));

      FixedComp::setConnect(3,Origin+X*(shieldWidthRight[n]),X);
      FixedComp::setLinkSurf(3,SMap.realSurf(SI+4));

      FixedComp::setConnect(4,Origin-Z*(shieldDepth[n]),-Z);
      FixedComp::setLinkSurf(4,-SMap.realSurf(SI+5));

      FixedComp::setConnect(5,Origin+Z*(shieldHeight[n]),Z);
      FixedComp::setLinkSurf(5,SMap.realSurf(SI+6));

      // for layerProccess
      FixedComp::setConnect(6,Origin+Y*(shieldForwardLength[n-1]),Y);
      FixedComp::setLinkSurf(6,SMap.realSurf(SI-10+2));
      FixedComp::setConnect(7,Origin-Z*(shieldDepth[n]),Z);
      FixedComp::setLinkSurf(7,SMap.realSurf(SI+5));
      ELog::EM << "Why instead of lp7 can't layerProccess use link point 4 with reverse direction?" << ELog::endDiag;
    } else
    {
      FixedComp::setConnect(0,Origin,-Y);
      FixedComp::setLinkSurf(0,-SMap.realSurf(surfIndex+1));

      FixedComp::setConnect(1,Origin+Y*(length),Y);
      FixedComp::setLinkSurf(1,SMap.realSurf(surfIndex+2));

      FixedComp::setConnect(2,Origin-X*(outerRadius)+Y*(length/2.0),-X);
      FixedComp::setLinkSurf(2,SMap.realSurf(surfIndex+17));
      FixedComp::setBridgeSurf(2,-SMap.realSurf(surfIndex+300));

      FixedComp::setConnect(3,Origin+X*(outerRadius)+Y*(length/2.0),X);
      FixedComp::setLinkSurf(3,SMap.realSurf(surfIndex+17));
      FixedComp::setBridgeSurf(3,SMap.realSurf(surfIndex+300));

      FixedComp::setConnect(4,Origin-Z*(outerRadius)+Y*(length/2.0),-Z);
      FixedComp::setLinkSurf(4,SMap.realSurf(surfIndex+17));
      FixedComp::setBridgeSurf(4,-SMap.realSurf(surfIndex+500));

      FixedComp::setConnect(5,Origin+Z*(outerRadius)+Y*(length/2.0),Z);
      FixedComp::setLinkSurf(5,SMap.realSurf(surfIndex+17));
      FixedComp::setBridgeSurf(5,SMap.realSurf(surfIndex+500));
    }

  return;
}




void
FaradayCup::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("FaradayCup","createAll");

  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  if (active)
    {
      const size_t j=nShieldLayers-1;
      layerProcess(System, "ForwardShield" + std::to_string(j), 7, 2, 10, shieldMat[j]);
      layerProcess(System, "LateralShield" + std::to_string(j), 8, 6, 20, shieldMat[j]);
    }

  return;
}

}  // essSystem
