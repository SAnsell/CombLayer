/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/FlightLine.cxx
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#include <stack>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <numeric>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfRegister.h"
#include "Quadratic.h"
#include "Plane.h"
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
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "surfExpand.h"
#include "FlightLine.h"

namespace moderatorSystem
{

FlightLine::FlightLine(const std::string& Key)  :
  attachSystem::ContainedGroup("inner","outer"),
  attachSystem::FixedRotate(Key,12),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  plateIndex(0),nLayer(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

FlightLine::FlightLine(const FlightLine& A) : 
  attachSystem::ContainedGroup(A),
  attachSystem::FixedRotate(A),
  attachSystem::ExternalCut(A),
  attachSystem::CellMap(A),
  height(A.height),width(A.width),
  plateIndex(A.plateIndex),nLayer(A.nLayer),lThick(A.lThick),
  lMat(A.lMat),capActive(A.capActive),capLayer(A.capLayer),
  capRule(A.capRule),attachRuleHR(A.attachRuleHR)
  /*!
    Copy constructor
    \param A :: FlightLine to copy
  */
{
  anglesXY[0]=A.anglesXY[0];
  anglesXY[1]=A.anglesXY[1];
  anglesZ[0]=A.anglesZ[0];
  anglesZ[1]=A.anglesZ[1];
}

FlightLine&
FlightLine::operator=(const FlightLine& A)
  /*!
    Assignment operator
    \param A :: FlightLine to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedGroup::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      attachSystem::CellMap::operator=(A);
      anglesXY[0]=A.anglesXY[0];
      anglesXY[1]=A.anglesXY[1];
      anglesZ[0]=A.anglesZ[0];
      anglesZ[1]=A.anglesZ[1];
      height=A.height;
      width=A.width;
      plateIndex=A.plateIndex;
      nLayer=A.nLayer;
      lThick=A.lThick;
      lMat=A.lMat;
      capActive=A.capActive;
      capLayer=A.capLayer;
      capRule=A.capRule;
      attachRuleHR=A.attachRuleHR;
    }
  return *this;
}

FlightLine::~FlightLine() 
 /*!
   Destructor
 */
{}

void
FlightLine::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: Database to use
 */
{
  ELog::RegMethod RegA("FlightLine","populate");

  FixedRotate::populate(Control);
  // First get inner widths:

  anglesXY[0]=Control.EvalVar<double>(keyName+"AngleXY1");
  anglesXY[1]=Control.EvalVar<double>(keyName+"AngleXY2");

  anglesZ[0]=Control.EvalVar<double>(keyName+"AngleZTop");
  anglesZ[1]=Control.EvalVar<double>(keyName+"AngleZBase");

  height=Control.EvalVar<double>(keyName+"Height");
  width=Control.EvalVar<double>(keyName+"Width");

  innerMat=ModelSupport::EvalDefMat(Control,keyName+"InnerMat",0);

  nLayer=Control.EvalDefVar<size_t>(keyName+"NLiner",0);
  lThick.clear();
  lMat.clear();
  for(size_t i=0;i<nLayer;i++)
    {
      const std::string idxStr=std::to_string(i+1);
      lThick.push_back(Control.EvalVar<double>(keyName+"LinerThick"+idxStr));
      lMat.push_back(ModelSupport::EvalMat<int>
		     (Control,keyName+"LinerMat"+idxStr));
      capLayer.push_back(Control.EvalDefVar<int>(keyName+"LinerCap"+idxStr,0));
    }
  
  if (Control.hasVariable(keyName+"SideIndex"))
    plateIndex=Control.EvalVar<long int>(keyName+"SideIndex");
  
  return;
}
  


void
FlightLine::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("FlightLine","createSurfaces");

  // Sides: Layers:
  Geometry::Vec3D xDircA(X);   
  Geometry::Vec3D xDircB(X);
  Geometry::Vec3D zDircA(Z);   
  Geometry::Vec3D zDircB(Z);

  Geometry::Quaternion::calcQRotDeg(anglesXY[0],Z).rotate(xDircA);
  Geometry::Quaternion::calcQRotDeg(-anglesXY[1],Z).rotate(xDircB);
  Geometry::Quaternion::calcQRotDeg(-anglesZ[0],X).rotate(zDircA);
  Geometry::Quaternion::calcQRotDeg(anglesZ[1],X).rotate(zDircB);


  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),xDircA);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),xDircB);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),zDircA);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),zDircB);

  double layT(0.0);
  for(size_t i=0;i<nLayer;i++)
    {
      const int II(static_cast<int>(i));
      layT+=lThick[i];
	  
      ModelSupport::buildPlane(SMap,buildIndex+II*10+13,
			       Origin-X*(width/2.0)-xDircA*layT,xDircA);
      ModelSupport::buildPlane(SMap,buildIndex+II*10+14,
			       Origin+X*(width/2.0)+xDircB*layT,xDircB);
      ModelSupport::buildPlane(SMap,buildIndex+II*10+15,
			       Origin-Z*(height/2.0)-zDircA*layT,zDircA);
      ModelSupport::buildPlane(SMap,buildIndex+II*10+16,
			       Origin+Z*(height/2.0)+zDircB*layT,zDircB);
    }

  // CREATE LINKS
  int signVal(-1);
  for(size_t i=3;i<7;i++)
    {
      const int sNum(buildIndex+static_cast<int>(10*nLayer+i));
      FixedComp::setLinkSurf(i-1,signVal*SMap.realSurf(sNum));
      const int tNum(buildIndex+static_cast<int>(i));
      FixedComp::setLinkSurf(i+5,signVal*SMap.realSurf(tNum));
      signVal*=-1;
    } 

  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setConnect(1,Origin,Y);

  FixedComp::setConnect(2,Origin-X*(width/2.0)-xDircA*layT,-xDircA);
  FixedComp::setConnect(3,Origin+X*(width/2.0)+xDircB*layT,xDircB);
  FixedComp::setConnect(4,Origin-Z*(height/2.0)-zDircA*layT,-zDircA);
  FixedComp::setConnect(5,Origin+Z*(height/2.0)+zDircB*layT,zDircB);

  FixedComp::setConnect(6,Origin,-Y);
  FixedComp::setConnect(7,Origin,Y);
  FixedComp::setConnect(8,Origin-X*(width/2.0),-xDircA);
  FixedComp::setConnect(9,Origin+X*(width/2.0),xDircB);
  FixedComp::setConnect(10,Origin-Z*(height/2.0),-zDircA);
  FixedComp::setConnect(11,Origin+Z*(height/2.0),zDircB);

  return;
}


HeadRule
FlightLine::getRotatedDivider(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Control divider planes if a masterXY / Z rotation has happened.
    \param FC :: FixedComp
    \param sideIndex :: initial size of link surface to 
    \return Rotated new headrule
    \todo Use new bridge surface to isolate primary
  */
{
  ELog::RegMethod RegA("FlightLine","getRotatedDivider");

  static int offset(750);

  attachRuleHR=FC.getMainRule(sideIndex);
  if (std::abs(zAngle)<45.0) 
    return attachRuleHR;

  HeadRule rotHead(FC.getCommonRule(sideIndex));
  const std::set<int> commonSN=
    rotHead.getSignedSurfaceNumbers();

  for(const int SN : commonSN)
    {
      const Geometry::Plane* PN=
	SMap.realPtr<Geometry::Plane>(abs(SN));
      const int signV((SN>0)? 1 : -1);
      if (PN)
	{
	  const Geometry::Quaternion QrotXY=
	    Geometry::Quaternion::calcQRotDeg(zAngle,Z);
	  const Geometry::Quaternion QrotZ=
	    Geometry::Quaternion::calcQRotDeg(xAngle,X);
	  Geometry::Vec3D PAxis=PN->getNormal();
	  QrotZ.rotate(PAxis);
	  QrotXY.rotate(PAxis);
	  const Geometry::Plane* PX=
	    ModelSupport::buildPlane(SMap,buildIndex+offset,
				     FC.getCentre(),PAxis);
	  const int PXNum=PX->getName();
	  
	  rotHead.substituteSurf(abs(SN),signV*PXNum,PX);
	}
    }
  attachRuleHR*=rotHead;
  return attachRuleHR;
}

void
FlightLine::createCapSurfaces(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the surfaces needed for the capping object
    \param FC :: FixedComp 
    \param sideIndex :: side to expand
   */
{
  ELog::RegMethod RegA("FlightLine","createCapSurfaces");

  const HeadRule& MainUnit=FC.getMainRule(sideIndex);
  const std::set<int> SurNum(MainUnit.getSignedSurfaceNumbers());
  
  int surfN(501+buildIndex);
  int newSurfN;
  std::stack<double> capThickStack;
  capThickStack.push(0.0);
  double capThick(0.0);
	
  for(size_t i=0;i<nLayer;i++)
    {
      capRule.push_back(MainUnit);      // for cases were not needed
      if (capLayer[i])
	{
	  // Create a stack of each layers thickness
	  if (!i || capLayer[i]>capLayer[i-1])
	    {
	      capThick=capThickStack.top();
	      capThickStack.push(capThick+lThick[i]);
	    }
	  else
	    {
	      capThickStack.pop();
	      capThick=capThickStack.top();
	    }
	  if (capLayer[i]>1)
	    {
	      HeadRule CRule(MainUnit);
	      for(const int SN : SurNum)
		{
		  // Get surface and expand:
		  const Geometry::Surface* SPtr=
		    SMap.realSurfPtr(SN);
		  const double signV((SN>0) ? 1.0 : -1.0);
		  Geometry::Surface* ExpSurf=
		    ModelSupport::surfaceCreateExpand(SPtr,signV*capThick);
		  // Find a new number for surface
		  while(SMap.hasSurf(surfN))
		    surfN++;
		  // register surface
		  ExpSurf->setName(surfN);
		  // This can remove the surfeaces:
		  newSurfN=SMap.registerSurf(ExpSurf);
		  ExpSurf=SMap.realSurfPtr(newSurfN);
		  // Substitute rule with new surface:
		  CRule.substituteSurf(abs(SN),newSurfN,ExpSurf);
		}
	      capRule[i]=CRule;
	    }
	}
    }
  return;
}

void
FlightLine::createObjects(Simulation& System,
			  const attachSystem::FixedComp& FC,
			  const long int sideIndex)
  /*!
    Creates the objects for the flightline
    \param System :: Simulation to create objects in
    \param FC :: Inner Object
    \param sideIndex :: Side index
  */
{
  ELog::RegMethod RegA("FlightLine","createObjects(FC,sideIndex)");
  
  const int outIndex=buildIndex+static_cast<int>(nLayer)*10;

  // attachRule SET in getRotatedDivider
  const HeadRule dividerHR=getRotatedDivider(FC,sideIndex);
  attachRuleHR=dividerHR*FC.getMainRule(sideIndex);

  HeadRule HR;
  
  HR=ModelSupport::getHeadRule(SMap,outIndex," 3 -4 5 -6 ");
  addOuterSurf("outer",HR*attachRuleHR);

  // Inner Void
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 3 -4 5 -6 ");
  HR*=attachRuleHR;
  addOuterSurf("inner",HR);

  HR*=ExternalCut::getRule("BeOuter");

  makeCell("Inner",System,cellIndex++,innerMat,0.0,HR);

  //Flight layers:
  for(size_t i=0;i<nLayer;i++)
    {
      const int II(static_cast<int>(i));
      if (i && capLayer[i]>1)        // only object to be capped 
	{
	  HR=ModelSupport::getHeadRule(SMap,buildIndex+10*II,
				     "13 -14 15 -16 (-3:4:-5:6)");
	  const HeadRule ICut=capRule[i].complement();
	  const HeadRule IOut=HR*ICut*capRule[i-1]*dividerHR;
	  makeCell("CapLayer",System,cellIndex++,lMat[i-1],0.0,IOut);
	  HR*=capRule[i]*dividerHR;
	}
      else
	{
	  HR=ModelSupport::getHeadRule(SMap,buildIndex+10*II,
				     " 13 -14 15 -16 (-3:4:-5:6)");
	  HR*=attachRuleHR;         // forward boundary of object
	}      
      makeCell("Outer",System,cellIndex++,lMat[i],0.0,HR);
    }      

  return;
}


void
FlightLine::processIntersectMajor(Simulation& System,
				  const attachSystem::ContainedGroup& CC,
				  const std::string& iKey,
				  const std::string&)
  /*!
    Intersect as a major flightline : Thus the CC unit only 
    cuts into the surround guide material not the middle
    \param System :: Simulation to use    
    \param CC :: Contained object [Must have outer key]
    \param iKey :: Name of inner key [from CC]
    \param :: Name of outer key [from this -- not used yet]
   */
{
  ELog::RegMethod RegA("FlightLine","processIntersectMajor");

  int metalCell(buildIndex+2);
  for(size_t i=0;i<nLayer;i++)
    {
      MonteCarlo::Object* Obj=System.findObject(metalCell++);
      if (!Obj)
	throw ColErr::InContainerError<int>
	  (metalCell-1,"Cell not found at layer"+std::to_string(i+1));
      const HeadRule HR=Obj->getHeadRule().complement()*
	CC.getOuterSurf(iKey).complement();
      Obj->procHeadRule(HR);
    }

  HeadRule HR;
  const int outIndex=buildIndex+static_cast<int>(nLayer)*10;
  
  HR=ModelSupport::getHeadRule(SMap,outIndex,"3 -4 5 -6");
  HR*=attachRuleHR;
  HR*=ModelSupport::getHeadRule(SMap,buildIndex,"(-3:4:-5:6)");  
  HR*=CC.getOuterSurf(iKey);
  makeCell("Inner",System,cellIndex++,0,0.0,HR);

  return;
}

void
FlightLine::processIntersectMinor(Simulation& System,
				  const attachSystem::ContainedGroup& CC,
				  const std::string&,
				  const std::string& oKey)
  /*!
    Intersect as a minor flightline : Thus the CC unit 
    cuts into the surround guide material and the middle
    \param System :: Simulation to use    
    \param CC :: Contained object [Must have outer key]
    \param :: Name of inner key [from CC -- not used yet]
    \param oKey :: Name of outer key [from this]

    \todo This should use a named cell not changeCell
  */
{
  ELog::RegMethod RegA("FlightLine","processIntersectMinor");

  int changeCell(buildIndex+1);
  for(size_t i=0;i<=nLayer;i++)
    {
      MonteCarlo::Object* Obj=System.findObject(changeCell++);
      if (!Obj)
	throw ColErr::InContainerError<int>
	  (changeCell-1,"Cell no found at layer"+std::to_string(i+1));
      const std::string ObjStr=Obj->cellCompStr()+CC.getExclude(oKey);
      Obj->procString(ObjStr);
    }
  return;
}

void
FlightLine::getInnerVec(std::vector<int>& ISurf) const
  /*!
    Fill in the inner surface vectors so that they can be used as a 
    reference plane.
    \param ISurf :: Vector for viewing
  */
{
  ELog::RegMethod RegA("FlightLine","getInnerVec");
  ISurf.clear();
  for(int i=0;i<4;i++)
    ISurf.push_back(SMap.realSurf(buildIndex+i+3));

  return;
}

void
FlightLine::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int sideIndex)
  /*!
    Global creation of the vac-vessel
    \param System :: Simulation to add vessel to
    \param FC :: Moderator Object (or innner link)
    \param sideIndex :: Side index [ +/- 1:Index]
  */
{
  ELog::RegMethod RegA("FlightLine","createAll(FC,int)");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();

  if (sideIndex==0)
    throw ColErr::IndexError<long int>
      (0,0,"sideIndex == 0 no long possible"
       " as flightline cannot current track from centre origin");

  createCapSurfaces(FC,sideIndex);
  FixedComp::setLinkCopy(0,FC,sideIndex);
  FixedComp::setLinkCopy(6,FC,sideIndex);

  createObjects(System,FC,sideIndex);
  insertObjects(System);       

  return;
}


  
}  // NAMESPACE moderatorSystem
