/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1Build/t1WaterModerator.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include <numeric>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

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
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Tally.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
#include "surfDIter.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "KGroup.h"
#include "Source.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "LinearComp.h"
#include "ContainedComp.h"
#include "t1Reflector.h"
#include "t1WaterModerator.h"

namespace ts1System
{

t1WaterModerator::t1WaterModerator(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,0),
  waterIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(waterIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

t1WaterModerator::t1WaterModerator(const t1WaterModerator& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  waterIndex(A.waterIndex),cellIndex(A.cellIndex),
  xStep(A.xStep),yStep(A.yStep),zStep(A.zStep),
  xyAngle(A.xyAngle),xOutSize(A.xOutSize),yOutSize(A.yOutSize),
  zOutSize(A.zOutSize),alThickOut(A.alThickOut),alMat(A.alMat),waterMat(A.waterMat)
  /*!
    Copy constructor
    \param A :: t1WaterModerator to copy
  */
{}

t1WaterModerator&
t1WaterModerator::operator=(const t1WaterModerator& A)
  /*!
    Assignment operator
    \param A :: t1WaterModerator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      xyAngle=A.xyAngle;
      xOutSize=A.xOutSize;
      yOutSize=A.yOutSize;
      zOutSize=A.zOutSize;
      alThickOut=A.alThickOut;
      alMat=A.alMat;
      waterMat=A.waterMat;
    }
  return *this;
}

t1WaterModerator::~t1WaterModerator() 
 /*!
   Destructor
 */
{}

void
t1WaterModerator::populate(const Simulation& System)
 /*!
   Populate all the variables
   \param System :: Simulation to use
 */
{
  ELog::RegMethod RegA("t1WaterModerator","populate");
  
  const FuncDataBase& Control=System.getDataBase();

  // Master values
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYangle");

  xOutSize=Control.EvalVar<double>(keyName+"XOutSize");
  yOutSize=Control.EvalVar<double>(keyName+"YOutSize");
  zOutSize=Control.EvalVar<double>(keyName+"ZOutSize");

  alThickOut=Control.EvalVar<double>(keyName+"AlThickOut");

  // Materials
  alMat=ModelSupport::EvalMat<int>(Control,keyName+"AlMat");
  waterMat=ModelSupport::EvalMat<int>(Control,keyName+"WaterMat");


  return;
}
  
void
t1WaterModerator::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    - Y Down the beamline
    \param FC :: Linked object
  */
{
  ELog::RegMethod RegA("t1WaterModerator","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC);

  const masterRotate& MR=masterRotate::Instance();
   
  Origin+=X*xStep+Y*yStep+Z*zStep;
  ELog::EM<<"Origin == "<<MR.calcRotate(Origin)<<ELog::endDebug;
  const Geometry::Quaternion Qxy=
    Geometry::Quaternion::calcQRotDeg(xyAngle,Z);
  Qxy.rotate(Y);
  Qxy.rotate(X);
  return;
}

void
t1WaterModerator::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("t1WaterModerator","createSurface");

  // Top layer out:
  ModelSupport::buildPlane(SMap,waterIndex+1,Origin-Y*yOutSize/2.0,Y);
  ModelSupport::buildPlane(SMap,waterIndex+2,Origin+Y*yOutSize/2.0,Y);
  ModelSupport::buildPlane(SMap,waterIndex+3,Origin-X*xOutSize/2.0,X);
  ModelSupport::buildPlane(SMap,waterIndex+4,Origin+X*xOutSize/2.0,X);
  ModelSupport::buildPlane(SMap,waterIndex+5,Origin-Z*zOutSize/2.0,Z);
  ModelSupport::buildPlane(SMap,waterIndex+6,Origin+Z*zOutSize/2.0,Z);
  // Top layer in:
  ModelSupport::buildPlane(SMap,waterIndex+11,Origin-
                                 Y*(yOutSize/2.0-alThickOut),Y);
  ModelSupport::buildPlane(SMap,waterIndex+12,Origin+
                                 Y*(yOutSize/2.0-alThickOut),Y);
  ModelSupport::buildPlane(SMap,waterIndex+13,Origin-
                                 X*(xOutSize/2.0-alThickOut),X);
  ModelSupport::buildPlane(SMap,waterIndex+14,Origin+
                                 X*(xOutSize/2.0-alThickOut),X);
  ModelSupport::buildPlane(SMap,waterIndex+15,Origin-
                                 Z*(zOutSize/2.0-alThickOut),Z);
  ModelSupport::buildPlane(SMap,waterIndex+16,Origin+
                                 Z*(zOutSize/2.0-alThickOut),Z);

  return;
}

void
t1WaterModerator::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("t1WaterModerator","createObjects");
  
  std::string Out;

  Out=ModelSupport::getComposite(SMap,waterIndex,"1 -2 3 -4 5 -6 ");
  addOuterSurf(Out);

  // Al 
  Out=ModelSupport::getComposite(SMap,waterIndex,
	"1 -2 3 -4 5 -6 (-11:12:-13:14:-15:16) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,0.0,Out));
  // water 
  Out=ModelSupport::getComposite(SMap,waterIndex,
	"11 -12 13 -14 15 -16 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,0.0,Out));


  return;
}

void
t1WaterModerator::createLinks()
  /*!
    Creates a full attachment set
  */
{
  // // set Links:
  // FixedComp::setConnect(0,BVec[0]+Y*(vacPosGap+alPos+terPos+
  // 				     outPos+clearNeg),Y);
  // FixedComp::setConnect(1,BVec[1]-Y*(vacNegGap+alPos+terNeg+
  // 				     outNeg+clearNeg),-Y);
  // FixedComp::setConnect(2,BVec[2]-
  // 			X*(vacSide+alSide+terSide+outSide+clearSide),-X);
  // FixedComp::setConnect(3,BVec[3]+
  // 			X*(vacSide+alSide+terSide+outSide+clearSide),X);
  // FixedComp::setConnect(4,BVec[4]-
  // 			Z*(vacBase+alBase+terBase+outBase+clearBase),-Z);
  // FixedComp::setConnect(5,BVec[5]+
  // 			Z*(vacTop+alTop+terTop+outTop+clearTop),Z);

  // // Set Connect surfaces:
  // for(int i=2;i<6;i++)
  //   FixedComp::setLinkSurf(i,SMap.realSurf(vacIndex+41+i));

  // // For Cylindrical surface must also have a divider:
  // // -- Groove:
  // FixedComp::setLinkSurf(0,SMap.realSurf(vacIndex+41));
  // FixedComp::addLinkSurf(0,SMap.realSurf(divideSurf));

  // FixedComp::setLinkSurf(1,SMap.realSurf(vacIndex+42));
  // FixedComp::addLinkSurf(1,-SMap.realSurf(divideSurf));

  return;
}

void
t1WaterModerator::createAll(Simulation& System,
			  const attachSystem::FixedComp& FC)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: Fixed Component to place object within
  */
{
  ELog::RegMethod RegA("t1WaterModerator","createAll");
  populate(System);

  createUnitVector(FC);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE ts1System
