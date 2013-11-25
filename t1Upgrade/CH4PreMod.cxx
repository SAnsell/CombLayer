/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1Upgrade/CH4PreMod.cxx
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
#include "surfExpand.h"
#include "chipDataStore.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "CH4PreModBase.h"
#include "CH4PreMod.h"

namespace ts1System
{

CH4PreMod::CH4PreMod(const std::string& Key)  :
  CH4PreModBase(Key,6)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

CH4PreMod::CH4PreMod(const CH4PreMod& A) : 
  CH4PreModBase(A),
  sideThick(A.sideThick),topThick(A.topThick),baseThick(A.baseThick),
  frontExt(A.frontExt),backExt(A.backExt),touchSurf(A.touchSurf),
  alThick(A.alThick),vacThick(A.vacThick),modTemp(A.modTemp),
  modMat(A.modMat),alMat(A.alMat),sidePts(A.sidePts),
  sideAxis(A.sideAxis)
  /*!
    Copy constructor
    \param A :: CH4PreMod to copy
  */
{}

CH4PreMod&
CH4PreMod::operator=(const CH4PreMod& A)
  /*!
    Assignment operator
    \param A :: CH4PreMod to copy
    \return *this
  */
{
  if (this!=&A)
    {
      CH4PreModBase::operator=(A);
      sideThick=A.sideThick;
      topThick=A.topThick;
      baseThick=A.baseThick;
      frontExt=A.frontExt;
      backExt=A.backExt;
      touchSurf=A.touchSurf;
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

CH4PreMod::~CH4PreMod() 
  /*!
    Destructor
  */
{}

CH4PreMod*
CH4PreMod::clone()  const
  /*!
    Clone method
    \return copy of this
  */
{
  return new CH4PreMod(*this);
}

void
CH4PreMod::getConnectPoints(const attachSystem::FixedComp& FC,
			  const size_t index)
  /*!
    Get connection points
    \param FC :: FixedComponent
    \param index :: Index points
   */
{
  ELog::RegMethod RegA("CH4PreMod","getConnectPoints");

  const size_t sequence[6][6]={ {0,1,2,3,4,5},
				{1,0,3,2,4,5},
				{2,3,0,1,4,5},
				{3,2,1,0,4,5},
				{4,5,2,3,0,1}, 
				{5,4,3,2,1,0} };

  if (FC.NConnect()<6)
    throw ColErr::RangeError<size_t>(0,6,FC.NConnect(),"Number of connects");
  if (FC.NConnect()<6)
    throw ColErr::IndexError<size_t>(index,6,"Index");

  
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
CH4PreMod::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("CH4PreMod","populate");
  
  const FuncDataBase& Control=System.getDataBase();
  
  sideThick=Control.EvalVar<double>(keyName+"SideThick");
  topThick=Control.EvalVar<double>(keyName+"TopThick");
  baseThick=Control.EvalVar<double>(keyName+"BaseThick");
  frontExt=Control.EvalVar<double>(keyName+"FrontExt");
  backExt=Control.EvalVar<double>(keyName+"BackExt");

  alThick=Control.EvalVar<double>(keyName+"AlThick");
  vacThick=Control.EvalVar<double>(keyName+"VacThick");

  modTemp=Control.EvalVar<double>(keyName+"ModTemp");
  modMat=ModelSupport::EvalMat<int>(Control,keyName+"ModMat");
  alMat=ModelSupport::EvalMat<int>(Control,keyName+"AlMat");
  
  return;
}
  
void
CH4PreMod::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: Component to connect to
  */
{
  ELog::RegMethod RegA("CH4PreMod","createUnitVector");

  FixedComp::createUnitVector(FC);

  return;
}
  
void
CH4PreMod::createSurfaces(const attachSystem::FixedComp& FC,
			  const size_t frontIndex)
  /*!
    Create All the surfaces
    \param FC :: Fixed unit that connects to this moderator
    \param frontIndex :: Front surface index
  */
{
  ELog::RegMethod RegA("CH4PreMod","createSurface");


  // 1-6 are effectively controlled:
  getConnectPoints(FC,frontIndex);
  
  // Front point is effectively a falling back surface
  ModelSupport::buildPlane(SMap,preIndex+13,
			   sidePts[2]-X*alThick,X);
  ModelSupport::buildPlane(SMap,preIndex+14,
			   sidePts[3]+X*alThick,X);
  ModelSupport::buildPlane(SMap,preIndex+15,
			   sidePts[4]-Z*alThick,Z);
  ModelSupport::buildPlane(SMap,preIndex+16,
			   sidePts[5]+Z*alThick,Z);

  // Water phase
  ModelSupport::buildPlane(SMap,preIndex+23,
			   sidePts[2]-X*(alThick+sideThick),X);
  ModelSupport::buildPlane(SMap,preIndex+24,
			   sidePts[3]+X*(alThick+sideThick),X);
  ModelSupport::buildPlane(SMap,preIndex+25,
			   sidePts[4]-Z*(alThick+baseThick),Z);
  ModelSupport::buildPlane(SMap,preIndex+26,
			   sidePts[5]+Z*(alThick+topThick),Z);

  // OuterAl phase
  ModelSupport::buildPlane(SMap,preIndex+33,
			   sidePts[2]-X*(2.0*alThick+sideThick),X);
  ModelSupport::buildPlane(SMap,preIndex+34,
			   sidePts[3]+X*(2.0*alThick+sideThick),X);
  ModelSupport::buildPlane(SMap,preIndex+35,
			   sidePts[4]-Z*(2*alThick+baseThick),Z);
  ModelSupport::buildPlane(SMap,preIndex+36,
			   sidePts[5]+Z*(2*alThick+topThick),Z);


  // Vac phase
  ModelSupport::buildPlane(SMap,preIndex+43,
			   sidePts[2]-X*(2.0*alThick+sideThick+vacThick),X);
  ModelSupport::buildPlane(SMap,preIndex+44,
			   sidePts[3]+X*(2.0*alThick+sideThick+vacThick),X);
  ModelSupport::buildPlane(SMap,preIndex+45,
			   sidePts[4]-Z*(2.0*alThick+baseThick+vacThick),Z);
  ModelSupport::buildPlane(SMap,preIndex+46,
			   sidePts[5]+Z*(2.0*alThick+topThick+vacThick),Z);

  // Al + Water End caps:
  ModelSupport::buildPlane(SMap,preIndex+121,sidePts[0]+Y*frontExt,Y);
  ModelSupport::buildPlane(SMap,preIndex+122,sidePts[1]-Y*backExt,Y);

  ModelSupport::buildPlane(SMap,preIndex+131,sidePts[0]+
			   Y*(frontExt+alThick),Y);
  ModelSupport::buildPlane(SMap,preIndex+132,sidePts[1]-
			   Y*(backExt+alThick),Y);

  ModelSupport::buildPlane(SMap,preIndex+141,sidePts[0]+
			   Y*(frontExt+alThick+vacThick),Y);
  ModelSupport::buildPlane(SMap,preIndex+142,sidePts[1]-
			   Y*(backExt+alThick+vacThick),Y);


  return;
}

void
CH4PreMod::createObjects(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const size_t frontIndex)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
    \param FC :: FixedComp Attached
    \param frontIndex :: Index of the front face
  */
{
  ELog::RegMethod RegA("CH4PreMod","createObjects");

  const size_t backIndex((frontIndex % 2) ? frontIndex-1 : frontIndex+1);
  int addFlag(0);
  std::string touch;
  std::string compTouch;
  std::string Inner(" (");
  std::string FullInner(" (");
  for(size_t i=0;i<6;i++)
    {
      if (i==touchSurf)
	{
	  touch=FC.getLinkComplement(i);
	  compTouch=FC.getLinkString(i);
	}
      else if (i!=frontIndex && i!=backIndex)
	{
	  if (addFlag) Inner+=":";
	  addFlag=1;
	  Inner+=FC.getLinkString(i);
	}
      FullInner+=FC.getLinkString(i)+":";
    }
  Inner+=") ";
  FullInner[FullInner.size()-1]=')';
  // Wrap AL:
  std::string Out;
  Out=ModelSupport::getSetComposite(SMap,preIndex,"13 -14 15 -16 -131 132");
  Out+=Inner+touch;
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,modTemp,Out));

  // Water:
  Out=ModelSupport::getSetComposite(SMap,preIndex,
		  "-121 122 23 -24 25 -26 (-13:14:-15:16)");
  Out+=touch;
  System.addCell(MonteCarlo::Qhull(cellIndex++,modMat,modTemp,Out));

  Out=ModelSupport::getSetComposite(SMap,preIndex,
		  "-131 132 33 -34 35 -36 (-23:24:-25:26)");
  Out+=touch;
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,modTemp,Out));

  // Front al wrapper
  Out=ModelSupport::getSetComposite(SMap,preIndex,
		  "-131 121 23 -24 25 -26 (-13:14:-15:16)");
  Out+=touch;
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,modTemp,Out));

  // Front al wrapper
  Out=ModelSupport::getSetComposite(SMap,preIndex,
		  "132 -122 23 -24 25 -26 (-13:14:-15:16)");
  Out+=touch;
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,modTemp,Out));

  const std::string FFace=FC.getLinkComplement(frontIndex);
  const std::string BFace=FC.getLinkComplement(backIndex);

  // VAC Outer:
  std::string IOut;
  Out=ModelSupport::getSetComposite(SMap,preIndex,"-131 132 43 -44 45 -46 ");  
  IOut=ModelSupport::getSetComposite(SMap,preIndex,"33 -34 35 -36 -131 132");
  IOut+=Inner+touch;
  Out+="#("+IOut+")";
  Out+=FullInner;
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  voidCell=cellIndex-1;

  Out=ModelSupport::getSetComposite(SMap,preIndex,
				    "43 -44 45 -46 ");  
  Out+=(backExt+alThick>0.0) ? 
    ModelSupport::getSetComposite(SMap,preIndex,"132 ") : 
    BFace;
  Out+=(frontExt+alThick>0.0) ? 
    ModelSupport::getSetComposite(SMap,preIndex,"-131 ") : 
    FFace;
  addOuterSurf(Out);
  return;
}

void 
CH4PreMod::createLinks()
  /*!
    Create link points and surfaces
   */
{
  ELog::RegMethod RegA("CH4PreMod","createLinks");

  const double layT(2.0*alThick+sideThick+vacThick);

  FixedComp::setConnect(0,sidePts[0]+Y*(backExt+alThick+vacThick),Y);
  FixedComp::setConnect(1,sidePts[1]-Y*(backExt+alThick+vacThick),-Y);
  FixedComp::setConnect(2,sidePts[2]-X*layT,-X);
  FixedComp::setConnect(3,sidePts[3]+X*layT,X);
  FixedComp::setConnect(4,sidePts[4]-Z*layT,-Z);
  FixedComp::setConnect(5,sidePts[5]+Z*layT,Z);

  FixedComp::setLinkSurf(0,-SMap.realSurf(preIndex+141));
  FixedComp::setLinkSurf(1,SMap.realSurf(preIndex+142));
  FixedComp::setLinkSurf(2,-SMap.realSurf(preIndex+43));
  FixedComp::setLinkSurf(3,SMap.realSurf(preIndex+44));
  FixedComp::setLinkSurf(4,-SMap.realSurf(preIndex+45));
  FixedComp::setLinkSurf(5,SMap.realSurf(preIndex+46));
  return;
}

void
CH4PreMod::createAll(Simulation& System,
		   const attachSystem::FixedComp& FC,
		     const size_t frontIndex,
		     const size_t touchIndex)
		   /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed unit that connects to this moderator
    \param frontIndex :: Front face index
    \param touchIndex :: Touching surface if present
  */
{
  ELog::RegMethod RegA("CH4PreMod","createAll");
  populate(System);

  touchSurf=touchIndex;
  createUnitVector(FC);
  createSurfaces(FC,frontIndex);
  createObjects(System,FC,frontIndex);
  createLinks();
  insertObjects(System);       
  
  return;
}
  
}  // NAMESPACE moderatorSystem
