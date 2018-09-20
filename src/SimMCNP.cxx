/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   src/SimMCNP.cxx
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
#include "SimMCNP.h"

SimMCNP::SimMCNP()  :
  Simulation(),
  mcnpVersion(6),
  PhysPtr(new physicsSystem::PhysicsCards)
  
  /*!
    Start of simulation Object
  */
{
  ModelSupport::SimTrack::Instance().addSim(this);
}


SimMCNP::~SimMCNP()
  /*!
    Destructor :: deletes the memory in the Tally
    list
  */
{
  ELog::RegMethod RegA("SimMCNP","delete operator");

  delete PhysPtr;
  deleteTally();
}

void
SimMCNP::resetAll()
  /*!
    The big reset
  */
{
  deleteTally();
  Simulation::resetAll();
  return;
}

void
SimMCNP::deleteTally()
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
SimMCNP::setMCNPversion(const int V)
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



tallySystem::Tally*
SimMCNP::getTally(const int Index) const
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
SimMCNP::getSSWTally() const
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
SimMCNP::nextTallyNum(int tNum) const
  /*!
    Quick funciton to get the next tally point
    \param tNum :: initial index
    \return Next free tally number
   */
{
  ELog::RegMethod RegA("SimMCNP","nextTallyNum");

  while(TItem.find(tNum)!=TItem.end())
    tNum+=10;

  return tNum;
}

void
SimMCNP::setEnergy(const double EMin) 
  /*!
    Set the cut energy
    \param EMin :: Energy minimum
   */
{
  ELog::RegMethod RegA("SimMCNP","setEnergy");

  for(TallyTYPE::value_type& mc : TItem)
    mc.second->cutEnergy(EMin);

  PhysPtr->setEnergyCut(EMin);
  
  //  Simulation::setEnergy(EMin);
  return;
}

void
SimMCNP::removeCell(const int cellNumber)
  /*!
    Removes the cell [Must exist]
    \param cellNumber :: cell to remove
   */
{
  ELog::RegMethod RegItem("SimMCNP","removeCell");

  Simulation::removeCell(cellNumber);
  PhysPtr->removeCell(cellNumber);

  return;
}

void
SimMCNP::removeAllTally()
  /*!
    Removes all of the tallies
   */
{
  for(TallyTYPE::value_type& mc : TItem)
    delete mc.second;

  TItem.clear();
  return;
}

int
SimMCNP::removeTally(const int Key)
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
SimMCNP::addTally(const tallySystem::Tally& TRef)
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
  TItem.emplace(keyNum,TX);
  return 0;
}

void
SimMCNP::setForCinder()
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

void
SimMCNP::substituteAllSurface(const int oldSurfN,const int newSurfN)
  /*!
    Remove all the surface of name oldSurfN
    from the simulation. Then remove
    surface oldSurf from the surface map.
    \param oldSurfN :: Number of surface to alter
    \param newSurfN :: Number of new surface + sign to swap KeyN sign.
    \throw IncontainerError if the key does not exist
  */
{
  ELog::RegMethod RegA("Simulation","substituteAllSurface");

  Simulation::substituteAllSurface(oldSurfN,newSurfN);


  for(TallyTYPE::value_type& tc : TItem)
    tc.second->renumberSurf(oldSurfN,newSurfN);
  
  return;
}

				 
std::map<int,int>
SimMCNP::renumberCells(const std::vector<int>& cOffset,
		       const std::vector<int>& cRange)
  /*!
    Re-arrange all the cell numbers to be sequentual from 1-N.
    \param cOffset :: Protected start
    \param cRange :: Protected range
  */
{
  ELog::RegMethod RegA("SimMCNP","renumberCells");

  const std::map<int,int> RMap=
    Simulation::renumberCells(cOffset,cRange);

  // CARE HERE: RMap is the old number. The objects themselve
  //  have already been updated
  for(const std::map<int,int>::value_type& RMItem : RMap)
    {
      const int cNum=RMItem.first;
      const int nNum=RMItem.second;
      MonteCarlo::Qhull* oPtr=Simulation::findQhull(nNum);   // NOTE new number
      if (!oPtr->isPlaceHold())
	{
	  PhysPtr->substituteCell(cNum,nNum);
	  for(TallyTYPE::value_type& TI : TItem)
	    TI.second->renumberCell(cNum,nNum);
	}
    }
  return RMap;
}


void
SimMCNP::writeTally(std::ostream& OX) const
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
SimMCNP::writeTransform(std::ostream& OX) const
  /*!
    Write all the transforms in standard MCNPX output 
    type [These should now not be used].
    \param OX :: Output stream
  */

{
  OX<<"c -------------------------------------------------------"<<std::endl;
  OX<<"c --------------- TRANSFORM CARDS -----------------------"<<std::endl;
  OX<<"c -------------------------------------------------------"<<std::endl;

  for(const TransTYPE::value_type& vt : TList)
    vt.second.write(OX);

  OX<<"c ++++++++++++++++++++++ END ++++++++++++++++++++++++++++"<<std::endl;
  return;
}


void
SimMCNP::writeCells(std::ostream& OX) const
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
SimMCNP::writeSurfaces(std::ostream& OX) const
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
  
  for(const ModelSupport::surfIndex::STYPE::value_type& sm : SurMap)
    sm.second->write(OX);

  OX<<"c ++++++++++++++++++++++ END ++++++++++++++++++++++++++++"<<std::endl;
  OX<<std::endl;
  return;
}

void
SimMCNP::writeMaterial(std::ostream& OX) const
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
SimMCNP::writeWeights(std::ostream& OX) const
  /*!
    Write all the used Weight in standard MCNPX output 
    type.
    \param OX :: Output stream
  */

{
  ELog::RegMethod RegA("SimMCNP","writeWeights");
  
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
SimMCNP::writeSource(std::ostream& OX) const
  /*!
    Write the source care standard MCNPX output 
    type.
    \param OX :: Output stream
  */

{
  ELog::RegMethod RegA("SimMCNP","writeSource");
  
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
SimMCNP::writePhysics(std::ostream& OX) const
  /*!
    Write all the used Weight in standard MCNPX output 
    type. Note that it also has to add the rdum cards
    to the physics
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("SimMCNP","writePhysics");

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
SimMCNP::writeCinder() const
  /*!
    Write out files useful to cinder
  */
{
  ELog::RegMethod RegA("SimMCNP","writeCinder");

  writeCinderMat();
  return;
}


void
SimMCNP::writeCinderMat() const
  /*!
    Writes out a cinder material output deck
  */
{
  ELog::RegMethod RegA("SimMCNP","writeCinderMat");
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

void
SimMCNP::write(const std::string& Fname) const
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
