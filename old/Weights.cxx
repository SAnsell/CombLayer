/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/Weights.cxx
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
#include <iomanip>
#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <iterator>
#include <functional>
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/format.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Transform.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Weights.h"

namespace WeightSystem
{

WItem::WItem(const int N) :
  cellN(N),Density(0),Tmp(300)
  /*!
    Constructor  (sets default temperature)
    \param N :: Cell number 
  */
{}

WItem::WItem(const int N,const double D,const double T) :
  cellN(N),Density(D),Tmp(T)
  /*!
    Constructor 
    \param N :: Cell number 
    \param D :: Density [atom/A^3]
    \param T :: Temperature [kelvin]
  */
{}

WItem::WItem(const WItem& A) :
  cellN(A.cellN),Density(A.Density),Tmp(A.Tmp)
  /*!
    Copy Constructor
    \param A :: Item to copy
  */
{}

WItem&
WItem::operator=(const WItem& A) 
  /*!
    Copy Constructor
    \param A :: Item to copy
    \return *this
  */
{
  if (this!=&A)
    {
      cellN=A.cellN;
      Density=A.Density;
      Tmp=A.Tmp;
    }
  return *this;
}

WItem::~WItem()
  /*!
    Destructor
  */
{}


void
WItem::mask()
  /*!
    Mask out a cell (assumes that it has already
    had the size of Val set.
  */
{
  fill(Val.begin(),Val.end(),-1);
  return;
}

void
WItem::mask(const int Index)
  /*!
    Mask out particluar component on a a cell.
    \param Index :: Item to use on mask
  */
{
  if (Index<0 || Index>=static_cast<int>(Val.size()))
    throw ColErr::IndexError<long int>(Index,Val.size(),"WItem::mask");
  Val[Index]=-1;
  return;
}

void
WItem::setWeight(const std::vector<double>& WT)
  /*!
    Sets all the cells to a specific weight
    \param WT :: New weights (size is already checked)
  */
{
  Val=WT;
  return;
}

void
WItem::setWeight(const int Index,const double V)
  /*!
    Sets all the cells to a specific weight
    \param Index :: Index of the weight to set
    \param V :: Value to set 
  */
{
  if (Index<0 || Index>=static_cast<int>(Val.size()))
    throw ColErr::IndexError<long int>(Index,Val.size(),"WItem::setWeight");
  Val[Index]=V;
  return;
}

void
WItem::rescale(const double Tscale,const double SF)
  /*!
    Sets all the cells to a specific weight.
    Divides the weight by 10 if temperature below Tscale
    \param Tscale :: Temperature to scale 
    \param SF :: Scale factor
  */
{
  if (Tmp<Tscale)
    {
      transform(Val.begin(),Val.end(),Val.begin(),
		std::bind2nd(std::divides<double>(),SF));
    }
  return;
}

void
WItem::rescale(const int cA,const int cB,const double SF)
  /*!
    Scales all the cells by a scale factor.
    If the cell nubmer is between cA and cB
    \param cA :: Start cell number 
    \param cB :: End cell number 
    \param SF :: Scale factor
  */
{
  if (cellN>=cA && cellN<=cB) 
    {
      transform(Val.begin(),Val.end(),Val.begin(),
		std::bind2nd(std::divides<double>(),SF));
    }
  return;
}


void
WItem::write(std::ostream& OX) const
  /*!
    Writes out an individual cell weight item
    \param OX :: Output stream
  */
{
  boost::format FMT("%1$5.2e ");
  std::ostringstream cx;
  cx<<cellN<<"   ";

  std::vector<double>::const_iterator vc;
  for(vc=Val.begin();vc!=Val.end();vc++)
    cx<<FMT % (*vc);

  StrFunc::writeMCNPX(cx.str(),OX);
  return;
}

//---------------------------------------------------------------------------
//          WControl 
//---------------------------------------------------------------------------

WControl::WControl() :
  ptype('n'),
  wupn(2.0),wsurv(0.7*wupn),maxsp(5),
  mwhere(0),mtime(0)
  /*!
    Basic constructor with MCNPX defaults
  */
{}

WControl::WControl(const WControl& A) :
  ptype(A.ptype),Energy(A.Energy),
  wupn(A.wupn),wsurv(A.wsurv),maxsp(A.maxsp),
  mwhere(A.mwhere),mtime(A.mtime),WVal(A.WVal)
  /*!
    Standard copy constructor
    \param A:: WControl object to copy
    \return *this
  */
{}

WControl&
WControl::operator=(const WControl& A) 
  /*!
    Standard Assignment operator
    \param A:: WControl object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      ptype=A.ptype;
      Energy=A.Energy;
      
      wupn=A.wupn;
      wsurv=A.wsurv;
      maxsp=A.maxsp;
      mwhere=A.mwhere;
      mtime=A.mtime;

      WVal=A.WVal;
    }
  return *this;
}

WControl::~WControl()
  /*!
    Destructor
  */
{}

bool
WControl::operator==(const WControl& A) const
  /*!
    Operator to determine if although the types
    are different the energy and the parameters
    are the same.
    \param A :: WControl object to compare
    \return A == This
  */
{
  if (A.Energy.size()!=Energy.size())
    return 0;
  if (!equal(Energy.begin(),Energy.end(),A.Energy.begin()))
    return 0;
  if (wupn!=A.wupn ||
      wsurv!=A.wsurv ||
      maxsp!=A.maxsp ||
      mwhere!=A.mwhere ||
      mtime!=A.mtime)
    return 0;
  // Everthing equal
  return 1;
}

int
WControl::setParam(const double WP,const double WS,
		   const int MS,const int WC,
		   const int MT)
  /*!
    Sets the Control parameters
    \param WP :: up-split weight
    \param WS :: distruction weight
    \param MS :: Max number of splits
    \param WC :: Where to check
    \param MT :: Time / Energy
    \return 0 on succes and 
  */
{
  if (WP<2 || WS<1 || WS>=WP ||
      MS<=1 || WC<-1 || WC>1 || (MT!=0 && MT!=1))
    {
      std::cerr<<"Error with WControl paramerter"<<std::endl;
      return -1;
    }
  wupn=WP;
  wsurv=WS;
  maxsp=MS;
  mwhere=WC;
  mtime=MT;
    
  return 0;
}

void 
WControl::populateCells(const std::map<int,MonteCarlo::Qhull*>& Object)
  /*!
    Populate cell types for each object
    \param Object :: The Qhull map to interigate
  */
{
  std::map<int,MonteCarlo::Qhull*>::const_iterator vc;
  for(vc=Object.begin();vc!=Object.end();vc++)
    {
      ItemTYPE::iterator ac;
      const int cellN(vc->first);
      const MonteCarlo::Qhull& QRef(*vc->second);

      ac=WVal.find(cellN);
      if (QRef.isPlaceHold())      // IF virtual remove
        {
	  if (ac!=WVal.end())
	    WVal.erase(ac);
	}
      else         // Good particle
        {
	  if (ac!=WVal.end())
	    {
	      ac->second=WItem(cellN,QRef.getDensity(),QRef.getTemp());
	    }
	  else
	    {
	      WVal.insert(ItemTYPE::value_type(cellN,
		  WItem(cellN,QRef.getDensity(),QRef.getTemp())));
	    }
	}
    }
  return;
}

void
WControl::setCellInfo(const int cellN,const double D,const double T)
  /*!
    Sets the cell number, the density and the Temperature.
    \param cellN :: Cell number (created if not existing)
    \param D :: density [atoms/A^3]
    \param T :: Temperature [kelvin]
  */
{
  ItemTYPE::iterator vc= WVal.find(cellN);
  if (vc==WVal.end())
    {
      WVal.insert(ItemTYPE::value_type(cellN,WItem(cellN,D,T))); 
    }
  else
    {
      vc->second.setDensity(D);
      vc->second.setTemp(T);
    }

  return;
}

void
WControl::maskCell(const int cellN)
  /*!
    Mask out a particular cell
   \param cellN :: Cell number
  */
{
  ItemTYPE::iterator vc= WVal.find(cellN);
  if (vc==WVal.end())
    throw ColErr::InContainerError<int>(cellN,"WControl::maskCell");

  vc->second.mask();
  return;
}

void
WControl::maskCellComp(const int cellN,const int Index)
  /*!
    Mask out a particular cell
   \param cellN :: Cell number
   \param Index :: Item componenet
  */
{
  ItemTYPE::iterator vc= WVal.find(cellN);
  if (vc==WVal.end())
    throw ColErr::InContainerError<int>(cellN,"WControl::maskCell");

  vc->second.mask(Index);
  return;
}

void
WControl::setWeights(const int cellN,const int Index,const double V)
  /*!
    Mask out a particular cell
    \param cellN :: Cell number
    \param Index :: Item componenet
    \param V :: New value
  */
{
  ItemTYPE::iterator vc= WVal.find(cellN);
  if (vc==WVal.end())
    throw ColErr::InContainerError<int>(cellN,"WControl::maskCell");

  vc->second.setWeight(Index,V);
  return;
}

void
WControl::setWeights(const std::vector<double>& WT)
  /*!
    Sets all the cells to a specific weight.
    \param WT :: Basic weight values to use in all cells
  */
{
  ELog::RegMethod RegA("WControl","setWeights");
  ELog::EM<<"WVal size == "<<WVal.size()<<" "<<ptype<<ELog::endDiag;

  if (WT.size()!=Energy.size())
    throw ColErr::MisMatch<long int>(WT.size(),Energy.size(),
				     "WControl::setWeights");

  ItemTYPE::iterator vc;
  for(vc=WVal.begin();vc!=WVal.end();vc++)
    {
      vc->second.setWeight(WT);
    }
  return;
}

void
WControl::setWeights(const int CN,const std::vector<double>& WT)
  /*!
    Sets all the cells to a specific weight
    \param CN :: Cell number 
    \param WT :: Basic weight values to use in all cells
  */
{
  ELog::RegMethod RegA("WControl","setWeights");

  if (WT.size()!=Energy.size())
    throw ColErr::MisMatch<long int>(WT.size(),Energy.size(),
				     "WControl::setWeights");
  ItemTYPE::iterator vc;
  for(vc=WVal.begin();vc!=WVal.end() &&
	vc->second.getCellNumber()!=CN;vc++) ;
  if (vc!=WVal.end())
    vc->second.setWeight(WT);
  else
    {
      ELog::EM<<"Cell Number not found "<<CN<<ELog::endErr;
      exit(1);
    }
  return;
}

void
WControl::rescale(const double TCut,const double SF)
  /*!
    Scales all the cells by a scale factor.
    \param TCut :: Temperature to scale 
    \param SF :: Scale factor
  */

{
  ItemTYPE::iterator vc;
  for(vc=WVal.begin();vc!=WVal.end();vc++)
    {
      vc->second.rescale(TCut,SF);
    }
  return;
}

void
WControl::rescale(const int cA,const int cB,const double SF)
  /*!
    Scales all the cells by a scale factor.
    If the cell nubmer is between cA and cB
    \param cA :: lowest cell number to scale
    \param cB :: highest cell number to scale
    \param SF :: Scale factor
  */
{
  ItemTYPE::iterator vc;
  for(vc=WVal.begin();vc!=WVal.end();vc++)
    {
      vc->second.rescale(cA,cB,SF);
    }
  return;
}

void
WControl::renumber(const int oldIndex,const int newIndex)
  /*!
    Renumber all the cells. No checking or error report 
    on missing cell
    \param oldIndex :: oldIndex 
    \param newIndex :: newValue
  */
{
  ItemTYPE::iterator mc=WVal.find(oldIndex);
  if (mc!=WVal.end())
    mc->second.setCellNumber(newIndex);
  return;
}


void
WControl::setEnergy(const std::vector<double>& E)
  /*!
    Sets the energy list
    \param E :: Energy vector (already sorted)
  */
{
  Energy=E;
  return;
}

void
WControl::writeTable(std::ostream& OX) const
  /*!
    Writes out the table of objects and associated 
    weights
    \param OX :: Output stream
  */
{
  std::ostringstream cx;
  cx<<"# ";
  for(int i=0;i<static_cast<int>(Energy.size());i++)
    cx<<"  wwn"<<i+1<<":"<<ptype;
  StrFunc::writeMCNPX(cx.str(),OX);
  // This line avoids a stupid bug.
  cx.str("");   // THIS IS A GCC BUG since cx.str("c ") DOES NOT WORK
  cx<<"c   ";
  for(int i=0;i<static_cast<int>(Energy.size());i++)
    cx<<"c        ";
  StrFunc::writeMCNPX(cx.str(),OX);

  ItemTYPE::const_iterator vc;
  for(vc=WVal.begin();vc!=WVal.end();vc++)
    {
      vc->second.write(OX);
    }
  return;
}
      
void 
WControl::writeHead(std::ostream& OX) const
  /*!
    Write out the header section from the file
    \param OX :: Output stream
  */
{
  std::ostringstream cx;
  cx<<"wwe:"<<ptype<<" ";
  copy(Energy.begin(),Energy.end(),std::ostream_iterator<double>(cx," "));
  StrFunc::writeMCNPX(cx.str(),OX);
  
  cx.str("");  
  cx<<"wwp:"<<ptype<<" ";
  cx<<wupn<<" "<<wsurv<<" "<<maxsp<<" "<<mwhere<<" 0 "<<mtime;
  StrFunc::writeMCNPX(cx.str(),OX);
  return;
}

//---------------------------------------------------------------------------
//          WEIGHTS 
//---------------------------------------------------------------------------

Weights::Weights() :
  Ngrp(0)
  /*!
    Constructor
  */
{}

Weights::Weights(const Weights& A) :
  Ngrp(A.Ngrp),WC(A.WC)
  /*!
    Copy Constructor
    \param A :: Object to copy
  */
{}

Weights&
Weights::operator=(const Weights& A)
  /*!
    Assignment operator
    \param A :: Weights Item to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Ngrp=A.Ngrp;
      WC=A.WC;
    }
  return *this;
}

Weights::~Weights()
  /*!
    Destructor
  */
{}

void
Weights::clear()
  /*!
    Clear everything for a new read
  */
{
  Ngrp=0;
  WC.erase(WC.begin(),WC.end());
  return;
}

void
Weights::addParticle(const char A) 
  /*!
    Adds a particle type (if not already in the set)
    \param A :: Name of the particle
  */
{
  std::map<char,WControl>::const_iterator vc;
  vc=WC.find(A);
  if (vc==WC.end())
    {
      WC.insert(CtrlTYPE::value_type(A,WControl()));
      Ngrp++;
    }
  return;
} 

void 
Weights::setEnergy(const char A,const std::vector<double>& E)
  /*!
    Sets the energy distribution
    \param A :: name of transported particle
    \param E :: Energy list
  */
{
  CtrlTYPE::iterator vc;
  vc=WC.find(A);
  if (vc==WC.end())
    throw ColErr::InContainerError<char>(A,"Weights::setEnergy");

  vc->second.setEnergy(E);
  return;
}

void 
Weights::populateCells(const std::map<int,MonteCarlo::Qhull*>& Object)
  /*!
    Populate cell types for each object
    \param Object :: The complete Qhull map from Simulation
  */
{
  CtrlTYPE::iterator vc;             // set of WControls
  for(vc=WC.begin();vc!=WC.end();vc++) 
    {
       vc->second.populateCells(Object);
    }
  return;
}


void
Weights::setCell(const char T,const int CN,
		 const std::vector<double>& Values)
  /*!
    Sets a single cell weight
    \param T :: Type
    \param CN :: Cell number
    \param Values :: Weight values
  */
{
  CtrlTYPE::iterator vc;
  vc=WC.find(T);
  if (vc==WC.end())
    throw ColErr::InContainerError<char>(T,"Weights::setCell");
  
  vc->second.setWeights(CN,Values);
  return;
}

void
Weights::maskCell(const char T,const int CN)
  /*!
    Makes a cell -ve weight (importance 0)
    for the given cell
    \param T :: particle type 
    \param CN :: Cell number
  */
{
  CtrlTYPE::iterator vc;
  vc=WC.find(T);
  if (vc==WC.end())
    throw ColErr::InContainerError<char>(T,"Weights::maskCell");

  vc->second.maskCell(CN);
  return;
}

void
Weights::maskCellComp(const char T,const int CN,const int Index)
  /*!
    Makes a cell -ve weight (importance 0)
    for the given cell
    \param T :: particle type 
    \param CN :: Cell number
    \param Index :: Item to set
  */
{
  CtrlTYPE::iterator vc;
  vc=WC.find(T);
  if (vc==WC.end())
    throw ColErr::InContainerError<char>(T,"Weights::maskCell");

  vc->second.maskCellComp(CN,Index);
  return;
}

void
Weights::setWeights(const char T,const int CN,const int Index,const double V)
  /*!
    Makes a cell index V weight 
    for the given cell
    \param T :: particle type 
    \param CN :: Cell number
    \param Index :: Item to set
    \param V :: Value to used
  */
{
  CtrlTYPE::iterator vc;
  vc=WC.find(T);
  if (vc==WC.end())
    throw ColErr::InContainerError<char>(T,"Weights::maskCell");

  vc->second.setWeights(CN,Index,V);
  return;
}

void
Weights::setWeights(const char T,const int CN,const std::vector<double>& WVec)
  /*!
    Makes a cell index V weight 
    for the given cell
    \param T :: particle type 
    \param CN :: Cell number
    \param WVec :: Weights vector
  */
{
  CtrlTYPE::iterator vc;
  vc=WC.find(T);
  if (vc==WC.end())
    throw ColErr::InContainerError<char>(T,"Weights::maskCell");

  vc->second.setWeights(CN,WVec);
  return;
}

void
Weights::balanceScale(const char T,const std::vector<double>& W)
  /*!
    Scales the weight in the system 
    \param T :: particle type
    \param W :: list of values
  */
{
  CtrlTYPE::iterator vc;
  vc=WC.find(T);
  if (vc==WC.end())
    throw ColErr::InContainerError<char>(T,"Weights::balanceScale");

  vc->second.setWeights(W);
  vc->second.rescale(200,10.0);
  return;

}

void
Weights::renumber(const int oldIndex,const int newIndex)
  /*!
    Renumber all the cells. No checking or error report 
    on missing cell
    \param oldIndex :: oldIndex 
    \param newIndex :: newValue
  */
{
  CtrlTYPE::iterator mc;
  for(mc=WC.begin();mc!=WC.end();mc++)
    mc->second.renumber(oldIndex,newIndex);
  return;
}

void 
Weights::rescale(const char T,const int cA,const int cB,const double SF)
  /*!
    Rescale a numerical block of weights
    \param T :: Charater of particle to scale
    \param cA :: Start of blcok
    \param cB :: End of blcok
    \param SF :: ScaleFactor
    \todo Make T a string to deal with multi-char names
  */
{
  CtrlTYPE::iterator vc;
  vc=WC.find(T);
  if (vc==WC.end())
    throw ColErr::InContainerError<char>(T,"Weights::rescale");

  vc->second.rescale(cA,cB,SF);
  return;
}

void
Weights::write(std::ostream& OX) const
  /*!
    Write out the system in MCNPX output format
    \param OX :: Output stream
  */
{
  CtrlTYPE::const_iterator wc;
  for(wc=WC.begin();wc!=WC.end();wc++)
    {
      wc->second.writeHead(OX);      
      wc->second.writeTable(OX);
    }
  
  return;
}

}   // NAMESPACE WeightSystem
