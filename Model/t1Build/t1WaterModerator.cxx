/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Build/t1WaterModerator.cxx
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
#include <numeric>
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
#include "localRotate.h"
#include "masterRotate.h"
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
#include "t1Reflector.h"
#include "t1WaterModerator.h"

namespace ts1System
{

t1WaterModerator::t1WaterModerator(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedOffset(Key,0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

t1WaterModerator::t1WaterModerator(const t1WaterModerator& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  xOutSize(A.xOutSize),yOutSize(A.yOutSize),
  zOutSize(A.zOutSize),alThickOut(A.alThickOut),
  alMat(A.alMat),waterMat(A.waterMat)
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
      attachSystem::FixedOffset::operator=(A);
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
t1WaterModerator::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param System :: Simulation to use
 */
{
  ELog::RegMethod RegA("t1WaterModerator","populate");

  FixedOffset::populate(Control);

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
t1WaterModerator::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("t1WaterModerator","createSurface");

  // Top layer out:
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*yOutSize/2.0,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*yOutSize/2.0,Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*xOutSize/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*xOutSize/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*zOutSize/2.0,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*zOutSize/2.0,Z);
  // Top layer in:
  ModelSupport::buildPlane(SMap,buildIndex+11,Origin-
                                 Y*(yOutSize/2.0-alThickOut),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+
                                 Y*(yOutSize/2.0-alThickOut),Y);
  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-
                                 X*(xOutSize/2.0-alThickOut),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+
                                 X*(xOutSize/2.0-alThickOut),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-
                                 Z*(zOutSize/2.0-alThickOut),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+
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

  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 3 -4 5 -6 ");
  addOuterSurf(Out);

  // Al 
  Out=ModelSupport::getComposite(SMap,buildIndex,
	"1 -2 3 -4 5 -6 (-11:12:-13:14:-15:16) ");
  System.addCell(MonteCarlo::Object(cellIndex++,alMat,0.0,Out));
  // water 
  Out=ModelSupport::getComposite(SMap,buildIndex,
	"11 -12 13 -14 15 -16 ");
  System.addCell(MonteCarlo::Object(cellIndex++,waterMat,0.0,Out));


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
			    const attachSystem::FixedComp& FC,
			    const long int sideIndex)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: Fixed Component to place object within
  */
{
  ELog::RegMethod RegA("t1WaterModerator","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE ts1System
