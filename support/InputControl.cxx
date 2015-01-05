/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   support/InputControl.cxx
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
#include <cmath>
#include <vector>
#include <map>
#include <list>
#include <string>
#include <algorithm>
#include <functional>

#include "Exception.h"
#include "support.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "InputControl.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"

namespace InputControl
{

int 
mainVector(const int ARGC,char** ARGV,std::vector<std::string>& V)
  /*!
    Convert the main input into a string vector
    \param ARGC :: Number of arguments including the program title
    \param ARGV :: Character array
    \param V :: Vector of strings of the char
    \return number of components
  */
{
  V.clear();
  for(int i=1;i<ARGC;i++)
    V.push_back(std::string(ARGV[i]));

  return static_cast<int>(V.size());
}

std::string
getFileName(std::vector<std::string>& Names)
  /*!
    Extract the filename at the end of the input 
    and remove it from the stack.
    \param Names :: vector of values
    \return Filename
  */
{
  ELog::RegMethod RegA("InputControl","getFileName");
  // NOTE THE REVERSE ORDER:
  std::string Oname("-");
  if (!Names.empty())
    {
      Oname=Names.back();   // Output file name
      Names.pop_back();
    }
  return Oname;
}

int
flagExtract(std::vector<std::string>& MArg,
	    const std::string& Option)
  /*!
    Process Main data to remove -x option 
    \param MArg:: Main argv list 
    \param Option :: option flag (short/long form) to return object
    \return 1 :: success
  */
{
  std::vector<std::string>::iterator vc;
  for(vc=MArg.begin();vc!=MArg.end();vc++)
    {
      if ((*vc)[0]=='-' && vc->substr(1)==Option)
        {
	  MArg.erase(vc);
	  return 1;
	}
    }
  return 0;
}

int
flagExtract(std::vector<std::string>& MArg,
	    const std::string& ShortOpt,const std::string& LongOpt)
  /*!
    Process Main data to remove -x option 
    \param MArg:: Main argv list 
    \param ShortOpt :: Short options flag (e.g. -h) [without -]
    \param LongOpt :: Long options flag (e.g. --help) [without --]
    \retval 1 :: short success
    \retval 1 :: long success
    \retval 0 :: nothing found
  */
{
  std::vector<std::string>::iterator vc;
  const size_t sFlag(ShortOpt.length());
  const size_t lFlag(LongOpt.length());
  for(vc=MArg.begin();vc!=MArg.end();vc++)
    {
      const size_t Alen=vc->length();
      if (Alen>=2 && (*vc)[0]=='-')        // general flag object
        {
	  if (Alen-sFlag==1)          // cannot match otherwise!
	    {
	      if (vc->substr(1)==ShortOpt)
	        {
		  MArg.erase(vc);
		  return 1;
		}
	    }
	  if (lFlag && (Alen-lFlag==2)
	      && (*vc)[1]=='-' && vc->substr(2)==LongOpt)
	    {
	      MArg.erase(vc);
	      return 1;
	    }
	}
    }
  return 0;
}

template<typename T,typename U>
int
flagVExtract(std::vector<std::string>& MArg,
	    const std::string& ShortOpt,const std::string& LongOpt,
	     T& Value,U& ValueB)
  /*!
    Process Main data to remove -x option 
    \param MArg:: Main argv list 
    \param ShortOpt :: Short options flag (e.g. -h) [without -]
    \param LongOpt :: Long options flag (e.g. --help) [without --]
    \param Value :: First value to get
    \param ValueB :: Second value to get
    \retval Number found :: short success
    \retval Number found :: long success
    \retval 0 :: nothing found
  */
{
  std::vector<std::string>::iterator vc;
  std::vector<std::string>::iterator ac;
  const size_t sFlag(ShortOpt.length());
  const size_t lFlag(LongOpt.length());
  int found(0);
  for(vc=MArg.begin();vc!=MArg.end();vc++)
    {
      const size_t Alen=vc->length();
      if (Alen>=2 && (*vc)[0]=='-')        // general flag object
        {
	  if (Alen-sFlag==1)          // cannot match otherwise!
	    {
	      if (vc->substr(1)==ShortOpt ||
		  (lFlag && (Alen-lFlag==2) && 
		   (*vc)[1]=='-' && vc->substr(2)==LongOpt))
	        {
		  ac=vc;
		  ac++;
		  if (ac!=MArg.end() && StrFunc::convert(*ac,Value))
		    {
		      ac++;
		      found++;
		      if (ac!=MArg.end() && StrFunc::convert(*ac,ValueB))
			{
			  found++;
			  ac++;
			}
		      MArg.erase(vc,ac);
		      return found;
		    }
		}
	    }
	}
    }
  return 0;
}

template<typename T,typename U,typename V>
int
flagVExtract(std::vector<std::string>& MArg,
	    const std::string& ShortOpt,const std::string& LongOpt,
	     T& Value,U& ValueB,V& ValueC)
  /*!
    Process Main data to remove -x option 
    \param MArg:: Main argv list 
    \param ShortOpt :: Short options flag (e.g. -h) [without -]
    \param LongOpt :: Long options flag (e.g. --help) [without --]
    \param Value :: First value to get
    \param ValueB :: Second value to get
    \param ValueC :: Third value to get
    \retval 0 :: Failure
    \retval 1 :: Success [No variables]
    \retval N :: Number of variables + 1 
  */
{
  std::vector<std::string>::iterator vc;
  std::vector<std::string>::iterator ac;
  const size_t sFlag(ShortOpt.length());
  const size_t lFlag(LongOpt.length());
  for(vc=MArg.begin();vc!=MArg.end();vc++)
    {
      const size_t Alen=vc->length();
      int cnt(1);
      if (Alen>=2 && (*vc)[0]=='-')        // general flag object
        {
	  if ((Alen-sFlag==1 && vc->substr(1)==ShortOpt) ||
	      ( lFlag && (Alen-lFlag==2) 
		&& (*vc)[1]=='-' && vc->substr(2)==LongOpt))
	    {
	      ac=vc;
	      ac++;
	      if (ac!=MArg.end() && StrFunc::convert(*ac,Value))
		{
		  cnt++;
		  ac++;
		  if (ac!=MArg.end() && StrFunc::convert(*ac,ValueB))
		    {
		      cnt++;
		      ac++;
		      if (ac!=MArg.end() && StrFunc::convert(*ac,ValueC))
			{
			  ac++;
			  cnt++;
			}
		    }
		}
	      MArg.erase(vc,ac);
	      return cnt;
	    }
	}
    }
  return 0;
}

template<typename T>
int
flagVExtract(std::vector<std::string>& MArg,
	    const std::string& ShortOpt,const std::string& LongOpt,
	     T& Value)
  /*!
    Process Main data to remove -x option 
    \param MArg:: Main argv list 
    \param ShortOpt :: Short options flag (e.g. -h) [without -]
    \param LongOpt :: Long options flag (e.g. --help) [without --]
    \param Value :: Value to extract
    \retval 2 :: Variable + option found
    \retval 1 :: short/long success
    \retval 0 :: nothing found
  */
{
  std::vector<std::string>::iterator vc;
  std::vector<std::string>::iterator ac;
  const size_t sFlag(ShortOpt.length());
  const size_t lFlag(LongOpt.length());
  for(vc=MArg.begin();vc!=MArg.end();vc++)
    {
      const size_t Alen=vc->length();
      if (Alen>=2 && (*vc)[0]=='-')        // general flag object
        {
	  if ((Alen-sFlag==1 && vc->substr(1)==ShortOpt) ||
	      ( lFlag && (Alen-lFlag==2) 
		&& (*vc)[1]=='-' && vc->substr(2)==LongOpt))
	    {
	      ac=vc;
	      ac++;
	      if (ac==MArg.end() || !StrFunc::convert(*ac,Value))
		{
		  MArg.erase(vc,ac);
		  return 1;
		}
	      MArg.erase(vc,ac+1);
	      return 2;
	    }
	}
    }
  return 0;
}

template<>
int
flagVExtract(std::vector<std::string>& MArg,
	    const std::string& ShortOpt,const std::string& LongOpt,
	     Geometry::Vec3D& Value)
  /*!
    Process Main data to remove -x option.
    Note this removes the excess numbers even if not a Vec3D
    \param MArg:: Main argv list 
    \param ShortOpt :: Short options flag (e.g. -h) [without -]
    \param LongOpt :: Long options flag (e.g. --help) [without --]
    \param Value :: Value to extract
    \retval 2 :: short/long success + variable
    \retval 1 :: variable not found
    \retval 0 :: nothing found

  */
{
  ELog::RegMethod RegA("InputControl","flagVExtract<Vec3D>");
  // Trick use <T,U,V> version with double
  double a(0.0),b(0.0),c(0.0);
  const int flag=flagVExtract(MArg,ShortOpt,LongOpt,a,b,c);
  if (flag==4)
    {
      Value(a,b,c);

      return 2;
    }
  return (flag) ? 1 : 0; 
}

template<typename T>
int
flagVExtract(std::vector<std::string>& MArg,
	    const std::string& Option,T& Value)
  /*!
    Process Main data to remove -x option 
    \param MArg :: Main argv list 
    \param Option :: Option flag to find 
    \param Value :: integers of the main argument
    \return 1 :: success
    \retval -1 :: Flag found but unable to convert
  */
{
  std::vector<std::string>::iterator vc;
  std::vector<std::string>::iterator ac;
  for(vc=MArg.begin();vc!=MArg.end();vc++)
    {
      if ((*vc)[0]=='-' && vc->substr(1)==Option)
        {
	  ac=vc;
	  ac++;
	  if (ac==MArg.end() || !StrFunc::convert(*ac,Value))
	    return -1;
	  ac++;   // for erase point
	  MArg.erase(vc,ac);
	  return 1;
	}
    }
  return 0;
}

int
processMainInput(const int AC,char** AV,
		 std::vector<std::string>& Value,
		 std::vector<std::string>& OptionVec)
  /*!
    This function processes the main input and moves AC ::
    \param AC :: Number of parameters (argc)
    \param AV :: argv list 
    \param Value :: integers of the main argument
    \param OptionVec :: option flags (-flag) type 
    \return Numnber of options
  */
{
  // Note need to skip name of the program
  int cnt(0);
  for(int i=1;i<AC;i++)
    {
      if (AV[i][0]=='-')
        {
	  OptionVec.push_back(std::string(AV[i]+1));
	  cnt++;
	}
      else
	Value.push_back(std::string(AV[i]));
    }
  return cnt;
}

int
findFlag(const std::vector<std::string>& Names,const std::string& F)
  /*!
    Determines if F is in the list
    \param Names :: list of Flag names 
    \param F :: Flag name
    \retval 0 :: Failure to find
    \retval Position + 1 (if successful)
   */
{
  std::vector<std::string>::const_iterator vc =
    find(Names.begin(),Names.end(),F);
  if (vc==Names.end())
    return 0;
  return 1+static_cast<int>(distance(Names.begin(),vc));
}

///\cond TEMPLATE 

template int flagVExtract(std::vector<std::string>&,
			 const std::string&,double&);

template int flagVExtract(std::vector<std::string>&,
			 const std::string&,int&);


template int flagVExtract(std::vector<std::string>&,
			 const std::string&,const std::string&,
			 double&);

template int flagVExtract(std::vector<std::string>&,
			 const std::string&,const std::string&,
			 long int&);

template int flagVExtract(std::vector<std::string>&,
			 const std::string&,const std::string&,
			  std::string&);

template int flagVExtract(std::vector<std::string>&,
			 const std::string&,const std::string&,
			 int&);

template int flagVExtract(std::vector<std::string>&,
			  const std::string&,const std::string&,
			  std::string&,std::string&);

template int flagVExtract(std::vector<std::string>&,
			  const std::string&,const std::string&,
			  std::string&,double&);

template int flagVExtract(std::vector<std::string>&,
			  const std::string&,const std::string&,
			  double&,double&);

template int flagVExtract(std::vector<std::string>&,
			  const std::string&,const std::string&,
			  int&,int&);

template int flagVExtract(std::vector<std::string>&,
			 const std::string&,const std::string&,
			 double&,double&,double&);

template int flagVExtract(std::vector<std::string>&,
			 const std::string&,const std::string&,
			 int&,int&,int&);

///\endcond TEMPLATE

}  // NAMESPACE InputControl
