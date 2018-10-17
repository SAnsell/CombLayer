/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   src/Simulation.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include <cmath>
#include <complex> 
#include <vector>
#include <list> 
#include <map> 
#include <set>
#include <string>
#include <algorithm>
#include <functional>
#include <numeric>
#include <iterator>
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
#include "mathSupport.h"
#include "support.h"
#include "writeSupport.h"
#include "version.h"
#include "Element.h"
#include "Zaid.h"
#include "MapSupport.h"
#include "MXcards.h"
#include "Material.h"
#include "DBMaterial.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Triple.h"
#include "NList.h"
#include "NRange.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Transform.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfEqual.h"
#include "Quadratic.h"
#include "surfaceFactory.h"
#include "objectRegister.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FItem.h"
#include "FuncDataBase.h"
#include "SurInter.h"
#include "BnId.h"
#include "AcompTools.h"
#include "Acomp.h"
#include "Algebra.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "WForm.h"
#include "weightManager.h"
#include "ModeCard.h"
#include "PhysCard.h"
#include "LSwitchCard.h"
#include "PhysImp.h"
#include "Source.h"
#include "inputSupport.h"
#include "SourceBase.h"
#include "sourceDataBase.h"
#include "ObjSurfMap.h"
#include "PhysicsCards.h"
#include "ReadFunctions.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SimTrack.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"

Simulation::Simulation()  :
  OSMPtr(new ModelSupport::ObjSurfMap),
  cellDNF(0),cellCNF(0)
  /*!
    Start of simulation Object
  */
{
  ModelSupport::SimTrack::Instance().addSim(this);
}

Simulation::Simulation(const Simulation& A) :
  objectGroups(A),
  inputFile(A.inputFile),
  cmdLine(A.cmdLine),DB(A.DB),
  OSMPtr(new ModelSupport::ObjSurfMap(*A.OSMPtr)),
  TList(A.TList),cellDNF(A.cellDNF),cellCNF(A.cellCNF),
  cellOutOrder(A.cellOutOrder),
  voidCells(A.voidCells),sourceName(A.sourceName)
  /*!
    Copy constructor
    \param A :: Simulation to copy
  */
{
  ELog::EM<<"CALLING INCORRECT Simulation(Simulation)"<<ELog::endCrit;
}

Simulation&
Simulation::operator=(const Simulation& A)
  /*!
    Assignment operator
    \param A :: Simulation to copy
    \return *this
  */
{
  if (this!=&A)
    {
      ELog::EM<<"CALLING INCORRECT Simulation::operator="<<ELog::endCrit;
      inputFile=A.inputFile;
      cmdLine=A.cmdLine;
      DB=A.DB;
      TList=A.TList;
      cellDNF=A.cellDNF;
      cellCNF=A.cellCNF;
      OList=A.OList;
      cellOutOrder=A.cellOutOrder;
      voidCells=A.voidCells;
      sourceName=A.sourceName;
    }
  return *this;
}


Simulation::~Simulation()
  /*!
    Destructor :: deletes the memory in the Tally
    list
  */
{
  ELog::RegMethod RegA("Simulation","delete operator");

  delete OSMPtr;
  deleteObjects();
  ModelSupport::SimTrack::Instance().clearSim(this);
  
}

void
Simulation::resetAll()
  /*!
    The big reset
  */
{
  ModelSupport::surfIndex::Instance().reset();
  TList.erase(TList.begin(),TList.end());
  OSMPtr->clearAll();
  deleteObjects();
  cellOutOrder.clear();
  masterRotate& MR = masterRotate::Instance();
  MR.clearGlobal();
  return;
}

void
Simulation::deleteObjects()
  /*!
    Delete all the Objects 
  */
{
  ELog::RegMethod RegA("Simulation","deleteObjects");
  
  ModelSupport::SimTrack::Instance().setCell(this,0);
  for(OTYPE::value_type& mc : OList)
    delete mc.second;
  
  OList.erase(OList.begin(),OList.end());
  cellOutOrder.clear();
  return;
}


int
Simulation::checkInsert(const MonteCarlo::Qhull& A)
  /*!
    When a new Qhull object is to be inserted this
    checks to see if it can be done, returns 1 on 
    success and 0 on failure 
    \param A :: Hull to insert in the main list
    \returns 1 on success and 0 on overwrite
  */
{
  ELog::RegMethod RegA("Simulation","checkInsert");

  const int cellNumber=A.getName();
  OTYPE::iterator mpt=OList.find(cellNumber);
  
  if (mpt!=OList.end())
    {
      ELog::EM<<"Over-writing Object ::"<<cellNumber<<ELog::endWarn;
      (*mpt->second)=A;
      return 0;
    }
  OList.insert(OTYPE::value_type(cellNumber,A.clone()));
  return 1;
}

int
Simulation::getNextCell(int CN) const
  /*!
    Get next cell
    \param CN :: Cell number to start offset from
    \return next free offset
  */
{
  while (OList.find(CN)!=OList.end())
    {
      CN++;
    }
  return CN;
}

int
Simulation::addCell(const MonteCarlo::Qhull& A)
  /*!
    Adds a cell the the simulation.
    \todo Can the checkCell be removed??
    \param A :: New cell
    \return 1 on success and 0 on failure
  */
{
  return addCell(A.getName(),A);
}

int
Simulation::addCell(const int cellNumber,const MonteCarlo::Qhull& A)
  /*!
    Adds a cell the the simulation.
    \param cellNumber :: Cell Id
    \param A :: New cell
    \return 1 on success and 0 on failure
  */
{
  ELog::RegMethod RegA("Simulation","addCell(int,Qhull)");

  OTYPE::iterator mpt=OList.find(cellNumber);
  
  // Adding existing cell:
  if (mpt!=OList.end())
    {
      ELog::EM<<"Cell Exists Object ::"<<cellNumber<<std::endl;
      throw ColErr::ExitAbort("Cell number in use");
    }
  OList.insert(OTYPE::value_type(cellNumber,A.clone()));
  MonteCarlo::Qhull* QHptr=OList[cellNumber];

  QHptr->setName(cellNumber);

   if (setMaterialDensity(cellNumber))
    {
      ELog::EM<<"No material in found:"<<cellNumber<<ELog::endCrit;
      throw ColErr::InContainerError<int>(cellNumber,"cellNumber");
    }


  if (!QHptr->hasComplement() ||
      removeComplement(*QHptr))
    {
      QHptr->createSurfaceList();
    }
  else
    {
      ELog::EM<<"Cell has no object map"<<QHptr->getName()<<ELog::endCrit;
      ELog::EM<<"Cell==:"<<*QHptr<<ELog::endCrit;
      ELog::EM<<"Cell==:"<<QHptr->hasComplement()<<ELog::endCrit;
    }


  QHptr->setFCUnit(objectGroups::addActiveCell(cellNumber));
  
  return 1;
}

int
Simulation::addCell(const int Index,const int MatNum,
		    const std::string& RuleLine)
  /*!
    From the information try to add a new cell
    \param Index :: Identifier of cell
    \param MatNum :: Material number 
    \param RuleLine :: List of rules
    \retval -1 :: failure [no new cell etc]
    \retval 0 :: success
   */
{
  ELog::RegMethod RegA("Simulation","addCell(int,int,string)");
  return addCell(Index,MatNum,0.0,RuleLine);
}

int
Simulation::addCell(const int Index,const int matNum,
		    const double matTemp,
		    const std::string& RuleLine)
  /*!
    From the information try to add a new cell
    \param Index :: Identifier of cell
    \param matNum :: Material number 
    \param matTemp :: Material temperature
    \param RuleLine :: List of rules
    \retval -1 :: failure [no new cell etc]
    \retval 0 :: success
   */
{
  ELog::RegMethod RegA("Simulation","addCell(int,int,double,string)");
  
  MonteCarlo::Qhull TX;
  TX.setName(Index);
  TX.setMaterial(matNum);
  TX.setTemp(matTemp);  
  TX.procString(RuleLine);         // This always is successful. 
  return addCell(Index,TX);
}

int
Simulation::addCell(const int Index,const int matNum,
		    const double matTemp,
		    const HeadRule& RuleItem)
  /*!
    From the information try to add a new cell
    \param Index :: Identifier of cell
    \param matNum :: Material number 
    \param matTemp :: Material temperature
    \param RuleItem :: List of rules
    \retval -1 :: failure [no new cell etc]
    \retval 0 :: success
   */
{
  ELog::RegMethod RegA("Simulation","addCell(int,int,double,HeadRule)");
  
  MonteCarlo::Qhull TX;
  TX.setName(Index);
  TX.setMaterial(matNum);
  TX.setTemp(matTemp);  
  TX.procHeadRule(RuleItem);

  return addCell(Index,TX);
}

 
void
Simulation::setENDF7()
  /*!
    Converts the zaids into ENDF7 format
  */
{
  ModelSupport::DBMaterial::Instance().setENDF7();
  return;
}


int
Simulation::removeDeadSurfaces(const int placeFlag)
  /*!
    Object is to remove all un-used surfaces
    \param placeFlag :: Exclude placeholder cells
    \return number of surfaces removed
  */
{
  // Find a set of all active surfaces:
  ELog::RegMethod RegA("Simulation","removeDeadSurface");
  const ModelSupport::surfIndex& SI=ModelSupport::surfIndex::Instance();
  const ModelSupport::surfIndex::STYPE& SurMap =SI.surMap();

  populateCells();
  removeNullSurfaces();
  std::set<int> SFound;
  OTYPE::iterator mc;
  for(mc=OList.begin();mc!=OList.end();mc++)
    {
      if (!placeFlag || !mc->second->isPlaceHold())
	{
	  const std::vector<int> MX=(mc->second)->getSurfaceIndex();
	  copy(MX.begin(),MX.end(),
	       std::insert_iterator<std::set<int> >(SFound,SFound.begin()));
	}
    }
  // KEEP Surfaces:
  const std::vector<int> keepVec=SI.keepVector();
  copy(keepVec.begin(),keepVec.end(),
       std::insert_iterator<std::set<int> >(SFound,SFound.begin()));
  
  // Create elimination list:
  std::vector<int> Dead;
        
  std::map<int,Geometry::Surface*>::const_iterator sc;
  for(sc=SurMap.begin();sc!=SurMap.end();sc++)
    {
      if (SFound.find(sc->first)==SFound.end())
	Dead.push_back(sc->first);
    }

  // Dead:  
  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();  
  for(const int DSurf : Dead)
    SurI.deleteSurface(DSurf);
  
  return 0;
}

void
Simulation::removeCell(const int cellNumber)
  /*!
    Removes the cell [Must exist]
    \param cellNumber :: cell to remove
   */
{
  ELog::RegMethod RegItem("Simulation","removeCell");

  OTYPE::iterator vc=OList.find(cellNumber);
  if (vc==OList.end())
    throw ColErr::InContainerError<int>(cellNumber,"cellNumber in OList");

  OSMPtr->removeObject(vc->second);
  
  ModelSupport::SimTrack& ST(ModelSupport::SimTrack::Instance());
  ST.checkDelete(this,vc->second);
  delete vc->second;

  OList.erase(vc);

  objectGroups::removeActiveCell(cellNumber);

  return;
}

int 
Simulation::removeAllSurface(const int KeyN)
  /*!
    Remove all the surface of name KeyN
    from the simulation. Then remove
    surface KeyN from the surface map.
    \param KeyN :: Surface number to remove
    \return Number of surface removed (will do)
  */
{
  ELog::RegMethod RegA("Simulation","removeAllSurface");
  
  for(OTYPE::value_type& om : OList)
    {
      om.second->removeSurface(KeyN);
      om.second->populate();
      om.second->createSurfaceList();
    }
  ModelSupport::surfIndex::Instance().deleteSurface(KeyN);
  return 0;
}

void
Simulation::substituteAllSurface(const int oldSurfN,const int newSurfN)
  /*!
    Remove all the surface of name oldSurfN
    from the simulation. Then remove
    surface KeyN from the surface map.
    \param oldSurfN :: Number of surface to alter
    \param newSurfN :: Number of new surface + sign to swap KeyN sign.
    \throw IncontainerError if the key does not exist
  */
{
  ELog::RegMethod RegA("Simulation","substituteAllSurface");
  
  SDef::sourceDataBase& SDB=SDef::sourceDataBase::Instance();

  // SurfaceIndex and surfaces already updated
  const int NS(newSurfN>0 ? newSurfN : -newSurfN);
  Geometry::Surface* XPtr=ModelSupport::surfIndex::Instance().getSurf(NS);
  if (!XPtr)
    throw ColErr::InContainerError<int>(newSurfN,"Surface number not found");

  OTYPE::iterator oc;
  for(oc=OList.begin();oc!=OList.end();oc++)
    oc->second->substituteSurf(oldSurfN,newSurfN,XPtr);

  // Source:
  if (!sourceName.empty())
    {
      SDef::SourceBase* SPtr=
	SDB.getSourceThrow<SDef::SourceBase>(sourceName,"Source not known");
      SPtr->substituteSurface(oldSurfN,newSurfN);
    }
  
  return;
}


int
Simulation::setMaterialDensity(OTYPE& ObjGroup)
  /*!
    Sets the density of the material in the cells
    \param ObjGroup :: Object stack to set materials
    \retval 0 :: success
    \retval -1 :: failure
   */
{
  ELog::RegMethod RegA("Simulation","setMaterialDensity");
  OTYPE::iterator vc;

  ModelSupport::DBMaterial& DB=ModelSupport::DBMaterial::Instance();  
  std::map<int,MonteCarlo::Material>::const_iterator mc;

  for(vc=ObjGroup.begin();vc!=ObjGroup.end();vc++)
    {
      const int matN=vc->second->getMat();
      if (matN!=0)
        {
	  const MonteCarlo::Material& MC=DB.getMaterial(matN);
	  vc->second->setDensity(MC.getAtomDensity());
	}
    }
  return 0;
}

int
Simulation::setMaterialDensity(const int cellNum)
  /*!
    Sets the density of the material for cellNum
    \param cellNum :: cell nubmer to get proper density
    \throw InContainer on material List: Object material mismatch
    \retval 0 :: success
   */
{
  ELog::RegMethod RegA("Simulation","setMaterialDensity(int)");

  OTYPE ObjSet;
  OTYPE::iterator vc;
  std::map<int,MonteCarlo::Material>::const_iterator mc;
  vc=OList.find(cellNum);
  if (vc==OList.end())
    return -1;
  
  // Passes a single object to the objset.
  ObjSet.insert(*vc);
  setMaterialDensity(ObjSet);
  return 0;
}

std::vector<std::pair<int,int>>
Simulation::getCellImp() const
  /*!
    Now process Physics so importance/volume cards can be set.
    \return pair of cellNumber : importance in cell 
  */
{
  ELog::RegMethod RegA("Simulation","getCellImp");

  std::vector<std::pair<int,int>> cellImp;

  for(const OTYPE::value_type& VC: OList)
    {
      if (!VC.second->isPlaceHold())
	{
	  cellImp.push_back
	    (std::pair<int,int>(VC.first,(VC.second->getImp()>0) ? 1 : 0));
	}
    }
  return cellImp;
}

int
Simulation::removeComplement(MonteCarlo::Qhull& OB) const
  /*!
    Remove a complement form an object
    \param OB :: object to extract complement
    \return 0 :: success -1 failure ?    
  */
{
  ELog::RegMethod RegA("Simulation","removeComplement");

  MonteCarlo::Algebra AX;

  AX.setFunctionObjStr(OB.cellStr(OList));

  return OB.procString(AX.writeMCNPX());
}


int
Simulation::removeComplements()
  /*!
    Expand each complement on a tree.
    \retval 0 on success, 
    \retval -1 failed to find surface key
  */
{
  ELog::RegMethod RegA("Simulation","removeComplements");

  populateCells();
  int retVal(0);
  OTYPE::iterator vc;
  for(vc=OList.begin();vc!=OList.end();vc++)
    {
      MonteCarlo::Qhull& workObj= *(vc->second);

      if (workObj.hasComplement())
        {  
	  if (workObj.isPopulated())
	    {
	      MonteCarlo::Algebra AX;
	      AX.setFunctionObjStr(workObj.cellStr(OList));
	      if (!workObj.procString(AX.writeMCNPX()))
		throw ColErr::InvalidLine(AX.writeMCNPX(),
					  "Algebra Complement");

	      workObj.populate();
	      workObj.createSurfaceList();
	    }
	  else 
	    {
	      ELog::EM<<"Skipping "<<vc->first<<" "
		      <<workObj.isPopulated()<<ELog::endErr;
	      retVal=-1;
	    }
	}
    }
  return retVal;
}

int
Simulation::removeNullSurfaces()
  /*!
    Remove all the surface of type NullSurface
    from the simulation. Then remove
    surface KeyN from the surface map.
    \returns Number of surface removed (will do)
  */
{
  
  ModelSupport::surfIndex& SI=ModelSupport::surfIndex::Instance();
  const ModelSupport::surfIndex::STYPE& SurMap=SI.surMap();
  std::map<int,Geometry::Surface*>::const_iterator sf;
  std::vector<int> dead;
  for(sf=SurMap.begin();sf!=SurMap.end();sf++)
    {
      if (sf->second->isNull())
        {
	  const int keyN=sf->first;
	  OTYPE::iterator oc;
	  for(oc=OList.begin();oc!=OList.end();oc++)
	    {
	      MonteCarlo::Qhull& workObj= *(oc->second);
	      workObj.removeSurface(keyN);
	    }
	  dead.push_back(keyN);
	}
    }
  
  for(const int DSurf : dead)
    SI.deleteSurface(DSurf);

  return 0;
}

int 
Simulation::populateCells()
  /*!
    Place a surface* with each keyN in the cell list 
    Generate the Qhull map.
    \retval 0 on success, 
    \retval -1 failed to find surface key
  */
{
  ELog::RegMethod RegA("Simulation","populateCells");
  
  OTYPE::iterator oc;

  int retVal(0);
  for(oc=OList.begin();oc!=OList.end();oc++)
    {
      MonteCarlo::Qhull& workObj= *(oc->second);
      try
        {
	  workObj.populate();
	  workObj.createSurfaceList();
	}
      catch (ColErr::InContainerError<int>& A)
        {
	  ELog::EM<<"Cell "<<workObj.getName()<<" failed on surface :"
		  <<A.getItem()<<ELog::endCrit;
	  throw;
	}
    }
  return -retVal;
}

int 
Simulation::populateCells(const std::vector<int>& cellVec)
  /*!
    Place a surface* with each keyN in the cell list 
    Generate the Qhull map.
    \param cellVec :: Cells to consider
    \retval 0 on success, 
    \retval -1 failed to find surface key
  */
{
  ELog::RegMethod RegA("Simulation","populateCells");
  
  OTYPE::iterator oc;

  for(const int CN : cellVec)
    {
      oc=OList.find(CN);
      if (oc==OList.end()) return -1;
      MonteCarlo::Qhull& workObj= *(oc->second);
      try
        {
	  workObj.populate();
	  workObj.createSurfaceList();
	}
      catch (ColErr::InContainerError<int>& A)
        {
	  ELog::EM<<"Cell "<<workObj.getName()<<" failed on surface :"
		  <<A.getItem()<<ELog::endCrit;
	  throw;
	}
    }
  return 0;
}

int
Simulation::applyTransforms()
  /*! 
    Applys the transforms to each surfaces 
    \retval 1 :: cant find a transform
    \retval 0 :: success
  */
{
  
  ELog::RegMethod RegA("Simulation","applyTransforms");
  const ModelSupport::surfIndex::STYPE& SurMap =
    ModelSupport::surfIndex::Instance().surMap();
  std::map<int,Geometry::Surface*>::const_iterator sm;
  for(sm=SurMap.begin();sm!=SurMap.end();sm++)
    {
      if (sm->second->applyTransform(TList)<0)
        {
	  ELog::EM<<"Failed on "<<sm->first<<ELog::endErr;
	  return 1;
	}
    }
  return 0;
}

MonteCarlo::Qhull*
Simulation::findQhull(const int CellN)
  /*!
    Helper function to determine the hull object 
    given a particular cell number
    \param CellN : Number of the Object to find
    \returns Qhull pointer to the object
  */
{
  ELog::RegMethod RegA("Simulation","findQhull");
  OTYPE::iterator mp=OList.find(CellN);
  return (mp==OList.end()) ? 0 : mp->second;
}

const MonteCarlo::Qhull*
Simulation::findQhull(const int CellN) const
  /*! 
    Helper function to determine the hull object 
    given a particulat cell number (const varient)
    \param CellN :: Cell number 
    \return Qhull pointer 
  */
{
  ELog::RegMethod RegA("Simulation","findQhull const");

  OTYPE::const_iterator mp=OList.find(CellN);
  return (mp==OList.end()) ? 0 : mp->second;
}


int
Simulation::calcVertex(const int CellN)
  /*! 
     Calculates the vertexes in the Cell and stores
     in the Qhull. The number of vertexes found are returned. 
     \param CellN :: Cell object number
     \returns CellN if valid, 0 if failed
  */
{
  ELog::RegMethod RegA("Simulation","calcVertex");
  MonteCarlo::Qhull* QH=findQhull(CellN);
  if (!QH) return 0;

  QH->calcVertex();
  return CellN;
}

void
Simulation::calcAllVertex()
  /*! 
     Calculates the vertexes in the Cell and stores
     in the Qhull. The number of vertexes found are returned. 
  */
{
  ELog::RegMethod RegA("Simulation","calcAllVertex");


  OTYPE::iterator mc;

  for(mc=OList.begin();mc!=OList.end();mc++)
    {
      // This point may be outside of the point
      if (!mc->second->calcVertex())   
	mc->second->calcMidVertex();
    }
  return;
}

void
Simulation::updateSurface(const int SN,const std::string& SLine)
  /*!
    Update a surface that has already been assigned:
    \param SN :: Surface number
    \param SLine :: Surface string
   */
{
  ELog::RegMethod RegA("Simulation","updateSurface");

  ModelSupport::surfIndex& SurI=ModelSupport::surfIndex::Instance();
  if (SurI.getSurf(SN))
    SurI.deleteSurface(SN);
  SurI.createSurface(SN,0,SLine);
  //
  OTYPE::iterator oc;
  for(oc=OList.begin();oc!=OList.end();oc++)
    {
      if (oc->second->hasSurface(SN))
	{
	  oc->second->rePopulate();
	}
    }
  return;
}

void
Simulation::addObjSurfMap(MonteCarlo::Qhull* QPtr)
  /*! 
    Add an object surface mappings.
    \param QPtr :: Qhull object
  */
{
  ELog::RegMethod RegA("Simulation","addObjSurfMap");
  if (QPtr->getSurfSet().empty())
    QPtr->createSurfaceList();
  
  OSMPtr->addSurfaces(QPtr);
  QPtr->setObjSurfValid();
  return;
}

void
Simulation::validateObjSurfMap()
  /*!
    Given a group of cells process the ObjSurfMap 
  */
{
  ELog::RegMethod RegA("Simulation","validateObjSurfMap");
  OTYPE::iterator mc;

  for(mc=OList.begin();mc!=OList.end();mc++)
    {
      MonteCarlo::Object* objPtr(mc->second);
      if (!objPtr->isObjSurfValid() || !objPtr->isPopulated())
	{
	  objPtr->createSurfaceList();
	  OSMPtr->addSurfaces(objPtr);
	  objPtr->setObjSurfValid();
	}
    }
  return;
} 

void
Simulation::createObjSurfMap()
  /*! 
    Creates all the object surface mappings.
  */
{
  ELog::RegMethod RegA("Simulation","createObjSurfMap");

  OSMPtr->clearAll();

  OTYPE::iterator mc;
  for(mc=OList.begin();mc!=OList.end();mc++)
    {
      if (mc->second->getSurfSet().empty())
	mc->second->createSurfaceList();
      // First add surface that are opposite 
      OSMPtr->addSurfaces(mc->second);
    }  
  return;
}

const ModelSupport::ObjSurfMap* 
Simulation::getOSM() const
  /*!
    Get the Object surface map : 
    Causes an error to be thrown if OSM is not set
    \return OSMPtr
  */
{
  return OSMPtr;
}
  
void
Simulation::printVertex(const int CellN) const
  /*! 
    Print out the appropiate Vertex information
    \param CellN :: Cell number to access
  */
{
  const MonteCarlo::Qhull* QH=findQhull(CellN);
  if (!QH) 
    return;

  QH->printVertex();
  return;
}

void
Simulation::setEnergy(const double) 
  /*!
    Set the cut energy
    \param EMin :: Energy minimum
   */
{
  ELog::RegMethod RegA("Simulation","setEnergy");

  // cut source + weight window (?)
  //  PhysPtr->setEnergyCut(EMin);


  return;
}

void
Simulation::setSourceName(const std::string& S)
 /*!
   Set the source name from the database
   \param S :: Source name
  */
{
  ELog::EM<<"Source == "<<S<<ELog::endDiag;
  sourceName=S;
  return;
}

int
Simulation::existCell(const int cellNumber) const
  /*! 
     Given an object number check that it exist
     \param cellNumber :: number to check
     \returns 1 if item exist and 0 if not (with error message)
  */
{
  ELog::RegMethod RegA("Simulation","existCell");
  const MonteCarlo::Qhull* QH=findQhull(cellNumber);
  if (!QH) return 0;

  if (QH->getName()!=cellNumber)
    {
      ELog::EM<<"Error with findCellID:: cellNumber != mapKey"<<ELog::endErr;
      throw ColErr::ExitAbort(RegA.getFull());
    }
  return 1;
}

int
Simulation::getCellMaterial(const int cellNumber) const
  /*! 
     Given an identification number return 
     cell material
     Returns -1 on failure to find cell, because 
     0 is a valid material (vacuum)
     \param cellNumber :: number to check
     \retval Material number (0 for vacuum)
     \retval -1 if item exist 
  */
  
{
  const MonteCarlo::Qhull* QH=findQhull(cellNumber);
  if (!QH) 
    return -1;

  return QH->getMat();
}

std::pair<const MonteCarlo::Object*,const MonteCarlo::Object*>
Simulation::findCellPair(const int SN) const
  /*!
    Find a cell pair (or 1) that have -/+SN as a valid cell 
    \param SN :: Surface number
    \return Pair Object valid for -SN : Object valid for +SN 
  */
{
  ELog::RegMethod RegA("Simulation","findCellPair(Surf ONLY)");
  
  std::pair<const MonteCarlo::Object*,const MonteCarlo::Object*> Out(0,0);

  const ModelSupport::ObjSurfMap::STYPE& negType=OSMPtr->getObjects(-SN);
  const ModelSupport::ObjSurfMap::STYPE& plusType=OSMPtr->getObjects(SN);


  for(const MonteCarlo::Object* OPtr : negType)
    {
      if (!OPtr->isPlaceHold() &&
	{
	  Out.first=OPtr;
	  break;
	}
    }

  for(const MonteCarlo::Object* OPtr : plusType)
    {
      if (!OPtr->isPlaceHold())
	{
	  Out.second=OPtr;
	  break;
	}
    }
  return Out;
    
}
std::pair<const MonteCarlo::Object*,const MonteCarlo::Object*>
Simulation::findCellPair(const Geometry::Vec3D& Pt,const int SN) const
  /*!
    Find a cell pair (or 1) that have -/+SN as a valid cell 
    \param Pt :: Point to test
    \param SN :: Surface number
    \return Pair Object valid for -SN : Object valid for +SN 
  */
{
  ELog::RegMethod RegA("Simulation","findCellPair");
  
  std::pair<const MonteCarlo::Object*,const MonteCarlo::Object*> Out(0,0);

  const ModelSupport::ObjSurfMap::STYPE& negType=OSMPtr->getObjects(-SN);
  const ModelSupport::ObjSurfMap::STYPE& plusType=OSMPtr->getObjects(SN);


  for(const MonteCarlo::Object* OPtr : negType)
    {
      if (!OPtr->isPlaceHold() &&
	  OPtr->isDirectionValid(Pt,-SN))
	{
	  Out.first=OPtr;
	  break;
	}
    }

  for(const MonteCarlo::Object* OPtr : plusType)
    {
      if (!OPtr->isPlaceHold() &&
	  OPtr->isDirectionValid(Pt,SN))
	{
	  Out.second=OPtr;
	  break;
	}
    }
  return Out;
    
}
				 
int
Simulation::isValidCell(const int cellNumber,
			const Geometry::Vec3D& Pt) const
  /*!
    Check if a point is in the cell
    \param cellNumber :: number of cell to check
    \param Pt :: Point to check
    \retval 0 on if not valid or cell doesn't exist
    \retval 1 if valid
  */
{
  /* Does Oi correspond to an object */
  const MonteCarlo::Qhull* QH=findQhull(cellNumber);
  if (!QH)
    return 0;

  return QH->isValid(Pt);
}

int
Simulation::findCellNumber(const Geometry::Vec3D& Pt,
			   const int cellNumber) const
  /*!
    Find a cell based on Pt and old number
    \param Pt :: Point to find in cell
    \param cellNumber :: First guess search point
    \return cell number / 0
  */
{
  MonteCarlo::Object* Obj(0);
  if (cellNumber)
    {
      OTYPE::const_iterator mpc=OList.find(cellNumber);
      if (mpc!=OList.end())
	Obj=mpc->second;
    }
  Obj=findCell(Pt,Obj);
  return (Obj) ? Obj->getName() : 0;
}

MonteCarlo::Object*
Simulation::findCell(const Geometry::Vec3D& Pt,
		     MonteCarlo::Object* testCell) const
  /*! 
    Object that a given the point is in.
    \param Pt :: Point to find
    \param testCell :: Last Cell (since points often are close together 
    \retval Object ptr
    \retval 0 :: No cell exists
  */
{
  ModelSupport::SimTrack& ST(ModelSupport::SimTrack::Instance());
  // First test users guess:
  if (testCell && testCell->isValid(Pt))
    {
      ST.setCell(this,testCell);
      return testCell;
    }
  // Ok how about our last find
  MonteCarlo::Object* curObjPtr=ST.curCell(this);
  if (curObjPtr && curObjPtr!=testCell 
      && curObjPtr->isValid(Pt))
    return curObjPtr;
      
  // now we need to search everthing
  OTYPE::const_iterator mpc;
  for(mpc=OList.begin();mpc!=OList.end();mpc++)
    {
      if (!mpc->second->isPlaceHold() &&
	  mpc->second->isValid(Pt))
        {
	  ST.setCell(this,mpc->second);
	  return mpc->second;
	}
    }
  // Found NOTHING :-(
  ST.setCell(this,0);
  return 0;
}

void
Simulation::reZeroFromVertex(const int CellN,const unsigned int M,
			     const unsigned int A,const unsigned int B,
			     const unsigned int C,
			     Geometry::Vec3D& Dis,Geometry::Matrix<double>& MR)
  /*! 
     Given points M , A (x) , B (y) ,C (z)  
     rezero the points matrix around the centre for a given Qhull
     The qhull must have had convex hull intersects calculated.

     \param CellN :: Cell ID number
     \param M :: Vertex point in object for zero
     \param A :: Vertex point for x-axis (A-M)
     \param B :: Vertex point for y-axis (B-M)
     \param C :: Vertex point for z-axis (C-M)
     \param Dis :: Displacement vector 
     \param MR :: Rotation matrix that is required
   */
{
  const MonteCarlo::Qhull* QHX=findQhull(CellN);
  if (!QHX) 
    return;

  MR=QHX->getRotation(M,A,B,C);
  Dis=QHX->getDisplace(M);
  return;
}


std::vector<int>
Simulation::getCellTempRange(const double LT,const double HT) const
  /*!
    Ugly function to return the current vector of cells
    which have a given temparature range
    \param LT :: Low temperature [Kelvin]
    \param HT :: High temperature [Kelvin]
    \return vector of cell numbers (ordered)
  */
{
  std::vector<int> cellOrder;
  OTYPE::const_iterator mc;
  for(mc=OList.begin();mc!=OList.end();mc++)
    {
      if (!mc->second->isPlaceHold())
        {
	  const double T=mc->second->getTemp();
	  if (T>LT && T<HT)
	    cellOrder.push_back(mc->first);
	}
    }
  return cellOrder;
}
 
std::vector<int>
Simulation::getCellVector() const
  /*!
    Ugly function to return the current
    vector of cells
    \return vector of cell numbers (ordered)
  */
{
  std::vector<int> cellOrder;
  OTYPE::const_iterator mc;
  for(mc=OList.begin();mc!=OList.end();mc++)
    {
      if (!mc->second->isPlaceHold())
	cellOrder.push_back(mc->first);
    }
  sort(cellOrder.begin(),cellOrder.end());
  return cellOrder;
}

std::vector<int>
Simulation::getCellInputVector() const
  /*!
    Ugly function to return the current
    vector of cells in order they were read.
    \return vector of cell numbers (ordered)
  */
{
  std::vector<int> cellOrder;
  std::vector<int> index;
  OTYPE::const_iterator mc;
  
  for(mc=OList.begin();mc!=OList.end();mc++)
    {
      if (!mc->second->isPlaceHold())
        {
	  cellOrder.push_back(mc->first);
	  index.push_back(mc->second->getCreate());
	}
    }
  mathFunc::crossSort(index,cellOrder);
  return cellOrder;
}

std::vector<int>
Simulation::getCellWithMaterial(const int matN) const
  /*!
    Ugly function to return the current
    vector of cells with a particular material type
    \param matN :: Material number [-1 : all not void / -2 all]
    \return vector of cell numbers (ordered)
    \todo Make this with a transform, not a loop.
  */
{
  ELog::RegMethod RegA("Simulation","getCellWithMaterial");
  
  std::vector<int> cellOrder;

  for(const OTYPE::value_type& cellItem : OList)
    {
      const MonteCarlo::Qhull* QPtr=cellItem.second;
      if (!QPtr->isPlaceHold() &&
          (matN==-2 ||                            // all
          (matN==-1 && QPtr->getMat()) ||         // not void
           (QPtr->getMat()==matN) ) )             // match
        {
          cellOrder.push_back(cellItem.first);
        }
    }
  sort(cellOrder.begin(),cellOrder.end());
  return cellOrder;
}

std::vector<int>
Simulation::getCellWithZaid(const size_t zaidNum) const
  /*!
    Ugly function to return the current
    vector of cells with a particular zaid type
    \param zaidNum :: Material zaid number
    \return vector of cell numbers (ordered)
    \todo Make this with a transform, not a loop.
  */
{
  ELog::RegMethod RegA("Simulation","getCellWithZaid");

  const ModelSupport::DBMaterial& DB=
    ModelSupport::DBMaterial::Instance();

  std::vector<int> cellOrder;
  std::map<int,int> matZaidCache;

  OTYPE::const_iterator mc;
  std::map<int,int>::const_iterator mz;

  for(mc=OList.begin();mc!=OList.end();mc++)
    {
      if (!mc->second->isPlaceHold())
	{
	  const int matN=mc->second->getMat();
	  mz=matZaidCache.find(matN);
	  if (mz==matZaidCache.end())
	    {
	      // does this return a valid iterator?
	      matZaidCache.insert
		(std::map<int,int>::value_type
		 (matN,DB.getMaterial(matN).hasZaid(zaidNum,0,0)));
	      mz=matZaidCache.find(matN);
	    }
	  if (mz->second)
	    cellOrder.push_back(mc->first);
	}
    }

  sort(cellOrder.begin(),cellOrder.end());
  return cellOrder;
}

std::vector<int>
Simulation::getNonVoidCellVector() const
  /*!
    Ugly function to return the current
    vector of cells which are not vacuum.
    \return vector of cell numbers (ordered)
  */
{
  std::vector<int> cellOrder;
  OTYPE::const_iterator mc;
  for(mc=OList.begin();mc!=OList.end();mc++)
    {
      if (mc->second->getMat() && !mc->second->isPlaceHold())
	cellOrder.push_back(mc->first);
    }
  sort(cellOrder.begin(),cellOrder.end());
  return cellOrder;
}

std::vector<int>
Simulation::getCellVectorRange(const int RA,const int RB) const
  /*!
    Ugly function to return the current
    vector of cells which are not vacuum.
    \return vector of cell numbers (ordered)
  */
{
  ELog::RegMethod RegA("Simluation","getCellVecotRange");

  std::vector<int> cellOrder;

  // OTYPE is an ORDER map by number
  OTYPE::const_iterator mc=OList.lower_bound(RA);
  while(mc!=OList.end() && mc->first<RA+RB)
    {
      if (mc->second->getMat() && !mc->second->isPlaceHold())
	cellOrder.push_back(mc->first);
    }
  return cellOrder;
}

std::map<int,int>
Simulation::calcCellRenumber(const std::vector<int>& cOffset,
			     const std::vector<int>& cRange) const
  /*!
    Re-arrange all the cell numbers to be sequentual from 1-N.
    excluding those in the ranges between cOffset and cOffset+cRange.
    Then each cOffset in order is set at 10001, 20001 unless the
    number of normal cells have exceeded 10001 etc and then it is 20001 etc
    \param cOffset :: Protected start
    \param cRange :: Protected range
    \return map of oldCell and new cell nubmers
  */
{
  ELog::RegMethod RegA("Simulation","calcCellRenumber");

  //  const groupRange& zeroRange=objectGroups::getGroup("World");
  // create a protected range unit to test ALL active cells
  groupRange fullRange;
  std::vector<groupRange> partRange;
  for(size_t i=0;i<cOffset.size();i++)
    {
      fullRange.addItem(cOffset[i],cOffset[i]+cRange[i]);
      partRange.push_back(groupRange(cOffset[i],cOffset[i]+cRange[i]));
    }

  std::map<int,int> renumberMap;
  int nNum(1);  // alway present
  
  std::set<int> protectedCell;
  OTYPE::const_iterator vc;  
  for(vc=OList.begin();vc!=OList.end();vc++)
    {
      const int cNum=vc->second->getName();      
      if (!fullRange.valid(cNum))
	{
	  nNum++;
	  renumberMap.emplace(cNum,nNum);
	}
      else
	{
	  protectedCell.insert(cNum);
	}
    }
  
  const int baseCN(10000*(1+(nNum/10000)));
  std::vector<int> offset(partRange.size()+1);  // just in case
  for(size_t i=0;i<offset.size();i++)
    offset[i]=static_cast<int>(i)*10000+baseCN;
  for(const int cNum : protectedCell)
    {
      size_t index;
      for(index=0;index<partRange.size() &&
	    !partRange[index].valid(cNum);index++) ;
      
      renumberMap.emplace(cNum,offset[index]);
      offset[index]++;
      
    }
  return renumberMap;
}


std::map<int,int>
Simulation::renumberCells(const std::vector<int>& cOffset,
			  const std::vector<int>& cRange)
  /*!
    Re-arrange all the cell numbers to be sequentual from 1-N.
    \param cOffset :: Protected start
    \param cRange :: Protected range
    \returns the map for further use if required
  */
{
  ELog::RegMethod RegA("Simulation","renumberCells");

  WeightSystem::weightManager& WM=
    WeightSystem::weightManager::Instance();

  const std::map<int,int> RMap=
    calcCellRenumber(cOffset,cRange);
    
  OTYPE newMap;           // New map with correct numbering
  for(const std::map<int,int>::value_type& RMItem : RMap)
    {
      const int cNum=RMItem.first;
      const int nNum=RMItem.second;
      MonteCarlo::Qhull* oPtr=findQhull(cNum);
      if (oPtr)
	{
	  oPtr->setName(nNum);      
	  newMap.emplace(nNum,oPtr);

	  WM.renumberCell(cNum,nNum);
	  objectGroups::renumberCell(cNum,nNum);
	  ELog::RN<<"Cell Changed :"<<cNum<<" "<<nNum
		  <<" Object:"<<oPtr->getFCUnit()<<ELog::endBasic;
	}

    }    
  OList=newMap;
  return RMap;
}

void
Simulation::renumberAll()
  /*!
    Default renumber
  */
{
  ELog::RegMethod RegA("Simulation","renumberAll");
  std::vector<int> A;
  std::vector<int> B;
  renumberSurfaces(A,B);
  renumberCells(A,B);
  return;
}

void
Simulation::renumberSurfaces(const std::vector<int>& rLow,
			     const std::vector<int>& rHigh)
 /*!
    Re-arrange all the surface numbers to be sequentual from 
    1-N.
    \param rLow :: lowRange number to put into offset
    \param rHigh :: highRange number to put into offset
 */
{
  ELog::RegMethod RegA("Simulation","renumberSurfaces");

  ModelSupport::surfIndex& SI=ModelSupport::surfIndex::Instance();
  std::vector<std::pair<int,int> > ChangeList;
  
  if (SI.calcRenumber(rLow,rHigh,10000,ChangeList))
    {
      std::vector< std::pair<int,int> >::const_iterator dc;
      for(dc=ChangeList.begin();dc!=ChangeList.end();dc++)
	{
	  ELog::RN<<"Surf Change:"<<dc->first<<" "<<dc->second<<ELog::endDiag;
	  SI.renumber(dc->first,dc->second);
	  substituteAllSurface(dc->first,dc->second);		    
	}
    }
  return;
}

void
Simulation::voidObject(const std::string& ObjName)
  /*!
    Given an object set the material void
    \param ObjName :: Object to void
  */
{
  ELog::RegMethod RegA("Simulation","voidObject");

  // full name:
  //   (a) Object:CellMap
  //   (b) Number - Number
  //   (c) All
  //   (d) Object
  const std::vector<int> cellRange=getObjectRange(ObjName);
  for(const int cellN : cellRange)
    {
      MonteCarlo::Qhull* QH=findQhull(cellN);
      if (QH) 
	QH->setMaterial(0);
    }
  return;
}

int
Simulation::splitObject(const int CA,const int SN)
  /*!
    Split a cell into two based on surface. Does not do 
    any optimizatoin of the new cells. Uses the Shannon derivative
    to produce the system
    Note the original surface is in the negative direction
    \param CA :: Cell number
    \param SN :: surface number
    \return new cell number
   */
{
  ELog::RegMethod RegA("Simulation","splitObject");


  MonteCarlo::Object* CPtr = findQhull(CA);
  if (!CPtr)
    throw ColErr::InContainerError<int>(CA,"Cell not found");
  CPtr->populate();
  // get next cell
  const int CB=getNextCell(CA);
  
  // headrules +/- surface
  HeadRule CHead=CPtr->getHeadRule();
  HeadRule DHead(CHead);

  CHead.addIntersection(-SN);
  DHead.addIntersection(SN);

  addCell(CB,CPtr->getMat(),CPtr->getTemp(),DHead);
  CPtr->procHeadRule(CHead);

  MonteCarlo::Object* DPtr = findQhull(CB);  
  CPtr->populate();
  CPtr->createSurfaceList();

  DPtr->populate();
  DPtr->createSurfaceList();

  // get implicates relative to a surface:
  const std::vector<std::pair<int,int>>
    IP=CPtr->getImplicatePairs(std::abs(SN));

  // Now make two cells and replace this cell with A + B

  MonteCarlo::Algebra AX;
  AX.setFunctionObjStr(CHead.display());
  
  AX.addImplicates(IP);
  if (AX.constructShannonDivision(-SN))
    CPtr->procString(AX.writeMCNPX());

  AX.setFunctionObjStr(DHead.display());
  if (AX.constructShannonDivision(SN))
    DPtr->procString(AX.writeMCNPX());
  
  return CB;
}

void
Simulation::minimizeObject(const int CN)
  /*
    Carry out minimization of a cell to remove 
    literals which can be removed due to implicates [e.g. a->b etc]
    due to parallel surfaces
    \param CN :: Cell to minimize
   */
{
  ELog::RegMethod RegA("Simualation","minimizeObject");

  MonteCarlo::Object* CPtr = findQhull(CN);
  if (!CPtr)
    throw ColErr::InContainerError<int>(CN,"Cell not found");
  
  if (!CPtr->isPlaceHold())
    {
      CPtr->populate();
      CPtr->createSurfaceList();
      
      const std::vector<std::pair<int,int>>
	IP=CPtr->getImplicatePairs();
            
      MonteCarlo::Algebra AX;
      AX.setFunctionObjStr(CPtr->cellCompStr());
      AX.addImplicates(IP);
	    
      AX.constructShannonExpansion();

      if (!CPtr->procString(AX.writeMCNPX()))
	throw ColErr::InvalidLine(AX.writeMCNPX(),
				  "Algebra Export");
    }
      
  return;
}
  
void
Simulation::makeObjectsDNForCNF()
   /*!
     Expand the objects into DNF form or CNF form
   */
{
  ELog::RegMethod RegA("Simulation","makeObjectsDNForCNF");

  if (cellCNF || cellDNF)
    {
      size_t cellIndex(0);
      ELog::EM<<"Cells :"<<ELog::endDiag;;
      for(OTYPE::value_type& OC : OList)
	{
	  MonteCarlo::Object* CPtr = OC.second;
	  if (!CPtr->isPlaceHold())
	    {
	      MonteCarlo::Algebra AX;
	      AX.setFunctionObjStr(CPtr->cellCompStr());
	      const size_t NL=AX.countLiterals();
	      if (NL<=cellDNF || NL<=cellCNF)
		{
		  // Note both together possible
		  if (NL<=cellDNF)
		      AX.expandBracket();
		  if (NL<=cellCNF)
		    AX.expandCNFBracket();
		  
		  if (!CPtr->procString(AX.writeMCNPX()))
		    {
		      ELog::EM<<ELog::endDiag;
		      throw ColErr::InvalidLine(AX.writeMCNPX(),
						"Algebra ExpandD/CNFBracket");
		    }
		  const size_t NLX=AX.countLiterals();
		  if (NLX !=NL)
		    {
		      ELog::EM<<CPtr->getName()<<"["<<NL<<","<<NLX<<"] ";
		      cellIndex++;
		      if (!(cellIndex % 8)) ELog::EM<<ELog::endDiag;
		    }
		  
		}
	      else
		{
		  if (cellIndex % 8) ELog::EM<<ELog::endDiag;
		  ELog::EM<<"\nNOT Cell "<<CPtr->getName()
			  <<"["<<NL<<"]"<<ELog::endCrit;
		  ELog::EM<<"\n";
		  cellIndex=0;
		}
	    }
	}
      ELog::EM<<"\n END DNF/CNF "<<ELog::endDiag;
    }
  validateObjSurfMap();
  return;
}

void
Simulation::prepareWrite() 
  /*!
    Carries out preparations that are
    needed to write. Mainly to set the output cell order
  */
{
  ELog::RegMethod RegA("Simulation","prepareWrite");
  
  cellOutOrder.clear();
  voidCells.clear();

  for(const std::pair<int,MonteCarlo::Qhull*>& OVal : OList)
    {
      if (!OVal.second->isPlaceHold())
	{
	  cellOutOrder.push_back(OVal.first);
	  if (!OVal.second->getMat())
	    voidCells.insert(OVal.first);
	}
    }
  return;
}


void
Simulation::masterRotation() 
  /*!
    Generally rotate ALL the surfaces in the system 
  */
{
  ELog::RegMethod RegA("Simulation","masterRotation");


  masterRotate& MR = masterRotate::Instance();
  
  const ModelSupport::surfIndex::STYPE& SurMap=
    ModelSupport::surfIndex::Instance().surMap();

  std::map<int,Geometry::Surface*>::const_iterator sc;
  for(sc=SurMap.begin();sc!=SurMap.end();sc++)
    MR.applyFull(sc->second);

  // Apply to QHull if calculated:
  OTYPE::iterator oc;
  for(oc=OList.begin();oc!=OList.end();oc++)
    MR.applyFull(oc->second);

  objectGroups::rotateMaster();
  
  return;
}

void
Simulation::masterSourceRotation()
  /*!
    Apply master rotations to the physics system
   */
{
  ELog::RegMethod RegA("Simulation","masterPhysicsRotation");

  SDef::sourceDataBase& SDB=SDef::sourceDataBase::Instance();
  const masterRotate& MR = masterRotate::Instance();
  
  // Source:
  if (!sourceName.empty())
    {
      SDef::SourceBase* SPtr=
	SDB.getSourceThrow<SDef::SourceBase>(sourceName,"Source not known");
      SPtr->rotate(MR);
    }
  return;
}

void
Simulation::writeVariables(std::ostream& OX,
			   const char commentChar) const
  /*!
    Write all the variables in standard MCNPX output format
    \param OX :: Output stream
    \param commentChar :: character for comments
  */
{
  ELog::RegMethod RegA("SimMCNP","writeVaraibles");
  OX<<commentChar<<" ---------- VERSION NUMBER ------------------"<<std::endl;
  OX<<commentChar<<"  ===Git: "<<version::Instance().getBuildTag()
    <<" ====== "<<std::endl;
  OX<<commentChar<<"  ========= "<<version::Instance().getIncrement()
    <<" ========== "<<std::endl;
  OX<<commentChar<<" ----------------------------------------------"<<std::endl;
  OX<<commentChar<<" --------------- VARIABLE CARDS ---------------"<<std::endl;
  OX<<commentChar<<" ----------------------------------------------"<<std::endl;
  const varList& Ptr= DB.getVarList();
  varList::varStore::const_iterator vc;
  for(vc=Ptr.begin();vc!=Ptr.end();vc++)
    {
      std::string Val;
      if (vc->second->isActive())
	{
	  vc->second->getValue(Val);
	  OX<<commentChar<<" "<<vc->first<<" "
	    <<Val<<std::endl;
	}
    }  
  return;
}
