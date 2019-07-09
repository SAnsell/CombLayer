/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/cosaxs/DiffPumpXIADP03.cxx
 *
 * Copyright (c) 2019 by Konstantin Batkov
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
#include "Quadratic.h"
#include "Plane.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "inputParam.h"
#include "HeadRule.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ReadFunctions.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"
#include "SurInter.h"

#include "DiffPumpXIADP03.h"

namespace constructSystem
{

DiffPumpXIADP03::DiffPumpXIADP03(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,7),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::ExternalCut()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

DiffPumpXIADP03::~DiffPumpXIADP03()
  /*!
    Destructor
  */
{}

void
DiffPumpXIADP03::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("DiffPumpXIADP03","populate");

  FixedOffset::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  apertureHeight=Control.EvalVar<double>(keyName+"ApertureHeight");
  apertureWidth=Control.EvalVar<double>(keyName+"ApertureWidth");

  flangeRadius=Control.EvalVar<double>(keyName+"FlangeRadius");
  flangeThick=Control.EvalVar<double>(keyName+"FlangeThick");
  flangeVoidWidth=Control.EvalVar<double>(keyName+"FlangeVoidWidth");
  flangeVoidHeight=Control.EvalVar<double>(keyName+"FlangeVoidHeight");
  flangeVoidThick=Control.EvalVar<double>(keyName+"FlangeVoidThick");
  magnetWidth=Control.EvalVar<double>(keyName+"MagnetWidth");
  magnetLength=Control.EvalVar<double>(keyName+"MagnetLength");
  magnetThick=Control.EvalVar<double>(keyName+"MagnetThick");
  magnetGap=Control.EvalVar<double>(keyName+"MagnetGap");

  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  magnetMat=ModelSupport::EvalMat<int>(Control,keyName+"MagnetMat");
  flangeMat=ModelSupport::EvalMat<int>(Control,keyName+"FlangeMat");

  return;
}

void
DiffPumpXIADP03::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: object for origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("DiffPumpXIADP03","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
DiffPumpXIADP03::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("DiffPumpXIADP03","createSurfaces");

  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+21,Origin,Y);
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+21));
    }

  if (!isActive("back"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+22,Origin+Y*(length),Y);
      ExternalCut::setCutSurf("back",-SMap.realSurf(buildIndex+22));
    }

  // main cell
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin+Y*(flangeThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length-flangeThick),Y);

  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

  // aperture
  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(apertureWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(apertureWidth/2.0),X);

  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(apertureHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(apertureHeight/2.0),Z);

  // flange
  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,Y,flangeRadius);
  // rectangular void inside flange
  const Geometry::Plane* FPtr=
    SMap.realPtr<Geometry::Plane>(getRule("front").getPrimarySurface());
  const Geometry::Plane* BPtr=
    SMap.realPtr<Geometry::Plane>(getRule("back").getPrimarySurface());

  ModelSupport::buildShiftedPlane(SMap,buildIndex+31,FPtr,flangeVoidThick);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+32,BPtr,-flangeVoidThick);
  SurfMap::setSurf("innerBack",-SMap.realSurf(buildIndex+32));
  
  ModelSupport::buildPlane(SMap,buildIndex+33,
			   Origin-X*(flangeVoidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+34,
			   Origin+X*(flangeVoidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+35,
			   Origin-Z*(flangeVoidHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+36,
			   Origin+Z*(flangeVoidHeight/2.0),Z);

  // magnets
  ModelSupport::buildPlane(SMap,buildIndex+41,
			   Origin+Y*(length/2.0-magnetLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+42,
			   Origin+Y*(length/2.0+magnetLength/2.0),Y);

  ModelSupport::buildPlane(SMap,buildIndex+43,Origin-X*(magnetWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+44,Origin+X*(magnetWidth/2.0),X);

  ModelSupport::buildPlane
    (SMap,buildIndex+45,Origin-Z*(height/2.0+magnetThick+magnetGap),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+46,Origin-Z*(height/2.0+magnetGap),Z);

  ModelSupport::buildPlane
    (SMap,buildIndex+55,Origin+Z*(height/2.0+magnetGap),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+56,Origin+Z*(height/2.0+magnetGap+magnetThick),Z);


  return;
}

void
DiffPumpXIADP03::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("DiffPumpXIADP03","createObjects");

  std::string Out,side;
  const std::string frontStr(getRuleStr("front"));
  const std::string backStr(getRuleStr("back"));

  Out=ModelSupport::getComposite(SMap,buildIndex," -1 -27 (-33:34:-35:36) ");
  makeCell("FlangeBack",System,cellIndex++,flangeMat,0.0,Out+frontStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," 2 -27 (-33:34:-35:36) ");
  makeCell("FlangeFront",System,cellIndex++,flangeMat,0.0,Out+backStr);

  Out=ModelSupport::getComposite(SMap,buildIndex," -31 33 -34 35 -36 ");
  makeCell("FlangeBackVoid",System,cellIndex++,0,0.0,Out+frontStr);

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 " 1 -31 3 -4 5 -6 (-33:34:-35:36) ");
  System.addCell(cellIndex++,flangeMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 32 33 -34 35 -36 ");
  makeCell("FlangeFrontVoid",System,cellIndex++,0,0.0,Out+backStr);

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 32 -2 3 -4 5 -6 (-33:34:-35:36) ");
  System.addCell(cellIndex++,flangeMat,0.0,Out);

  side=ModelSupport::getComposite(SMap,buildIndex," 31 -32 ");
  Out=ModelSupport::getComposite(SMap,buildIndex," 3 -4 5 -6 (-13:14:-15:16) ");
  makeCell("Body",System,cellIndex++,mat,0.0,Out+side);

  Out=ModelSupport::getComposite(SMap,buildIndex," 13 -14 15 -16 ");
  makeCell("Aperture",System,cellIndex++,0,0.0,Out+side);

  side=ModelSupport::getComposite(SMap,buildIndex,"41 -42 43 ");

  Out=ModelSupport::getComposite(SMap,buildIndex," -44 46 -5 ");
  makeCell("MagnetLowVoid",System,cellIndex++,0,0.0,Out+side);

  Out=ModelSupport::getComposite(SMap,buildIndex," -44 45 -46 ");
  makeCell("MagnetLow",System,cellIndex++,magnetMat,0.0,Out+side);

  Out=ModelSupport::getComposite(SMap,buildIndex," -44 6 -55 ");
  makeCell("MagnetUpVoid",System,cellIndex++,0,0.0,Out+side);

  Out=ModelSupport::getComposite(SMap,buildIndex," -44 55 -56 ");
  makeCell("MagnetUp",System,cellIndex++,magnetMat,0.0,Out+side);

  // void around base and magnets
  side=ModelSupport::getComposite(SMap,buildIndex,"41 -42 -27");

  Out=ModelSupport::getComposite(SMap,buildIndex,"6 44 41 -42 -27");
  makeCell("ExternalVoid",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"43 -44 56 41 -42 -27");
  makeCell("ExternalVoid",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,"-43 6 41 -42 -27");
  makeCell("ExternalVoid",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 4 5 -6 41 -42 -27");
  makeCell("ExternalVoid",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -43 -5 41 -42 -27");
  makeCell("ExternalVoid",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 44 -5 41 -42 -27");
  makeCell("ExternalVoid",System,cellIndex++,0,0.0,Out);
  
  Out=ModelSupport::getComposite(SMap,buildIndex," 43 -44 -45 41 -42 -27");
  makeCell("ExternalVoid",System,cellIndex++,0,0.0,Out);
  
  Out=ModelSupport::getComposite(SMap,buildIndex," -3 5 -6 41 -42 -27");
  makeCell("ExternalVoid",System,cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -41 -27 (-3:4:-5:6)");
  System.addCell(cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," 42 -2 -27 (-3:4:-5:6)");
  System.addCell(cellIndex++,0,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex," -27 ");
  addOuterSurf(Out+frontStr+backStr);

  return;
}


void
DiffPumpXIADP03::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("DiffPumpXIADP03","createLinks");

  ExternalCut::createLink("front",*this,0,Origin,Y);
  ExternalCut::createLink("back",*this,1,Origin,Y);

  FixedComp::setConnect(2,Origin-X*(width/2.0),-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));

  FixedComp::setConnect(3,Origin+X*(width/2.0),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));

  FixedComp::setConnect(4,Origin-Z*(height/2.0),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(5,Origin+Z*(height/2.0),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));


  // FixedComp::setLinkSurf(6,-SMap.realSurf(buildIndex+32));
  // const Geometry::Plane* PPtr=SMap.realPtr<Geometry::Plane>(buildIndex+32);
  // FixedComp::setConnect(6,SurInter::getLinePoint(Origin,-Y,PPtr),-Y);

  SurfMap::createLink("innerBack", *this, 6, Origin, -Y);

  return;
}

void
DiffPumpXIADP03::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("DiffPumpXIADP03","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // constructSystem
