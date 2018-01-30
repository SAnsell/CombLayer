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
#include "localRotate.h"
#include "masterRotate.h"
#include "Triple.h"
#include "NList.h"
#include "NRange.h"
#include "pairRange.h"
#include "Tally.h"
#include "cellFluxTally.h"
#include "pointTally.h"
#include "heatTally.h"
#include "tmeshTally.h"
#include "sswTally.h"
#include "tallyFactory.h"
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
#include "SourceBase.h"
#include "sourceDataBase.h"
#include "KCode.h"
#include "ObjSurfMap.h"
#include "PhysicsCards.h"
#include "ReadFunctions.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SimTrack.h"
#include "Simulation.h"

Simulation::Simulation()  :
  mcnpVersion(6),CNum(100000),OSMPtr(new ModelSupport::ObjSurfMap),
  PhysPtr(new physicsSystem::PhysicsCards)
  /*!
    Start of simulation Object
  */
{
  ModelSupport::SimTrack::Instance().addSim(this);
}

Simulation::Simulation(const Simulation& A)  :
  mcnpVersion(A.mcnpVersion),inputFile(A.inputFile),
  CNum(A.CNum),DB(A.DB),
  OSMPtr(new ModelSupport::ObjSurfMap),
  TList(A.TList),  cellOutOrder(A.cellOutOrder),
  PhysPtr(new physicsSystem::PhysicsCards(*A.PhysPtr))
  /*!
    Copy constructor:: makes a deep copy of the point objects
    object including calling the virtual clone on the 
    Surface pointers
    \param A :: object to copy
  */
{
  ModelSupport::SimTrack::Instance().addSim(this);
  // Object
  OTYPE::const_iterator mc;
  for(mc=A.OList.begin();mc!=A.OList.end();mc++)
    {
      OList.insert(std::pair<int,MonteCarlo::Qhull*>
		   (mc->first,(mc->second)->clone()));
    }
  // Tally
  std::map<int,tallySystem::Tally*>::const_iterator tc;
  for(tc=A.TItem.begin();tc!=A.TItem.end();tc++)
    {
      TItem.insert(std::pair<int,tallySystem::Tally*>
		   (tc->first,(tc->second)->clone()));
    }

  createObjSurfMap();

}

Simulation&
Simulation::operator=(const Simulation& A) 
  /*!
    Assignment operator :: 
    \param A :: object to copy
    \return *this
  */
{
  ELog::RegMethod RegA("Simulation","operator=");

  if (this!=&A)
    {
      inputFile=A.inputFile;
      CNum=A.CNum;
      DB=A.DB;
      TList=A.TList;
      cellOutOrder=A.cellOutOrder;
      delete PhysPtr;
      PhysPtr=new physicsSystem::PhysicsCards(*A.PhysPtr);
      deleteObjects();
      deleteTally();
      // Object
      OTYPE::const_iterator mc;
      for(mc=A.OList.begin();mc!=A.OList.end();mc++)
	{
	  OList.insert(std::pair<int,MonteCarlo::Qhull*>
		       (mc->first,(mc->second)->clone()));
	}
      std::map<int,tallySystem::Tally*>::const_iterator tc;
      for(tc=A.TItem.begin();tc!=A.TItem.end();tc++)
        {
	  TItem.insert(std::pair<int,tallySystem::Tally*>
			(tc->first,(tc->second)->clone()));
	}
      ModelSupport::SimTrack::Instance().setCell(this,0);
      createObjSurfMap();
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

  delete PhysPtr;
  delete OSMPtr;
  deleteObjects();
  deleteTally();
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
  PhysPtr->clearAll();
  deleteTally();
  deleteObjects();
  cellOutOrder.clear();
  CNum=0;
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

void
Simulation::deleteTally()
  /*!
    Delete all the Tallies and clear the maps
  */
{
  for(TallyTYPE::value_type& MVal : TItem)
    delete MVal.second;

  TItem.erase(TItem.begin(),TItem.end());
  return;
}


void
Simulation::setMCNPversion(const int V)
  /*!
    Sets the MCNP version number 
    Use 10 for MCNPX
    \param V :: Version
   */
{
  mcnpVersion=V;
  PhysPtr->setMCNPversion(V);
  return;
}


int
Simulation::readTransform(std::istream& IX)
  /*!
    Read the transformation zone .
    Now makes use of the current state of the data base.
    \param IX :: Input stream
    \returns 
      - 0 on failure
      - Number of successful read transforms
  */
{
  ELog::RegMethod RegA("Simulation","readTransform");
  std::string Line;
  std::string Snd;
  int Tcount(0);

  std::string ExtraStr;
  while(IX.good())
    {
      Line += StrFunc::getLine(IX);
      Line=StrFunc::fullBlock(Line);
      if (Line.substr(0,2)=="tr" || Line.substr(0,2)=="TR" ||
	  Line.substr(0,3)=="*tr" || Line.substr(0,3)=="*TR")   //tr line
        {
	  ReadFunc::removeDollarComment(Line);
	  Geometry::Transform Tr;
	  ReadFunc::processDollarString(DB,Line);
	  int etype=Tr.setTransform(Line);
	  if (!etype)
	    {
	      TList[Tr.getName()]=Tr;
	      Tcount++;
	      Line="";
	    }
	  else if (etype<0)
	    {
	      ELog::EM<<"Error with trans : "<<Line<<":"<<Snd<<ELog::endErr;
	      throw ColErr::ExitAbort("eType<0");
	    }
	}
      else if (Line.find("END")!=std::string::npos ||
	       Line.find("CARDS")!=std::string::npos)
        {
	  return Tcount;
	}
      else
        {
	  Line="";
	}
    }
  return Tcount;
}

int
Simulation::readTally(std::istream& OX)
  /*!
    Read the Tally information
    \param OX :: Input stream
    \retval 0 on failure
    \retval Number of successful read tallies
  */
{
  ELog::RegMethod RegA("Simulation","readTally");
  std::string Line;             /// 
  std::string ProcLine;         /// Line to proces
  int currentID(-1000);
  tallySystem::Tally* TX(0);
  std::map<int,tallySystem::Tally*>::iterator vc;
  while(OX.good() && Line.find("END")==std::string::npos)
    {
      Line = StrFunc::getLine(OX);
      const int flag=tallySystem::Tally::firstLine(Line);
      if (!flag)                 // Cont line
        {
	  ProcLine+=Line+" ";
	}
      else if (!ProcLine.empty())        // Real line + process line
        {
	  int id=tallySystem::Tally::getID(ProcLine);
	  // New/old tally (hopefully)
	  if (id>=0 && id!=currentID)
	    {
	      vc=TItem.find(id);
	      if (vc==TItem.end())
	        {
		  TX=tallySystem::tallyFactory::Instance()->createTally(id);
		  TItem[id]=TX;
		}
	      else
		TX=vc->second;

	      currentID=id;
	    }
	  // This avoids the -1 error type
	  if (id==currentID)
	    {
	      if (id<0 || TX->addLine(ProcLine))
	        {
		  ELog::EM<<"Error reading line "<<ProcLine<<ELog::endErr;
		  throw ColErr::ExitAbort("id<0 || addLine fail");
		}
	    }
	  ProcLine="";
	}
      
      if (flag>0)                 // Not a cont line
	ProcLine= Line+" ";
    }
  return static_cast<int>(TItem.size());
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

tallySystem::Tally*
Simulation::getTally(const int Index) const
  /*!
    Gets a tally from the map
    \param Index :: Tally item to search for
    \return Tally pointer ( or 0 on null)
   */
{
  std::map<int,tallySystem::Tally*>::const_iterator vc; 
  vc=TItem.find(Index);
  if (vc!=TItem.end())
    return vc->second;
  return 0;
}

tallySystem::sswTally*
Simulation::getSSWTally() const
  /*!
    Gets the sswTally 
    \return Tally pointer ( or 0 on null)
   */
{
  std::map<int,tallySystem::Tally*>::const_iterator vc; 
  vc=TItem.find(0);
  if (vc!=TItem.end())
    return dynamic_cast<tallySystem::sswTally*>(vc->second);

  return 0;
}

int
Simulation::nextTallyNum(int tNum) const
  /*!
    Quick funciton to get the next tally point
    \param tNum :: initial index
    \return Next free tally number
   */
{
  ELog::RegMethod RegA("Simulation","nextTallyNum");

  while(TItem.find(tNum)!=TItem.end())
    tNum+=10;

  return tNum;
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
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OTYPE::iterator mpt=OList.find(cellNumber);
  
  // Adding existing cell:
  if (mpt!=OList.end())
    {
      ELog::EM<<"Cell Exists Object ::"<<cellNumber<<std::endl;
      ELog::EM<<"Call from: "<<RegA.getBasePtr()->getItem(-1)<<ELog::endCrit;
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
      
  // Add Volume unit [default]:
  PhysPtr->setVolume(cellNumber,1.0);
  OR.addActiveCell(cellNumber);

  // Add surfaces to OSMPtr:
  //  OSMPtr->addSurfaces(QHptr);

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

 
void
Simulation::setENDF7()
  /*!
    Converts the zaids into ENDF7 format
  */
{
  ModelSupport::DBMaterial::Instance().setENDF7();
  return;
}

void
Simulation::readMaster(const std::string& Fname)
  /*! 
    Processes the full  master file which is the
    Modified MCNPX file:
    Required sections:
    - Transforms 
    - Surfaces
    - Cells
    - Materials
    - Weights
    - Physics
    \param Fname :: Master file name (with full path)
  */
  
{
  ELog::RegMethod RegA("Simulation","readMaster");

  std::ifstream IX;
  IX.open(Fname.c_str(),std::ios::in);
  if (!IX)
    {
      ELog::EM<<"No Master file found:"<<std::endl;
      if (Fname.empty())
	ELog::EM<<" No Master file specified"<<ELog::endErr;
      else
	ELog::EM<<" Attempted to load : "<<Fname<<ELog::endErr;
      throw ColErr::ExitAbort(RegA.getFull());
      return;
    }
  inputFile=Fname;
/* Read components and then calculate sections */
  int nT(0);     // Transforms 
  int nS(0);     // Surfaces 
  int nC(0);     // Cells 
  int nM(0);     // Materials
  int nW(0);     // Weights
  int nTa(0);    // Tallys
  int nP(0);     // Physics

  while(IX.good())
    {
      std::string Einfo=StrFunc::getLine(IX);
      if (Einfo.find("TRANSFORM CARDS")!=std::string::npos)
        {
	  ELog::EM.debug("Reading Transforms");
	  nT+=readTransform(IX);
	}
      else if (Einfo.find("SURFACE CARDS")!=std::string::npos)
        {
	  ELog::EM.debug("Reading surfaces");
	  nS+=ReadFunc::readSurfaces(DB,IX,0);
	}
      else if (Einfo.find("CELL CARDS")!=std::string::npos)
        {
	  ELog::EM.debug("Reading cells");
	  nC+=ReadFunc::readCells(DB,IX,0,OList);
	}
      else if (Einfo.find("MATERIAL CARDS")!=std::string::npos)
        {
	  ELog::EM.debug("Reading materials");
	  nM+=ReadFunc::readMaterial(IX);
	}
      else if (Einfo.find("TALLY CARDS")!=std::string::npos)
        {
	  ELog::EM.debug("Reading Tally");
	  nTa+=readTally(IX);
	}
      else if (Einfo.find("PHYSICS CARDS")!=std::string::npos)
        {
	  ELog::EM.debug("Reading Physics");
	  nP+=ReadFunc::readPhysics(DB,IX,PhysPtr);
	}
    }
  RegA.incIndent();
  ELog::EM<<"Number of  Transforms == "<<nT<<ELog::endTrace;
  ELog::EM<<"Number of Surfaces == "<<nS<<ELog::endTrace;
  ELog::EM<<"Number of Cells == "<<nC<<ELog::endTrace;
  ELog::EM<<"Number of Materials == "<<nM<<ELog::endTrace;
  ELog::EM<<"Number of Weights == "<<nW<<ELog::endTrace;
  ELog::EM<<"Number of Tallies == "<<nTa<<ELog::endTrace;
  ELog::EM<<"Number of Physics == "<<nP<<ELog::endTrace;
  RegA.decIndent();
  IX.close();

  if (applyTransforms())
    ELog::EM<<"Error applying transform for surfaces"<<ELog::endErr;
  else if (populateCells())        //error getting surfaces 
    ELog::EM<<"There is an error getting surface for cells"<<ELog::endErr;
  else if (removeComplements())
    ELog::EM<<"Removing complements failed"<<ELog::endErr;
  else if (setMaterialDensity(OList))
    ELog::EM<<"Materials and cells do not balance"<<ELog::endErr;
  else
    {
      ELog::EM<<"Cells are populated "<<ELog::endBasic;
      return;
    }
  throw ColErr::ExBase("Build object error");
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

int
Simulation::removeCells(const int startN,const int endN)
  /*!
    Removes the cells in the range
    \param startN :: First cell to remove
    \param endN :: Last cell to remove [if -1 use max number]
    \return 1 on success  0 on failure
  */
{
  ELog::RegMethod RegItem("Simulation","removeCells");
  ModelSupport::SimTrack& ST(ModelSupport::SimTrack::Instance());
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  // It seems quicker to create a new map and copy
  OTYPE newOList;
  OTYPE::iterator vc;
  for(vc=OList.begin();vc!=OList.end();vc++)
    {
      // Replace if not placeholder / and in range
      if (!vc->second->isPlaceHold() &&
	  (vc->first>=startN && (endN<0 || vc->first<=endN)))
	{
	  ST.checkDelete(this,vc->second);
          PhysPtr->removeCell(vc->first);
          OR.removeActiveCell(vc->first);
	  delete vc->second;
	}
      else
	newOList.insert(OTYPE::value_type(vc->first,vc->second));
    }
  OList=newOList;

// Now process Physics so importance/volume cards can be set
//  processCellsImp();
//  ELog::CellM.processReport(std::cout);
  return 1;
}

void
Simulation::removeCell(const int cellNumber)
  /*!
    Removes the cell [Must exist]
    \param cellNumber :: cell to remove
   */
{
  ELog::RegMethod RegItem("Simulation","removeCell");
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  OTYPE::iterator vc=OList.find(cellNumber);
  if (vc==OList.end())
    throw ColErr::InContainerError<int>(cellNumber,"cellNumber in OList");
  
  ModelSupport::SimTrack& ST(ModelSupport::SimTrack::Instance());
  ST.checkDelete(this,vc->second);
  delete vc->second;
  OList.erase(vc);

  // Add Volume unit [default]:
  PhysPtr->removeCell(cellNumber);
  OR.removeActiveCell(cellNumber);

  return;
}

int 
Simulation::removeAllSurface(const int KeyN)
  /*!
    Remove all the surface of name KeyN
    from the simulation. Then remove
    surface KeyN from the surface map.
    \param KeyN :: Surface number to remove
    \returns Number of surface removed (will do)
  */
{
  OTYPE::iterator oc;
  for(oc=OList.begin();oc!=OList.end();oc++)
    {
      oc->second->removeSurface(KeyN);
      oc->second->populate();
      oc->second->createSurfaceList();
    }
  ModelSupport::surfIndex::Instance().deleteSurface(KeyN);
  return 0;
}

int 
Simulation::substituteAllSurface(const int KeyN,const int NsurfN)
  /*!
    Remove all the surface of name KeyN
    from the simulation. Then remove
    surface KeyN from the surface map.
    \param KeyN :: Number of surface to alter
    \param NsurfN :: Number of new surface + sign to swap KeyN sign.
    \throw IncontainerError if the key does not exist
    \returns Number of surface removed (will do)
  */
{
  ELog::RegMethod RegA("Simulation","substituteAllSurface");
  SDef::sourceDataBase& SDB=SDef::sourceDataBase::Instance();
  
  const int NS(NsurfN>0 ? NsurfN : -NsurfN);
  Geometry::Surface* XPtr=ModelSupport::surfIndex::Instance().getSurf(NS);
  if (!XPtr)
    throw ColErr::InContainerError<int>
      (NsurfN,"Surface number not found");

  OTYPE::iterator oc;
  for(oc=OList.begin();oc!=OList.end();oc++)
    oc->second->substituteSurf(KeyN,NsurfN,XPtr);

  TallyTYPE::iterator tc;
  for(tc=TItem.begin();tc!=TItem.end();tc++)
    tc->second->renumberSurf(KeyN,NsurfN);

  // Source:
  if (!sourceName.empty())
    {
      SDef::SourceBase* SPtr=
	SDB.getSourceThrow<SDef::SourceBase>(sourceName,"Source not known");
      SPtr->substituteSurface(KeyN,NsurfN);
    }

  
  return 0;
}

int
Simulation::bindCell(const int CellVirtual,const int CellBound)
  /*!
    Given two cells it uses a virtual cell to bind the extent
    of a non-virtual cell
    \param CellVirtual :: Virtual cell name (note that -ve indicates that 
    the complementary of the cell is to be use)
    \param CellBound :: cell to be changed.
    \return -ve if error / 0 on success
  */
{
  const int CVN((CellVirtual>0)  ? CellVirtual : -CellVirtual);
  const MonteCarlo::Qhull* CV=findQhull(CVN);
  MonteCarlo::Qhull* CB=findQhull(CellBound);
  if (!CV || !CB)
    return -1;
  if (CVN!=CellVirtual)
    {
      
      MonteCarlo::Qhull* CX=new MonteCarlo::Qhull(*CV);
      CX->makeComplement();
      removeComplement(*CX);
      const int res=CB->addSurfString(CX->cellCompStr());
      delete CX;
      return res;
    }
  return CB->addSurfString(CV->cellCompStr());
}


int
Simulation::makeVirtual(const int CellN) 
  /*!
    Sets a cell to be virtual
    \param CellN :: cell number to make virtual
    \retval 0 :: success
    \retval -1 :: cell not found
   */
{
  OTYPE::iterator vc;
  vc=OList.find(CellN);
  if (vc==OList.end())
    {
      return -1;
    }
  vc->second->setPlaceHold(1);
  return 0;
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

void
Simulation::processCellsImp()
  /*!
    Now process Physics so importance/volume cards can be set.
    \todo : What is the official score on cookie-cutter cells??
  */
{
  ELog::RegMethod RegA("Simulation","processCellsImp");

  std::vector<int> cellOrder;
  std::vector<double> impValue;

  OTYPE::const_iterator vc;
  for(vc=OList.begin();vc!=OList.end();vc++)
    {
      if (!vc->second->isPlaceHold())
	{
	  cellOrder.push_back(vc->first);
	  impValue.push_back( (vc->second->getImp()>0) ? 1.0 : 0.0);
	}
    }
  
  PhysPtr->setCellNumbers(cellOrder,impValue);
  return;
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
		{
		  ELog::EM<<"Error processing Algebra Complement : "
			  <<ELog::endErr;
		  throw ColErr::ExitAbort(RegA.getFull());
		}
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
      if (!objPtr->isObjSurfValid())
	{
	  if (objPtr->getSurfSet().empty())
	    objPtr->createSurfaceList();
	  // First add surface that are opposite 
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
Simulation::setEnergy(const double EMin) 
  /*!
    Set the cut energy
    \param EMin :: Energy minimum
   */
{
  ELog::RegMethod RegA("Simulation","setEnergy");

  // cut source + weight window (?)
  PhysPtr->setEnergyCut(EMin);
  TallyTYPE::iterator mc;
  for(mc=TItem.begin();mc!=TItem.end();mc++)
    mc->second->cutEnergy(EMin);

  return;
}

void
Simulation::removeAllTally()
  /*!
    Removes all of the tallies
   */
{
  for_each(TItem.begin(),TItem.end(),
	   MapSupport::mapDelete<TallyTYPE>());
  TItem.clear();
  return;
}

int
Simulation::removeTally(const int Key)
  /*!
    Rather drastic destruction of a tally
    (it can be turned off with setActive(0)).
    This removes the tally from TItem.
    \param Key :: Item to remove
    \retval 0 :: Tally removed
    \retval -1 :: Tally not found
  */
{
  TallyTYPE::iterator vc;
  vc=TItem.find (Key);
  if (vc==TItem.end())
    return -1;
  delete vc->second;
  TItem.erase(vc);
  return 0;
}

int
Simulation::addTally(const tallySystem::Tally& TRef)
  /*!
    Adds a tally to the main TItem list.
    \param TRef :: Tally item to insert
    \return 0 :: Successful
    \return -1 :: Tally already in use
  */
{
  ELog::RegMethod RegA("Simlation","addTally");
  
  const int keyNum=TRef.getKey();
  if (TItem.find(keyNum)!=TItem.end())
    return -1;

  tallySystem::Tally* TX=TRef.clone();
  TItem.insert(TallyTYPE::value_type(keyNum,TX));
  return 0;
}

void
Simulation::setSourceName(const std::string& S)
 /*!
   Set the source name from the database
   \param S :: Source name
  */
{
  sourceName=S;
  return;
}

void
Simulation::setForCinder()
  /*!
    Assuming that the cell tallies have
    been set. Set the system to have unit volumes
    The card is required regardless of its existance in 
    the mcnpx input deck.
  */
{
  ELog::RegMethod RegA("Simuation","setForCinder");

  PhysPtr->setHist(1);      // add histp card
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
				 
int
Simulation::isValidCell(const int cellNumber,const Geometry::Vec3D& Pt) const
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
Simulation::writeTally(std::ostream& OX) const
  /*!
    Writes out the tallies using a nice boost binding
    construction.
    \param OX :: Output stream
   */
{
  OX<<"c -----------------------------------------------------------"<<std::endl;
  OX<<"c ------------------- TALLY CARDS ---------------------------"<<std::endl;
  OX<<"c -----------------------------------------------------------"<<std::endl;
  std::vector<tallySystem::tmeshTally*> TMeshVec;
  for(const TallyTYPE::value_type& TM : TItem)
    {
      tallySystem::tmeshTally* TMPtr=
	dynamic_cast<tallySystem::tmeshTally*>(TM.second);
      if (!TMPtr)
	TM.second->write(OX);
      else
	TMeshVec.push_back(TMPtr);
    }
  int index(1);

  typedef std::vector<tallySystem::tmeshTally*> doseTYPE;

  doseTYPE doseMesh;
  if (!TMeshVec.empty())
    {
      OX<<"tmesh"<<std::endl;
      for(tallySystem::tmeshTally* TMPtr : TMeshVec)
	{
	  if (TMPtr->hasActiveMSHMF())
            {
              // check if mesh currently exists:
              const pairRange& mshValue=TMPtr->getMSHMF();
              doseTYPE::const_iterator mc=
                std::find_if(doseMesh.begin(),doseMesh.end(),
                             [&mshValue](const doseTYPE::value_type& A) -> bool
                             {
                               return (A->getMSHMF() == mshValue);
                             });
              if (mc==doseMesh.end())
                {
                  TMPtr->setActiveMSHMF(index++);
                  doseMesh.push_back(TMPtr);
                }
              else
                {
                  TMPtr->setActiveMSHMF(-(*mc)->hasActiveMSHMF());
                }
            }
          TMPtr->write(OX);
	}
      OX<<"endmd"<<std::endl;
    }
  
  return;
}

void
Simulation::writeTransform(std::ostream& OX) const
  /*!
    Write all the transforms in standard MCNPX output 
    type [These should now not be used].
    \param OX :: Output stream
  */

{
  OX<<"c -------------------------------------------------------"<<std::endl;
  OX<<"c --------------- TRANSFORM CARDS -----------------------"<<std::endl;
  OX<<"c -------------------------------------------------------"<<std::endl;

  TransTYPE::const_iterator vt;
  for(vt=TList.begin();vt!=TList.end();vt++)
    {
      vt->second.write(OX);
    }
  OX<<"c ++++++++++++++++++++++ END ++++++++++++++++++++++++++++"<<std::endl;
  return;
}


void
Simulation::writeCells(std::ostream& OX) const
  /*!
    Write all the cells in standard MCNPX output 
    type.
    \param OX :: Output stream
  */

{
  OX<<"c -------------------------------------------------------"<<std::endl;
  OX<<"c --------------- CELL CARDS --------------------------"<<std::endl;
  OX<<"c -------------------------------------------------------"<<std::endl;
  OTYPE::const_iterator mp;
  for(mp=OList.begin();mp!=OList.end();mp++)
    {
      mp->second->write(OX);
    }
  OX<<"c ++++++++++++++++++++++ END ++++++++++++++++++++++++++++"<<std::endl;
  OX<<std::endl;  // Empty line manditory for MCNPX
  return;
}


void
Simulation::writeSurfaces(std::ostream& OX) const
  /*!
    Write all the surfaces in standard MCNPX output 
    type.
    \param OX :: Output stream
  */

{
  OX<<"c -------------------------------------------------------"<<std::endl;
  OX<<"c --------------- SURFACE CARDS -------------------------"<<std::endl;
  OX<<"c -------------------------------------------------------"<<std::endl;

  const ModelSupport::surfIndex::STYPE& SurMap =
    ModelSupport::surfIndex::Instance().surMap();

  std::map<int,Geometry::Surface*>::const_iterator mp;
  for(mp=SurMap.begin();mp!=SurMap.end();mp++)
    {
      (mp->second)->write(OX);
    }
  OX<<"c ++++++++++++++++++++++ END ++++++++++++++++++++++++++++"<<std::endl;
  OX<<std::endl;
  return;
}

void
Simulation::writeMaterial(std::ostream& OX) const
  /*!
    Write all the used Materials in standard MCNPX output 
    type.
    \param OX :: Output stream
  */

{
  OX<<"c -------------------------------------------------------"<<std::endl;
  OX<<"c --------------- MATERIAL CARDS ------------------------"<<std::endl;
  OX<<"c -------------------------------------------------------"<<std::endl;
  ModelSupport::DBMaterial& DB=ModelSupport::DBMaterial::Instance();  
  DB.resetActive();


  if (!PhysPtr->getMode().hasElm("h"))
    DB.deactivateParticle("h");
  
  OTYPE::const_iterator mp;
  for(mp=OList.begin();mp!=OList.end();mp++)
    {
      DB.setActive(mp->second->getMat());
    }

  DB.writeMCNPX(OX);
  OX<<"c ++++++++++++++++++++++ END ++++++++++++++++++++++++++++"<<std::endl;
  return;
}

void
Simulation::writeWeights(std::ostream& OX) const
  /*!
    Write all the used Weight in standard MCNPX output 
    type.
    \param OX :: Output stream
  */

{
  ELog::RegMethod RegA("Simulation","writeWeights");
  
  WeightSystem::weightManager& WM=
    WeightSystem::weightManager::Instance();  
  OX<<"c -------------------------------------------------------"<<std::endl;
  OX<<"c --------------- WEIGHT CARDS --------------------------"<<std::endl;
  OX<<"c -------------------------------------------------------"<<std::endl;
  WM.write(OX);
  OX<<"c ++++++++++++++++++++++ END ++++++++++++++++++++++++++++"<<std::endl;
  return;
}


void
Simulation::writeSource(std::ostream& OX) const
  /*!
    Write the source care standard MCNPX output 
    type.
    \param OX :: Output stream
  */

{
  ELog::RegMethod RegA("Simulation","writeSource");
  
  SDef::sourceDataBase& SDB=
    SDef::sourceDataBase::Instance();

  OX<<"c -------------------------------------------------------"<<std::endl;
  OX<<"c --------------- SOURCE CARDS --------------------------"<<std::endl;
  OX<<"c -------------------------------------------------------"<<std::endl;

  if (!sourceName.empty())
    {
      SDef::SourceBase* SPtr=
	SDB.getSourceThrow<SDef::SourceBase>(sourceName,"Source not known");
      SPtr->write(OX);
    }
  OX<<"c ++++++++++++++++++++++ END ++++++++++++++++++++++++++++"<<std::endl;
  return;
}


void
Simulation::writePhysics(std::ostream& OX) const
  /*!
    Write all the used Weight in standard MCNPX output 
    type. Note that it also has to add the rdum cards
    to the physics
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("Simulation","writePhysics");

  OX<<"c -------------------------------------------------------"<<std::endl;
  OX<<"c --------------- PHYSICS CARDS --------------------------"<<std::endl;
  OX<<"c -------------------------------------------------------"<<std::endl;

  // Processing for point tallies
  std::map<int,tallySystem::Tally*>::const_iterator mc;
  std::vector<int> Idum;
  std::vector<Geometry::Vec3D> Rdum;
  if (mcnpVersion==10)
    {
      for(mc=TItem.begin();mc!=TItem.end();mc++)
	{
	  const tallySystem::pointTally* Ptr=
	    dynamic_cast<const tallySystem::pointTally*>(mc->second);
	  if(Ptr && Ptr->hasRdum())
	    {
	      Idum.push_back(Ptr->getKey());
	      for(size_t i=0;i<4;i++)
		Rdum.push_back(Ptr->getWindowPt(i));
	      Rdum.push_back(Geometry::Vec3D(Ptr->getSecondDist(),0,0));
	    }
	}
      
      if (!Idum.empty())
	{
	  OX<<"idum "<<Idum.size()<<" ";
	  copy(Idum.begin(),Idum.end(),std::ostream_iterator<int>(OX," "));
	  OX<<std::endl;
	  OX<<"rdum       "<<Rdum.front()<<std::endl;
	  std::vector<Geometry::Vec3D>::const_iterator vc;
	  for(vc=Rdum.begin()+1;vc!=Rdum.end();vc++)
	    OX<<"           "<< *vc<<std::endl;
	}
    }
  
  // Remaining Physics cards
  PhysPtr->write(OX,cellOutOrder,voidCells);
  OX<<"c ++++++++++++++++++++++ END ++++++++++++++++++++++++++++"<<std::endl;
  OX<<std::endl;  // MCNPX requires a blank line to terminate
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
  ELog::RegMethod RegA("Simulation","writeVaraibles");
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
  

void
Simulation::reZeroFromVertex(const int CellN,const unsigned int M,
			     const unsigned int A,
			     const unsigned int B,const unsigned int C,
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

void 
Simulation::writeCinder() const
  /*!
    Write out files useful to cinder
  */
{
  ELog::RegMethod RegA("Simulation","writeCinder");

  writeCinderMat();
  return;
}


void
Simulation::writeCinderMat() const
  /*!
    Writes out a cinder material output deck
  */
{
  ELog::RegMethod RegA("Simulation","writeCinderMat");
  std::ofstream OX("material");  
  // Get used material list

  ModelSupport::DBMaterial& DB=ModelSupport::DBMaterial::Instance();  
  DB.resetActive();

  OTYPE::const_iterator mp;
  for(mp=OList.begin();mp!=OList.end();mp++)
    DB.setActive(mp->second->getMat());

  DB.writeCinder(OX);
  OX.close();
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
    \param zaidNumber :: Material zaid number
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

void
Simulation::renumberCells(const std::vector<int>& cOffset,
			  const std::vector<int>& cRange)
  /*!
    Re-arrange all the cell numbers to be sequentual from 1-N.
    \param cOffset :: Protected start
    \param cRange :: Protected range
  */
{
  ELog::RegMethod RegA("Simulation","renumberCells");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  WeightSystem::weightManager& WM=
    WeightSystem::weightManager::Instance();

  //Offset index  
  const int cIndex(10000);

  OTYPE newMap;           // New map with correct numbering
  int nNum(0);
  int index(1);

  std::string oldUnit,keyUnit;
  int startNum(0);
  const attachSystem::CellMap* CMapPtr(0);
  // This is ordered:
  OTYPE::const_iterator vc;  
  for(vc=OList.begin();vc!=OList.end();vc++)
    {
      const int cNum=vc->second->getName();
      keyUnit=OR.inRange(cNum);
      // Determine inf the cell is within cRange:
      size_t j=0;
      while(j<cOffset.size())
	{
	  if (cNum>=cOffset[j] && cNum<cOffset[j]+cRange[j])
	    {
	      nNum=cIndex*static_cast<int>(j+1)+(cNum-cOffset[j]);
	      break;
	    }
	  j++;
	}
      // if not find the next index:
      if (j==cOffset.size())
	{
	  nNum=index++;
	  // skip index over preserved ranges:
	  if (index==cIndex)
	    index+=cIndex*static_cast<int>(cOffset.size());
	}
      // Do renumber:
      vc->second->setName(nNum);      
      newMap.insert(OTYPE::value_type(nNum,vc->second));
      WM.renumberCell(cNum,nNum);

      if (!vc->second->isPlaceHold())
	{
	  PhysPtr->substituteCell(cNum,nNum);
	  for(TallyTYPE::value_type& TI : TItem)
	    TI.second->renumberCell(cNum,nNum);
	}
      if (keyUnit!=oldUnit)
	{
	  if (startNum)
	    OR.setRenumber(oldUnit,startNum,nNum-1);
	  oldUnit=keyUnit;
	  startNum=nNum;
          CMapPtr=OR.getObject<attachSystem::CellMap>(keyUnit);
	}
      
      ELog::RN<<"Cell Changed :"<<cNum<<" "<<nNum
	      <<" Object:"<<keyUnit;
      if (CMapPtr)
        {
          const std::string& cName=
            CMapPtr->getName(cNum);
          if (!cName.empty())
            ELog::RN<<" ("<<cName<<")";
          const std::string& xName=
            CMapPtr->getName(nNum);
          if (!xName.empty())
            ELog::EM<<"Found "<<xName<<" "<<cNum<<" "<<nNum<<ELog::endCrit;
         }
      ELog::RN<<ELog::endBasic;
    }

  // Last item
  OR.setRenumber(keyUnit,startNum,nNum);
  OList=newMap;
  return;
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
    Given an object set remove it from system 
    \param ObjName :: Object to void
  */
{
  ELog::RegMethod RegA("Simulation","voidObject");

  const ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  const int cellN=OR.getCell(ObjName);
  const int cellRange=OR.getRange(ObjName);
  if (!cellN)
    throw ColErr::InContainerError<std::string>(ObjName,"ObjName");

  for(int i=1;i<=cellRange;i++)
    {
      MonteCarlo::Qhull* QH=findQhull(cellN+i);
      if (!QH)
	return;
      QH->setMaterial(0);
    }
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

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

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

  OR.rotateMaster();
  
  return;
}

void
Simulation::masterPhysicsRotation()
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
Simulation::write(const std::string& Fname) const
  /*!
    Write out all the system (in MCNPX output format)
    \param Fname :: Output file 
  */
{
  std::ofstream OX(Fname.c_str());
  
  OX<<"Input File:"<<inputFile<<std::endl;
  StrFunc::writeMCNPXcomment("RunCmd:"+cmdLine,OX);
  writeVariables(OX);
  writeCells(OX);
  writeSurfaces(OX);
  writeMaterial(OX);
  writeTransform(OX);
  writeWeights(OX);
  writeTally(OX);
  writeSource(OX);
  writePhysics(OX);
  OX.close();
  return;
}
