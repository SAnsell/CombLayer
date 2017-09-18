/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Upgrade/CH4PreFlat.cxx
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
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
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
#include "LayerComp.h"
#include "CH4PreModBase.h"
#include "CH4PreFlat.h"

namespace ts1System
{

CH4PreFlat::CH4PreFlat(const std::string& Key)  :
  CH4PreModBase(Key,6)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

CH4PreFlat::CH4PreFlat(const CH4PreFlat& A) : 
  CH4PreModBase(A),
  width(A.width),height(A.height),depth(A.depth),alThick(A.alThick),
  vacThick(A.vacThick),modTemp(A.modTemp),modMat(A.modMat),
  alMat(A.alMat)
  /*!
    Copy constructor
    \param A :: CH4PreFlat to copy
  */
{}

CH4PreFlat&
CH4PreFlat::operator=(const CH4PreFlat& A)
  /*!
    Assignment operator
    \param A :: CH4PreFlat to copy
    \return *this
  */
{
  if (this!=&A)
    {
      CH4PreModBase::operator=(A);
      width=A.width;
      height=A.height;
      depth=A.depth;
      alThick=A.alThick;
      vacThick=A.vacThick;
      modTemp=A.modTemp;
      modMat=A.modMat;
      alMat=A.alMat;
    }
  return *this;
}

CH4PreFlat::~CH4PreFlat() 
  /*!
    Destructor
  */
{}

CH4PreFlat*
CH4PreFlat::clone()  const
  /*!
    Clone method
    \return copy of this
  */
{
  return new CH4PreFlat(*this);
}

void
CH4PreFlat::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("CH4PreFlat","populate");

  FixedOffset::populate(Control);

  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");

  alThick=Control.EvalVar<double>(keyName+"AlThick");
  vacThick=Control.EvalVar<double>(keyName+"VacThick"); 
  modTemp=Control.EvalVar<double>(keyName+"ModTemp");
  modMat=ModelSupport::EvalMat<int>(Control,keyName+"ModMat");
  alMat=ModelSupport::EvalMat<int>(Control,keyName+"AlMat");

  zStep+=alThick+height/2.0;
  return;
}
  
void
CH4PreFlat::createUnitVector(const attachSystem::FixedComp& FC,
			     const long int linkPt)
  /*!
    Create the unit vectors
    \param FC :: Component to connect to
    \param linkPt :: Link point to base of moderator
  */
{
  ELog::RegMethod RegA("CH4PreFlat","createUnitVector");

  FixedComp::createUnitVector(FC,linkPt);
  applyOffset();

  return;
}
  
void
CH4PreFlat::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("CH4PreFlat","createSurface");

  // NOTE Origin is moved from moderator base:

  ModelSupport::buildPlane(SMap,preIndex+1,Origin-Y*(depth/2.0),Y);
  ModelSupport::buildPlane(SMap,preIndex+2,Origin+Y*(depth/2.0),Y);
  ModelSupport::buildPlane(SMap,preIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,preIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,preIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,preIndex+6,Origin+Z*(height/2.0),Z);


  // AL plane contacts
  ModelSupport::buildPlane(SMap,preIndex+11,
			   Origin-Y*(depth/2.0+alThick),Y);
  ModelSupport::buildPlane(SMap,preIndex+12,
			   Origin+Y*(depth/2.0+alThick),Y);
  ModelSupport::buildPlane(SMap,preIndex+13,
			   Origin-X*(width/2.0+alThick),X);
  ModelSupport::buildPlane(SMap,preIndex+14,
			   Origin+X*(width/2.0+alThick),X);
  ModelSupport::buildPlane(SMap,preIndex+15,
			   Origin-Z*(height/2.0+alThick),Z);
  ModelSupport::buildPlane(SMap,preIndex+16,
			   Origin+Z*(height/2.0+alThick),Z);


  // VAC GAP
  ModelSupport::buildPlane(SMap,preIndex+21,
			   Origin-Y*(depth/2.0+alThick+vacThick),Y);
  ModelSupport::buildPlane(SMap,preIndex+22,
			   Origin+Y*(depth/2.0+alThick+vacThick),Y);
  ModelSupport::buildPlane(SMap,preIndex+23,
			   Origin-X*(width/2.0+alThick+vacThick),X);
  ModelSupport::buildPlane(SMap,preIndex+24,
			   Origin+X*(width/2.0+alThick+vacThick),X);
  ModelSupport::buildPlane(SMap,preIndex+25,
			   Origin-Z*(height/2.0+alThick+vacThick),Z);
  ModelSupport::buildPlane(SMap,preIndex+26,
			   Origin+Z*(height/2.0+alThick+vacThick),Z);

  return;
}

void
CH4PreFlat::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("CH4PreFlat","createObjects");
  std::string Out;

  Out=ModelSupport::getSetComposite(SMap,preIndex,"1 -2 3 -4 5 -6");
  System.addCell(MonteCarlo::Qhull(cellIndex++,modMat,modTemp,Out));
  // AL layer
  Out=ModelSupport::getSetComposite(SMap,preIndex,
				    " 11 -12 13 -14 15 -16 (-1:2:-3:4:-5:6) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,modTemp,Out));

  // VAC Outer:
  Out=ModelSupport::getSetComposite(SMap,preIndex,
		       " 21 -22 23 -24 25 -26 (-11:12:-13:14:-15:16) ");

  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  //  voidCell=cellIndex-1;
  
  Out=ModelSupport::getSetComposite(SMap,preIndex," 21 -22 23 -24 25 -26 ");

  addOuterSurf(Out);
  return;
}

Geometry::Vec3D
CH4PreFlat::getSurfacePoint(const size_t layerIndex,
			    const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link point
    \param sideIndex :: Side [0-5]
    \param layerIndex :: layer, 0 is inner moderator 
    \return Surface point
  */
{
  ELog::RegMethod RegA("CH4PreFlat","getSurfacePoint");

  const size_t SI((sideIndex>0) ?
                  static_cast<size_t>(sideIndex-1) :
                  static_cast<size_t>(-1-sideIndex));
  
  if (SI>5) 
    throw ColErr::IndexError<long int>(sideIndex,5,"sideIndex");
  if (layerIndex>=nLayers) 
    throw ColErr::IndexError<size_t>(layerIndex,nLayers,"layerIndex");

  const double LVec[]={depth/2.0,depth/2.0,width/2.0,
		       width/2.0,height/2.0,height/2.0};
  const double TVec[]={0.0,alThick,alThick+vacThick};

  const Geometry::Vec3D XYZ[6]={-Y,Y,-X,X,-Z,Z};
  return Origin+XYZ[SI]*(LVec[SI]+TVec[layerIndex]);
}


std::string
CH4PreFlat::getLayerString(const size_t layerIndex,
			   const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link surf
    \param sideIndex :: Side [0-5]
    \param layerIndex :: layer, 0 is inner moderator [0-LVec]
    \return Surface string
  */
{
  ELog::RegMethod RegA("CH4PreFlat","getLayerString");
  return StrFunc::makeString(getLayerSurf(layerIndex,sideIndex));
}

int
CH4PreFlat::getLayerSurf(const size_t layerIndex,
			 const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link surf
    \param sideIndex :: Side [0-5]
    \param layerIndex :: layer, 0 is inner moderator [0-4]
    \return Surface string
  */
{
  ELog::RegMethod RegA("CH4PreFlat","getLayerSurf");

  if (layerIndex>=nLayers) 
    throw ColErr::IndexError<size_t>(layerIndex,nLayers,"layerIndex");
  if (sideIndex>6 || sideIndex<-6 || !sideIndex)
    throw ColErr::IndexError<long int>(sideIndex,6,"sideIndex");

  const size_t uSIndex(static_cast<size_t>(std::abs(sideIndex)));
  const int SI(preIndex+static_cast<int>(layerIndex*10+uSIndex));
  const int signValue((sideIndex % 2) ? -1 : 1);
  return signValue*SMap.realSurf(SI);
}

void 
CH4PreFlat::createLinks()
  /*!
    Create link points and surfaces
   */
{
  ELog::RegMethod RegA("CH4PreFlat","createLinks");

  const double layT(alThick+vacThick);

  FixedComp::setConnect(0,Origin-Y*(depth/2.0+layT),-Y);
  FixedComp::setConnect(1,Origin+Y*(depth/2.0+layT),Y);
  FixedComp::setConnect(2,Origin-X*(width/2.0+layT),-X);
  FixedComp::setConnect(3,Origin+X*(width/2.0+layT),X);
  FixedComp::setConnect(4,Origin-Z*(height/2.0+layT),-Z);
  FixedComp::setConnect(5,Origin+Z*(height/2.0+layT),Z);


  FixedComp::setLinkSurf(0,-SMap.realSurf(preIndex+21));
  FixedComp::setLinkSurf(1,SMap.realSurf(preIndex+22));
  FixedComp::setLinkSurf(2,-SMap.realSurf(preIndex+23));
  FixedComp::setLinkSurf(3,SMap.realSurf(preIndex+24));
  FixedComp::setLinkSurf(4,-SMap.realSurf(preIndex+25));
  FixedComp::setLinkSurf(5,SMap.realSurf(preIndex+26));
  return;
}


void
CH4PreFlat::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int linkPt,
		      const long int)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed unit that connects to this moderator
    \param linkPt :: Base index
  */
{
  ELog::RegMethod RegA("CH4PreFlat","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,linkPt);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       
  
  return;
}
  
}  // NAMESPACE moderatorSystem
