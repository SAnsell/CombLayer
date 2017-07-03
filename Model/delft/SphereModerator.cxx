/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delft/SphereModerator.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "pipeUnit.h"
#include "PipeLine.h"
#include "virtualMod.h"
#include "SphereModerator.h"

namespace delftSystem
{

SphereModerator::SphereModerator(const std::string& Key)  :
  virtualMod(Key),
  hydIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(hydIndex+1),HCell(0),
  InnerA("SphereA"),InnerB("SphereB")
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

SphereModerator::SphereModerator(const SphereModerator& A) : 
  virtualMod(A),
  hydIndex(A.hydIndex),cellIndex(A.cellIndex),
  innerRadius(A.innerRadius),
  innerAl(A.innerAl),outerRadius(A.outerRadius),
  outerAl(A.outerAl),outYShift(A.outYShift),fYShift(A.fYShift),
  modTemp(A.modTemp),modMat(A.modMat),alMat(A.alMat),
  HCell(A.HCell),InnerA(A.InnerA),InnerB(A.InnerB)
  /*!
    Copy constructor
    \param A :: SphereModerator to copy
  */
{}

SphereModerator&
SphereModerator::operator=(const SphereModerator& A)
  /*!
    Assignment operator
    \param A :: SphereModerator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      virtualMod::operator=(A);
      cellIndex=A.cellIndex;
      innerRadius=A.innerRadius;
      innerAl=A.innerAl;
      outerRadius=A.outerRadius;
      outerAl=A.outerAl;
      outYShift=A.outYShift;
      fYShift=A.fYShift;
      modTemp=A.modTemp;
      modMat=A.modMat;
      alMat=A.alMat;
      HCell=A.HCell;
      InnerA=A.InnerA;
      InnerB=A.InnerB;
    }
  return *this;
}

SphereModerator*
SphereModerator::clone() const
  /*!
    Clone copy constructor
    \return new this
  */
{
  return new SphereModerator(*this); 
}

SphereModerator::~SphereModerator() 
  /*!
    Destructor
  */
{}

void
SphereModerator::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("SphereModerator","populate");

  FixedOffset::populate(Control);
  
  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");
  innerAl=Control.EvalVar<double>(keyName+"InnerAl");
  outerRadius=Control.EvalVar<double>(keyName+"OuterRadius");
  outerAl=Control.EvalVar<double>(keyName+"OuterAl");
  outYShift=Control.EvalVar<double>(keyName+"OutYShift");
  fYShift=Control.EvalVar<double>(keyName+"ForwardStep");
  capThick=Control.EvalVar<double>(keyName+"CapThick");

  pipeLen=Control.EvalVar<double>(keyName+"PipeLen");
  pipeRadius=Control.EvalVar<double>(keyName+"PipeRadius");
  pipeAlRadius=Control.EvalVar<double>(keyName+"PipeAlRadius");

  modTemp=Control.EvalVar<double>(keyName+"ModTemp");
  modMat=ModelSupport::EvalMat<int>(Control,keyName+"ModMat");
  alMat=ModelSupport::EvalMat<int>(Control,keyName+"AlMat");

  return;
}
  

void
SphereModerator::createUnitVector(const attachSystem::FixedComp& CUnit,
				  const long int sideIndex)
  /*!
    Create the unit vectors
    \param CUnit :: Fixed unit that it is connected to 
    \param sideIndex :: Link point
  */
{
  ELog::RegMethod RegA("SphereModerator","createUnitVector");

  FixedComp::createUnitVector(CUnit,sideIndex);
  applyOffset();

  return;
}
  
void
SphereModerator::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("SphereModerator","createSurfaces");

  ModelSupport::buildSphere(SMap,hydIndex+7,Origin,innerRadius-innerAl);
  ModelSupport::buildSphere(SMap,hydIndex+17,Origin,innerRadius);
  ModelSupport::buildSphere(SMap,hydIndex+27,Origin+Y*outYShift,
			    outerRadius);
  ModelSupport::buildSphere(SMap,hydIndex+37,Origin+Y*outYShift,
			    outerRadius+innerAl);

  ModelSupport::buildCylinder(SMap,hydIndex+107,Origin,
			      Y,innerRadius-innerAl);
  ModelSupport::buildCylinder(SMap,hydIndex+117,Origin,
			      Y,innerRadius);
  ModelSupport::buildCylinder(SMap,hydIndex+127,Origin,
			      Y,outerRadius);
  ModelSupport::buildCylinder(SMap,hydIndex+137,Origin,
			      Y,outerRadius+outerAl);

  // Front divide plane:
  ModelSupport::buildPlane(SMap,hydIndex+1,Origin,Y);

  // Front divide plane:
  ModelSupport::buildPlane(SMap,hydIndex+101,Origin+Y*fYShift,Y);
  // Front divide plane:
  ModelSupport::buildPlane(SMap,hydIndex+111,
			   Origin+Y*(fYShift+capThick),Y);
  
  return;
}

void
SphereModerator::createLinks()
  /*!
    Create links
  */
{
  ELog::RegMethod RegA("SphereModerator","createLinks");

  FixedComp::addLinkSurf(0,SMap.realSurf(hydIndex+37));
  FixedComp::addLinkSurf(1,-SMap.realSurf(hydIndex+111));
  FixedComp::addLinkSurf(2,SMap.realSurf(hydIndex+37));
  FixedComp::addLinkSurf(3,SMap.realSurf(hydIndex+37));
  FixedComp::addLinkSurf(4,SMap.realSurf(hydIndex+37));
  FixedComp::addLinkSurf(5,SMap.realSurf(hydIndex+37));

  // // set Links:
  FixedComp::setConnect(0,Origin+Y*(outYShift-outerRadius-innerAl),-Y);
  FixedComp::setConnect(1,Origin+Y*(fYShift+capThick),Y);

  FixedComp::setConnect(2,Origin-X*(outerRadius+innerAl),-X);
  FixedComp::setConnect(3,Origin+X*(outerRadius+innerAl),X);
  FixedComp::setConnect(4,Origin-Z*(outerRadius+innerAl),-Z);
  FixedComp::setConnect(5,Origin+Z*(outerRadius+innerAl),Z);

  return;
}
  
void
SphereModerator::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("SphereModerator","createObjects");

  std::string Out;  

  // ARRANGEMENT FOR POSITIVE Radius:
  Out=ModelSupport::getComposite(SMap,hydIndex," -37 -1 ");
  addOuterSurf(Out);
  Out=ModelSupport::getComposite(SMap,hydIndex,"1 -111 -137 ");
  addOuterUnionSurf(Out);

  // void
  Out=ModelSupport::getComposite(SMap,hydIndex,"-1 -7 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  // al
  Out=ModelSupport::getComposite(SMap,hydIndex,"-1 7 -17");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,modTemp,Out));

  // Hydrogen
  Out=ModelSupport::getComposite(SMap,hydIndex,"-1 17 -27");
  System.addCell(MonteCarlo::Qhull(cellIndex++,modMat,modTemp,Out));

  // Outer Al
  Out=ModelSupport::getComposite(SMap,hydIndex,"-1 27 -37");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,0.0,Out));
  
  // CYCLINDERS

  Out=ModelSupport::getComposite(SMap,hydIndex,"1 -111 -107 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  // al
  Out=ModelSupport::getComposite(SMap,hydIndex,"1 -101 107 -117");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,modTemp,Out));

  // Hydrogen
  Out=ModelSupport::getComposite(SMap,hydIndex,"1 -101 117 -127");
  System.addCell(MonteCarlo::Qhull(cellIndex++,modMat,modTemp,Out));

  // Outer Al
  Out=ModelSupport::getComposite(SMap,hydIndex,"1 -101 127 -137");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,0.0,Out));

  // Outer Al
  Out=ModelSupport::getComposite(SMap,hydIndex,"101 -111 107 -137");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,0.0,Out));

  return;
}

int
SphereModerator::getDividePlane() const
  /*!
    Get the dividing plane
    \return Dividing plane [pointing out]
  */
{
  return SMap.realSurf(hydIndex+1);
}

int
SphereModerator::viewSurf() const
  /*!
    View surface 
    \return view surface [pointing out]
   */
{
  return SMap.realSurf(hydIndex+2);
}

void 
SphereModerator::postCreateWork(Simulation& System)
  /*!
    Add a pipe to the hydrogen system:
    \remark This should be called after the void vessel has
    been constructed and all objects inserted.
    \param System :: Simulation to add pipe to
    \param VC :: Vacuum vessel of decoupled
  */
{
  ELog::RegMethod RegA("SphereModerator","postCreateWork");

  const Geometry::Vec3D APt(Origin+X*(innerRadius+outerRadius)/2.0+
			    Y*fYShift);
  const Geometry::Vec3D BPt(Origin-X*(innerRadius+outerRadius)/2.0+
			    Y*fYShift);

  // Outer Points
  InnerA.addPoint(APt);
  InnerA.addPoint(APt+Y*pipeLen);
  InnerA.addRadius(pipeRadius,modMat,modTemp);
  InnerA.addRadius(pipeAlRadius,alMat,modTemp); 
  InnerA.createAll(System);

  // Outer Points
  InnerB.addPoint(BPt);
  InnerB.addPoint(BPt+Y*pipeLen);
  InnerB.addRadius(pipeRadius,modMat,modTemp);
  InnerB.addRadius(pipeAlRadius,alMat,modTemp); 
  InnerB.createAll(System);

  return;
}
  
void
SphereModerator::createAll(Simulation& System,
			   const attachSystem::FixedComp& FUnit,
			   const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation to create objects in
    \param FUnit :: Fixed Base unit
    \param sideIndex :: link point to side
  */
{
  ELog::RegMethod RegA("SphereModerator","createAll");
  populate(System.getDataBase());

  createUnitVector(FUnit,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       
  
  return;
}
  
}  // NAMESPACE delftSystem
