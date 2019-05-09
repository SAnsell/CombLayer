/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeam/Octupole.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include "surfDIter.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "SurInter.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "SimProcess.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ExternalCut.h" 
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h" 

#include "Octupole.h"

namespace xraySystem
{

Octupole::Octupole(const std::string& Key) :
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  baseName(Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

Octupole::Octupole(const std::string& Base,
		   const std::string& Key) : 
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  baseName(Base)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Base :: Base KeyName
    \param Key :: KeyName
  */
{}


Octupole::~Octupole() 
  /*!
    Destructor
  */
{}

void
Octupole::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("Octupole","populate");

  FixedRotate::populate(Control);
  
  length=Control.EvalPair<double>(keyName,baseName,"Length");

  frameRadius=Control.EvalPair<double>(keyName,baseName,"FrameRadius");
  frameThick=Control.EvalPair<double>(keyName,baseName,"FrameThick");

  poleGap=Control.EvalPair<double>(keyName,baseName,"PoleGap");
  poleRadius=Control.EvalPair<double>(keyName,baseName,"PoleRadius");
  poleWidht=Control.EvalPair<double>(keyName,baseName,"PoleWidth");

  coilWidth=Control.EvalPair<double>(keyName,baseName,"CoilWidth");
  
  poleMat=ModelSupport::EvalMat<int>(Control,keyName+"PoleMat",
				       baseName+"PoleMat");
  coreMat=ModelSupport::EvalMat<int>(Control,keyName+"CoreMat",
				       baseName+"CoreMat");
  coilMat=ModelSupport::EvalMat<int>(Control,keyName+"CoilMat",
				       baseName+"CoilMat");
  frameMat=ModelSupport::EvalMat<int>(Control,keyName+"FrameMat",
				       baseName+"FrameMat");

  return;
}

void
Octupole::createUnitVector(const attachSystem::FixedComp& FC,
    	                     const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: FixedComp to attach to
    \param sideIndex :: Link point
  */
{
  ELog::RegMethod RegA("Octupole","createUnitVector");
  
  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  return;
}

void
Octupole::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("Octupole","createSurface");

  // mid line
  ModelSupport::buildPlane(SMap,buildIndex+1000,Origin,X);
  ModelSupport::buildPlane(SMap,buildIndex+2000,Origin,Z);
  
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length/2.0),Y);


  int CN(buildIndex+11);
  
  
  
  Geometry::Vec3D QY;
  double angle(M_PI*poleYAngle/180.0);
  for(size_t i=0;i<8;i++)
    {
      const Geometry::Vec3D QX=X*cos(angle)+Z*sin(angle);
      const Geometry::Vec3D QY=X*cos(angle)+Z*sin(angle);
      

  
  ModelSupport::buildPlane(SMap,buildIndex+13,
			   Origin-X*(frameThick+width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,
			   Origin+X*(frameThick+width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,
			   Origin-Z*(frameThick+height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,
			   Origin+Z*(frameThick+height/2.0),Z);


  // coils:
  ModelSupport::buildPlane(SMap,buildIndex+101,Origin-Y*(coilLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+102,Origin+Y*(coilLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+103,Origin-X*coilWidth,X);
  ModelSupport::buildPlane(SMap,buildIndex+104,Origin+X*coilWidth,X);
  ModelSupport::buildPlane(SMap,buildIndex+105,Origin-Z*(vertGap/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+106,Origin+Z*(vertGap/2.0),Z);
  // corners:
  const Geometry::Vec3D LFACent
    (Origin-X*(coilWidth-coilCornerRad)-Y*(coilLength/2.0-coilCornerRad));
  const Geometry::Vec3D LFBCent
    (Origin-X*coilCornerRad-Y*(coilLength/2.0-coilCornerRad));
  const Geometry::Vec3D LBACent
    (Origin-X*(coilWidth-coilCornerRad)+Y*(coilLength/2.0-coilCornerRad));
  const Geometry::Vec3D LBBCent
    (Origin-X*coilCornerRad+Y*(coilLength/2.0-coilCornerRad));

  const Geometry::Vec3D RFACent
    (Origin+X*(coilWidth-coilCornerRad)-Y*(coilLength/2.0-coilCornerRad));
  const Geometry::Vec3D RFBCent
    (Origin+X*coilCornerRad-Y*(coilLength/2.0-coilCornerRad));
  const Geometry::Vec3D RBACent
    (Origin+X*(coilWidth-coilCornerRad)+Y*(coilLength/2.0-coilCornerRad));
  const Geometry::Vec3D RBBCent
    (Origin+X*coilCornerRad+Y*(coilLength/2.0-coilCornerRad));

  
  ModelSupport::buildPlane(SMap,buildIndex+111,LFACent,Y);
  ModelSupport::buildPlane(SMap,buildIndex+112,LBACent,Y);

  ModelSupport::buildPlane(SMap,buildIndex+113,LFACent,X);
  ModelSupport::buildPlane(SMap,buildIndex+114,LFBCent,X);

  ModelSupport::buildPlane(SMap,buildIndex+133,RFACent,X);
  ModelSupport::buildPlane(SMap,buildIndex+134,RFBCent,X);

  ModelSupport::buildCylinder(SMap,buildIndex+117,LFACent,Z,coilCornerRad);
  ModelSupport::buildCylinder(SMap,buildIndex+127,LFBCent,Z,coilCornerRad);
  ModelSupport::buildCylinder(SMap,buildIndex+118,LBACent,Z,coilCornerRad);
  ModelSupport::buildCylinder(SMap,buildIndex+128,LBBCent,Z,coilCornerRad);
  
  ModelSupport::buildCylinder(SMap,buildIndex+137,RFACent,Z,coilCornerRad);
  ModelSupport::buildCylinder(SMap,buildIndex+147,RFBCent,Z,coilCornerRad);
  ModelSupport::buildCylinder(SMap,buildIndex+138,RBACent,Z,coilCornerRad);
  ModelSupport::buildCylinder(SMap,buildIndex+148,RBBCent,Z,coilCornerRad);


  // Pole pieces  
  // gap to centre:
  const double ang[]={poleYAngle,180-poleYAngle,-poleYAngle,180.0+poleYAngle};
  const double CGap(poleRadius-
		    sqrt(poleRadius*poleRadius-poleWidth*poleWidth/4.0));
	  
  ModelSupport::buildPlane(SMap,buildIndex+201,Origin-Y*(poleLength/2.0),Y);
  addSurf("FrontQuadPole",SMap.realSurf(buildIndex+201));
  ModelSupport::buildPlane(SMap,buildIndex+202,Origin+Y*(poleLength/2.0),Y);
  addSurf("BackQuadPole",-SMap.realSurf(buildIndex+202));
  
  int CI(buildIndex+200);
  for(size_t i=0;i<2;i++)
    {
      const Geometry::Vec3D ZOrg((!i) ? Origin-Z*poleZStep : Origin+Z*poleZStep);
      for(size_t j=0;j<2;j++)
	{
	  // centre of flat part
	  const Geometry::Quaternion QR=
	    Geometry::Quaternion::calcQRotDeg(ang[i*2+j],Y);
	  
	  Geometry::Vec3D AX(X);
	  Geometry::Vec3D AZ(Z);
	  QR.rotate(AX);
	  QR.rotate(AZ);
	  
	  Geometry::Vec3D fPoint(ZOrg+AX*(CGap+poleStep));
	  // centre of cylinder
	  Geometry::Vec3D cPoint(ZOrg+AX*(poleStep+poleRadius));
	  
	  ModelSupport::buildPlane(SMap,CI+3,ZOrg-AZ*(poleWidth/2.0),AZ);
	  ModelSupport::buildPlane(SMap,CI+4,ZOrg+AZ*(poleWidth/2.0),AZ);
	  ModelSupport::buildCylinder(SMap,CI+7,cPoint,Y,poleRadius);
	  ModelSupport::buildPlane(SMap,CI+6,fPoint,AX);
	  CI+=100;
	}
    }
  return;
}

void
Octupole::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("Octupole","createObjects");

  std::string Out;
  // Outer steel
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "1 -2 13 -14 15 -16 (-3:4:-5:6) ");
  makeCell("Frame",System,cellIndex++,frameMat,0.0,Out);

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "101 -1 13 -14 15 -16 (-3:4:-5:6) ");
  makeCell("FFrame",System,cellIndex++,0,0.0,Out);
  
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "-102 2 13 -14 15 -16 (-3:4:-5:6) ");
  makeCell("BFrame",System,cellIndex++,0,0.0,Out);

  std::string TB[2];
  TB[0]=ModelSupport::getComposite(SMap,buildIndex," -105 5 ");
  TB[1]=ModelSupport::getComposite(SMap,buildIndex," 106 -6 ");
  for(size_t i=0;i<2;i++)
    {
      // Left  coil
      Out=ModelSupport::getComposite(SMap,buildIndex,
				     "101 -102 103 -1000 "
				     "(-117 : 111 : 113) "
				     "(-118 : -112 : 113) "
				     "(-127 : 111 : -114) "
				     "(-128 : -112 : -114) ");
      makeCell("CoilLowerLeft",System,cellIndex++,coilMat,0.0,Out+TB[i]);
      
      // corners for coils
      Out=ModelSupport::getComposite(SMap,buildIndex,"103 101 117 -111 -113 ");
      makeCell("CoilLLCorner",System,cellIndex++,0,0.0,Out+TB[i]);
      Out=ModelSupport::getComposite(SMap,buildIndex,"103 -102 118 112 -113 ");
      makeCell("CoilLLCorner",System,cellIndex++,0,0.0,Out+TB[i]);
      Out=ModelSupport::getComposite(SMap,buildIndex,"-1000 101 127 -111 114 ");
      makeCell("CoilLLCorner",System,cellIndex++,0,0.0,Out+TB[i]);
      Out=ModelSupport::getComposite(SMap,buildIndex,
				     "-1000 -102 128  112 114 ");
      makeCell("CoilLLCorner",System,cellIndex++,0,0.0,Out+TB[i]);
      
      // Right Lower coil
      Out=ModelSupport::getComposite(SMap,buildIndex,
				     "101 -102 -104 1000  "
				     "(-137 : 111 : -133) "
				     "(-138 : -112 : -133) "
				     "(-147 : 111 : 134) "
				     "(-148 : -112 : 134) ");
      makeCell("CoilLowerLeft",System,cellIndex++,coilMat,0.0,Out+TB[i]);
  
      // corners for coils
      Out=ModelSupport::getComposite(SMap,buildIndex,
				     "-104 101 137 -111 133 ");
      makeCell("CoilLRCorner",System,cellIndex++,0,0.0,Out+TB[i]);
      Out=ModelSupport::getComposite(SMap,buildIndex,
				     "-104 -102 138 112 133 ");
      makeCell("CoilLRCorner",System,cellIndex++,0,0.0,Out+TB[i]);
      Out=ModelSupport::getComposite(SMap,buildIndex,
				     "1000 101 147 -111 -134 ");
      makeCell("CoilLRCorner",System,cellIndex++,0,0.0,Out+TB[i]);
      Out=ModelSupport::getComposite(SMap,buildIndex,
				     "1000 -102 148  112 -134 ");
      makeCell("CoilLRCorner",System,cellIndex++,0,0.0,Out+TB[i]);

    }
  // Now make edge voids:
  Out=ModelSupport::getComposite(SMap,buildIndex,"101 -102 3 -103 5 -6 ");
  makeCell("LEdge",System,cellIndex++,0,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex,"101 -102 104 -4 5 -6 ");
  makeCell("REdge",System,cellIndex++,0,0.0,Out);

  // Pole Pieces
  const std::string ICell=innerTube.display();
  
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "105 201 -202 203 -204 (206:-207) ");
  makeCell("Pole",System,cellIndex++,poleMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "105 -2000 201 -202 1000 -104  (-203:204:(-206  207) )");
  makeCell("VoidPoleA",System,cellIndex++,0,0.0,Out+ICell);
  
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "105 201 -202 303 -304 (306:-307) ");
  makeCell("Pole",System,cellIndex++,poleMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "105 -2000 201 -202 -1000 103  (-303:304:(-306  307) )");
  makeCell("VoidPoleB",System,cellIndex++,0,0.0,Out+ICell);

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "-106 201 -202 403 -404 (406:-407) ");
  makeCell("Pole",System,cellIndex++,poleMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "-106 2000 201 -202 1000 -104  (-403:404:(-406  407) )");
  makeCell("VoidPoleC",System,cellIndex++,0,0.0,Out+ICell);
  
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "-106 201 -202 503 -504 (506:-507) ");
  makeCell("Pole",System,cellIndex++,poleMat,0.0,Out);
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "-106 2000 201 -202 -1000 103  (-503:504:(-506  507) )");
  makeCell("VoidPoleD",System,cellIndex++,0,0.0,Out+ICell);

  if (poleLength<coilLength-Geometry::zeroTol)
    {
      Out=ModelSupport::getComposite
	(SMap,buildIndex,"105 -106 202 -102 103 -104 ");
      makeCell("ExtraPoleVoidA",System,cellIndex++,0,0.0,Out+ICell);
      Out=ModelSupport::getComposite
	(SMap,buildIndex,"105 -106 101 -201 103 -104 ");
      makeCell("ExtraPoleVoidB",System,cellIndex++,0,0.0,Out+ICell);
    }
  Out=ModelSupport::getComposite(SMap,buildIndex,"101 -102 13 -14 15 -16");  
  addOuterSurf(Out);      

  return;
}

void 
Octupole::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("Octupole","createLinks");

  FixedComp::setConnect(0,Origin-Y*(coilLength/2.0),-Y);     
  FixedComp::setConnect(1,Origin+Y*(coilLength/2.0),Y);     
  FixedComp::setConnect(2,Origin-X*(width/2.0),-X);     
  FixedComp::setConnect(3,Origin+X*(width/2.0),X);     
  FixedComp::setConnect(4,Origin-Z*(height/2.0),-Z);     
  FixedComp::setConnect(5,Origin+Z*(height/2.0),Z);     

  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+101));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+102));
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));
  
  return;
}

void
Octupole::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track 
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("Octupole","createAll");
  
  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE epbSystem
