/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/MidWaterDivider.cxx 
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#include <array>
#include <algorithm>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "surfRegister.h"
#include "geomSupport.h"
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
#include "ContainedComp.h"
#include "LayerComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurInter.h"
#include "H2Wing.h"
#include "MidWaterDivider.h"

namespace essSystem
{

MidWaterDivider::MidWaterDivider(const std::string& baseKey,
				 const std::string& extraKey) :
  attachSystem::FixedComp(baseKey+extraKey,14),
  attachSystem::ContainedComp(),
  baseName(baseKey),AWingPtr(nullptr),BWingPtr(nullptr)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param baseKey :: Base Name for item in search
    \param extraKey :: extra [specialized] Name for item in search
  */
{}

MidWaterDivider::MidWaterDivider(const MidWaterDivider& A) : 
  attachSystem::FixedComp(A),
  attachSystem::ContainedComp(A),
  baseName(A.baseName),
  midYStep(A.midYStep),
  midAngle(A.midAngle),length(A.length),height(A.height),
  wallThick(A.wallThick),modMat(A.modMat),wallMat(A.wallMat),
  modTemp(A.modTemp),AWingPtr(A.AWingPtr),BWingPtr(A.BWingPtr)
  /*!
    Copy constructor
    \param A :: MidWaterDivider to copy
  */
{}

MidWaterDivider&
MidWaterDivider::operator=(const MidWaterDivider& A)
  /*!
    Assignment operator
    \param A :: MidWaterDivider to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedComp::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      midYStep=A.midYStep;
      midAngle=A.midAngle;
      length=A.length;
      height=A.height;
      wallThick=A.wallThick;
      modMat=A.modMat;
      wallMat=A.wallMat;
      modTemp=A.modTemp;
      AWingPtr=A.AWingPtr;
      BWingPtr=A.BWingPtr;
    }
  return *this;
}

MidWaterDivider*
MidWaterDivider::clone() const
  /*!
    Virtual copy constructor
    \return new (this)
  */
{
  return new MidWaterDivider(*this);
}
  
MidWaterDivider::~MidWaterDivider() 
  /*!
    Destructor
  */
{}

void
MidWaterDivider::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("MidWaterDivider","populate");

  cutLayer=Control.EvalDefVar<size_t>(keyName+"CutLayer",3);
  
  midYStep=Control.EvalVar<double>(keyName+"MidYStep");
  midAngle=Control.EvalVar<double>(keyName+"MidAngle");

  length=Control.EvalVar<double>(keyName+"Length");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  topThick=Control.EvalDefVar<double>(keyName+"TopThick",0.0);
  baseThick=Control.EvalDefVar<double>(keyName+"BaseThick",0.0);
  cornerRadius=Control.EvalDefVar<double>(keyName+"CornerRadius",0.0);
  
  modMat=ModelSupport::EvalMat<int>(Control,keyName+"ModMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  modTemp=Control.EvalVar<double>(keyName+"ModTemp");

  return;
}
  

void
MidWaterDivider::createLinks()

  /*!
    Construct links:
    - 1+2 and 3+4 are the triangle surfaces (left/right)
    - 5+6 and 7+8 are corner points for view
    - 9+10 are top/bottom
    
  */
{
  ELog::RegMethod RegA("MidWaterDivider","createLinks");

  // main angles
  FixedComp::setLinkSurf(0, SMap.realSurf(buildIndex+103)); 
  FixedComp::setLinkSurf(1, -SMap.realSurf(buildIndex+104));  
  FixedComp::setLinkSurf(2, SMap.realSurf(buildIndex+123));  
  FixedComp::setLinkSurf(3, -SMap.realSurf(buildIndex+124)); 


  // small cutting edged
  FixedComp::setLinkSurf(5, SMap.realSurf(buildIndex+111));
  FixedComp::setLinkSurf(6, SMap.realSurf(buildIndex+112));
  FixedComp::setLinkSurf(7, SMap.realSurf(buildIndex+131));  
  FixedComp::setLinkSurf(8, SMap.realSurf(buildIndex+132));  

  std::vector<int> surfN;
  surfN.push_back(AWingPtr->getLinkSurf(1));
  surfN.push_back(AWingPtr->getLinkSurf(3));
  surfN.push_back(BWingPtr->getLinkSurf(1));
  surfN.push_back(BWingPtr->getLinkSurf(3));

  // Now deterermine point which are divider points
  const Geometry::Surface* midPlane=
    SMap.realSurfPtr(buildIndex+200);

  const std::vector<std::pair<int,int>> InterVec =
    {
      {103,104},{103,104},
      {123,124},{123,124},
      {111,-2},{112,-3},
      {131,-1},{132,-4}
    };
  const std::vector<Geometry::Vec3D> Axis
    ({Y,Y,-Y,-Y,Y,Y,-Y,-Y});

  
  for(size_t index=0;index<InterVec.size();index++)
    {
      const std::pair<int,int>& Item(InterVec[index]);
      const int SA(buildIndex+Item.first);
      const int SB(Item.second>0 ? buildIndex+Item.second :
		   surfN[static_cast<size_t>(-Item.second-1)]);
      const Geometry::Surface* PA=SMap.realSurfPtr(SA);
      const Geometry::Surface* PB=SMap.realSurfPtr(SB);
      FixedComp::setConnect
      	(index,SurInter::getPoint(PA,PB,midPlane),Axis[index]);
    }

  // full cut out

  HeadRule HR;

  HR=ModelSupport::getHeadRule(SMap,buildIndex,
      "((-123 (-137:138)) : (124 (-127:128))) -131 -132");
  HR.makeComplement();
  FixedComp::setLinkSurf(10,HR);
  FixedComp::setBridgeSurf(10,-SMap.realSurf(buildIndex+100));

  // +ve Y
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex, "( (-103 (-117:118)) : (104  (-107:108)) ) -111 -112");
  HR.makeComplement();  
  FixedComp::setLinkSurf(11,HR);
  FixedComp::setBridgeSurf(11,SMap.realSurf(buildIndex+100));


  FixedComp::setLinkSurf(12,SMap.realSurf(buildIndex+100));
  FixedComp::setConnect(12,Origin,Y);
  
  return;
}
  
void
MidWaterDivider::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("MidWaterDivider","createSurface");

  // Mid divider
  ModelSupport::buildPlane(SMap,buildIndex+100,Origin,Y);
  // Mid Vertical divider
  ModelSupport::buildPlane(SMap,buildIndex+200,Origin,Z);
  // Mid Vertical divider
  ModelSupport::buildPlane(SMap,buildIndex+300,Origin,X);

  // +Y section
  ModelSupport::buildPlaneRotAxis
    (SMap,buildIndex+3,Origin+Y*midYStep,X,-Z,-midAngle/2.0);
  ModelSupport::buildPlaneRotAxis
    (SMap,buildIndex+4,Origin+Y*midYStep,X,-Z,midAngle/2.0);

  // -Y section
  ModelSupport::buildPlaneRotAxis
    (SMap,buildIndex+23,Origin-Y*midYStep,-X,-Z,-midAngle/2.0);
  ModelSupport::buildPlaneRotAxis
    (SMap,buildIndex+24,Origin-Y*midYStep,-X,-Z,midAngle/2.0);

  // Make lengths:
  Geometry::Vec3D leftNorm(Y);
  Geometry::Quaternion::calcQRotDeg(-midAngle/2.0,Z).rotate(leftNorm);  
  Geometry::Vec3D rightNorm(Y);
  Geometry::Quaternion::calcQRotDeg(midAngle/2.0,Z).rotate(rightNorm);  
  
  ModelSupport::buildPlane(SMap,buildIndex+11,Origin+leftNorm*length,leftNorm);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+rightNorm*length,rightNorm);

  // Length below [note reverse of normals]
  ModelSupport::buildPlane(SMap,buildIndex+31,Origin-rightNorm*length,-rightNorm);
  ModelSupport::buildPlane(SMap,buildIndex+32,Origin-leftNorm*length,-leftNorm);

  
  // Aluminum layers [+100]
  // +Y section
  const double LStep(midYStep+wallThick/sin(M_PI*midAngle/360.0));
  ModelSupport::buildPlaneRotAxis
    (SMap,buildIndex+103,Origin+Y*LStep,X,-Z,-midAngle/2.0);
  ModelSupport::buildPlaneRotAxis
    (SMap,buildIndex+104,Origin+Y*LStep,X,-Z,midAngle/2.0);

  // -Y section
  ModelSupport::buildPlaneRotAxis
    (SMap,buildIndex+123,Origin-Y*LStep,-X,-Z,-midAngle/2.0);
  ModelSupport::buildPlaneRotAxis
    (SMap,buildIndex+124,Origin-Y*LStep,-X,-Z,midAngle/2.0);

  
  ModelSupport::buildPlane(SMap,buildIndex+111,
			   Origin+leftNorm*(length+wallThick),leftNorm);
  ModelSupport::buildPlane(SMap,buildIndex+112,
			   Origin+rightNorm*(length+wallThick),rightNorm);
  // Length below [note reverse of normals]
  ModelSupport::buildPlane(SMap,buildIndex+131,
			   Origin-rightNorm*(wallThick+length),-rightNorm);
  ModelSupport::buildPlane(SMap,buildIndex+132,
			   Origin-leftNorm*(wallThick+length),-leftNorm);


  if (topThick>Geometry::zeroTol)
    ModelSupport::buildPlane(SMap,buildIndex+5,
                             Origin+Z*(height/2.0-topThick),Z);


  // Create corners:
  if (cornerRadius>Geometry::zeroTol)
    {
      const std::vector<int> sideSurf({-11, -12, -32, -31});
      const std::vector<int> frontSurf({4, -3,  24, -23});

      int CI(buildIndex);
      for(size_t i=0;i<4;i++)
        {
          HeadRule CCorner=ModelSupport::getHeadRule(SMap,buildIndex,
                                         std::to_string(sideSurf[i])+" "+
                                         std::to_string(frontSurf[i]));

          CCorner.populateSurf();
	  const std::tuple<Geometry::Vec3D,Geometry::Vec3D,Geometry::Vec3D>
	    RCircle=Geometry::findCornerCircle
            (CCorner,
             *SMap.realPtr<Geometry::Plane>(buildIndex+std::abs(sideSurf[i])),
             *SMap.realPtr<Geometry::Plane>(buildIndex+std::abs(frontSurf[i])),
             *SMap.realPtr<Geometry::Plane>(60000),
             cornerRadius);
	  const Geometry::Vec3D& CPt=std::get<0>(RCircle);
	  const Geometry::Vec3D& APt=std::get<1>(RCircle);
	  const Geometry::Vec3D& BPt=std::get<2>(RCircle);
	  
	  // axis away from outer radius
	  const Geometry::Vec3D CAxis((CPt*2.0-APt-BPt).unit());

	  
          ModelSupport::buildCylinder(SMap,CI+7,CPt,Z,cornerRadius);
          // Towards centre of circle
          ModelSupport::buildPlane(SMap,CI+8,APt,BPt,APt+Z,CAxis);

	  // Construct secondaries
	  
          ModelSupport::buildCylinder(SMap,CI+107,CPt,Z,
				      cornerRadius+wallThick);
          ModelSupport::buildShiftedPlane(SMap,CI+108,
					  SMap.realPtr<Geometry::Plane>(CI+8),
					  -wallThick);
				      

	  
          CI+=10;
        }      
    }
  
  return;
}

void
MidWaterDivider::createObjects(Simulation& System)
  /*!
    Adds the main components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("MidWaterDivider","createObjects");

  const HeadRule BaseHR=AWingPtr->getFullRule(-5);
  const HeadRule TopHR=AWingPtr->getFullRule(-6);
  
  const HeadRule LCut(AWingPtr->getLayerHR(cutLayer,-7));
  const HeadRule RCut(BWingPtr->getLayerHR(cutLayer,-7));

  HeadRule HR;

  if (topThick>Geometry::zeroTol)
    {
      HR=ModelSupport::getSetHeadRule
	(SMap,buildIndex,"300 100 4 -11 -5 (-7:8)");
      System.addCell(cellIndex++,modMat,modTemp,HR*LCut*BaseHR);

      HR=ModelSupport::getSetHeadRule
	(SMap,buildIndex,"-300 100 -3 -12 -5 (-17:18)");
      System.addCell(cellIndex++,modMat,modTemp,HR*RCut*BaseHR);

      HR=ModelSupport::getSetHeadRule
	(SMap,buildIndex,"300 100 4 -11  5 (-7:8)");
      System.addCell(cellIndex++,wallMat,modTemp,HR*LCut*TopHR);

      HR=ModelSupport::getSetHeadRule
	(SMap,buildIndex,"-300 100 -3 -12 5 (-17:18)");
      System.addCell(cellIndex++,wallMat,modTemp,HR*RCut*TopHR);

      // Reverse side
      HR=ModelSupport::getSetHeadRule(SMap,buildIndex,
                                    "300 -100 -23  -31  -5 (-37:38)");
      System.addCell(cellIndex++,modMat,modTemp,HR*LCut*BaseHR);
      
      HR=ModelSupport::getSetHeadRule(SMap,buildIndex,
                                    "-300 -100 24 -32 -5 (-27:28)");
      System.addCell(cellIndex++,modMat,modTemp,HR*RCut*BaseHR);

      
      HR=ModelSupport::getSetHeadRule(SMap,buildIndex,
                                    "300 -100 -23 -31 5 (-37:38)");
      System.addCell(cellIndex++,wallMat,modTemp,HR*LCut*TopHR);

      HR=ModelSupport::getSetHeadRule(SMap,buildIndex,
                                    "-300 -100 24 -32 5 (-27:28)");
      System.addCell(cellIndex++,wallMat,modTemp,HR*RCut*TopHR);
    }
  else 
    {
      HR=ModelSupport::getSetHeadRule
	(SMap,buildIndex,"300 100 4 -11 (-7:8)");
      System.addCell(cellIndex++,modMat,modTemp,HR*LCut*BaseHR*TopHR);
      
      HR=ModelSupport::getSetHeadRule
	(SMap,buildIndex,"-300 100 -3 -12 (-17:18)");
      System.addCell(cellIndex++,modMat,modTemp,HR*RCut*BaseHR*TopHR);

      // Reverse layers
      HR=ModelSupport::getSetHeadRule
	(SMap,buildIndex,"300 -100 -23 -31 (-37:38)");
      System.addCell(cellIndex++,modMat,modTemp,HR*LCut*BaseHR*TopHR);

      HR=ModelSupport::getSetHeadRule
	(SMap,buildIndex,"-300 -100 24 -32 (-27:28)");
      System.addCell(cellIndex++,modMat,modTemp,HR*RCut*BaseHR*TopHR);
      }
  
  HR=ModelSupport::getSetHeadRule
    (SMap,buildIndex,"300 100 104 -111 (-107:108) (-4 : 11 : (7 -8) )");
  System.addCell(cellIndex++,wallMat,modTemp,HR*BaseHR*TopHR*LCut);


  HR=ModelSupport::getSetHeadRule
    (SMap,buildIndex,"-300 100 -103 -112 (-117:118) (3 : 12 : (17 -18))");
  System.addCell(cellIndex++,wallMat,modTemp,HR*RCut*BaseHR*TopHR);

  HR=ModelSupport::getSetHeadRule
    (SMap,buildIndex,"100 ( (-103 (-117:118)) : (104  (-107:108)) ) -111 -112");
  addOuterSurf(HR);

  // Reverse side
  HR=ModelSupport::getSetHeadRule
    (SMap,buildIndex,"300 -100 -123 -131 (-137:138) (23 : 31 : (37 -38))");
  System.addCell(cellIndex++,wallMat,modTemp,HR*LCut*BaseHR*TopHR);

  HR=ModelSupport::getSetHeadRule
    (SMap,buildIndex,"-300 -100 124 -132 (-127:128) (-24 : 32 : (27 -28))");
  System.addCell(cellIndex++,wallMat,modTemp,HR*RCut*BaseHR*TopHR);

  HR=ModelSupport::getSetHeadRule
    (SMap,buildIndex,"-100 ( (-123 (-137:138)) : (124 (-127:128)) ) -131 -132");
  addOuterUnionSurf(HR);
  
   
  return;
}

void
MidWaterDivider::cutOuterWing(Simulation& System) const
  /*!
    Cut the outer surface layer of the wings with the
    exclude version of the water layer
    \param System :: Simuation to extract objects rom
  */
{
  ELog::RegMethod RegA("MidWaterDivider","cutOuterWing");


  const size_t lWing=AWingPtr->getNLayers();
  const size_t rWing=BWingPtr->getNLayers();

  const HeadRule LBase=
    AWingPtr->getFullRule(-5)*AWingPtr->getFullRule(-6);
  const HeadRule RBase=
    BWingPtr->getFullRule(-5)*BWingPtr->getFullRule(-6);

  HeadRule cutRuleHR;

  if (cutLayer+1<lWing)
    {
      const int cellA=AWingPtr->getCell("Outer");
      
      MonteCarlo::Object* OPtr=System.findObject(cellA);
      if (!OPtr)
	throw ColErr::InContainerError<int>(cellA,"leftWing Cell: Outer");
      cutRuleHR=ModelSupport::getHeadRule
	(SMap,buildIndex,"(-100  11) : (100 31)");
      OPtr->addIntersection(cutRuleHR);
    }
  if (cutLayer+1<rWing)
    {
      const int cellB=BWingPtr->getCell("Outer");
      MonteCarlo::Object* OPtr=System.findObject(cellB);
      if (!OPtr)
	throw ColErr::InContainerError<int>(cellB,"rightWing Cell: Outer");
      cutRuleHR=ModelSupport::getHeadRule
	(SMap,buildIndex,"(-100  12) : (100 32)");
      OPtr->addIntersection(cutRuleHR);
    }
  return;
}

void
MidWaterDivider::createAll(Simulation& System,
			   const attachSystem::FixedComp& FC,
			   const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed object just for origin/axis
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("MidWaterDivider","createAll");

  if (AWingPtr==BWingPtr || !AWingPtr || !BWingPtr)
    throw ColErr::EmptyContainer("A/BWingPtr error");
  
  populate(System.getDataBase());
  height=AWingPtr->getLinkDistance(5,6)-topThick;

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  cutOuterWing(System);
  createLinks();
  insertObjects(System);       
  return;
}
  
}  // NAMESPACE essSystem
