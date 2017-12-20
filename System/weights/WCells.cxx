/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weights/WCells.cxx
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
#include "WForm.h"
#include "WItem.h"
#include "WCells.h"


namespace WeightSystem
{

//---------------------------------------------------------------------------
//          WCells 
//---------------------------------------------------------------------------

WCells::WCells() : WForm()
  /*!
    Basic constructor with MCNPX defaults
  */
{}

WCells::WCells(const std::string& pType) :
  WForm(pType)
  /*!
    Basic constructor with MCNPX defaults
    \param pType :: particle type
  */
{}

WCells::WCells(const WCells& A) :
  WForm(A),WVal(A.WVal)
  /*!
    Standard copy constructor
    \param A:: WCells object to copy
    \return *this
  */
{}

WCells&
WCells::operator=(const WCells& A) 
  /*!
    Standard Assignment operator
    \param A:: WCells object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      WForm::operator=(A);
      WVal=A.WVal;
    }
  return *this;
}

WCells::~WCells()
  /*!
    Destructor
  */
{}

bool
WCells::operator==(const WCells& A) const
  /*!
    Operator to determine if although the types
    are different the energy and the parameters
    are the same.
    \param A :: WCells object to compare
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


void 
WCells::populateCells(const std::map<int,MonteCarlo::Qhull*>& Object)
  /*!
    Populate cell types for each object
    \param Object :: The Qhull map to interigate
  */
{
  ELog::RegMethod RegA("WCells","populateCells");
  
  std::map<int,MonteCarlo::Qhull*>::const_iterator vc;
  for(vc=Object.begin();vc!=Object.end();vc++)
    {
      const int cellN(vc->first);
      const MonteCarlo::Qhull& QRef(*vc->second);

      ItemTYPE::iterator ac=WVal.find(cellN);
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
	      WVal.emplace(cellN,
			   WItem(cellN,QRef.getDensity(),QRef.getTemp()));
	    }

	  if (!QRef.getImp())
	    maskCell(cellN);
	}
    }
  return;
}

void
WCells::setCellInfo(const int cellN,const double D,const double T)
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

bool
WCells::isMasked(const int cellN) const
  /*!
    Check if a cell is masked out
    \param cellN :: Cell number
    \return true if masked / false if no or no cell
  */
{
  ItemTYPE::const_iterator vc= WVal.find(cellN);
  if (vc==WVal.end()) return 0;

  return vc->second.isMasked();
}

void
WCells::maskCell(const int cellN)
  /*!
    Mask out a particular cell
    \param cellN :: Cell number
  */
{
  ItemTYPE::iterator vc= WVal.find(cellN);
  if (vc==WVal.end())
    throw ColErr::InContainerError<int>(cellN,"WCells::maskCell");

  vc->second.mask();
  return;
}

void
WCells::maskCellComp(const int cellN,const size_t Index)
  /*!
    Mask out a particular cell
    \param cellN :: Cell number
    \param Index :: Item componenet
  */
{
  ItemTYPE::iterator vc= WVal.find(cellN);
  if (vc==WVal.end())
    throw ColErr::InContainerError<int>(cellN,"WCells::maskCell");

  vc->second.mask(Index);
  return;
}

void
WCells::setWeights(const int cellN,const size_t Index,const double V)
  /*!
    Mask out a particular cell
    \param cellN :: Cell number
    \param Index :: Item componenet
    \param V :: New value
  */
{
  ItemTYPE::iterator vc= WVal.find(cellN);
  if (vc==WVal.end())
    throw ColErr::InContainerError<int>(cellN,"WCells::setWeights");

  vc->second.setWeight(Index,V);
  return;
}

const std::vector<double>&
WCells::getWeights(const int cellN) const
  /*!
    Get weights for individual cell
    \param cellN :: Cell number
    \return Cell Weight
  */
{
  ELog::RegMethod RegA("WCells","getWeights");

  ItemTYPE::const_iterator vc= WVal.find(cellN);
  if (vc==WVal.end())
    throw ColErr::InContainerError<int>(cellN,"cellN");

  return vc->second.getWeight();
}

void
WCells::setWeights(const std::vector<double>& WT)
  /*!
    Sets all the cells to a specific weight.
    \param WT :: Basic weight values to use in all cells
  */
{
  ELog::RegMethod RegA("WCells","setWeights");

  ELog::EM<<"Cell Weighting system: "<<ELog::endDiag
	  <<"  Cells == "<<WVal.size()<<"   Particle = "
	  <<pType<<ELog::endDiag;

  if (WT.size()!=Energy.size())
    throw ColErr::MisMatch<size_t>(WT.size(),Energy.size(),
				     "WCells::setWeights");

  ItemTYPE::iterator vc;
  for(vc=WVal.begin();vc!=WVal.end();vc++)
    {
      vc->second.setWeight(WT);
    }
  return;
}

void
WCells::setWeights(const int CN,const std::vector<double>& WT)
  /*!
    Sets all the cells to a specific weight
    \param CN :: Cell number 
    \param WT :: Basic weight values to use in all cells
  */
{
  ELog::RegMethod RegA("WCells","setWeights");

  if (WT.size()!=Energy.size())
    throw ColErr::MisMatch<size_t>(WT.size(),Energy.size(),
				     "WCells::setWeights");

  ItemTYPE::iterator vc;
  for(vc=WVal.begin();vc!=WVal.end() &&
	vc->second.getCellNumber()!=CN;vc++) ;
  if (vc!=WVal.end())
    vc->second.setWeight(WT);
  else
    {
      ELog::EM<<"Cell Number not found "<<CN<<ELog::endErr;
      throw ColErr::ExitAbort(RegA.getFull());
    }
  return;
}

void
WCells::scaleWeights(const int CN,const std::vector<double>& WT)
  /*!
    Sets all the cells to a specific weight
    \param CN :: Cell number 
    \param WT :: Basic weight values toscale in all cells
  */
{
  ELog::RegMethod RegA("WCells","scaleWeights");

  if (WT.size()!=Energy.size())
    throw ColErr::MisMatch<size_t>(WT.size(),Energy.size(),
				     "WCells::setWeights");

  ItemTYPE::iterator vc;
  for(vc=WVal.begin();vc!=WVal.end() &&
	vc->second.getCellNumber()!=CN;vc++) ;
  if (vc==WVal.end())
    throw ColErr::InContainerError<int>(CN,"Cell number");

  vc->second.scaleWeight(WT);
  return;
}

void
WCells::scaleWeights(const int CN,const double& WT) 
  /*!
    Sets all the cells to a specific weight
    \param CN :: Cell number 
    \param WT :: Single value to scale all cells
  */
{
  ELog::RegMethod RegA("WCells","scaleWeights");

  ItemTYPE::iterator vc;
  for(vc=WVal.begin();vc!=WVal.end() &&
	vc->second.getCellNumber()!=CN;vc++) ;
  if (vc==WVal.end())
    throw ColErr::InContainerError<int>(CN,"Cell number");

  vc->second.scaleWeight(WT);
  return;
}


void
WCells::rescale(const double TCut,const double SF)
  /*!
    Scales all the cells by a scale factor.
    \param TCut :: Temperature to scale 
    \param SF :: Scale factor
  */

{
  ELog::RegMethod RegA("WCells","rescale");

  ItemTYPE::iterator vc;
  for(vc=WVal.begin();vc!=WVal.end();vc++)
    vc->second.rescale(TCut,SF);

  return;
}

void
WCells::rescale(const int cA,const int cB,const double SF)
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
WCells::balanceScale(const std::vector<double>& W)
  /*!
    Rebalance weights
    \param W :: Weights to use
   */
{
  setWeights(W);
  rescale(200.0,10.0);  // temperature rescale
  return;
}

void
WCells::renumberCell(const int oldIndex,const int newIndex)
  /*!
    Renumber all the cells. No checking or error report 
    on missing cell
    \param oldIndex :: oldIndex 
    \param newIndex :: newValue
  */
{
  ELog::RegMethod RegA("WCells","renumberCell");
  if (newIndex!=oldIndex)
    {
      ItemTYPE::iterator mc=WVal.find(oldIndex);
      if (mc!=WVal.end())
	{
	  ItemTYPE::const_iterator newmc=WVal.find(newIndex);
	  mc->second.setCellNumber(newIndex);
	  if (newmc!=WVal.end())
	    {
	      ELog::EM<<"New point found "<<oldIndex<<" "<<newIndex<<
		ELog::endCrit;
	      ELog::EM<<"Mc == "<<newmc->first<<" "
		      <<mc->second.getCellNumber()<<ELog::endErr;
	    }
	  WVal.insert(ItemTYPE::value_type(newIndex,mc->second));
	  WVal.erase(mc);
	}
      mc=WVal.find(newIndex);
    }
  return;
}

void
WCells::writeTable(std::ostream& OX) const
  /*!
    Writes out the table of objects and associated 
    weights
    \param OX :: Output stream
  */
{
  std::ostringstream cx;
  cx<<"# ";
  for(int i=0;i<static_cast<int>(Energy.size());i++)
    cx<<"  wwn"<<i+1<<":"<<pType;
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
WCells::writeHead(std::ostream& OX) const
  /*!
    Write out the header section from the file
    \param OX :: Output stream
  */
{
  std::ostringstream cx;
  cx<<"wwe:"<<pType<<" ";
  copy(Energy.begin(),Energy.end(),std::ostream_iterator<double>(cx," "));
  StrFunc::writeMCNPX(cx.str(),OX);

  if (activeWWP)
    {
      cx.str("");  
      cx<<"wwp:"<<pType<<" ";
      cx<<wupn<<" "<<wsurv<<" "<<maxsp<<" "<<mwhere<<" 0 "<<mtime;
      StrFunc::writeMCNPX(cx.str(),OX);
    }
  return;
}

void 
WCells::writePHITSHead(std::ostream& OX) const
  /*!
    Write out the header section from the file
    \param OX :: Output stream
  */
{
  OX<<" mwhere      = "<<mwhere<<std::endl;
  OX<<" wupn        = "<<wupn<<std::endl;
  OX<<" wsurvn      = "<<wsurv<<std::endl;
  OX<<" maxspln     = "<<maxsp<<std::endl;
  return;
}


void
WCells::writePHITS(std::ostream& OX) const
  /*!
    Generic write output
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("WCells","writePHITS");

  for(const ItemTYPE::value_type& wf : WVal)
    {
      if (!wf.second.isNegative())
	wf.second.write(OX);
    }

  return;
}

void
WCells::write(std::ostream& OX) const
  /*!
    Generic write output
    \param OX :: Output stream
  */
{
  writeHead(OX);
  writeTable(OX);
  return;
}

}   // NAMESPACE WeightSystem
