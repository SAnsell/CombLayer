/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/ZoomGuide.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>

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
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
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
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "KGroup.h"
#include "Source.h"
#include "SimProcess.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "chipDataStore.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "LinearComp.h"
#include "ZoomGuide.h"


namespace zoomSystem
{

ZoomGuide::ZoomGuide(const std::string& Key) : 
  attachSystem::LinearComp(),
  guideIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  keyName(Key),cellIndex(guideIndex+1),populated(0),
  nInsert(0),midBoxCell(0),nLayers(0),outerBoxCell(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: search lookup name
  */
{}

 ZoomGuide::ZoomGuide(const ZoomGuide& A) : attachSystem::LinearComp(A),
  guideIndex(A.guideIndex),keyName(A.keyName),cellIndex(A.cellIndex),
  populated(A.populated),guideLength(A.guideLength),AUp(A.AUp),
  ADown(A.ADown),ALeft(A.ALeft),ARight(A.ARight),BUp(A.BUp),
  BDown(A.BDown),BLeft(A.BLeft),BRight(A.BRight),midUp(A.midUp),
  midDown(A.midDown),midLeft(A.midLeft),midRight(A.midRight),
  roofTotal(A.roofTotal),floorTotal(A.floorTotal),leftTotal(A.leftTotal),
  rightTotal(A.rightTotal),cutterLen(A.cutterLen),focusLen(A.focusLen),
  innerCollMat(A.innerCollMat),shieldMat(A.shieldMat),nInsert(A.nInsert),
  midBoxCell(A.midBoxCell),innerFrac(A.innerFrac),innerMat(A.innerMat),
  nLayers(A.nLayers),outerBoxCell(A.outerBoxCell),outerFrac(A.outerFrac),
  outerMat(A.outerMat),Vlist(A.Vlist)
  /*!
    Copy constructor
    \param A :: ZoomGuide to copy
  */
{}

ZoomGuide&
ZoomGuide::operator=(const ZoomGuide& A)
  /*!
    Assignment operator
    \param A :: ZoomGuide to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::LinearComp::operator=(A);
      cellIndex=A.cellIndex;
      populated=A.populated;
      guideLength=A.guideLength;
      AUp=A.AUp;
      ADown=A.ADown;
      ALeft=A.ALeft;
      ARight=A.ARight;
      BUp=A.BUp;
      BDown=A.BDown;
      BLeft=A.BLeft;
      BRight=A.BRight;
      midUp=A.midUp;
      midDown=A.midDown;
      midLeft=A.midLeft;
      midRight=A.midRight;
      roofTotal=A.roofTotal;
      floorTotal=A.floorTotal;
      leftTotal=A.leftTotal;
      rightTotal=A.rightTotal;
      cutterLen=A.cutterLen;
      focusLen=A.focusLen;
      innerCollMat=A.innerCollMat;
      shieldMat=A.shieldMat;
      nInsert=A.nInsert;
      midBoxCell=A.midBoxCell;
      innerFrac=A.innerFrac;
      innerMat=A.innerMat;
      nLayers=A.nLayers;
      outerBoxCell=A.outerBoxCell;
      outerFrac=A.outerFrac;
      outerMat=A.outerMat;
      Vlist=A.Vlist;
    }
  return *this;
}

ZoomGuide::~ZoomGuide() 
  /*!
    Destructor
  */
{}

void
ZoomGuide::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  const FuncDataBase& Control=System.getDataBase();

  guideLength=Control.EvalVar<double>(keyName+"Length");

  AUp=Control.EvalVar<double>(keyName+"AUp");
  ADown=Control.EvalVar<double>(keyName+"ADown");
  ALeft=Control.EvalVar<double>(keyName+"ALeft");
  ARight=Control.EvalVar<double>(keyName+"ARight");

  BUp=Control.EvalVar<double>(keyName+"BUp");
  BDown=Control.EvalVar<double>(keyName+"BDown");
  BLeft=Control.EvalVar<double>(keyName+"BLeft");
  BRight=Control.EvalVar<double>(keyName+"BRight");

  midUp=Control.EvalVar<double>(keyName+"MidUp");
  midDown=Control.EvalVar<double>(keyName+"MidDown");
  midLeft=Control.EvalVar<double>(keyName+"MidLeft");
  midRight=Control.EvalVar<double>(keyName+"MidRight");

  roofTotal=Control.EvalVar<double>(keyName+"Roof");
  floorTotal=Control.EvalVar<double>(keyName+"Floor");
  leftTotal=Control.EvalVar<double>(keyName+"WallLeft");
  rightTotal=Control.EvalVar<double>(keyName+"WallRight");

  cutterLen=Control.EvalVar<double>(keyName+"ICutterLen");
  focusLen=Control.EvalVar<double>(keyName+"IFocusLen");

  nInsert=Control.EvalVar<int>(keyName+"NInsert");  
  nLayers=Control.EvalVar<int>(keyName+"NLayers");
  innerCollMat=Control.EvalVar<int>(keyName+"InnerCollMat");
  shieldMat=Control.EvalVar<int>(keyName+"ShieldMat");

  
  // INNER IS BASIC:
  // Note: three layers == two fractions

  if (nInsert>0)
    {
      ModelSupport::populateDivide(Control,nInsert,keyName+"InnerMat_",
				   innerCollMat,innerMat);
      ModelSupport::populateDivide(Control,nInsert,
			       keyName+"InnerFrac_",innerFrac);
    }
  else if (nInsert<0)
    {
      nInsert*=-1;
      populateInsert(Control);
    }
  // SHIELD IS BASIC:
  ModelSupport::populateDivide(Control,nLayers,keyName+"ShieldMat_",
			       shieldMat,outerMat);
  ModelSupport::populateDivide(Control,nLayers,
			       keyName+"ShieldFrac_",outerFrac);

  populated=1;
  return;
}

void
ZoomGuide::populateInsert(const FuncDataBase& Control)
  /*!
    Special function to save me populating the guide
    \param Control :: use to get change in variable 
  */
{
  innerMat.clear();
  innerFrac.clear();

  double cLen(0.0);
  double addLen[2]={cutterLen,focusLen};
  int addMat[2]={innerCollMat,0};
  int cnt(0);
  do
    {
      addMat[cnt % 2]=
	SimProcess::getDefIndexVar<int>(Control,keyName+"InnerMat_","",
					cnt,addMat[cnt % 2]);
      addLen[cnt % 2]=
	SimProcess::getDefIndexVar<double>(Control,keyName+"InnerLen_","",
					   cnt,addLen[cnt % 2]);
      cLen+=addLen[cnt % 2];
      const double frac(cLen/guideLength);
      if (frac<1.0)
        {
	  innerFrac.push_back(frac);
	  innerMat.push_back(addMat[cnt % 2]);
	  cnt++;
	}
    }
  while(cLen<guideLength);

  innerMat.push_back(addMat[cnt % 2]);
  nInsert=static_cast<int>(innerMat.size());
  return;
}

void
ZoomGuide::createUnitVector(const attachSystem::LinearComp& LC)
  /*!
    Create the unit vectors
    \param LC :: Linear Component to attach guide onto.
  */
{
  ELog::RegMethod RegA("ZoomGuide","createUnitVector");

  const masterRotate& MR=masterRotate::Instance();
  chipIRDatum::chipDataStore& CS=chipIRDatum::chipDataStore::Instance();

  attachSystem::LinearComp::createUnitVector(LC);

  setExit(Origin+Y*guideLength,Y);  
  CS.setENum(chipIRDatum::zoomGuideExit,MR.calcRotate(getExit()));  
  
  return;
}


void
ZoomGuide::createSurfaces()
  /*!
    Create All the surfaces
   */
{
  ELog::RegMethod RegA("ZoomGuide","createSurface");
  
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
   
  // INNER PLANES
  
  // Front
  Geometry::Plane* PX;
  PX=SurI.createUniqSurf<Geometry::Plane>(guideIndex+1);  
  PX->setPlane(Origin,Y);
  SMap.registerSurf(guideIndex+1,PX);

  // Back
  PX=SurI.createUniqSurf<Geometry::Plane>(guideIndex+2);  
  PX->setPlane(getExit(),Y);
  SMap.registerSurf(guideIndex+2,PX);
  setExitSurf(SMap.realSurf(guideIndex+2));

  // up
  PX=SurI.createUniqSurf<Geometry::Plane>(guideIndex+6);  
  PX->setPlane(Origin+Z*AUp,Origin+X+Z*AUp,getExit()+Z*BUp);
  PX->reversePtValid(-1,Origin);
  SMap.registerSurf(guideIndex+6,PX);

  // down
  PX=SurI.createUniqSurf<Geometry::Plane>(guideIndex+5);  
  PX->setPlane(Origin-Z*ADown,Origin+X-(Z*ADown),getExit()-Z*BDown);
  PX->reversePtValid(1,Origin);
  SMap.registerSurf(guideIndex+5,PX);

  // left
  PX=SurI.createUniqSurf<Geometry::Plane>(guideIndex+3);  
  PX->setPlane(Origin-X*ALeft,Origin+Z-(X*ALeft),getExit()-X*BLeft);
  PX->reversePtValid(1,Origin);
  SMap.registerSurf(guideIndex+3,PX);

  // right
  PX=SurI.createUniqSurf<Geometry::Plane>(guideIndex+4);  
  PX->setPlane(Origin+X*ARight,Origin+Z+(X*ARight),getExit()+X*BRight);
  PX->reversePtValid(-1,Origin);
  SMap.registerSurf(guideIndex+4,PX);
  
  // MID:

  // up
  PX=SurI.createUniqSurf<Geometry::Plane>(guideIndex+16);  
  PX->setPlane(Origin+Z*midUp,Z);
  SMap.registerSurf(guideIndex+16,PX);

  // down
  PX=SurI.createUniqSurf<Geometry::Plane>(guideIndex+15);  
  PX->setPlane(Origin-Z*midDown,Z);
  SMap.registerSurf(guideIndex+15,PX);

  // left
  PX=SurI.createUniqSurf<Geometry::Plane>(guideIndex+13);  
  PX->setPlane(Origin-X*midLeft,X);
  SMap.registerSurf(guideIndex+13,PX);

  // right
  PX=SurI.createUniqSurf<Geometry::Plane>(guideIndex+14);  
  PX->setPlane(Origin+X*midRight,X);
  SMap.registerSurf(guideIndex+14,PX);

  // :: Outer shielding ::

  // roof
  PX=SurI.createUniqSurf<Geometry::Plane>(guideIndex+26);  
  PX->setPlane(Origin+Z*roofTotal,Z);
  SMap.registerSurf(guideIndex+26,PX);

  // floor
  PX=SurI.createUniqSurf<Geometry::Plane>(guideIndex+25);  
  PX->setPlane(Origin-Z*floorTotal,Z);
  SMap.registerSurf(guideIndex+25,PX);

  // left
  PX=SurI.createUniqSurf<Geometry::Plane>(guideIndex+23);  
  PX->setPlane(Origin-X*leftTotal,X);
  SMap.registerSurf(guideIndex+23,PX);

  // right
  PX=SurI.createUniqSurf<Geometry::Plane>(guideIndex+24);  
  PX->setPlane(Origin+X*rightTotal,X);
  SMap.registerSurf(guideIndex+24,PX);
  
  return;
}

void
ZoomGuide::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("ZoomGuide","createObjects");
  Vlist.clear();

  std::string Out;
  // Inner box:
  Out=ModelSupport::getComposite(SMap,guideIndex,"1 -2 3 -4 5 -6");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  // Mid Box
  Out=ModelSupport::getComposite(SMap,guideIndex,
				 "1 -2 13 -14 15 -16 (-3 : 4 : -5 : 6)");
  midBoxCell=cellIndex;
  System.addCell(MonteCarlo::Qhull(cellIndex++,innerCollMat,0.0,Out));
  
  // Outer Box
  Out=ModelSupport::getComposite(SMap,guideIndex,
				 "1 -2 23 -24 25 -26 (-13 : 14 : -15 : 16)");
  outerBoxCell=cellIndex;
  System.addCell(MonteCarlo::Qhull(cellIndex++,shieldMat,0.0,Out));



  Out=ModelSupport::getComposite(SMap,guideIndex,"1 -2 23 -24 25 -26");
  System.addCell(MonteCarlo::Qhull(cellIndex,0,0.0,Out));
  System.makeVirtual(cellIndex);
  Vlist.push_back(cellIndex);
  cellIndex++;
    
  return;
}

void
ZoomGuide::addOuterVoid(Simulation& System)
  /*!
    Create outer virtual space that includes the beamstop etc
    \param System :: Simulation to add to 
   */
{
  ELog::RegMethod RegA("ZoomGuide","addOuterVoid");

  MonteCarlo::Qhull* OuterVoid=System.findQhull(74123);
  if (OuterVoid)
    {
      if (!Vlist.empty())
	{
	  std::ostringstream cx;
	  std::vector<int>::const_iterator vc;
	  for(vc=Vlist.begin();vc!=Vlist.end();vc++)
	    cx<<" #"<<*vc;
	  OuterVoid->addSurfString(cx.str()); 
	}
    }
  else
    ELog::EM.error("Failed to find OuterVoid ");

  return;
}

int
ZoomGuide::exitWindow(const double Dist,std::vector<int>& window,
		      Geometry::Vec3D& Pt) const
  /*!
    Outputs a window for a point tally
    \param Dist :: Distance from window
    \param window :: window vector of paired planes
    \param Pt :: Point for 
    \return Point at exit + Dist
  */
{
  window.clear();
  window.push_back(SMap.realSurf(guideIndex+23));
  window.push_back(SMap.realSurf(guideIndex+24));
  window.push_back(SMap.realSurf(guideIndex+25));
  window.push_back(SMap.realSurf(guideIndex+26));
  Pt=getExit()+Y*Dist;  
  return SMap.realSurf(guideIndex+2);
}


void 
ZoomGuide::layerProcess(Simulation& System)
  /*!
    Processes the splitting of the surfaces into a multilayer system
    \param System :: Simulation to work on
  */
{
  ELog::RegMethod RegA("ZoomGuide","LayerProcess");
  
  if (nInsert && midBoxCell)
    {
      ModelSupport::surfDivide DA;
      // Generic
      for(int i=0;i<nInsert-1;i++)
	{
	  DA.addFrac(innerFrac[i]);
	  DA.addMaterial(innerMat[i]);
	}
      DA.addMaterial(innerMat[nInsert-1]);
      
      DA.setCellN(midBoxCell);
      DA.setOutNum(cellIndex,guideIndex+201);
      DA.makePair<Geometry::Plane>(SMap.realSurf(guideIndex+1),
		       -SMap.realSurf(guideIndex+2));
      DA.activeDivide(System);
      cellIndex=DA.getCellNum();
    }
  
  if (nLayers && outerBoxCell) 
    { 
      ModelSupport::surfDivide DA;
  
      // Generic
      for(int i=0;i<nLayers-1;i++)
        {
	  DA.addFrac(outerFrac[i]);
	  DA.addMaterial(outerMat[i]);
	}
      DA.addMaterial(outerMat[nLayers-1]);
      // Cell Specific:
      DA.setCellN(outerBoxCell);
      DA.setOutNum(cellIndex,guideIndex+301);
      DA.makePair<Geometry::Plane>(SMap.realSurf(guideIndex+13),
				   SMap.realSurf(guideIndex+23));
      DA.makePair<Geometry::Plane>(SMap.realSurf(guideIndex+14),
				   -SMap.realSurf(guideIndex+24));
      DA.makePair<Geometry::Plane>(SMap.realSurf(guideIndex+15),
				  SMap.realSurf(guideIndex+25));
      DA.makePair<Geometry::Plane>(SMap.realSurf(guideIndex+16),
				   -SMap.realSurf(guideIndex+26));
      DA.activeDivide(System);
    }
  return;
}
  
void
ZoomGuide::createAll(Simulation& System,const attachSystem::LinearComp& LC)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param LC :: Zoom Bend to connect box to
  */
{
  ELog::RegMethod RegA("ZoomGuide","createAll");
  populate(System);
  createUnitVector(LC);
  createSurfaces();
  createObjects(System);
  addOuterVoid(System);
  layerProcess(System);
  return;
}
  
}  // NAMESPACE shutterSystem
