/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   lensModel/layers.cxx
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
#include <functional>
#include <iterator>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfExpand.h"
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Sphere.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "SimProcess.h"
#include "SurInter.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "siModerator.h"
#include "surfDIter.h"
#include "ProtonFlight.h"
#include "FlightLine.h"
#include "FlightCluster.h"
#include "candleStick.h"
#include "boxValues.h"
#include "layers.h"

namespace lensSystem
{

layers::layers(const std::string& Key) :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,3),
  surIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(surIndex+1),populated(0),innerCompSurf(0),
  PA("proton"),FC("flight")
  /*!
    Constructor
    \param Key :: KeyName
  */
{}

layers::layers(const layers& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  surIndex(A.surIndex),cellIndex(A.cellIndex),
  populated(A.populated),innerCompSurf(A.innerCompSurf),PA(A.PA),
  FC(A.FC),waterRad(A.waterRad),DCRad(A.DCRad),leadRad(A.leadRad),
  bPolyRad(A.bPolyRad),epoxyRad(A.epoxyRad),outPolyRad(A.outPolyRad),
  outVoidRad(A.outVoidRad),waterAlThick(A.waterAlThick),
  waterHeight(A.waterHeight),
  waterDepth(A.waterDepth),DCHeight(A.DCHeight),DCDepth(A.DCDepth),
  leadHeight(A.leadHeight),leadDepth(A.leadDepth),bPolyHeight(A.bPolyHeight),
  bPolyDepth(A.bPolyDepth),epoxyHeight(A.epoxyHeight),epoxyDepth(A.epoxyDepth),
  outPolyHeight(A.outPolyHeight),outPolyDepth(A.outPolyDepth),
  wedgeWidth(A.wedgeWidth),wedgeHeight(A.wedgeHeight),
  nWedge(A.nWedge),wedgeLiner(A.wedgeLiner),wedgeLinerMat(A.wedgeLinerMat),
  waterMat(A.waterMat),alMat(A.alMat),DCMat(A.DCMat),leadMat(A.leadMat),
  bPolyMat(A.bPolyMat),epoxyMat(A.epoxyMat),outPolyMat(A.outPolyMat)
  /*!
    Copy constructor
    \param A :: layers to copy
  */
{
  wedgeAngleXY[0]=A.wedgeAngleXY[0];      
  wedgeAngleXY[1]=A.wedgeAngleXY[1];
}

layers&
layers::operator=(const layers& A)
  /*!
    Assignment operator
    \param A :: layers to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      populated=A.populated;
      innerCompSurf=A.innerCompSurf;
      PA=A.PA;
      FC=A.FC;
      waterRad=A.waterRad;
      DCRad=A.DCRad;
      leadRad=A.leadRad;
      bPolyRad=A.bPolyRad;
      epoxyRad=A.epoxyRad;
      outPolyRad=A.outPolyRad;
      outVoidRad=A.outVoidRad;
      waterAlThick=A.waterAlThick;
      waterHeight=A.waterHeight;
      waterDepth=A.waterDepth;
      DCHeight=A.DCHeight;
      DCDepth=A.DCDepth;
      leadHeight=A.leadHeight;
      leadDepth=A.leadDepth;
      bPolyHeight=A.bPolyHeight;
      bPolyDepth=A.bPolyDepth;
      epoxyHeight=A.epoxyHeight;
      epoxyDepth=A.epoxyDepth;
      outPolyHeight=A.outPolyHeight;
      outPolyDepth=A.outPolyDepth;
      wedgeWidth=A.wedgeWidth;
      wedgeHeight=A.wedgeHeight;
      wedgeAngleXY[0]=A.wedgeAngleXY[0];      
      wedgeAngleXY[1]=A.wedgeAngleXY[1];
      nWedge=A.nWedge;
      wedgeLiner=A.wedgeLiner;
      wedgeLinerMat=A.wedgeLinerMat;
      waterMat=A.waterMat;
      alMat=A.alMat;
      DCMat=A.DCMat;
      leadMat=A.leadMat;
      bPolyMat=A.bPolyMat;
      epoxyMat=A.epoxyMat;
      outPolyMat=A.outPolyMat;
    }
  return *this;
}

void
layers::createUnitVector(const attachSystem::FixedComp& FC)
/*!
  Create the unit vectors
  - Y towards the normal of the target
  - X across the moderator
  - Z up / down (gravity)
  \param FC :: A Contained FixedComp to use as basis set
*/
{
  ELog::RegMethod RegA("layers","createUnitVector");
  FixedComp::createUnitVector(FC);  
  return;
}
  
void
layers::populate(const FuncDataBase& Control)
  /*!
    Populate the variables
    \param Control :: Database
  */
{
  ELog::RegMethod RegA("layers","populate");
  
  FC.clearCells();
  waterRad=Control.EvalVar<double>(keyName+"WaterRadius");
  DCRad=Control.EvalVar<double>(keyName+"DCRadius");
  leadRad=Control.EvalVar<double>(keyName+"LeadRadius");
  bPolyRad=Control.EvalVar<double>(keyName+"BPolyRadius");
  epoxyRad=Control.EvalVar<double>(keyName+"EpoxyRadius");
  outPolyRad=Control.EvalVar<double>(keyName+"OutPolyRadius");
  outVoidRad=Control.EvalVar<double>(keyName+"OutVoidRadius");

  waterHeight=Control.EvalVar<double>(keyName+"WaterHeight");
  DCHeight=Control.EvalVar<double>(keyName+"DCHeight");
  leadHeight=Control.EvalVar<double>(keyName+"LeadHeight");
  bPolyHeight=Control.EvalVar<double>(keyName+"BPolyHeight");
  epoxyHeight=Control.EvalVar<double>(keyName+"EpoxyHeight");
  outPolyHeight=Control.EvalVar<double>(keyName+"OutPolyHeight");

  waterDepth=Control.EvalVar<double>(keyName+"WaterDepth");
  DCDepth=Control.EvalVar<double>(keyName+"DCDepth");
  leadDepth=Control.EvalVar<double>(keyName+"LeadDepth");
  bPolyDepth=Control.EvalVar<double>(keyName+"BPolyDepth");
  epoxyDepth=Control.EvalVar<double>(keyName+"EpoxyDepth");
  outPolyDepth=Control.EvalVar<double>(keyName+"OutPolyDepth");

  waterMat=ModelSupport::EvalMat<int>(Control,keyName+"WaterMat");
  DCMat=ModelSupport::EvalMat<int>(Control,keyName+"DCMat");
  leadMat=ModelSupport::EvalMat<int>(Control,keyName+"LeadMat");
  bPolyMat=ModelSupport::EvalMat<int>(Control,keyName+"BPolyMat");
  epoxyMat=ModelSupport::EvalMat<int>(Control,keyName+"EpoxyMat");
  outPolyMat=ModelSupport::EvalMat<int>(Control,keyName+"OutPolyMat");
  alMat=ModelSupport::EvalMat<int>(Control,keyName+"AlMat");
  
  waterAlThick=Control.EvalVar<double>(keyName+"WaterAlThick");

  wedgeAngleXY[0]=Control.EvalVar<double>(keyName+"WAngleXY1");
  wedgeAngleXY[1]=Control.EvalVar<double>(keyName+"WAngleXY2");
  wedgeWidth=Control.EvalVar<double>(keyName+"WWidth");
  wedgeHeight=Control.EvalVar<double>(keyName+"WHeight");

  nWedge=Control.EvalVar<size_t>(keyName+"NWedgeLayers");

  ModelSupport::populateDivide(Control,nWedge,
			      keyName+"WLinerMat",0,wedgeLinerMat);
  ModelSupport::populateDivide(Control,nWedge+1,
			       keyName+"WLiner",wedgeLiner);
  wedgeLinerMat.push_back(0); // Void in middle

  ELog::EM<<"Size == "<<wedgeLinerMat[0]<<" "<<wedgeLinerMat.size()
	  <<ELog::endTrace;
  populated=1;
  return;
}

void 
layers::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("layers","createSurfaces");
  
  // Water Layer:
  ModelSupport::buildCylinder(SMap,surIndex+107,Origin,Z,waterRad);
  ModelSupport::buildPlane(SMap,surIndex+105,Origin-Z*waterDepth,Z);
  ModelSupport::buildPlane(SMap,surIndex+106,Origin+Z*waterHeight,Z);

  // Aluminium Layer : 
  ModelSupport::buildCylinder(SMap,surIndex+117,Origin,Z,waterRad+waterAlThick);
  ModelSupport::buildPlane(SMap,surIndex+115,Origin-Z*(waterDepth+waterAlThick),Z);
  ModelSupport::buildPlane(SMap,surIndex+116,Origin+Z*(waterHeight+waterAlThick),Z);

  // Decoupler Layer:
  ModelSupport::buildCylinder(SMap,surIndex+127,Origin,Z,DCRad);
  ModelSupport::buildPlane(SMap,surIndex+125,Origin-Z*DCDepth,Z);
  ModelSupport::buildPlane(SMap,surIndex+126,Origin+Z*DCHeight,Z);

  // Lead layer
  ModelSupport::buildCylinder(SMap,surIndex+137,Origin,Z,leadRad);
  ModelSupport::buildPlane(SMap,surIndex+135,Origin-Z*leadDepth,Z);
  ModelSupport::buildPlane(SMap,surIndex+136,Origin+Z*leadHeight,Z);

  // BPoly
  ModelSupport::buildCylinder(SMap,surIndex+147,Origin,Z,bPolyRad);
  ModelSupport::buildPlane(SMap,surIndex+145,Origin-Z*bPolyDepth,Z);
  ModelSupport::buildPlane(SMap,surIndex+146,Origin+Z*bPolyHeight,Z);

  // Epoxy
  ModelSupport::buildCylinder(SMap,surIndex+157,Origin,Z,epoxyRad);
  ModelSupport::buildPlane(SMap,surIndex+155,Origin-Z*epoxyDepth,Z);
  ModelSupport::buildPlane(SMap,surIndex+156,Origin+Z*epoxyHeight,Z);

  // Outer
  ModelSupport::buildCylinder(SMap,surIndex+167,Origin,Z,outPolyRad);
  ModelSupport::buildPlane(SMap,surIndex+165,Origin-Z*outPolyDepth,Z);
  ModelSupport::buildPlane(SMap,surIndex+166,Origin+Z*outPolyHeight,Z);

  // Outer void
  ModelSupport::buildCylinder(SMap,surIndex+177,Origin,Z,outVoidRad);

  // Wedge 
  Geometry::Vec3D xDircA(X);
  Geometry::Vec3D xDircB(X);
  Geometry::Quaternion::calcQRotDeg(wedgeAngleXY[0],Z).rotate(xDircA);
  Geometry::Quaternion::calcQRotDeg(wedgeAngleXY[1],Z).rotate(xDircB);

  ModelSupport::buildPlane(SMap,surIndex+203,
			   Origin-X*wedgeWidth/2.0,xDircA);
  ModelSupport::buildPlane(SMap,surIndex+204,
			   Origin+X*wedgeWidth/2.0,xDircB);
  ModelSupport::buildPlane(SMap,surIndex+205,Origin-Z*wedgeHeight/2.0,Z);
  ModelSupport::buildPlane(SMap,surIndex+206,Origin+Z*wedgeHeight/2.0,Z);

  // Layers of Wedge:
  for(int i=0;i<static_cast<int>(nWedge);i++)
    {
      const unsigned int uI(static_cast<unsigned int>(i));
      ModelSupport::buildPlane(SMap,surIndex+213+10*i,
       Origin-X*wedgeWidth/2.0+xDircA*wedgeLiner[uI],xDircA);
      ModelSupport::buildPlane(SMap,surIndex+214+10*i,
       Origin+X*wedgeWidth/2.0-xDircB*wedgeLiner[uI],xDircB);

      ModelSupport::buildPlane(SMap,surIndex+215+10*i,
         Origin-Z*wedgeHeight/2.0+Z*wedgeLiner[uI],Z);
      ModelSupport::buildPlane(SMap,surIndex+216+10*i,
          Origin+Z*wedgeHeight/2.0-Z*wedgeLiner[uI],Z);
    }
  return;
}

void 
layers::createObjects(Simulation& System,
		      const candleStick& CS)
  /*!
    Create all the objects
    \param System :: Simulation
    \param CS :: CandleStick object (for exclude)
  */
{
  ELog::RegMethod RegA("layers","createObjects");

  std::string Wedge,Out;
  // Build Wedge Takeout
  Wedge=ModelSupport::getComposite(SMap,surIndex,"-127 ")+
    CS.getLinkString(2);
  
  // Build Wedge units [Always build 1]
  for(int i=0;i<=static_cast<int>(nWedge);i++)
    {
      const unsigned int uI(static_cast<unsigned int>(i));
      Out=ModelSupport::getComposite(SMap,surIndex+i*10,"203 -204 205 -206 ");
      Out+=Wedge;
      if (i!=static_cast<int>(nWedge))
	Out+=ModelSupport::getComposite(SMap,surIndex+(i+1)*10,
					"(-203:204:-205:206) ");

      System.addCell(MonteCarlo::Qhull(cellIndex++,
				       wedgeLinerMat[uI],0.0,Out));
    }
  
  Wedge+=ModelSupport::getComposite(SMap,surIndex," 203 -204 205 -206 ");  

  // Build objects:
  // Inner water
  Out=ModelSupport::getComposite(SMap,surIndex,"105 -106 -107 ");  
  Out+=CS.getTopExclude();
  Out+="#("+Wedge+")";
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,0.0,Out));
  //  FC.addInsertCell(cellIndex-1);
  PA.addAllInsertCell(cellIndex-1);

  // Outer Al :
  Out=ModelSupport::getComposite(SMap,surIndex,"115 -116 -117 (-105:106:107) ");
  Out+=CS.getTopExclude();
  Out+="#("+Wedge+")";
  FC.addInsertCell(cellIndex);
  PA.addInsertCell("line",cellIndex);
  System.addCell(MonteCarlo::Qhull(cellIndex++,alMat,0.0,Out));

  // DECOUPLER:
  Out=ModelSupport::getComposite(SMap,surIndex,"125 -126 -127 (-115 : 116 : 117) ");
  //				 "(-71 : 72 :-73 : 74 : -115 )");
  Out+=CS.getExclude()+CS.getHeadExclude();
  Out+="#("+Wedge+")";
  System.addCell(MonteCarlo::Qhull(cellIndex++,DCMat,0.0,Out));  
  FC.addInsertCell(cellIndex-1);
  PA.addInsertCell("line",cellIndex-1);

  // LEAD:
  Out=ModelSupport::getComposite(SMap,surIndex,"135 -136 -137 (-125 : 126 : 127)");
  // (56 : -71 : 72 : -73 : 74 : -125)");    
  Out+=CS.getExclude()+CS.getHeadExclude();
  System.addCell(MonteCarlo::Qhull(cellIndex++,leadMat,0.0,Out));
  FC.addInsertCell(cellIndex-1);
  PA.addInsertCell("line",cellIndex-1);

  // BPOLY
  Out= ModelSupport::getComposite(SMap,surIndex,"145 -146 -147 "
				  "(-135 : 136 : 137) ");
  Out+=CS.getExclude()+CS.getHeadExclude();
  System.addCell(MonteCarlo::Qhull(cellIndex++,bPolyMat,0.0,Out));  
  FC.addInsertCell(cellIndex-1);
  PA.addInsertCell("line",cellIndex-1);

  // EPOXY
  Out= ModelSupport::getComposite(SMap,surIndex,"155 -156 -157 "
				  "(-145 : 146 : 147) ");
  Out+=CS.getExclude();
  System.addCell(MonteCarlo::Qhull(cellIndex++,epoxyMat,0.0,Out));
  FC.addInsertCell(cellIndex-1);
  PA.addInsertCell("line",cellIndex-1);

  // OUT POLY
  Out=ModelSupport::getComposite(SMap,surIndex,"165 -166 -167 "
				 "(-155 : 156 : 157) ");
  Out+=CS.getExclude();
  System.addCell(MonteCarlo::Qhull(cellIndex++,outPolyMat,0.0,Out));
  FC.addInsertCell(cellIndex-1);
  PA.addInsertCell("line",cellIndex-1);

  // OUT VOID
  Out=ModelSupport::getComposite(SMap,surIndex,"165 -166 -177 167");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  FC.addInsertCell(cellIndex-1);
  PA.addInsertCell("line",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,surIndex,"165 -166 -177 ");
  addOuterSurf(Out);
  return;
}

void
layers::createLinks(const attachSystem::FixedComp& CS)
  /*!
    Create teh links. This is really really ugly since
    it uses the Candle stick to get the dividing surface
    AND y is positive in both directions [is this right?]
    \param CS :: CandleStick
   */
{
  ELog::RegMethod RegA("layers","createLinks");


  for(size_t i=0;i<2;i++)
    {
      FixedComp::setConnect(i,Origin,Y);
      FixedComp::setLinkSurf(i,SMap.realSurf(surIndex+127));
      FixedComp::addLinkSurf(i,CS.getLinkSurf(1));
    }

  return;
}


void
layers::constructFlightLines(Simulation& System,
			     const attachSystem::FixedComp& Vac)
/*!
    Construction Flight Lines:
    \param System :: Simulation
    \param Vac :: Vacuum container
  */
{
  ELog::RegMethod RegA("layers","constructFlightLines");

  FC.createAll(System,*this,-SMap.realSurf(surIndex+177));

  // Add outer cylinder
  PA.addBack(SMap.realSurf(surIndex+177));
  PA.createAll(System,Vac,0);

  return;
}


void
layers::createAll(Simulation& System,const candleStick& CS)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param CS :: CandleStick
   */
{
  ELog::RegMethod RegA("layers","createAll");
  
  populate(System.getDataBase());
  createUnitVector(CS);
  createSurfaces();
  createObjects(System,CS);
  createLinks(CS);
  constructFlightLines(System,CS);
  insertObjects(System);   

  return;
}

} // NAMESPACE LensSystem

