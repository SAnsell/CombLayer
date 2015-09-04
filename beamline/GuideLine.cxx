/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   beamline/GuideLine.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include "stringCombine.h"
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
#include "surfDivide.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "Rules.h"
#include "surfFunctors.h"
#include "SurInter.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "RuleSupport.h"
#include "Object.h"
#include "Qhull.h"
#include "SimProcess.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"  
#include "FixedComp.h" 
#include "ContainedComp.h"
#include "FixedGroup.h" 
#include "ShapeUnit.h"
#include "PlateUnit.h"
#include "BenderUnit.h"
#include "GuideLine.h"

#include "debugMethod.h"

namespace beamlineSystem
{

GuideLine::GuideLine(const std::string& Key) : 
  attachSystem::ContainedComp(),
  attachSystem::FixedGroup(Key,"Shield",6,"GuideOrigin",0),
  SUItem(200),SULayer(20),
  guideIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(guideIndex+1),nShapeLayers(0),activeEnd(0),  
  nShapes(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

GuideLine::GuideLine(const GuideLine& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedGroup(A),
  SUItem(A.SUItem),SULayer(A.SULayer),
  guideIndex(A.guideIndex),cellIndex(A.cellIndex),
  xStep(A.xStep),yStep(A.yStep),zStep(A.zStep),
  xyAngle(A.xyAngle),zAngle(A.zAngle),beamXStep(A.beamXStep),
  beamYStep(A.beamYStep),beamZStep(A.beamZStep),
  beamXYAngle(A.beamXYAngle),beamZAngle(A.beamZAngle),
  length(A.length),height(A.height),depth(A.depth),
  leftWidth(A.leftWidth),rightWidth(A.rightWidth),
  nShapeLayers(A.nShapeLayers),layerThick(A.layerThick),
  layerMat(A.layerMat),activeEnd(A.activeEnd),
  endCut(A.endCut),nShapes(A.nShapes),
  activeShield(A.activeShield),feMat(A.feMat)
  /*!
    Copy constructor
    \param A :: GuideLine to copy
  */
{
  std::vector<ShapeUnit*>::const_iterator vc;
  for(vc=shapeUnits.begin();vc!=shapeUnits.end();vc++)
    shapeUnits.push_back((*vc)->clone());
}

GuideLine&
GuideLine::operator=(const GuideLine& A)
  /*!
    Assignment operator
    \param A :: GuideLine to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedGroup::operator=(A);
      cellIndex=A.cellIndex;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
      beamXStep=A.beamXStep;
      beamYStep=A.beamYStep;
      beamZStep=A.beamZStep;
      beamXYAngle=A.beamXYAngle;
      beamZAngle=A.beamZAngle;
      frontCut=A.frontCut;
      beamFrontCut=A.beamFrontCut;
      length=A.length;
      height=A.height;
      depth=A.depth;
      leftWidth=A.leftWidth;
      rightWidth=A.rightWidth;
      nShapeLayers=A.nShapeLayers;
      layerThick=A.layerThick;
      layerMat=A.layerMat;
      nShapes=A.nShapes;
      activeEnd=A.activeEnd;
      endCut=A.endCut;
      activeShield=A.activeShield;
      feMat=A.feMat;
    }
  return *this;
}


GuideLine::~GuideLine() 
  /*!
    Destructor
  */
{
  clear();
}

void
GuideLine::clear()
  /*!
    Erase everything
  */
{
  ELog::RegMethod RegA("GuideLine","clear");

  for(ShapeUnit* SU : shapeUnits)
    delete SU;
  return;
}


void
GuideLine::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: FuncDataBase to add
  */
{
  ELog::RegMethod RegA("GuideLine","populate");

  xStep=Control.EvalDefVar<double>(keyName+"XStep",0.0);
  yStep=Control.EvalDefVar<double>(keyName+"YStep",0.0);
  zStep=Control.EvalDefVar<double>(keyName+"ZStep",0.0);
  xyAngle=Control.EvalDefVar<double>(keyName+"XYAngle",0.0);
  zAngle=Control.EvalDefVar<double>(keyName+"ZAngle",0.0);

  beamXStep=Control.EvalDefVar<double>(keyName+"BeamXStep",xStep);
  beamYStep=Control.EvalDefVar<double>(keyName+"BeamYStep",yStep);
  beamZStep=Control.EvalDefVar<double>(keyName+"BeamZStep",zStep);

  beamXYAngle=Control.EvalDefVar<double>(keyName+"BeamXYAngle",xyAngle);
  beamZAngle=Control.EvalDefVar<double>(keyName+"BeamZAngle",xyAngle);

  activeShield=Control.EvalDefVar<int>(keyName+"ActiveShield",1);    
  length=Control.EvalVar<double>(keyName+"Length");
  
  if (activeShield)
    {
      height=Control.EvalVar<double>(keyName+"Height");
      depth=Control.EvalVar<double>(keyName+"Depth");
      leftWidth=Control.EvalVar<double>(keyName+"LeftWidth");
      rightWidth=Control.EvalVar<double>(keyName+"RightWidth");
      feMat=ModelSupport::EvalMat<int>(Control,keyName+"FeMat");
    }
  nShapes=Control.EvalVar<size_t>(keyName+"NShapes");
  nShapeLayers=Control.EvalVar<size_t>(keyName+"NShapeLayers");
  
  double T(0.0);
  int M;
  for(size_t i=0;i<nShapeLayers;i++)
    {
      const std::string NStr=StrFunc::makeString(i);
      if (i)
	T+=Control.EvalVar<double>(keyName+"LayerThick"+NStr);
      // Always get material
      M=ModelSupport::EvalMat<int>(Control,keyName+"LayerMat"+NStr);
      layerThick.push_back(T);
      layerMat.push_back(M);
    }

  // set frontcut based on offset:
  beamFrontCut=(fabs(beamYStep)>Geometry::zeroTol) ? 1 : 0;
  frontCut=(fabs(yStep)>Geometry::zeroTol) ? 1 : 0;
  return;
}

void
GuideLine::addGuideUnit(const size_t index,
			const Geometry::Vec3D& POrigin,
			const double bX,const double bZ,
			const double bXYang,const double bZang)
  /*!
    Set the guide unit to the fixed system
    The direction/rotation are applied to previous fixed unit
    out track.

    \param Index :: index for the unit
    \param POrigin :: Previous Origin [link on]
    \param bX :: X shift on origin
    \param bZ :: Z shift on origin
    \param bXYang :: xy Angle rotation [deg]
    \param bZang :: z Angle rotation [deg]
  */
{
  ELog::RegMethod RegA("GuideLine","addGuideUnit");

  const std::string GKey="Guide"+StrFunc::makeString(index);

  attachSystem::FixedComp& guideFC=FixedGroup::addKey(GKey,2);
  
  const std::string PGKey=(index) ? 
    "Guide"+StrFunc::makeString(index-1) :  "GuideOrigin";
  attachSystem::FixedComp& prevFC=FixedGroup::getKey(PGKey);

  guideFC.createUnitVector(prevFC,POrigin);
  guideFC.applyShift(bX,0.0,bZ);
  guideFC.applyAngleRotate(bXYang,bZang);
    
  return;
}

void
GuideLine::checkRectangle(const double W,const double H) const
  /*!
    Check the rectangle size relative to the outside values
    \param W :: Width
    \param H :: Height
  */
{
  ELog::RegMethod RegA("GuideLine","checkRectangle");

  if (activeShield)
    {
      const double TThick=
	std::accumulate(layerThick.begin(),layerThick.end(),0.0);
      
      if ((TThick+W/2.0)>leftWidth || (TThick+W/2.0)>rightWidth)
	ELog::EM<<"Guide["<<keyName<<"] rectangle width/thick "<<W<<":"<<TThick
		<<" > ("<<leftWidth<<":"<<rightWidth<<")"<<ELog::endErr;
      if ((TThick+H/2.0)>height || (TThick+H/2)>depth)
	ELog::EM<<"Guide["<<keyName<<"] rectangle height/thick "<<H<<":"<<TThick
		<<" > ("<<depth<<":"<<height<<")"<<ELog::endErr;
    }

  return;
}
  
void
GuideLine::processShape(const FuncDataBase& Control)
  /*!
    Build a simple shape component from the Control Values
    \param Control :: Fundermental variables for shapes
   */
{
  ELog::RegMethod RegA("GuideLine","processShape");

  const attachSystem::FixedComp& guideFC=FixedGroup::getKey("GuideOrigin");
  
  Geometry::Vec3D StartPt=guideFC.getCentre();
  double beamX,beamZ,bXYang,bZang;

  for(size_t index=0;index<nShapes;index++)
    {
      const std::string NStr=StrFunc::makeString(index);
      const std::string BKey=keyName+NStr;

      const std::string typeID= 
	Control.EvalVar<std::string>(keyName+NStr+"TypeID");
      // PROCESS NEXT SECTION:
      // Beam position [def original]

      // Initial directions:
      beamX=Control.EvalDefVar<double>(BKey+"XStep",0.0);
      beamZ=Control.EvalDefVar<double>(BKey+"ZStep",0.0);
      bXYang=Control.EvalDefVar<double>(BKey+"XYAngle",0.0);
      bZang=Control.EvalDefVar<double>(BKey+"ZAngle",0.0);

      if (index)
	{
	  addGuideUnit(index,shapeUnits.back()->getEnd(),
		       beamX,beamZ,bXYang,bZang);
	}
      else
	addGuideUnit(index,StartPt,beamX,beamZ,bXYang,bZang);

      setDefault("Guide"+NStr);
      const double L=Control.EvalVar<double>(keyName+NStr+"Length");      

      // Simple rectangle projection:
      // ALL PROJECTIONS IN ROTATED DISTANCE
      const int GINumber(guideIndex+SUItem*static_cast<int>(index+1));
      if (typeID=="Rectangle")   
	{
	  PlateUnit* SU=new PlateUnit(GINumber,SULayer);
	  const double H=Control.EvalVar<double>(keyName+NStr+"Height");
	  const double W=Control.EvalVar<double>(keyName+NStr+"Width");

	  SU->addPrimaryPoint(Geometry::Vec3D(-W/2.0,0,-H/2.0));
	  SU->addPrimaryPoint(Geometry::Vec3D(W/2.0,0,-H/2.0));
	  SU->addPrimaryPoint(Geometry::Vec3D(W/2.0,0,H/2.0));
	  SU->addPrimaryPoint(Geometry::Vec3D(-W/2.0,0,H/2.0));
	  SU->setEndPts(Origin,Origin+Y*L);
	  SU->setXAxis(X,Z);      
	  SU->constructConvex();
	  shapeUnits.push_back(SU);

	  checkRectangle(W,H);
	}
      else if (typeID=="Tapper")   
	{
	  PlateUnit* SU=new PlateUnit(GINumber,SULayer);
	  const double HA=Control.EvalVar<double>(keyName+NStr+"HeightStart");
	  const double WA=Control.EvalVar<double>(keyName+NStr+"WidthStart");
	  const double HB=Control.EvalVar<double>(keyName+NStr+"HeightEnd");
	  const double WB=Control.EvalVar<double>(keyName+NStr+"WidthEnd");

	  SU->addPairPoint(Geometry::Vec3D(-WA/2.0,0.0,-HA/2.0),
			   Geometry::Vec3D(-WB/2.0,0.0,-HB/2.0));
	  SU->addPairPoint(Geometry::Vec3D(WA/2.0,0.0,-HA/2.0),
			   Geometry::Vec3D(WB/2.0,0.0,-HB/2.0));
	  SU->addPairPoint(Geometry::Vec3D(WA/2.0,0.0,HA/2.0),
			   Geometry::Vec3D(WB/2.0,0.0,HB/2.0));
	  SU->addPairPoint(Geometry::Vec3D(-WA/2.0,0.0,HA/2.0),
			   Geometry::Vec3D(-WB/2.0,0.0,HB/2.0));

	  SU->setEndPts(Origin,Origin+Y*L);      	  
	  SU->setXAxis(X,Z);      
	  SU->constructConvex();
	  shapeUnits.push_back(SU);

	}
      else if (typeID=="Bend")
	{
	  BenderUnit* BU=new BenderUnit(GINumber,SULayer);

	  const double HA=Control.EvalVar<double>(keyName+NStr+"AHeight");
	  const double HB=Control.EvalDefVar<double>(keyName+NStr+"BHeight",HA);
	  const double WA=Control.EvalVar<double>(keyName+NStr+"AWidth");
	  const double WB=Control.EvalDefVar<double>(keyName+NStr+"BWidth",WA);
	  // angular rotation of bend direciton from +Z
	  const double bendAngDir=
	    Control.EvalVar<double>(keyName+NStr+"AngDir");
	  const double radius=
	    Control.EvalVar<double>(keyName+NStr+"Radius");

	  BU->setValues(HA,HB,WA,WB,L,radius,bendAngDir);
	  BU->setOriginAxis(Origin,X,Y,Z);
	  //	  BU->setEndPts(Origin,Origin+Y*L);      	  
	  shapeUnits.push_back(BU);
	}

      else
	{
	  throw ColErr::InContainerError<std::string>
	    (typeID,"TypeID no known");
	}
    }

  return;
}


void
GuideLine::createUnitVector(const attachSystem::FixedComp& mainFC,
			    const long int mainLP,
			    const attachSystem::FixedComp& beamFC,
			    const long int beamLP)
  /*!
    Create the unit vectors
    \param mainFC :: Outer unit vector 
    \param mainLP :: Outer link unit designator
    \param beamFC :: Beamline unit vector 
    \param beamLP :: beamline link unit designator
  */
{
  ELog::RegMethod RegA("GuideLine","createUnitVector");
  
  attachSystem::FixedComp& shieldFC=FixedGroup::getKey("Shield");
  shieldFC.createUnitVector(mainFC,mainLP);
  shieldFC.applyShift(xStep,yStep,zStep);
  shieldFC.applyAngleRotate(xyAngle,zAngle);

  attachSystem::FixedComp& guideFC=FixedGroup::getKey("GuideOrigin");
  guideFC.createUnitVector(beamFC,beamLP);
  guideFC.applyShift(beamXStep,beamYStep,beamZStep);
  guideFC.applyAngleRotate(beamXYAngle,beamZAngle);


  setDefault("GuideOrigin");
  return;
}

void
GuideLine::createSurfaces()
  /*!
    Create All the surfaces
    \param mainLP :: Front object construction surface
  */
{
  ELog::RegMethod RegA("GuideLine","createSurface");

  FixedGroup::setDefault("Shield");
  const attachSystem::FixedComp& beamFC=
    FixedGroup::getKey("GuideOrigin");

  // Only need to build if not provided
  if (frontCut)
    ModelSupport::buildPlane(SMap,guideIndex+1,Origin,Y);
  if (beamFrontCut)
    ModelSupport::buildPlane(SMap,guideIndex+1001,
			       beamFC.getCentre(),beamFC.getY());

  if (!activeEnd)
    ModelSupport::buildPlane(SMap,guideIndex+2,Origin+Y*length,Y);

  if (activeShield)
    {
      ModelSupport::buildPlane(SMap,guideIndex+3,Origin-X*leftWidth,X);
      ModelSupport::buildPlane(SMap,guideIndex+4,Origin+X*rightWidth,X);
      ModelSupport::buildPlane(SMap,guideIndex+5,Origin-Z*depth,Z);
      ModelSupport::buildPlane(SMap,guideIndex+6,Origin+Z*height,Z);
    }
  // Note we ignore the length component of the last item 
  // and use the guide closer
  int GI(guideIndex+1000);
  for(size_t i=0;i<nShapes;i++)
    {
      if (i)
	ModelSupport::buildPlane(SMap,GI+static_cast<int>(i),
				   shapeUnits[i]->getBegin(),Y);
      shapeUnits[i]->createSurfaces(SMap,layerThick);
      GI+=100;
    }
    
  return;
}

std::string
GuideLine::shapeBackSurf(const size_t index) const
  /*!
    Determine the backcutting surface
    \param Index :: index of shape number
    \return cutting surface string
   */
{
  ELog::RegMethod RegA("GuideLine","shapeBackSurf");

  const int frontNum(guideIndex+9+static_cast<int>(index));
  if (index!=nShapes-1)
    return ModelSupport::getComposite(SMap,frontNum," -2 ");

  return (activeEnd) ?
    endCut.display() :
    ModelSupport::getComposite(SMap,guideIndex," -2 ");
  
}
  
void
GuideLine::createObjects(Simulation& System,
			 const attachSystem::FixedComp& mainFC,
			 const long int mainLP)
  /*!
    Adds the  guide components
    \param System :: Simulation to create objects in
    \param mainFC :: link object 
    \param mainLP :: link point [first surface]
   */
{
  ELog::RegMethod RegA("GuideLine","createObjects");

  std::string Out;
  std::string startSurf;

  if (beamFrontCut)
    startSurf=ModelSupport::getComposite(SMap,guideIndex," 1001 ");
  else
    {
      startSurf=(frontCut) ?
	ModelSupport::getComposite(SMap,guideIndex," 1 ")  :
	mainFC.getSignedLinkString(mainLP);
    }
      
  HeadRule excludeCell;
  int frontNum(guideIndex+9);
  std::string shapeLayer;
  std::string back;
  for(size_t i=0;i<nShapes;i++)
    {
      // front
      const std::string front= (!i) ? startSurf : 
	ModelSupport::getComposite(SMap,frontNum," 1 ");
      
      back=shapeBackSurf(i);
      for(size_t j=0;j<nShapeLayers;j++)
	{
	  // Note that shapeUnits has own offset but
	  shapeLayer=shapeUnits[i]->getString(SMap,j);

	  Out=shapeLayer;
	  Out+=front+back;
	  if (j)
	    Out+=shapeUnits[i]->getExclude(SMap,j-1);

	  System.addCell(MonteCarlo::Qhull(cellIndex++,layerMat[j],0.0,Out));
	}
      
      // Last one add exclude:
      std::string ExOut(shapeLayer);
      if (i) ExOut+=front;
      if (i!=nShapes-1) ExOut+=back;
      excludeCell.addUnion(ExOut);
      frontNum++;
    }

  if (activeShield)
    {	    
      // Outer steel
      if (!activeEnd)
	Out=ModelSupport::getComposite(SMap,guideIndex," -2 3 -4 5 -6 ");
      else
	Out=ModelSupport::getComposite(SMap,guideIndex," 3 -4 5 -6 ")+
	  endCut.display();  

      Out+=startSurf;
      addOuterSurf(Out);      
      
      excludeCell.makeComplement();
      Out+=excludeCell.display();
      System.addCell(MonteCarlo::Qhull(cellIndex++,feMat,0.0,Out));
    }
  else
    {
	
      if (activeEnd)
	addOuterSurf(startSurf+excludeCell.display());
      else
	addOuterSurf(startSurf+excludeCell.display()+back);
    }

  return;
}

void 
GuideLine::createMainLinks(const attachSystem::FixedComp& mainFC,
			   const long int mainLP)
  /*!
    Create the linked units
    \param mainFC :: link object 
    \param mainLP :: link point
   */
{
  ELog::RegMethod RegA("GuideLine","createMainLinks");

  attachSystem::FixedComp& shieldFC=FixedGroup::getKey("Shield");
  const size_t sLP(static_cast<size_t>
		   ((mainLP>0) ? mainLP-1 : -1-mainLP));
  if (!mainLP)
    {
      shieldFC.setConnect(0,Origin,-Y);       
      shieldFC.setLinkSurf(0,SMap.realSurf(guideIndex+1));
    }
  else if (mainLP>0)
    shieldFC.setLinkComponent(0,mainFC,sLP);       
  else
    shieldFC.setLinkCopy(0,mainFC,sLP);       

  shieldFC.setConnect(1,Origin+Y*length,Y);
  if (!activeEnd)
    shieldFC.setLinkSurf(1,SMap.realSurf(guideIndex+2));
  else
    shieldFC.setLinkSurf(1,endCut.display());
  
  if (activeShield)
    {
      shieldFC.setConnect(2,Origin-X*leftWidth/2.0,-X);     
      shieldFC.setConnect(3,Origin+X*rightWidth/2.0,X);     
      shieldFC.setConnect(4,Origin-Z*depth,-Z);     
      shieldFC.setConnect(5,Origin+Z*height,Z);
      int sign(-1);
      for(int i=2;i<6;i++)
	{
	  shieldFC.setLinkSurf(static_cast<size_t>(i),
			       sign*SMap.realSurf(guideIndex+i+1));
	  sign*=-1;
	}
    }

  return;
}

Geometry::Vec3D
GuideLine::calcActiveEndIntercept() 
  /*!
    Determine the active end point intercept
    with the list link point.
    Note that it is not constant because it needs to
    populate the surfaces of the endCut HeadRule.
  */
{
  ELog::RegMethod RegA("GuideLine","calcActiveEndIntercept");

  // Start of track
  const Geometry::Vec3D APt =
    shapeUnits.back()->getEnd();
  const Geometry::Vec3D AAxis =
    shapeUnits.back()->getEndAxis();
  std::vector<Geometry::Vec3D> Pts;
  std::vector<int> SNum;
  
  // This should not need to be called:
  if (!activeEnd)
    endCut.procString(ModelSupport::getComposite(SMap,guideIndex," -2 "));
  endCut.populateSurf();

  endCut.calcSurfIntersection(APt,AAxis,Pts,SNum);
  const size_t indexA=SurInter::closestPt(Pts,APt);
  return Pts[indexA];
}

  
void
GuideLine::createUnitLinks()
  /*!
    Creates the guide links
  */
{
  ELog::RegMethod RegA("GuideLin","createUnitLinks");

  attachSystem::FixedComp& shieldFC=FixedGroup::getKey("Shield");
  // Create GuideLinks
  int SN(guideIndex+10);
  for(size_t i=0;i<nShapes;i++)
    {
      const std::string GKey="Guide"+StrFunc::makeString(i);
      attachSystem::FixedComp& guideFC=
	FixedGroup::getKey(GKey);
      
      guideFC.setConnect(0,shapeUnits[i]->getBegin(),
			 shapeUnits[i]->getBegAxis());

      if (i!=nShapes-1)
	guideFC.setConnect(1,shapeUnits[i]->getEnd(),
			   shapeUnits[i]->getEndAxis());     
      else
	guideFC.setConnect(1,calcActiveEndIntercept(),
			   shapeUnits[i]->getEndAxis());     

      // decide which point to used 
      if (!i && !beamFrontCut && !frontCut)
	guideFC.setLinkCopy(0,shieldFC,0);       
      else if (!i && beamFrontCut)
	guideFC.setLinkSurf(0,-SMap.realSurf(guideIndex+1001));
      else if (!i && frontCut)
	guideFC.setLinkSurf(0,-SMap.realSurf(guideIndex+1));
      else 
	guideFC.setLinkSurf(0,-SMap.realSurf(SN));       

      // END Link Pts
      if (i==nShapes-1)
	guideFC.setLinkSurf(1,SMap.realSurf(guideIndex+2));
      else
	guideFC.setLinkSurf(1,SMap.realSurf(SN+1));       

      SN++;
    }
  return;
}


void
GuideLine::addEndCut(const std::string& EC)
  /*!
    Add an end cut to the string
    \param EC :: End cut
  */
{
  ELog::RegMethod RegA("GuideLine","addEndCut");

  if (EC.empty())
    activeEnd=0;
  else
    {
      activeEnd=1;
      endCut.procString(EC);
    }
  return;
}

void
GuideLine::createAll(Simulation& System,
		     const attachSystem::FixedComp& mainFC,
		     const long int mainLP,
		     const attachSystem::FixedComp& beamFC,
		     const long int beamLP)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param mainFC :: Outer unit vector 
    \param mainLP :: Outer link unit designator
    \param beamFC :: Beamline unit vector 
    \param beamLP :: beamline link unit designator
  */
{
  ELog::RegMethod RegA("GuideLine","createAll");

  populate(System.getDataBase());
  createUnitVector(mainFC,mainLP,beamFC,beamLP);
  processShape(System.getDataBase());
  createSurfaces();
  createObjects(System,mainFC,mainLP);
  createMainLinks(mainFC,mainLP);
  createUnitLinks();
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE beamlineSystem


