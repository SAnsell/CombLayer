/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Build/MonoPlug.cxx
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
#include <iterator>
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
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "SimProcess.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "ContainedComp.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "MonoPlug.h"

namespace shutterSystem
{

MonoPlug::MonoPlug(const std::string& Key)  : 
  attachSystem::FixedComp(Key,3),attachSystem::ContainedComp(),
  plugIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(plugIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Key to use
  */
{}

MonoPlug::MonoPlug(const MonoPlug& A) : 
  attachSystem::FixedComp(A),attachSystem::ContainedComp(A),
  plugIndex(A.plugIndex),cellIndex(A.cellIndex),
  nPlugs(A.nPlugs),plugRadii(A.plugRadii),
  plugZLen(A.plugZLen),plugClearance(A.plugClearance),
  dividerZLen(A.dividerZLen),steelMat(A.steelMat),
  concMat(A.concMat)
  /*!
    Copy constructor
    \param A :: MonoPlug to copy
  */
{}

MonoPlug&
MonoPlug::operator=(const MonoPlug& A)
  /*!
    Assignment operator
    \param A :: MonoPlug to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedComp::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      cellIndex=A.cellIndex;
      nPlugs=A.nPlugs;
      plugRadii=A.plugRadii;
      plugZLen=A.plugZLen;
      plugClearance=A.plugClearance;
      dividerZLen=A.dividerZLen;
      steelMat=A.steelMat;
      concMat=A.concMat;
    }
  return *this;
}

MonoPlug::~MonoPlug() 
  /*!
    Destructor
  */
{}

void
MonoPlug::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("MonoPlug","populate");

  const FuncDataBase& Control=System.getDataBase();

  nPlugs=Control.EvalVar<size_t>(keyName+"NPlugs");   
  const std::string PRad=keyName+"PlugRadius";
  const std::string PLen=keyName+"PlugZLen";
  for(size_t i=0;i<nPlugs;i++)
    {
      plugRadii.push_back
	(Control.EvalVar<double>(StrFunc::makeString(PRad,i+1)));
      plugZLen.push_back
	(Control.EvalVar<double>(StrFunc::makeString(PLen,i+1)));
    }
  plugClearance=Control.EvalVar<double>(keyName+"PlugClearance");   
  dividerZLen=Control.EvalVar<double>(keyName+"DivideLen");   
 
  steelMat=ModelSupport::EvalMat<int>(Control,keyName+"SteelMat");
  concMat=ModelSupport::EvalMat<int>(Control,keyName+"ConcMat");
      
  return;
}

void
MonoPlug::createUnitVector(const attachSystem::FixedComp& VoidFC,
			   const long int sideIndex)
  /*!
    Create the unit vectors
    \param VoidFC :: VoidVessel to get top and axises
    \param sideIndex  :: Top surface
  */
{
  ELog::RegMethod RegA("MonoPlug","createUnitVector");

  attachSystem::FixedComp::createUnitVector(VoidFC);
  Origin=VoidFC.getLinkPt(sideIndex);
  return;
}

void
MonoPlug::createSurfaces()
  /*!
    Create all the surfaces
  */
{
  ELog::RegMethod RegA("MonoPlug","createSurface");

  //
  // OUTER VOID
  //

  int pI(plugIndex+7);
  for(size_t i=0;i<nPlugs;i++)
    {
      ModelSupport::buildCylinder(SMap,pI,Origin,Z,plugRadii[i]);
      ModelSupport::buildCylinder(SMap,pI+10,Origin,Z,
				  plugRadii[i]+plugClearance);
      pI+=100;
    }
  // Separations
  pI=plugIndex+6;
  for(size_t i=0;i<nPlugs;i++)
    {
      ModelSupport::buildPlane(SMap,pI,
			       Origin+Z*plugZLen[i],Z);
      ModelSupport::buildPlane(SMap,pI+10,
			       Origin+Z*(plugZLen[i]-plugClearance),Z);
      pI+=100;
    }
  // External stuff
  ModelSupport::buildPlane(SMap,plugIndex+5006,
			   Origin+Z*dividerZLen,Z);
  
  return;
}

void
MonoPlug::createObjects(Simulation& System,
			const long int vLCIndex,
			const attachSystem::FixedComp& VoidFC,
			const attachSystem::FixedComp& BulkFC)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
    \param vLCIndex :: void link point
    \param VoidFC :: Main outer void
    \param BulkFC :: Bulk steel object
  */
{
  ELog::RegMethod RegA("MonoPlug","createObjects");

  // No work to do!
  if (!nPlugs) return; 
  
  std::string Out;

  // The outside stuff
  const std::string voidSurf=VoidFC.getLinkString(vLCIndex);
  const std::string outSurf=VoidFC.getLinkString(-1);
  

  const std::string bulkSurf=
    BulkFC.getLinkString(-vLCIndex);

  // SPECIAL FOR ONE SINGLE ITEM:
  if (nPlugs==1)
    {
      Out=ModelSupport::getComposite(SMap,plugIndex," -7 ")+
	voidSurf+" "+bulkSurf;
      System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,0.0,Out));

      Out=ModelSupport::getComposite(SMap,plugIndex,"7 -17 ")+
	voidSurf+" "+bulkSurf;
      System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

      Out=ModelSupport::getComposite(SMap,plugIndex," 17 ")+
	voidSurf+" "+bulkSurf+" "+outSurf;
      System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,0.0,Out));
      return;
    }

  // First Layer
  Out=ModelSupport::getComposite(SMap,plugIndex,"-6 -7 ")+
    voidSurf;
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,plugIndex,"-6 7 -17 ")+
    voidSurf;
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  
  Out=ModelSupport::getComposite(SMap,plugIndex,"-6 16 17 -117 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  Out=ModelSupport::getComposite(SMap,plugIndex,"-16 17 ")+
    voidSurf+outSurf;
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,0.0,Out));
  
  // MAIN LOOP
  int pI=plugIndex;
  for(size_t i=1;i<nPlugs-1;i++)
    {
      // Steel inner:
      Out=ModelSupport::getComposite(SMap,pI, "6 -106 -107");      
      System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,0.0,Out));
      
      Out=ModelSupport::getComposite(SMap,pI, "6 -106 -117 107 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
     
      Out=ModelSupport::getComposite(SMap,pI+100, "-6 16 17 -117 ");      
      System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
      
      Out=ModelSupport::getComposite(SMap,pI,"16 -116 117 ")+outSurf;
      System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,0.0,Out));
      // Next loop index
      pI+=100;
    }
  
  // TOP Layer
  Out=ModelSupport::getComposite(SMap,pI, "6 -107")+bulkSurf;
  System.addCell(MonteCarlo::Qhull(cellIndex++,concMat,0.0,Out));
  
  Out=ModelSupport::getComposite(SMap,pI, "6 -117 107 ")+bulkSurf;
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  
  Out=ModelSupport::getComposite(SMap,pI, "16 117 ")+bulkSurf+outSurf;
  System.addCell(MonteCarlo::Qhull(cellIndex++,concMat,0.0,Out));
  
  return;
}


void
MonoPlug::createLinks()
  /*!
    Create links
   */
{
  ELog::RegMethod RegA("MonoPlug","createLinks");
  return;
}

void
MonoPlug::createAll(Simulation& System,
		    const long int vLCIndex,
		    const attachSystem::FixedComp& VoidFC,
		    const attachSystem::FixedComp& BulkFC)
  /*!
    Create the shutter
    \param System :: Simulation to process
    \param vLCIndex :: void link point
    \param VoidFC :: Main outer void
    \param BulkFC :: Bulk steel object
  */
{
  ELog::RegMethod RegA("MonoPlug","createAll");
  populate(System);
  createUnitVector(VoidFC,vLCIndex);
  createSurfaces();
  createObjects(System,vLCIndex,VoidFC,BulkFC);
  createLinks();
  insertObjects(System);
  return;
}
  
}  // NAMESPACE shutterSystem
