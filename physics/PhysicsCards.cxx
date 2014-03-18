/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   physics/PhysicsCards.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <map>
#include <string>
#include <algorithm>
#include <functional>
#include <boost/algorithm/string.hpp>
#include <boost/functional.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "support.h"
#include "Triple.h"
#include "SrcData.h"
#include "SrcItem.h"
#include "DSTerm.h"
#include "Source.h"
#include "KCode.h"
#include "ModeCard.h"
#include "PhysImp.h"
#include "PhysCard.h"
#include "LSwitchCard.h"
#include "NList.h"
#include "NRange.h"
#include "KGroup.h"
#include "PhysicsCards.h"

namespace physicsSystem
{

PhysicsCards::PhysicsCards() :
  rndSeed(123456781L),nps(10000),
  histp(0),voidCard(0),prdmp("1e7 1e7 0 2 1e7"),
  Volume("vol")
  /*!
    Constructor
  */
{}

PhysicsCards::PhysicsCards(const PhysicsCards& A) : 
  rndSeed(A.rndSeed),nps(A.nps),histp(A.histp),
  histpCells(A.histpCells),Basic(A.Basic),mode(A.mode),
  voidCard(A.voidCard),printNum(A.printNum),prdmp(A.prdmp),
  ImpCards(A.ImpCards),PhysCards(A.PhysCards),LEA(A.LEA),
  sdefCard(A.sdefCard),Volume(A.Volume)
  /*!
    Copy constructor
    \param A :: PhysicsCards to copy
  */
{}

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
      rndSeed=A.rndSeed;
      nps=A.nps;
      histp=A.histp;
      histpCells=A.histpCells;
      Basic=A.Basic;      
      mode=A.mode;
      voidCard=A.voidCard;
      printNum=A.printNum;
      prdmp=A.prdmp;
      ImpCards=A.ImpCards;
      PhysCards=A.PhysCards;
      LEA=A.LEA;
      sdefCard=A.sdefCard;
      Volume=A.Volume;
    }
  return *this;
}


PhysicsCards::~PhysicsCards()
  /*!
    Destructor
  */
{}

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
  PhysCards.clear();
  Volume.clear();
  sdefCard.clear();
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
  // Helper func ptr for boost since can't resolve type as overloaded:
  void (tallySystem::NList<int>::*fn)(const int&) = 
    &tallySystem::NList<int>::addComp;

  for_each(AL.begin(),AL.end(),
	   boost::bind<void>(fn,boost::ref(histpCells),_1));
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
      Comd.erase(0,pos+4);
      if (!StrFunc::convert(Comd,rndSeed))
        {
	  ELog::EM<<"DBCN [rnd] overflow  == "<<Comd<<ELog::endErr;
	  rndSeed=1234567;
	}
      return 1;
    }

  pos=Comd.find("cut:");
  if (pos!=std::string::npos)
    {
      Comd.erase(0,pos+4);
      PhysCards.push_back(PhysCard("cut"));
      std::string Item;
      if (StrFunc::section(Comd,Item))
        {
	  std::vector<std::string> part;
	  boost::split(part,Item,boost::is_any_of(","));
	  for_each(part.begin(),part.end(),
		   boost::bind(&boost::trim<std::string>,_1,std::locale()));
	  // ensure no empty particles
	  part.erase(remove_if(part.begin(),part.end(),
			       boost::bind<bool>(&std::string::empty,_1)),
		     part.end());
	  PhysCard& PC(PhysCards.back());
	  for_each(part.begin(),part.end(), 
		   boost::bind(&PhysCard::addElm,boost::ref(PC),_1));
	  // Strip and process numbers / j
	  for(size_t i=0;i<5 && !Comd.empty();i++)
	    {
	      double d;
	      if (StrFunc::section(Comd,d))
		PC.setValue(i,d);
	      else if (StrFunc::section(Comd,Item))
	        {
		  if (Item.size()==1 && tolower(Item[0]=='j'))
		    PC.setDef(i);
		}
	    }
	}
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
  for_each(PhysCards.begin(),PhysCards.end(),
	   boost::bind(&PhysCard::setEnergyCut,_1,E));
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
      // remove and continue after catch
      FImp.removeParticle(Particle);
    }
  catch (ColErr::InContainerError<std::string>&)
    { }       
  // Create a new object
  ImpCards.push_back(PhysImp(Type));
  ImpCards.back().addElm(Particle);
  return ImpCards.back();
}


PhysCard&
PhysicsCards::addPhysCard(const std::string& Key,
			  const std::string& Particle)
  /*!
    Adds a particular card [if not already present].
    If a particle is found in the current cards, then it
    is added, if not then a new card is created
    \param Key :: KeyName
    \param Particle :: Particle to add [n,p,e] etc
    \return PhysCard
  */
{
  ELog::RegMethod RegA("PhysicsCards","addPhysCard");
  const std::vector<std::string> PList=
    StrFunc::StrParts(Particle);
  
  std::vector<PhysCard>::iterator vc;
  std::vector<std::string>::const_iterator ac;
  for(vc=PhysCards.begin();vc!=PhysCards.end();vc++)
    if (vc->getKey()==Key)  	// First determine if any key exists:
      {

	ac=find_if(PList.begin(),PList.end(),
		  boost::bind(&PhysCard::hasElm,*vc,_1));
	// If item , add all additional items to the card
	if (ac!=PList.end())
	  {
	    for(ac=PList.begin();ac!=PList.end();ac++)
	      if (!vc->hasElm(*ac))
		vc->addElm(*ac);
	    return *vc;
	  }
      }

  PhysCards.push_back(PhysCard(Key));
  PhysCard& PC=PhysCards.back();
  for(ac=PList.begin();ac!=PList.end();ac++)
    PC.addElm(*ac);
  return PhysCards.back();
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
  if(cellInfo.size()!=impValue.size())
    throw ColErr::MisMatch<size_t>(cellInfo.size(),
				   impValue.size(),
				   "PhysicsCards.setCellNumbers");

  for_each(ImpCards.begin(),ImpCards.end(),
	   boost::bind<void>(&PhysImp::setAllCells,_1,boost::cref(cellInfo),
			     boost::cref(impValue)));

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
  std::vector<PhysImp>::iterator vc;
  vc=find_if(ImpCards.begin(),ImpCards.end(),
	     boost::bind(&PhysImp::isType,_1,Type));
  while(vc!=ImpCards.end())
    {
      vc->setCells(cellInfo,defValue);
      vc++;
      vc=find_if(vc,ImpCards.end(),
		 boost::bind(&PhysImp::isType,_1,Type));
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
  std::vector<PhysImp>::iterator vc;
  vc=find_if(ImpCards.begin(),ImpCards.end(),
	     boost::bind(&PhysImp::isType,_1,Type));
  while(vc!=ImpCards.end())
    {
      vc->setValue(cellID,defValue);
      vc++;
      vc=find_if(vc,ImpCards.end(),
		 boost::bind(&PhysImp::isType,_1,Type));
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
  std::vector<PhysImp>::iterator vc;
  vc=find_if(ImpCards.begin(),ImpCards.end(),
	     boost::bind(&PhysImp::isType,_1,Type));
  while(vc!=ImpCards.end())
    {
      if (vc->hasElm(Particle))
        {
	  vc->setValue(cellID,V);
	  return;
	}
      vc++;
      vc=find_if(vc,ImpCards.end(),
		 boost::bind(&PhysImp::isType,_1,Type));
    }
  throw ColErr::InContainerError<std::string>(Type+"::"+Particle,
				 "PhysicsCars::setCells:");
  return;
}

void
PhysicsCards::removeCell(const int Index)
  /*!
    Remove a particular cell (normally since a cutter cell)
    \param Index :: Cell id to remove
  */
{
  for_each(ImpCards.begin(),ImpCards.end(),
	   boost::bind(&PhysImp::removeCell,_1,Index));
  return;
}

double
PhysicsCards::getValue(const std::string& Type,
		       const std::string& Particle,const int cellID) const
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
PhysicsCards::getPhysImp(const std::string& type,const std::string& particle)
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
  vc=find_if(ImpCards.begin(),ImpCards.end(),
	     boost::bind(&PhysImp::isType,_1,type));
  while(vc!=ImpCards.end())
    {
      if (vc->hasElm(particle))
	return *vc;
      vc++;
      vc=find_if(vc,ImpCards.end(),
		 boost::bind(&PhysImp::isType,_1,type));
    }
  throw ColErr::InContainerError<std::string>(particle,RegA.getFull());
}


const PhysImp&
PhysicsCards::getPhysImp(const std::string& type,const std::string& particle) const
  /*!
    Return the PhysImp for a particle type
    \param type :: type of importance
    \param particle :: particle type
    \throw InContainerError if pType is not found
    \return importance of the cell
  */
{
  std::vector<PhysImp>::const_iterator vc;
  vc=find_if(ImpCards.begin(),ImpCards.end(),
	     boost::bind(&PhysImp::isType,_1,type));
  while(vc!=ImpCards.end())
    {
      if (vc->hasElm(particle))
	return *vc;
      vc++;
      vc=find_if(vc,ImpCards.end(),
		 boost::bind(&PhysImp::isType,_1,type));
    }
  throw ColErr::InContainerError<std::string>(type+particle,"PhysicsCards::getImp const");
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
PhysicsCards::substituteCell(const int oldCell,const int newCell)
  /*!
    Substitute all cells in all physics cards that use cells
    \param oldCell :: old cell number
    \param newCell :: new cell number
   */
{
  sdefCard.substituteCell(oldCell,newCell);
  histpCells.changeItem(oldCell,newCell);
  for_each(ImpCards.begin(),ImpCards.end(),
	  boost::bind(&PhysImp::renumberCell,_1,oldCell,newCell));
  Volume.renumberCell(oldCell,newCell);
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
		    const std::vector<int>& cellOutOrder) const 
  /*!
    Write out each of the cards
    \param OX :: Output stream
    \param cellOutOrder :: Cell List
    \todo Check that histp does not need a line cut.
  */
{
  ELog::RegMethod RegA("PhyiscsCards","write");

  OX<<"dbcn "<<rndSeed<<std::endl;
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
  
  for_each(ImpCards.begin(),ImpCards.end(),
	   boost::bind<void>(&PhysImp::write,_1,boost::ref(OX),
			     boost::ref(cellOutOrder)));

  for_each(PhysCards.begin(),PhysCards.end(),
	   boost::bind<void>(&PhysCard::write,_1,boost::ref(OX)));
  
  for_each(Basic.begin(),Basic.end(),
	   boost::bind2nd(&StrFunc::writeMCNPX,OX));

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

/*
  std::vector<std::string>::const_iterator vc;
  for(vc=Basic.begin();vc!=Basic.end();vc++)
    {
      StrFunc::writeMCNPX(*vc,OX);
    }
*/    
  return;
}


} // NAMESPACE PhysicsCards	
      
   
