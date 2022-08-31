/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Build/CH4Moderator.cxx
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
#include <algorithm>
#include <numeric>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "surfRegister.h"
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
#include "BaseMap.h"
#include "CellMap.h"
#include "LayerComp.h"
#include "ContainedComp.h"
#include "CH4Moderator.h"

namespace ts1System
{

CH4Moderator::CH4Moderator(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::LayerComp(4),
  attachSystem::FixedRotate(Key,6),
  attachSystem::CellMap()
  
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

CH4Moderator::CH4Moderator(const CH4Moderator& A) : 
  attachSystem::ContainedComp(A),attachSystem::LayerComp(A),
  attachSystem::FixedRotate(A),attachSystem::CellMap(A),
  width(A.width),height(A.height),depth(A.depth),
  viewSphere(A.viewSphere),innerThick(A.innerThick),
  vacThick(A.vacThick),outerThick(A.outerThick),
  clearThick(A.clearThick),vacTop(A.vacTop),
  outerView(A.outerView),clearTop(A.clearTop),
  poisonXStep(A.poisonXStep),poisonYStep(A.poisonYStep),
  poisonZStep(A.poisonZStep),poisonGdThick(A.poisonGdThick),
  poisonAlThick(A.poisonAlThick),alMat(A.alMat),
  ch4Mat(A.ch4Mat),voidMat(A.voidMat),
  poisonMat(A.poisonMat),ch4Temp(A.ch4Temp)
  /*!
    Copy constructor
    \param A :: CH4Moderator to copy
  */
{}

CH4Moderator&
CH4Moderator::operator=(const CH4Moderator& A)
  /*!
    Assignment operator
    \param A :: CH4Moderator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::LayerComp::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      attachSystem::CellMap::operator=(A);
      width=A.width;
      height=A.height;
      depth=A.depth;
      viewSphere=A.viewSphere;
      innerThick=A.innerThick;
      vacThick=A.vacThick;
      outerThick=A.outerThick;
      clearThick=A.clearThick;
      vacTop=A.vacTop;
      outerView=A.outerView;
      clearTop=A.clearTop;
      poisonXStep=A.poisonXStep;
      poisonYStep=A.poisonYStep;
      poisonZStep=A.poisonZStep;
      poisonGdThick=A.poisonGdThick;
      poisonAlThick=A.poisonAlThick;      
      alMat=A.alMat;
      ch4Mat=A.ch4Mat;
      voidMat=A.voidMat;
      poisonMat=A.poisonMat;      
      ch4Temp=A.ch4Temp;
    }
  return *this;
}

CH4Moderator::~CH4Moderator() 
 /*!
   Destructor
 */
{}

void
CH4Moderator::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: DataBase to use
 */
{
  ELog::RegMethod RegA("CH4Moderator","populate");
  
  nLayers=4;  
  FixedRotate::populate(Control);
  
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  viewSphere=Control.EvalVar<double>(keyName+"ViewSphere");

  innerThick=Control.EvalVar<double>(keyName+"InnerThick");
  vacThick=Control.EvalVar<double>(keyName+"VacThick");
  outerThick=Control.EvalVar<double>(keyName+"OuterThick");
  clearThick=Control.EvalVar<double>(keyName+"ClearThick");

  vacTop=Control.EvalVar<double>(keyName+"VacTop");
  outerView=Control.EvalVar<double>(keyName+"OuterView");
  clearTop=Control.EvalVar<double>(keyName+"ClearTop");

// Poison
  poisonXStep=Control.EvalVar<double>(keyName+"PoisonXStep");
  poisonYStep=Control.EvalVar<double>(keyName+"PoisonYStep");
  poisonZStep=Control.EvalVar<double>(keyName+"PoisonZStep");
  poisonGdThick=Control.EvalVar<double>(keyName+"PoisonGdThick");
  poisonAlThick=Control.EvalVar<double>(keyName+"PoisonAlThick");

  // Materials
  alMat=ModelSupport::EvalMat<int>(Control,keyName+"AlMat");
  ch4Mat=ModelSupport::EvalMat<int>(Control,keyName+"CH4Mat");
  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  poisonMat=ModelSupport::EvalMat<int>(Control,keyName+"PoisonMat");
  
  ch4Temp=Control.EvalVar<double>(keyName+"CH4Temp");
  return;
}
  

void
CH4Moderator::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("CH4Moderator","createSurface");

  // Inner surfaces
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*depth/2.0,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*depth/2.0,Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*width/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*width/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*height/2.0,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*height/2.0,Z);

  ModelSupport::buildSphere(SMap,buildIndex+7,
   			    Origin-Y*(depth/2.0-viewSphere),
   			    viewSphere);
  ModelSupport::buildSphere(SMap,buildIndex+8,
   			    Origin+Y*(depth/2.0-viewSphere),
   			    viewSphere);

  // Modification map:
  std::map<size_t,double> modLayer;
  std::map<size_t,double>::const_iterator mc;
  const double layer[]={innerThick,vacThick,outerThick,clearThick};


  std::vector<double> T(6);  // Zero size
  int HI(buildIndex+10);
  for(size_t i=0;i<nLayers;i++)
    {
      // Add the new layer +/- the modification
      for(size_t surfIndex=0;surfIndex<6;surfIndex++)
	{
	  mc=modLayer.find(i*10+surfIndex+1);
	  T[surfIndex]+=(mc!=modLayer.end()) ? mc->second : layer[i];
	}

      ModelSupport::buildPlane(SMap,HI+1,
			       Origin-Y*(depth/2.0+T[0]),Y);
      ModelSupport::buildPlane(SMap,HI+2,
			       Origin+Y*(depth/2.0+T[1]),Y);
      ModelSupport::buildPlane(SMap,HI+3,
			       Origin-X*(width/2.0+T[2]),X);
      ModelSupport::buildPlane(SMap,HI+4,
			       Origin+X*(width/2.0+T[3]),X);
      ModelSupport::buildPlane(SMap,HI+5,
			       Origin-Z*(height/2.0+T[4]),Z);
      ModelSupport::buildPlane(SMap,HI+6,
			       Origin+Z*(height/2.0+T[5]),Z);
      HI+=10;
    }
//       ELog::EM<<"T = "<<T[2]<<ELog::endDebug;	    
    
  // Special 
  ModelSupport::buildSphere(SMap,buildIndex+17,
   			    Origin-Y*(innerThick+depth/2.0-viewSphere),
   			    viewSphere);
  ModelSupport::buildSphere(SMap,buildIndex+18,
   			    Origin+Y*(innerThick+depth/2.0-viewSphere),
   			    viewSphere);
   			    
  // Special 2 
  ModelSupport::buildSphere(SMap,buildIndex+27,
   			    Origin-Y*(innerThick+vacThick+depth/2.0-viewSphere),
   			    viewSphere);
  ModelSupport::buildSphere(SMap,buildIndex+28,
   			    Origin+Y*(innerThick+vacThick+depth/2.0-viewSphere),
   			    viewSphere); 
   			    
   // Special 3 
  ModelSupport::buildSphere(SMap,buildIndex+37,
   			    Origin-Y*(innerThick+vacThick+outerThick+depth/2.0-viewSphere),
   			    viewSphere);
  ModelSupport::buildSphere(SMap,buildIndex+38,
   			    Origin+Y*(innerThick+vacThick+outerThick+depth/2.0-viewSphere),
   			    viewSphere);   			      			      			    

  // Poison layers (Gd+Al):
  ModelSupport::buildPlane(SMap,buildIndex+101,Origin+
                      Y*(poisonYStep-poisonGdThick/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+102,Origin+
                      Y*(poisonYStep+poisonGdThick/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+111,Origin+
                      Y*(poisonYStep-(poisonGdThick+poisonAlThick)/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+112,Origin+
                      Y*(poisonYStep+(poisonGdThick+poisonAlThick)/2.0),Y);
  return;
}

void
CH4Moderator::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("CH4Moderator","createObjects");

  HeadRule HR;
  
    // CH4 + Poison
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"101 -102 3 -4 5 -6");
  makeCell("Poison",System,cellIndex++,poisonMat,ch4Temp,HR);
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,
	"111 -101 3 -4 5 -6 ");
  makeCell("AlLayer",System,cellIndex++,alMat,ch4Temp,HR);

  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-112 102 3 -4 5 -6");
  makeCell("AlLayer",System,cellIndex++,alMat,ch4Temp,HR);
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-7 -111 3 -4 5 -6");
  makeCell("CH4",System,cellIndex++,ch4Mat,ch4Temp,HR);
  
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-8 112 3 -4 5 -6");
  makeCell("CH4",System,cellIndex++,ch4Mat,ch4Temp,HR);

  // Inner al
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-17 -18 13 -14 15 -16 "
				 " (7:8:-3:4:-5:6)");
  makeCell("InnerAl",System,cellIndex++,alMat,ch4Temp,HR);

  // Vac layer
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"23 -24 -27 -28 25 -26 "
				 " (17:18:-13:14:-15:16)");
  makeCell("VacLayer",System,cellIndex++,0,0.0,HR);

  // Outer Al
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"33 -34 -37 -38 35 -36 "
				 " (-23:24:27:28:-25:26)");  
  makeCell("OuterAl",System,cellIndex++,alMat,0.0,HR);

  // Outer Al
  HR=ModelSupport::getHeadRule(SMap,buildIndex,"41 -42 43 -44 45 -46");
  addOuterSurf(HR);

  HR*=ModelSupport::getHeadRule(SMap,buildIndex,"(-33:34:37:38:-35:36)");  
  makeCell("OuterVoid",System,cellIndex++,0,0.0,HR);



  return;
}

void
CH4Moderator::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("CH4Moderator","createLinks");

  // set Links:
  const double T(innerThick+vacThick+outerThick+clearThick);
//            ELog::EM<<"T = "<<T<<ELog::endDebug;		 
  FixedComp::setConnect(0,Origin-Y*(depth/2.0+T),-Y);
  FixedComp::setConnect(1,Origin+Y*(depth/2.0+T),Y);  
  FixedComp::setConnect(2,Origin-X*(width/2.0+T),-X);
  FixedComp::setConnect(3,Origin+X*(width/2.0+T),X);
  FixedComp::setConnect(4,Origin-Z*(height/2.0+T),-Z);
  FixedComp::setConnect(5,Origin+Z*(height/2.0+T),Z);

  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+41));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+42));
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+43));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+44));
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+45));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+46));

  return;
}

Geometry::Vec3D
CH4Moderator::getSurfacePoint(const size_t layerIndex,
			      const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link point
    \param sideIndex :: Side [0-5]
    \param layerIndex :: layer, 0 is inner moderator [0-4]
    \return Surface point
  */
{
  ELog::RegMethod RegA("CH4Moderator","getSurfacePoint");

  /// accessor to origin:
  if (!sideIndex) return Origin;

  
  const size_t SI((sideIndex>0) ?
                  static_cast<size_t>(sideIndex-1) :
                  static_cast<size_t>(-1-sideIndex));
  if (layerIndex>nLayers || nLayers!=4)           // system only build for 4 
    throw ColErr::IndexError<size_t>(layerIndex,nLayers,"layer");


  // Modification map:
  std::map<size_t,double> modLayer;
  std::map<size_t,double>::const_iterator mc;
//  const double layer[]={0.0,innerThick,vacThick,outerThick,clearThick};
  const double layer[]={innerThick,vacThick,outerThick,clearThick};
  double T(0.0);
//  for(size_t i=0;i<=layerIndex;i++)
  for(size_t i=0;i<layerIndex;i++)
    {
      mc=modLayer.find(i*10+SI+1);
      T+=(mc!=modLayer.end()) ? mc->second : layer[i];
    }
  
  switch(SI)
    {
    case 0:
      return Origin-Y*(depth/2.0+T);
    case 1:
      return Origin+Y*(depth/2.0+T);
    case 2:
      return Origin-X*(width/2.0+T);
    case 3:
      return Origin+X*(width/2.0+T);    
    case 4:
      return Origin-Z*(height/2.0+T);
    case 5:
      return Origin+Z*(height/2.0+T);
    }
  // IMPOSSIBLE TO GET HERE
  throw ColErr::IndexError<long int>(sideIndex,6,"sideIndex ");
}

int
CH4Moderator::getLayerSurf(const size_t layerIndex,
			   const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link surf
    \param layerIndex :: layer, 0 is inner moderator [0-4]
    \param sideIndex :: Side [1-6]
    \return Surface number
  */
{
  ELog::RegMethod RegA("CH4Moderator","getLayerSurf");
  ELog::EM<<"CHECK OF LAYER/SIDE ORDER"<<ELog::endErr;
  if (!sideIndex || std::abs(sideIndex)>6) 
    throw ColErr::IndexError<long int>(sideIndex,6,"sideIndex ");
  if (layerIndex>4) 
    throw ColErr::IndexError<size_t>(layerIndex,4,"layer");

  int signValue=(sideIndex % 2 ) ? -1 : 1;
  const int dirValue=(sideIndex<0) ? -1 : 1;
  const int uSIndex(static_cast<int>(std::abs(sideIndex)));
  
  if (uSIndex>3 || layerIndex>2)
    {
      const int surfN(buildIndex+
		      static_cast<int>(10*layerIndex)+uSIndex);
      return dirValue*signValue*SMap.realSurf(surfN);
    }
  
  const int surfN(buildIndex+
		  static_cast<int>(10*layerIndex)+uSIndex+6);
  return dirValue*SMap.realSurf(surfN);
}

std::string
CH4Moderator::getLayerString(const size_t layerIndex,
			     const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link surf
    \param layerIndex :: layer, 0 is inner moderator [0-4]
    \param sideIndex :: Side [1-6]
    \return Surface string
  */
{
  ELog::RegMethod RegA("CH4Moderator","getLayerString");

  if (sideIndex>6 || sideIndex<-6 || !sideIndex) 
    throw ColErr::IndexError<long int>(sideIndex,6,"sideIndex");
  if (layerIndex>4) 
    throw ColErr::IndexError<size_t>(layerIndex,4,"layer");

  std::ostringstream cx;
  const int signValue=(sideIndex % 2) ? -1 : 1;
  const int uSIndex(static_cast<int>(std::abs(sideIndex)));
  HeadRule HR;
  
  if (uSIndex>3 || layerIndex>2)
    {
      const int surfN(buildIndex+static_cast<int>(10*layerIndex)+uSIndex);
      HR.addIntersection(signValue*SMap.realSurf(surfN));
    }
  else
    {
      const int surfN(buildIndex+static_cast<int>(10*layerIndex)+uSIndex+6);
      HR.addIntersection(SMap.realSurf(surfN));
    }
  if (sideIndex<0)
    HR.makeComplement();
  return HR.display();
}


void
CH4Moderator::createAll(Simulation& System,
			const attachSystem::FixedComp& FC,
			const long int sideIndex)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: Fixed Component to place object within
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("CH4Moderator","createAll");

  populate(System.getDataBase());

  createUnitVector(FC,sideIndex); 
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE ts1System
