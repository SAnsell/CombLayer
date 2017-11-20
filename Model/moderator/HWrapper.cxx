/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   moderator/HWrapper.cxx
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
#include "surfExpand.h"
#include "chipDataStore.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "HWrapper.h"

namespace moderatorSystem
{

HWrapper::HWrapper(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,0),
  preIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(preIndex+1),populated(0),
  divideSurf(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

HWrapper::HWrapper(const HWrapper& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  preIndex(A.preIndex),cellIndex(A.cellIndex),
  populated(A.populated),divideSurf(A.divideSurf),sideExt(A.sideExt),
  heightExt(A.heightExt),backExt(A.backExt),forwardExt(A.forwardExt),
  wingLen(A.wingLen),vacInner(A.vacInner),alInner(A.alInner),
  alOuter(A.alOuter),vacOuter(A.vacOuter),modTemp(A.modTemp),
  modMat(A.modMat),alMat(A.alMat)
  /*!
    Copy constructor
    \param A :: HWrapper to copy
  */
{}

HWrapper&
HWrapper::operator=(const HWrapper& A)
  /*!
    Assignment operator
    \param A :: HWrapper to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      populated=A.populated;
      divideSurf=A.divideSurf;
      sideExt=A.sideExt;
      heightExt=A.heightExt;
      backExt=A.backExt;
      forwardExt=A.forwardExt;
      wingLen=A.wingLen;
      vacInner=A.vacInner;
      alInner=A.alInner;
      alOuter=A.alOuter;
      vacOuter=A.vacOuter;
      modTemp=A.modTemp;
      modMat=A.modMat;
      alMat=A.alMat;
    }
  return *this;
}

HWrapper::~HWrapper() 
  /*!
    Destructor
  */
{}

void
HWrapper::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("HWrapper","populate");
  
  const FuncDataBase& Control=System.getDataBase();
  
  sideExt=Control.EvalVar<double>(keyName+"SideExt");
  heightExt=Control.EvalVar<double>(keyName+"HeightExt");
  backExt=Control.EvalVar<double>(keyName+"BackExt");
  forwardExt=Control.EvalVar<double>(keyName+"ForwardExt");

  vacInner=Control.EvalVar<double>(keyName+"VacInner");
  vacOuter=Control.EvalVar<double>(keyName+"VacOuter");
  alInner=Control.EvalVar<double>(keyName+"AlInner");
  alOuter=Control.EvalVar<double>(keyName+"VacOuter");

  wingLen=Control.EvalVar<double>(keyName+"WingLen");

  modTemp=Control.EvalVar<double>(keyName+"ModTemp");
  modMat=ModelSupport::EvalMat<int>(Control,keyName+"ModMat");
  alMat=ModelSupport::EvalMat<int>(Control,keyName+"AlMat");
  
  populated |= 1;
  return;
}

void
HWrapper::createSurfMesh(const int NSurf,const int GPlus,
			 const int* baseSurf,const double* Offset,
			 const int* signDir)
  /*!
    Construct a set of surfaces
    \param NSurf :: Number of surfaces
    \param GPlus :: Global offset 
    \param baseSurf :: List of the base surface set
    \param Offset :: Distance to move
    \param signDir :: Sign to multiply by
  */
{
  ELog::RegMethod RegA("HWrapper","createSurfMesh");

  Geometry::Surface* SX;
  for(int i=0;i<NSurf;i++)
    {
      SX=ModelSupport::surfaceCreateExpand
	(SMap.realSurfPtr(preIndex+baseSurf[i]),Offset[i]);
      SX->setName(preIndex+GPlus+baseSurf[i]);
      if (signDir[i]<0)
	{
	  Geometry::Plane* PPtr=dynamic_cast<Geometry::Plane*>(SX);
	  if (PPtr)
	    PPtr->mirrorSelf();
	  else
	    ELog::EM<<"Negative surf "<<SX->getName()<<ELog::endErr;
	}
      SMap.registerSurf(preIndex+GPlus+baseSurf[i],SX);
    }
  return;
}
  
  
void
HWrapper::createUnitVector(const attachSystem::FixedComp& VacFC)
  /*!
    Create the unit vectors
    \param VacFC :: Vacuum vessel
  */
{
  ELog::RegMethod RegA("HWrapper","createUnitVector");
  //  const masterRotate& MR=masterRotate::Instance();

  FixedComp::createUnitVector(VacFC);
  FixedComp::applyRotation(Z,180.0);
  return;
}
  
void
HWrapper::createSurfaces(const attachSystem::FixedComp& VacFC,
			 const attachSystem::FixedComp& FLine)
  /*!
    Create All the surfaces
    \param VacFC :: Fixed unit that connects to this moderator
    \param FLine :: FLight line that co-joines
  */
{
  ELog::RegMethod RegA("HWrapper","createSurface");
  const int NSurf(10);
  //  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  //  const masterRotate& MR=masterRotate::Instance();

  // Number of surfaces are required :: 
  //  Base/Top/Sides/Divider/CylinderFront.
  //   5  / 6 / 3,4 /   1   /   2 

  // Note can use the divide surface as a real surface.
  SMap.addMatch(preIndex+1,divideSurf);
  SMap.addMatch(preIndex+2,VacFC.getLinkSurf(2));  // front
  SMap.addMatch(preIndex+3,VacFC.getLinkSurf(3));  // side
  SMap.addMatch(preIndex+4,VacFC.getLinkSurf(4));  // side
  SMap.addMatch(preIndex+5,VacFC.getLinkSurf(5));  // base
  SMap.addMatch(preIndex+6,VacFC.getLinkSurf(6));  // top  [target]

  // FLight line
  SMap.addMatch(preIndex+103,FLine.getLinkSurf(-3));  // side 
  SMap.addMatch(preIndex+104,FLine.getLinkSurf(4));  // side
  SMap.addMatch(preIndex+105,FLine.getLinkSurf(-5));  // base [outer]
  SMap.addMatch(preIndex+106,FLine.getLinkSurf(6));  // top  [target]

  const double sideWater=sideExt-(alInner+vacOuter+vacInner);
  const double topWater=heightExt-(alOuter+alInner+vacOuter+vacInner);
  //  const double frontWater=forwardExt-(alOuter+alInner+vacOuter+vacInner);

  const int baseSurf[NSurf]={1,2,3,4,5,6,103,104,105,106};
  double mainOffset[NSurf]={backExt,forwardExt,-sideExt,sideExt,
			    -heightExt,0.0,0,0,0,0};
  const int signDir[NSurf]={-1,1,1,1,1,1,1,1,1,1};
  const int addDir[NSurf]={1,1,-1,1,-1,1,1,-1,1,-1};
  // CREATE GLOBAL:
  createSurfMesh(NSurf,10,baseSurf,mainOffset,signDir);  

  // CREATE OUTER VAC  
  for(int i=0;i<NSurf-4;i++)
    mainOffset[i]-=addDir[i]*vacOuter;
  createSurfMesh(NSurf,20,baseSurf,mainOffset,signDir);  

  // CREATE OUTER AL
  for(int i=0;i<NSurf;i++)
    mainOffset[i]-=addDir[i]*alOuter;
  createSurfMesh(NSurf,30,baseSurf,mainOffset,signDir);  

  // CREATE Water LAYER: Complex:
  for(int i=0;i<NSurf-4;i++)
    mainOffset[i]=addDir[i]*(alInner+vacInner);
  int idx=NSurf-4;
  mainOffset[idx]= -addDir[idx]*(sideWater+alInner); idx++;
  mainOffset[idx]= -addDir[idx]*(sideWater+alInner); idx++;
  mainOffset[idx]= -addDir[idx]*(topWater+alInner); idx++;
  mainOffset[idx]= -addDir[idx]*(topWater+alInner); 
  createSurfMesh(NSurf,40,baseSurf,mainOffset,signDir);  

  // CREATE AL inner LAYER: 
  for(int i=0;i<NSurf-4;i++)
    mainOffset[i]=addDir[i]*vacInner;
  for(int i=NSurf-4;i<NSurf;i++)
    mainOffset[i]-=addDir[i]*alOuter;
  createSurfMesh(NSurf,50,baseSurf,mainOffset,signDir);  

  // Just the outer values:
  for(int i=NSurf-4;i<NSurf;i++)
    mainOffset[i]-=addDir[i]*alOuter;
  createSurfMesh(4,60,baseSurf+NSurf-4,
		 mainOffset+NSurf-4,signDir+NSurf-4);  

  // Wing extension:
  
  // Full length
  Geometry::Surface* SX;
  SX=ModelSupport::surfaceCreateExpand
    (SMap.realSurfPtr(preIndex+2),wingLen);
  SX->setName(preIndex+202);
  SMap.registerSurf(preIndex+202,SX);

  SX=ModelSupport::surfaceCreateExpand
    (SMap.realSurfPtr(preIndex+2),wingLen-vacOuter);
  SX->setName(preIndex+212);
  SMap.registerSurf(preIndex+212,SX);

  SX=ModelSupport::surfaceCreateExpand
    (SMap.realSurfPtr(preIndex+2),wingLen-(alOuter+vacOuter));
  SX->setName(preIndex+222);
  SMap.registerSurf(preIndex+222,SX);
  
  return;
}

void
HWrapper::createObjects(Simulation& System,
			const attachSystem::ContainedComp& CM)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
    \param CM :: Basic Pre-moderator [to avoid clipping]
  */
{
  ELog::RegMethod RegA("HWrapper","createObjects");
    
  std::string Out;
  // basic HWrapper
  Out=ModelSupport::getComposite(SMap,preIndex,"11 -12 13 -14 15 -6 ");
  addOuterSurf(Out);
  // Outer horn object
  Out=ModelSupport::getComposite(SMap,preIndex,"1 12 163 -164 165 -166 -202");
  addOuterUnionSurf(Out);

  Out=ModelSupport::getComposite(SMap,preIndex,"11 -12 13 -14 15 -6 "
				 " (2 : -3 : 4 : -5 ) "
				 "(-23 : 24 : 22 : -25 : -21 : 26)"
				 "( -31 : -153 : 154 : -155 : 156) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  // Al skin
  Out=ModelSupport::getComposite(SMap,preIndex,"21 -22 23 -24 25 -26 "
				 " (52 : -53 : 54 : -55 ) "
				 "(-33 : 34 : 32 : -35 : -31 : 36)"
				 "( -1 : -143 : 144 : -145 : 146) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,0.0,Out));

  // Al EXTRA [26 > 156]
  Out=ModelSupport::getComposite(SMap,preIndex,"1 2 -22 153 -154 26 -156 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,modTemp,Out));

  // Al EXTRA [36 > 146]
  Out=ModelSupport::getComposite(SMap,preIndex,"1 52 -42 143 -144 36 -146 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,modTemp,Out));

  // Water
  Out=ModelSupport::getComposite(SMap,preIndex,"31 -32 33 -34 35 -36 "
				 " (2 : -3 : 4 : -5 ) "
				 "(-43 : 44 : 42 : -45 : -41 )"
				 "( -1 : -133 : 134 : -135 : 136) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,modMat,modTemp,Out));

  // Water EXTRA [36 > 146]
  Out=ModelSupport::getComposite(SMap,preIndex,"1 42 -32 143 -144 36 -146 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,modMat,modTemp,Out));

  // AL Inner
  Out=ModelSupport::getComposite(SMap,preIndex,"31 -42 43 -44 45 -36 "
				 " (2 : -3 : 4 : -5 ) "
				 "(-53 : 54 : 52 : -55 : -51 : 56)"
				 "( -1 : -103 : 104 : -105 : 106) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,modTemp,Out));


  // Vac inner
  Out=ModelSupport::getComposite(SMap,preIndex,"21 -52 53 -54 55 -26 "
				 " (2 : -3 : 4 : -5 ) "
				 "( -1 : -103 : 104 : -105 : 106) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  // WINGS:

  // Al inner
  Out=ModelSupport::getComposite(SMap,preIndex,"1 42 133 -134 135 -136 -212 "
				 " (-103 : 104 : -105 : 106) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,modTemp,Out));

  // Water inner
  Out=ModelSupport::getComposite(SMap,preIndex,"1 32 143 -144 145 -146 -222 "
				 " (-133 : 134 : -135 : 136) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,modMat,modTemp,Out));

  // outer Al
  Out=ModelSupport::getComposite(SMap,preIndex,"1 22 153 -154 155 -156 -212 "
				 " (-143 : 144 : 222 : -145 : 146) "
				 " (-133 : 134 : -135 : 136) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,modTemp,Out));

  // outer Vac
  Out=ModelSupport::getComposite(SMap,preIndex,"1 12 163 -164 165 -166 -202 "
				 " (-153 : 154 : 212 : -155 : 156) "
				 " (-103 : 104 : -105 : 106) ");
  Out+=CM.getExclude();
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  return;
}

void
HWrapper::createAll(Simulation& System,
		    const attachSystem::FixedComp& vacFC,
		    const attachSystem::FixedComp& FLine,
		    const attachSystem::ContainedComp& CM)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param vacFC :: Fixed unit that connects to this moderator
    \param FLine :: Hydrogen Flight line
    \param CM :: Central Pre-moderator
  */
{
  ELog::RegMethod RegA("HWrapper","createAll");
  populate(System);
  
  createUnitVector(vacFC);
  createSurfaces(vacFC,FLine);
  createObjects(System,CM);
  insertObjects(System);       
  
  return;
}
  
}  // NAMESPACE moderatorSystem
