/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/Box.cxx
 *
 * Copyright (c) 2017 by Konstantin Batkov
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "inputParam.h"
#include "HeadRule.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ReadFunctions.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "LayerComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "surfDBase.h"
#include "surfDIter.h"
#include "surfDivide.h"
#include "mergeTemplate.h"
#include "SurInter.h"

#include "BoxShutter.h"

namespace essSystem
{

BoxShutter::BoxShutter(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,9),
  attachSystem::LayerComp(0),
  attachSystem::CellMap()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

BoxShutter::BoxShutter(const BoxShutter& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),
  attachSystem::LayerComp(A),
  attachSystem::CellMap(A),
  // engActive(A.engActive),
  nSlices(A.nSlices),
  length(A.length),width(A.width),height(A.height),
  depth(A.depth),
  mat(A.mat),
  temp(A.temp),
  sideRule(A.sideRule)
  /*!
    Copy constructor
    \param A :: BoxShutter to copy
  */
{}

BoxShutter&
BoxShutter::operator=(const BoxShutter& A)
  /*!
    Assignment operator
    \param A :: BoxShutter to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      //    attachSystem::LayerComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      // engActive=A.engActive;
      nSlices=A.nSlices;
      length=A.length;
      width=A.width;
      height=A.height;
      depth=A.depth;
      mat=A.mat;
      temp=A.temp;
      sideRule=A.sideRule;
    }
  return *this;
}


  BoxShutter::~BoxShutter()
  /*!
    Destructor
  */
{}

void
BoxShutter::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("BoxShutter","populate");

  FixedOffset::populate(Control);
  // engActive=Control.EvalPair<int>(keyName,"","EngineeringActive");

  double L(0.0);
  double W(0.0);
  double H(0.0);
  double D(0.0);
  double T(0.0);
  size_t nL(0);
  int M;
  nSlices=Control.EvalVar<size_t>(keyName+"NSlices");
      width=Control.EvalVar<double>(keyName+"Width");
     height=Control.EvalVar<double>(keyName+"Height");
     depth=Control.EvalVar<double>(keyName+"Depth");

  for (size_t i=0; i<nSlices; i++)
    {
      const std::string NStr(std::to_string(i));
      L=Control.EvalVar<double>(keyName+"Length"+NStr) ;
      nL=Control.EvalVar<size_t>(keyName+"NLayers"+NStr);
      nLayers.push_back(nL);
      //      W+=Control.EvalVar<double>(keyName+"Width"+NStr) * ((i==0) ? 1.0 : 2.0);
      //      H+=Control.EvalVar<double>(keyName+"Height"+NStr);
      //      D+=Control.EvalVar<double>(keyName+"Depth"+NStr);
      M=ModelSupport::EvalMat<int>(Control,keyName+"Mat"+NStr);
      const std::string TStr=keyName+"Temp"+NStr;
      T=(!M || !Control.hasVariable(TStr)) ? 0.0 : Control.EvalVar<double>(TStr);

      length.push_back(L);
      // width.push_back(W);
      // height.push_back(H);
      // depth.push_back(D);
      //      width = Control.EvalVar<double>(keyName+"Width");
      //      height = Control.EvalVar<double>(keyName+"Height");
      //      depth =Control.EvalVar<double>(keyName+"Depth");
      mat.push_back(M);
      temp.push_back(T);
    }

  return;
}

void
BoxShutter::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: object for origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("BoxShutter","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  //  const double D=(depth.empty()) ? 0.0 : depth.back();
  const double D=depth;
      applyShift(0,0,D);

  return;
}

void
BoxShutter::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("BoxShutter","createSurfaces");

  int SI(buildIndex);
      ModelSupport::buildPlane(SMap,SI+3,Origin-X*width/2.0,X);
      ModelSupport::buildPlane(SMap,SI+4,Origin+X*width/2.0,X);
      ModelSupport::buildPlane(SMap,SI+5,Origin-Z*(height/2.0),Z);
      ModelSupport::buildPlane(SMap,SI+6,Origin+Z*(height/2.0),Z);

      //      ModelSupport::buildPlane(SMap,SI+1,Origin-Y*L,Y);
      // ModelSupport::buildPlane(SMap,SI+2,Origin+Y*L,Y);
  
  int LI(0);    
  double L (0);
  for (size_t i=0;i<nSlices;i++)
    {
      L+=length[i];
   };
  double l = L/2.0;
      for (size_t i=0;i<nSlices;i++)
    {
      ModelSupport::buildPlane(SMap,SI+LI+1,Origin-Y*l,Y);
      LI += 10;
      l-=length[i];
     
    }
    ModelSupport::buildPlane(SMap,SI+2,Origin+Y*L/2.0,Y);
 
  return;
}

void
BoxShutter::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("BoxShutter","createObjects");

  std::string Out;
  int SI(buildIndex),LI(0);
  for (size_t i=0; i<nSlices-1; i++)
    {
      Out=ModelSupport::getComposite
	(SMap,SI,std::to_string(LI+1)+" -"+std::to_string(LI+11) + " 3 -4 5 -6 ");
      
      System.addCell(MonteCarlo::Object(cellIndex++,mat[i],temp[i],Out));
      CellMap::setCell("Layer"+std::to_string(i),cellIndex-1);

      LI += 10;
    }
      Out=ModelSupport::getComposite
	(SMap,SI,std::to_string(LI+1)+" -2 3 -4 5 -6 ");
      
  System.addCell(MonteCarlo::Object(cellIndex++,mat[nSlices-1],temp[nSlices-1],Out));
  CellMap::setCell("Layer"+std::to_string(nSlices-1),cellIndex-1);

     
  

  //  Out=ModelSupport::getComposite(SMap,SI-10," 1 -2 3 -4 5 -6 ");
    Out=ModelSupport::getComposite(SMap,SI," 1 -2 3 -4 5 -6 ");
  addOuterSurf(Out);

  sideRule=ModelSupport::getComposite(SMap,SI-10," 1 -2 3 -4 ");

  return;
}


void 
BoxShutter::layerProcess(Simulation& System)
  /*!
    Processes the splitting of the surfaces into a multilayer system
    \param System :: Simulation to work on
  */
{
  ELog::RegMethod RegA("BoxShutter","layerProcess");
  // Wall layers
  //  layerSpecial(System);
  int SI(buildIndex);
   int LI(0);
      for (size_t is=0; is<nSlices-1; is++){  
      std::string OutA,OutB;
      ModelSupport::surfDivide DA;
      double baseFrac = 1.0/static_cast<double>(nLayers[is]);
        for (size_t i=1; i<nLayers[is]; i++){
	  //	  DA.addFrac(wallFracList[i-1]);
	  //	  DA.addMaterial(wallMatList[i-1]);	  
	  DA.addFrac(baseFrac);
	  baseFrac += 1.0/static_cast<double>(nLayers[is]);
      	  DA.addMaterial(mat[is]);
	}
      DA.addMaterial(mat[is]);


      DA.setCellN(CellMap::getCell("Layer"+std::to_string(is)));
         DA.setOutNum(cellIndex,buildIndex+9001);

      ModelSupport::mergeTemplate<Geometry::Plane,
				  Geometry::Plane> surroundRule;
      // Original
      surroundRule.setSurfPair(SMap.realSurf(SI+LI+11),
			       SMap.realSurf(SI+LI+1));
      OutA=ModelSupport::getComposite(SMap,SI,"-"+std::to_string(LI+11));
      OutB=ModelSupport::getComposite(SMap,SI,
				      std::to_string(LI+1));
      LI+=10;
      surroundRule.setInnerRule(OutA);
      surroundRule.setOuterRule(OutB);
      DA.addRule(&surroundRule);
      DA.activeDivideTemplate(System);
      cellIndex=DA.getCellNum();
      }


      std::string OutA,OutB;
      ModelSupport::surfDivide DA;
        for (size_t i=1; i<nLayers[nSlices-1]; i++){
	  //	  DA.addFrac(wallFracList[i-1]);
	  //	  DA.addMaterial(wallMatList[i-1]);	  
	  DA.addFrac((1.0*i/nLayers[nSlices-1]));
      	  DA.addMaterial(mat[nSlices-1]);
	}
      DA.addMaterial(mat[nSlices-1]);


      DA.setCellN(CellMap::getCell("Layer"+std::to_string(nSlices-1)));
      DA.setOutNum(cellIndex,buildIndex+1001);

      ModelSupport::mergeTemplate<Geometry::Plane,
				  Geometry::Plane> surroundRule;
      // Original
      surroundRule.setSurfPair(SMap.realSurf(SI+2),
			       SMap.realSurf(SI+LI+1));
      OutA=ModelSupport::getComposite(SMap,SI,"-2");
      OutB=ModelSupport::getComposite(SMap,SI,
				      std::to_string(LI+1));
      surroundRule.setInnerRule(OutA);
      surroundRule.setOuterRule(OutB);
      DA.addRule(&surroundRule);
      DA.activeDivideTemplate(System);

      
      return;
    }


  
void
BoxShutter::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("BoxShutter","createLinks");

  //  const int SI(buildIndex+static_cast<int>(nSlices-1)*10);
    const int SI(buildIndex);

  const double l = length[nSlices-1]/2.0;
  const double w = width/2.0;

  FixedComp::setConnect(0,Origin-Y*l,-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(SI+1));
  FixedComp::setConnect(1,Origin+Y*l,Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(SI+2));

  FixedComp::setConnect(2,Origin-X*w,-X);
  FixedComp::setLinkSurf(2,-SMap.realSurf(SI+3));
  FixedComp::setConnect(3,Origin+X*w,X);
  FixedComp::setLinkSurf(3,SMap.realSurf(SI+4));

  FixedComp::setConnect(4,Origin-Z*(depth),-Z);
  FixedComp::setLinkSurf(4,-SMap.realSurf(SI+5));
  FixedComp::setConnect(5,Origin+Z*(height),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(SI+6));

  // inner link points for F5 collimators
  FixedComp::setConnect(7,Origin-Z*depth,Z);
  FixedComp::setLinkSurf(7,SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(8,Origin+Z*height,-Z);
  FixedComp::setLinkSurf(8,-SMap.realSurf(buildIndex+6));

  return;
}

int
BoxShutter::getLayerSurf(const size_t layerIndex,
		  const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link surf
    \param layerIndex :: layer, 0 is inner moderator [0-4]
    \param sideIndex :: Side [1-4]
    \return Surface string
  */
{
  ELog::RegMethod RegA("BoxShutter","getLinkSurf");

  throw ColErr::AbsObjMethod("Not implemented yet - do it similar to DiskPreMod?");
}


std::string
BoxShutter::getLayerString(const size_t layerIndex,
		    const long int sideIndex) const
  /*!
    Given a side and a layer calculate the layer string
    \param layerIndex :: layer, 0 is inner moderator [0-4]
    \param sideIndex :: Side [0-3]
    \return Surface string
  */
{
  ELog::RegMethod RegA("BoxShutter","getLayerString");

  if (layerIndex>nSlices)
    throw ColErr::IndexError<size_t>(layerIndex,nSlices,"layer");

  const int SI(10*static_cast<int>(layerIndex)+buildIndex);

  std::string Out;
  const long int uSIndex(std::abs(sideIndex));
  switch(uSIndex)
    {
    case 1:
      Out=ModelSupport::getComposite(SMap,SI," -1 ");
      break;
    case 2:
      Out=ModelSupport::getComposite(SMap,SI," 2 ");
      break;
    case 3:
      Out=ModelSupport::getComposite(SMap,SI," -3 ");
      break;
    case 4:
      Out=ModelSupport::getComposite(SMap,SI," 4 ");
      break;
    case 5:
      Out=ModelSupport::getComposite(SMap,SI," -5 ");
      break;
    case 6:
      Out=ModelSupport::getComposite(SMap,SI," 6 ");
      break;
    default:
      throw ColErr::IndexError<long int>(sideIndex,6,"sideIndex");
    }
  if (sideIndex<0)
    {
      HeadRule HR(Out);
      HR.makeComplement();
      return HR.display();
    }

  return Out;
}




void
BoxShutter::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("BoxShutter","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);
  layerProcess(System);

  return;
}

}  // essSystem
