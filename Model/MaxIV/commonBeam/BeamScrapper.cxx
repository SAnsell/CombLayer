/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeam/BeamScrapper.cxx
 *
 * Copyright (c) 2004-2023 Stuart Ansell
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
#include "Line.h"
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
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "Quaternion.h"

#include "Surface.h"

#include "BeamScrapper.h"

namespace xraySystem
{

BeamScrapper::BeamScrapper(const std::string& Key)  :
  attachSystem::ContainedGroup("Payload","Connect","Outer"),
  attachSystem::FixedRotate(Key,6),
  attachSystem::ExternalCut(),
  attachSystem::CellMap()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}


BeamScrapper::~BeamScrapper()
  /*!
    Destructor
  */
{}

void
BeamScrapper::calcImpactVector()
  /*!
    Calculate the impact points of the main beam  on the screen surface:
    We have the beamAxis this must intersect the screen and mirror closest to 
    their centre points. It DOES NOT need to hit the centre points as the mirror
    system is confined to moving down the Y axis of the object. 
    [-ve Y from flange  to beam centre]
  */
{
  ELog::RegMethod RegA("BeamScrapper","calcImpactVector");

  // defined points:

  // This point is the beam centre point between the main axis:

  std::tie(std::ignore,beamCentre)=
    beamAxis.closestPoints(Geometry::Line(Origin,Y));

  ELog::EM<<"PlateCentre == "<<Origin<<" : "<<X<<" : "
	  <<Y<<" : "<<Z<<ELog::endDiag;
  ELog::EM<<"PlateCentre == "<<beamCentre<<":"<<beamAxis<<ELog::endDiag;

  // Thread point
  plateCentre=beamCentre-Y*plateOffset;
    ELog::EM<<"NEW PlateCentre == "<<plateCentre<<ELog::endDiag;

  return;
}


void
BeamScrapper::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("BeamScrapper","populate");

  FixedRotate::populate(Control);

  tubeRadius=Control.EvalVar<double>(keyName+"TubeRadius");
  tubeOffset=Control.EvalVar<double>(keyName+"TubeOffset");
  tubeWall=Control.EvalVar<double>(keyName+"TubeWall");

  plateOffset=Control.EvalVar<double>(keyName+"PlateOffset");
  plateYStep=Control.EvalVar<double>(keyName+"PlateYStep");
  plateAngle=Control.EvalVar<double>(keyName+"PlateAngle");
  plateLength=Control.EvalVar<double>(keyName+"PlateLength");
  plateHeight=Control.EvalVar<double>(keyName+"PlateHeight");
  plateThick=Control.EvalVar<double>(keyName+"PlateThick");

  tubeWidth=Control.EvalVar<double>(keyName+"TubeWidth");
  tubeHeight=Control.EvalVar<double>(keyName+"TubeHeight");

  inletZOffset=Control.EvalVar<double>(keyName+"InletZOffset");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  waterMat=ModelSupport::EvalMat<int>(Control,keyName+"WaterMat");
  copperMat=ModelSupport::EvalMat<int>(Control,keyName+"CopperMat");

  return;
}

void
BeamScrapper::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("BeamScrapper","createSurfaces");

  calcImpactVector();

  // Ring box outer [note extra for tubewall clearance]:
  const double outerTube(tubeRadius+1.02*tubeWall);

  const Geometry::Quaternion QP=
    Geometry::Quaternion::calcQRotDeg(plateAngle,Z);
  const Geometry::Vec3D PX=QP.makeRotate(X);
  const Geometry::Vec3D PY=QP.makeRotate(Y);
  const Geometry::Vec3D PZ=QP.makeRotate(Z);
  
  if (!isActive("FlangePlate"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
      ExternalCut::setCutSurf("FlangePlate",SMap.realSurf(buildIndex+1));
    }

  const Geometry::Vec3D inletCentre=beamCentre+Z*inletZOffset;
  
  ModelSupport::buildCylinder
    (SMap,buildIndex+7,inletCentre-X*tubeOffset,Y,tubeRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+8,inletCentre+X*tubeOffset,Y,tubeRadius);

  ModelSupport::buildCylinder
    (SMap,buildIndex+17,inletCentre-X*tubeOffset,Y,tubeRadius+tubeWall);
  ModelSupport::buildCylinder
    (SMap,buildIndex+18,inletCentre+X*tubeOffset,Y,tubeRadius+tubeWall);

  // container cylinder
  ModelSupport::buildCylinder
    (SMap,buildIndex+107,inletCentre,Y,tubeOffset+outerTube);
  
  // screen+mirror thread
  ModelSupport::buildPlane(SMap,buildIndex+201,plateCentre,PY);

  const Geometry::Vec3D inletCentreA=beamCentre
    -X*tubeOffset
    -Y*(plateOffset+tubeOffset*tan(M_PI*plateAngle/180.0))
    +Z*inletZOffset;

  const Geometry::Vec3D inletCentreB=beamCentre
    +X*tubeOffset
    -Y*(plateOffset-tubeOffset*tan(M_PI*plateAngle/180.0))
    +Z*inletZOffset;

  ELog::EM<<"Main == "<<Origin<<":"<<X<<":"<<Y<<":"<<Z<<ELog::endDiag;
  ELog::EM<<"Main == "<<beamCentre-Origin<<":"<<PX<<":"<<PY<<":"<<PZ<<ELog::endDiag;
  
  // 45 centre bend 
  const Geometry::Vec3D inletA=inletCentreA+
    PZ*(tubeHeight-tubeOffset/sqrt(2.0));
  const Geometry::Vec3D inletB=inletCentreB+
    PZ*(tubeHeight-tubeOffset/sqrt(2.0));

  const Geometry::Vec3D centreA=inletCentreA+
    PZ*tubeHeight+PX*(tubeOffset/sqrt(2.0));
  const Geometry::Vec3D centreB=inletCentreB+
    PZ*tubeHeight-PX*(tubeOffset/sqrt(2.0));

  // ordered list of the above
  const std::vector<Geometry::Vec3D> chain
    ({
      inletCentreA-Y,
      inletCentreA,inletA,centreA,
      centreB,inletB,inletCentreB,
      inletCentreB-Y
    });

  int BI(buildIndex+1000);
  for(size_t i=1;i<chain.size()-1;i++)
    {
      const Geometry::Vec3D aAxis=(chain[i]-chain[i-1]).unit();
      const Geometry::Vec3D bAxis=(chain[i+1]-chain[i]).unit();
      // divide centre axis
      const Geometry::Vec3D dAxis=(bAxis+aAxis)/2.0;

      ModelSupport::buildPlane(SMap,BI+1,chain[i],dAxis);
      if (i+2!=chain.size())
	{
	  ModelSupport::buildCylinder(SMap,BI+7,chain[i],bAxis,tubeRadius);
	  ModelSupport::buildCylinder
	    (SMap,BI+8,chain[i],bAxis,tubeRadius+tubeWall);
	}
      BI+=10;
    }  
  
  // main plate
  const Geometry::Vec3D mainC=plateCentre+PY*(plateYStep+plateThick/2.0);
  
  ModelSupport::buildPlane(SMap,buildIndex+2001,mainC-PY*(plateThick/2.0),PY);
  ModelSupport::buildPlane(SMap,buildIndex+2002,mainC+PY*(plateThick/2.0),PY);
  ModelSupport::buildPlane(SMap,buildIndex+2003,mainC-PX*(plateLength/2.0),PX);
  ModelSupport::buildPlane(SMap,buildIndex+2004,mainC+PX*(plateLength/2.0),PX);
  ModelSupport::buildPlane(SMap,buildIndex+2005,mainC-PZ*(plateHeight/2.0),PZ);
  ModelSupport::buildPlane(SMap,buildIndex+2006,mainC+PZ*(plateHeight/2.0),PZ);

  // vertical for payload

  ModelSupport::buildPlane(SMap,buildIndex+2101,inletCentreA-PY*outerTube,PY);
  ModelSupport::buildPlane(SMap,buildIndex+2105,inletCentre-PZ*outerTube,PZ);
  ModelSupport::buildPlane(SMap,buildIndex+2106,centreA+PZ*outerTube,PZ);

  return;
}

void
BeamScrapper::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("BeamScrapper","createObjects");

  const HeadRule plateHR=getRule("FlangePlate");

  HeadRule HR,HRBox;

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1001 -7");
  makeCell("supplyWater",System,cellIndex++,waterMat,0.0,HR*plateHR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-1001 -17 7");
  makeCell("supportPipe",System,cellIndex++,copperMat,0.0,HR*plateHR);

  // NOTE: 1001 is a y/z cut plane used as a vertical diagonal cutter
  HRBox=ModelSupport::getHeadRule(SMap,buildIndex,
				  "2101 -2002 2003 -2004 2105 -2106");
  addOuterSurf("Payload",HRBox);


  std::vector<HeadRule> pipes;
  int BI(buildIndex+1000);
  for(size_t i=0;i<5;i++)
    {
      HR=ModelSupport::getHeadRule(SMap,BI,"1 -11 -7");
      makeCell("supplyWater",System,cellIndex++,waterMat,0.0,HR);
      HR=ModelSupport::getHeadRule(SMap,BI,"1 -11 7 -8");
      makeCell("supplyPipe",System,cellIndex++,copperMat,0.0,HR);
      HR=ModelSupport::getHeadRule(SMap,BI,"1 -11 -8");
      pipes.push_back(HR.complement());
      BI+=10;
    }

  HR=ModelSupport::getHeadRule(SMap,buildIndex,BI,"1M -8");
  makeCell("supportWater",System,cellIndex++,waterMat,0.0,HR*plateHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,BI,"1M 8 -18");
  makeCell("supportPipe",System,cellIndex++,copperMat,0.0,HR*plateHR);

  // Connect box:
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2101 -107 17 18");
  makeCell("supplyBox",System,cellIndex++,voidMat,0.0,HR*plateHR);
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-2101 -107");
  addOuterUnionSurf("Connect",HR*plateHR);

  // main deflector plate:
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"2001 -2002 2003 -2004 2005 -2006 1008 1048");
  makeCell("deflectorPlate",System,cellIndex++,copperMat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"2101 -2002 2003 -2004 2105 -2005 "
     "(-1001:1008) (1051:1048) (17:1001) (18:-1051)");
  makeCell("deflectorVoid",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"2101 -2002 2003 -2004 -2106 2006 ");

  // maybe need to divide this:
  for(const HeadRule& pHR : pipes)
    HR*=pHR;
  makeCell("deflectorVoid",System,cellIndex++,voidMat,0.0,HR);

  // mid void
  HR=ModelSupport::getHeadRule(SMap,buildIndex,
			       "2101 -2001 2003 -2004 2005 -2006 1008 1048");
  makeCell("deflectorVoid",System,cellIndex++,voidMat,0.0,HR);

  /*
  // pipe extras:
  if (plateAngle>Geometry::zeroTol)
    {
    }
  else if (plateAngle<-Geometry::zeroTol)
    {
      ELog::EM<<"Adding unit "<<ELog::endDiag;
      HR=ModelSupport::getHeadRule(SMap,buildIndex,"1048 -18 -1001 -1051");
      makeCell("deflectorVoid",System,cellIndex++,voidMat,0.0,HR);
    }
  */
  return;
}


void
BeamScrapper::createLinks()
  /*!
    Create all the linkes [need front/back to join/use InnerZone]
  */
{
  ELog::RegMethod RegA("BeamScrapper","createLinks");

  return;
}

void
BeamScrapper::setBeamAxis(const attachSystem::FixedComp& FC,
			    const long int sIndex)
  /*!
    Set the screen centre
    \param FC :: FixedComp to use
    \param sIndex :: Link point index
  */
{
  ELog::RegMethod RegA("BeamScrapper","setBeamAxis(FC)");

  beamAxis=Geometry::Line(FC.getLinkPt(sIndex),
			  -FC.getLinkAxis(sIndex));


  ELog::EM<<"Set beamAxis:"<<beamAxis<<ELog::endDiag;

  return;
}

void
BeamScrapper::setBeamAxis(const Geometry::Vec3D& Org,
		       const Geometry::Vec3D& Axis)
  /*!
    Set the screen centre
    \param Org :: Origin point for line
    \param Axis :: Axis of line
  */
{
  ELog::RegMethod RegA("BeamScrapper","setBeamAxis(Vec3D)");

  ELog::EM<<"Set beamAxis:"<<Org<<" : "<<Axis<<ELog::endDiag;
  beamAxis=Geometry::Line(Org,Axis);
  return;
}

void
BeamScrapper::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("BeamScrapper","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // NAMESPACE xraySystem
