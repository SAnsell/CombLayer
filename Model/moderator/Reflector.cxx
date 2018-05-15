/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   moderator/Reflector.cxx
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
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "FrontBackCut.h"
#include "TargetBase.h"
#include "TS2target.h"
#include "TS2ModifyTarget.h"
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

#include "insertObject.h"
#include "insertPlate.h"

namespace moderatorSystem
{

Reflector::Reflector(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedOffset(Key,10),
  refIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(refIndex+1),
  TarObj(new TMRSystem::TS2target("t2Target")),
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

  OR.addObject(TarObj);
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
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  refIndex(A.refIndex),cellIndex(A.cellIndex),
  xySize(A.xySize),zSize(A.zSize),cutSize(A.cutSize),
  defMat(A.defMat),
  TarObj(new TMRSystem::TS2target(*A.TarObj)),
  GrooveObj(new Groove(*A.GrooveObj)),
  HydObj(new Hydrogen(*A.HydObj)),
  VacObj(new VacVessel(*A.VacObj)),
  FLgroove(new FlightLine(*A.FLgroove)),
  FLhydro(new FlightLine(*A.FLhydro)),
  PMgroove(new PreMod(*A.PMgroove)),
  PMhydro(new PreMod(*A.PMhydro)),
  Horn(new HWrapper(*A.Horn)),
  //  DMod((A.DMod) ? std::shared_ptr<Decoupled>(A.DMod->clone()) : A.DMod),
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
      xySize=A.xySize;
      zSize=A.zSize;
      cutSize=A.cutSize;
      defMat=A.defMat;
      *TarObj=*A.TarObj;
      *GrooveObj = *A.GrooveObj;
      *HydObj = *A.HydObj;
      *VacObj = *A.VacObj;
      *FLgroove = *A.FLgroove;
      *FLhydro = *A.FLhydro;
      *PMgroove = *A.PMgroove;
      *PMhydro = *A.PMhydro;
      *Horn = *A.Horn;
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
{}

void
Reflector::populate(const FuncDataBase& Control)
/*!
  Populate all the variables
  \param Control :: Data base for variables
*/
{
  ELog::RegMethod RegA("Reflector","populate");
  
  FixedOffset::populate(Control);
  
  xySize=Control.EvalVar<double>(keyName+"XYSize");
  zSize=Control.EvalVar<double>(keyName+"ZSize");
  cutSize=Control.EvalVar<double>(keyName+"CutSize");
  cornerAngle=Control.EvalVar<double>(keyName+"CornerAngle");
  
  defMat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  const size_t nPads=Control.EvalVar<size_t>(keyName+"NPads");
  for(size_t i=0;i<nPads;i++)
    Pads.push_back(CoolPad("coolPad",i+1));

  return;
}
  
void
Reflector::createUnitVector(const attachSystem::FixedComp& FC,
			    const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed Comp 
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("Reflector","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
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
    Geometry::Quaternion::calcQRotDeg(cornerAngle,Z);
  Geometry::Vec3D XR(X);
  Geometry::Vec3D YR(Y);
  Qxy.rotate(XR);
  Qxy.rotate(YR);
  
  // Simple box planes

  ModelSupport::buildPlane(SMap,refIndex+1,Origin-Y*xySize,Y);
  ModelSupport::buildPlane(SMap,refIndex+2,Origin+Y*xySize,Y);

  ModelSupport::buildPlane(SMap,refIndex+3,Origin-X*xySize,X);
  ModelSupport::buildPlane(SMap,refIndex+4,Origin+X*xySize,X);

  ModelSupport::buildPlane(SMap,refIndex+5,Origin-Z*zSize,Z);
  ModelSupport::buildPlane(SMap,refIndex+6,Origin+Z*zSize,Z);
 
  // Corner cuts:
  ModelSupport::buildPlane(SMap,refIndex+11,Origin-YR*cutSize,YR);
  ModelSupport::buildPlane(SMap,refIndex+12,Origin+YR*cutSize,YR);

  ModelSupport::buildPlane(SMap,refIndex+13,Origin-XR*cutSize,XR);
  ModelSupport::buildPlane(SMap,refIndex+14,Origin+XR*cutSize,XR);

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

  FixedComp::setConnect(0,Origin-Y*xySize,-Y);  // chipIR OPPOSITE
  FixedComp::setConnect(1,Origin+Y*xySize,Y);   // chipIR
  FixedComp::setConnect(2,Origin-X*xySize,-X);
  FixedComp::setConnect(3,Origin+X*xySize,X);
  FixedComp::setConnect(4,Origin-Z*zSize,-Z);
  FixedComp::setConnect(5,Origin+Z*zSize,Z);

  FixedComp::setConnect(6,Origin-YR*cutSize,-YR);
  FixedComp::setConnect(7,Origin+YR*cutSize,YR);
  FixedComp::setConnect(8,Origin-XR*cutSize,-XR);
  FixedComp::setConnect(9,Origin+XR*cutSize,XR);

  FixedComp::setLinkSurf(0,-SMap.realSurf(refIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(refIndex+2));
  FixedComp::setLinkSurf(2,-SMap.realSurf(refIndex+3));
  FixedComp::setLinkSurf(3,SMap.realSurf(refIndex+4));
  FixedComp::setLinkSurf(4,-SMap.realSurf(refIndex+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(refIndex+6));
  FixedComp::setLinkSurf(6,-SMap.realSurf(refIndex+11));
  FixedComp::setLinkSurf(7,SMap.realSurf(refIndex+12));
  FixedComp::setLinkSurf(8,-SMap.realSurf(refIndex+13));
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

  for(CoolPad& PD : Pads)
    PD.addInsertCell(74123);
  
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
      DFPtr->createAllFromFile(System,*this,0,
          IParam.getValue<std::string>("decFile"));
      DMod=std::shared_ptr<Decoupled>(DFPtr);  
      OR.addObject(DMod);
      return;
    }

  const std::string DT=IParam.getValue<std::string>("decType");
  if (DT=="standard")  // Standard one
    {
      std::shared_ptr<Decoupled> DModPtr(new Decoupled("decoupled"));
      OR.addObject(DModPtr);
      DModPtr->createAll(System,World::masterTS2Origin(),0);
      DMod=DModPtr;
    }
  else if (DT=="layer")  // layer
    {
      std::shared_ptr<Decoupled> DModPtr(new DecLayer("decoupled","decLayer")); 
      OR.addObject(DModPtr);
      DModPtr->createAll(System,World::masterTS2Origin(),0);
      DMod=DModPtr;
    }
  else if (DT=="plate")  // layer
    {
      std::shared_ptr<insertSystem::insertPlate>
	PPtr(new insertSystem::insertPlate("decPlate")); 
       OR.addObject(PPtr);
       PPtr->setNoInsert();
       PPtr->addInsertCell(cellIndex-1);
       PPtr->createAll(System,World::masterTS2Origin(),0);
       DMod=PPtr;
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
Reflector::createInternalObjects(Simulation& System,
				 const mainSystem::inputParam& IParam)
  /*!
    Build the inner objects
    \param System :: Simulation to use
    \param IParam :: Input paramters for decoupled type
  */
{
  ELog::RegMethod RegA("Reflector","createInternalObjects");
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  const std::string TarName=
    IParam.getValue<std::string>("targetType",0);
  const std::string DT=IParam.getValue<std::string>("decType");

  TarObj->setRefPlates(-SMap.realSurf(refIndex+12),
		       -SMap.realSurf(refIndex+11));
  TarObj->createAll(System,World::masterTS2Origin());


  if (TarName=="tMoly")
    {
      std::shared_ptr<TMRSystem::TS2ModifyTarget> TarObjModify
	(new TMRSystem::TS2ModifyTarget("tMoly"));
      TarObjModify->createAll(System,*TarObj);
      OR.addObject(TarObjModify);
    }

  TarObj->addProtonLineInsertCell(cellIndex-1);
  TarObj->addProtonLine(System,*this,-7);

  GrooveObj->createAll(System,World::masterTS2Origin());
  HydObj->createAll(System,*GrooveObj,0);
  if (IParam.flag("orthoH"))
    {
      OrthoInsert OI("ortho");
      OI.createAll(System,*HydObj,*GrooveObj);
    }
  VacObj->createAllPair(System,*GrooveObj,*HydObj);
  std::string Out;
  Out=ModelSupport::getComposite(SMap,refIndex,"-14 -2 -4");
  FLgroove->addBoundarySurf("inner",Out);  
  FLgroove->addBoundarySurf("outer",Out);  
  FLgroove->createAll(System,*VacObj,1);
  
  Out=ModelSupport::getComposite(SMap,refIndex,"1 13");
  FLhydro->addBoundarySurf("inner",Out);  
  FLhydro->addBoundarySurf("outer",Out);  
  FLhydro->createAll(System,*VacObj,2);

  PMgroove->setTargetSurf(TarObj->getLinkSurf(1));
  PMgroove->setDivideSurf(VacObj->getDivideSurf());
  PMgroove->setEdge();
  PMgroove->createAll(System,*VacObj,6,0);

  PMhydro->setTargetSurf(TarObj->getLinkSurf(1));
  PMhydro->setDivideSurf(-VacObj->getDivideSurf());
  PMhydro->setEdge();
  PMhydro->createAll(System,*VacObj,6,1);

  Horn->setDivideSurf(-VacObj->getDivideSurf());
  Horn->createAll(System,*VacObj,*FLhydro,*PMhydro);
  
  processDecoupled(System,IParam);
  const attachSystem::ContainedComp* CMod=
    OR.getObjectThrow<attachSystem::ContainedComp>
    (DMod->getKeyName(),"DMod to CC failed");

  if (DT!="plate")
    {
      DVacObj->createAll(System,*DMod,*CMod);

      Out=ModelSupport::getComposite(SMap,refIndex,"-2 13 3");
      FLnarrow->addBoundarySurf("inner",Out);  
      FLnarrow->addBoundarySurf("outer",Out);  
      FLnarrow->createAll(System,*DVacObj,1);
      
      Out=ModelSupport::getComposite(SMap,refIndex,"11 -4 -14");
      FLwish->addBoundarySurf("inner",Out);  
      FLwish->addBoundarySurf("outer",Out);
      FLwish->createAll(System,*DVacObj,2);
      
      PMdec->setTargetSurf(TarObj->getLinkSurf(1));
      PMdec->createAll(System,*DVacObj,5,1);
    }
  else
    {
      Out=ModelSupport::getComposite(SMap,refIndex,"-2 13 3");
      FLnarrow->addBoundarySurf("inner",Out);  
      FLnarrow->addBoundarySurf("outer",Out);  
      FLnarrow->createAll(System,*DMod,1);
      
      Out=ModelSupport::getComposite(SMap,refIndex,"11 1 -14");
      FLwish->addBoundarySurf("inner",Out);  
      FLwish->addBoundarySurf("outer",Out);  
      FLwish->createAll(System,*DMod,2);
      
      PMdec->setTargetSurf(TarObj->getLinkSurf(1));
      PMdec->createAll(System,*DMod,6,1);
    }  
  Out=ModelSupport::getComposite(SMap,refIndex," 3 ");
  IRcut->addBoundarySurf(Out);  
  IRcut->createAll(System,*TarObj);
  
  CdBucket->addBoundarySurf(FLwish->getExclude("outer"));
  CdBucket->addBoundarySurf(FLnarrow->getExclude("outer"));
  CdBucket->addBoundarySurf(TarObj->getExclude());
  CdBucket->createAll(System,*this,0);

  for(CoolPad& PD : Pads)
    PD.createAll(System,*this,3);
      
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
  return;
  CouplePipe CP("cplPipe");
  System.createObjSurfMap();
  CP.createAll(System,*HydObj,4,*VacObj);

  DecouplePipe DP("decPipe");
  //  DP.createAll(System,*DMod,5,*DVacObj,DMod->needsHePipe());

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
      return DVacObj->getLinkSurf(2);
    }
  if (BeamLine<9)      // H2
    {
      FLhydro->getInnerVec(Window);
      return VacObj->getLinkSurf(2);
    }
  if (BeamLine<14)      // Groove
    {
      FLgroove->getInnerVec(Window);
      dSurf=GrooveObj->getDividePlane();
      return VacObj->getLinkSurf(1);
    }
  // WISH
      FLwish->getInnerVec(Window);
      //      dSurf=DMod->getDividePlane(0);
      return DVacObj->getLinkSurf(2);
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
    throw ColErr::IndexError<int>(BeamLine,18,"BeamLine");
  
  if (BeamLine<4)       // NARROW
    {
      return DVacObj->getSurfacePoint(0,1);
    }
  if (BeamLine<9)      // H2
    {
      return VacObj->getSurfacePoint(0,2);
    }
  if (BeamLine<14)      // Groove
    {
      // Need to get correct Z:
      return GrooveObj->getViewPoint();
    }
  // WISH
  return DVacObj->getSurfacePoint(0,2);
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
  for(const CoolPad& PD : Pads)
    Out+=PD.getExclude();
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
  populate(System.getDataBase());

  createUnitVector(World::masterTS2Origin(),0);

  createSurfaces();
  createObjects(System);
  createInternalObjects(System,IParam);
  insertObjects(System);              

  return;
}

}  // NAMESPACE shutterSystem
