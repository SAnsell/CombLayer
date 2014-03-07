/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   funcBaseInc/varNameOrder.h
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
#ifndef varNameOrder_h
#define varNameOrder_h

class varNameOrder
{
 private:

  boost::regex Re;          /// Regular expression

 public:

  /// Construct to build expression : Name Number 
  varNameOrder() : Re("^(\\D*)(\\d*)(.*)") {} 

  template<typename T> bool
    operator()(const std::pair<std::string,T>& A,
	       const std::pair<std::string,T>& B) const
    {
      return operator()(A.first,B.first);
    }

    
   
  bool operator()(const std::string& A,
		  const std::string& B) const
    /*!
      Sort function that splits a name into (A number B)
      then sotrs of A B number 
     */
    {
      std::vector<std::string> AOutVec;
      std::vector<std::string> BOutVec;
      int ANum,BNum;
      if (StrFunc::StrFullSplit(A,Re,AOutVec) &&
	  StrFunc::StrFullSplit(B,Re,BOutVec) &&
	   AOutVec[0]==BOutVec[0])
	 {
	   if (AOutVec[2]!=BOutVec[2])
	     return AOutVec[2]<BOutVec[2];
	   if (StrFunc::convert(AOutVec[1],ANum) &&
	       StrFunc::convert(BOutVec[1],BNum))
	     return ANum<BNum;
	}							  
      return (A<B);
    }
};

#endif
