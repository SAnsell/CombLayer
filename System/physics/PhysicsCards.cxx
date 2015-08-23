/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physics/PhysicsCards.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <functional>
#include <memory>
#include <array>
#include <boost/algorithm/string.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "MapRange.h"
#include "Triple.h"
#include "SrcData.h"
#include "SrcItem.h"
#include "DSTerm.h"
#include "Source.h"
#include "KCode.h"
#include "ModeCard.h"
#include "PhysImp.h"
#include "PhysCard.h"
#include "PStandard.h"
#include "LSwitchCard.h"
#include "NList.h"
#include "NRange.h"
#include "dbcnCard.h"
#include "EUnit.h"
#include "ExtControl.h"
#include "PWTControl.h"
#include "PhysicsCards.h"

namespace physicsSystem
{

PhysicsCards::PhysicsCards() :
  nps(10000),histp(0),dbCard(new dbcnCard),
  voidCard(0),nImpOut(0),prdmp("1e7 1e7 0 2 1e7"),
  Volume("vol"),ExtCard(new ExtControl),
  PWTCard(new PWTControl)
  /*!
    Constructor
  */
{}

PhysicsCards::PhysicsCards(const PhysicsCards& A) : 
  nps(A.nps),histp(A.histp),histpCells(A.histpCells),
  dbCard(new dbcnCard(*dbCard)),Basic(A.Basic),mode(A.mode),
  voidCard(A.voidCard),nImpOut(A.nImpOut),printNum(A.printNum),
  prdmp(A.prdmp),ImpCards(A.ImpCards),
  PCards(),LEA(A.LEA),sdefCard(A.sdefCard),
  Volume(A.Volume),
  ExtCard(new ExtControl(*A.ExtCard)),
  PWTCard(new PWTControl(*A.PWTCard))
  /*!
    Copy constructor
    \param A :: PhysicsCards to copy
  */
{
  for(const PhysCard* PC : A.PCards)
    PCards.push_back(PC->clone());      
}

PhysicsCards&
PhysicsCards::operator=(const PhysicsCards& A)
  /*!
    Assignment operator
    \param A :: PhysicsCards to copy
    \return *this
  */
{
  if (this!=&A)
    {
      nps=A.nps;
      histp=A.histp;
      histpCells=A.histpCells;
      *dbCard = *A.dbCard;
      Basic=A.Basic;      
      mode=A.mode;
      voidCard=A.voidCard;
      printNum=A.printNum;
      prdmp=A.prdmp;
      ImpCards=A.ImpCards;
      LEA=A.LEA;
      sdefCard=A.sdefCard;
      Volume=A.Volume;
      *ExtCard= *A.ExtCard;
      *PWTCard= *A.PWTCard;

      deletePCards();
      for(const PhysCard* PC : A.PCards)
	PCards.push_back(PC->clone());      

    }
  return *this;
}


PhysicsCards::~PhysicsCards()
  /*!
    Destructor
  */
{
  deletePCards();
}

void
PhysicsCards::deletePCards()
  /*!
    Delete the memory
  */
{
  for(PhysCard* PC : PCards)
    delete PC;
  PCards.clear();
  return;
}
  
void
PhysicsCards::clearAll()
  /*!
    The big reset
  */
{
  histpCells.clear();
  Basic.clear();
  mode.clear();
  printNum.clear();
  ImpCards.clear();
  deletePCards();
  Volume.clear();
  sdefCard.clear();
  dbCard->reset();
  ExtCard->clear();
  PWTCard->clear();
  return;
}

void
PhysicsCards::addHistpCells(const std::vector<int>& AL)
  /*!
    Adds a cell list to the hist tape file
    \param AL :: List to add
   */
{
  ELog::RegMethod RegA("PhysicsCards","addHistpCells");
  for(const int CellN : AL)
    histpCells.addComp(CellN);
  return;
}

void
PhysicsCards::clearHistpCells()
  /*!
    Clears the cell list from the hist tape file
   */
{
  histpCells.clear();
  return;
}

int 
PhysicsCards::processCard(const std::string& Line)
  /*!
    Attempt to process a card.
    If comment, store in 'Comments' and insert '0' into 'Cards'
    If card, insert "" into Comments and store in Cards.
    \param Line :: string to process
    \retval :: -ve on fail
    \retval 1 on successful addition of a card
    \retval 0 on null line/comment line
    \todo Trim leading space
  */
{
  ELog::RegMethod RegA("PhysicsCards","processCard");
  if(Line.empty())
    return 0;  

  int extCell(0);
  std::string Comd=Line;
  StrFunc::stripComment(Comd);
  Comd=StrFunc::fullBlock(Comd);

  // Check the first 2 characters for comment
  if(Comd.size()<2 || (isspace(Comd[1]) &&
		       ( tolower(Comd[0])=='c' || Comd[0]=='!'))) 
    return 0;
  
  std::string::size_type pos=Comd.find("mode ");
  if (pos!=std::string::npos)
    {
      std::string item;
      Comd.erase(0,pos+5);
      while(StrFunc::section(Comd,item))
	mode.addElm(item);
      return 1;
    }
  pos=Comd.find("print ");
  if (pos!=std::string::npos)
    {
      printNum.clear();
      std::string Item=Comd.substr(pos+1);
      int pNum;
      while(StrFunc::section(Item,pNum))
	printNum.push_back(pNum);
    }

  
  pos=Comd.find("imp:");
  if (pos!=std::string::npos)
    {
      Comd.erase(0,pos+4);
      ImpCards.push_back(PhysImp("imp"));
      // Ugly hack to get all the a,b,c,d items 
      // since I can't think of the regular expression
      unsigned int index;
      for(index=0;index<Comd.length() && !isspace(Comd[index]);index++)
	if (Comd[index]!=',')
	  ImpCards.back().addElm(std::string(1,Comd[index]));
      return 1;
    }

  pos=Comd.find("vol");
  if (pos!=std::string::npos)
    {
      Comd.erase(0,pos+3);
      Volume=PhysImp("vol");
      return 1;
    }
  
  pos=Comd.find("histp");
  if (pos!=std::string::npos)
    {
      histp=1;
      return 1;
    }

  // This is ignored since we have a prdmp card
  pos=Comd.find("prdmp");    
  if (pos!=std::string::npos)
    {
      return 1;
    }

  pos=Comd.find("nps");
  if (pos!=std::string::npos)
    {
      Comd.erase(0,pos+3);
      if (!StrFunc::convert(Comd,nps))
	nps=800000;
      return 1;
    }

  pos=Comd.find("dbcn");
  if (pos!=std::string::npos)
    {
      long int RS;
      size_t kType;
      double D;
      std::string keyName;
      Comd.erase(0,pos+4);
      if (StrFunc::convert(Comd,RS))
	dbCard->setComp("rndSeed",RS);
      else if (StrFunc::section(Comd,keyName) &&
	       (kType=dbCard->keyType(keyName)) )
	{
	  if (kType==1 && StrFunc::section(Comd,RS))
	    dbCard->setComp(keyName,RS);
	  else if (StrFunc::section(Comd,D))
	    dbCard->setComp(keyName,RS);
	  else
	    ELog::EM<<"DBCN ["<<keyName<<"] error  == "
		    <<Comd<<ELog::endErr;
	}
      else
	{
	  ELog::EM<<"DBCN [rnd] error  == "<<Comd<<ELog::endErr;
	}
      return 1;
    }

  pos=Comd.find("cut:");
  if (pos!=std::string::npos)
    {
      Comd.erase(0,pos+4);
      PStandard* PC=new PStandard("cut");
      std::string Item;
      if (StrFunc::section(Comd,Item))
        {
	  std::vector<std::string> part;
	  boost::split(part,Item,boost::is_any_of(","));
	  for(std::string& PStr : part)
	    boost::trim(PStr,std::locale());
	  // ensure no empty particles
	  part.erase(
		     remove_if(part.begin(),part.end(),
			       std::bind<bool>(&std::string::empty,
					       std::placeholders::_1)),
		     part.end());
	  
	  for(const std::string& PStr : part)
	    PC->addElm(PStr);
	  // Strip and process numbers / j
	  for(size_t i=0;i<5 && !Comd.empty();i++)
	    {
	      double d;
	      if (StrFunc::section(Comd,d))
		PC->setValue(i,d);
	      else if (StrFunc::section(Comd,Item))
	        {
		  if (Item.size()==1 && tolower(Item[0]=='j'))
		    PC->setDef(i);
		}
	    }
	  PCards.push_back(PC);
	}
      return 1;
    }
  // ext card
  pos=Comd.find("ext:");
  if (pos!=std::string::npos)
    {
      Comd.erase(0,pos+4);
      // Ugly hack to get all the a,b,c,d items 
      // since I can't think of the regular expression
      size_t index;
      for(index=0;index<Comd.length() && !isspace(Comd[index]);index++)
	if (Comd[index]!=',')
	  ExtCard->addElm(std::string(1,Comd[index]));
      // NOW HAVE PROBLEM BECAUSE MULTI-LINE
      extCell=1;
      if (ExtCard->addUnitList(extCell,Comd))
	return 1;
      // drops through to further processing
    }

  if (extCell)
    {
      if (ExtCard->addUnitList(extCell,Comd))
	return 1;
    }
	
  // Component:
  Basic.push_back(Line);
  return 1;
}

void
PhysicsCards::setEnergyCut(const double E) 
  /*!
    Sets the minimium energy cut
    \param E :: Energy [MeV]
  */
{
  ELog::RegMethod RegA("PhysicsCards","setEnergyCut");

  for(PhysCard* PC : PCards)
    PC->setEnergyCut(E);

  sdefCard.cutEnergy(E);
  return;
}

PhysImp&
PhysicsCards::addPhysImp(const std::string& Type,const std::string& Particle)
  /*!
    Adds / returns the imporance and particle type
    This can also separate a particular particle from 
    a list
    \param Type :: Type to use [imp:vol etc]
    \param Particle :: Particle to add [n,p,e] etc
    \return PhysImp item
  */
{
  ELog::RegMethod RegA("PhysicsCards","addPhysImp");
  try 
    {
      PhysImp& FImp=getPhysImp(Type,Particle);
      if (FImp.particleCount()==1)
	return FImp;
      // remove particle so a new PhysImp can be specialised
      FImp.removeParticle(Particle);
    }
  catch (ColErr::InContainerError<std::string>&)
    { }       
  // Create a new object
  ImpCards.push_back(PhysImp(Type));
  ImpCards.back().addElm(Particle);
  return ImpCards.back();
}

  

template<typename T>
T*
PhysicsCards::addPhysCard(const std::string& Key,
			  const std::string& Particle)
  /*!
    Adds a particular card [if not already present].
    If ANY particle is found in the current cards, then 
    all remaining particles are added and the 
    card returned. If not then a new card is created
    \tparam T :: PhysCard Template
    \param Key :: KeyName
    \param Particle :: Particle to add [n,p,e] etc

    \return PhysCard
  */
{
  ELog::RegMethod RegA("PhysicsCards","addPhysCard");
  const std::vector<std::string> PList=
    StrFunc::StrParts(Particle);
  
  for(PhysCard* PC : PCards)
    {
      int found(0);
      if (PC->getKey()==Key)  	// First determine if any key exists:
	{
	  T* PX=(dynamic_cast<T*>(PC));
	  if (PX)
	    {
	      for(size_t i=0;i<PList.size();i++)
		{
		  const std::string particle=PList[i];
		  if (!found && PX->hasElm(particle))
		    {
		      // Add old non-found particles [previous]
		      for(size_t j=0;j<i;j++)
			PX->addElm(PList[i]);
		      found=1;
		    }
		  else if (found && !PX->hasElm(particle))
		    PX->addElm(particle);
		}
	      if (found)
		return PX;
	    }
	}
    }
  // No card found add: ADD PSTANDARD
  T* PX=new T(Key);
  PCards.push_back(PX);
  for(const std::string& particle :  PList)
    PX->addElm(particle);
  return PX;
}

const PhysCard*
PhysicsCards::getPhysCard(const std::string& Key,
			  const std::string& particle) const
  /*!
    Gets a particular card with Key and 
    particle [if empty first card with key]
    \param Key :: KeyName
    \param particle :: Particle to find [n,p,e] etc [empty for any]
    \return PhysCard
  */
{
  ELog::RegMethod RegA("PhysicsCards","getPhysCard");
  
  for(const PhysCard* PC : PCards)
    if (PC->getKey()==Key &&
	(particle.empty() || PC->hasElm(particle)))  
      return PC;

  throw ColErr::InContainerError<std::string>
    (Key+":"+particle,"PhysCard search");
}

// General All Importance
void
PhysicsCards::setCellNumbers(const std::vector<int>& cellInfo,
			     const std::vector<double>& impValue)
  /*!
    Process the list of the valid cells 
    over each importance group.
    \param cellInfo :: list of cells
    \param impValue  :: Value of the important
  */

{
  ELog::RegMethod RegA("PhysicsCards","setCellNumbers");
  
  if(cellInfo.size()!=impValue.size())
    throw ColErr::MisMatch<size_t>(cellInfo.size(),
				   impValue.size(),
				   "PhysicsCards.setCellNumbers");
  for(PhysImp& PI : ImpCards)
    PI.setAllCells(cellInfo,impValue);

  Volume.setCells(cellInfo,1.0);
  return;
}

// General object [All Particles]:
void
PhysicsCards::setCells(const std::string& Type,
		       const std::vector<int>& cellInfo,
		       const double defValue)
  /*!
    Process the list of the valid cells 
    over each importance group.
    \param Type :: imp type (imp vol etc)
    \param cellInfo :: list of cells
    \param defValue :: default value
  */

{
  ELog::RegMethod RegA("PhysicsCards","setCells");
  
  for(PhysImp& PI : ImpCards)
      {
	if (PI.isType(Type))
	  PI.setCells(cellInfo,defValue);
      }    
  return;
}

// General object [All Particles]:

void
PhysicsCards::setCells(const std::string& Type,
		       const int cellID,const double defValue)
  /*!
    Process the list of the valid cells 
    over each importance group.
    \param Type :: imp type (imp vol etc)
    \param cellID :: Cell number
    \param defValue :: default value
  */
{
  ELog::RegMethod RegA("PhysicsCards","setCells");
  
  for(PhysImp& PI : ImpCards)
    {
      if (PI.isType(Type))
	PI.setValue(cellID,defValue);
    }    
  return;
}


// Specific : Particle + Type

void
PhysicsCards::setCells(const std::string& Type,
		       const std::string& Particle,
		       const int cellID,const double V)
  /*!
    Sets the value of a particular cell and type
    \param Type :: type of physics card
    \param Particle :: particle list 
    \param cellID :: the particular cell number to set
    \param V :: New importance value  
  */
{
  ELog::RegMethod RegA("PhysicsCards","setCells(string,string,int,double)");

  for(PhysImp& PI : ImpCards)
    {
      if (PI.isType(Type) &&
	  PI.hasElm(Particle))
	{
	  PI.setValue(cellID,V);
	  return;
	}
    }    

  throw ColErr::InContainerError<std::string>(Type+"/"+Particle,
				 "Type/Particle");
}

void
PhysicsCards::removeCell(const int Index)
  /*!
    Remove a particular cell (normally since a cutter cell)
    \param Index :: Cell id to remove
  */
{
  ELog::RegMethod RegA("PhysicsCards","removeCell");
  
  for(PhysImp& PI : ImpCards)
    PI.removeCell(Index);
  return;
}

double
PhysicsCards::getValue(const std::string& Type,
		       const std::string& Particle,
		       const int cellID) const
  /*!
    Return the importance of a given cell
    \param Type :: importance identifier
    \param Particle :: particle list
    \param cellID :: Cell number
    \throw InContainerError if Type+Particle is not found
    \return importance of the cell
  */
{
  const PhysImp& AP=getPhysImp(Type,Particle);
  return AP.getValue(cellID);
}

PhysImp&
PhysicsCards::getPhysImp(const std::string& type,
			 const std::string& particle)
  /*!
    Return the PhysImp for a particle type
    \param type :: Importance type
    \param particle :: particular particle (in list)
    \throw InContainerError if pType is not found
    \return importance of the cell
  */
{
  ELog::RegMethod RegA("PhysicsCards","getPhysImp");

  std::vector<PhysImp>::iterator vc;
  for(PhysImp& PI : ImpCards)
    {
      if (PI.isType(type) &&
	  PI.hasElm(particle))
	return PI;
    }
  throw ColErr::InContainerError<std::string>
        (type+"/"+particle,"type/particle not found");
}


const PhysImp&
PhysicsCards::getPhysImp(const std::string& type,
			 const std::string& particle) const
  /*!
    Return the PhysImp for a particle type
    \param type :: type of importance
    \param particle :: particle type
    \throw InContainerError if pType is not found
    \return importance of the cell
  */
{
  ELog::RegMethod RegA("PhysicsCards","getPhysImp(const)");
    
  std::vector<PhysImp>::const_iterator vc;
  for(const PhysImp& PI : ImpCards)
    {
      if (PI.isType(type) &&
	  PI.hasElm(particle))
	return PI;
    }
  
  throw ColErr::InContainerError<std::string>
    (type+"/"+particle,"type/particle not found");
}
  
void
PhysicsCards::setVolume(const std::vector<int>& cellInfo,const double defValue)
  /*!
    Process the list of the valid cells 
    setting the volume list
    \param cellInfo :: list of cells
    \param defValue :: default value
  */
{
  Volume.setCells(cellInfo,defValue);
  return;
}

void
PhysicsCards::setVolume(const int cellID,const double V)
  /*!
    Sets the volume of a particular cell
    \param cellID :: the particular cell number to set
    \param V :: New importance value  
  */
{
  Volume.setValue(cellID,V);
  return;
}


void
PhysicsCards::setPWT(const int cellID,const double V)
  /*!
    Sets the PWT of a particular cell
    \param cellID :: the particular cell number to set
    \param V :: New importance value  
  */
{
  PWTCard->setUnit(cellID,V);
  return;
}

void
PhysicsCards::setRND(const long int N)
{
  dbCard->setComp("rndSeed",N);
  return;
}

long int
PhysicsCards::getRND() const
  /*!
    
   */
{
  return dbCard->getComp<long int>("rndSeed");
}
  
void
PhysicsCards::substituteCell(const int oldCell,const int newCell)
  /*!
    Substitute all cells in all physics cards that use cells
    \param oldCell :: old cell number
    \param newCell :: new cell number
   */
{
  ELog::RegMethod RegA("PhysicsCards","substituteCell");
  sdefCard.substituteCell(oldCell,newCell);
  histpCells.changeItem(oldCell,newCell);
  for(PhysImp& PI : ImpCards)
    PI.renumberCell(oldCell,newCell);
  
  Volume.renumberCell(oldCell,newCell);
  PWTCard->renumberCell(oldCell,newCell);
  ExtCard->renumberCell(oldCell,newCell);

  return;
}

void
PhysicsCards::substituteSurface(const int oldSurf,const int newSurf)
  /*!
    Substitute all surfaces in all physics cards that use surface
    \param oldSurf :: old surface number
    \param newSurf :: new surface number
  */
{
  sdefCard.substituteSurface(oldSurf,newSurf);
  return;
}

void
PhysicsCards::setMode(std::string Particles) 
  /*!
    Set the mode card if not already set
    \param Particles :: Particle list
  */
{
  ELog::RegMethod RegA("PhysicsCards","setMode");
  if (!mode.isSet())
    {
      std::string item;
      while(StrFunc::section(Particles,item))
	{
	  mode.addElm(item);
	  if (item!="n")
	    addPhysImp("imp",item);
	}
    }
  return;
}

void
PhysicsCards::setPrintNum(std::string Numbers) 
  /*!
    Set the print card if not already set
    \param Numbers ::  list of number [not checked]
  */
{
  ELog::RegMethod RegA("PhysicsCards","setPrintNum");
  if (printNum.empty())
    {
      int item;
      while(StrFunc::section(Numbers,item))
	printNum.push_back(item);
    }
  return;
}

void 
PhysicsCards::write(std::ostream& OX,
		    const std::vector<int>& cellOutOrder,
		    const std::set<int>& voidCells) const 
  /*!
    Write out each of the cards
    \param OX :: Output stream
    \param cellOutOrder :: Cell List
    \param voidCell :: List of void cells
    \todo Check that histp does not need a line cut.
  */
{
  ELog::RegMethod RegA("PhyiscsCards","write");

  dbCard->write(OX);
  OX<<"nps "<<nps<<std::endl;
  if (voidCard)
    OX<<"void"<<std::endl;

  if (histp)
    {
      std::ostringstream cx;
      cx<<"histp";
      if (!histpCells.empty())
	cx<<" -500000000 "<<histpCells;
      StrFunc::writeMCNPX(cx.str(),OX);
    }
  mode.write(OX);
  Volume.write(OX,cellOutOrder);
   for(const PhysImp& PI : ImpCards)
    PI.write(OX,cellOutOrder);  

   PWTCard->write(OX,cellOutOrder,voidCells);

  for(const PhysCard* PC : PCards)
    PC->write(OX);

  for(const std::string& PC : Basic)
    StrFunc::writeMCNPX(PC,OX);

  ExtCard->write(OX,cellOutOrder,voidCells);
  
  LEA.write(OX);
  sdefCard.write(OX);
  kcodeCard.write(OX);
  
  if (!prdmp.empty())
    StrFunc::writeMCNPX("prdmp "+prdmp,OX);
  
  if (!printNum.empty())
    {
      std::ostringstream cx;
      cx<<"print ";
      copy(printNum.begin(),printNum.end(),
	   std::ostream_iterator<int>(cx," "));
      StrFunc::writeMCNPX(cx.str(),OX);
    }

  return;
}

template PStandard*
PhysicsCards::addPhysCard(const std::string&,const std::string&);

} // NAMESPACE PhysicsCards	
      
   
