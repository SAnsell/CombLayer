/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/BeamMonitor.cxx
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
#include "surfEqual.h"
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
#include "SimProcess.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"

#include "BeamMonitor.h"

namespace essSystem
{

BeamMonitor::BeamMonitor(const std::string& Key) :
  attachSystem::ContainedComp(),attachSystem::FixedOffset(Key,3),
  monIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(monIndex+1)
  /*!
    Constructor
    \param Key :: Keyname for system
  */
{}

BeamMonitor::BeamMonitor(const BeamMonitor& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  monIndex(A.monIndex),cellIndex(A.cellIndex),nSec(A.nSec),
  radius(A.radius),thick(A.thick),mat(A.mat),halfThick(A.halfThick)
  /*!
    Copy constructor
    \param A :: BeamMonitor to copy
  */
{}

BeamMonitor&
BeamMonitor::operator=(const BeamMonitor& A)
  /*!
    Assignment operator
    \param A :: BeamMonitor to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      cellIndex=A.cellIndex;
      nSec=A.nSec;
      radius=A.radius;
      thick=A.thick;
      mat=A.mat;
      halfThick=A.halfThick;
    }
  return *this;
}

BeamMonitor::~BeamMonitor()
  /*!
    Destructor
   */
{}

void
BeamMonitor::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("BeamMonitor","populate");

  FixedOffset::populate(Control);

  nSec=Control.EvalVar<size_t>(keyName+"BoxNSections");   
  double RW(0.0);
  double TW(0.0);
  int MW;

  for(size_t i=0;i<nSec;i++)
    {
      RW=Control.EvalDefVar<double>
	(StrFunc::makeString(keyName+"BoxRadius",i+1),-1.0);   
      TW=Control.EvalDefVar<double>
	(StrFunc::makeString(keyName+"BoxThick",i+1),-1.0);     
      MW=ModelSupport::EvalDefMat<int>
	(Control,StrFunc::makeString(keyName+"BoxMat",i+1),0);   
      if (RW<0.0 || TW<0.0)
	{
	  if (i<nSec/2)
	    throw ColErr::InContainerError<std::string>
	      (StrFunc::makeString(keyName+"BoxRadius/Thick",i+1),"Search");   
	  
	  const size_t index=nSec-(i+1);
	  RW=radius[index];
	  TW=thick[index];
	  MW=mat[index];
	}
      radius.push_back(RW);
      thick.push_back(TW);
      mat.push_back(MW);
    }
  return;
}

void
BeamMonitor::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int linkIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
    \param linkIndex :: Link index position
  */
{
  ELog::RegMethod RegA("BeamMonitor","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,linkIndex);
  applyOffset();
  return;
}

void
BeamMonitor::createSurfaces()
  /*!
    Create beamline surface
  */
{
  ELog::RegMethod RegA("BeamMonitor","createSurfaces");

  halfThick=std::accumulate(thick.begin(),thick.end(),
			    0.0,std::plus<double>())/2.0;

  int BM(monIndex);
  double T(-halfThick);
  for(size_t i=0;i<nSec;i++)
    {
      ModelSupport::buildPlane(SMap,BM+1,Origin+Y*T,Y);  
      ModelSupport::buildCylinder(SMap,BM+7,Origin,Y,radius[i]);  
      ELog::EM<<"I == "<<i<<" "<<T<<" "<<thick[i]<<" "
	      <<Origin+Y*T<<ELog::endTrace;
      T+=thick[i];
      
      BM+=10;
    }
  // Back plane
  ModelSupport::buildPlane(SMap,BM+1,Origin+Y*T,Y);  
  return; 
}

std::string
BeamMonitor::calcExclude(const size_t index,
			 const attachSystem::ContainedGroup& CG, 
			 const std::string& CName) const
  /*!
    Calculate if the ContainedGroup needs to be excluded from the 
    object
    \param index :: Index value
    \param CG :: contained group
    \param CName :: Contained Name
    \return Exclude string
  */
{
  ELog::RegMethod RegA("BeamMonitor","calcExclude");
  
  const double totalThick=
    std::accumulate(thick.begin(),thick.begin()+
		    static_cast<std::ptrdiff_t>(index),
		    0.0,std::plus<double>());

  const Geometry::Vec3D PtA(Origin+Y*(totalThick-halfThick));
  const Geometry::Vec3D PtB(Origin+Y*(totalThick-halfThick+thick[index]));

  std::string Out;
  for(size_t i=0;i<CG.nGroups();i++)
    {
      const std::string CKey=CName+StrFunc::makeString(i);
      if (CG.hasKey(CKey))
	{
	  const ContainedComp& CA=CG.getCC(CKey);
	  if (CA.isOuterLine(PtA,PtB))
	    {
	      ELog::EM<<"Found["<<index<<"] "<<i<<ELog::endTrace;
	      Out+=CA.getExclude();
	    }
	}
    }
  ELog::EM<<"Pts = "<<PtA<<" :: "<<PtB<<ELog::endTrace;
  ELog::EM<<"Exclude["<<index<<"] == "<<Out<<ELog::endTrace;
      
  return Out;
}


void
BeamMonitor::createObjects(Simulation& System,
			   const attachSystem::ContainedGroup& CG, 
			   const std::string& CName)
  /*!
    Adds the components
    \param System :: Simulation to create objects in
    \param CG :: Contained Group
    \param CName :: Contained Segment name      
  */
{
  ELog::RegMethod RegA("BeamMonitor","createObjects");
    
  std::string Out;
  int BM(monIndex);
  for(size_t i=0;i<nSec;i++)
    {
      Out=ModelSupport::getComposite(SMap,BM,"1 -11 -7 ");      
      addOuterSurf(Out);
      const std::string Exclude=
	calcExclude(i,CG,CName);

      System.addCell(MonteCarlo::Qhull(cellIndex++,mat[i],0.0,Out+Exclude));
      BM+=10;
    }
  return;
}


void
BeamMonitor::createLinks()
  /*!
    Creates a full attachment set
  */
{
  
  return;
}

void
BeamMonitor::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int linkIndex,
		       const attachSystem::ContainedGroup& CG,
		       const std::string& CGName)
  /*!
    Global creation of the monitor
    \param System :: Simulation to add vessel to
    \param FC :: Proton beam FixedComp for origin
    \param linkIndex :: link point on FC
    \param CG :: Contained group unit
    \param CGName :: Contained Group sequence name
  */
{
  ELog::RegMethod RegA("BeamMonitor","createAll");
  populate(System.getDataBase());

  std::string PBeam;

  createUnitVector(FC,linkIndex);
  createSurfaces();
  createObjects(System,CG,CGName);
  createLinks();
  insertObjects(System); 
  return;
}

}  // NAMESPACE essSystem
