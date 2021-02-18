/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/CryoTransferLine.cxx
 *
 * Copyright (c) 2018-2021 by Konstantin Batkov
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
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "CryoTransferLine.h"

namespace essSystem
{

CryoTransferLine::CryoTransferLine(const std::string& Key)  :
  attachSystem::ContainedGroup(),
  attachSystem::FixedOffset(Key,15),
  attachSystem::CellMap(),
  attachSystem::FrontBackCut()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

CryoTransferLine::CryoTransferLine(const CryoTransferLine& A) :
  attachSystem::ContainedGroup(A),
  attachSystem::FixedOffset(A),
  attachSystem::CellMap(A),
  attachSystem::FrontBackCut(A),
  length(A.length),width(A.width),height(A.height),
  widthHall(A.widthHall),
  wallThick(A.wallThick),
  roofThick(A.roofThick),
  nDucts(A.nDucts),
  mainMat(A.mainMat),wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: CryoTransferLine to copy
  */
{}

CryoTransferLine&
CryoTransferLine::operator=(const CryoTransferLine& A)
  /*!
    Assignment operator
    \param A :: CryoTransferLine to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedGroup::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::FrontBackCut::operator=(A);

      length=A.length;
      width=A.width;
      height=A.height;
      wallThick=A.wallThick;
      widthHall=A.widthHall;
      roofThick=A.roofThick;
      nDucts=A.nDucts;
      mainMat=A.mainMat;
      wallMat=A.wallMat;
    }
  return *this;
}

CryoTransferLine*
CryoTransferLine::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new CryoTransferLine(*this);
}

CryoTransferLine::~CryoTransferLine()
  /*!
    Destructor
  */
{}

void
CryoTransferLine::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("CryoTransferLine","populate");

  FixedOffset::populate(Control);

  const size_t Nlegs(5);
  for (size_t i=0; i<Nlegs-1; i++)
    {
      const double L = Control.EvalVar<double>(keyName+"Length"+
					       std::to_string(i+1));
      length.push_back(L);
    }

  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  roofThick=Control.EvalVar<double>(keyName+"RoofThick");

  nDucts=Control.EvalDefVar<size_t>(keyName+"NDucts",0);
  widthHall=Control.EvalVar<double>(keyName+"WidthHall");

  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  return;
}

void
CryoTransferLine::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("CryoTransferLine","createSurfaces");

  //Rotation for the slanted walls
  const Geometry::Quaternion QR=
    Geometry::Quaternion::calcQRotDeg(-45.0,Z);
  Geometry::Vec3D AX(X);
  QR.rotate(AX);

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(width/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(width/2.0),Y);

  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(length[0]),X);

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

  //2nd leg
  ModelSupport::buildShiftedPlane(SMap,buildIndex+101,SMap.realPtr<Geometry::Plane>(buildIndex+1),+length[1]);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+104,SMap.realPtr<Geometry::Plane>(buildIndex+4),-widthHall);
  //Slanted wall between 1st and 2nd leg
  Geometry::Vec3D fPoint(length[0]-widthHall-50.0,0.5*width+50,0);
  ModelSupport::buildPlane(SMap,buildIndex+103,fPoint+Origin,AX);
  //std::cout << fPoint[0] << " " << fPoint[1] << " " << fPoint[2] << std::endl;
  //std::cout << Origin[0] << " " << Origin[1] << " " << Origin[2] << std::endl;
  //3rd Leg
  ModelSupport::buildShiftedPlane(SMap,buildIndex+201,SMap.realPtr<Geometry::Plane>(buildIndex+101),-widthHall);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+204,SMap.realPtr<Geometry::Plane>(buildIndex+104),+length[2]);
  //4th leg
  ModelSupport::buildShiftedPlane(SMap,buildIndex+301,SMap.realPtr<Geometry::Plane>(buildIndex+201),+length[3]);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+304,SMap.realPtr<Geometry::Plane>(buildIndex+204),-widthHall);
  //Slanted wall between 3rd and 4th leg
  Geometry::Vec3D fPoint2(length[2]-2*widthHall+length[0]-100.0,length[1]+100.0-0.5*width,0);
  ModelSupport::buildPlane(SMap,buildIndex+303,fPoint2+Origin,AX);

  //----------------------------For the concrete walls and ceiling

  
  // THIS IS IMCOMPREHENSIBLE -- avoid shifted plane unless the base surface is
  // from an external source. In that case use a DIRECTIONAL Shifted Plane

  ModelSupport::buildShiftedPlane
    (SMap,buildIndex+7,SMap.realPtr<Geometry::Plane>(buildIndex+1),-wallThick-20.0); //additional 20 cm
  ModelSupport::buildShiftedPlane
    (SMap,buildIndex+8,SMap.realPtr<Geometry::Plane>(buildIndex+2),+wallThick+20.0);
  ModelSupport::buildShiftedPlane
    (SMap,buildIndex+10,SMap.realPtr<Geometry::Plane>(buildIndex+4),+wallThick);

  ModelSupport::buildPlane(SMap,buildIndex+11,Origin-Z*(height/2.0+roofThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Z*(height/2.0+roofThick),Z);

  // THIS IS IMCOMPREHENSIBLE -- avoid shifted plane unless the base surface is
  // from an external source. In that case use a DIRECTIONAL Shifted Plane
  
  //2nd leg 
  ModelSupport::buildShiftedPlane(SMap,buildIndex+107,
				  SMap.realPtr<Geometry::Plane>(buildIndex+101),+wallThick);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+110,
				  SMap.realPtr<Geometry::Plane>(buildIndex+104),-wallThick);
  //Slanted wall between 1st and 2nd leg
  ModelSupport::buildShiftedPlane(SMap,buildIndex+109,
				  SMap.realPtr<Geometry::Plane>(buildIndex+103),-wallThick-20.0);
  //3rd Leg
  ModelSupport::buildShiftedPlane(SMap,buildIndex+207,
				  SMap.realPtr<Geometry::Plane>(buildIndex+201),-wallThick);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+210,
				  SMap.realPtr<Geometry::Plane>(buildIndex+204),+wallThick);
  //4th leg
  ModelSupport::buildShiftedPlane(SMap,buildIndex+307,
				  SMap.realPtr<Geometry::Plane>(buildIndex+301),+wallThick);
  ModelSupport::buildShiftedPlane(SMap,buildIndex+310,
				  SMap.realPtr<Geometry::Plane>(buildIndex+304),-wallThick);
  //Slanted wall between 3rd and 4th leg
  ModelSupport::buildShiftedPlane(SMap,buildIndex+309,
				  SMap.realPtr<Geometry::Plane>(buildIndex+303),-wallThick);
 
  return;
}

void
CryoTransferLine::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("CryoTransferLine","createObjects");

  // Seriously FULL should be a general wrap-around all the components
  // it can be 10 surfaces and doesnt need to be 200.
  
  
  std::string Out;
  attachSystem::ContainedGroup::addCC("Full");
  
  attachSystem::ContainedGroup::addCC("Leg1");

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 -4 5 -6 ")+backRule();
  makeCell("Leg1",System,cellIndex++,mainMat,0.0,Out);
    addOuterSurf("Leg1",Out);
  addOuterUnionSurf("Full",Out);
  //2nd leg
  attachSystem::ContainedGroup::addCC("Leg2");
  Out=ModelSupport::getComposite(SMap,buildIndex," 2 -101 104 -4 5 -6 ");
  makeCell("Leg2",System,cellIndex++,mainMat,0.0,Out);
  addOuterUnionSurf("Leg2",Out);
  addOuterUnionSurf("Full",Out);

  //Slanted wall 1st and 2nd leg
  attachSystem::ContainedGroup::addCC("SlantLeg2");
  Out=ModelSupport::getComposite(SMap,buildIndex," 2 103 -104 5 -6 ");
  makeCell("SlantLeg12",System,cellIndex++,mainMat,0.0,Out);
  addOuterUnionSurf("SlantLeg2",Out);
  addOuterUnionSurf("Full",Out);

  //3rd leg
  attachSystem::ContainedGroup::addCC("Leg3");
  Out=ModelSupport::getComposite(SMap,buildIndex," 201 -101 4 -204 5 -6 ");
  makeCell("Leg3",System,cellIndex++,mainMat,0.0,Out);
  addOuterUnionSurf("Leg3",Out);
  addOuterUnionSurf("Full",Out);

  //4th Leg
  attachSystem::ContainedGroup::addCC("Leg4");
  Out=ModelSupport::getComposite(SMap,buildIndex," 101 -301 304 -204 5 -6 ");
  makeCell("Leg4",System,cellIndex++,mainMat,0.0,Out);
  addOuterUnionSurf("Leg4",Out);
  addOuterUnionSurf("Full",Out);

  //Slanted wall 3rd and 4th leg
  attachSystem::ContainedGroup::addCC("SlantLeg4");
  Out=ModelSupport::getComposite(SMap,buildIndex," 101 303 -304 5 -6 ");
  makeCell("SlantLeg34",System,cellIndex++,mainMat,0.0,Out);
  addOuterUnionSurf("SlantLeg4",Out);
  addOuterUnionSurf("Full",Out);
  
  //--------------Concrete Side walls
  Out=ModelSupport::getComposite(SMap,buildIndex," 7 -8 -10 11 -12 (-1:2:4) (-2:-104:8:4) (-103:-2:4)")+backRule();
  makeCell("Leg1",System,cellIndex++,wallMat,0.0,Out);
  addOuterUnionSurf("Leg1",Out);
  addOuterUnionSurf("Full",Out);
  //2nd leg
  Out=ModelSupport::getComposite(SMap,buildIndex," 8 -107 110 -10 11 -12 (-104:4:101) (-201:101:-4:10) (-103:-2:4)");
  makeCell("Leg2",System,cellIndex++,wallMat,0.0,Out);
  addOuterUnionSurf("Full",Out);

  //Slanted wall 1st and 2nd leg
  Out=ModelSupport::getComposite(SMap,buildIndex," 8 109 -110 11 -12 (-103:-2:4)");
  makeCell("SlantLeg12",System,cellIndex++,wallMat,0.0,Out);
  addOuterUnionSurf("Full",Out);
  
  //3rd leg
  Out=ModelSupport::getComposite(SMap,buildIndex," 207 -107 10 -210 11 -12 (-10:-201:101:204) (204:-101:107:-303) ");
  makeCell("Leg3",System,cellIndex++,wallMat,0.0,Out);
  addOuterUnionSurf("Full",Out);
  
  //4th Leg
  Out=ModelSupport::getComposite(SMap,buildIndex," 107 -307 310 -210 11 -12 (-107:-304:204:301) (-303:-101:304)");
  makeCell("Leg4",System,cellIndex++,wallMat,0.0,Out);
  addOuterUnionSurf("Full",Out);
  
  //Slanted wall 3rd and 4th leg
  Out=ModelSupport::getComposite(SMap,buildIndex," 107 309 -310 11 -12 (-303:-101:304)");
  makeCell("SlantLeg34",System,cellIndex++,wallMat,0.0,Out);
  addOuterUnionSurf("Full",Out);

  //--------------Roof and floor

  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 -4 11 -12 (-1:2:4:-5:6)")+backRule();
  makeCell("Leg1",System,cellIndex++,wallMat,0.0,Out);
  addOuterUnionSurf("Leg1",Out);
  addOuterUnionSurf("Full",Out);
  //2nd leg
  Out=ModelSupport::getComposite(SMap,buildIndex," 2 -101 104 -4 11 -12 (-2:101:-104:4:-5:6)");
  makeCell("Leg2",System,cellIndex++,wallMat,0.0,Out);
  addOuterUnionSurf("Full",Out);

  //Slanted wall 1st and 2nd leg
  Out=ModelSupport::getComposite(SMap,buildIndex," 2 103 -104 11 -12 (-2:-103:104:-5:6)");
  makeCell("SlantLeg12",System,cellIndex++,wallMat,0.0,Out);
  addOuterUnionSurf("Full",Out);

  //3rd leg
  Out=ModelSupport::getComposite(SMap,buildIndex," 201 -101 4 -204 11 -12 (-201:101:-4:204:-5:6) ");
  makeCell("Leg3",System,cellIndex++,wallMat,0.0,Out);
  addOuterUnionSurf("Full",Out);

  //4th Leg
  Out=ModelSupport::getComposite(SMap,buildIndex," 101 -301 304 -204 11 -12 (-101:301:-304:204:-5:6)");
  makeCell("Leg4",System,cellIndex++,wallMat,0.0,Out);
  addOuterUnionSurf("Full",Out);

  //Slanted wall 3rd and 4th leg
  Out=ModelSupport::getComposite(SMap,buildIndex," 101 303 -304 11 -12 (-101:-303:304:-5:6)");
  makeCell("SlateWall",System,cellIndex++,wallMat,0.0,Out);
  addOuterUnionSurf("Full",Out);
  
  
  
  return;
}


void
CryoTransferLine::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("CryoTransferLine","createLinks");

  // this creates first two links:
  FrontBackCut::createLinks(*this, Origin, X);

  // Leg 1
  FixedComp::setConnect(2,Origin+X*(length[0]+wallThick),X);
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+14));

  FixedComp::setConnect(3,Origin+X*(length[0]-height)-Y*(width/2.0+wallThick),-Y);
  FixedComp::setLinkSurf(3,-SMap.realSurf(buildIndex+11));

  FixedComp::setConnect(4,Origin+X*(length[0]-height)+Y*(width/2.0+wallThick),Y);
  FixedComp::setLinkSurf(4,SMap.realSurf(buildIndex+12));

  FixedComp::setConnect(5,Origin+X*(length[0]-height)-Z*(height/2.0+wallThick),-Z);
  FixedComp::setLinkSurf(5,-SMap.realSurf(buildIndex+15));

  FixedComp::setConnect(6,Origin+X*(length[0]-height-wallThick*2)+Z*(height/2.0+wallThick),Z);
  FixedComp::setLinkSurf(6,SMap.realSurf(buildIndex+16));
  return;
}




void
CryoTransferLine::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("CryoTransferLine","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // essSystem
