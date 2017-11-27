/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Upgrade/HPreMod.cxx
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
#include "ContainedComp.h"
#include "HPreMod.h"

namespace ts1System
{

HPreMod::HPreMod(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,6),
  preIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(preIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

HPreMod::HPreMod(const HPreMod& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  preIndex(A.preIndex),cellIndex(A.cellIndex),
  populated(A.populated),centOrgFlag(A.centOrgFlag),
  lSideThick(A.lSideThick),rSideThick(A.rSideThick),topThick(A.topThick),
  baseThick(A.baseThick),backThick(A.backThick),
  alThick(A.alThick),vacThick(A.vacThick),modTemp(A.modTemp),
  modMat(A.modMat),alMat(A.alMat),sidePts(A.sidePts),
  sideAxis(A.sideAxis)
  /*!
    Copy constructor
    \param A :: HPreMod to copy
  */
{}

HPreMod&
HPreMod::operator=(const HPreMod& A)
  /*!
    Assignment operator
    \param A :: HPreMod to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      populated=A.populated;
      centOrgFlag=A.centOrgFlag;
      lSideThick=A.lSideThick;
      rSideThick=A.rSideThick;
      topThick=A.topThick;
      baseThick=A.baseThick;
      backThick=A.backThick;
      alThick=A.alThick;
      vacThick=A.vacThick;
      modTemp=A.modTemp;
      modMat=A.modMat;
      alMat=A.alMat;
      sidePts=A.sidePts;
      sideAxis=A.sideAxis;
    }
  return *this;
}

HPreMod::~HPreMod() 
  /*!
    Destructor
  */
{}

HPreMod*
HPreMod::clone() const
  /*!
    Clone function
    \return HPreMod(this)
  */
{
  return new HPreMod(*this);
}

void
HPreMod::getConnectPoints(const attachSystem::FixedComp& FC,
			  const long int sideIndex)
  /*!
    Get connection points
    \param FC :: FixedComponent
    \param sideIndex :: Index points
   */
{
  ELog::RegMethod RegA("HPreMod","getConnectPoints");

  const long int sequence[6][6]={ {1,2,3,4,5,6},
				  {2,1,4,3,5,6},
				  {3,4,1,2,5,6},
				  {4,3,2,1,5,6},
				  {5,6,3,4,1,2}, 
				  {6,5,4,3,2,1} };

  const size_t index(static_cast<size_t>(std::abs(sideIndex)-1));
  
  if (FC.NConnect()<6)
    throw ColErr::RangeError<size_t>(0,6,FC.NConnect(),"Number of connects");
  if (index>5)
    throw ColErr::RangeError<long int>(sideIndex,1,6,"sideIndex");
  
  sidePts.clear();
  sideAxis.clear();
  for(size_t i=0;i<6;i++)
    {
      sidePts.push_back(FC.getLinkPt(sequence[index][i]));
      sideAxis.push_back(FC.getLinkAxis(sequence[index][i]));
    }
  Y=sideAxis[0];
  Z=sideAxis[5];
  X=Z*Y;

  return;
}

void
HPreMod::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("HPreMod","populate");
  
  const FuncDataBase& Control=System.getDataBase();
  
  rSideThick=Control.EvalVar<double>(keyName+"RSideThick");
  lSideThick=Control.EvalVar<double>(keyName+"LSideThick");
  topThick=Control.EvalVar<double>(keyName+"TopThick");
  baseThick=Control.EvalVar<double>(keyName+"BaseThick");
  backThick=Control.EvalVar<double>(keyName+"BackThick");

  alThick=Control.EvalVar<double>(keyName+"AlThick");
  vacThick=Control.EvalVar<double>(keyName+"VacThick");

  modTemp=Control.EvalVar<double>(keyName+"ModTemp");

  modMat=ModelSupport::EvalMat<int>(Control,keyName+"ModMat");
  alMat=ModelSupport::EvalMat<int>(Control,keyName+"AlMat");
  
  return;
}
  
void
HPreMod::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: Component to connect to
  */
{
  ELog::RegMethod RegA("HPreMod","createUnitVector");

  FixedComp::createUnitVector(FC);

  return;
}
  
void
HPreMod::createSurfaces(const attachSystem::FixedComp& FC,
			const long int frontIndex)
  /*!
    Create All the surfaces
    \param FC :: Fixed unit that connects to this moderator
    \param frontIndex :: Front surface index
  */
{
  ELog::RegMethod RegA("HPreMod","createSurface");


  // 1-6 are effectively controlled:
  getConnectPoints(FC,frontIndex);
  
  // Front point is effectively a falling back surface
  ModelSupport::buildPlane(SMap,preIndex+11,
			   sidePts[0]-Y*alThick,Y);
  ModelSupport::buildPlane(SMap,preIndex+12,
			   sidePts[1]-Y*alThick,Y);
  ModelSupport::buildPlane(SMap,preIndex+13,
			   sidePts[2]-X*alThick,X);
  ModelSupport::buildPlane(SMap,preIndex+14,
			   sidePts[3]+X*alThick,X);
  ModelSupport::buildPlane(SMap,preIndex+15,
			   sidePts[4]-Z*alThick,Z);
  ModelSupport::buildPlane(SMap,preIndex+16,
			   sidePts[5]+Z*alThick,Z);

  // Water phase
  ModelSupport::buildPlane(SMap,preIndex+22,
			   sidePts[1]-Y*(alThick+backThick),Y);
  ModelSupport::buildPlane(SMap,preIndex+23,
			   sidePts[2]-X*(alThick+lSideThick),X);
  ModelSupport::buildPlane(SMap,preIndex+24,
			   sidePts[3]+X*(alThick+rSideThick),X);
  ModelSupport::buildPlane(SMap,preIndex+25,
			   sidePts[4]-Z*(alThick+baseThick),Z);
  ModelSupport::buildPlane(SMap,preIndex+26,
			   sidePts[5]+Z*(alThick+topThick),Z);

  // OuterAl phase
  ModelSupport::buildPlane(SMap,preIndex+32,
			   sidePts[1]-Y*(2.0*alThick+backThick),Y);
  ModelSupport::buildPlane(SMap,preIndex+33,
			   sidePts[2]-X*(2.0*alThick+lSideThick),X);
  ModelSupport::buildPlane(SMap,preIndex+34,
			   sidePts[3]+X*(2.0*alThick+rSideThick),X);
  ModelSupport::buildPlane(SMap,preIndex+35,
			   sidePts[4]-Z*(2*alThick+baseThick),Z);
  ModelSupport::buildPlane(SMap,preIndex+36,
			   sidePts[5]+Z*(2*alThick+topThick),Z);


  // Vac phase
  ModelSupport::buildPlane(SMap,preIndex+42,
			   sidePts[1]-Y*(2.0*alThick+backThick+vacThick),Y);
  ModelSupport::buildPlane(SMap,preIndex+43,
			   sidePts[2]-X*(2.0*alThick+lSideThick+vacThick),X);
  ModelSupport::buildPlane(SMap,preIndex+44,
			   sidePts[3]+X*(2.0*alThick+rSideThick+vacThick),X);
  ModelSupport::buildPlane(SMap,preIndex+45,
			   sidePts[4]-Z*(2.0*alThick+baseThick+vacThick),Z);
  ModelSupport::buildPlane(SMap,preIndex+46,
			   sidePts[5]+Z*(2.0*alThick+topThick+vacThick),Z);

  return;
}

void
HPreMod::createObjects(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int frontIndex)
  /*!
    Adds the main object components
    \param System :: Simulation to create objects in
    \param FC :: FixedComp Attached
    \param frontIndex :: Index of the front face
  */
{
  ELog::RegMethod RegA("HPreMod","createObjects");

  int addFlag(0);
  std::string Inner(" (");
  for(long int i=1;i<7;i++)
    {
      if (i!=frontIndex)
	{
	  if (addFlag) Inner+=":";
	  addFlag=1;
	  Inner+=FC.getLinkString(i);
	}
    }
  Inner+=")";

  const std::string FFace=FC.getLinkString(-frontIndex);
  // Set first link Point:
  FixedComp::setLinkSignedCopy(0,FC,frontIndex);

  // Wrap AL:
  std::string Out;
  Out=ModelSupport::getSetComposite(SMap,preIndex,"12 13 -14 15 -16 ");
  Out+=FFace+Inner;
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,modTemp,Out));

  Out=ModelSupport::getSetComposite(SMap,preIndex,
			       "32 33 -34 35 -36 (11:-22:-23:24:-25:26) "
				    "(-11:-13:14:-15:16)");
  Out+=FFace;
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,modTemp,Out));
  // Water:
  Out=ModelSupport::getSetComposite(SMap,preIndex,
		  " -11 22 23 -24 25 -26 (-12:-13:14:-15:16) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,modMat,modTemp,Out));

  // Clearance Void
  Out=ModelSupport::getSetComposite(SMap,preIndex,
		  " 42 43 -44 45 -46 (-32:-33:34:-35:36) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out+FFace));

  Out=ModelSupport::getSetComposite(SMap,preIndex,"42 43 -44 45 -46 ");
  Out+=FFace;
  addOuterSurf(Out);
  return;
}

void
HPreMod::createLinks()
  /*!
    Create the links [0 set in createSurfaces]
   */
{
  ELog::RegMethod RegA("CH4PreMod","createLinks");

  const double layT(2.0*alThick+backThick+vacThick);

  FixedComp::setConnect(1,sidePts[1]-Y*layT,-Y);
  FixedComp::setConnect(2,sidePts[2]-X*layT,-X);
  FixedComp::setConnect(3,sidePts[3]+X*layT,X);
  FixedComp::setConnect(4,sidePts[4]-Z*layT,-Z);
  FixedComp::setConnect(5,sidePts[5]+Z*layT,Z);


  FixedComp::setLinkSurf(1,SMap.realSurf(preIndex+42));
  FixedComp::setLinkSurf(2,-SMap.realSurf(preIndex+43));
  FixedComp::setLinkSurf(3,SMap.realSurf(preIndex+44));
  FixedComp::setLinkSurf(4,-SMap.realSurf(preIndex+45));
  FixedComp::setLinkSurf(5,SMap.realSurf(preIndex+46));
  return;

}

void
HPreMod::createAll(Simulation& System,
		   const attachSystem::FixedComp& FC,
		   const long int frontIndex)
   /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed unit that connects to this moderator
    \param frontIndex :: Front face index
  */
{
  ELog::RegMethod RegA("HPreMod","createAll");
  populate(System);

  createUnitVector(FC);
  createSurfaces(FC,frontIndex);
  createObjects(System,FC,frontIndex);
  createLinks();
  insertObjects(System);       
  
  return;
}
  
}  // NAMESPACE moderatorSystem
