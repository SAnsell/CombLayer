/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   log/OutputLog.cxx
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
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <boost/format.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "Debug.h"
#include "OutputLog.h"

namespace ELog
{

template<typename RepClass>
OutputLog<RepClass>::OutputLog() :
  colourFlag(0),activeBits(255),actionBits(0),
  debugBits(0),typeFlag(1),locFlag(1),
  storeFlag(0),NBasePtr(0)
  /*!
    Constructor
  */
{}

template<typename RepClass>
OutputLog<RepClass>::OutputLog(const std::string&) :
  colourFlag(0),activeBits(255),actionBits(0),debugBits(0),
  typeFlag(1),locFlag(1),storeFlag(0),NBasePtr(0)
  /*!
    Constructor with string
  */
{}

template<>
OutputLog<ELog::FileReport>::OutputLog(const std::string& Fname) :
  colourFlag(0),activeBits(255),actionBits(0),debugBits(0),typeFlag(1),
  locFlag(1),storeFlag(0),NBasePtr(0),
  FOut(Fname)
  /*!
    Constructor 
    \param Fname :: Name of the log-file to open 
    (old log files are deleted)
  */
{}

template<typename RepClass>
OutputLog<RepClass>::OutputLog(const OutputLog<RepClass>& A)  :
  colourFlag(A.colourFlag),
  activeBits(A.activeBits),actionBits(A.actionBits),
  debugBits(A.debugBits),typeFlag(A.typeFlag),locFlag(A.locFlag),
  storeFlag(A.storeFlag),NBasePtr(A.NBasePtr),
  FOut(A.FOut),EText(A.EText),EType(A.EType)
  /*!
    Standard Copy Constructor
    \param A :: OutputLog object to copy
    \return *this
  */

{}

template<typename RepClass>
OutputLog<RepClass>&
OutputLog<RepClass>::operator=(const OutputLog<RepClass>& A)
  /*!
    Standard Assignment operator 
    \param A :: OutputLog to copy
    \returns *this
  */
{
  if (this!=&A)
    {
      colourFlag=1;
      activeBits=A.activeBits;
      actionBits=A.actionBits;
      debugBits=A.debugBits;
      typeFlag=A.typeFlag;
      locFlag=A.locFlag;
      storeFlag=A.storeFlag;
      NBasePtr=A.NBasePtr;
      FOut=A.FOut;
      EText=A.EText;
      EType=A.EType;
    }
  return *this;
}

template<typename RepClass>
OutputLog<RepClass>::~OutputLog() 
  /*!
    Destructor
  */
{}

template<typename RepClass>
bool
OutputLog<RepClass>::isActive(const int Flag) const
  /*!
    Check for activity. Note that 0 is considered 
    always to be written.
    \param Flag :: Thing to check
    \return true is flags has appropiate bit set
  */
{
  const int debugFlag=debugStatus::Instance().getFlag();

  const size_t part=
    (Flag<=0) ? 1 : static_cast<size_t>(2*Flag);
  
  return ((part & activeBits) && 
	  (debugFlag || !(part & debugBits))) ? 1 : 0;
}

template<typename RepClass>
std::string
OutputLog<RepClass>::getColour(const int) const
  /*!
    Get the Colour string for termial output
    - Only for terminal output so this basic give nothing
    \param  :: placeholder for colour id
    \return Colour string 
  */
{
  return std::string();
}

template<>
std::string
OutputLog<EReport>::getColour(const int Flag) const
  /*!
    Get the Colour string for termial output
    - Only for terminal output so this basic give nothing
    \param Flag :: Thing to check
    \return Colour string 
  */
{
  if (colourFlag) 
    {
      const unsigned int part=(Flag<=0) ? 1 : 
	static_cast<unsigned int>(2*Flag);
      switch(part)
        {
	case ELog::basic:     
	  return std::string("\033[01;30m");   // standard
	case ELog::warn:
	  return std::string("\033[22;36m");   // Yellow
	case ELog::error:
	  return std::string("\033[0;91m");    // RED
	case ELog::debug:
	  return std::string("\033[01;35m");   // purple
	case ELog::diag:
	  return std::string("\033[04;32m");  // green
	case ELog::crit:
	  return std::string("\033[01;31m");   // red
	case ELog::trace:
	  return std::string("\033[0;33m");    // Brown
	}
    }
  return std::string();
}

template<typename RepClass>
void
OutputLog<RepClass>::makeAction(const int Flag)
  /*!
    Check for action
    always to be written.
    \param Flag :: Thing to check
    \return true is flags has appropiate bit set
  */
{
  const size_t part=(Flag<0) ? 
    static_cast<size_t>(-Flag*2+1) : 
    static_cast<size_t>(Flag*2+1);
  if (part & actionBits) 
    {
      locFlag=2;
      std::string full=locString();
      std::cout<<full<<std::endl;
      throw ColErr::ExitAbort("OutputLog<x>::makeAction",1);
    }
  return;
}

template<typename RepClass>
void 
OutputLog<RepClass>::report(const std::string& M,const int T)
  /*!
    Adds a report to the error Log and 
    call the reporter (if one exists)
    \param M :: Message
    \param T :: Type of error 
  */
{
  static int length(0);

  std::string cxItem=M;
  std::string::size_type pos;

  boost::format FMTbPart("%1$s%2$s");
  boost::format FMTStr80("%1$s%|80t|");
  boost::format FMTStr120("%1$s%|120t|");
  boost::format FMTStr160("%1$s%|160t|");

//  boost::format FMTStrL80("%3$s%|80t|[ %2$s% ] ");
  std::string Item;
  std::string colour;
  std::string colourReset;
  if (colourFlag)
    { 
      colour=getColour(T);
      colourReset="\033[0m";
      //      colourReset="\033[22;37m";
    }

  if (isActive(T))
    {
      const std::string Tag=colour+
	(FMTbPart % eType(T) % locString()).str()+
	colourReset;
      do
        {
	  pos=cxItem.find('\n');
	  const size_t TL=cxItem.length()+getIndentLength();
	  if (!length && (pos<80 || (pos==std::string::npos && TL<80)))
	    {
	      Item=
		(FMTStr80 % (indent()+cxItem.substr(0,pos))).str()+Tag;
	    }
	  else if (length<2 && (pos<120 || (pos==std::string::npos && TL<120)))
	    {
	      Item=(FMTStr120 % (indent()+cxItem.substr(0,pos))).str()+Tag;
	      length=1;
	    }
	  // This is always the default test:
	  else if (pos<160 || (pos==std::string::npos && TL<160))    
	    {
	      Item=(FMTStr160 % (indent()+cxItem.substr(0,pos))).str()+Tag;
	      length=2;
	    }
	  else  // Overflow on this one line 
	    {
	      Item=indent()+cxItem.substr(0,pos)+"\n"+Tag;
	    }

	  if (!storeFlag)
	    {
	      FOut.process(Item,T);
	    }
	  else
	    {
	      EText.push_back(Item);
	      EType.push_back(T);
	    }
	  cxItem.erase(0,pos+1);
	} while(pos!=std::string::npos);
    }
  return;
}

template<typename RepClass>
void 
OutputLog<RepClass>::report(const int T)
  /*!
    Adds a report to the error Log using
    and clearing the stream.
    call the reporter (if one exists)
    \param T :: Type of error 
  */
{
  report(cx.str(),T);
  cx.str("");
  makeAction(T);
  return;
}

template<typename RepClass>
std::string
OutputLog<RepClass>::indent() const
  /*!
    Outputs the indent
    \return string of spaces for the indent
  */
{
  return std::string(getIndentLength(),' ');
}


template<typename RepClass>
size_t
OutputLog<RepClass>::getIndentLength() const
  /*!
    Calculate the indent length
    \return length of the string of spaces
  */
{
  const long int ID=(NBasePtr) ? NBasePtr->indent() : 0;
  return (ID>0) ? static_cast<size_t>(ID) : 0;
}

template<typename RepClass>
std::string 
OutputLog<RepClass>::eType(const int T) const
  /*!
    Returns the most server error status.
    [Note that we start from zero so offset by *2]
    \param T :: report type		      
    \return string of error type
  */
{
  if (typeFlag)
   {
     if (T & 1)
       return "Warning :: ";
     if (T & 2)
       return "Error :: ";
     if (T & 4)
       return "Debug :: ";
     if (T & 8)
       return "Diagnostic :: ";
     if (T & 16)
       return "Critical :: ";
     if (T & 32)
       return "Trace :: ";

     return "Standard :: ";
   }
  return "";
}

template<typename RepClass>
std::string 
OutputLog<RepClass>::locString() const
  /*!
    Returns the location string if flag set
    \return String of error type
  */
{
  if (NBasePtr)
    {
      switch (locFlag)
	{
	case 1:
	  return NBasePtr->getBase();
	case 2:
	  return NBasePtr->getFullTree();
	}
    }
  return "";
}


template<typename RepClass>
void 
OutputLog<RepClass>::basic(const std::string& M)
  /*!
    Prints out the error message
    \param M :: String to write
   */
{
  report(M,0);
  return;
}

template<typename RepClass>
void 
OutputLog<RepClass>::warning(const std::string& M)
  /*!
    Prints out the warning
    \param M :: String to write
   */
{
  report(M,1);
  return;
}

template<typename RepClass>
void 
OutputLog<RepClass>::error(const std::string& M)
  /*!
    Prints out the error message
    \param M :: String to write
   */
{
  report(M,2);
  return;
}

template<typename RepClass>
void 
OutputLog<RepClass>::debug(const std::string& M)
  /*!
    Prints out the debug message
    \param M :: String to write
   */
{
  report(M,4);
  return;
}


template<typename RepClass>
void 
OutputLog<RepClass>::diagnostic(const std::string& M)
  /*!
    Prints out the diagnostic message
    \param M :: String to write
  */
{
  report(M,8);
  return;
}

template<typename RepClass>
void 
OutputLog<RepClass>::critical(const std::string& M)
  /*!
    Prints out the diagnostic message
    \param M :: String to write
   */
{
  report(M,16);
  return;
}

template<typename RepClass>
void 
OutputLog<RepClass>::trace(const std::string& M)
  /*!
    Prints out the diagnostic message
    \param M :: String to write
   */
{
  report(M,32);
  return;
}

template<typename RepClass>
void
OutputLog<RepClass>::dispatch(const int errFlag)
  /*!
    Clear and dispatch if necessary
    \param errFlag :: IF an error occured output / else clear only
  */
{
  if (errFlag) disLevel(0);
  storeFlag=0;
  return;
}

template<typename RepClass>
void
OutputLog<RepClass>::disLevel(const int levelFlag)
  /*!
    Clear and dispatch if necessary
    \param levelFlag to dispatch [low is more]
  */
{
  std::ostringstream cx;
  cx<<"Log BEGIN: Dispatch at level "<<levelFlag;
  FOut.process(cx.str(),0);
  for(unsigned int i=0;i<EText.size();i++)
    {
      if ((EType[i]) >= levelFlag)
	FOut.process(EText[i],EType[i]);
    }
  cx.str("");
  cx<<"Log END: Dispatch at level "<<levelFlag;
  FOut.process(cx.str(),0);

  EText.clear();
  EType.clear();
  return;
}

// ENDL FUNCTION :

template<typename RepClass>
OutputLog<RepClass>& 
endBasic(OutputLog<RepClass>& OLX)
  /*!
    Dispatch an endl of basic
    \param OLX :: Output stream
    \return Stream
  */
{
  OLX.basic();
  return OLX;
}

template<typename RepClass>
OutputLog<RepClass>& 
endErr(OutputLog<RepClass>& OLX)
  /*!
    Dispatch an endl of error
    \param OLX :: Output stream
    \return Stream
  */
{
  OLX.error();
  return OLX;
}

template<typename RepClass>
OutputLog<RepClass>& 
endWarn(OutputLog<RepClass>& OLX)
  /*!
    Dispatch an endl of warning
    \param OLX :: Output stream
    \return Stream
  */
{
  OLX.warning();
  return OLX;
}

template<typename RepClass>
OutputLog<RepClass>& 
endDebug(OutputLog<RepClass>& OLX)
  /*!
    Dispatch an endl of a debug
    \param OLX :: Output stream
    \return Stream
  */
{
  OLX.debug();
  return OLX;
}

template<typename RepClass>
OutputLog<RepClass>& 
endDiag(OutputLog<RepClass>& OLX)
  /*!
    Dispatch an endl of a dialog
    \param OLX :: Output stream
    \return Stream
  */
{
  OLX.diagnostic();
  return OLX;
}


template<typename RepClass>
OutputLog<RepClass>& 
endCrit(OutputLog<RepClass>& OLX)
  /*!
    Dispatch an endl of a critical 
    \param OLX :: Output stream
    \return Stream
  */
{
  OLX.critical();
  return OLX;
}

template<typename RepClass>
OutputLog<RepClass>& 
endTrace(OutputLog<RepClass>& OLX)
  /*!
    Dispatch an endl of trace 
    \param OLX :: Output stream
    \return Stream
  */
{
  OLX.trace();
  return OLX;
}

///\cond TEMPLATE 

template class OutputLog<EReport>;
template class OutputLog<FileReport>;
template class OutputLog<StreamReport>;

template OutputLog<EReport>& endDiag(OutputLog<EReport>&);
template OutputLog<EReport>& endBasic(OutputLog<EReport>&);
template OutputLog<EReport>& endErr(OutputLog<EReport>&);
template OutputLog<EReport>& endWarn(OutputLog<EReport>&);
template OutputLog<EReport>& endDebug(OutputLog<EReport>&);
template OutputLog<EReport>& endCrit(OutputLog<EReport>&);
template OutputLog<EReport>& endTrace(OutputLog<EReport>&);

template OutputLog<FileReport>& endDiag(OutputLog<FileReport>&);
template OutputLog<FileReport>& endBasic(OutputLog<FileReport>&);
template OutputLog<FileReport>& endErr(OutputLog<FileReport>&);
template OutputLog<FileReport>& endWarn(OutputLog<FileReport>&);
template OutputLog<FileReport>& endDebug(OutputLog<FileReport>&);
template OutputLog<FileReport>& endCrit(OutputLog<FileReport>&);
template OutputLog<FileReport>& endTrace(OutputLog<FileReport>&);

///\endcond TEMPLATE 

}  // NAMESPACE ELog




