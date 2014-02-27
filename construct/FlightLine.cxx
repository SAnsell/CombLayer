/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   construct/FlightLine.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

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
#include "surfEqual.h"
#include "surfDivide.h"
#include "surfDIter.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
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
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "surfExpand.h"
#include "FlightLine.h"

namespace moderatorSystem
{

FlightLine::FlightLine(const std::string& Key)  :
  attachSystem::ContainedGroup("inner","outer"),
  attachSystem::FixedComp(Key,12),
  flightIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(flightIndex+1),plateIndex(0),nLayer(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

FlightLine::FlightLine(const FlightLine& A) : 
  attachSystem::ContainedGroup(A),attachSystem::FixedComp(A),
  flightIndex(A.flightIndex),cellIndex(A.cellIndex),
  xStep(A.xStep),zStep(A.zStep),masterXY(A.masterXY),
  masterZ(A.masterZ),height(A.height),width(A.width),
  plateIndex(A.plateIndex),nLayer(A.nLayer),lThick(A.lThick),
  lMat(A.lMat),capActive(A.capActive),capLayer(A.capLayer),
  capRule(A.capRule),attachRule(A.attachRule)
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
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      xStep=A.xStep;
      zStep=A.zStep;
      masterXY=A.masterXY;
      masterZ=A.masterZ;
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
      attachRule=A.attachRule;
    }
  return *this;
}

FlightLine::~FlightLine() 
 /*!
   Destructor
 */
{}

void
FlightLine::populate(const Simulation& System)
 /*!
   Populate all the variables
   \param System :: Simulation to use
 */
{
  ELog::RegMethod RegA("FlightLine","populate");
  
  const FuncDataBase& Control=System.getDataBase();

  // First get inner widths:
  xStep=Control.EvalVar<double>(keyName+"XStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");

  masterXY=Control.EvalDefVar<double>(keyName+"MasterXY",0.0);
  masterZ=Control.EvalDefVar<double>(keyName+"MasterZ",0.0);

  anglesXY[0]=Control.EvalVar<double>(keyName+"AngleXY1");
  anglesXY[1]=Control.EvalVar<double>(keyName+"AngleXY2");

  anglesZ[0]=Control.EvalVar<double>(keyName+"AngleZTop");
  anglesZ[1]=Control.EvalVar<double>(keyName+"AngleZBase");

  height=Control.EvalVar<double>(keyName+"Height");
  width=Control.EvalVar<double>(keyName+"Width");

  nLayer=Control.EvalDefVar<size_t>(keyName+"NLiner",0);
  lThick.clear();
  lMat.clear();
  for(size_t i=0;i<nLayer;i++)
    {
      const std::string idxStr=StrFunc::makeString(i+1);
      lThick.push_back(Control.EvalVar<double>(keyName+"LinerThick"+idxStr));
      lMat.push_back(ModelSupport::EvalMat<int>
		     (Control,keyName+"LinerMat"+idxStr));
      capLayer.push_back(Control.EvalDefVar<int>(keyName+"LinerCap"+idxStr,0));
    }
  
  if (Control.hasVariable(keyName+"SideIndex"))
    plateIndex=Control.EvalVar<int>(keyName+"SideIndex");
  
  return;
}
  

void
FlightLine::createUnitVector(const attachSystem::FixedComp& FC,
			     const size_t sideIndex)
  /*!
    Create the unit vectors
    - Y Points towards the beamline
    - X Across the Face
    - Z up (towards the target)
    \param FC :: A Contained FixedComp to use as basis set
    \param sideIndex :: Index on fixed unit
  */
{
  ELog::RegMethod RegA("FlightLine","createUnitVector<FC,side>");

  // PROCESS Origin of a point
  const attachSystem::LinkUnit& LU=FC.getLU(sideIndex);
  createUnitVector(LU.getConnectPt(),LU.getAxis(),FC.getZ());
  return;
}

void
FlightLine::createRotatedUnitVector(const attachSystem::FixedComp& FC,
				    const size_t origIndex,
				    const size_t sideIndex)
  /*!
    Create the unit vectors
    - Y Points towards the beamline
    - X Across the Face
    - Z up (towards the target)
    \param FC :: A Contained FixedComp to use as basis set
    \param originIndex :: Index for centre of rotation
    \param sideIndex :: Index on fixed unit
  */
{
  ELog::RegMethod RegA("FlightLine","createRotatedUnitVector");
  const Geometry::Vec3D CentPt=
    (!origIndex) ? FC.getCentre() : FC.getLinkPt(origIndex-1);

  const attachSystem::LinkUnit& LU=FC.getLU(sideIndex);
  createUnitVector(LU.getConnectPt(),LU.getAxis(),FC.getZ());

  Z= FC.getZ().unit();
  Y= LU.getAxis().unit();
  X= Y*Z;

  Origin=FC.getLinkPt(sideIndex);
  applyFullRotate(masterXY,masterZ,CentPt);
  Origin+= X*xStep + Z*zStep;
  return;
}

void
FlightLine::createUnitVector(const attachSystem::FixedComp& FC,
			     const size_t origIndex,
			     const size_t sideIndex)
  /*!
    Create the unit vectors
    - Y Points towards the beamline
    - X Across the Face
    - Z up (towards the target)
    \param FC :: A Contained FixedComp to use as basis set
    \param originIndex :: Index for centre 
    \param sideIndex :: Index on fixed unit
  */
{
  ELog::RegMethod RegA("FlightLine","createUnitVector(FC,orig,side)");
  if (!origIndex)
    createUnitVector(FC.getCentre(),
		     FC.getLinkPt(sideIndex),FC.getZ());
  else
    createUnitVector(FC.getLinkPt(origIndex-1),
		     FC.getLinkPt(sideIndex),FC.getZ());
  return;
}

void
FlightLine::createUnitVector(const Geometry::Vec3D& Og,
			     const Geometry::Vec3D& Ax,
			     const Geometry::Vec3D& Zunit)

  /*!
    Create the unit vectors
    \param Og :: Origin
    \param Ax :: Axis
    \param Zunit :: Z direction [since AX is Y]
  */
{
  ELog::RegMethod RegA("FlightLine","createUnitVector<Vec,Vec>");
  
  Z= Zunit.unit();
  Y= Ax.unit();
  X= Y*Z;

  applyAngleRotate(masterXY,masterZ);
  Origin=Og+X*xStep+Z*zStep;

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


  ModelSupport::buildPlane(SMap,flightIndex+3,Origin-X*(width/2.0),xDircA);
  ModelSupport::buildPlane(SMap,flightIndex+4,Origin+X*(width/2.0),xDircB);
  ModelSupport::buildPlane(SMap,flightIndex+5,Origin-Z*(height/2.0),zDircA);
  ModelSupport::buildPlane(SMap,flightIndex+6,Origin+Z*(height/2.0),zDircB);

  double layT(0.0);
  for(size_t i=0;i<nLayer;i++)
    {
      const int II(static_cast<int>(i));
      layT+=lThick[i];
	  
      ModelSupport::buildPlane(SMap,flightIndex+II*10+13,
			       Origin-X*(width/2.0)-xDircA*layT,xDircA);
      ModelSupport::buildPlane(SMap,flightIndex+II*10+14,
			       Origin+X*(width/2.0)+xDircB*layT,xDircB);
      ModelSupport::buildPlane(SMap,flightIndex+II*10+15,
			       Origin-Z*(height/2.0)-zDircA*layT,zDircA);
      ModelSupport::buildPlane(SMap,flightIndex+II*10+16,
			       Origin+Z*(height/2.0)+zDircB*layT,zDircB);
    }

  // CREATE LINKS
  int signVal(-1);
  for(size_t i=3;i<7;i++)
    {
      const int sNum(flightIndex+static_cast<int>(10*nLayer+i));
      FixedComp::setLinkSurf(i-1,signVal*SMap.realSurf(sNum));
      const int tNum(flightIndex+static_cast<int>(i));
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

void
FlightLine::removeObjects(Simulation& System)
  /*!
    Remove all the objects
    \param System :: Simulation to remove objects from
  */
{
  ELog::RegMethod RegA("FlightLine","removeObjects");

  if (cellIndex!=flightIndex+1)
    {
      System.removeCells(flightIndex+1,cellIndex-1);
      cellIndex=flightIndex+1;
    }
  return;
}

std::string
FlightLine::getRotatedDivider(const attachSystem::FixedComp& FC,
			      const size_t sideIndex)
  /*!
    Control divider planes if a masterXY / Z rotation has happened.
    \param FC :: FixedComp
    \param sideIndex :: initial size of link surface to 
    \return Rotated new string 
    \todo Use new bridge surface to isolate primary
  */
{
  ELog::RegMethod RegA("FlightLine","getRotatedDivider");
  static int offset(750);
  std::string attachRule=FC.getCommonString(sideIndex);

  if (fabs(masterXY)<45.0) 
    return " "+attachRule+" ";

  const std::string primary=FC.getMasterString(sideIndex);


  HeadRule rotHead;
  if (!rotHead.procString(attachRule))
    return primary;

  int SN;
  while(StrFunc::section(attachRule,SN))
    {
      const Geometry::Plane* PN=
	SMap.realPtr<Geometry::Plane>(abs(SN));
      const int signV((SN>0)? 1 : -1);
      if (PN)
	{
	  const Geometry::Quaternion QrotXY=
	    Geometry::Quaternion::calcQRotDeg(masterXY,Z);
	  const Geometry::Quaternion QrotZ=
	    Geometry::Quaternion::calcQRotDeg(masterZ,X);
	  Geometry::Vec3D PAxis=PN->getNormal();
	  QrotZ.rotate(PAxis);
	  QrotXY.rotate(PAxis);
	  const Geometry::Plane* PX=
	    ModelSupport::buildPlane(SMap,flightIndex+offset,
				     FC.getCentre(),PAxis);
	  const int PXNum=PX->getName();
	  
	  rotHead.substituteSurf(abs(SN),signV*PXNum,PX);
	}
    }
  return " "+primary+" "+rotHead.display()+" ";
}

void
FlightLine::createCapSurfaces(const attachSystem::FixedComp& FC,
			      const size_t sideIndex)
  /*!
    Create the surfaces needed for the capping object
    \param FC :: FixedComp 
    \param sideIndex :: side to expand
   */
{
  ELog::RegMethod RegA("FlightLine","createCapSurfaces");

  const HeadRule& MainUnit=FC.getMainRule(sideIndex);
  const std::vector<int> SurNum(MainUnit.getSurfaceNumbers());
  
  int surfN(501+flightIndex);
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
	      std::vector<int>::const_iterator vc;
	      for(vc=SurNum.begin();vc!=SurNum.end();vc++)
		{
		  // Get surface and expand:
		  const Geometry::Surface* SPtr=
		    SMap.realSurfPtr(*vc);
		  const double signV((*vc>0) ? 1.0 : -1.0);
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
		  CRule.substituteSurf(abs(*vc),newSurfN,ExpSurf);
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
			  const size_t sideIndex)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
    \param FC :: Inner Object
    \param sideIndex :: Side index
  */
{
  ELog::RegMethod RegA("FlightLine","createObjects");
  
  const int outIndex=flightIndex+static_cast<int>(nLayer)*10;

  const std::string divider=getRotatedDivider(FC,sideIndex);
  attachRule+=divider+FC.getMasterString(sideIndex);

  std::string Out;
  Out=ModelSupport::getComposite(SMap,outIndex," 3 -4 5 -6 ");
  Out+=attachRule;         // forward boundary of object
  addOuterSurf("outer",Out);
  // Inner Void
  Out=ModelSupport::getComposite(SMap,flightIndex," 3 -4 5 -6 ");
  Out+=attachRule;
  addOuterSurf("inner",Out);

  Out=ModelSupport::getComposite(SMap,flightIndex," 3 -4 5 -6 ");
  Out+=attachRule;         // forward boundary of object
  Out+=" "+ContainedGroup::getContainer("outer");      // Be outer surface
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  //Flight layers:
  for(size_t i=0;i<nLayer;i++)
    {
      const int II(static_cast<int>(i));
      if (i && capLayer[i]>1)        // only object to be capped 
	{
	  Out=ModelSupport::getComposite(SMap,flightIndex+10*II,
				     "13 -14 15 -16 (-3:4:-5:6) ");
	  HeadRule ICut=capRule[i];
	  ICut.makeComplement();
	  const std::string IOut=Out+ICut.display()+" "+
	    capRule[i-1].display()+divider;
	  System.addCell(MonteCarlo::Qhull(cellIndex++,lMat[i-1],0.0,IOut));
	  Out+=capRule[i].display()+divider;
	}
      else
	{
	  Out=ModelSupport::getComposite(SMap,flightIndex+10*II,
				     " 13 -14 15 -16 (-3:4:-5:6) ");
	  Out+=attachRule;         // forward boundary of object
	}
      Out+=" "+ContainedGroup::getContainer("outer");      // Be outer surface
      System.addCell(MonteCarlo::Qhull(cellIndex++,lMat[i],0.0,Out));
    }      

  return;
}

void
FlightLine::createObjects(Simulation& System,
			  const attachSystem::FixedComp& FC,
			  const int surfSign,
			  const size_t sideIndex,
			  const attachSystem::ContainedComp& CC)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
    \param FC :: SUrface linked plane 
    \param sideInde :: side index
    \param CC :: Inner Object
    \param sideIndex :: Side index
  */
{
  ELog::RegMethod RegA("FlightLine","createObjects");
  
  const int outIndex=flightIndex+static_cast<int>(nLayer)*10;

  // Note this is negative
  const int baseSurf( (surfSign>0) ? FC.getLinkSurf(sideIndex) : 
		      -FC.getLinkSurf(sideIndex) );

  std::string Out;
  Out=ModelSupport::getComposite(SMap,outIndex," 3 -4 5 -6 ");
  Out+=StrFunc::makeString(baseSurf);
  addOuterSurf("outer",Out);
  addOuterSurf("inner",Out);
  const std::string attachRule=StrFunc::makeString(baseSurf)
    +" "+CC.getExclude();

  Out=ModelSupport::getComposite(SMap,flightIndex," 3 -4 5 -6 ");
  Out+=attachRule;         // forward boundary of object
  Out+=" "+ContainedGroup::getContainer("outer");      // Be outer surface
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  //Flight layers:
  for(size_t i=0;i<nLayer;i++)
    {
      Out=ModelSupport::getComposite(SMap,flightIndex+10*static_cast<int>(i),
				     " 13 -14 15 -16 (-3:4:-5:6) ");
      Out+=attachRule;         // forward boundary of object
      Out+=" "+ContainedGroup::getContainer("outer");      // Be outer surface
      System.addCell(MonteCarlo::Qhull(cellIndex++,lMat[i],0.0,Out));
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
    \param oKey :: Name of outer key [from this -- not used yet]
   */
{
  ELog::RegMethod RegA("FlightLine","processIntersectMajor");

  int metalCell(flightIndex+2);
  for(size_t i=0;i<nLayer;i++)
    {
      MonteCarlo::Object* Obj=System.findQhull(metalCell++);
      if (!Obj)
	throw ColErr::InContainerError<int>
	  (metalCell-1,"Cell no found at layer"+StrFunc::makeString(i+1));
      const std::string ObjStr=Obj->cellCompStr()+CC.getExclude(iKey);
      Obj->procString(ObjStr);
    }

  std::string Out;
  const int outIndex=flightIndex+static_cast<int>(nLayer)*10;
  Out=ModelSupport::getComposite(SMap,outIndex," 3 -4 5 -6 ");
  Out+=attachRule;
  Out+=" "+ContainedGroup::getContainer("outer");      // Be outer surface
  Out+=ModelSupport::getComposite(SMap,flightIndex," (-3:4:-5:6) ");  
  Out+=CC.getCompExclude(iKey);
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

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
    \param iKey :: Name of inner key [from CC -- not used yet]
    \param oKey :: Name of outer key [from this]
  */
{
  ELog::RegMethod RegA("FlightLine","processIntersectMinor");

  int changeCell(flightIndex+1);
  for(size_t i=0;i<=nLayer;i++)
    {
      MonteCarlo::Object* Obj=System.findQhull(changeCell++);
      if (!Obj)
	throw ColErr::InContainerError<int>
	  (changeCell-1,"Cell no found at layer"+StrFunc::makeString(i+1));
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
    ISurf.push_back(SMap.realSurf(flightIndex+i+3));

  return;
}

void
FlightLine::createAll(Simulation& System,const size_t sideIndex,
		      const attachSystem::FixedComp& FC)
  /*!
    Global creation of the vac-vessel
    \param System :: Simulation to add vessel to
    \param sideIndex :: Side index
    \param FC :: Moderator Object
  */
{
  ELog::RegMethod RegA("FlightLine","createAll");
  populate(System);
  createUnitVector(FC,sideIndex);
  createSurfaces();

  createCapSurfaces(FC,sideIndex);

  FixedComp::setLinkSurf(0,FC,sideIndex);
  FixedComp::setLinkSurf(6,FC,sideIndex);

  createObjects(System,FC,sideIndex);
  insertObjects(System);       

  return;
}

void
FlightLine::createAll(Simulation& System,
		      const size_t orgIndex,
		      const size_t sideIndex,
		      const attachSystem::FixedComp& FC)
  /*!
    Global creation of the vac-vessel
    \param System :: Simulation to add vessel to
    \param orgIndex :: Origin index for rotation
    \param sideIndex :: Side index
    \param FC :: Moderator Object
  */
{
  ELog::RegMethod RegA("FlightLine","createAll");
  populate(System);

  createRotatedUnitVector(FC,orgIndex,sideIndex);
  createSurfaces();
  FixedComp::setLinkSurf(0,FC,sideIndex);
  FixedComp::setLinkSurf(6,FC,sideIndex);
  createObjects(System,FC,sideIndex);
  insertObjects(System);       

  return;
}

void
FlightLine::reBoundary(Simulation& System,
		       const size_t sideIndex,
		       const attachSystem::FixedComp& FC)
  /*!
    Reposition a flightline after initial construction
    \param System :: Simulation to add vessel to
    \param sideIndex :: Side index
    \param FC :: Moderator Object
  */
{
  ELog::RegMethod RegA("FlightLine","reboundary");

  removeObjects(System);
  createObjects(System,FC,sideIndex);
  insertObjects(System);       

  return;
}


void
FlightLine::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const attachSystem::ContainedComp& CC)
  /*!
    Global creation of the vac-vessel
    \param System :: Simulation to add vessel to
    \param FC :: Moderator Object
    \param CC :: Full Object
  */
{
  ELog::RegMethod RegA("FlightLine","createAll");
  populate(System);

  if (plateIndex==0)
    ELog::EM<<"Plate Index for Fligthline not set "<<ELog::endErr;
  const size_t sideIndex=static_cast<size_t>(abs(plateIndex))-1;
  const int sideSign=(plateIndex>0) ? 1 : -1;

  createUnitVector(FC,sideIndex);
  createSurfaces();
  FixedComp::setLinkSurf(0,FC,sideIndex);
  FixedComp::setLinkSurf(6,FC,sideIndex);
  createObjects(System,FC,sideSign,sideIndex,CC);
  insertObjects(System);       

  return;
}


  
}  // NAMESPACE moderatorSystem
