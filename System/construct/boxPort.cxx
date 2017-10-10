/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/boxPort.cxx
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
#include <array>

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
#include "FixedGroup.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "FrontBackCut.h"
#include "boxPort.h"

namespace constructSystem
{

boxPort::boxPort(const std::string& BKey,
		 const std::string& PKey) :
  attachSystem::FixedOffset(BKey+PKey,6),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),attachSystem::SurfMap(),
  attachSystem::FrontBackCut(),
  baseName(BKey),
  boxIndex(ModelSupport::objectRegister::Instance().cell(keyName)),
  cellIndex(boxIndex+1),populated(0),innerExclude(0),
  NBolts(0),sealRadius(0.0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param BKey :: Base KeyName
    \param PKey :: primary name
  */
{}


boxPort::~boxPort() 
  /*!
    Destructor
  */
{}


void
boxPort::setDimensions(const size_t NB,
		       const double IW,const double OW,
		       const double IH,const double OH,
		       const double TK,const double BR)
/*!
    Set the demensions of the system
    \param NB :: Number of bolts
    \param IW :: Inner width
    \param OW :: Outer width
    \param IH :: Inner height
    \param OH :: Outer height
    \param TK :: Thickness
    \param BR :: Bolt-Radius
   */
{
  ELog::RegMethod RegA("boxPort","setDimensions");

  NBolts=NB;

  if (OH-IH<Geometry::zeroTol || IH<Geometry::zeroTol)
    throw ColErr::OrderError<double>(IH,OH,"Inner Height >= Outer Height");
  if (OW-IW<Geometry::zeroTol || IW<Geometry::zeroTol)
    throw ColErr::OrderError<double>(IW,OW,"Inner Width >= Outer Width");
  
  if (TK<Geometry::zeroTol)
    throw ColErr::SizeError<double>(TK,0.0,"Thickness");

  if (NBolts && (BR<Geometry::zeroTol ||
		 BR>(OW-IW)/2.0 ||
		 BR>(OH-IH)/2.0))
    throw ColErr::SizeError<double>(BR,0.0,"Bolt radius");

  innerHeight=IH;
  innerWidth=IW;

  outerHeight=OH;
  outerWidth=OW;
  
  thick=TK;
    
  boltRadius=BR;

  populated |= 1;
  return;
}

void
boxPort::setMaterials(const std::string& boltM,const std::string& mainM)
  /*!
    Set the materials
    \param boltM :: bolt material
    \param mainM :: Main material
   */
{
  ELog::RegMethod RegA("boxPort","setMaterials");
  
  boltMat=ModelSupport::EvalMatName(boltM);
  mainMat=ModelSupport::EvalMatName(mainM);
  populated |= 2;
  return;
}
  
void
boxPort::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase of variables
  */
{
  ELog::RegMethod RegA("boxPort","populate");

  FixedOffset::populate(Control);
  if (!(populated & 1))
    {
      const size_t NB=
	Control.EvalDefPair<size_t>(keyName,baseName,"NBolts",0);
      const double BR=
	Control.EvalDefPair<double>(keyName,baseName,"BoltRadius",0.0);

      const double IW=Control.EvalPair<double>(keyName,baseName,"InnerWidth");
      const double OW=Control.EvalPair<double>(keyName,baseName,"OuterWidth");
      const double IH=Control.EvalPair<double>(keyName,baseName,"InnerHeight");
      const double OH=Control.EvalPair<double>(keyName,baseName,"OuterHeight");

      double TK(1.0);  // default value to ignore if active/front/back
      if (!backActive() || !frontActive())
	TK=Control.EvalPair<double>(keyName,baseName,"Thickness");
      
      setDimensions(NB,IW,OW,IH,OH,TK,BR);
    }
  if (!(populated & 2))
    {
      boltMat=ModelSupport::EvalDefMat<int>(Control,keyName+"BoltMat",
					    baseName+"BoltMat",0);
      mainMat=ModelSupport::EvalMat<int>(Control,
					 keyName+"MainMat",
					 baseName+"MainMat");
      voidMat=ModelSupport::EvalDefMat<int>
	(Control,keyName+"VoidMat",baseName+"VoidMat",0);

      populated |= 2;
    }
  if (!(populated & 4))
    {
      sealRadius=Control.EvalDefPair<double>
	(keyName,baseName,"SealRadius",0.0);
      sealThick=Control.EvalDefPair<double>
	(keyName,baseName,"SealThick",0.2);
      sealMat=ModelSupport::EvalDefMat<int>
	(Control,keyName+"SealMat",baseName+"SealMat",0);
      populated |= 4;
    }

  return;
}

  
void
boxPort::createUnitVector(const attachSystem::FixedComp& FC,
                              const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed component to link to
    \param sideIndex :: Link point and direction [0 for origin]
  */
{
  ELog::RegMethod RegA("boxPort","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}


void
boxPort::createSurfaces()
  /*!
    Create the surfaces
  */
{
  ELog::RegMethod RegA("boxPort","createSurfaces");
   // Construct surfaces:

  ModelSupport::buildPlane(SMap,boxIndex+3,Origin-X*(innerWidth/2.0),X);
  ModelSupport::buildPlane(SMap,boxIndex+4,Origin+X*(innerWidth/2.0),X);
  ModelSupport::buildPlane(SMap,boxIndex+5,Origin-Z*(innerHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,boxIndex+6,Origin+Z*(innerHeight/2.0),Z);

  ModelSupport::buildPlane(SMap,boxIndex+13,Origin-X*(outerWidth/2.0),X);
  ModelSupport::buildPlane(SMap,boxIndex+14,Origin+X*(outerWidth/2.0),X);
  ModelSupport::buildPlane(SMap,boxIndex+15,Origin-Z*(outerHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,boxIndex+16,Origin+Z*(outerHeight/2.0),Z);

  if (!frontActive())
    {
      ModelSupport::buildPlane(SMap,boxIndex+1,Origin-Y*(thick/2.0),Y);
      setFront(SMap.realSurf(boxIndex+1));
    }
  if (!backActive())
    {
      ModelSupport::buildPlane(SMap,boxIndex+2,Origin+Y*(thick/2.0),Y);
      setBack(-SMap.realSurf(boxIndex+2));
      
    }
  
  // BOLTS:
  if (NBolts>0)
    {
    }
			   
  
  return;
}
  
void
boxPort::createObjects(Simulation& System)
  /*!
    Adds the vacuum box
    \param System :: Simulation to create objects in
    */
{
  ELog::RegMethod RegA("boxPort","createObjects");

  std::string Out;

  const std::string FBStr=frontRule()+backRule();
  const std::string EdgeStr=ModelSupport::getComposite
    (SMap,boxIndex," 13 -14 15 -16 (-3:4:-5:6) ");


  System.addCell(MonteCarlo::Qhull(cellIndex++,mainMat,0.0,FBStr+EdgeStr));
  addCell("Box",cellIndex-1);


  if (innerExclude)
    {
      Out=ModelSupport::getComposite(SMap,boxIndex," 3 -4 5 -6 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,voidMat,0.0,Out+FBStr));
      addCell("InnerVoid",cellIndex-1);
      
      Out=ModelSupport::getComposite(SMap,boxIndex," 13 -14 15 -16 ");      
      addOuterSurf(Out+FBStr);
    }
  else
    addOuterSurf(EdgeStr+FBStr);
  
  return;
}



void
boxPort::createLinks()
  /*!
    Determines the link point on the outgoing plane.
    It must follow the beamline, but exit at the plane
  */
{
  ELog::RegMethod RegA("boxPort","createLinks");

  return;
}

void
boxPort::createAll(Simulation& System,
		    const attachSystem::FixedComp& mainFC,
		    const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param beamFC :: FixedComp at the beam centre
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("boxPort","createAll(FC)");

  populate(System.getDataBase());
  createUnitVector(mainFC,sideIndex);
  createSurfaces();    
  createObjects(System);
  
  createLinks();
  insertObjects(System);   

  return;
}
  
}  // NAMESPACE constructSystem

