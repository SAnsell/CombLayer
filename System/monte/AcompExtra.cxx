/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monte/AcompExtra.cxx
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
#include <climits>
#include <cmath>
#include <vector>
#include <set>
#include <map>
#include <stack>
#include <string>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <functional>
#include <iterator>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "support.h"
#include "stringCombine.h"
#include "mathSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "RotCounter.h"
#include "BnId.h"

#include "AcompTools.h"
#include "Acomp.h"

namespace MonteCarlo 
{

Acomp
Acomp::expandIII(const Acomp& A) const
  /*!
    Addition to an intersection unit with an intersection via UNIION
    \param A :: Other object to add
    \return A
   */
{
  Acomp Out(1);   // Intersection
  Out.Units.insert(Out.Units.end(),Units.begin(),Units.end());
  Out.Comp.insert(Out.Comp.end(),Comp.begin(),Comp.end());

  Out.Units.insert(Out.Units.end(),A.Units.begin(),A.Units.end());
  Out.Comp.insert(Out.Comp.end(),A.Comp.begin(),A.Comp.end());

  return Out;
}

Acomp
Acomp::expandIIU(const Acomp& A) const
  /*!
    Addition to an intersection unit with an Union via intersection
    \param A :: Other object to add
    \return A
   */
{	
  Acomp Out(0);     // union
  for(const int AI : A.Units)
    {
      Acomp addItem=interCombine(AI,*this);
      AcompTools::unitSort(addItem.Units);
      Out.primativeAddItem(addItem);	
    }
  for(const Acomp& AC : A.Comp)
    {
      Acomp addItem=interCombine(*this,AC);
      AcompTools::unitSort(addItem.Units);
      Out.primativeAddItem(addItem);
    }
  
  return Out;
}

Acomp
Acomp::expandUII(const Acomp& A) const
  /*!
    Addition to an intersection unit with an Union via intersection
    \param A :: Other object to add
    \return A
   */
{
  return A.expandIIU(*this);
}

Acomp
Acomp::expandUIU(const Acomp& A) const
  /*!
    Addition to a union unit with a union via intersection
    \param A :: Other object to add
    \return A
   */
{
  Acomp Out(0);     // union
  for(const int AI : Units)
    {
      // N.aN
      for(const int BI : A.Units)
	{
	  Acomp addItem=interCombine(AI,BI);
	  AcompTools::unitSort(addItem.Units);
	  Out.primativeAddItem(addItem);
	}
      // N.ac
      for(const Acomp& AC : A.Comp)
	{
	  Acomp addItem=interCombine(AI,AC);
	  AcompTools::unitSort(addItem.Units);
	  Out.primativeAddItem(addItem);
	}
    }
  for(const Acomp& AC : Comp)
    {
      // C.aN
      for(const int BI : A.Units)
	{
	  Acomp addItem=interCombine(BI,AC);
	  AcompTools::unitSort(addItem.Units);
	  Out.primativeAddItem(addItem);
	}
      // C.aC
      for(const Acomp& BC : A.Comp)
	{
	  Acomp addItem=interCombine(AC,BC);
	  AcompTools::unitSort(addItem.Units);
	  Out.primativeAddItem(addItem);
	}
    }
  return Out;
} 

  
Acomp
Acomp::expandIUI(const Acomp& A) const
  /*!
    Addition to an intersection unit with an intersection via UNION
    \param A :: Other object to add
    \return A
   */
{
  Acomp Out(1);   // Union
  for(const int AI : Units)
    {
      // N.aN
      for(const int BI : A.Units)
	{
	  Acomp addItem=unionCombine(AI,BI);
	  AcompTools::unitSort(addItem.Units);
	  Out.primativeAddItem(addItem);
	}

      // N.ac
      for(const Acomp& AC : A.Comp)
	{
	  Acomp addItem=unionCombine(AI,AC);
	  AcompTools::unitSort(addItem.Units);
	  Out.primativeAddItem(addItem);
	}
    }
  for(const Acomp& AC : Comp)
    {
      // C.aN
      for(const int BI : A.Units)
	{
	  Acomp addItem=unionCombine(BI,AC);
	  AcompTools::unitSort(addItem.Units);
	  Out.primativeAddItem(addItem);
	}
      // C.aC
      for(const Acomp& BC : A.Comp)
	{
	  Acomp addItem=unionCombine(AC,BC);
	  AcompTools::unitSort(addItem.Units);
	  Out.primativeAddItem(addItem);
	}
    }
  return Out;
} 

Acomp
Acomp::expandIUU(const Acomp& A) const
  /*!
    Addition to an intersection unit with a union via UNION
    UPDATED
    \param A :: Other object to add
    \return grouped object
   */
{  
  Acomp Out(1);  // intersect
  for(const int AI : Units)
    {
      // N.aN
      Acomp addItem(0);  // union
      addItem.Units=A.Units;
      addItem.Units.push_back(AI);
      AcompTools::unitSort(addItem.Units);
      Out.primativeAddItem(addItem);
    
      // N.ac
      for(const Acomp& AC : A.Comp)
	{
	  Acomp addItem=unionCombine(AI,AC);
	  AcompTools::unitSort(addItem.Units);
	  Out.primativeAddItem(addItem);
	}
    }
  // this is not correct:
  for(const Acomp& AC : Comp)
    {
      // C.aN
      for(const int BI : A.Units)
	{
	  Acomp addItem=unionCombine(BI,AC);
	  AcompTools::unitSort(addItem.Units);
	  Out.primativeAddItem(addItem);
	}
      // C.aC
      for(const Acomp& BC : A.Comp)
	{
	  Acomp addItem=unionCombine(AC,BC);
	  AcompTools::unitSort(addItem.Units);
	  Out.primativeAddItem(addItem);
	}
    }
  return Out;
} 

Acomp
Acomp::expandUUI(const Acomp& A) const
  /*!
    Addition to an intersection unit with a union via UNION
    \param A :: Other object to add
    \return A
   */
{
  return A.expandIUU(*this);
}

  
Acomp
Acomp::expandUUU(const Acomp& A) const
  /*!
    Addition to an uniion unit with a union via UNION
    \param A :: Other object to add
    \return A
   */
{
  Acomp Out(0);   // Union

  ELog::EM<<"UUU"<<ELog::endDiag;
  Out.Units.insert(Out.Units.end(),Units.begin(),Units.end());
  Out.Comp.insert(Out.Comp.end(),Comp.begin(),Comp.end());
  
  Out.Units.insert(Out.Units.end(),A.Units.begin(),A.Units.end());
  Out.Comp.insert(Out.Comp.end(),A.Comp.begin(),A.Comp.end());
  return Out;
}

  
      
Acomp
Acomp::componentExpand(const int interFlag,const Acomp& A) const
  /*
    Triplet pair expanded
    \param interFlag :: Intersection (1) or Union (0)
    \param A :: Component to multiply by
  */
{
  ELog::RegMethod RegA("Acomp","componentExpand");

  // Case by case:
  Acomp Out(0);
  if (Intersect==1 && interFlag==1 && A.Intersect==1)
    Out=expandIII(A);
  else if (Intersect==1 && interFlag==1 && A.Intersect==0)
    Out=expandIIU(A);
  else if (Intersect==0 && interFlag==1 && A.Intersect==1)
    Out=expandUII(A);
  else if (Intersect==0 && interFlag==1 && A.Intersect==0)
    Out=expandUIU(A);
  
  else if (Intersect==1 && interFlag==0 && A.Intersect==1)
    {
      Out=expandIUI(A);
    }
  else if (Intersect==1 && interFlag==0 && A.Intersect==0)
    {
      Out=expandIUU(A);
    }
  else if (Intersect==0 && interFlag==0 && A.Intersect==1)
    {
      Out=expandUUI(A);
    }
  else if (Intersect==0 && interFlag==0 && A.Intersect==0)
    {
      Out=expandUUU(A);
    }
  else
    throw ColErr::InContainerError<int>(Intersect,
					"Intersect/interflag not correct");
  
  AcompTools::unitSort(Out.Units);

  return  Out;
}

Acomp
Acomp::interCombine(const int A,const int B)
  /*!
    Intersection combine of two integers
    \param A :: Index A 
    \param B :: Index B
    \return 0 [Nothing] / or iterm
  */
{
  Acomp Out(1);  // inter
  if (A != -B)   // need a special for this
    {
      Out.Units.push_back(A);
      if (A!=B)
	Out.Units.push_back(B);
    }
  return Out;
} 

Acomp
Acomp::unionCombine(const int A,const int B)
  /*!
    Intersection combine of two integers
    \param A :: Index A 
    \param B :: Index B
    \return 0 [Nothing] / 1 
  */
{
  Acomp Out(0);  // union
  if (A == -B)   // need a special for this
    ELog::EM<<"Totology"<<ELog::endDiag;
  Out.Units.push_back(A);
  if (A!=B)
    Out.Units.push_back(B);
  return Out;
} 

Acomp
Acomp::interCombine(const int A,const Acomp& B)
  /*!
    Intersection combine of two integers
    \param A :: Index A 
    \param B :: Component
    \return 0 [Nothing] / or Acomp units
  */
{
  Acomp Out(1);  // inter
  if (B.isSingle() && B.isSimple())
    return interCombine(A,B.itemN(0));

  Out.Units.push_back(A);
  Out.Comp.push_back(B);

  return Out;
} 

Acomp
Acomp::unionCombine(const int A,const Acomp& B)
  /*!
    Intersection combine of one integers/one comp
    \param A :: Index A 
    \param B :: Component
    \return 0 [Nothing] / or iterm
  */
{
  Acomp Out(0);  // union
  if (B.isSingle() && B.isSimple())
    return unionCombine(A,B.getSinglet());

  Out.Units.push_back(A);
  Out.Comp.push_back(B);

  return Out;
} 

Acomp
Acomp::interCombine(const Acomp& A,const Acomp& B)
  /*!
    Intersection combine of one comp/one comp
    \param A :: Component A 
    \param B :: Component B
    \return 0 [Nothing] / or iterm
  */
{
  if (A.isNull()) return B;
  if (B.isNull()) return A;
  
  if (A.Intersect==1 && B.Intersect==1)
    {
      Acomp Out(A);
      Out.primativeAddItem(B);
      return Out;
    }

  if (A.isSingle() && A.isSimple())
    return interCombine(A.getSinglet(),B);
      
  if (A.Intersect)
    return A.expandIIU(B);
  
  ELog::EM<<"Failure "<<ELog::endErr;
  
  return Acomp(1);
} 

Acomp
Acomp::unionCombine(const Acomp& A,const Acomp& B)
  /*!
    Intersection combine of one comp/one comp
    \param A :: Component A 
    \param B :: Component B
    \return 0 [Nothing] / or iterm
  */
{
  if (A.isNull()) return B;
  if (B.isNull()) return A;
  
  if (A.Intersect==0 && B.Intersect==0)
    {
      Acomp Out(A);
      Out.primativeAddItem(B);
      return Out;
    }

  if (A.isSingle() && A.isSimple())
    return unionCombine(A.getSinglet(),B);
      
  if (A.Intersect)
    return A.expandUUU(B);
  else
    return A.expandUUI(B);
  
  ELog::EM<<"Failure "<<ELog::endErr;
  
  return Acomp(0);
} 
  
 
void
Acomp::primativeAddItem(const Acomp& AC)
  /*!
    Primative addition based on AC size
    \param AC :: Item to add
  */  
{

  if (!AC.isNull())
    {
      if (!AC.isSingle())
	Comp.push_back(AC);
      else
	Units.push_back(AC.itemN(0));
    }
  return;
}

void
Acomp::expandBracket()
  /*!
    Expand all the Intersection brackets
  */
{
  static int cnt(0);


  cnt++;
  // all lower units
  for(Acomp& AC : Comp)
    AC.expandBracket();

  if (this->Intersect)
    {
      if (!Units.empty() && !Comp.empty())
	{
	  Acomp N(1);  // intersect
	  N.Units=Units;
	  Comp[0]=N.componentExpand(1,Comp[0]);
	  Units.clear();
	}
      while (Comp.size()>1)
	{
	  for(size_t i=0;i<Comp.size();i+=2)
	    if (i+1!=Comp.size())
	      Comp[i/2]=Comp[i].componentExpand(1,Comp[i+1]);
	    else
	      Comp[i/2]=Comp[i];
	  Comp.erase(Comp.begin()+Comp.size()/2+1,Comp.end());
	}
    }
  
  upMoveComp();
  merge();
  makeNull();
  return;
}

bool
Acomp::removeEqUnion()
  /*!
    Find if two Components can be replaced by one
    Assume BOTH components are simple Union
    \return true if something removed
  */
{
  bool outFlag(0);
  if (Intersect==1)
    {
      std::vector<int>::iterator au;
      // remove units from literals
      for(const int CN : Units)
	{
	  for(Acomp& AC : Comp)
	    {
	      au=std::remove(AC.Units.begin(),AC.Units.end(),CN);
	      if (au!=AC.Units.end())
		{
		  outFlag=1;
		  AC.Units.erase(au,AC.Units.end());
		}
	      if (std::find(AC.Units.begin(),AC.Units.end(),-CN)!=
		  AC.Units.end() )        // union of a+ a'
		{
		  outFlag=1;
		  AC.Units.clear();
		  AC.Comp.clear();
		}
	    }
	}
      
      // remove nulls
      std::vector<Acomp>::iterator ac=
	std::remove_if(Comp.begin(),Comp.end(),
		       [&](const Acomp& AC)->bool
		       { return AC.isNull();});
      Comp.erase(ac,Comp.end());
    }

  return outFlag;
}

bool
Acomp::removeUnionPair()
  /*!
    Find if two Components can be replaced by one
    Assume BOTH components are simple Union
    \return true if something removed
  */
{
  Acomp Hold(*this);

  bool outFlag(0);
  if (Intersect==1)
    {
      std::vector<int>::iterator au;
      // remove units from literals
      for(const int CN : Units)
	{
	  for(Acomp& AC : Comp)
	    {
	      if (std::find(AC.Units.begin(),AC.Units.end(),-CN) !=
		  AC.Units.end() )        // union of a+ a'
		{
		  outFlag=1;
		  AC.Units.clear();
		  AC.Comp.clear();
		}
	    }
	}
      // remove nulls
      std::vector<Acomp>::iterator ac=
	std::remove_if(Comp.begin(),Comp.end(),
		       [&](const Acomp& AC)->bool
		       { return AC.isNull();});
      Comp.erase(ac,Comp.end());
    }

  std::set<size_t> removedComp;
  for(size_t i=0;i<Comp.size();i++)
    {
      if (removedComp.find(i)==removedComp.end())
	{
	  Acomp& AC(Comp[i]);
	  for(size_t j=i+1;j<Comp.size();j++)
	    {
	      if (removedComp.find(j)==removedComp.end())
		{
		  Acomp& BC(Comp[j]);
		  const int lc=AC.logicalIntersectCover(BC);
		  if (lc == 2 || lc == 3)
		    {
		      AC.clear();
		      removedComp.insert(i);
		      break;  // exit to outer loop
		    }
		  else if (lc == 1)
		    {
		      BC.clear();
		      removedComp.insert(j);
		    }
		}
	    }
	}
    }
  // Now remove empty cells
  if (!removedComp.empty())
    {
      outFlag=1;
      std::vector<Acomp>::iterator ac=
	std::remove_if(Comp.begin(),Comp.end(),
		       [&](const Acomp& AC)->bool
		       { return AC.isNull();});
      Comp.erase(ac,Comp.end());
    }
  return outFlag;
}
  
void
Acomp::expandDNFBracket()
  /*!
    Expand all the Union brackets and then find DNF
  */
{
  ELog::RegMethod RegA("Acomp","expandCNFBracket");
  expandCNFBracket();
  complement();
  return;
}

void
Acomp::expandCNFBracket()
  /*!
    Expand all the Union brackets
  */
{
  ELog::RegMethod RegA("Acomp","expandCNFBracket");

  static int cnt(0);

  isNumberSorted();
  cnt++;
  // all lower units
  for(Acomp& AC : Comp)
    AC.expandCNFBracket();

  if (!this->Intersect)
    {
      if (!Units.empty() && !Comp.empty())
	{
	  Acomp N(0);  // union
	  N.Units=Units;
	  Comp[0]=N.componentExpand(0,Comp[0]);
	  Comp[0].isNumberSorted();
	  Units.clear();
	}
      while (Comp.size()>1)
	{
	  Comp[0]=Comp[0].componentExpand(0,Comp[1]);
	  Comp.erase(Comp.begin()+1);
	}
    }
  do
    {
      upMoveComp();
      merge();
      makeNull();
    }
  while(removeUnionPair());

  return;
}

void
Acomp::removeNotPresent(const int target)
  /*!
    If rule is not present remove
    NOTE :: decends
  */
{
  return;
}

void
Acomp::removeTarget(const int target)
  /*!
    Set target to true and remove. 
    NOTE: If this is a TOP Acomp we CANNOT remove
    the target 
    \param target :: target rule
  */
{
  if (Intersect)
    {
      stlFunc::removeIfUnit(Units,target);
      std::erase(
		 std::remove_if(Comp.begin(),Comp.end(),
				[&target](const Acomp& AC) -> bool
				{
				  return AC.hasUnitInUnit(target);
				}),
		 Comp.end());
    }
  for(Acomp& AC : Comp)
    AC.removePlus(target);
  return;
}


void
Acomp::minimize()
  /*
    Call out a simple minimization
  */
{
  // Assume CNF form
  expandCNFBracket();
  complement();  
  return;
}

  
} // NAMESPACE MonteCarlo
