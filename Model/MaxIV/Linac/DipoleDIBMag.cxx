/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/Linac/DipoleDIBMag.cxx
 *
 * Copyright (c) 2004-2020 by Konstantin Batkov
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Surface.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "ExternalCut.h"

#include "DipoleDIBMag.h"

namespace tdcSystem
{

DipoleDIBMag::DipoleDIBMag(const std::string& Key)  :
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::ExternalCut()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

DipoleDIBMag::DipoleDIBMag(const DipoleDIBMag& A) :
  attachSystem::FixedRotate(A),
  attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),
  attachSystem::SurfMap(A),
  attachSystem::ExternalCut(A),
  magOffset(A.magOffset),magHeight(A.magHeight),magWidth(A.magWidth),
  magLength(A.magLength),
  magInnerWidth(A.magInnerWidth),
  frameHeight(A.frameHeight),
  frameWidth(A.frameWidth),
  frameLength(A.frameLength),
  hGap(A.hGap),
  vGap(A.vGap),
  voidMat(A.voidMat),coilMat(A.coilMat),
  frameMat(A.frameMat)
  /*!
    Copy constructor
    \param A :: DipoleDIBMag to copy
  */
{}

DipoleDIBMag&
DipoleDIBMag::operator=(const DipoleDIBMag& A)
  /*!
    Assignment operator
    \param A :: DipoleDIBMag to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      magOffset=A.magOffset;
      magHeight=A.magHeight;
      magWidth=A.magWidth;
      magLength=A.magLength;
      magInnerWidth=A.magInnerWidth;
      frameHeight=A.frameHeight;
      frameWidth=A.frameWidth;
      frameLength=A.frameLength;
      hGap=A.hGap;
      vGap=A.vGap;
      voidMat=A.voidMat;
      coilMat=A.coilMat;
      frameMat=A.frameMat;
    }
  return *this;
}

DipoleDIBMag*
DipoleDIBMag::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new DipoleDIBMag(*this);
}

DipoleDIBMag::~DipoleDIBMag()
  /*!
    Destructor
  */
{}

void
DipoleDIBMag::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("DipoleDIBMag","populate");

  FixedRotate::populate(Control);

  magOffset=Control.EvalVar<double>(keyName+"MagOffset");
  magHeight=Control.EvalVar<double>(keyName+"MagHeight");
  magWidth=Control.EvalVar<double>(keyName+"MagWidth");
  magLength=Control.EvalVar<double>(keyName+"MagLength");
  magInnerWidth=Control.EvalVar<double>(keyName+"MagInnerWidth");
  frameHeight=Control.EvalVar<double>(keyName+"FrameHeight");
  frameWidth=Control.EvalVar<double>(keyName+"FrameWidth");
  frameLength=Control.EvalVar<double>(keyName+"FrameLength");
  hGap=Control.EvalVar<double>(keyName+"HGap");
  vGap=Control.EvalVar<double>(keyName+"VGap");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  coilMat=ModelSupport::EvalMat<int>(Control,keyName+"CoilMat");
  frameMat=ModelSupport::EvalMat<int>(Control,keyName+"FrameMat");

  return;
}

void
DipoleDIBMag::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: object for origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("DipoleDIBMag","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
DipoleDIBMag::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("DipoleDIBMag","createSurfaces");

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(magLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(magLength/2.0),Y);

  // frame
  ModelSupport::buildPlane(SMap,buildIndex+11,Origin-Y*(frameLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(frameLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(frameWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(frameWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(frameHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(frameHeight/2.0),Z);

  // magnets
  ModelSupport::buildPlane(SMap,buildIndex+1003,Origin-X*(magWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+1004,Origin+X*(magWidth/2.0),X);

  ModelSupport::buildPlane(SMap,buildIndex+1013,Origin-X*(magInnerWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+1014,Origin+X*(magInnerWidth/2.0),X);

  const Geometry::Vec3D LOrg(Origin-Z*(magOffset/2.0)); // lower tier
  ModelSupport::buildPlane(SMap,buildIndex+1005,LOrg-Z*magHeight,Z);
  ModelSupport::buildPlane(SMap,buildIndex+1006,LOrg,Z);

  const Geometry::Vec3D TOrg(Origin+Z*(magOffset/2.0)); // upper tier
  ModelSupport::buildPlane(SMap,buildIndex+2005,TOrg,Z);
  ModelSupport::buildPlane(SMap,buildIndex+2006,TOrg+Z*magHeight,Z);

  const double Rout = magWidth/2.0;
  ModelSupport::buildCylinder(SMap,buildIndex+1007,
			      Origin-Y*(magLength/2.0-Rout),Z,Rout);
  ModelSupport::buildCylinder(SMap,buildIndex+1008,
			      Origin+Y*(magLength/2.0-Rout),Z,Rout);

  const double Rin = magInnerWidth/2.0;
  ModelSupport::buildCylinder(SMap,buildIndex+1017,
			      Origin-Y*(magLength/2.0-Rout),Z,Rin);
  ModelSupport::buildCylinder(SMap,buildIndex+1018,
			      Origin+Y*(magLength/2.0-Rout),Z,Rin);

  // auxillary planes
  ModelSupport::buildPlane(SMap,buildIndex+1001,Origin-Y*(magLength/2.0-Rout),Y);
  ModelSupport::buildPlane(SMap,buildIndex+1002,Origin+Y*(magLength/2.0-Rout),Y);

  // gaps between frame and magnets

  ModelSupport::buildPlane(SMap,buildIndex+23,Origin-X*(magWidth/2.0+hGap),X);
  ModelSupport::buildPlane(SMap,buildIndex+24,Origin+X*(magWidth/2.0+hGap),X);

  ModelSupport::buildPlane(SMap,buildIndex+25,LOrg-Z*(magHeight+vGap),Z);

  //  ModelSupport::buildPlane(SMap,buildIndex+26,TOrg+Z*(magHeight+vGap),Z);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+26,buildIndex+2006,vGap);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+33,buildIndex+1013,vGap);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+34,buildIndex+1014,-vGap);

  return;
}

void
DipoleDIBMag::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("DipoleDIBMag","createObjects");

  std::string Out;
  // insert cell [e.g. pipe]
  const std::string ICell=isActive("Inner") ? getRuleStr("Inner") : "";

  // // Frame
  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 13 -14 26 -16" );
  makeCell("FrameTop",System,cellIndex++,frameMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 23 -33 2006 -26 " );
  makeCell("FrameTopGap",System,cellIndex++,voidMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 34 -24 2006 -26 " );
  makeCell("FrameTopGap",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 13 -14 15 -25 " );
  makeCell("FrameBottom",System,cellIndex++,frameMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 23 -33 25 -1005 " );
  makeCell("FrameBottomGap",System,cellIndex++,voidMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 34 -24 25 -1005 " );
  makeCell("FrameBottomGap",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 13 -23 25 -26 " );
  makeCell("FrameLeft",System,cellIndex++,frameMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 23 -1003 1005 -2006 " );
  makeCell("FrameLeftGap",System,cellIndex++,voidMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 24 -14 25 -26 " );
  makeCell("FrameRight",System,cellIndex++,frameMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 1004 -24 1005 -2006 " );
  makeCell("FrameRightGap",System,cellIndex++,voidMat,0.0,Out);

  for (const std::string side : {" 12 -2 ", " 1 -11 "}) // front/back ends
    {
      Out=ModelSupport::getComposite(SMap,buildIndex,side+" 13 -1003 15 -16 " );
      makeCell("FrameLeftVoidCorner",System,cellIndex++,voidMat,0.0,Out);

      Out=ModelSupport::getComposite(SMap,buildIndex,side+" 1004 -14 15 -16 " );
      makeCell("FrameRightVoidCorner",System,cellIndex++,voidMat,0.0,Out);

      Out=ModelSupport::getComposite(SMap,buildIndex,side+" 1003 -1004 2006 -16 " );
      makeCell("FrameTopVoidMiddle",System,cellIndex++,voidMat,0.0,Out);

      Out=ModelSupport::getComposite(SMap,buildIndex,side+" 1003 -1004 15 -1005 " );
      makeCell("FrameBottomVoidMiddle",System,cellIndex++,voidMat,0.0,Out);
    }

  size_t tier(0);
  const std::vector<std::string> tbInner = {" 25 -1006 ", " 2005 -26 "};
  const std::vector<std::string> tbGap = {" 25 -1005 ", " 2006 -26 "};

  // Magnets
  int BI(buildIndex);
  for(const std::string partName : {"Bottom", "Top"} )
    {
      const std::string tb(ModelSupport::getComposite(SMap,BI," 1005 -1006 "));

      Out=ModelSupport::getComposite(SMap,buildIndex,
		 " 1003 -1013 1001 -1002 ");
      makeCell(partName+"MagLeftRec",System,cellIndex++,coilMat,0.0,Out+tb);

      Out=ModelSupport::getComposite(SMap,buildIndex,
      		 " 1014 -1004 1001 -1002 ");
      makeCell(partName+"MagRightRec",System,cellIndex++,coilMat,0.0,Out+tb);

      Out=ModelSupport::getComposite(SMap,buildIndex,
      		 " 1002 -1008 1018 ");
      makeCell(partName+"MagCyl",System,cellIndex++,coilMat,0.0,Out+tb);

      Out=ModelSupport::getComposite(SMap,buildIndex,
      		 " -1001 -1007 1017 ");
      makeCell(partName+"MagCyl",System,cellIndex++,coilMat,0.0,Out+tb);

      // Frame inside the magnet
      Out=ModelSupport::getComposite(SMap,buildIndex,
				     " 33 -34 1001 -1002 " + tbInner[tier]);
      makeCell(partName+"FrameInner",System,cellIndex++,frameMat,0.0,Out);

      Out=ModelSupport::getComposite(SMap,buildIndex,
      				     " 1013 -33 1001 -1002 ");
      makeCell(partName+"FrameInnerGap1",System,cellIndex++,voidMat,0.0,Out+tb);

      Out=ModelSupport::getComposite(SMap,buildIndex,
      				     " 34 -1014 1001 -1002 ");
      makeCell(partName+"FrameInnerGap1",System,cellIndex++,voidMat,0.0,Out+tb);

      Out=ModelSupport::getComposite(SMap,buildIndex,
      				     " 33 -34 1002 -12 " + tbGap[tier]);
      makeCell(partName+"FrameInnerGap2",System,cellIndex++,voidMat,0.0,Out);
      Out=ModelSupport::getComposite(SMap,buildIndex,
      				     " 33 -34 11 -1001 " + tbGap[tier]);
      makeCell(partName+"FrameInnerGap2",System,cellIndex++,voidMat,0.0,Out);


      // void inside inner cylinders
      Out=ModelSupport::getComposite(SMap,buildIndex,
      				     " 1002 -1018 ");
      makeCell(partName+"MagVoidCylIn",System,cellIndex++,voidMat,0.0,Out+tb);

      Out=ModelSupport::getComposite(SMap,buildIndex,
      				     " -1001 -1017 ");
      makeCell(partName+"MagVoidCylIn",System,cellIndex++,voidMat,0.0,Out+tb);

      // void outside outer cylinders
      Out=ModelSupport::getComposite(SMap,buildIndex,
      				     " 1003 -1004 1002 1008 -2 ");
      makeCell(partName+"MagVoidCylOut",System,cellIndex++,voidMat,0.0,Out+tb);

      Out=ModelSupport::getComposite(SMap,buildIndex,
      				     " 1 1003 -1004 -1001 1007 ");
      makeCell(partName+"MagVoidCylOut",System,cellIndex++,voidMat,0.0,Out+tb);

      BI+=1000;
      tier++;
    }

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " 1 -2 1003 -1004 1006 -2005 ");
  makeCell("VoidMiddle",System,cellIndex++,voidMat,0.0,Out+ICell);
  
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 1 -2 13 -14 15 -16 " );
  addOuterSurf(Out);

  return;
}


void
DipoleDIBMag::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("DipoleDIBMag","createLinks");

  FixedComp::setConnect(0,Origin-Y*(magLength/2.0),-Y);
  FixedComp::setConnect(1,Origin+Y*(magLength/2.0),Y);

  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));

  return;
}

void
DipoleDIBMag::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("DipoleDIBMag","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // tdcSystem
