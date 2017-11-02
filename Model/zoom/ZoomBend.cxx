/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   zoom/ZoomBend.cxx
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
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
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
#include "shutterBlock.h"
#include "SimProcess.h"
#include "surfDIter.h"
#include "SurInter.h"
#include "surfDBase.h"
#include "mergeTemplate.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "chipDataStore.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "GeneralShutter.h"
#include "collInsertBase.h"
#include "collInsertBlock.h"
#include "ZoomShutter.h"
#include "BulkInsert.h"
#include "BulkShield.h"
#include "bendSection.h"
#include "ZoomBend.h"

namespace zoomSystem
{

ZoomBend::ZoomBend(const std::string& Key)  : 
  attachSystem::ContainedGroup("A","B","C","D"),
  attachSystem::TwinComp(Key,6),
  bendIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(bendIndex+1),populated(0),
  innerCell(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Key to use
  */
{} 

ZoomBend::ZoomBend(const ZoomBend& A) : 
  attachSystem::ContainedGroup(A),attachSystem::TwinComp(A),
  bendIndex(A.bendIndex),cellIndex(A.cellIndex),
  populated(A.populated),BCentre(A.BCentre),normalOut(A.normalOut),
  bendAngle(A.bendAngle),bendVertAngle(A.bendVertAngle),
  bendXAngle(A.bendXAngle),bendRadius(A.bendRadius),
  bendLength(A.bendLength),NVanes(A.NVanes),bendWidth(A.bendWidth),
  bendHeight(A.bendHeight),vaneThick(A.vaneThick),
  vaneMat(A.vaneMat),xStep(A.xStep),yStep(A.yStep),zStep(A.zStep),
  BSector(A.BSector),wallThick(A.wallThick),innerMat(A.innerMat),
  wallMat(A.wallMat),innerCell(A.innerCell)
  /*!
    Copy constructor
    \param A :: ZoomBend to copy
  */
{}

ZoomBend&
ZoomBend::operator=(const ZoomBend& A)
  /*!
    Assignment operator
    \param A :: ZoomBend to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedGroup::operator=(A);
      attachSystem::TwinComp::operator=(A);
      cellIndex=A.cellIndex;
      populated=A.populated;
      BCentre=A.BCentre;
      normalOut=A.normalOut;
      bendAngle=A.bendAngle;
      bendVertAngle=A.bendVertAngle;
      bendXAngle=A.bendXAngle;
      bendRadius=A.bendRadius;
      bendLength=A.bendLength;
      NVanes=A.NVanes;
      bendWidth=A.bendWidth;
      bendHeight=A.bendHeight;
      vaneThick=A.vaneThick;
      vaneMat=A.vaneMat;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      BSector=A.BSector;
      wallThick=A.wallThick;
      innerMat=A.innerMat;
      wallMat=A.wallMat;
      innerCell=A.innerCell;
    }
  return *this;
}



ZoomBend::~ZoomBend() 
  /*!
    Destructor
  */
{}

void
ZoomBend::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("ZoomBend","populate");

  const FuncDataBase& Control=System.getDataBase();

  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");

  bxStep=Control.EvalVar<double>(keyName+"EnterXStep");
  bzStep=Control.EvalVar<double>(keyName+"EnterZStep");
  
  bendAngle=Control.EvalVar<double>(keyName+"Angle");
  bendVertAngle=Control.EvalVar<double>(keyName+"VertAngle");
  bendXAngle=Control.EvalVar<double>(keyName+"XAngle");
  
  bendLength=0.0;
  for(size_t i=0;i<4;i++)
    {
      const std::string kN=StrFunc::makeString(keyName+"Sec",i+1);
      BSector[i].xStep=Control.EvalVar<double>(kN+"XStep");
      BSector[i].zStep=Control.EvalVar<double>(kN+"ZStep");
      BSector[i].length=Control.EvalVar<double>(kN+"Length");
      BSector[i].width=Control.EvalVar<double>(kN+"Width");
      BSector[i].height=Control.EvalVar<double>(kN+"Height");
      bendLength+=BSector[i].length;
    }
  
  nAttn=Control.EvalVar<size_t>(keyName+"NAttn");
  attnZStep=Control.EvalVar<double>(keyName+"AttnZStep");
  upperYPos.clear();
  upperDist.clear();
  for(size_t i=0;i<nAttn;i++)
    {
      const std::string kN=StrFunc::makeString(keyName+"Attn",i+1);
      upperYPos.push_back(Control.EvalVar<double>(kN+"YPos"));
      upperDist.push_back(Control.EvalVar<double>(kN+"Dist"));
    }

  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  innerMat=ModelSupport::EvalMat<int>(Control,keyName+"InnerMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  // Stuff for the bender
  bendWidth=Control.EvalVar<double>(keyName+"PathWidth");
  bendHeight=Control.EvalVar<double>(keyName+"PathHeight");
  vaneThick=Control.EvalVar<double>(keyName+"VaneThick");
  vaneMat=ModelSupport::EvalMat<int>(Control,keyName+"VaneMat");
  NVanes=Control.EvalVar<size_t>(keyName+"NVanes");

  populated=1;
  return;
}

void
ZoomBend::createUnitVector(const attachSystem::FixedComp& ZS)
  /*!
    Create the unit vectors.
    Note the outer core goes down teh beam
    \param ZS :: shutter direction
  */
{
  ELog::RegMethod RegA("ZoomBend","createUnitVector");

  chipIRDatum::chipDataStore& CS=chipIRDatum::chipDataStore::Instance();
  const masterRotate& MR=masterRotate::Instance();
  // This must be checked but the EXIT needs to be the GENERAL 
  // exit 
  
  const attachSystem::TwinComp* TCPtr=
    dynamic_cast<const attachSystem::TwinComp*>(&ZS);
  if (TCPtr)
    attachSystem::TwinComp::createUnitVector(*TCPtr);
  else
    {
      attachSystem::FixedComp::createUnitVector(ZS);
      Z*=-1;
      bX=X;
      bY=Y;
      bZ=Z;
      const Geometry::Quaternion Qbx=
	Geometry::Quaternion::calcQRot(bendXAngle/1000.0,X);
      Qbx.rotate(bZ);
      Qbx.rotate(bY);
      bEnter=ZS.getCentre();
      bEnter+= X*bxStep+Z*bzStep;
    }
  // link point 
  //  FixedComp::createUnitVector(ZS.getBackPt(),ZS.getY());  

  Origin+=X*xStep+Y*yStep+Z*zStep;
  Geometry::Vec3D Diff=bEnter-Origin;
  // TODO :: 
  // STUFF FOR BEND OCCURS AT DIFFERENT PLACE :

  // Calculate the difference between the Y and the bendYAxis
  // to correctly set bend length
  bendLength/=cos(bendAngle/1000.0);

  // Rotate beamAxis to the final angle [ bendAngle in mRad]
  // Note : the sign of the angle dertermine which side of
  //    the curve to go. Need -ve to go out from the shutter.
  // - The Centre +/- sign on X determines the curve direction.
  // -- current X is -ve -ve 0 so need to subtract to bend away
  //    from E2.

  bendRadius=1000.0*bendLength/bendAngle;
  if(bendRadius<0)
    {
      bZ*=-1;
      //      bX=-1;
      bendRadius*=-1;
      bendAngle*=-1;
    }
  normalOut=bY;
  zOut=bZ;
  Geometry::Quaternion::calcQRot(bendAngle/1000.0,bX).rotate(normalOut);
  Geometry::Quaternion::calcQRot(bendAngle/1000.0,bX).rotate(zOut);

  BCentre=bEnter-bZ*bendRadius;  // correct: X is tangent at this pt.
  bExit= bZ*bendRadius;
  Geometry::Quaternion::calcQRot(bendAngle/1000.0,bX).rotate(bExit);
  bExit += BCentre;


  SecondTrack::setBeamExit(bExit,normalOut);
  CS.setVNum(chipIRDatum::zoomBendLength,bendLength);
  CS.setVNum(chipIRDatum::zoomBendRadius,bendRadius);
  CS.setENum(chipIRDatum::zoomBendShutter,MR.calcRotate(bEnter));  
  CS.setENum(chipIRDatum::zoomBendStart,MR.calcRotate(Origin));  
  CS.setENum(chipIRDatum::zoomBendCentre,MR.calcRotate(BCentre));  
  CS.setENum(chipIRDatum::zoomBendExit,MR.calcRotate(bExit));  
  CS.setENum(chipIRDatum::zoomBendStartNorm,MR.calcAxisRotate(bY));  
  CS.setENum(chipIRDatum::zoomBendExitNorm,MR.calcAxisRotate(normalOut));  
  return;
}

void
ZoomBend::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("ZoomBend","createSurface");

  // Forward Plane:
  ModelSupport::buildPlane(SMap,bendIndex+1,Origin,Y);  

  double sectorLen(0.0);
  for(size_t i=0;i<4;i++)
    {
      const int offset(bendIndex+static_cast<int>(i)*10);
      const Geometry::Vec3D effO=Origin+Y*sectorLen+
	X*BSector[i].xStep+Z*BSector[i].zStep;

      // Match to previous start surface
      if (i)
       	SMap.addMatch(offset+1,offset-8);         

      ModelSupport::buildPlane(SMap,offset+2,
			       effO+Y*BSector[i].length,Y);  
      ModelSupport::buildPlane(SMap,offset+3,
			       effO-X*BSector[i].width/2.0,X);  
      ModelSupport::buildPlane(SMap,offset+4,
			       effO+X*BSector[i].width/2.0,X);  
      ModelSupport::buildPlane(SMap,offset+5,
			       effO-Z*BSector[i].height/2.0,Z);  
      ModelSupport::buildPlane(SMap,offset+6,
			       effO+Z*BSector[i].height/2.0,Z);  

      ModelSupport::buildPlane(SMap,offset+101,
			       effO+Y*wallThick,Y);  
      ModelSupport::buildPlane(SMap,offset+102,
			       effO+Y*(BSector[i].length-wallThick),Y);  
      ModelSupport::buildPlane(SMap,offset+103,
			       effO-X*(BSector[i].width/2.0-wallThick),X);  
      ModelSupport::buildPlane(SMap,offset+104,
			       effO+X*(BSector[i].width/2.0-wallThick),X);  
      ModelSupport::buildPlane(SMap,offset+105,
			       effO-Z*(BSector[i].height/2.0-wallThick),Z);  
      ModelSupport::buildPlane(SMap,offset+106,
			       effO+Z*(BSector[i].height/2.0-wallThick),Z);  

      sectorLen+=BSector[i].length;
    }

  // Create Bend [Edges]
  
  ModelSupport::buildPlane(SMap,bendIndex+505,
			   bEnter-bX*(bendWidth/2.0),bX);
  ModelSupport::buildPlane(SMap,bendIndex+506,
			   bEnter+bX*(bendWidth/2.0),bX);
  
  ModelSupport::buildCylinder(SMap,bendIndex+503,BCentre+bZ*(bendHeight/2.0),
			      bX,fabs(bendRadius));
  ModelSupport::buildCylinder(SMap,bendIndex+504,BCentre-bZ*(bendHeight/2.0),
			      bX,fabs(bendRadius));  
  return;
}

void
ZoomBend::createAttenuation(Simulation& System)
  /*!
    Construct all the little square intersections
    \param System :: Simualation to add objects too
  */
{
  ELog::RegMethod RegA("ZoomBend","createAttenuation");

  // Create +/- insert bend
  ModelSupport::buildCylinder(SMap,bendIndex+1003,
			      BCentre+bZ*(attnZStep+bendHeight/2.0),
			      bX,fabs(bendRadius));
  ModelSupport::buildCylinder(SMap,bendIndex+1004,
			      BCentre-bZ*(attnZStep+bendHeight/2.0),
			      bX,fabs(bendRadius));
  
  std::string Out;
  int ONum(bendIndex+1000);
  for(size_t i=0;i<nAttn;i++)
    {
      const Geometry::Vec3D APt(Origin+bY*(upperYPos[i]-upperDist[i]/2.0));  
      const Geometry::Vec3D BPt(Origin+bY*(upperYPos[i]+upperDist[i]/2.0));  
      ModelSupport::buildPlane(SMap,ONum+1,APt,bY);
      ModelSupport::buildPlane(SMap,ONum+2,BPt,bY);

      Out=ModelSupport::getComposite(SMap,bendIndex,ONum,
				     "1M -2M 503 -1003 505 -506 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

      Out=ModelSupport::getComposite(SMap,bendIndex,ONum,
				     " (-1M:2M:-503:1003:-505:506) ");
      double sectorLenA(0.0);
      for(size_t j=0;j<4;j++)
	{ 
	  double sectorLenB(BSector[j].length+sectorLenA);
	  const double PA=upperYPos[i]-upperDist[i]/2.0;
	  const double PB=upperYPos[i]+upperDist[i]/2.0;
	  if ((PA>=sectorLenA && PA<=sectorLenB) ||
	      (PB>=sectorLenA && PB<=sectorLenB) )
	    {
	      MonteCarlo::Qhull* CRPtr=System.findQhull(innerShield[j]);
	      if (!CRPtr)
		throw ColErr::InContainerError<int>(innerShield[j],"inner shield cell");
	      CRPtr->addSurfString(Out);
	    }
	  sectorLenA=sectorLenB;
	}
      ONum+=10;
    }

  return;
}

void
ZoomBend::createVanes(Simulation& System)
  /*!
    Construct the vane object
    \param System :: System object
   */
{
  ELog::RegMethod RegA("ZoomBend","createVanes");

  if (NVanes)
    {
      const double DNVanes(static_cast<double>(NVanes));
      std::vector<double> VGap;
      std::vector<int> Mat;
      // first construct fractional gap
      const double SiFrac=vaneThick/bendWidth;
      const double VacFrac=(1.0-SiFrac*DNVanes)/(DNVanes+1.0);

      if (VacFrac<=0.0)
	throw ColErr::RangeError<double>(SiFrac*DNVanes,0.0,1.0,"VacFrac");
      
      // Construct layer process:
      ModelSupport::surfDivide DA;
      double gapSum=0.0;

      for(size_t i=0;i<NVanes;i++)
	{ 
	  gapSum+=VacFrac;
	  DA.addFrac(gapSum);
	  DA.addMaterial(0);
	  gapSum+=SiFrac;
	  DA.addFrac(gapSum);
	  DA.addMaterial(vaneMat);
	}
      DA.addMaterial(0);

      DA.init();
      DA.setCellN(innerCell);
      DA.setOutNum(cellIndex,bendIndex+801);

      // Modern divider system:
      ModelSupport::mergeTemplate<Geometry::Cylinder,
				  Geometry::Cylinder> vaneRule;
      vaneRule.setSurfPair(SMap.realSurf(bendIndex+503),
			   SMap.realSurf(bendIndex+504));
      
      const std::string OutA=
	ModelSupport::getComposite(SMap,bendIndex," -503 ");
      const std::string OutB=
	ModelSupport::getComposite(SMap,bendIndex," 504 ");
      vaneRule.setInnerRule(OutA);
      vaneRule.setOuterRule(OutB);
  
      DA.addRule(&vaneRule);
      DA.activeDivideTemplate(System);
      cellIndex=DA.getCellNum();
    }
  return;
}

int
ZoomBend::getSectionSurf(const int SecN,const int SurfN) const
  /*!
    Get a section number of use in the chopper etc.
    \param SecN :: Section number
    \param SurfN :: Surface number
    \return Surface number 
   */
{
  ELog::RegMethod RegA("ZoomBend","createObjects");
  const int NSection(4);
  if (SecN>=NSection || SecN<0)
    throw ColErr::IndexError<int>(SecN,NSection,"Sec number at "+
				  RegA.getFull());
  if (SurfN>6 || SurfN<1)
    throw ColErr::IndexError<int>(SurfN,NSection,"Surf number at "+
				  RegA.getFull());

  return SMap.realSurf(bendIndex+10*SecN+SurfN);
}

void
ZoomBend::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("ZoomBend","createObjects");
  const int NSection(4);
  const char* SecName[NSection]={"A","B","C","D"};
  
  const std::string Inner=
    ModelSupport::getComposite(SMap,bendIndex," (503:-504:-505:506) ");
  std::string Out;
  
  
  innerShield.clear();
  for(int i=0;i<NSection;i++)
    {
      const int offset(bendIndex+i*10);
      Out=ModelSupport::getComposite(SMap,offset,"1 -2 3 -4 5 -6");
      addOuterUnionSurf(SecName[i],Out);      
      Out=ModelSupport::getComposite(SMap,offset,"1 -2 3 -4 5 -6 "
				     "(-101:102:-103:104:-105:106)");
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,0.0,Out+Inner));

      Out=ModelSupport::getComposite(SMap,offset,"101 -102 103 -104 105 -106");
      System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat,0.0,Out+Inner));
      innerShield.push_back(cellIndex-1);
    }
  // Create Bend itself [NOTE DEPENDENT ON NSection]
  Out=ModelSupport::getComposite(SMap,bendIndex,"1 -32 -503 504 505 -506 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  innerCell=cellIndex-1;
  return;
}

  
int
ZoomBend::exitWindow(const double Dist,std::vector<int>& window,
		     Geometry::Vec3D& Pt) const
  /*!
    Outputs a window
    \param Dist :: Dist from exit point
    \param window :: window vector of paired planes
    \param Pt :: Output point for tally
    \return Master Plane
  */
{
  window.clear();
  window.push_back(SMap.realSurf(bendIndex+13));
  window.push_back(SMap.realSurf(bendIndex+14));
  window.push_back(SMap.realSurf(bendIndex+15));
  window.push_back(SMap.realSurf(bendIndex+16));
  Pt=getExit()+normalOut*Dist;  
  return SMap.realSurf(bendIndex+31); 
}

void
ZoomBend::createLinks()
  /*!
    Set Exit -- and link points
  */
{
  ELog::RegMethod RegA("ZoomBend","createLinks");

  Y=normalOut-Z*(normalOut.dotProd(Z));
  Y.makeUnit();
  X=Y*Z;

  // Determine the low-left and upper-right corner of the bend intersect
  std::vector<Geometry::Vec3D> Pts=
    SurInter::makePoint
    (SMap.realPtr<Geometry::Plane>(bendIndex+32),
     SMap.realPtr<Geometry::Plane>(bendIndex+506),
     SMap.realPtr<Geometry::Cylinder>(bendIndex+504));

  // find closest point in the set
  Geometry::Vec3D MidPt=
    SurInter::nearPoint(Pts,Origin+Y*bendLength);
  // 
  Pts=SurInter::makePoint
    (SMap.realPtr<Geometry::Plane>(bendIndex+32),
     SMap.realPtr<Geometry::Plane>(bendIndex+505),
     SMap.realPtr<Geometry::Cylinder>(bendIndex+503));
  MidPt+=SurInter::nearPoint(Pts,Origin+Y*bendLength);
  MidPt/=2.0;

  // Set inner link points:[pointing into the void]
  BCentre=bEnter-bX*bendRadius;  // correct because X is tangent at this point
  FixedComp::setConnect(2,bEnter-bX*(bendHeight/2.0),bX);
  FixedComp::setConnect(3,bEnter+bX*(bendHeight/2.0),-bX);
  FixedComp::setConnect(4,bEnter-bZ*(bendWidth/2.0),bZ);
  FixedComp::setConnect(5,bEnter+bZ*(bendWidth/2.0),-bZ);

  FixedComp::setLinkSurf(2,SMap.realSurf(bendIndex+504));
  FixedComp::setLinkSurf(3,-SMap.realSurf(bendIndex+503));
  FixedComp::setLinkSurf(4,SMap.realSurf(bendIndex+505));
  FixedComp::setLinkSurf(5,-SMap.realSurf(bendIndex+506));


  SecondTrack::setBeamExit(MidPt,normalOut);
  setExit(Origin+Y*bendLength,Y);
  return;
}

void
ZoomBend::createAll(Simulation& System,
		    const attachSystem::FixedComp& ZC)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param ZC :: Zoom Shutter / FixedPoint origin
  */
{
  ELog::RegMethod RegA("ZoomBend","createAll");


  populate(System);
  createUnitVector(ZC);
  createSurfaces();
  createObjects(System);
  createVanes(System);

  createAttenuation(System);
  insertObjects(System);       
  createLinks();

  // Adjust bY for exit

  bY=normalOut;
  bZ=zOut;
  return;
}
  
}  // NAMESPACE zoomSystem
