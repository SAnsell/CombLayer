/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Upgrade/CH4Layer.cxx
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
#include <numeric>
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
#include "Quadratic.h"
#include "Plane.h"
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
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "LayerComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "t1Reflector.h"
#include "ModBase.h"
#include "LayerInfo.h"
#include "CH4Layer.h"

namespace ts1System
{

CH4Layer::CH4Layer(const std::string& Key)  :
  constructSystem::ModBase(Key,6)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

CH4Layer::CH4Layer(const CH4Layer& A) : 
  constructSystem::ModBase(A),
  LVec(A.LVec)
  /*!
    Copy constructor
    \param A :: CH4Layer to copy
  */
{}

CH4Layer&
CH4Layer::operator=(const CH4Layer& A)
  /*!
    Assignment operator
    \param A :: CH4Layer to copy
    \return *this
  */
{
  if (this!=&A)
    {
      constructSystem::ModBase::operator=(A);
      LVec=A.LVec;
    }
  return *this;
}


CH4Layer*
CH4Layer::clone() const
 /*!
   Clone constructor
   \return new(this)
 */
{
  return new CH4Layer(*this);
}

CH4Layer::~CH4Layer() 
 /*!
   Destructor
 */
{}

double
CH4Layer::checkUnit(const FuncDataBase& Control,const std::string& KN,
		    const size_t Index,const std::string& A,
		    const double AScale,const std::string& B,
		    const double BScale,const std::string& C,
		    const double CScale,const bool defFlag,
		    const double defVal) const
  /*!
    Checks a given value exists in the data base
    and returns the scaled component
    \param Control :: DataBase
    \param KN :: Key name 
    \param Index :: Index value
    \param A :: Primary string
    \param AScale :: Scale factor for [if found]
    \param B :: Primary string
    \param BScale :: Scale factor for [if found]
    \param C :: Primary string
    \param CScale :: Scale factor for [if found]
    \param defFlag :: trap later 
    \param defVal :: Default value [if defFlag]
    \return Var(A/B/C)*Scale
   */
{
  ELog::RegMethod RegA("CH4Layer","checkUnit");
  const std::string keyA=KN+StrFunc::makeString(A,Index);

  if (Control.hasVariable(keyA))
    return Control.EvalVar<double>(keyA)*AScale;
  const std::string keyB=KN+StrFunc::makeString(B,Index);
  if (Control.hasVariable(keyB))
    return Control.EvalVar<double>(keyB)*BScale;
  const std::string keyC=KN+StrFunc::makeString(C,Index);
  // Allow this to throw
  if (!defFlag)
    return Control.EvalVar<double>(keyC)*CScale;

  return Control.EvalDefVar<double>(keyC,defVal);
}

void
CH4Layer::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: DataBase 
 */
{
  ELog::RegMethod RegA("CH4Layer","populate");
  
  ModBase::populate(Control);

  nLayers=Control.EvalVar<size_t>(keyName+"NLayer");
  if (!nLayers)
    throw ColErr::IndexError<size_t>(0,1,"nLayers");
  double Front,Back,Left,Right,Top,
    Base,frontRad,backRad,round,temp;
  int mat;

  for(size_t i=0;i<nLayers;i++)
    {
      Top=checkUnit(Control,keyName,i+1,"Top",1.0,"Height",0.5,"Layer",1.0);
      Base=checkUnit(Control,keyName,i+1,"Base",1.0,"Height",0.5,"Layer",1.0);
      Left=checkUnit(Control,keyName,i+1,"Left",1.0,"Width",0.5,"Layer",1.0);
      Right=checkUnit(Control,keyName,i+1,"Right",1.0,"Width",0.5,"Layer",1.0);
      Front=checkUnit(Control,keyName,i+1,"Front",1.0,"Depth",0.5,"Layer",1.0);
      Back=checkUnit(Control,keyName,i+1,"Back",1.0,"Depth",0.5,"Layer",1.0);
      frontRad=checkUnit(Control,keyName,i+1,"FrontRad",1.0,
			"FrontRadius",1.0,"Radius",1.0,1,0.0);
      backRad=checkUnit(Control,keyName,i+1,"BackRad",1.0,
			"BackRadius",1.0,"Radius",1.0,1,0.0);
      round=checkUnit(Control,keyName,i+1,"Round",1.0,
		      "Round",0.5,"Round",1.0,1,0.0);

      mat=ModelSupport::EvalMat<int>
	(Control,keyName+StrFunc::makeString("Mat",i+1));
      temp=Control.EvalDefVar<double>
	(keyName+StrFunc::makeString("Temp",i+1),0.0);
      
      LayerInfo IDX(Front,Back,Left,Right,Base,Top);
      IDX.setRounds(frontRad,backRad,round);
      IDX.setMat(mat,temp);
      if (i)
	IDX+=LVec.back();
      LVec.push_back(IDX);
    }

  return;
}
  
void
CH4Layer::createLayerSurf(const std::vector<LayerInfo>& LayVec,
			  const int index) 
  /*!
    Create Layer surfaces based on the primary field and 
    the LayerInfo values.
    \param LayVec :: Info on each layer 
    \param index  :: Index to layer nubmer
  */
{
  ELog::RegMethod RegA("CH4Layer","createLayerSurf");

  const Geometry::Vec3D XYZ[6]={-Y,Y,-X,X,-Z,Z};
  int ch4Layer(index);

  for(size_t i=0;i<LayVec.size();i++)
    {
      const double LWidth=LayVec[i].Item(2);
      const double RWidth=LayVec[i].Item(3);
      const double MWidth=(LWidth+RWidth)/2.0;
      const double radF=LayVec[i].frontRadius();
      const double radB=LayVec[i].backRadius();
      const double FD=LayVec[i].Item(0);	  
      const double BD=LayVec[i].Item(1);	  
      // Front surface 
      if (fabs(radF)>Geometry::zeroTol)
	{
	  double Yshift=radF*radF-MWidth*MWidth;
	  if (Yshift<Geometry::zeroTol)
	    throw ColErr::RangeError<double>(radF,MWidth,1e10,
					     "RadF too low of surface:"+
					     StrFunc::makeString(i+1));
	  
	  Yshift=sqrt(Yshift);
	  const Geometry::Vec3D RCent=Origin-Y*(FD-Yshift)+
	    X*(LWidth-RWidth);
	  ModelSupport::buildCylinder(SMap,ch4Layer+1,RCent,Z,radF);

	  // Now build corner rad Calc centre
	  // Point connects RCentre and side surface at normals
	  const double cRad=LayVec[i].cornerRad();
	  if (fabs(cRad)>Geometry::zeroTol)
	    {
	      const double lX=(2.0*LWidth-RWidth)-cRad;
	      const double lY=sqrt((radF-cRad)*(radF-cRad)-lX*lX);
	      const Geometry::Vec3D LC=RCent-Y*lY-X*lX;
	      Geometry::Vec3D DUnit=(LC-RCent)*Z;
	      // front curve / cylinder impact point
	      ModelSupport::buildCylinder(SMap,ch4Layer+13,LC,Z,cRad);
	      ModelSupport::buildPlane(SMap,ch4Layer+1011,LC,Y);
	      ModelSupport::buildPlane(SMap,ch4Layer+1012,LC,DUnit);

	      const double rX=(2.0*RWidth-LWidth)-cRad;
	      const double rY=sqrt((radF-cRad)*(radF-cRad)-rX*rX);
	      const Geometry::Vec3D RC=RCent-Y*rY+X*rX;
	      DUnit=(RC-RCent)*Z;
	      ModelSupport::buildCylinder(SMap,ch4Layer+14,RC,Z,cRad);
	      ModelSupport::buildPlane(SMap,ch4Layer+1013,RC,Y);
	      ModelSupport::buildPlane(SMap,ch4Layer+1014,RC,DUnit);
	      
	    }
	}
      else
	ModelSupport::buildPlane(SMap,ch4Layer+1,Origin-Y*FD,Y);

      // Back surface 
      if (fabs(radB)>Geometry::zeroTol)
	{
	  double Yshift=radB*radB-MWidth*MWidth;
	  if (Yshift<Geometry::zeroTol)
	    throw ColErr::RangeError<double>(radB,MWidth,1e10,
					     "RadB too low of surface:"+
					     StrFunc::makeString(i+1));
	  Yshift=sqrt(Yshift);
	  const Geometry::Vec3D RCent=Origin+Y*(BD-Yshift)+
	    X*(LWidth-RWidth);
	  ModelSupport::buildCylinder(SMap,ch4Layer+2,RCent,Z,radB);

	  const double cRad=LayVec[i].cornerRad();
	  if (fabs(cRad)>Geometry::zeroTol)
	    {
	      const double lX=(2.0*LWidth-RWidth)-cRad;
	      const double lY=sqrt((radB-cRad)*(radB-cRad)-lX*lX);
	      const Geometry::Vec3D LC=RCent+Y*lY-X*lX;
	      Geometry::Vec3D DUnit=(LC-RCent)*Z;
	      // front curve / cylinder impact point
	      ModelSupport::buildCylinder(SMap,ch4Layer+23,LC,Z,cRad);
	      ModelSupport::buildPlane(SMap,ch4Layer+1021,LC,Y);
	      ModelSupport::buildPlane(SMap,ch4Layer+1022,LC,DUnit);

	      const double rX=(2.0*RWidth-LWidth)-cRad;
	      const double rY=sqrt((radB-cRad)*(radB-cRad)-rX*rX);
	      const Geometry::Vec3D RC=RCent+Y*rY+X*rX;
	      DUnit=(RC-RCent)*Z;
	      ModelSupport::buildCylinder(SMap,ch4Layer+24,RC,Z,cRad);
	      ModelSupport::buildPlane(SMap,ch4Layer+1023,RC,Y);
	      ModelSupport::buildPlane(SMap,ch4Layer+1024,RC,DUnit);
	    }

	}
      else
	ModelSupport::buildPlane(SMap,ch4Layer+2,Origin+Y*BD,Y);

      // 1+j-j%2 gives 1,1,3,3,5,5 as j iterates

      for(size_t j=2;j<6;j++)
	{
	  ModelSupport::buildPlane(SMap,ch4Layer+static_cast<int>(j+1),
				   Origin+XYZ[j]*LayVec[i].Item(j),
				   XYZ[1+j-j%2]);
	}
      ch4Layer+=30;
    }
  
  return;
}

void
CH4Layer::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("CH4Layer","createSurface");

  createLayerSurf(LVec,modIndex);
  return;
}

void
CH4Layer::createFrontRule(const LayerInfo& LInfo,
			  const int offsetIndex,
			  const size_t lIndex,
			  HeadRule& FX) const
  /*!
    Populates the head rules for front/back depending on 
    radius status
    \param LInfo :: Layer Info for radius etc
    \param offsetIndex :: Object registered number
    \param lIndex :: layerIndex normaly : modIndex+30*lIndex
    \param FX :: Front head rule

  */
{
  ELog::RegMethod RegA("CH4Layer","createFrontRule");

  const double radF=LInfo.frontRadius();
  const double cRad=LInfo.cornerRad();

  std::string Out;
  const int ch4Layer(offsetIndex+30*static_cast<int>(lIndex));
  if (radF<Geometry::zeroTol) 
    Out=ModelSupport::getComposite(SMap,ch4Layer," 1 ");
  else
    {
      if (cRad>Geometry::zeroTol)
	Out=ModelSupport::getComposite
	  (SMap,ch4Layer,"-1 (-13:1011:-1012) (-14:1013:1014)" );
      else
	Out=ModelSupport::getComposite(SMap,ch4Layer," -1 ");
    }
  FX.procString(Out);      
  return;
}

void
CH4Layer::createBackRule(const LayerInfo& LInfo,
			  const int offsetIndex,
			  const size_t lIndex,
			  HeadRule& BX) const
  /*!
    Populates the head rules for front/back depending on 
    radius status
    \param lIndex :: layerIndex
    \param BX :: Back head rule
  */
{
  ELog::RegMethod RegA("CH4Layer","createBackRule");
  
  const double radB=LInfo.backRadius();
  const double cRad=LInfo.cornerRad();

  std::string Out;
  const int ch4Layer(offsetIndex+30*static_cast<int>(lIndex));

  if (radB<Geometry::zeroTol) 
    Out=ModelSupport::getComposite(SMap,ch4Layer," -2 ");
  else
    {
      if (cRad>Geometry::zeroTol)
	Out=ModelSupport::getComposite
	  (SMap,ch4Layer,"-2 (-23:-1021:1022) (-24:-1023:-1024)");
      else
	Out=ModelSupport::getComposite(SMap,ch4Layer," -2 ");
    }
  BX.procString(Out);      
  return;
}


void
CH4Layer::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("CH4Layer","createObjects");

  std::string Out;

  int ch4Layer(modIndex);

  HeadRule frontX,backX;

  Out=ModelSupport::getComposite(SMap,ch4Layer," 3 -4 5 -6 ");
  createFrontRule(LVec[0],modIndex,0,frontX);
  createBackRule(LVec[0],modIndex,0,backX);
		 
  // Outer layers:
  ch4Layer+=30;
  for(size_t i=1;i<LVec.size();i++)
    {
      HeadRule Exclude;
      Exclude.procString(Out);
      Exclude.addIntersection(frontX);
      Exclude.addIntersection(backX);
      Exclude.makeComplement();
      createFrontRule(LVec[i],modIndex,i,frontX);
      createBackRule(LVec[i],modIndex,i,backX);

      HeadRule Main;
      Out=ModelSupport::getComposite(SMap,ch4Layer," 3 -4 5 -6 ");
      Main.procString(Out);
      Main.addIntersection(Exclude);

      Main.addIntersection(frontX);
      Main.addIntersection(backX);
      System.addCell(MonteCarlo::Qhull
		     (cellIndex++,LVec[i].getMat(),
		      LVec[i].getTemp(),Main.display()));

      ch4Layer+=30;
    }
  ch4Layer-=30;
  Out=ModelSupport::getComposite(SMap,ch4Layer," 3 -4 5 -6 ");
  Out+=frontX.display()+" "+backX.display();
  addOuterSurf(Out);
  addBoundarySurf(Out);

  return;
}


Geometry::Vec3D
CH4Layer::getSurfacePoint(const size_t layerIndex,
			  const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link point
    \param sideIndex :: Side [0-5]
    \param layerIndex :: layer, 0 is inner moderator [0-6]
    \return Surface point
  */
{
  ELog::RegMethod RegA("CH4Layer","getSurfacePoint");

  const size_t SI((sideIndex>0) ?
                  static_cast<size_t>(sideIndex-1) :
                  static_cast<size_t>(-1-sideIndex));

  if (SI>5) 
    throw ColErr::IndexError<long int>(sideIndex,6,"sideIndex ");
  if (layerIndex>=LVec.size()) 
    throw ColErr::IndexError<size_t>(layerIndex,LVec.size(),"layerIndex");

  const Geometry::Vec3D XYZ[6]={-Y,Y,-X,X,-Z,Z};
  return Origin+XYZ[SI]*LVec[layerIndex].Item(SI);
}

std::string
CH4Layer::getLayerString(const size_t layerIndex,
			 const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link surf
    \param sideIndex :: Side [1-6]
    \param layerIndex :: layer, 0 is inner moderator [0-LVec]
    \return Surface string
  */
{
  ELog::RegMethod RegA("CH4Layer","getLayerString");

  const long int uSIndex(std::abs(sideIndex));
  if (uSIndex==1 || uSIndex==2)  // Front/back
    {
      HeadRule RX;
      if (uSIndex==1)
	createFrontRule(LVec[layerIndex],modIndex,layerIndex,RX);
      else
	createBackRule(LVec[layerIndex],modIndex,layerIndex,RX);
      if (sideIndex<0)
	RX.makeComplement();
      return RX.display();
    }
  return StrFunc::makeString(getLayerSurf(layerIndex,sideIndex));
}

int
CH4Layer::getLayerSurf(const size_t layerIndex,
		       const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link surf
    \param sideIndex :: Side [1-6]
    \param layerIndex :: layer, 0 is inner moderator [0-4]
    \return Surface size
  */
{
  ELog::RegMethod RegA("CH4Layer","getLayerSurf");

  if (layerIndex>=LVec.size()) 
    throw ColErr::IndexError<size_t>(layerIndex,LVec.size(),"layerIndex");
  if (sideIndex>6 || sideIndex<-6 || !sideIndex)
    throw ColErr::IndexError<long int>(sideIndex,6,"sideIndex");

  const size_t uSIndex(static_cast<size_t>(std::abs(sideIndex)));
  const int SI(modIndex+static_cast<int>(layerIndex*30+uSIndex));
  int signValue=(sideIndex % 2) ? -1 : 1;
  if (sideIndex<0)
    signValue*=-1;

  if ((sideIndex==1 || sideIndex==2) && 
      LVec[uSIndex-1].Item(uSIndex+5)>Geometry::zeroTol)
    return SMap.realSurf(SI);
  

  return signValue*SMap.realSurf(SI);
}


void
CH4Layer::createLinks()
  /*!
    Creates a full attachment set
  */
{
  // set Links:
  ELog::RegMethod RegA("CH4Layer","createLinks");

  const Geometry::Vec3D XYZ[6]={-Y,Y,-X,X,-Z,Z};
  // NOT CORRECT?
  for(size_t j=0;j<6;j++)
    {
      FixedComp::setConnect(j,Origin+XYZ[j]*
			    LVec.back().Item(j),XYZ[j]);
    }
  const size_t lIndex(LVec.size()-1);
  for(size_t i=0;i<6;i++)
    FixedComp::setLinkSurf(i,getLayerSurf(lIndex,static_cast<long int>(i+1)));

  return;
}

void
CH4Layer::createAll(Simulation& System,
		    const attachSystem::FixedComp& axisFC,
		    const attachSystem::FixedComp* orgFC,
		    const long int sideIndex)
  /*!
    Extrenal build layered moderator
    \param System :: Simulation
    \param axisFC :: FixedComp to get axis [origin if orgFC == 0]
    \param orgFC :: Extra origin point if required
    \param sideIndex :: link point for origin if given
   */
{
  ELog::RegMethod RegA("CH4Layer","createAll");

  populate(System.getDataBase());
  createUnitVector(axisFC,orgFC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       
  return;
}

  
}  // NAMESPACE ts1System
