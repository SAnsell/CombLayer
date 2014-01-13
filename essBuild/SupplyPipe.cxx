/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   essBuild/SupplyPipe.cxx
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
#include "stringCombine.h"
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
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "LayerComp.h"
#include "pipeUnit.h"
#include "PipeLine.h"
#include "SupplyPipe.h"
#include "Debug.h"

namespace essSystem
{

SupplyPipe::SupplyPipe(const std::string& Key)  :
  attachSystem::FixedComp(Key,0),
  pipeIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(pipeIndex+1),Coaxial(Key+"CoAx")
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

SupplyPipe::SupplyPipe(const SupplyPipe& A) : 
  attachSystem::FixedComp(A),
  pipeIndex(A.pipeIndex),cellIndex(A.cellIndex),
  NSegIn(A.NSegIn),inRadius(A.inRadius),
  inAlRadius(A.inAlRadius),midAlRadius(A.midAlRadius),
  voidRadius(A.voidRadius),outAlRadius(A.outAlRadius),
  inMat(A.inMat),inAlMat(A.inAlMat),midAlMat(A.midAlMat),
  voidMat(A.voidMat),outAlMat(A.outAlMat),
  Coaxial(A.Coaxial),PPts(A.PPts)
  /*!
    Copy constructor
    \param A :: SupplyPipe to copy
  */
{}

SupplyPipe&
SupplyPipe::operator=(const SupplyPipe& A)
  /*!
    Assignment operator
    \param A :: SupplyPipe to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      NSegIn=A.NSegIn;
      inRadius=A.inRadius;
      inAlRadius=A.inAlRadius;
      midAlRadius=A.midAlRadius;
      voidRadius=A.voidRadius;
      outAlRadius=A.outAlRadius;
      inMat=A.inMat;
      inAlMat=A.inAlMat;
      midAlMat=A.midAlMat;
      voidMat=A.voidMat;
      outAlMat=A.outAlMat;
      Coaxial=A.Coaxial;
      PPts=A.PPts;
    }
  return *this;
}

SupplyPipe::~SupplyPipe() 
  /*!
    Destructor
  */
{}

void
SupplyPipe::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("SupplyPipe","populate");
  
  const FuncDataBase& Control=System.getDataBase();
  
  
  NSegIn=Control.EvalVar<size_t>(keyName+"NSegIn");
  for(size_t i=0;i<=NSegIn;i++)
    {
      PPts.push_back(Control.EvalVar<Geometry::Vec3D>
		     (StrFunc::makeString(keyName+"PPt",i)));
    }
  inRadius=Control.EvalVar<double>(keyName+"InRadius"); 
  inAlRadius=Control.EvalVar<double>(keyName+"InAlRadius"); 
  midAlRadius=Control.EvalVar<double>(keyName+"MidAlRadius"); 
  voidRadius=Control.EvalVar<double>(keyName+"VoidRadius"); 
  outAlRadius=Control.EvalVar<double>(keyName+"OutAlRadius"); 

  inMat=ModelSupport::EvalMat<int>(Control,keyName+"InMat"); 
  inAlMat=ModelSupport::EvalMat<int>(Control,keyName+"InAlMat"); 
  midAlMat=ModelSupport::EvalMat<int>(Control,keyName+"MidAlMat"); 
  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat"); 
  outAlMat=ModelSupport::EvalMat<int>(Control,keyName+"OutAlMat"); 
  
  return;
}
  

void
SupplyPipe::createUnitVector(const attachSystem::FixedComp& FC,
			     const size_t layerIndex,
			     const size_t sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed unit that it is connected to 
    \param sideIndex :: Connection point to use as origin [0 for origin]
  */
{
  ELog::RegMethod RegA("SupplyPipe","createUnitVector");

  FixedComp::createUnitVector(FC);
  const attachSystem::LayerComp* LC=
    dynamic_cast<const attachSystem::LayerComp*>(&FC);

  if (LC && sideIndex)
    {
      Origin=LC->getSurfacePoint(layerIndex,sideIndex-1);
      FC.selectAltAxis(sideIndex-1,X,Y,Z);
    }
  return;
}

void 
SupplyPipe::insertInlet(Simulation& System,
			const attachSystem::FixedComp& FC,
			const size_t lSideIndex)
  /*!
    Add a pipe to the hydrogen system:
    \remark This should be called after the moderator has
    been constructed and all objects inserted.
    \param System :: Simulation to add pipe to
    \param FC :: layer values
    \param lSideIndex :: point for side of moderator
  */
{
  ELog::RegMethod RegA("SupplyPipe","insertInlet");
  const attachSystem::LayerComp* LC=
    dynamic_cast<const attachSystem::LayerComp*>(&FC);
 
  if (!LC)
    throw ColErr::CastError<void>(0,"FixedComp to LayerComp");

  Geometry::Vec3D Pt= Origin+X*PPts[0].X()+
    Y*PPts[0].Y()+Z*PPts[0].Z();

  // GET Z Point from layer
  Geometry::Vec3D PtZ=LC->getSurfacePoint(0,lSideIndex);
  const int commonSurf=LC->getCommonSurf(lSideIndex);
  const std::string commonStr=(commonSurf) ? 		       
    StrFunc::makeString(commonSurf) : "";
  if (PtZ!=Pt)
    Coaxial.addPoint(Pt);

  Coaxial.addSurfPoint(PtZ,
		       LC->getLayerString(0,lSideIndex),
		       commonStr);


  PtZ=LC->getSurfacePoint(2,lSideIndex);
  if (PtZ!=Pt)
    Coaxial.addSurfPoint(PtZ,LC->getLayerString(2,lSideIndex),
			 commonStr);

  // Inner Points
  for(size_t i=1;i<=NSegIn;i++)
    {
       Pt=Origin+X*PPts[i].X()+Y*PPts[i].Y()+Z*PPts[i].Z();
       Coaxial.addPoint(Pt);
    }  
  
  Coaxial.addRadius(inRadius,inMat,0.0);
  Coaxial.addRadius(inAlRadius,inAlMat,0.0);
  Coaxial.addRadius(midAlRadius,midAlMat,0.0);
  Coaxial.addRadius(voidRadius,voidMat,0.0);
  Coaxial.addRadius(outAlRadius,outAlMat,0.0);

  Coaxial.setActive(0,1);  
  // Coaxial.setActive(1,5);
  // Coaxial.setActive(2,29);
  // Coaxial.setActive(3,29);
  
  Coaxial.createAll(System);
  return;
}
  
void
SupplyPipe::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const size_t orgLayerIndex,
		      const size_t orgSideIndex,
		      const size_t exitSideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation to create objects in
    \param FC :: Fixed Base unit
    \param sideIndex :: Link point for origin 
    \param sideIndex :: Link point for origin 
    \param laySideIndex :: layer to pass pipe though
  */
{
  ELog::RegMethod RegA("SupplyPipe","createAll");
  populate(System);

  createUnitVector(FC,orgLayerIndex,orgSideIndex);
  insertInlet(System,FC,exitSideIndex);
    
  return;
}
  
}  // NAMESPACE moderatorSystem
