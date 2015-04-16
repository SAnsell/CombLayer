/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   moderator/Hydrogen.cxx
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
#include "chipDataStore.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "LinearComp.h"
#include "ContainedComp.h"
#include "pipeUnit.h"
#include "PipeLine.h"
#include "Hydrogen.h"

namespace moderatorSystem
{

Hydrogen::Hydrogen(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,6),
  hydIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(hydIndex+1),populated(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

Hydrogen::Hydrogen(const Hydrogen& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  hydIndex(A.hydIndex),cellIndex(A.cellIndex),
  populated(A.populated),width(A.width),height(A.height),
  depth(A.depth),radius(A.radius),innerXShift(A.innerXShift),
  alDivide(A.alDivide),alFront(A.alFront),alTop(A.alTop),
  alBase(A.alBase),alSide(A.alSide),modTemp(A.modTemp),modMat(A.modMat),
  alMat(A.alMat),HCentre(A.HCentre)
  /*!
    Copy constructor
    \param A :: Hydrogen to copy
  */
{}

Hydrogen&
Hydrogen::operator=(const Hydrogen& A)
  /*!
    Assignment operator
    \param A :: Hydrogen to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      populated=A.populated;
      width=A.width;
      height=A.height;
      depth=A.depth;
      radius=A.radius;
      innerXShift=A.innerXShift;
      alDivide=A.alDivide;
      alFront=A.alFront;
      alTop=A.alTop;
      alBase=A.alBase;
      alSide=A.alSide;
      modTemp=A.modTemp;
      modMat=A.modMat;
      alMat=A.alMat;
      HCentre=A.HCentre;
    }
  return *this;
}

Hydrogen::~Hydrogen() 
  /*!
    Destructor
  */
{}

void
Hydrogen::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Hydrogen","populate");
  
  const FuncDataBase& Control=System.getDataBase();

  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  radius=Control.EvalVar<double>(keyName+"Radius");
  innerXShift=Control.EvalVar<double>(keyName+"InnerXShift");

  alDivide=Control.EvalVar<double>(keyName+"AlDivide"); 
  alFront=Control.EvalVar<double>(keyName+"AlFront"); 
  alTop=Control.EvalVar<double>(keyName+"AlTop"); 
  alBase=Control.EvalVar<double>(keyName+"AlBase"); 
  alSide=Control.EvalVar<double>(keyName+"AlSide"); 

  modTemp=Control.EvalVar<double>(keyName+"ModTemp");

  modMat=ModelSupport::EvalMat<int>(Control,keyName+"ModMat");
  alMat=ModelSupport::EvalMat<int>(Control,keyName+"AlMat");

  populated |= 1;
  return;
}
  

void
Hydrogen::createUnitVector(const attachSystem::FixedComp& CUnit,
			   const size_t sideIndex)
  /*!
    Create the unit vectors
    - Y Points down the Hydrogen direction
    - X Across the Hydrogen
    - Z up (towards the target)
    \param CUnit :: Fixed unit that it is connected to 
    \param sideIndex :: Index on fixed unit
  */
{
  ELog::RegMethod RegA("Hydrogen","createUnitVector");

  FixedComp::createUnitVector(CUnit);

  // Opposite since other face:
  const attachSystem::LinkUnit& LU=CUnit.getLU(sideIndex);

  Y= LU.getAxis();
  X= Y*Z;
  Origin=LU.getConnectPt();
  HCentre=Origin-Y*(radius-depth-alDivide)+X*innerXShift;

  return;
}
  
void
Hydrogen::createSurfaces(const attachSystem::LinkUnit& LU)
  /*!
    Create All the surfaces
    \param LU :: Linked unit to the hydrogen [groove]
  */
{
  ELog::RegMethod RegA("Hydrogen","createSurface");

  // set Links:
  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setConnect(1,HCentre+Y*(alFront+radius),Y); 
  FixedComp::setConnect(2,Origin-X*(alSide+width/2.0),-X);
  FixedComp::setConnect(3,Origin+X*(alSide+width/2.0),X);
  FixedComp::setConnect(4,Origin-Z*(height/2.0+alBase),-Z);
  FixedComp::setConnect(5,Origin+Z*(height/2.0+alTop),Z);


  SMap.addMatch(hydIndex+11,LU.getLinkSurf());
  // Hydrogen Layers
  ModelSupport::buildPlane(SMap,hydIndex+3,Origin-X*width/2.0,X);
  ModelSupport::buildPlane(SMap,hydIndex+4,Origin+X*width/2.0,X);
  ModelSupport::buildPlane(SMap,hydIndex+5,Origin-Z*height/2.0,Z);
  ModelSupport::buildPlane(SMap,hydIndex+6,Origin+Z*height/2.0,Z);

  // Inner divide Al 
  ModelSupport::buildPlane(SMap,hydIndex+1,Origin+Y*alDivide,Y);

  ModelSupport::buildPlane(SMap,hydIndex+13,Origin-X*(alSide+width/2.0),X);
  ModelSupport::buildPlane(SMap,hydIndex+14,Origin+X*(alSide+width/2.0),X);
  ModelSupport::buildPlane(SMap,hydIndex+15,Origin-Z*(alTop+height/2.0),Z);
  ModelSupport::buildPlane(SMap,hydIndex+16,Origin+Z*(alBase+height/2.0),Z);

  ModelSupport::buildCylinder(SMap,hydIndex+2,HCentre,Z,radius);
  ModelSupport::buildCylinder(SMap,hydIndex+12,HCentre+Y*alFront,Z,radius);
  
  int signVal(-1);
  for(int i=0;i<6;i++)
    {
      FixedComp::setLinkSurf(static_cast<size_t>(i),
			     signVal*SMap.realSurf(hydIndex+i+1));
      signVal*=-1;
    }
  // Set divide surface
  FixedComp::addLinkSurf(1,-SMap.realSurf(hydIndex+1));
  return;
}

void
Hydrogen::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("Hydrogen","createObjects");
  
  std::string Out;
  Out=ModelSupport::getComposite(SMap,hydIndex,"11 -12 13 -14 15 -16");
  addOuterSurf(Out);

  Out=ModelSupport::getComposite(SMap,hydIndex,"1 -2 3 -4 5 -6");
  addBoundarySurf(Out);
  System.addCell(MonteCarlo::Qhull(cellIndex++,modMat,modTemp,Out));
  HCell=cellIndex-1;
  // Al layers :
  Out=ModelSupport::getComposite(SMap,hydIndex,"11 -12 13 -14 15 -16 "
				 " (-1 : 2 : -3 : 4 : -5 : 6 ) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,modTemp,Out));
  
  return;
}

int
Hydrogen::getDividePlane() const
  /*!
    Get the dividing plane
    \return Dividing plane [pointing out]
  */
{
  return SMap.realSurf(hydIndex+1);
}

int
Hydrogen::viewSurf() const
  /*!
    View surface 
    \return view surface [pointing out]
   */
{
  return SMap.realSurf(hydIndex+2);
}
  
void
Hydrogen::createAll(Simulation& System,
		    const attachSystem::FixedComp& FUnit,
		    const size_t sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation to create objects in
    \param FUnit :: Fixed Base unit
    \param sideIndex :: Side to look at
  */
{
  ELog::RegMethod RegA("Hydrogen","createAll");
  populate(System);

  createUnitVector(FUnit,sideIndex);
  createSurfaces(FUnit.getLU(sideIndex));
  createObjects(System);
  insertObjects(System);       
  
  return;
}
  
}  // NAMESPACE moderatorSystem
