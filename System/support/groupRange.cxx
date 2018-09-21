#include <iostream>
#include <cmath>
#include <complex>
#include <vector>
#include <limits>

#include "Exception.h"
#include "mathSupport.h"
#include "groupRange.h"


std::ostream&
operator<<(std::ostream& OX,const groupRange& A)
  /*!
    Standard stream operator
    \param OX :: output stream
    \param A :: groupRange item
   */
{
  A.write(OX);
  return OX;
}

groupRange::groupRange() 
  /*!
    Constructor [empty]
  */
{}

groupRange::groupRange(const int V) :
  LowUnit({V}),HighUnit({V})
  /*!
    Constructor
    \param V :: Single Value to set 
  */
{}

groupRange::groupRange(const std::vector<int>& AVec) 
  /*!
    Constructor
    \param AVec :: Vect to set
  */
{
  for(const int I : AVec)
    addItem(I);
  return;
}

groupRange::groupRange(const int LV,const int HV) :
  LowUnit(std::min(LV,HV)),HighUnit(std::max(LV,HV))
  /*!
    Constructor
    \param LV :: Low values [not check]
    \param HV :: High values [not check]
  */
{}

groupRange::groupRange(const groupRange& A) : 
  LowUnit(A.LowUnit),HighUnit(A.HighUnit)
  /*!
    Copy constructor
    \param A :: groupRange to copy
  */
{}

groupRange&
groupRange::operator=(const groupRange& A)
  /*!
    Assignment operator
    \param A :: groupRange to copy
    \return *this
  */
{
  if (this!=&A)
    {
      LowUnit=A.LowUnit;
      HighUnit=A.HighUnit;
    }
  return *this;
}

size_t
groupRange::validIndex(const int V) const
  /*!
    Simple determination if V is with the range
    throw if not and then return the index
    \param V :: Value to check
    \return index
   */
{
  const size_t out=valueIndex(V);
  if (out>LowUnit.size())
    throw ColErr::InContainerError<int>(V,"V not in group index");
  return out;
}

size_t
groupRange::valueIndex(const int V) const
  /*!
    Simple determination if V is with the range
    \param V :: Value to check
    \return index or > UnitLow.size()
  */
{
  if (!LowUnit.empty())
    {
      std::vector<int>::const_iterator 
	xV=lower_bound(LowUnit.begin(),LowUnit.end(),V);
      if (xV!=LowUnit.end())
	{
	  if (V >= *xV)
	    {
	      const size_t index=
		static_cast<size_t>(distance(LowUnit.cbegin(),xV));
	      if (V <= HighUnit[index])
		return index;
	    }
	}
      else if ( V<=HighUnit.back())
	return HighUnit.size()-1;
    }
  return std::numeric_limits<size_t>::max();
}

bool
groupRange::valid(const int V) const
  /*!
    Simple determination if V is with the range
    \param V :: Value to check
    \return true if within range
   */
{
  std::vector<int>::const_iterator 
    xV=lower_bound(LowUnit.begin(),LowUnit.end(),V);
  if (xV==LowUnit.end() ||  V < *xV) return 0;
  const size_t index=static_cast<size_t>(distance(LowUnit.begin(),xV));
  return  (V > HighUnit[index] ) ? 0 : 1;
}


void
groupRange::merge()
  /*!
    Merge units that overlap
  */
{
  if (LowUnit.empty()) return;
  // First index sort the list
  pairSort(LowUnit,HighUnit);

  std::vector<int> lOut;
  std::vector<int> hOut;
  
  lOut.push_back(LowUnit.front());
  hOut.push_back(HighUnit.front());
  for(size_t index=1;index<LowUnit.size();index++)
    {
      const int LV(LowUnit[index]);
      const int HV(HighUnit[index]);
      if (LV<=hOut.back()+1 && HV>hOut.back())
	hOut.back()=HV;
      else 
	{
	  lOut.push_back(LV);
	  hOut.push_back(HV);
	}

    }
  LowUnit=lOut;
  HighUnit=hOut;
  return;
}

groupRange&
groupRange::combine(const groupRange& A) 
  /*!
    Combine the biggest mixed range
    \param A :: groupRange to combine
    \return new groupRange
  */
{
  std::vector<int> lOut;
  std::vector<int> hOut;
  size_t indexA(0);
  size_t indexB(0);

  while(indexA< LowUnit.size() && indexB< A.LowUnit.size())
    {
      const int ALV(LowUnit[indexA]);
      const int BLV(A.LowUnit[indexB]);
      const int AHV(HighUnit[indexA]);
      const int BHV(A.HighUnit[indexB]);
      
      if (ALV<=BLV)
	{
	  lOut.push_back(ALV);
	  if (BLV<=AHV)  // combinination possible
	    {
	      hOut.push_back(std::max(AHV,BHV));
	      indexA++;
	      indexB++;
	    }
	  else
	    {
	      hOut.push_back(AHV);
	      indexA++;
	    }
	}
      else   // reverse of above [can be made tidy?]
	{
	  lOut.push_back(BLV);
	  if (ALV<=BHV)  // combinination possible
	    {
	      hOut.push_back(std::max(AHV,BHV));
	      indexA++;
	      indexB++;
	    }
	  else
	    {
	      hOut.push_back(BHV);
	      indexB++;
	    }
	}
    }
  for(;indexA<LowUnit.size();indexA++)
    {
      lOut.push_back(LowUnit[indexA]);
      hOut.push_back(HighUnit[indexA]);
    }
  for(;indexB<A.LowUnit.size();indexB++)
    {
      lOut.push_back(A.LowUnit[indexB]);
      hOut.push_back(A.HighUnit[indexB]);
    }

  // now do cross-merge
  LowUnit=lOut;
  HighUnit=hOut;
  merge();
  return *this;
}

void
groupRange::setItems(const std::vector<int>& AVec)
  /*!
    Set the system to be a vector
    \param AVec :: Unordered vector
  */
{
  LowUnit.clear();
  HighUnit.clear();
  for(const int I : AVec)
    addItem(I);
  return;
}

void
groupRange::addItem(const std::vector<int>& AVec)
  /*!
    Add an item if required [uses merge for efficiency]
    \param AVec :: Item to add
  */
{
  groupRange A(AVec);
  this->combine(A);
  return;
}

void
groupRange::addItem(const int A)
  /*!
    Add an item if required
    \param A :: Item to remove
  */
{
  if (valid(A)) return;

  const size_t indexM=valueIndex(A-1);
  const size_t indexP=valueIndex(A+1);
  // special case exactly between two units
  if (indexP<LowUnit.size() && indexM<LowUnit.size())
    {
      HighUnit[indexM]=HighUnit[indexP];
      LowUnit.erase(LowUnit.begin()+indexP);
      HighUnit.erase(HighUnit.begin()+indexP);
      return;
    }
  
  // special case one below a unit:
  if (indexP<LowUnit.size())
    {
      LowUnit[indexP]--;
      return;
    }
  // special case one above a unit:
  if (indexM<LowUnit.size())
    {
      HighUnit[indexM]++;
      return;
    }
  
  // create a new unit:
  std::vector<int>::const_iterator 
    xV=lower_bound(LowUnit.begin(),LowUnit.end(),A);
  const long int offset=distance(LowUnit.cbegin(),xV);
  LowUnit.insert(xV,A);
  HighUnit.insert(HighUnit.begin()+offset,A);
  return;
}

void
groupRange::removeItem(const int A)
  /*!
    Remove an item if possible
    \param A :: Item to remove
  */
{
  const size_t index=valueIndex(A);
  if (index>=LowUnit.size()) return;

  // case 1: short unit to be deleted
  if (LowUnit[index]==HighUnit[index])
    {
      LowUnit.erase(LowUnit.begin()+index);
      HighUnit.erase(HighUnit.begin()+index);
      return;
    }

  // case 2: at front 
  if (LowUnit[index]==A)
    {
      LowUnit[index]++;
      return;
    }

  // case 3: at back
  if (HighUnit[index]==A)
    {
      HighUnit[index]--;
      return;
    }
  // splitting required
  const int HL=HighUnit[index];
  HighUnit[index]=A-1;
  LowUnit.insert(LowUnit.begin(),A+1);
  HighUnit.insert(HighUnit.begin(),HL);
  return;
}

void
groupRange::move(const int A,const int B)
  /*!
    Move A to B: 
    Note A MUST exist and B MUST NOT exist
    \param A :: A
  */
{ 
  if (!valid(A))
    throw ColErr::InContainerError<int>
      (A,"groupRange::Move:A not in group index");

  if (valid(B))
    throw ColErr::InContainerError<int>
      (B,"groupRange::Move:B value already present");
  
  removeItem(A);
  addItem(B);
  return;
}

std::vector<int>
groupRange::getAllCells() const
  /*!
    Fill a vector with all cells
    \return Vector of all cells
   */
{
  std::vector<int> Out;
  for(size_t i=0;i<LowUnit.size();i++)
    {
      int LA(LowUnit[i]);
      const int HA(HighUnit[i]);
      for(;LA<=HA;LA++)
	Out.push_back(LA);
    }
  return Out;
}

int
groupRange::getFirst() const
  /*!
    Get the first valid cell
    \return Vector of all cells
   */
{
  if (LowUnit.empty())
    throw ColErr::EmptyContainer("groupRange");
  return LowUnit.front();
}

int
groupRange::getLast() const
  /*!
    Fill a vector with all cells
    \return Vector of all cells
   */
{
  if (HighUnit.empty())
    throw ColErr::EmptyContainer("groupRange");
  return HighUnit.back();
}

int
groupRange::getNext(const int cellNV) const
  /*!
    Returns the next valid cell after cellN
    \param cellNV :: Cell number
    \throw RangeError if cellN exceeds last index
  */
{
  if (LowUnit.empty())
    throw ColErr::RangeError<int>(cellNV,0,0,"CellN out of groupRange");

  const ColErr::RangeError<int> ErrorOut
    (cellNV,LowUnit.front(),HighUnit.back(),"CellN out of groupRange");
  if (HighUnit.back()<=cellNV)
    throw ErrorOut;
  if (LowUnit.front()>cellNV)
    return LowUnit.front();
  
  const int cellPlus(cellNV+1);

  std::vector<int>::const_iterator 
    xV=lower_bound(LowUnit.begin(),LowUnit.end(),cellPlus);
  if (xV!=LowUnit.end())
    {
      if (cellPlus == *xV)
	return *xV;
      
      // xV CANNOT be begin, must be outer.
      const size_t index=
	static_cast<size_t>(distance(LowUnit.cbegin(),xV))-1;
      return (cellPlus > HighUnit[index]) ? *xV : cellPlus;
    }      
  else if ( cellNV<HighUnit.back())  // special case for outer unit
    {
      return cellPlus;
    }
  throw ErrorOut;
}

void
groupRange::write(std::ostream& OX) const
  /*!
    Output function
    \param OX :: Output stream
   */
{
  for(size_t i=0;i<LowUnit.size();i++)
    {
      const int LA(LowUnit[i]);
      const int HA(HighUnit[i]);
      if (LA==HA)
	OX<<LA<<" ";
      else
	OX<<"["<<LA<<" "<<HA<<"] ";
    }
  return;
}

