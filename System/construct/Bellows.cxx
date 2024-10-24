/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   construct/Bellows.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#include <array>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "SurfMap.h"
#include "Exception.h"

#include "GeneralPipe.h"
#include "Bellows.h"

namespace constructSystem
{

Bellows::Bellows(const std::string& Key) :
  GeneralPipe(Key)
  /*!
    Constructor
    \param Key :: KeyName
  */
{}

Bellows::Bellows(const Bellows& A) :
  GeneralPipe(A),
  bellowThick(A.bellowThick),
  bellowStep(A.bellowStep),
  wallThick(A.wallThick),
  nFolds(A.nFolds),
  bellowMat(A.bellowMat)
  /*!
    Copy constructor
    \param A :: Bellows to copy
  */
{}

Bellows&
Bellows::operator=(const Bellows& A)
  /*!
    Assignment operator
    \param A :: Bellows to copy
    \return *this
  */
{
  if (this!=&A)
    {
      GeneralPipe::operator=(A);
      bellowThick=A.bellowThick;
      bellowStep=A.bellowStep;
      wallThick=A.wallThick;
      nFolds=A.nFolds;
      bellowMat=A.bellowMat;
    }
  return *this;
}

Bellows::~Bellows()
  /*!
    Destructor
  */
{}

double
Bellows::getBellowLength() const
/*!
  Return the accordeon structure length
 */
{
  return length-flangeA.thick-flangeB.thick-bellowStep*2.0;
}


double
Bellows::getBellowThick() const
/*!
  Return bellow thickness based on nFolds
 */
{
  const double L = getBellowLength();
  const double foldLength = L/nFolds;
  const double halfFold = foldLength/2.0;
  const double R = std::max(flangeA.radius, flangeB.radius); // bellow outer radius at max compression TODO: don't guess, make it a variable, but check outerVoid below
  const double r = radius; // bellow inner radius
  const double maxThick = R-r-pipeThick; // thickness at max compression
  ELog::EM << maxThick << " " << halfFold << ELog::endDiag;
  if (maxThick<halfFold+Geometry::zeroTol)
    throw ColErr::NumericalAbort("Bellows: impossible combination of R, length and nFolds. Try to increase nFolds.");
  return sqrt(maxThick*maxThick - halfFold*halfFold);
}

double
Bellows::getDensityFraction() const
/*!
  Return density fraction of the homogenised accordion structure with
  respect to the density of its wall (i.e. return 1 for a fully
  compressed bellow)
 */
{
  const double L = nFolds*2*wallThick; // max compressed length
  const double l = getBellowLength();
  return L/l;
}

void
Bellows::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("Bellows","populate");

  GeneralPipe::populate(Control);
  bellowStep=Control.EvalDefVar<double>(keyName+"BellowStep",0.0);
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  nFolds=Control.EvalVar<int>(keyName+"NFolds");
  bellowThick=getBellowThick();

  const double frac = getDensityFraction();
  bellowMat=ModelSupport::EvalDefMat(Control,keyName+"BellowMat",pipeMat);
  // bellowMat=ModelSupport::EvalMat<int>(Control,
  // 				  keyName+"BellowMat"+"%Void%"+std::to_string(frac));
  outerVoid=1;  // no options:
  return;
}

void
Bellows::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("Bellows","createSurfaces");


  GeneralPipe::createSurfaces();
  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,
			      Y,radius+pipeThick+bellowThick);


  FrontBackCut::getShiftedFront
    (SMap,buildIndex+121,Y,(flangeA.thick+bellowStep));


  FrontBackCut::getShiftedBack
    (SMap,buildIndex+221,Y,-(flangeB.thick+bellowStep));



  return;
}

void
Bellows::createObjects(Simulation& System)
  /*!
    Adds the vacuum pipe and surround
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("Bellows","createObjects");

  HeadRule HR;

  const HeadRule& frontHR=getRule("front");
  const HeadRule& backHR=getRule("back");

  // Void
  HR=HeadRule(SMap,buildIndex,-7);
  const HeadRule& voidFront =
    (flangeA.radius>flangeA.innerRadius+Geometry::zeroTol) ? HeadRule(SMap,buildIndex,101) : frontHR;
  const HeadRule& voidBack =
    (flangeB.radius>flangeB.innerRadius+Geometry::zeroTol) ? HeadRule(SMap,buildIndex,-201) : backHR;
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*voidFront*voidBack);

  GeneralPipe::createFlange(System,HR.complement());

  // Inner clip if present
  if (bellowStep>Geometry::zeroTol)
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -121 -17 7");
      makeCell("FrontClip",System,cellIndex++,pipeMat,0.0,HR);

      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-201 221 -17 7");
      makeCell("BackClip",System,cellIndex++,pipeMat,0.0,HR);

      HR=ModelSupport::getHeadRule(SMap,buildIndex,"121 -221 -27 7");
      makeCell("Bellow",System,cellIndex++,bellowMat,0.0,HR);

      HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -121 -27 17");
      makeCell("FrontSpaceVoid",System,cellIndex++,0,0.0,HR);

      HR=ModelSupport::getHeadRule(SMap,buildIndex,"-201 221 -27 17");
      makeCell("BackSpaceVoid",System,cellIndex++,0,0.0,HR);
    }
  else
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -221 -27 7");
      makeCell("Bellow",System,cellIndex++,bellowMat,0.0,HR);
    }

  HR=HeadRule(SMap,buildIndex,27);
  GeneralPipe::createOuterVoid(System,HR);
  return;
}

void
Bellows::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("Bellow","createLinks");

  // stuff for intersection

  FrontBackCut::createLinks(*this,Origin,Y);  //front and back
  FixedComp::setConnect(2,Origin-X*radius,-X);
  FixedComp::setConnect(3,Origin+X*radius,X);
  FixedComp::setConnect(4,Origin-Z*radius,-Z);
  FixedComp::setConnect(5,Origin+Z*radius,Z);
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(4,SMap.realSurf(buildIndex+7));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+7));

  FixedComp::setConnect(6,Origin-Z*(radius+bellowThick),-Z);
  FixedComp::setConnect(7,Origin+Z*(radius+bellowThick),Z);
  FixedComp::setLinkSurf(6,SMap.realSurf(buildIndex+27));
  FixedComp::setLinkSurf(7,SMap.realSurf(buildIndex+27));

  // pipe wall
  FixedComp::setConnect(8,Origin-Z*(radius+pipeThick),-Z);
  FixedComp::setConnect(9,Origin+Z*(radius+pipeThick),Z);
  FixedComp::setLinkSurf(8,SMap.realSurf(buildIndex+17));
  FixedComp::setLinkSurf(9,SMap.realSurf(buildIndex+17));


  return;
}

void
Bellows::createAll(Simulation& System,
		   const attachSystem::FixedComp& FC,
		   const long int FIndex)
 /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: FixedComp
    \param FIndex :: Fixed Index
  */
{
  ELog::RegMethod RegA("Bellows","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(FC,FIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE constructSystem
