/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/TMRCreate.cxx
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
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Tensor.h"
#include "Vec3D.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "weightManager.h"
#include "WForm.h"
#include "WItem.h"
#include "WCells.h"
#include "WeightModification.h"
#include "KGroup.h"
#include "Source.h"
#include "AlterSurfBase.h"
#include "AlterSurfTS2.h"
#include "RemoveCell.h"
#include "TS2remove.h"
#include "SimProcess.h"
#include "SurInter.h"
#include "SpecialSurf.h"
#include "Simulation.h"
#include "chipDataStore.h"
#include "TMRCreate.h"

namespace TMRSystem
{

void
setWeights(Simulation& System)
  /*!
    Function to set up the weights system.
    It replaces the old file read system.
    \param System :: Simulation component
  */
{
  ELog::RegMethod RegA("TMRCreate","setWeights(Simulation)");
  std::vector<double> Eval(6);
  Eval[0]=1.4678e-10;
  Eval[1]=3.1622e-10;
  Eval[2]=6.8130e-10;
  Eval[3]=1.4678e-9;
  Eval[4]=3.1622e-9;
  Eval[5]=5e7;

  std::vector<double> WT(6);
  WT[0]=0.02;WT[1]=0.07;WT[2]=0.1;
  WT[3]=0.1;WT[4]= 0.2;WT[5]=0.7;

  setWeights(System,Eval,WT);
  return;
}

void
setWeights(Simulation& System,const std::vector<double>& Eval,
	   const std::vector<double>& WT)
  /*!
    Function to set up the weights system.
    It replaces the old file read system.
    \param System :: Simulation component
    \param Eval :: Energy bins
    \param WT :: Weight scale
  */
{
  ELog::RegMethod RegA("TMRCreate","setWeights(Simulation,vec,vec)");

  WeightSystem::weightManager& WM=
    WeightSystem::weightManager::Instance();  

  WM.addParticle<WeightSystem::WCells>('n');
  WeightSystem::WCells* WF=
    dynamic_cast<WeightSystem::WCells*>(WM.getParticle('n'));

  WF->setEnergy(Eval);
  System.populateWCells();
  WF->balanceScale(WT);
  const Simulation::OTYPE& Cells=System.getCells();
  Simulation::OTYPE::const_iterator oc;
  for(oc=Cells.begin();oc!=Cells.end();oc++)
    if(!oc->second->getImp())
      WF->maskCell(oc->first);

  WF->maskCell(1);
  return;
}

void
setWeightsHighE(Simulation& System)
  /*!
    Function to set up the weights system.
    It replaces the old file read system.
    \param System :: Simulation component
  */
{
  ELog::RegMethod RegA("TMRCreate","setWeightsHighE");
  std::vector<double> Eval(6);
  Eval[0]=1e-4;
  Eval[1]=0.01;
  Eval[2]=1;
  Eval[3]=10;
  Eval[4]=100;
  Eval[5]=5e7;

  std::vector<double> WT(6);
  WT[0]=0.002;WT[1]=0.007;WT[2]=0.02;
  WT[3]=0.07;WT[4]= 0.2;WT[5]=0.7;
  setWeights(System,Eval,WT);

  return;
}

void
makeNGroove(Simulation& System)
 /*!
  Generate angular cut and look of the moderator
  \param System :: Simulation main to change
 */
{
  ELog::RegMethod RegA("TMRCreate","makeNGroove");
  const FuncDataBase& FBase=System.getDataBase();
  const int Curves[]=
    {
      11184,11302,
      11284,11902,
      11384,11802,
      11484,11402,
      11584,11502,
      11684,11602,
    };
  const int CurvesB[]=
    {
      11183,11302,
      11283,11902,
      11383,11802,
      11483,11402,
      11583,11502,
      11683,11602 
//      12103,13317 
    };

  const double ddr= FBase.EvalVar<double>("drr");      // angle of moderator
  double angle=FBase.EvalVar<double>("ngvangle");    // angle of groove
  angle=-5;
  // radian rotation
  angle*=M_PI/180.0;
  Geometry::Vec3D Norm(0,-sin(ddr-angle),cos(ddr-angle));
  for(unsigned int i=0;i<sizeof(Curves)/sizeof(int);i+=2)
    {
      const int APlane=Curves[i];
      const int BPlane=Curves[i+1];
      if (!ModelSupport::addPlane(APlane,Norm,APlane,BPlane,11685,-10102))
        {
	  ELog::EM<<"Failed on addPlane : "<<APlane<<ELog::endErr;
	  exit(1);
	}
    }

  Geometry::Vec3D NormB(0,-sin(ddr+angle),cos(ddr+angle));
  for(unsigned int i=0;i<sizeof(CurvesB)/sizeof(int);i+=2)
    {
      const int APlane=CurvesB[i];
      const int BPlane=CurvesB[i+1];
      if (!ModelSupport::addPlane(APlane,NormB,APlane,BPlane,11685,-10102))
        {
	  ELog::EM<<"Failed on addPlane : "<<APlane<<ELog::endErr;
	  exit(1);
	}
    }
  // GROOVE 
  double hangle=FBase.EvalVar<double>("ngvholeangle"); 
  hangle*=-M_PI/180.0;
  Geometry::Vec3D NormC(0,-sin(ddr-hangle),cos(ddr-hangle));
  Geometry::Vec3D NormD(0,-sin(ddr+hangle),cos(ddr+hangle));
  
  if (!ModelSupport::addPlane(12183,NormC,12183,13217,11685,-10102))
    {
      ELog::EM<<"Failed on addPlane : "<<12183<<ELog::endErr;
      exit(1);
    }
  if (!ModelSupport::addPlane(12184,NormD,12184,13217,11685,-10102))
    {
      ELog::EM<<"Failed on addPlane : 12184"<<ELog::endErr;
      exit(1);
    }     
  return;
}

void
makeBGroove(Simulation& System)
 /*!
   Generate angular cut and look of the moderator
  \param System :: Simulation main to change
 */
{
  ELog::RegMethod RegA("TMRCreate","makeBGroove");

  const FuncDataBase& FBase=System.getDataBase();
  const int Curves[]=
    {
      11103,10302,
      11203,12902,
      11303,12802,
      11403,10402,
      11503,10502,
      11603,10602 
//      12103,13317 
    };

  const double ddr=FBase.EvalVar<double>("drr"); 
  double angle=FBase.EvalVar<double>("bgvangle"); 
  // radian rotation
  angle*=M_PI/180.0;
  Geometry::Vec3D Norm(0,-sin(ddr-angle),cos(ddr-angle));
  for(unsigned int i=0;i<sizeof(Curves)/sizeof(int);i+=2)
    {
      const int APlane=Curves[i];
      const int BPlane=Curves[i+1];
      if (!ModelSupport::addPlane(APlane,Norm,APlane,BPlane,11605,10102))
        {
	  ELog::EM<<"Failed on addPlane : "<<APlane<<ELog::endErr;
	  exit(1);
	}

    }
  const int CurvesB[]=
    {
      11104,10302,
      11204,12902,
      11304,12802,
      11404,10402,
      11504,10502,
      11604,10602,
    };

  Geometry::Vec3D NormB(0,-sin(ddr+angle),cos(ddr+angle));
  for(unsigned int i=0;i<sizeof(CurvesB)/sizeof(int);i+=2)
    {
      const int APlane=CurvesB[i];
      const int BPlane=CurvesB[i+1];
      if (!ModelSupport::addPlane(APlane,NormB,APlane,BPlane,11605,10102))
        {
	  ELog::EM<<"Failed on addPlane : "<<APlane<<ELog::endErr;
	  exit(1);
	}
    }
  // GROOVE 
  double hangle=FBase.EvalVar<double>("bgvholeangle"); 
  hangle*=M_PI/180.0;
  Geometry::Vec3D NormC(0,-sin(ddr-hangle),cos(ddr-hangle));
  Geometry::Vec3D NormD(0,-sin(ddr+hangle),cos(ddr+hangle));
  
  if (!ModelSupport::addPlane(12103,NormC,12103,13317,11605,10102))
    {
      ELog::EM<<"Failed on addPlane : "<<12103<<ELog::endErr;
      exit(1);
    }
  ModelSupport::addPlane(12104,NormD,12104,13317,11605,10102);
     
  return;
}

void
addCorregation(Simulation& System)
  /*!
    Adds corregation to cell 22502 of the poisoned moderator
    \param System :: Simuation compoenent
    \todo FINISH: 
  */
{
  FuncDataBase& Control=System.getDataBase();
  const int ntubes=static_cast<int>(Control.EvalVar<double>("ntube"));
  
  if (ntubes<=0)
    return;

  // now get curve for 10501

  const double tubeR=Control.EvalVar<double>("tubeRadius");
  Control.Parse("alradius-(dalheat+vacgap+dalpress+midvacgap+daltern)"); 
  const double CR=Control.Eval();
  const double CentY=Control.EvalVar<double>("bcentre");
  Control.Parse("dalpress+vacgap+dalheat+daltern+midvacgap+gdlayer-totdcwidth");
  const double Lx=Control.Eval();
  Control.Parse("totdcwidth-(gdlayer+dalheat+vacgap+dalpress+daltern+midvacgap)");
  const double Rx=Control.Eval();
  
  // Now new items must fall on the centre line: 
  if (CR && tubeR && CentY && Lx && Rx)
    return;

  return;
}

void
setVirtualContainers(Simulation& System)
  /*!
    Create virtual contains within the system
    \param System :: Simulation model
  */
{
  ELog::RegMethod RegA("TMRCreate","setVirtualContainers");

  System.addCell(MonteCarlo::Qhull(78000,0,0.0,"184 -185 186 -187 -188 -189 -190 -191 -192 193"));
  System.makeVirtual(78000);        // Container for the methane moderator
  return;
}


}  // NAMESPACE shutterSystem
