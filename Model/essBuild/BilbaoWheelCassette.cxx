/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/BilbaoWheelCassette.cxx
 *
 * Copyright (c) 2017 by Stuart Ansell / Konstantin Batkov
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
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <numeric>
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
#include "AttachSupport.h"

#include "BilbaoWheelCassette.h"

namespace essSystem
{

  BilbaoWheelCassette::BilbaoWheelCassette(const std::string& baseKey,
					   const std::string& extraKey,
					   const size_t& Index)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(baseKey+extraKey+std::to_string(Index),40),
  baseName(baseKey),
  commonName(baseKey+extraKey),
  surfIndex(ModelSupport::objectRegister::Instance().cell(keyName)),
  cellIndex(surfIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

BilbaoWheelCassette::BilbaoWheelCassette(const BilbaoWheelCassette& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),
  baseName(A.baseName),
  commonName(A.commonName),
  surfIndex(A.surfIndex),cellIndex(A.cellIndex),
  engActive(A.engActive),
  bricksActive(A.bricksActive),
  wallThick(A.wallThick),delta(A.delta),temp(A.temp),
  nWallSeg(A.nWallSeg),
  wallSegLength(A.wallSegLength),
  wallSegDelta(A.wallSegDelta),
  wallSegThick(A.wallSegThick),
  mainMat(A.mainMat),wallMat(A.wallMat),
  heMat(A.heMat),
  floor(A.floor),
  roof(A.roof),
  back(A.back),
  front(A.front),
  nBricks(A.nBricks),
  brickWidth(A.brickWidth),
  brickLength(A.brickLength),
  brickGap(A.brickGap),
  brickSteelMat(A.brickSteelMat),
  brickWMat(A.brickWMat),
  pipeCellThick(A.pipeCellThick),
  pipeCellMat(A.pipeCellMat)
  /*!
    Copy constructor
    \param A :: BilbaoWheelCassette to copy
  */
{}

BilbaoWheelCassette&
BilbaoWheelCassette::operator=(const BilbaoWheelCassette& A)
  /*!
    Assignment operator
    \param A :: BilbaoWheelCassette to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      cellIndex=A.cellIndex;
      engActive=A.engActive;
      bricksActive=A.bricksActive;
      wallThick=A.wallThick;
      delta=A.delta;
      temp=A.temp;
      nWallSeg=A.nWallSeg;
      wallSegLength=A.wallSegLength;
      wallSegDelta=A.wallSegDelta;
      wallSegThick=A.wallSegThick;
      mainMat=A.mainMat;
      wallMat=A.wallMat;
      heMat=A.heMat;
      floor=A.floor;
      roof=A.roof;
      back=A.back;
      front=A.front;
      nBricks=A.nBricks;
      brickWidth=A.brickWidth;
      brickLength=A.brickLength;
      brickGap=A.brickGap;
      brickSteelMat=A.brickSteelMat;
      brickWMat=A.brickWMat;
      pipeCellThick=A.pipeCellThick;
      pipeCellMat=A.pipeCellMat;
    }
  return *this;
}

BilbaoWheelCassette*
BilbaoWheelCassette::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new BilbaoWheelCassette(*this);
}

BilbaoWheelCassette::~BilbaoWheelCassette()
  /*!
    Destructor
  */
{}

double
BilbaoWheelCassette::getSegWallArea() const
  /*!
    Calculate segmented wall area.
   */
{
  ELog::RegMethod RegA("BilbaoWheelCassette","getSegWallArea");

  // get segmented wall area:
  double s(0.0);
  double h1(wallSegThick); // downstream thick
  double h2(0.0); // upstream thick
  double h3(0.0); // leg of triangle: h2=h1+h3
  for (size_t i=0; i<nWallSeg; i++)
    {
      h3 = std::abs(wallSegLength[i])*tan(wallSegDelta*M_PI/180.0);
      h2 = h1+h3;
      const double s1 = h1*std::abs(wallSegLength[i]);
      const double s2 = wallSegLength[i]*h3/2.0;
      s += s1+s2;

      if (i<nWallSeg-1)
	h1 = wallSegLength[i+1]>0.0 ? h2+wallSegThick : h2-wallSegThick;
    }

  return s;
}

double
BilbaoWheelCassette::getSegWallThick() const
  /*!
    Calculate averate wall thickness for simplified (non-detailed)
    wall geometry. Used when bricksActive is false.
    Just calculates the thickness of a rectangular wall in order to
    have the same area as the segmented one.
   */
{
  ELog::RegMethod RegA("BilbaoWheelCassette","getSegWallThick");

  // Total wall length is sum of all its segments:
  double L = std::accumulate(std::next(wallSegLength.begin()), wallSegLength.end(),
			      std::abs(wallSegLength[0]),
			      [](double b, double c){
			       return std::abs(b)+std::abs(c);
			      });

  return getSegWallArea()/L;
}

void
BilbaoWheelCassette::buildBricks()
  /*!
    Build the bricks for the given segment
   */
{
  ELog::RegMethod RegA("BilbaoWheelCassette","buildBricks");
  return;
}

void
BilbaoWheelCassette::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("BilbaoWheelCassette","populate");

  FixedOffset::populate(Control);


  engActive=Control.EvalPair<int>(keyName,"","EngineeringActive");
  bricksActive=Control.EvalDefPair<int>(keyName,commonName,"BricksActive", 0);

  const double nSectors = Control.EvalVar<double>(baseName+"NSectors");
  delta = 360.0/nSectors;

  wallThick=Control.EvalPair<double>(keyName,commonName,"WallThick");
  wallThick /= 2.0; // there is half wall from each side of neighbouring sectors
  wallMat=ModelSupport::EvalMat<int>(Control,commonName+"WallMat",
				     keyName+"WallMat");
  heMat=ModelSupport::EvalMat<int>(Control,baseName+"HeMat");
  mainMat=ModelSupport::EvalMat<int>(Control,baseName+"WMat");
  temp=Control.EvalVar<double>(baseName+"Temp");

  // for detailed wall geometry (if bricksActive=true)
  nWallSeg=Control.EvalPair<size_t>(keyName,commonName,"NWallSeg");
  for (size_t i=0; i<nWallSeg; i++)
    {
      const double wl=Control.EvalPair<double>(keyName,commonName,
					       "WallSegLength"+std::to_string(i));
      wallSegLength.push_back(wl);
    }
  wallSegDelta=delta/2.0; // otherwise wall planes near bricks are not parallel
  wallSegThick=Control.EvalPair<double>(keyName,commonName,"WallSegThick");

  brickWidth=Control.EvalPair<double>(keyName,commonName,"BrickWidth");
  brickLength=Control.EvalPair<double>(keyName,commonName,"BrickLength");
  brickGap=Control.EvalPair<double>(keyName,commonName,"BrickGap");
  brickSteelMat=ModelSupport::EvalMat<int>(Control,commonName+"BrickSteelMat",
					   keyName+"BrickSteelMat");
  brickWMat=ModelSupport::EvalMat<int>(Control,commonName+"BrickWMat",
				       keyName+"BrickWMat");

  pipeCellThick=Control.EvalPair<double>(keyName,commonName,"PipeCellThick");
  pipeCellMat=ModelSupport::EvalMat<int>(Control,commonName+"PipeCellMat",
					 keyName+"PipeCellMat");

  return;
}

void
BilbaoWheelCassette::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: object for origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("BilbaoWheelCassette","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
BilbaoWheelCassette::createSurfaces(const attachSystem::FixedComp& FC)
  /*!
    Create All the surfaces
    \param FC :: wheel object
  */
{
  ELog::RegMethod RegA("BilbaoWheelCassette","createSurfaces");

  // divider
  ModelSupport::buildPlane(SMap,surfIndex+1,Origin,Y);
  // outer sides
  ModelSupport::buildPlaneRotAxis(SMap,surfIndex+3,Origin,X,Z,-delta/2.0);
  ModelSupport::buildPlaneRotAxis(SMap,surfIndex+4,Origin,X,Z,delta/2.0);

  const double dw = getSegWallThick();
  ModelSupport::buildPlaneRotAxis(SMap,surfIndex+13,Origin+X*(wallThick+dw),X,Z,-delta/2.0);
  ModelSupport::buildPlaneRotAxis(SMap,surfIndex+14,Origin-X*(wallThick+dw),X,Z,delta/2.0);

  // front plane
  double d = FC.getLinkDistance(back, front);

  Geometry::Cylinder *backCyl =
    SMap.realPtr<Geometry::Cylinder>(FC.getLinkSurf(back));
  const double R(backCyl->getRadius());

  // bircks start from this cylinder:
  ModelSupport::buildCylinder(SMap, surfIndex+7, Origin, Z, R+wallSegLength[0]);

  // d *= cos(delta*M_PI/180.0); //< distance from backCyl to the front plane
  Geometry::Vec3D offset = Origin-Y*(R+d);
  ModelSupport::buildPlane(SMap,surfIndex+11,offset,Y);

  offset += Y*pipeCellThick;
  ModelSupport::buildPlane(SMap,surfIndex+12,offset,Y);

  return;
}

void
BilbaoWheelCassette::createSurfacesBricks(const attachSystem::FixedComp& FC)
  /*!
    Create All the surfaces with bricks
    \param FC :: wheel object
  */
{
  ELog::RegMethod RegA("BilbaoWheelCassette","createSurfacesBricks");

  // divider
  ModelSupport::buildPlane(SMap,surfIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,surfIndex+5,Origin,Z);
  // outer sides
  ModelSupport::buildPlaneRotAxis(SMap,surfIndex+3,Origin,X,Z,-delta/2.0);
  ModelSupport::buildPlaneRotAxis(SMap,surfIndex+4,Origin,X,Z,delta/2.0);

  int SI(surfIndex+100);

  // detailed wall
  Geometry::Cylinder *backCyl =
    SMap.realPtr<Geometry::Cylinder>(FC.getLinkSurf(back));

  const double d2(M_PI*delta/180.0/2.0);
  double R(backCyl->getRadius());

  int SJ(SI);
  const double dx = R*sin(d2)-wallSegThick-wallThick;
  Geometry::Vec3D orig13=Origin-Y*(R*cos(d2)) - X*dx;
  Geometry::Vec3D orig14=Origin-Y*(R*cos(d2)) + X*dx;
  for (size_t j=0; j<nWallSeg; j++)
    {
      R += std::abs(wallSegLength[j])*cos(wallSegDelta*M_PI/180.0);
      Geometry::Vec3D offset = Origin-Y*(R);
      ModelSupport::buildPlane(SMap,SJ+11,offset,Y);

      const short dir = wallSegLength[j]>0.0 ? -1 : 1; // groove direction

      if (j>0)
	{
	  orig13 = SurInter::getPoint(SMap.realSurfPtr(SJ-1000+11),
				      SMap.realSurfPtr(SJ-1000+13),
				      SMap.realSurfPtr(surfIndex+5)) - X*wallSegThick*dir;
	  orig14 = SurInter::getPoint(SMap.realSurfPtr(SJ-1000+11),
				      SMap.realSurfPtr(SJ-1000+14),
				      SMap.realSurfPtr(surfIndex+5)) + X*wallSegThick*dir;
	}

      ModelSupport::buildPlaneRotAxis(SMap,SJ+13,
				      orig13,X,Z,
				      delta/2.0-wallSegDelta);
      ModelSupport::buildPlaneRotAxis(SMap,SJ+14,
				      orig14,X,Z,
				      wallSegDelta-delta/2.0);
      if (j==0)
	{
	  nBricks.push_back(0); // no bricks in the innermost segment
	}
      else // build the bricks
	{
	  // n = number of bircks in the given wall segment
	  // L = n*brickWidth + (n-1)*brickGap =>
	  // n = (L+brickGap) / (brickWidth+brickGap)
	  const double L(orig13.Distance(orig14));
	  const size_t n = static_cast<size_t>((L+brickGap)/(brickWidth+brickGap)+0.5+1);
	  nBricks.push_back(n);

	  int SBricks(SJ+100);
	  double bOffset(brickWidth);
	  ELog::EM << j << "\t" << n << " dist: " << L << ELog::endDiag;
	  for (size_t i=0; i<n; i++) // bricks
	    {
	      ModelSupport::buildShiftedPlane(SMap,SBricks+3,
					      SMap.realPtr<Geometry::Plane>(SJ+13),
					      bOffset);
	      ModelSupport::buildShiftedPlane(SMap,SBricks+4,
					      SMap.realPtr<Geometry::Plane>(SBricks+3),
					      brickGap);
	      SBricks += 10;
	      bOffset += brickWidth;
	    }
	}

      SJ += 1000;
    }

  return;
}

void
BilbaoWheelCassette::createObjects(Simulation& System,
				   const attachSystem::FixedComp& FC)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
    \param FC :: Tungsten layer rule
  */
{
  ELog::RegMethod RegA("BilbaoWheelCassette","createObjects");

  const std::string tb = FC.getLinkString(floor) + FC.getLinkString(roof);
  const std::string outer = tb + FC.getLinkString(back) + FC.getLinkString(front);

  std::string Out;
  Out=ModelSupport::getComposite(SMap,surfIndex," 3 -13 -1");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,temp,Out+outer));

  Out=ModelSupport::getComposite(SMap,surfIndex," 13 -14 12 7 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,mainMat,temp,Out+tb));

  Out=ModelSupport::getComposite(SMap,surfIndex," 13 -14 -7 ") + FC.getLinkString(back);
  System.addCell(MonteCarlo::Qhull(cellIndex++,heMat,temp,Out+tb));

  Out=ModelSupport::getComposite(SMap,surfIndex," 13 -14 -12 ") + FC.getLinkString(front);
  System.addCell(MonteCarlo::Qhull(cellIndex++,pipeCellMat,temp,Out+tb));

  Out=ModelSupport::getComposite(SMap,surfIndex," 14 -4 -1 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,temp,Out+outer));

  Out=ModelSupport::getComposite(SMap,surfIndex," 3 -4 -1 ");
  addOuterSurf(Out+outer);

  return;
}

void
BilbaoWheelCassette::createObjectsBricks(Simulation& System,
				   const attachSystem::FixedComp& FC)
  /*!
    Adds the all the components with bricks
    \param System :: Simulation to create objects in
    \param FC :: Tungsten layer rule
  */
{
  ELog::RegMethod RegA("BilbaoWheelCassette","createObjectsBricks");

  const std::string tb = FC.getLinkString(floor) + FC.getLinkString(roof);

  std::string Out;
  int SI(surfIndex+100);
  int SJ(SI);

  for (size_t j=0; j<nWallSeg; j++)
    {

      std::string Out1; // back-front surfaces
      if (j==0)
	Out1 = ModelSupport::getComposite(SMap,surfIndex,SJ," 11M -1 ") +
	  FC.getLinkString(back);
      else
	Out1 = ModelSupport::getComposite(SMap,SJ,SJ-1000," 11 -11M ");

      ///
      Out=ModelSupport::getComposite(SMap,surfIndex,SJ," 3 -13M ") + Out1;
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,temp,Out+tb));

      Out=ModelSupport::getComposite(SMap,surfIndex,SJ," 14M -4 ") + Out1;
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,temp,Out+tb));

      if (j==0)
	{
	  Out=ModelSupport::getComposite(SMap,SJ," 13 -14 ") + Out1;
	  //	  System.addCell(MonteCarlo::Qhull(cellIndex++,j==0?heMat:mainMat,temp,Out+tb));
	  System.addCell(MonteCarlo::Qhull(cellIndex++,heMat,temp,Out+tb));
	}
      else
	{
	  int SBricks(SJ+100);
	  std::string prev = ModelSupport::getComposite(SMap,SJ," 13 ");
	  for (size_t i=0; i<nBricks[j]; i++) // create brick cells
	    {
	      Out=ModelSupport::getComposite(SMap,SBricks," -3 ") + prev;
	      System.addCell(MonteCarlo::Qhull(cellIndex++,brickWMat,temp,Out+Out1+tb));

	      Out=ModelSupport::getComposite(SMap,SBricks," 3 -4 ");
	      System.addCell(MonteCarlo::Qhull(cellIndex++,heMat,temp,Out+Out1+tb));

	      prev = ModelSupport::getComposite(SMap,SBricks," 4 ");

	      SBricks += 10;
	    }
	  Out=ModelSupport::getComposite(SMap,SBricks-10,SJ," 4 -14M ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,heMat,temp,Out+Out1+tb));
	}
      SJ += 1000;
    }

  const std::string Out1 = FC.getLinkString(back) + FC.getLinkString(front);

  // Part from the left (remove)
  Out=ModelSupport::getComposite(SMap,surfIndex,SJ-1000," 3 -4 -11M ") +
    FC.getLinkString(front);
  System.addCell(MonteCarlo::Qhull(cellIndex++,heMat,temp,Out+tb));

  const std::string outer = tb + FC.getLinkString(front) + FC.getLinkString(back);

  Out=ModelSupport::getComposite(SMap,surfIndex," 3 -4 -1 ");
  addOuterSurf(Out+outer);

  return;
}


void
BilbaoWheelCassette::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("BilbaoWheelCassette","createLinks");

  if (bricksActive)
    {
      int SJ(surfIndex+100);
      size_t i(0);
      for (size_t j=0; j<nWallSeg; j++)
	{
	  Geometry::Vec3D p = SurInter::getPoint(SMap.realSurfPtr(SJ+11),
						 SMap.realSurfPtr(SJ+13),
						 SMap.realSurfPtr(surfIndex+5));
	  p += Y*std::abs(wallSegLength[j]/2.0);
	  FixedComp::setConnect(i,p,X);
	  FixedComp::setLinkSurf(i,SMap.realSurf(SJ+13));

	  i++;

	  p = SurInter::getPoint(SMap.realSurfPtr(SJ+11),
				 SMap.realSurfPtr(SJ+14),
				 SMap.realSurfPtr(surfIndex+5));
	  p += Y*std::abs(wallSegLength[j]/2.0);
	  FixedComp::setConnect(i,p,-X);
	  FixedComp::setLinkSurf(i,-SMap.realSurf(SJ+14));

	  ELog::EM << "LP " << j << ": " << p << ELog::endDiag;

	  SJ += 1000;
	  i++;
	}
    }

  return;
}




void
BilbaoWheelCassette::createAll(Simulation& System,
			       const attachSystem::FixedComp& FC,
			       const long int sideIndex,
			       const long int lpFloor,
			       const long int lpRoof,
			       const long int lpBack,
			       const long int lpFront,
			       const double& theta)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
    \param outer :: Tungsten layer rule
    \param angle :: angular coordinate of the given sector
  */
{
  ELog::RegMethod RegA("BilbaoWheelCassette","createAll");

  floor = lpFloor;
  roof = lpRoof;
  back = lpBack;
  front = lpFront;

  populate(System.getDataBase());
  xyAngle=theta;
  createUnitVector(FC,sideIndex);
  if (!bricksActive)
    {
      createSurfaces(FC);
      createObjects(System,FC);
    } else
    {
      createSurfacesBricks(FC);
      createObjectsBricks(System,FC);
    }
  createLinks();
  insertObjects(System);

  return;
}

}  // essSystem
