/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Linac/LQuadH.cxx
 *
 * Copyright (c) 2004-2022 by Konstantin Batkov / Stuart Ansell
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
#include "Vec3D.h"
#include "Quaternion.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h"

#include "LQuadF.h"
#include "LQuadH.h"

namespace tdcSystem
{

LQuadH::LQuadH(const std::string& Key) :
  tdcSystem::LQuadF(Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

LQuadH::LQuadH(const std::string& Base,
	       const std::string& Key) :
  tdcSystem::LQuadF(Base,Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Base :: Base KeyName
    \param Key :: KeyName
  */
{}


LQuadH::~LQuadH()
  /*!
    Destructor
  */
{}

void
LQuadH::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("LQuadH","populate");

  tdcSystem::LQuadF::populate(Control);
  polePitch=Control.EvalDefTail<double>(keyName,baseName,"PolePitch",26.0);

  return;
}


void
LQuadH::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("LQuadH","createSurface");

  tdcSystem::LQuadF::createSurfaces();

  const size_t NPole(4);

  const Geometry::Quaternion coilQCut=
    Geometry::Quaternion::calcQRotDeg(coilAngle,Y);

  int CN(buildIndex+2000);
  double angle(M_PI*poleYAngle/180.0);
  double pitchAngle(M_PI*polePitch/180.0);

  for(size_t i=0;i<NPole;i++)
    {
      const Geometry::Vec3D QR=X*cos(angle)+Z*sin(angle);
      const Geometry::Vec3D QX=X*cos(M_PI/2.0+angle)+Z*sin(M_PI/2.0+angle);
      const Geometry::Vec3D pitchQXMinus=
	X*cos(M_PI/2.0+angle-pitchAngle)+Z*sin(M_PI/2.0+angle-pitchAngle);
      const Geometry::Vec3D pitchQXPlus=
	X*cos(M_PI/2.0+angle+pitchAngle)+Z*sin(M_PI/2.0+angle+pitchAngle);

      ModelSupport::buildPlane(SMap,CN+213,Origin+QR*(coilRadius)-QX*(poleWidth/2.0),pitchQXMinus);
      ModelSupport::buildPlane(SMap,CN+214,Origin+QR*(coilRadius)+QX*(poleWidth/2.0),pitchQXPlus);

      angle+=2.0*M_PI/static_cast<double>(NPole);
      CN+=20;
    }

  return;
}

void
LQuadH::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("LQuadH","createObjects");
  const size_t NPole(4);

  HeadRule HR;
  std::string unitStr;

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -12 3 -4 5 -6");
  addOuterSurf(HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 3 -4 5 -6");
  unitStr=ModelSupport::getSeqUnion(1,static_cast<int>(2*NPole),1);
  HR*=ModelSupport::getHeadRule(SMap,buildIndex+1000,unitStr);
  makeCell("Yoke",System,cellIndex++,yokeMat,0.0,HR);


  const HeadRule ICellHR=getRule("Inner");
  /// plane front / back
  const HeadRule fbHR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2");
  const HeadRule frontSurf(SMap,buildIndex,-1);
  const HeadRule backSurf(SMap,buildIndex,2);
  const HeadRule frontRegion=ModelSupport::getHeadRule(SMap,buildIndex,"11 -1");
  const HeadRule backRegion=ModelSupport::getHeadRule(SMap,buildIndex,"2 -12");

  std::vector<HeadRule> CoilExclude;
  std::vector<HeadRule> PoleExclude;
  std::vector<HeadRule> frontExclude;
  std::vector<HeadRule> backExclude;

  int BN(buildIndex); // base
  int PN(buildIndex);
  for(size_t i=0; i<NPole;i++)
    {
      const HeadRule outerCut(SMap,BN,-1001);

      HR=ModelSupport::getHeadRule(SMap,PN,"2213 -2214 -2207 -2001");
      makeCell("PoleNose",System,cellIndex++,poleMat,0.0,HR*fbHR);
      PoleExclude.push_back(HR);

      HR=ModelSupport::getHeadRule(SMap,PN,"2203 -2204 2001");
      makeCell("PoleBase",System,cellIndex++,poleMat,0.0,HR*outerCut*fbHR);
      PoleExclude.back().addUnion(HR);

      HR=ModelSupport::getHeadRule
	(SMap,PN," 2003 -2004 2001 2013 2014 2017 2018 (-2203:2204)");
      makeCell("Coil",System,cellIndex++,coilMat,0.0,HR*outerCut*fbHR);
      HR=ModelSupport::getHeadRule
	(SMap,PN," 2003 -2004 2001 2013 2014 2017 2018");
      CoilExclude.push_back(HR);

      // Front extra pieces
      HR=ModelSupport::getHeadRule
	(SMap,PN,buildIndex,"2003 -2004 2001 2013 2014 2017 2018 -2009");
      makeCell("CoilFront",System,cellIndex++,coilMat,0.0,HR*frontSurf);
      frontExclude.push_back(HR);

      // back extra pieces
      HR=ModelSupport::getHeadRule
	(SMap,PN,buildIndex,"2003 -2004 2001 2013 2014 2017 2018 -2019");
      makeCell("CoilBack",System,cellIndex++,coilMat,0.0,HR*backSurf);
      backExclude.push_back(HR);

      BN+=2;
      PN+=20;
    }

  int aOffset(-1);
  int bOffset(0);
  PN=buildIndex;
  HeadRule hrA,hrB;
  const std::vector<std::string> sides({"-4 -6", "3 -6", "3 5", "-4 5"});
  for(size_t i=0;i<NPole;i++)
    {
      const HeadRule triCut(SMap,PN,2001);
      
      hrA=ModelSupport::getRangeHeadRule
	(SMap,501,504,bOffset,buildIndex,"501R -502R");
      hrB=ModelSupport::getRangeHeadRule
	(SMap,1001,1008,aOffset,buildIndex,"-1001R -1002R -1003R");

      makeCell("Triangle",System,cellIndex++,0,0.0,
	       hrA*hrB*fbHR*triCut*CoilExclude[i].complement());

      makeCell("InnerTri",System,cellIndex++,0,0.0,
	       hrA*fbHR*triCut.complement()*
	       PoleExclude[i].complement()*ICellHR);

      
      hrB=ModelSupport::getHeadRule(SMap,buildIndex,sides[i]);
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"11 -1");
      makeCell("FrontVoid",System,cellIndex++,0,0.0,
	       hrA*hrB*HR*frontExclude[i].complement()*ICellHR);

      HR=ModelSupport::getHeadRule(SMap,buildIndex,"2 -12");
      makeCell("BackVoid",System,cellIndex++,0,0.0,
	       hrA*hrB*HR*backExclude[i].complement()*ICellHR);

      aOffset+=2;
      bOffset+=1;
      PN+=20;
    }
  return;
}

void
LQuadH::createLinks()
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("LQuadH","createLinks");

  tdcSystem::LQuadF::createLinks();

  return;
}

void
LQuadH::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("LQuadH","createAll");

  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE tdcSystem
