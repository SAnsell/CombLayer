/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   moderator/DecouplePipe.cxx
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
#include "Quadratic.h"
#include "Plane.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "SimProcess.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "VacVessel.h"
#include "Decoupled.h"
#include "pipeUnit.h"
#include "PipeLine.h"
#include "DecouplePipe.h"


namespace moderatorSystem
{

DecouplePipe::DecouplePipe(const std::string& Key)  :
  attachSystem::FixedComp(Key,0),
  pipeIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(pipeIndex+1),populated(0),
  Outer("dOuter"),HeIn("HeIn"),HeOut("HeOut"),CH4In("CH4In"),
  CH4Out("CH4Out")
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

DecouplePipe::DecouplePipe(const DecouplePipe& A) : 
  attachSystem::FixedComp(A),
  pipeIndex(A.pipeIndex),cellIndex(A.cellIndex),
  populated(A.populated),Outer(A.Outer),HeIn(A.HeIn),HeOut(A.HeOut),
  CH4In(A.CH4In),CH4Out(A.CH4Out),Xoffset(A.Xoffset),Yoffset(A.Yoffset),
  HeInXStep(A.HeInXStep),HeInYStep(A.HeInYStep),HeOutXStep(A.HeOutXStep),
  HeOutYStep(A.HeOutYStep),outMat(A.outMat),outAlMat(A.outAlMat),
  outVacMat(A.outVacMat),innerAlMat(A.innerAlMat),innerMat(A.innerMat),
  heMat(A.heMat),heAlMat(A.heAlMat),outRadius(A.outRadius),
  outAlRadius(A.outAlRadius),outVacRadius(A.outVacRadius),
  innerAlRadius(A.innerAlRadius),innerRadius(A.innerRadius),
  heRadius(A.heRadius),heAlRadius(A.heAlRadius),fullLen(A.fullLen)
  /*!
    Copy constructor
    \param A :: DecouplePipe to copy
  */
{}

DecouplePipe&
DecouplePipe::operator=(const DecouplePipe& A)
  /*!
    Assignment operator
    \param A :: DecouplePipe to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      populated=A.populated;
      Outer=A.Outer;
      HeIn=A.HeIn;
      HeOut=A.HeOut;
      CH4In=A.CH4In;
      CH4Out=A.CH4Out;
      Xoffset=A.Xoffset;
      Yoffset=A.Yoffset;
      HeInXStep=A.HeInXStep;
      HeInYStep=A.HeInYStep;
      HeOutXStep=A.HeOutXStep;
      HeOutYStep=A.HeOutYStep;
      outMat=A.outMat;
      outAlMat=A.outAlMat;
      outVacMat=A.outVacMat;
      innerAlMat=A.innerAlMat;
      innerMat=A.innerMat;
      heMat=A.heMat;
      heAlMat=A.heAlMat;
      outRadius=A.outRadius;
      outAlRadius=A.outAlRadius;
      outVacRadius=A.outVacRadius;
      innerAlRadius=A.innerAlRadius;
      innerRadius=A.innerRadius;
      heRadius=A.heRadius;
      heAlRadius=A.heAlRadius;
      fullLen=A.fullLen;
    }
  return *this;
}

DecouplePipe::~DecouplePipe() 
  /*!
    Destructor
  */
{}

void
DecouplePipe::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase to use
  */
{
  ELog::RegMethod RegA("DecouplePipe","populate");
  
  Xoffset=Control.EvalVar<double>(keyName+"XOffset"); 
  Yoffset=Control.EvalVar<double>(keyName+"YOffset"); 
  fullLen=Control.EvalVar<double>(keyName+"FullLen");

  HeInXStep=Control.EvalVar<double>(keyName+"HeInXStep"); 
  HeInYStep=Control.EvalVar<double>(keyName+"HeInYStep"); 

  HeOutXStep=Control.EvalVar<double>(keyName+"HeOutXStep"); 
  HeOutYStep=Control.EvalVar<double>(keyName+"HeOutYStep"); 
 
  outRadius=Control.EvalVar<double>(keyName+"OutRad"); 
  outAlRadius=Control.EvalVar<double>(keyName+"OutAlRad"); 
  outVacRadius=Control.EvalVar<double>(keyName+"OutVacRad"); 
  innerAlRadius=Control.EvalVar<double>(keyName+"InnerAlRad"); 
  innerRadius=Control.EvalVar<double>(keyName+"InnerRad"); 
  heRadius=Control.EvalVar<double>(keyName+"HeRad"); 
  heAlRadius=Control.EvalVar<double>(keyName+"HeAlRad"); 

  HeTrack=SimProcess::getVarVec<Geometry::Vec3D>(Control,keyName+"HeTrack");
  if (HeTrack.empty())
    ELog::EM<<"He Track empty : "<<ELog::endErr;

  outMat=ModelSupport::EvalMat<int>(Control,keyName+"OutMat"); 
  outAlMat=ModelSupport::EvalMat<int>(Control,keyName+"OutAlMat"); 
  outVacMat=ModelSupport::EvalMat<int>(Control,keyName+"OutVacMat"); 
  innerAlMat=ModelSupport::EvalMat<int>(Control,keyName+"InnerAlMat"); 
  innerMat=ModelSupport::EvalMat<int>(Control,keyName+"InnerMat"); 
  heMat=ModelSupport::EvalMat<int>(Control,keyName+"HeMat"); 
  heAlMat=ModelSupport::EvalMat<int>(Control,keyName+"HeAlMat"); 
  
  populated |= 1;
  return;
}
  
void
DecouplePipe::createUnitVector(const attachSystem::FixedComp& CUnit,
			       const long int sideIndex)
  /*!
    Create the unit vectors
    - Y Points towards WISH
    - X Across the moderator
    - Z up (towards the target)
    \param CUnit :: Fixed unit that it is connected to 
    \param sideIndex :: Connection point to use as origin
  */
{
  ELog::RegMethod RegA("DecouplePipe","createUnitVector");

  FixedComp::createUnitVector(CUnit,sideIndex);
  return;
}

void 
DecouplePipe::insertOuter(Simulation& System,const VacVessel& VC)
  /*!
    Add a pipe to the hydrogen system:
    \remark This should be called after the void vessel has
    been constructed and all objects inserted.
    \param System :: Simulation to add pipe to
    \param VC :: Vacuum vessel of decoupled
  */
{
  ELog::RegMethod RegA("DecouplePipe","insertOuter");

  const long int dircNum(6);  //top

  // Inner Points
  Outer.addPoint(VC.getSurfacePoint(0,dircNum)+X*Xoffset+Y*Yoffset);
  Outer.addPoint(VC.getSurfacePoint(2,dircNum)+X*Xoffset+Y*Yoffset);
  Outer.addPoint(VC.getSurfacePoint(3,dircNum)+X*Xoffset+Y*Yoffset);
  Outer.addPoint(VC.getSurfacePoint(4,dircNum)+X*Xoffset+Y*Yoffset);
  Outer.addPoint(VC.getSurfacePoint(4,dircNum)+X*Xoffset+Y*Yoffset+Z*fullLen);
  Outer.setActive(0,3);
  Outer.setActive(1,7);
  
  Outer.addRadius(innerRadius,innerMat,0.0);
  Outer.addRadius(innerAlRadius,innerAlMat,0.0);
  Outer.addRadius(outVacRadius,outVacMat,0.0);
  Outer.addRadius(outAlRadius,outAlMat,0.0);
  Outer.addRadius(outRadius,outMat,0.0);
   
  Outer.createAll(System);
  return;
}

void 
DecouplePipe::insertHePipe(Simulation& System,const VacVessel& VC)
  /*!
    Add a pipe to the hydrogen system:
    \remark This should be called after the void vessel has
    been constructed and all objects inserted.
    \param System :: Simulation to add pipe to
    \param VC :: Vacuum vessel of decoupled
  */
{
  ELog::RegMethod RegA("DecouplePipe","insertHePipe");
  const long int dircNum(6);

  const Geometry::Vec3D APt(VC.getSurfacePoint(0,dircNum)+
			    X*(Xoffset+HeInXStep)+Y*(Yoffset+HeInYStep));
  const Geometry::Vec3D BPt(VC.getSurfacePoint(0,dircNum)+
			    X*(Xoffset+HeOutXStep)+Y*(Yoffset+HeOutYStep));
  // Outer Points
  HeIn.addPoint(VC.getSurfacePoint(4,dircNum)+
		X*(Xoffset+HeInXStep)+Y*(Yoffset+HeInYStep)+Z*fullLen);
  HeIn.addPoint(APt);

  std::vector<Geometry::Vec3D>::const_iterator vc;
  for(vc=HeTrack.begin();vc!=HeTrack.end();vc++)
    {
      const Geometry::Vec3D& tPt(*vc);
      HeIn.addPoint(APt+X*tPt[0]+Y*tPt[1]+Z*tPt[2]);
    }
  // Outer Points
  HeIn.addPoint(BPt);
  HeIn.addPoint(VC.getSurfacePoint(4,dircNum)+
   		X*(Xoffset+HeOutXStep)+Y*(Yoffset+HeOutYStep)+Z*fullLen);

  HeIn.addRadius(heRadius,heMat,0.0);
  HeIn.addRadius(heAlRadius,heAlMat,0.0); 

  HeIn.createAll(System);
  return;
}

void
DecouplePipe::insertPipes(Simulation& System)
  /*!
    Add a pipe to the master system system:
    \remark This should be called after the void vessel has
    been constructed and all objects inserted.
    \param System :: Simulation to add pipe to
  */
{
  ELog::RegMethod RegA("DecouplePipe","insertPipe");

  // First job is to re-create the OSM
  
  System.createObjSurfMap();

  return;
}

  
void
DecouplePipe::createAll(Simulation& System,
			const attachSystem::FixedComp& FUnit,
			const long int sideIndex,
			const VacVessel& VCell,
			const int flag)
  /*!
    Generic function to create everything
    \param System :: Simulation to create objects in
    \param FUnit :: Fixed Base unit
    \param sideIndex :: FixedComp side
    \param VCell :: Decoupled Vac Vessel
    \param flag :: decide if to add the He Pipe
  */
{
  ELog::RegMethod RegA("DecouplePipe","createAll");


  // First job is to re-create the OSM and populate cells
  populate(System.getDataBase());
  createUnitVector(FUnit,sideIndex);
  insertOuter(System,VCell); 

  if (flag)
    insertHePipe(System,VCell);

  //  insertPipes(System);       
  
  return;
}
  
}  // NAMESPACE moderatorSystem
