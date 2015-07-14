/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/VacTank.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "VacTank.h"

namespace zoomSystem
{

VacTank::VacTank(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,6),
  tankIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(tankIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}


VacTank::~VacTank() 
 /*!
   Destructor
 */
{}

void
VacTank::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: Database of variable
 */
{
  ELog::RegMethod RegA("VacTank","populate");
  
  FixedOffset::populate(Control);


  radius=Control.EvalVar<double>(keyName+"Radius");
  length=Control.EvalVar<double>(keyName+"Length");
  sideThick=Control.EvalVar<double>(keyName+"SideThick");
  backThick=Control.EvalVar<double>(keyName+"BackThick");
  frontThick=Control.EvalVar<double>(keyName+"FrontThick");


  windowThick=Control.EvalVar<double>(keyName+"WindowThick");
  windowRadius=Control.EvalVar<double>(keyName+"WindowRadius");
  windowLength=Control.EvalVar<double>(keyName+"WindowLength");
  
  // Material
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  windowMat=ModelSupport::EvalMat<int>(Control,keyName+"WindowMat");

  return;
}
  
void
VacTank::createUnitVector(const attachSystem::FixedComp& FC,
			  const long int sideIndex)
  /*!
    Create the unit vectors
    - Y Down the beamline
    \param FC :: Linked object
    \param sideIndex :: sinde track
  */
{
  ELog::RegMethod RegA("VacTank","createUnitVector");
  FixedComp::createUnitVector(FC,orgIndex);

  applyOffset();
  return;
}
  
void
VacTank::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("VacTank","createSurface");

  // Main cylinder tank
  ModelSupport::buildPlane(SMap,tankIndex+1,
			   Origin+Y*windowInsertLen,Y);
  ModelSupport::buildPlane(SMap,tankIndex+2,
			   Origin+Y*(windowInsertLen+length),Y);
  ModelSupport::buildPlane(SMap,tankIndex+7,
			   Origin,Y,radius);

  ModelSupport::buildPlane(SMap,tankIndex+12,
			   Origin+Y*(windowInsertLen+length+backThick),Y);
  ModelSupport::buildPlane(SMap,tankIndex+17,
			   Origin,Y,radius+sideThick);

  // Nose cone:
  
  ModelSupport::buildPlane(SMap,tankIndex+101,Origin,Y);
  ModelSupport::buildPlane(SMap,tankIndex+102,Origin+Y*windowThick,Y);
  Geometry::Vec3D ConeOrigin=
  ModelSupport::buildPlane(SMap,tankIndex+12,
			   Origin+Y*(cylTotalDepth+length-wallThick),Y);
  ModelSupport::buildPlane(SMap,tankIndex+13,
			   Origin-X*(width/2.0-wallThick),X);
  ModelSupport::buildPlane(SMap,tankIndex+14,
			   Origin+X*(width/2.0-wallThick),X);
  ModelSupport::buildPlane(SMap,tankIndex+15,
			   Origin-Z*(height/2.0-wallThick),Z);
  ModelSupport::buildPlane(SMap,tankIndex+16,
			   Origin+Z*(height/2.0-wallThick),Z);


  double depthSum=0.0;
  for(size_t i=0;i<nCylinder;i++)
    {
      const int iN(static_cast<int>(i));
      // Plates across
      ModelSupport::buildPlane(SMap,tankIndex+111+iN,
			       Origin+Y*depthSum,Y);
      ModelSupport::buildPlane(SMap,tankIndex+121+iN,
			       Origin+Y*(depthSum+wallThick),Y);
      depthSum+=CylDepth[i];
      // Radii [Outer]
      ModelSupport::buildCylinder(SMap,tankIndex+171+iN,
				  Origin+X*CylX[i]+Z*CylZ[i],
				  Y,CRadius[i]);
      // Radii [Inner]
      ModelSupport::buildCylinder(SMap,tankIndex+181+iN,
				  Origin+X*CylX[i]+Z*CylZ[i],
				  Y,CRadius[i]-wallThick);
    }      

  // Window stuff
  ModelSupport::buildCylinder(SMap,tankIndex+17,
			      Origin+X*CylX[0]+Z*CylZ[0],
			      Y,windowRadius);
  ModelSupport::buildPlane(SMap,tankIndex+101,
			   Origin+Y*(wallThick-windowThick),Y);
  
  return;
}

void
VacTank::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("VacTank","createObjects");
  
  std::string Out;
  Out=ModelSupport::getComposite(SMap,tankIndex,
				 "111 -2 3 -4 5 -6 (1:-173)");
  addOuterSurf(Out);

  // Dead Volume
  Out=ModelSupport::getComposite(SMap,tankIndex,"-173 171 111 -113 (-112:172)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  
  // Inner Volume
  Out=ModelSupport::getComposite(SMap,tankIndex,"11 -12 13 -14 15 -16 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  
  // first Cylinder:
  Out=ModelSupport::getComposite(SMap,tankIndex,"-181 121 -122");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  // skipping front window
  Out=ModelSupport::getComposite(SMap,tankIndex,"-171 181 121 -122");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));

  // second Cylinder:
  Out=ModelSupport::getComposite(SMap,tankIndex,"-182 122 -123");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  Out=ModelSupport::getComposite(SMap,tankIndex,"-172 171 112 -123 (-122:182)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));

  // third Cylinder:
  Out=ModelSupport::getComposite(SMap,tankIndex,"-183 123 -11");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  Out=ModelSupport::getComposite(SMap,tankIndex,"-173 172 113 -11 (-123:183)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));

  // Main bulk tank:
  Out=ModelSupport::getComposite(SMap,tankIndex,"1 -2 3 -4 5 -6 "
				 "((-11 173):12:-13:14:-15:16)");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  

  // window
  //   support:
  Out=ModelSupport::getComposite(SMap,tankIndex,"-171 17 111 -121");
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out));
  //  Silicon window
  Out=ModelSupport::getComposite(SMap,tankIndex,"-17 101 -121");
  System.addCell(MonteCarlo::Qhull(cellIndex++,windowMat,0.0,Out));
  //  void
  Out=ModelSupport::getComposite(SMap,tankIndex,"-17 111 -101 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  
  return;
}

void
VacTank::createLinks()
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
VacTank::createAll(Simulation& System,const attachSystem::TwinComp& FC)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: Fixed Component to place object within
  */
{
  ELog::RegMethod RegA("VacTank","createAll");
  populate(System);

  createUnitVector(FC);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE zoomSystem
