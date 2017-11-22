/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Upgrade/TriangleMod.cxx 
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
#include "RuleSupport.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "Vert2D.h"
#include "Convex2D.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "LayerComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "AttachSupport.h"
#include "ModBase.h"
#include "TriUnit.h"
#include "TriangleMod.h"

namespace moderatorSystem
{

TriangleMod::TriangleMod(const std::string& Key)  :
  constructSystem::ModBase(Key,5),
  Outer(modIndex+100)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{
  nLayers=3;
  nInnerLayers=0;
}

TriangleMod::TriangleMod(const TriangleMod& A) : 
  constructSystem::ModBase(A),
  absXStep(A.absXStep),absYStep(A.absYStep),
  absZStep(A.absZStep),absXYAngle(A.absXYAngle),
  absZAngle(A.absZAngle),Outer(A.Outer),nIUnits(A.nIUnits),
  IUnits(A.IUnits),height(A.height),wallThick(A.wallThick),
  flatClearance(A.flatClearance),topClearance(A.topClearance),
  baseClearance(A.baseClearance),nInnerLayers(A.nInnerLayers),
  innerThick(A.innerThick),innerMat(A.innerMat),
  modTemp(A.modTemp),modMat(A.modMat),wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: TriangleMod to copy
  */
{}

TriangleMod&
TriangleMod::operator=(const TriangleMod& A)
  /*!
    Assignment operator
    \param A :: TriangleMod to copy
    \return *this
  */
{
  if (this!=&A)
    {
      constructSystem::ModBase::operator=(A);
      absXStep=A.absXStep;
      absYStep=A.absYStep;
      absZStep=A.absZStep;
      absXYAngle=A.absXYAngle;
      absZAngle=A.absZAngle;
      Outer=A.Outer;
      nIUnits=A.nIUnits;
      IUnits=A.IUnits;
      height=A.height;
      wallThick=A.wallThick;
      flatClearance=A.flatClearance;
      topClearance=A.topClearance;
      baseClearance=A.baseClearance;
      nInnerLayers=A.nInnerLayers;
      innerThick=A.innerThick;
      innerMat=A.innerMat;
      modTemp=A.modTemp;
      modMat=A.modMat;
      wallMat=A.wallMat;
    }
  return *this;
}

TriangleMod::~TriangleMod() 
  /*!
    Destructor
  */
{}

void
TriangleMod::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DAtaBase for variables
  */
{
  ELog::RegMethod RegA("TriangleMod","populate");
  
  ModBase::populate(Control);

  absXStep=Control.EvalDefVar<double>(keyName+"AbsXStep",0.0);
  absYStep=Control.EvalDefVar<double>(keyName+"AbsYStep",0.0);
  absZStep=Control.EvalDefVar<double>(keyName+"AbsZStep",0.0);
  absXYAngle=Control.EvalDefVar<double>(keyName+"AbsXYangle",0.0);
  absZAngle=Control.EvalDefVar<double>(keyName+"AbsZangle",0.0);


  Outer.clear();
  const size_t NC=Control.EvalDefVar<size_t>(keyName+"NCorner",3);
  if (NC<3 || NC>20)
    throw ColErr::RangeError<size_t>(NC,3,20,"nCorner out of range");
  Geometry::Vec3D Pt;
  int absol;
  for(size_t i=0;i<NC;i++)
    {
      Pt=Control.EvalVar<Geometry::Vec3D>(keyName+"Corner"+
					  StrFunc::makeString(i+1));
      absol=Control.EvalDefVar(keyName+"Absolute"+
			     StrFunc::makeString(i+1),0);
      Outer.addObject(Pt,absol);
    }

  nIUnits=Control.EvalDefVar<size_t>(keyName+"NInnerUnits",0);
  IUnits.clear();
  for(size_t i=0;i<nIUnits;i++)
    {
      TriUnit tU(modIndex+3000+1000*static_cast<int>(i));
      const std::string IUStr=keyName+"Inner"+StrFunc::makeString(i+1);
      const size_t INC=Control.EvalVar<size_t>(IUStr+"NCorner");
      for(size_t j=0;j<INC;j++)
	{
	  Pt=Control.EvalVar<Geometry::Vec3D>
	    (IUStr+"Corner"+StrFunc::makeString(j+1));
	  absol=Control.EvalDefVar
	    (IUStr+"Absolute"+StrFunc::makeString(j+1),0);
	  tU.addObject(Pt,absol);
	}
      IUnits.push_back(tU);
    }

  height=Control.EvalVar<double>(keyName+"Height");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");
  flatClearance=Control.EvalVar<double>(keyName+"FlatClearance");
  topClearance=Control.EvalVar<double>(keyName+"TopClearance");
  baseClearance=Control.EvalVar<double>(keyName+"BaseClearance");

  if (nIUnits)
    {
      nInnerLayers=Control.EvalVar<size_t>(keyName+"NInnerLayers");
      if (nInnerLayers<1)
	{
	  ELog::EM<<"Unable to build inner section with zero layers"
		  <<ELog::endCrit;
	  throw ColErr::RangeError<size_t>(0,1,1000,"nInnerLayers");
	}
      int M;
      double D;
      for(size_t i=0;i<nInnerLayers;i++)
	{
	  const std::string NStr=StrFunc::makeString(i);
	  if ((i+1)!=nInnerLayers)
	    {
	      D=Control.EvalVar<double>(keyName+"InnerThick"+NStr);
	      innerThick.push_back(D);
	    }
	  M=ModelSupport::EvalMat<int>(Control,keyName+"InnerMat"+NStr);
	  innerMat.push_back(M);
	}
      innerThick.push_back(0.0);  // Null terminator
    }

  modTemp=Control.EvalVar<double>(keyName+"ModTemp");
  modMat=ModelSupport::EvalMat<int>(Control,keyName+"ModMat");
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");

  return;
}
  
void
TriangleMod::createConvex()
  /*!
    Create convex hulls and resequence the points
   */
{
  ELog::RegMethod RegA("TriangleMod","createConvex");
  
  Outer.applyAbsShift(absXStep,absYStep,absZStep);
  Outer.renormalize(Origin,X,Y,Z);
  Outer.constructConvex(Z);
  std::vector<TriUnit>::iterator vc;
  for(vc=IUnits.begin();vc!=IUnits.end();vc++)
    {
      if (vc->nCorner)
	{
	  vc->applyAbsShift(absXStep,absYStep,absZStep);
	  vc->renormalize(Origin,X,Y,Z);
	  vc->constructConvex(Z);
	}
    }
  return;
}


void
TriangleMod::createLinks()
  /*!
    Construct links for the triangel moderator
  */
{
  ELog::RegMethod RegA("TriangleMod","createLinks");

  // Loop over corners that are bound by convex

  FixedComp::setNConnect(Outer.nCorner+2);

  const double OSize=wallThick+flatClearance;

  // Top / Base 
  FixedComp::setConnect(0,Origin-Z*(height/2.0+OSize),-Z);
  FixedComp::setConnect(1,Origin+Z*(height/2.0+OSize),Z);

  
  FixedComp::setLinkSurf(0,-SMap.realSurf(modIndex+25));
  FixedComp::setLinkSurf(1,SMap.realSurf(modIndex+26));


  for(size_t i=0;i<Outer.nCorner;i++)
    {
      const int ii(static_cast<int>(i));
      if (!Outer.nonConvex[i])
	{
	  std::pair<Geometry::Vec3D,Geometry::Vec3D> CP=
	    cornerPair(Outer.Pts,i,i+1,OSize);
	  FixedComp::setConnect(i+2,(CP.first+CP.second)/2.0,
				sideNorm(CP));
	  FixedComp::setLinkSurf(i+2,-SMap.realSurf(modIndex+101+ii));
	}
      else 
	{
	  // Non-Convex point : 
	  // -- Set link point as point
          // --> set link surfaces as union
	  Geometry::Vec3D AvePt;
	  std::ostringstream cx;
	  cx<<"(";
	  size_t jA(i),jB((i+1) % Outer.nCorner);
	  do
	    {
	      jA= (jA) ? jA-1 : Outer.nCorner-1;
	      cx<<jA+1<<" : ";
	    }
	  while(Outer.nonConvex[jA]);
	  while(Outer.nonConvex[jB])
	    {
	      jB=(jB+1) % Outer.nCorner;
	      cx<<jB+1<<" : ";
	    } 
	  cx<<ii+1<<" )";

	  const std::string Out=ModelSupport::getComposite
	    (SMap,modIndex+300,cx.str());
	  HeadRule AX;
	  AX.procString(Out);
	  AX.makeComplement();
	  FixedComp::setLinkSurf(i+2,AX.display());
	  // Get Normal
	  jB= (jB) ? jB-1 : Outer.nCorner-1;
	  int nAve(0);
	  Geometry::Vec3D AveDir;
	  while(jA!=jB)
	    {
	      std::pair<Geometry::Vec3D,Geometry::Vec3D> CP=
		cornerPair(Outer.Pts,jA,jA+1,0.0);
	      AveDir+=sideNorm(CP);
	      nAve++;
	      jA = (jA+1) % Outer.nCorner;
	    }
	  if (!nAve)
	    throw ColErr::NumericalAbort
	      ("nAve==0 :: Points colinear etc, as not triangle found");
	  AveDir/=nAve;
	  Geometry::Vec3D CP(realPt(Outer.Pts[i]));
	  FixedComp::setConnect(i+2,CP,AveDir);
	}
    }
  

  return;
}
  

Geometry::Vec3D
TriangleMod::realPt(const Geometry::Vec3D& CPt) const
  /*!
    Given a point put it into the origin frame
    \param CPt :: Point to convert
    \return Converted point
  */
{
  ELog::RegMethod RegA("TriangleMod","realPt");

  Geometry::Vec3D APoint=
    Origin + X * CPt.X() + Y*CPt.Y();
  return APoint;
}

Geometry::Vec3D
TriangleMod::realAxis(const Geometry::Vec3D& CPt) const
  /*!
    Given a point put it into the origin framce
    \param CPt :: Axis to convert
    \return Cnverted point
  */
{
  ELog::RegMethod RegA("TriangleMod","realAxis");

  Geometry::Vec3D APoint=
    X * CPt.X() + Y*CPt.Y();
  return APoint;
}

std::pair<Geometry::Vec3D,Geometry::Vec3D>
TriangleMod::cornerPair(const std::vector<Geometry::Vec3D>& CPts,
			const size_t IndexA,const size_t IndexB,
			const double scale) const
  /*!
    Given two corners which define a normal to line.
    Step out by the distance scale.
    -- Converts both points into normal origin/X/Y
    \param CPts :: Corner/Inner points 
    \param IndexA :: First corner
    \param IndexB :: Second corner
    \param scale :: Distance out from the surface
    \return shifted points
  */
{
  ELog::RegMethod RegA("TriangleMod","cornerPair");
  const size_t nC(CPts.size());

  Geometry::Vec3D APoint=realPt(CPts[IndexA % nC]);
  Geometry::Vec3D BPoint=realPt(CPts[IndexB % nC]);

  if (fabs(scale)>Geometry::zeroTol)
    {
      Geometry::Vec3D ASide=(BPoint-APoint).unit();
      ASide=Z*ASide;
      APoint+=ASide*scale;
      BPoint+=ASide*scale;
    }
  return std::pair<Geometry::Vec3D,Geometry::Vec3D>
    (APoint,BPoint);
}


Geometry::Vec3D 
TriangleMod::sideNorm(const std::pair<Geometry::Vec3D,
		      Geometry::Vec3D>& CPts) const
  /*!
    Calculate the normal leaving the side of the triangle
    \param CPts :: Pair of points making up surface (with +Z)
    \return Normal
  */
{
  ELog::RegMethod RegA("TriangleMod","sideNorm");

  // Now get cross vector
  const Geometry::Vec3D ASide=(CPts.second-CPts.first).unit();
  return Z*ASide;
}

void
TriangleMod::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("TriangleMod","createSurface");

  // Surfaces 1-6 are the FC container
  // Surfaces 11-16 are the outer blades etc

  const double PSteps[]={wallThick,flatClearance,0.0};  

  for(size_t i=0;i<Outer.nCorner;i++)
    {
      const int ii(static_cast<int>(i));
      double PDepth(0.0);  // + from origin
      int triOffset(modIndex+101);
      for(size_t j=0;j<nLayers;j++)
	{
	  const std::pair<Geometry::Vec3D,Geometry::Vec3D>
	    CPts(cornerPair(Outer.Pts,i,i+1,PDepth));
	  // Note :: Side norm faces out
	  ModelSupport::buildPlane(SMap,triOffset+ii,
				   CPts.first,CPts.second,
				   CPts.second+Z,
				   -sideNorm(CPts));
	  triOffset+=100;
	  PDepth+=PSteps[j];
	}
    }

  ModelSupport::buildPlane(SMap,modIndex+5,Origin-Z*height/2.0,Z);
  ModelSupport::buildPlane(SMap,modIndex+6,Origin+Z*height/2.0,Z);

  ModelSupport::buildPlane(SMap,modIndex+15,
			   Origin-Z*(height/2.0+wallThick),Z);
  ModelSupport::buildPlane(SMap,modIndex+16,
			   Origin+Z*(height/2.0+wallThick),Z);

  ModelSupport::buildPlane(SMap,modIndex+25,
			   Origin-Z*(height/2.0+wallThick+baseClearance),Z);
  ModelSupport::buildPlane(SMap,modIndex+26,
			   Origin+Z*(height/2.0+wallThick+topClearance),Z);


  // NOW CREATE INTERNAL ITEM:
  // layer go by 100  : objects go by 1000
  for(size_t i=0;i<nIUnits;i++)
    {
      for(size_t j=0;j<IUnits[i].nCorner;j++)
	{
	  int triOffset(IUnits[i].getOffset()+1);
	  const int ii(static_cast<int>(j));
	  double PDepth(0.0);  // + from origin
	  for(size_t k=0;k<nInnerLayers;k++)
	    {
	      const std::pair<Geometry::Vec3D,Geometry::Vec3D>
		CPts(cornerPair(IUnits[i].Pts,j,j+1,PDepth));
	      // Note :: Side norm faces out
	      ModelSupport::buildPlane(SMap,triOffset+ii,
				       CPts.first,CPts.second,
				       CPts.second+Z,
				       -sideNorm(CPts));
	      PDepth-=innerThick[k];
	      triOffset+=100;
	    }

	}
    }
  return;
}


std::string
TriangleMod::createInnerObject(Simulation& System)
  /*!
    Create the inner void space if required
    \param System :: Simulation 
    \return Full Exluding string
  */
{
  ELog::RegMethod RegA("TriangleMod","createInnerObject");

  if (!nIUnits)  return "";

  HeadRule InnerExclude;
  const std::string vertUnit=
    ModelSupport::getComposite(SMap,modIndex," 5 -6");


  for(size_t i=0;i<nIUnits;i++)
    {
      std::string outUnit,inUnit;
      for(size_t k=0;k<nInnerLayers;k++)
	{
	  const size_t iK(nInnerLayers-(k+1));
	  // Full inside unit:
	  outUnit=IUnits[i].getString(iK);
	  IUnits[i].addCell(cellIndex);
	  System.addCell(MonteCarlo::Qhull(cellIndex++,innerMat[iK],modTemp,
					   inUnit+outUnit+vertUnit));
	  inUnit=IUnits[i].getExclude(iK);
	}
      InnerExclude.addUnion(outUnit);
    }

  InnerExclude.makeComplement();

  return InnerExclude.display();
}

void
TriangleMod::cutInnerObj(Simulation& System,const TriUnit& InnerUnit)
  /*!
    Cut an inner object
    \param System :: Simulation to use
    \param InnerUnit :: Inner unit for cutting
  */
{
  ELog::RegMethod RegA("TriangleMod","cutInnerObj");

  const std::vector<int>& Cells=InnerUnit.getCells();
  std::vector<int>::const_iterator vc;
  for(vc=Cells.begin();vc!=Cells.end();vc++)
    {
      MonteCarlo::Qhull* innerObj=System.findQhull(*vc);
      if (innerObj)
	innerObj->addSurfString(Outer.getString(0));
      else
	throw ColErr::InContainerError<int>(*vc,"Cell not found");      
    }
  return;
}

void
TriangleMod::cutOuterObj(Simulation& System,
			 const TriUnit& masterUnit,
			 const TriUnit& cutUnit)
  /*!
    Cut an inner object
    \param System :: Simulation to use
    \param masterUnit :: Object provide outer perimeter
    \param cutUnit :: Unit for cutting
  */
{
  ELog::RegMethod RegA("TriangleMod","cutOuterObj");

  const size_t nInnerLayer(2);

  const std::vector<int>& Cells=cutUnit.getCells();
  std::vector<int>::const_iterator vc;
  for(vc=Cells.begin();vc!=Cells.end();vc++)
    {
      MonteCarlo::Qhull* outerObj=System.findQhull(*vc);
      if (outerObj)
	{
	  const std::string cutStr=
	    masterUnit.getString(nInnerLayer-1);
	  outerObj->addSurfString(cutStr);
	}
      else
	throw ColErr::InContainerError<int>(*vc,"Cell not found");      
    }
  return;
}

void
TriangleMod::calcOverlaps(Simulation& System)
  /*!
    Calculate the objects that overlap and fix
    - bit crude as it fully inserts every object
    \param System :: Simulation
   */
{
  ELog::RegMethod RegA("TriangleMod","calcOverlaps");
  // Tmp Object for Outer:

  MonteCarlo::Qhull OHull(cellIndex++,0,0.0,Outer.getString(0));
  OHull.createSurfaceList();

  
  Geometry::Plane PZ(modIndex+5000,0);
  PZ.setPlane(Geometry::Vec3D(0,0,0),Z);

  std::vector<MonteCarlo::Qhull> IQH;
  for(size_t i=0;i<nIUnits;i++)
    {
      const std::string OLayer=IUnits[i].getString(0);
      IQH.push_back(MonteCarlo::Qhull(cellIndex++,0,0.0,OLayer));
      IQH.back().createSurfaceList();
    }
  for(size_t i=0;i<nIUnits;i++)
    {      
      if (attachSystem::checkPlaneIntersect(PZ,IQH[i],OHull))
	cutInnerObj(System,IUnits[i]);

      for(size_t k=i+1;k<nIUnits;k++)
	{
	  if (attachSystem::checkPlaneIntersect(PZ,IQH[i],IQH[k]))
	    {
	      ELog::EM<<"Cut inner "<<i<<" "<<k<<ELog::endDiag;
	      cutOuterObj(System,IUnits[i],IUnits[k]);
	    }
	}      
    }
  return;
}

void
TriangleMod::createObjects(Simulation& System)
  /*!
    Adds the main components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("TriangleMod","createObjects");

  std::string outUnit;
  std::string inUnit;
  std::string Out;

  // Full moderator
  outUnit=ModelSupport::getComposite(SMap,modIndex," 5 -6 ")+
    Outer.getString(0);
  
  Out=createInnerObject(System);
  System.addCell(MonteCarlo::Qhull(cellIndex++,modMat,modTemp,outUnit+Out));

  // Wall       
  inUnit=MonteCarlo::getComplementShape(outUnit);
  outUnit=ModelSupport::getComposite(SMap,modIndex," 15 -16 ")+
    Outer.getString(1);
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,modTemp,outUnit+inUnit));

  // Clearance
  inUnit=MonteCarlo::getComplementShape(outUnit);
  outUnit=ModelSupport::getComposite(SMap,modIndex," 25 -26 ")+
    Outer.getString(2);
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,outUnit+inUnit));
  
  // Outer
  addOuterSurf(outUnit);

  return;
}

int
TriangleMod::getExitWindow(const long int sideIndex,
			   std::vector<int>& window) const
  /*!
    Generic exit window system : 
    -- Requires at least 5 surfaces
    -- Requires 3-5 to be sign surf
    \param sideIndex :: link point 
    \param window :: window vector of paired planes
    \return Viewed surface
  */
{
  ELog::RegMethod RegA("TriangleMod","getExitWindow");
  if (LU.size()<5)
    throw ColErr::IndexError<size_t>(LU.size(),5,"Link size too small");


  const long int outIndex(std::abs(sideIndex));
  if (outIndex>6)
    throw ColErr::IndexError<long int>(outIndex,6,"outIndex too big");

  // 5: to get front + 4 on side
  window.clear();
  for(long int i=1;i<6;i++)
    if (i!=outIndex)
      window.push_back(std::abs(getLinkSurf(i)));

  window.push_back(0);
  return std::abs(SMap.realSurf(getLinkSurf(sideIndex)));
}

Geometry::Vec3D
TriangleMod::getSurfacePoint(const size_t layerIndex,
			     const long int sideIndex) const
  /*!
    Simplified layer system only for top/bottom
    \param layerIndex :: layer
    \param sideIndex :: 0 base / 1 top : index by wall nubmer
    \return Point in middle of surface
  */
{
  ELog::RegMethod RegA("TriangleMod","getSurfacePoint");

  const size_t SI((sideIndex>0) ?
                  static_cast<size_t>(sideIndex-1) :
                  static_cast<size_t>(-1-sideIndex));
  
  double LT[3];  
  if (SI>=Outer.Pts.size()+2) 
    throw ColErr::IndexError<size_t>(SI,Outer.Pts.size()+2,
                                       "SI");
  if (layerIndex>=nLayers) 
    throw ColErr::IndexError<size_t>(layerIndex,nLayers,"layer");
  

  if (SI>1)   // Main sized
    {
      LT[0]=0.0; LT[1]=LT[0]+wallThick; LT[2]=LT[1]+flatClearance;
      std::pair<Geometry::Vec3D,Geometry::Vec3D> CP=
	cornerPair(Outer.Pts,SI-2,SI-1,LT[layerIndex]);
      return (CP.first+CP.second)/2.0;
    }
  else if (SI)  // TOP
    {
      LT[0]=height/2.0; LT[1]=LT[0]+wallThick; LT[2]=LT[1]+topClearance;
      return Origin+Z*LT[layerIndex];
    }
  // BASE
  LT[0]=height/2.0; LT[1]=LT[0]+wallThick; LT[2]=LT[1]+baseClearance;
  return Origin-Z*LT[layerIndex];

} 

int
TriangleMod::getLayerSurf(const size_t layerIndex,
			  const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link surf
    \param sideIndex :: Side [1:Base/2:Top : side+3] 
    \param layerIndex :: layer, 0 is inner moderator [0-4]
    \return Surface number 
  */
{
  ELog::RegMethod RegA("TriangleMod","getLayerSurf");

  if (layerIndex>=nLayers) 
    throw ColErr::IndexError<size_t>(layerIndex,nLayers,"layerIndex");

  const size_t uSIndex(static_cast<size_t>(std::abs(sideIndex)));
  
  if (!sideIndex || uSIndex>Outer.Pts.size()+2) 
    throw ColErr::IndexError<size_t>
      (uSIndex,Outer.Pts.size()+2,"sideIndex");
  
  int signValue((sideIndex<0) ? -1 : 1);
 
  const int addNumber=modIndex+static_cast<int>(layerIndex)*10;

  if (uSIndex==2)  // TOP
    return signValue*SMap.realSurf(addNumber+6);
  if (uSIndex==1)      // BASE
    return signValue*SMap.realSurf(addNumber+5);

  const int lIndex=static_cast<int>(layerIndex);
  const int SN=modIndex+100*lIndex+101+static_cast<int>(uSIndex-3);
  return signValue*SMap.realSurf(SN);  
}

std::string
TriangleMod::getLayerString(const size_t layerIndex,
			    const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link surf
    \param sideIndex :: Side [1:Base/2:Top : side+3] 
    \param layerIndex :: layer, 0 is inner moderator [0-4]
    \return Surface number 
  */
{
  ELog::RegMethod RegA("TriangleMod","getLayerString");
  return StrFunc::makeString(getLayerSurf(layerIndex,sideIndex));
}

  
void
TriangleMod::createAll(Simulation& System,
		       const attachSystem::FixedComp& axisFC,
		       const attachSystem::FixedComp* orgFC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param axisFC :: FixedComp to get axis [origin if orgFC == 0]
    \param orgFC :: Extra origin point if required
    \param sideIndex :: link point for origin if given
  */
{
  ELog::RegMethod RegA("TriangleMod","createAll");
  
  populate(System.getDataBase());
  ModBase::createUnitVector(axisFC,orgFC,sideIndex);
  
  createConvex();
  createSurfaces();
  createObjects(System);
  calcOverlaps(System);

  createLinks();
  insertObjects(System);       
  return;
}
  
}  // NAMESPACE moderatorSystem
