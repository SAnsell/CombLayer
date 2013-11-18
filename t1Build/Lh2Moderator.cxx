/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1Build/Lh2Moderator.cxx
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
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "LinearComp.h"
#include "ContainedComp.h"
#include "t1Reflector.h"
#include "Lh2Moderator.h"

namespace ts1System
{

Lh2Moderator::Lh2Moderator(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,6),
  lh2Index(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(lh2Index+1),populated(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

Lh2Moderator::Lh2Moderator(const Lh2Moderator& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  lh2Index(A.lh2Index),
  cellIndex(A.cellIndex),populated(A.populated),
  xStep(A.xStep),yStep(A.yStep),zStep(A.zStep),
  xyAngle(A.xyAngle),xOutSize(A.xOutSize),yOutSize(A.yOutSize),
  zOutSize(A.zOutSize),alThickOut(A.alThickOut),alOffsetMid(A.alOffsetMid),
  alThickMid(A.alThickMid),alOffsetIn(A.alOffsetIn),
  alThickIn(A.alThickIn),alMat(A.alMat),lh2Mat(A.lh2Mat),voidMat(A.voidMat)
  /*!
    Copy constructor
    \param A :: Lh2Moderator to copy
  */
{}

Lh2Moderator&
Lh2Moderator::operator=(const Lh2Moderator& A)
  /*!
    Assignment operator
    \param A :: Lh2Moderator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      populated=A.populated;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      xyAngle=A.xyAngle;
      xOutSize=A.xOutSize;
      yOutSize=A.yOutSize;
      zOutSize=A.zOutSize;
      alThickOut=A.alThickOut;
      alOffsetMid=A.alOffsetMid;
      alThickMid=A.alThickMid;
      alOffsetIn=A.alOffsetIn;
      alThickIn=A.alThickIn;
      alMat=A.alMat;
      lh2Mat=A.lh2Mat;
      voidMat=A.voidMat;
    }
  return *this;
}

Lh2Moderator::~Lh2Moderator() 
 /*!
   Destructor
 */
{}

void
Lh2Moderator::populate(const Simulation& System)
 /*!
   Populate all the variables
   \param System :: Simulation to use
 */
{
  ELog::RegMethod RegA("Lh2Moderator","populate");
  
  const FuncDataBase& Control=System.getDataBase();

  // Master values
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYAngle");

  xOutSize=Control.EvalVar<double>(keyName+"XOutSize");
  yOutSize=Control.EvalVar<double>(keyName+"YOutSize");
  zOutSize=Control.EvalVar<double>(keyName+"ZOutSize");

  alThickOut=Control.EvalVar<double>(keyName+"AlThickOut");
  alOffsetMid=Control.EvalVar<double>(keyName+"AlOffsetMid");
  alThickMid=Control.EvalVar<double>(keyName+"AlThickMid");
  alOffsetIn=Control.EvalVar<double>(keyName+"AlOffsetIn");
  alThickIn=Control.EvalVar<double>(keyName+"AlThickIn");

  // Materials
  alMat=Control.EvalVar<int>(keyName+"AlMat");
  lh2Mat=Control.EvalVar<int>(keyName+"Lh2Mat");
  voidMat=Control.EvalVar<int>(keyName+"VoidMat");

  populated |= 1;
  return;
}
  
void
Lh2Moderator::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    - Y Down the beamline
    \param FC :: Linked object
  */
{
  ELog::RegMethod RegA("Lh2Moderator","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC);

  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,0);
  return;
}

void
Lh2Moderator::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("Lh2Moderator","createSurface");

  // Top layer out:
  ModelSupport::buildPlane(SMap,lh2Index+1,Origin-Y*yOutSize/2.0,Y);
  ModelSupport::buildPlane(SMap,lh2Index+2,Origin+Y*yOutSize/2.0,Y);
  ModelSupport::buildPlane(SMap,lh2Index+3,Origin-X*xOutSize/2.0,X);
  ModelSupport::buildPlane(SMap,lh2Index+4,Origin+X*xOutSize/2.0,X);
  ModelSupport::buildPlane(SMap,lh2Index+5,Origin-Z*zOutSize/2.0,Z);
  ModelSupport::buildPlane(SMap,lh2Index+6,Origin+Z*zOutSize/2.0,Z);
  // Top layer in:
  ModelSupport::buildPlane(SMap,lh2Index+11,Origin-
                                 Y*(yOutSize/2.0-alThickOut),Y);
  ModelSupport::buildPlane(SMap,lh2Index+12,Origin+
                                 Y*(yOutSize/2.0-alThickOut),Y);
  ModelSupport::buildPlane(SMap,lh2Index+13,Origin-
                                 X*(xOutSize/2.0-alThickOut),X);
  ModelSupport::buildPlane(SMap,lh2Index+14,Origin+
                                 X*(xOutSize/2.0-alThickOut),X);
  ModelSupport::buildPlane(SMap,lh2Index+15,Origin-
                                 Z*(zOutSize/2.0-alThickOut),Z);
  ModelSupport::buildPlane(SMap,lh2Index+16,Origin+
                                 Z*(zOutSize/2.0-alThickOut),Z);

  // Mid layer out:
  ModelSupport::buildPlane(SMap,lh2Index+21,Origin-
                                 Y*(yOutSize/2.0-alOffsetMid),Y);
  ModelSupport::buildPlane(SMap,lh2Index+22,Origin+
                                 Y*(yOutSize/2.0-alOffsetMid),Y);
  ModelSupport::buildPlane(SMap,lh2Index+23,Origin-
                                 X*(xOutSize/2.0-alOffsetMid),X);
  ModelSupport::buildPlane(SMap,lh2Index+24,Origin+
                                 X*(xOutSize/2.0-alOffsetMid),X);
  ModelSupport::buildPlane(SMap,lh2Index+25,Origin-
                                 Z*(zOutSize/2.0-alOffsetMid),Z);
  ModelSupport::buildPlane(SMap,lh2Index+26,Origin+
                                 Z*(zOutSize/2.0-alOffsetMid),Z);
  // Mid layer in:
  ModelSupport::buildPlane(SMap,lh2Index+31,Origin-
                      Y*(yOutSize/2.0-alOffsetMid-alThickMid),Y);
  ModelSupport::buildPlane(SMap,lh2Index+32,Origin+
                      Y*(yOutSize/2.0-alOffsetMid-alThickMid),Y);
  ModelSupport::buildPlane(SMap,lh2Index+33,Origin-
                      X*(xOutSize/2.0-alOffsetMid-alThickMid),X);
  ModelSupport::buildPlane(SMap,lh2Index+34,Origin+
                      X*(xOutSize/2.0-alOffsetMid-alThickMid),X);
  ModelSupport::buildPlane(SMap,lh2Index+35,Origin-
                      Z*(zOutSize/2.0-alOffsetMid-alThickMid),Z);
  ModelSupport::buildPlane(SMap,lh2Index+36,Origin+
                      Z*(zOutSize/2.0-alOffsetMid-alThickMid),Z);

  // Bottom layer out:
  ModelSupport::buildPlane(SMap,lh2Index+41,Origin-
                                 Y*(yOutSize/2.0-alOffsetIn),Y);
  ModelSupport::buildPlane(SMap,lh2Index+42,Origin+
                                 Y*(yOutSize/2.0-alOffsetIn),Y);
  ModelSupport::buildPlane(SMap,lh2Index+43,Origin-
                                 X*(xOutSize/2.0-alOffsetIn),X);
  ModelSupport::buildPlane(SMap,lh2Index+44,Origin+
                                 X*(xOutSize/2.0-alOffsetIn),X);
  ModelSupport::buildPlane(SMap,lh2Index+45,Origin-
                                 Z*(zOutSize/2.0-alOffsetIn),Z);
  ModelSupport::buildPlane(SMap,lh2Index+46,Origin+
                                 Z*(zOutSize/2.0-alOffsetIn),Z);
  // Bottom layer in:
  ModelSupport::buildPlane(SMap,lh2Index+51,Origin-
                      Y*(yOutSize/2.0-alOffsetIn-alThickIn),Y);
  ModelSupport::buildPlane(SMap,lh2Index+52,Origin+
                      Y*(yOutSize/2.0-alOffsetIn-alThickIn),Y);
  ModelSupport::buildPlane(SMap,lh2Index+53,Origin-
                      X*(xOutSize/2.0-alOffsetIn-alThickIn),X);
  ModelSupport::buildPlane(SMap,lh2Index+54,Origin+
                      X*(xOutSize/2.0-alOffsetIn-alThickIn),X);
  ModelSupport::buildPlane(SMap,lh2Index+55,Origin-
                      Z*(zOutSize/2.0-alOffsetIn-alThickIn),Z);
  ModelSupport::buildPlane(SMap,lh2Index+56,Origin+
                      Z*(zOutSize/2.0-alOffsetIn-alThickIn),Z);

  return;
}
void

// goran b
Lh2Moderator::addToInsertChain(attachSystem::ContainedComp& CC) const
  /*!
    Adds this object to the containedComp to be inserted.
    \param CC :: ContainedComp object to add to this
  */
 {
  for(int i=lh2Index+1;i<cellIndex;i++)
    CC.addInsertCell(i);
  return;
 }


void
// goran e

// Lh2Moderator::createObjects(Simulation& System)
Lh2Moderator::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("Lh2Moderator","createObjects");
  
  std::string Out;

  Out=ModelSupport::getComposite(SMap,lh2Index,"1 -2 3 -4 5 -6 ");
  addOuterSurf(Out);
  addBoundarySurf(Out);

  // Al 
  Out=ModelSupport::getComposite(SMap,lh2Index,
	"1 -2 3 -4 5 -6 (-11:12:-13:14:-15:16) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,0.0,Out));
  // void 
  Out=ModelSupport::getComposite(SMap,lh2Index,
	"11 -12 13 -14 15 -16 (-21:22:-23:24:-25:26) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,voidMat,0.0,Out));
  // Al 
  Out=ModelSupport::getComposite(SMap,lh2Index,
	"21 -22 23 -24 25 -26 (-31:32:-33:34:-35:36) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,0.0,Out));
  // void 
  Out=ModelSupport::getComposite(SMap,lh2Index,
	"31 -32 33 -34 35 -36 (-41:42:-43:44:-45:46) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,voidMat,0.0,Out));
  // Al 
  Out=ModelSupport::getComposite(SMap,lh2Index,
	"41 -42 43 -44 45 -46 (-51:52:-53:54:-55:56) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,0.0,Out));
  // LH2 
  Out=ModelSupport::getComposite(SMap,lh2Index,
	"51 -52 53 -54 55 -56 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,lh2Mat,0.0,Out));

  return;
}

 void

Lh2Moderator::createLinks()
  /*!
    Creates a full attachment set
  */
{
  // set Links:


  // Top layer out:

  FixedComp::setConnect(0,Origin+X*xOutSize/2.0,X);
  FixedComp::setConnect(1,Origin-X*xOutSize/2.0,-X);
  FixedComp::setConnect(2,Origin-Y*yOutSize/2.0,-Y);
  FixedComp::setConnect(3,Origin+Y*yOutSize/2.0,Y);
  FixedComp::setConnect(4,Origin-Z*zOutSize/2.0,-Z);
  FixedComp::setConnect(5,Origin+Z*zOutSize/2.0,Z);

  FixedComp::setLinkSurf(0,SMap.realSurf(lh2Index+4));
  FixedComp::setLinkSurf(1,-SMap.realSurf(lh2Index+3));
  FixedComp::setLinkSurf(2,-SMap.realSurf(lh2Index+1));
  FixedComp::setLinkSurf(3,SMap.realSurf(lh2Index+2));
  FixedComp::setLinkSurf(4,-SMap.realSurf(lh2Index+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(lh2Index+6));

  return;
}

// void

std::string
Lh2Moderator::getComposite(const std::string& surfList) const
  /*!
    Exposes local version of getComposite
    \param surfList :: surface list
    \return Composite string
  */
{
  return ModelSupport::getComposite(SMap,lh2Index,surfList);
}

void

Lh2Moderator::createAll(Simulation& System,
			const attachSystem::FixedComp& FC)
		      
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: Fixed Component to place object within
  */
{
  ELog::RegMethod RegA("Lh2Moderator","createAll");
  populate(System);

  createUnitVector(FC);
  createSurfaces();
//  createObjects(System);
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE ts1System
