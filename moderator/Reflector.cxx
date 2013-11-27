/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   moderator/Reflector.cxx
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
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

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
#include "Cylinder.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "inputParam.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ReadFunctions.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "LinearComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "TargetBase.h"
#include "TS2target.h"
#include "TS2moly.h"
#include "Groove.h"
#include "Hydrogen.h"
#include "OrthoInsert.h"
#include "VacVessel.h"
#include "FlightLine.h"
#include "World.h"
#include "PreMod.h"
#include "HWrapper.h"
#include "Decoupled.h"
#include "DecLayer.h"
#include "DecFileMod.h"
#include "pipeUnit.h"
#include "PipeLine.h"
#include "CouplePipe.h"
#include "DecouplePipe.h"
#include "Bucket.h"
#include "CoolPad.h"
#include "RefCutOut.h"
#include "RefBolts.h"
#include "Reflector.h"

namespace moderatorSystem
{

Reflector::Reflector(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,10),
  refIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(refIndex+1),TarObj(0),
  GrooveObj(new Groove("groove")),
  HydObj(new Hydrogen("hydrogen")),
  VacObj(new VacVessel("cvac")),
  FLgroove(new FlightLine("grooveFlight")),
  FLhydro(new FlightLine("hydroFlight")),
  PMgroove(new PreMod("groovePM")),
  PMhydro(new PreMod("hydroPM")),
  Horn(new HWrapper("hornPM")),
  DVacObj(new VacVessel("dvac")),
  FLwish(new FlightLine("wishFlight")),
  FLnarrow(new FlightLine("narrowFlight")),
  PMdec(new PreMod("decPM")),
  IRcut(new RefCutOut("chipIRCut")),
  CdBucket(new Bucket("cdBucket"))
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OR.addObject(GrooveObj);
  OR.addObject(HydObj);
  OR.addObject(VacObj);
  OR.addObject(FLgroove);
  OR.addObject(FLhydro);
  OR.addObject(PMgroove);
  OR.addObject(PMhydro);
  OR.addObject(Horn);

  OR.addObject(DVacObj);
  OR.addObject(FLwish);
  OR.addObject(FLnarrow);
  OR.addObject(PMdec);
  OR.addObject(IRcut);
  OR.addObject(CdBucket);
}


Reflector::Reflector(const Reflector& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  refIndex(A.refIndex),cellIndex(A.cellIndex),xStep(A.xStep),
  yStep(A.yStep),zStep(A.zStep),xyAngle(A.xyAngle),
  xySize(A.xySize),zSize(A.zSize),cutSize(A.cutSize),
  defMat(A.defMat),TarObj((A.TarObj) ? A.TarObj->clone() : 0),
   GrooveObj(new Groove(*A.GrooveObj)),
  HydObj(new Hydrogen(*A.HydObj)),
  VacObj(new VacVessel(*A.VacObj)),
  FLgroove(new FlightLine(*A.FLgroove)),
  FLhydro(new FlightLine(*A.FLhydro)),
  PMgroove(new PreMod(*A.PMgroove)),
  PMhydro(new PreMod(*A.PMhydro)),
  Horn(new HWrapper(*A.Horn)),
  DMod((A.DMod) ? boost::shared_ptr<Decoupled>(A.DMod->clone()) : A.DMod),
  DVacObj(new VacVessel(*A.DVacObj)),
  FLwish(new FlightLine(*A.FLwish)),
  FLnarrow(new FlightLine(*A.FLnarrow)),
  PMdec(new PreMod(*A.PMdec)),IRcut(new RefCutOut(*A.IRcut)),
  CdBucket(new Bucket(*A.CdBucket)),Pads(A.Pads)
  /*!
    Copy constructor
    \param A :: Reflector to copy
  */
{}

Reflector&
Reflector::operator=(const Reflector& A)
  /*!
    Assignment operator
    \param A :: Reflector to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      xyAngle=A.xyAngle;
      xySize=A.xySize;
      zSize=A.zSize;
      cutSize=A.cutSize;
      defMat=A.defMat;
      *GrooveObj = *A.GrooveObj;
      *HydObj = *A.HydObj;
      *VacObj = *A.VacObj;
      *FLgroove = *A.FLgroove;
      *FLhydro = *A.FLhydro;
      *PMgroove = *A.PMgroove;
      *PMhydro = *A.PMhydro;
      *Horn = *A.Horn;
      if (A.DMod)
	DMod = boost::shared_ptr<Decoupled>(A.DMod->clone());
      else
	DMod=A.DMod;       
      *DVacObj = *A.DVacObj;
      *FLwish = *A.FLwish;
      *FLnarrow = *A.FLnarrow;
      *PMdec = *A.PMdec;
      *IRcut = *A.IRcut;
      *CdBucket = *A.CdBucket;
      Pads=A.Pads;
    }
  return *this;
}

Reflector::~Reflector() 
  /*!
    Destructor
  */
{
  delete TarObj;
}

void
Reflector::populate(const Simulation& System)
/*!
  Populate all the variables
  \param System :: Simulation to use
*/
{
  ELog::RegMethod RegA("Reflector","populate");
  
  const FuncDataBase& Control=System.getDataBase();
  
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");

  xySize=Control.EvalVar<double>(keyName+"XYSize");
  zSize=Control.EvalVar<double>(keyName+"ZSize");
  cutSize=Control.EvalVar<double>(keyName+"CutSize");
  xyAngle=Control.EvalVar<double>(keyName+"XYAngle");
  
  defMat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  const int nPads=Control.EvalVar<int>(keyName+"NPads");
  for(int i=0;i<nPads;i++)
    Pads.push_back(CoolPad("coolPad",i+1));

  return;
}
  
void
Reflector::createUnitVector()
  /*!
    Create the unit vectors
  */
{
  ELog::RegMethod RegA("Reflector","createUnitVector");

  FixedComp::createUnitVector(World::masterTS2Origin());
  Origin+=X*xStep+Y*yStep+Z*zStep;
    
  return;
}
  
void
Reflector::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("Reflector","createSurface");

  // rotation of axis:
  const Geometry::Quaternion Qxy=
    Geometry::Quaternion::calcQRotDeg(xyAngle,Z);
  Geometry::Vec3D XR(X);
  Geometry::Vec3D YR(Y);
  Qxy.rotate(XR);
  Qxy.rotate(YR);
  
  // Simple box planes

  ModelSupport::buildPlane(SMap,refIndex+1,Origin-YR*xySize,YR);
  ModelSupport::buildPlane(SMap,refIndex+2,Origin+YR*xySize,YR);

  ModelSupport::buildPlane(SMap,refIndex+3,Origin-XR*xySize,XR);
  ModelSupport::buildPlane(SMap,refIndex+4,Origin+XR*xySize,XR);

  ModelSupport::buildPlane(SMap,refIndex+5,Origin-Z*zSize,Z);
  ModelSupport::buildPlane(SMap,refIndex+6,Origin+Z*zSize,Z);
 
  // Corner cuts:
  ModelSupport::buildPlane(SMap,refIndex+11,Origin-Y*cutSize,Y);
  ModelSupport::buildPlane(SMap,refIndex+12,Origin+Y*cutSize,Y);

  ModelSupport::buildPlane(SMap,refIndex+13,Origin-X*cutSize,X);
  ModelSupport::buildPlane(SMap,refIndex+14,Origin+X*cutSize,X);

  createLinks(XR,YR);

  return;
}

void
Reflector::createLinks(const Geometry::Vec3D& XR,
		       const Geometry::Vec3D& YR)
  /*!
    Build the links to the primary surfaces
    \param XR :: Size rotation direction
    \param YR :: Size rotation direction
  */
{
  ELog::RegMethod RegA("Reflector","createLinks");

  FixedComp::setConnect(0,Origin-YR*xySize,-YR);  // chipIR OPPOSITE
  FixedComp::setConnect(1,Origin+YR*xySize,YR);   // chipIR

  FixedComp::setConnect(2,Origin-XR*xySize,-XR);
  FixedComp::setConnect(3,Origin+XR*xySize,XR);
  FixedComp::setConnect(4,Origin-Z*zSize,-Z);
  FixedComp::setConnect(5,Origin+Z*zSize,Z);

  FixedComp::setConnect(6,Origin-Y*cutSize,-Y);
  FixedComp::setConnect(7,Origin+Y*cutSize,Y);
  FixedComp::setConnect(8,Origin-X*cutSize,-X);
  FixedComp::setConnect(9,Origin+X*cutSize,X);

  
  FixedComp::setLinkSurf(0,SMap.realSurf(refIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(refIndex+2));
  FixedComp::setLinkSurf(2,SMap.realSurf(refIndex+3));
  FixedComp::setLinkSurf(3,SMap.realSurf(refIndex+4));
  FixedComp::setLinkSurf(4,SMap.realSurf(refIndex+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(refIndex+6));
  FixedComp::setLinkSurf(6,SMap.realSurf(refIndex+11));
  FixedComp::setLinkSurf(7,SMap.realSurf(refIndex+12));
  FixedComp::setLinkSurf(8,SMap.realSurf(refIndex+13));
  FixedComp::setLinkSurf(9,SMap.realSurf(refIndex+14));

  return;
}

void
Reflector::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("Reflector","createObjects");

  std::string Out;
  Out=ModelSupport::getComposite(SMap,refIndex,"1 -2 3 -4 5 -6 11 -12 13 -14");
  addOuterSurf(Out);

  System.addCell(MonteCarlo::Qhull(cellIndex++,defMat,0.0,Out));
  TarObj->addInsertCell(cellIndex-1);

  VacObj->addInsertCell(cellIndex-1);
  FLgroove->addInsertCell("outer",cellIndex-1);
  FLhydro->addInsertCell("outer",cellIndex-1);
  PMgroove->addInsertCell(cellIndex-1);
  PMhydro->addInsertCell(cellIndex-1);
  Horn->addInsertCell(cellIndex-1);

  DVacObj->addInsertCell(cellIndex-1);
  FLwish->addInsertCell("outer",cellIndex-1);
  FLnarrow->addInsertCell("outer",cellIndex-1);
  PMdec->addInsertCell(cellIndex-1);

  IRcut->addInsertCell(cellIndex-1);
  CdBucket->addInsertCell(cellIndex-1);
  // torpedoCell=cellIndex-1;

  std::vector<CoolPad>::iterator vc;
  for(vc=Pads.begin();vc!=Pads.end();vc++)
    vc->addInsertCell(74123);
  
  return;
}

void
Reflector::processDecoupled(Simulation& System,				
			    const mainSystem::inputParam& IParam)
  /*!
    Create the decoupled item and build it
    \param System :: Simulation 
    \param IParam :: Parameters
   */
{
  ELog::RegMethod RegA("Reflector","processDecoupled");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  if (IParam.flag("decFile"))
    {
      // This strange construct :
      DecFileMod* DFPtr=new DecFileMod("decoupled");
      DFPtr->createAllFromFile(System,*this,
	    IParam.getValue<std::string>("decFile"));
      DMod=boost::shared_ptr<Decoupled>(DFPtr);  
      OR.addObject(DMod);
      return;
    }
  const std::string DT=IParam.getValue<std::string>("decType");
  if (DT=="standard")  // Standard one
    {
      DMod=boost::shared_ptr<Decoupled>(new Decoupled("decoupled"));  
      OR.addObject(DMod);
      DMod->createAll(System,World::masterTS2Origin());
    }
  else if (DT=="layer")  // layer
    {
      DMod=boost::shared_ptr<Decoupled>(new DecLayer("decoupled","decLayer"));  
      OR.addObject(DMod);
      DMod->createAll(System,World::masterTS2Origin());
    }
  else 
    {
      ELog::EM<<"Unable to understand type decType required:"
	      <<DT<<ELog::endErr;
      throw ColErr::InContainerError<std::string>(DT,"DT value");
    }
  return;
}

void
Reflector::setTarget(const mainSystem::inputParam& IParam)
  /*!
    Sets the target point based on the parameters in IParam
    \param IParam :: Input for target type
  */
{
  ELog::RegMethod RegA("Reflector","setTarget");

  const std::string TarName=
    IParam.getValue<std::string>("targetType",0);

  if (TarName=="tMoly")
    TarObj=new TMRSystem::TS2moly("tMoly","t2Target");
  else
    TarObj=new TMRSystem::TS2target("t2Target");
  return;
}


void
Reflector::createInternalObjects(Simulation& System,
				 const mainSystem::inputParam& IParam)
  /*!
    Build the inner objects
    \param System :: Simulation to use
    \param IParam :: Input paramters for decoupled type
  */
{
  ELog::RegMethod RegA("Reflector","createInternalObjects");

  TarObj->setRefPlates(-SMap.realSurf(refIndex+12),
		       -SMap.realSurf(refIndex+11));
  TarObj->createAll(System,World::masterTS2Origin());
  // if (MolObj)
  //   {
  //     TarObj->addInnerBoundary(*MolObj);
  //     MolObj->addInsertCell(TarObj->getMainBody());
  //     MolObj->createAll(System,*TarObj);
  //   }
  TarObj->addProtonLineInsertCell(cellIndex-1);
  TarObj->addProtonLine(System,*this,6);

  GrooveObj->createAll(System,World::masterTS2Origin());
  HydObj->createAll(System,*GrooveObj,0);
  if (IParam.flag("orthoH"))
    {
      OrthoInsert OI("ortho");
      OI.createAll(System,*HydObj,*GrooveObj);
    }
  VacObj->createAllPair(System,*GrooveObj,*HydObj);
    
  std::string Out;
  Out=ModelSupport::getComposite(SMap,refIndex,"1 -14 -4");
  FLgroove->addBoundarySurf("inner",Out);  
  FLgroove->addBoundarySurf("outer",Out);  
  FLgroove->createAll(System,0,*VacObj);

  Out=ModelSupport::getComposite(SMap,refIndex,"-2 13 3");
  FLhydro->addBoundarySurf("inner",Out);  
  FLhydro->addBoundarySurf("outer",Out);  
  FLhydro->createAll(System,1,*VacObj);

  PMgroove->setTargetSurf(TarObj->getLinkSurf(0));
  PMgroove->setDivideSurf(VacObj->getDivideSurf());
  PMgroove->setEdge();
  PMgroove->createAll(System,5,*VacObj,0);

  PMhydro->setTargetSurf(TarObj->getLinkSurf(0));
  PMhydro->setDivideSurf(-VacObj->getDivideSurf());
  PMhydro->setEdge();
  PMhydro->createAll(System,5,*VacObj,1);

  Horn->setDivideSurf(-VacObj->getDivideSurf());
  Horn->createAll(System,*VacObj,*FLhydro,*PMhydro);
  
  processDecoupled(System,IParam);
  DVacObj->createAll(System,*DMod,*DMod);

  Out=ModelSupport::getComposite(SMap,refIndex,"-2 13 3");
  FLnarrow->addBoundarySurf("inner",Out);  
  FLnarrow->addBoundarySurf("outer",Out);  
  FLnarrow->createAll(System,0,*DVacObj);

  Out=ModelSupport::getComposite(SMap,refIndex,"11 1 -14");
  FLwish->addBoundarySurf("inner",Out);  
  FLwish->addBoundarySurf("outer",Out);  
  FLwish->createAll(System,1,*DVacObj);

  PMdec->setTargetSurf(TarObj->getLinkSurf(0));
  PMdec->createAll(System,4,*DVacObj,1);

  Out=ModelSupport::getComposite(SMap,refIndex,"-2");
  IRcut->addBoundarySurf(Out);  
  IRcut->createAll(System,*TarObj);

  CdBucket->addBoundarySurf(FLwish->getExclude("outer"));
  CdBucket->addBoundarySurf(FLnarrow->getExclude("outer"));
  CdBucket->addBoundarySurf(TarObj->getExclude());
  CdBucket->createAll(System,*this);

  std::vector<CoolPad>::iterator vc;
  for(vc=Pads.begin();vc!=Pads.end();vc++)
    vc->createAll(System,*this);


  return;
}

void
Reflector::insertPipeObjects(Simulation& System,
			     const mainSystem::inputParam& IParam)
  /*!
    Calls all the objects that own a pipe to be
    inserted
    \param System :: Simulation to use
    \param IParam :: Parameter table
  */
{
  ELog::RegMethod RegA("Reflector","insertPipeObjects");


  CouplePipe CP("cplPipe");
  System.createObjSurfMap();
  CP.createAll(System,*HydObj,4,*VacObj);

  ELog::EM<<"DEBUG RETURN"<<ELog::endDebug;
  return;

  DecouplePipe DP("decPipe");
  DP.createAll(System,*DMod,5,*DVacObj,DMod->needsHePipe());

  if (IParam.flag("bolts"))
    {
      RefBolts RB("refBolt");
      RB.createAll(System,*this);
    }

  return;
}
  
int
Reflector::calcModeratorPlanes(const int BeamLine,
			       std::vector<int>& Window,
			       int& dSurf) const
  /*!
    Given the beamline calculate the defining view and the 
    active surface 
    \param BeamLine :: BeamLine number to use
    \param Window :: Window of surface to use
    \param dSurf :: divide surface
    \return Master Plane
  */
{
  ELog::RegMethod RegA("Reflector","calcModeratorPlanes");

  if (BeamLine<0 || BeamLine>17)
    throw ColErr::IndexError<int>(BeamLine,18," <beamline number> ");
				  
  
  dSurf=0;
  if (BeamLine<4)       // NARROW
    {
      FLnarrow->getInnerVec(Window);
      dSurf=DMod->getDividePlane(1);
      return DVacObj->getLinkSurf(0);
    }
  if (BeamLine<9)      // H2
    {
      FLhydro->getInnerVec(Window);
      dSurf=HydObj->getDividePlane();
      return VacObj->getLinkSurf(1);
    }
  if (BeamLine<14)      // Groove
    {
      FLgroove->getInnerVec(Window);
      dSurf=GrooveObj->getDividePlane();
      return VacObj->getLinkSurf(0);
    }
  // WISH
      FLwish->getInnerVec(Window);
      dSurf=DMod->getDividePlane(0);
      return DVacObj->getLinkSurf(1);
}

Geometry::Vec3D
Reflector::getViewOrigin(const int BeamLine) const
  /*!
    Given the beamline calculate the defining view and the 
    active surface 
    \param BeamLine :: BeamLine number to use
    \return View point
  */
{
  ELog::RegMethod RegA("Reflector","getViewOrigin");

  if (BeamLine<0 || BeamLine>17)
    throw ColErr::IndexError<int>(BeamLine,18,RegA.getBase());
  
  if (BeamLine<4)       // NARROW
    {
      return DVacObj->getSurfacePoint(0,0);
    }
  if (BeamLine<9)      // H2
    {
      return VacObj->getSurfacePoint(1,0);
    }
  if (BeamLine<14)      // Groove
    {
      // Need to get correct Z:
      return GrooveObj->getViewPoint();
    }
  // WISH
  return DVacObj->getSurfacePoint(1,0);
}

std::string
Reflector::getExclude() const 
  /*!
    Virtual function to add the cooling pads
    \return Full Exlcude path
  */
{
  ELog::RegMethod RegA("Reflector","getExclude");

  std::string Out=ContainedComp::getExclude();
  std::vector<CoolPad>::const_iterator vc;
  for(vc=Pads.begin();vc!=Pads.end();vc++)
    Out+=vc->getExclude();
  return Out;
}

void
Reflector::createAll(Simulation& System,
		     const mainSystem::inputParam& IParam)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param IParam :: Parameter table
  */
{
  ELog::RegMethod RegA("Reflector","createAll");
  populate(System);

  createUnitVector();
  if (!TarObj) setTarget(IParam);
  createSurfaces();
  createObjects(System);
  createInternalObjects(System,IParam);
  insertObjects(System);              

  return;
}

}  // NAMESPACE shutterSystem
