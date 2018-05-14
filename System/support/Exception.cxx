/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   support/Exception.cxx
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
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <typeinfo>
#include <stdexcept>
#include <vector>
#include <map>
#include <cmath>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"

namespace ColErr
{

ExBase::ExBase(const int A,const std::string& Err) : 
    std::exception(),
    state(A),ErrLn(Err),OutLine(Err),
    CodeLocation(ELog::RegMethod::getFull())
  /*!
    Constructor
    \param A :: State variable
    \param Err :: Class:method string
  */
{}

ExBase::ExBase(const std::string& Err) :
  std::exception(),state(0),ErrLn(Err),
  OutLine(Err),CodeLocation(ELog::RegMethod::getFull())
  /*!
    Constructor
    \param Err :: Class:method string
  */
{ }

ExBase::ExBase(const ExBase& A) :
  std::exception(A),state(A.state),ErrLn(A.ErrLn),
  OutLine(A.OutLine),CodeLocation(A.CodeLocation)
  /*!
    Copy Constructor
    \param A :: ExBase object to copy
  */
{ }

ExBase&
ExBase::operator=(const ExBase& A)
  /*!
    Assignment operator
    \param A :: ExBase to copy
    \return *this
  */
{
  if(this!=&A)
    {
      std::exception::operator=(A);
      state=A.state;
      ErrLn=A.ErrLn;
      OutLine=A.OutLine;
      CodeLocation=A.CodeLocation;
    }
  return *this;
}

const char*
ExBase::what() const throw()
   /*!
     Write out the code position using the RegMethod system
     Note the used of a static string since exception is cleared later
     \return const char* to the string 
   */ 
{
  static std::string Item;
  Item=OutLine+"\nCode Stack:\n"+CodeLocation+"\n";
  return Item.c_str();
}


//---------------------------------------
//           IndexError class
//---------------------------------------

template<typename T>
IndexError<T>::IndexError(const T& V,const T& B,
		       const std::string& Place) :
  ExBase(0,Place),Val(V),maxVal(B)
  /*!
    Constructor 
    \param V :: Valued access
    \param B :: Max value allowed
    \param Place :: Description string
   */
{
  setOutLine();
}

template<typename T> 
IndexError<T>::IndexError(const IndexError<T>& A) :
  ExBase(A),Val(A.Val),maxVal(A.maxVal)
  /*!
    Copy constructor
    \param A :: Object to copy
  */
{}

template<typename T>
IndexError<T>&
IndexError<T>::operator=(const IndexError<T>& A) 
  /*!
    Assignment operator
    \param A :: Object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      ExBase::operator=(A);
    }
  return *this;
}


template<typename T>
void
IndexError<T>::setOutLine()
  /*!
    Writes out the range and limits
    to the outline
  */
{
  std::stringstream cx;
  cx<<"IndexError:"<<ExBase::getErr()<<" "<<Val<<" :: 0 <==> "<<maxVal;
  OutLine=cx.str();
  return;
}

//---------------------------------------
//           SizeError class
//---------------------------------------

template<typename T>
SizeError<T>::SizeError(const T& V,const T& B,
		       const std::string& Place) :
  ExBase(0,Place),Val(V),minVal(B)
  /*!
    Constructor 
    \param V :: Valued access
    \param B :: Max value allowed
    \param Place :: Description string
   */
{
  setOutLine();
}

template<typename T> 
SizeError<T>::SizeError(const SizeError<T>& A) :
  ExBase(A),Val(A.Val),minVal(A.minVal)
  /*!
    Copy constructor
    \param A :: Object to copy
  */
{}

template<typename T>
SizeError<T>&
SizeError<T>::operator=(const SizeError<T>& A) 
  /*!
    Assignment operator
    \param A :: Object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      ExBase::operator=(A);
    }
  return *this;
}


template<typename T>
void
SizeError<T>::setOutLine()
  /*!
    Writes out the range and limits
    to the outline
  */
{
  std::stringstream cx;
  cx<<"SizeError:"<<ExBase::getErr()<<" "<<Val<<" less than "<<minVal;
  OutLine=cx.str();
  return;
}

//-------------------------
// FileError
//-------------------------

FileError::FileError(const int V,const std::string& FName,
		     const std::string& Place) :
  ExBase(V,Place),fileName(FName)
  /*!
    Constructor
    \param V :: Error number
    \param FName :: Filename 
    \param Place :: Function description
  */
{
  setOutLine();
}

FileError::FileError(const FileError& A) :
  ExBase(A),fileName(A.fileName)
  /*!
    Copy constructor
    \param A :: FileError
  */
{}

FileError&
FileError::operator=(const FileError& A) 
  /*!
    Assignment operator
    \param A :: Object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      ExBase::operator=(A);
    }
  return *this;
}

void
FileError::setOutLine()
  /*!
    Writes out the range and limits
    to OutLine
  */
{
  std::stringstream cx;
  cx<<getErr()<<" in "<<fileName;
  OutLine=cx.str();
  return;
}

//-------------------------
// EmptyValue
//-------------------------
template<typename T>
EmptyValue<T>::EmptyValue(const std::string& Place) :
  ExBase(Place)
  /*!
    Constructor
    \param Place :: Function description
  */
{
  setOutLine();
}

template<typename T>
EmptyValue<T>::EmptyValue(const EmptyValue& A) :
  ExBase(A)
  /*!
    Copy constructor
    \param A :: EmptyValue
  */
{}

template<typename T>
EmptyValue<T>&
EmptyValue<T>::operator=(const EmptyValue<T>& A) 
  /*!
    Assignment operator
    \param A :: Object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      ExBase::operator=(A);
    }
  return *this;
}

template<typename T>
void
EmptyValue<T>::setOutLine()
  /*!
    Writes out the range and limits
    to OutLine
  */
{
  std::stringstream cx;
  cx<<getErr()<<" EmptyValue for type "<<typeid(T).name();
  OutLine=cx.str();
  return;
}

//-------------------------
// EmptyContainer
//-------------------------

EmptyContainer::EmptyContainer(const std::string& Place) :
  ExBase(Place)
  /*!
    Constructor
    \param Place :: Function description
  */
{
  setOutLine();
}

EmptyContainer::EmptyContainer(const EmptyContainer& A) :
  ExBase(A)
  /*!
    Copy constructor
    \param A :: EmptyContainer
  */
{}

EmptyContainer&
EmptyContainer::operator=(const EmptyContainer& A) 
  /*!
    Assignment operator
    \param A :: Object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      ExBase::operator=(A);
    }
  return *this;
}

void
EmptyContainer::setOutLine()
  /*!
    Writes out the range and limits
    to OutLine
  */
{
  std::stringstream cx;
  cx<<getErr()<<" EmptyContainer ";
  OutLine=cx.str();
  return;
}


//-------------------------
// InContainerError
//-------------------------

template<typename T>
InContainerError<T>::InContainerError(const T& V,const std::string& Place) :
  ExBase(0,Place),SearchObj(V)
  /*!
    Constructor
    \param V :: Value not found
    \param Place :: Function description
  */
{
  setOutLine();
}

template<typename T>
InContainerError<T>::InContainerError(const InContainerError& A) :
  ExBase(A),SearchObj(A.SearchObj)
  /*!
    Copy constructor
    \param A :: Object to copy
  */
{}

template<typename T>
InContainerError<T>&
InContainerError<T>::operator=(const InContainerError<T>& A) 
  /*!
    Assignment operator
    \param A :: Object to copy
    \return *this
  */
{
  if (this != &A)
    {
      ExBase::operator=(A);
    }
  return *this;
}


template<typename T>
void
InContainerError<T>::setOutLine() 
  /*!
    Sets the output string with 
    an appropiate value
  */
{
  std::stringstream cx;
  cx<<"InContainerError<>::\n";
  cx<<getErr()<<"\nkey== "<<SearchObj<<" ==";
  OutLine=cx.str();
  return;
}

//-------------------------
// RangeError
//-------------------------
template<typename T>
RangeError<T>::RangeError(const T& V,const T& aV,const T& bV,
		       const std::string& Place) :
  ExBase(0,Place),Index(V),minV(aV),maxV(bV)
  /*!
    Set a RangeError
    \param V :: Value that caused the problem
    \param aV :: low value 
    \param bV :: high value 
    \param Place :: String describing the place
  */
{
  setOutLine();
}

template<typename T>
RangeError<T>::RangeError(const RangeError<T>& A) :
  ExBase(A),Index(A.Index),minV(A.minV),maxV(A.maxV)
  /*!
    Copy constructor
    \param A :: RangeError to copy
  */
{}


template<typename T>
RangeError<T>&
RangeError<T>::operator=(const RangeError<T>& A) 
  /*!
    Assignment operator
    \param A :: Object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      ExBase::operator=(A);
    }
  return *this;
}

template<typename T>
void
RangeError<T>::setOutLine()
  /*!
    Writes out the range and aim point
    to Outline
  */
{
  std::stringstream cx;
  cx<<"RangeError<>"<<std::endl;
  cx<<getErr()<<" Value == "<<Index<<
    " Min == "<<minV<<
    " Max == "<<maxV;
  OutLine=cx.str();
  return;
}

//-------------------------
// OrderError
//-------------------------
template<typename T>
OrderError<T>::OrderError(const T& aV,const T& bV,
			  const std::string& Place) :
  ExBase(0,Place),lowValue(aV),highValue(bV)
  /*!
    Set a RangeError
    \param aV :: low value 
    \param bV :: high value 
    \param Place :: String describing the place
  */
{
  setOutLine();
}

template<typename T>
OrderError<T>::OrderError(const OrderError<T>& A) :
  ExBase(A),lowValue(A.lowValue),highValue(A.highValue)
  /*!
    Copy constructor
    \param A :: OrderError to copy
  */
{}


template<typename T>
OrderError<T>&
OrderError<T>::operator=(const OrderError<T>& A) 
  /*!
    Assignment operator
    \param A :: Object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      ExBase::operator=(A);
    }
  return *this;
}

template<typename T>
void
OrderError<T>::setOutLine()
  /*!
    Writes out the range and aim point
    to Outline
  */
{
  std::stringstream cx;
  cx<<"OrderError<>"<<std::endl;
  cx<<getErr()<<" value == "<<lowValue<<
    " = NOT LESS THAN = "<<highValue;
  OutLine=cx.str();
  return;
}

//-------------------------
// DimensionError
//-------------------------

template<unsigned int ndim,typename T>
DimensionError<ndim,T>::DimensionError(const T* A,const T* R,
				       const std::string& Place) :
  ExBase(0,Place)
  /*!
    Set a DimensionError
    \param A :: Array size
    \param R :: Required size
    \param Place :: String describing the place
  */
{
  for(unsigned int i=0;i<ndim;i++)
    {
      indexSize[i]=A[i];
      reqSize[i]=R[i];
    }  
  setOutLine();
}

template<unsigned int ndim,typename T>
DimensionError<ndim,T>::DimensionError(const std::vector<T>& A,
				       const std::vector<T>& R,
				       const std::string& Place) :
  ExBase(0,Place)
  /*!
    Set a DimensionError
    \param A :: Array size
    \param R :: Required size
    \param Place :: String describing the place
  */
{
  for(size_t i=0;i<ndim;i++)
    {
      indexSize[i]=(i<A.size()) ? A[i] : T(0);
      reqSize[i]=(i<R.size()) ? R[i] : T(0);
    }
  setOutLine();
}

template<unsigned int ndim,typename T>
DimensionError<ndim,T>::DimensionError(const DimensionError<ndim,T>& A) :
  ExBase(A)
  /*!
    Copy constructor 
    \param A :: Object to copy
  */
{
  for(unsigned int i=0;i<ndim;i++)
    {
      indexSize[i]=A.indexSize[i];
      reqSize[i]=A.reqSize[i];
    }  
}

template<unsigned int ndim,typename T>
DimensionError<ndim,T>&
DimensionError<ndim,T>::operator=(const DimensionError<ndim,T>& A) 
  /*!
    Assignment operator
    \param A :: Object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      ExBase::operator=(A);
      for(unsigned int i=0;i<ndim;i++)
        {
	  indexSize[i]=A.indexSize[i];
	  reqSize[i]=A.reqSize[i];
	}
    }
  return *this;
}

template<unsigned int ndim,typename T>
void
DimensionError<ndim,T>::setOutLine()
  /*!
    Writes out the range and aim point
    to OutLine
  */
{
  std::stringstream cx;
  cx<<"DimensionError<"<<ndim<<">"<<std::endl;
  cx<<getErr()<<":";

  for(unsigned int i=0;i<ndim;i++)
    {
      cx<<indexSize[i]<<" ("<<reqSize[i]<<") ";
    }
  OutLine=cx.str();
  return;
}

//-------------------------
// ArrayError
//-------------------------

template<int ndim>
ArrayError<ndim>::ArrayError(const int* A,const int* I,
			     const std::string& Place) :
  ExBase(0,Place)
  /*!
    Set a ArrayError
    \param A :: Array size
    \param I :: Index given
    \param Place :: String describing the place
  */
{
  for(int i=0;i<ndim;i++)
    {
      arraySize[i]=A[i];
      indexSize[i]=I[i];
    }  
  setOutLine();
}

template<int ndim>
ArrayError<ndim>::ArrayError(const ArrayError<ndim>& A) :
  ExBase(A)
  /*!
    Copy constructor 
    \param A :: Object to copy
  */
{
  for(int i=0;i<ndim;i++)
    {
      arraySize[i]=A.arraySize[i];
      indexSize[i]=A.indexSize[i];
    }
}

template<int ndim>
ArrayError<ndim>&
ArrayError<ndim>::operator=(const ArrayError<ndim>& A) 
  /*!
    Assignment operator
    \param A :: Object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      ExBase::operator=(A);
      for(int i=0;i<ndim;i++)
        {
	  arraySize[i]=A.arraySize[i];
	  indexSize[i]=A.indexSize[i];
	}
    }
  return *this;
}

template<int ndim>
void
ArrayError<ndim>::setOutLine()
  /*!
    Writes out the range and aim point
    to OutLine
  */
{
  std::stringstream cx;
  cx<<"ArrayError<"<<ndim<<">"<<std::endl;
  cx<<getErr()<<":";

  for(int i=0;i<ndim;i++)
    {
      cx<<indexSize[i]<<" ("<<arraySize[i]<<") ";
    }
  OutLine=cx.str();
  return;
}

//-------------------------
// MisMatch
//-------------------------

template<typename T> 
MisMatch<T>::MisMatch(const T& A,const T& B,const std::string& Place) :
  ExBase(0,Place),Aval(A),Bval(B) 
  /*!
    Constructor store two mismatched items
    \param A :: Item to store
    \param B :: Item to store
    \param Place :: Reason/Code item for error
  */
{
  setOutLine();
}

template<typename T> 
MisMatch<T>::MisMatch(const MisMatch<T>& A) :
  ExBase(A),Aval(A.Aval),Bval(A.Bval)
  /*!
    Copy Constructor
    \param A :: MisMatch to copy
   */
{}

template<typename T>
MisMatch<T>&
MisMatch<T>::operator=(const MisMatch<T>& A) 
  /*!
    Assignment operator
    \param A :: Object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      ExBase::operator=(A);
    }
  return *this;
}


template<typename T> 
void
MisMatch<T>::setOutLine()
  /*!
    Writes out the two mismatched items
    to outLine
  */
{
  std::stringstream cx;
  cx<<"MisMatch<>"<<std::endl;
  cx<<getErr()<<" Item A!=B "<<Aval<<
    " "<<Bval<<" ";
  OutLine=cx.str();
  return;
}

//-------------------------
// TypeMatch
//-------------------------

TypeMatch::TypeMatch(const std::string& A,
		     const std::string& B,
		     const std::string& Place) :
  ExBase(0,Place),AName(A),BName(B) 
  /*!
    Constructor store two mismatched items
    \param A :: Item to store
    \param B :: Item to store
    \param Place :: Reason/Code item for error
  */
{
  setOutLine();
}

TypeMatch::TypeMatch(const TypeMatch& A) :
  ExBase(A),AName(A.AName),BName(A.BName)
  /*!
    Copy Constructor
    \param A :: MisMatch to copy
   */
{}

TypeMatch&
TypeMatch::operator=(const TypeMatch& A) 
  /*!
    Assignment operator
    \param A :: Object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      ExBase::operator=(A);
    }
  return *this;
}


void
TypeMatch::setOutLine()
  /*!
    Writes out the two mismatched items
    to outLine
  */
{
  std::stringstream cx;
  cx<<"TypeMatch:"<<std::endl;
  cx<<getErr()<<" Names: "<<AName
    <<" "<<BName<<" ";
  OutLine=cx.str();
  return;
}

//-------------------------
// InvalidLine
//-------------------------

InvalidLine::InvalidLine(const std::string& Place,
			 const std::string& L,const size_t P) :
  ExBase(0,Place),pos(P),Line(L)
  /*!
    Constructor of an invalid line
    \param Place :: Reason/Code item for error
    \param L :: Line causing the error
    \param P :: Positions of error
  */
{
  setOutLine();
}


InvalidLine::InvalidLine(const InvalidLine& A) :
  ExBase(A),pos(A.pos),Line(A.Line)
  /*!
    Copy constructor 
    \param A :: InvalidLine to copy
  */
{}

InvalidLine&
InvalidLine::operator=(const InvalidLine& A) 
  /*!
    Assignment operator
    \param A :: InvalidLine to copy
    \return *this
  */
{
  if (this!=&A)
    {
      ExBase::operator=(A);
      pos=A.pos;
      Line=A.Line;
    }
  return *this;
}

void
InvalidLine::setOutLine()
  /*!
    Writes out the line and positions of the error
    to the OutLine
  */
{
  std::stringstream cx;
  cx<<"InvalidLine"<<std::endl;
  cx<<getErr()<<"\n Line: "<<Line<<" @ "<<pos;
  OutLine=cx.str();
  return;
}


//--------------------
// CastError
//--------------------

template<typename Ptr>
CastError<Ptr>::CastError(const Ptr* B,
			  const std::string& Place)  :
  ExBase(0,Place),Base(B)
  /*!
    Constructor of an invalid line
    \param B :: Point of base object
    \param Place :: Reason/Code item for error
  */
{
  setOutLine();
}

template<typename Ptr>
CastError<Ptr>::CastError(const CastError<Ptr>& A) :
  ExBase(A),Base(A.Base)
  /*!
    Copy constructor 
    \param A :: CastError to copy
  */
{}

template<typename Ptr>
CastError<Ptr>&
CastError<Ptr>::operator=(const CastError<Ptr>& A) 
  /*!
    Assignment operator
    \param A :: CastError to copy
    \return *this
  */
{
  if (this!=&A)
    {
      ExBase::operator=(A);
      Base=A.Base;
    }
  return *this;
}

template<typename Ptr>
void
CastError<Ptr>::setOutLine()
  /*!
    Writes out the line and positions of the error
    to the OutLine
  */
{
  std::stringstream cx;
  cx<<"CastError:"<<typeid(Base).name()<<"\n";
  cx<<"\nCast Obj[Address]: "<<reinterpret_cast<long int>(Base);
  cx<<"\n";
  cx<<"Reason :: "<<getErr()<<std::endl;
  OutLine=cx.str();
  return;
}

//--------------------
// TypeConvError
//--------------------

template<typename T,typename U>
TypeConvError<T,U>::TypeConvError(const T& B,
				  const std::string& Place)  :
  ExBase(0,Place),ABase(B)
  /*!
    Constructor of an invalid line
    \param B :: Point of base object
    \param Place :: Reason/Code item for error
  */
{
  setOutLine();
}

template<typename T,typename U>
TypeConvError<T,U>::TypeConvError(const TypeConvError<T,U>& A) :
  ExBase(A),ABase(A.ABase)
  /*!
    Copy constructor 
    \param A :: TypeConvError to copy
  */
{}

template<typename T,typename U>
TypeConvError<T,U>&
TypeConvError<T,U>::operator=(const TypeConvError<T,U>& A) 
  /*!
    Assignment operator
    \param A :: TypeConvError to copy
    \return *this
  */
{
  if (this!=&A)
    {
      ExBase::operator=(A);
      ABase=A.ABase;
    }
  return *this;
}

template<typename T,typename U>
void
TypeConvError<T,U>::setOutLine()
  /*!
    Writes out the line and positions of the error
    to the OutLine
  */
{
  std::stringstream cx;
  cx<<"TypeConvError:"<<typeid(T).name()
    <<" -> "<<typeid(U).name()<<"\n";
  cx<<"\nCast Obj: "<<ABase;
  cx<<"\n";
  cx<<"Reason :: "<<getErr()<<std::endl;
  OutLine=cx.str();
  return;
}


  
//--------------------
// DynamicConv
//--------------------


DynamicConv::DynamicConv(const std::string& B,const std::string& D,
			 const std::string& Place)  :
  ExBase(0,Place),Base(B),Derived(D)
  /*!
    Constructor of an invalid line
    \param B :: Base class
    \param D :: Derived Class
    \param Place :: Reason/Code item for error
  */
{
  setOutLine();
}

DynamicConv::DynamicConv(const DynamicConv& A) :
  ExBase(A),Base(A.Base),Derived(A.Derived)
  /*!
    Copy constructor
    \param A :: Object to copy
  */
{}

DynamicConv&
DynamicConv::operator=(const DynamicConv& A) 
  /*!
    Assignment operator
    \param A :: Object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      ExBase::operator=(A);
      Base=A.Base;
      Derived=A.Derived;
    }
  return *this;
}

void
DynamicConv::setOutLine()
  /*!
    Writes out the range and limits
    to outline
  */
{
  std::stringstream cx;
  cx<<"DynamicConv:"<<getErr()<<"::\n"
    <<Base<<"  ==>> "<<Derived;
  OutLine=cx.str();
  return;
}


  
  
// ----------------------------------------------
//           CommandError
// ----------------------------------------------

CommandError::CommandError(const std::string& C,const std::string& Place) :
  ExBase(0,Place),Cmd(C)
  /*!
    Constructor 
    \param C :: Command that failed
    \param Place :: Description string
   */
{
  setOutLine();
}

CommandError::CommandError(const CommandError& A) :
  ExBase(A),Cmd(A.Cmd)
  /*!
    Copy constructor
    \param A :: Object to copy
  */
{}

CommandError&
CommandError::operator=(const CommandError& A) 
  /*!
    Assignment operator
    \param A :: Object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      ExBase::operator=(A);
      Cmd=A.Cmd;
    }
  return *this;
}

void
CommandError::setOutLine()
  /*!
    Writes out the range and limits
    to outline
  */
{
  std::stringstream cx;
  cx<<"CommandError:"<<getErr()<<" ::"<<Cmd;
  OutLine=cx.str();
  return;
}

// ----------------------------------------------
//           ConstructionError
// ----------------------------------------------

ConstructionError::ConstructionError(const std::string& N,
				     const std::string& Place) :
  ExBase(0,Place),Name(N)
  /*!
    Constructor 
    \param N :: Name of object/thing that failed
    \param Place :: Description string
   */
{
  setOutLine();
}

  
ConstructionError::ConstructionError(const std::string& N,
				     const std::string& Place,
				     const std::string& M) :
  ExBase(0,Place),Name(N),method(M)
  /*!
    Constructor 
    \param N :: Name of object/thing that failed
    \param Place :: Description string
    \param M :: Method
   */
{
  setOutLine();
}

ConstructionError::ConstructionError(const std::string& N,
				     const std::string& Place,
				     const std::string& M,
				     const std::string& IP1) :
  ExBase(0,Place),Name(N),method(M),input({IP1})
  /*!
    Constructor 
    \param N :: Name of object/thing that failed
    \param Place :: Description string
    \param M :: Method
    \param IP1 :: Input 1
   */
{
  setOutLine();
}

ConstructionError::ConstructionError(const std::string& N,
				     const std::string& Place,
				     const std::string& M,
				     const std::string& IP1,
				     const std::string& IP2) :
  ExBase(0,Place),Name(N),method(M),input({IP1,IP2})
  /*!
    Constructor 
    \param N :: Name of object/thing that failed
    \param Place :: Description string
    \param M :: Method
    \param IP1 :: Input 1
    \param IP2 :: Input 2
   */
{
  setOutLine();
}

ConstructionError::ConstructionError(const std::string& N,
				     const std::string& Place,
				     const std::string& M,
				     const std::string& IP1,
				     const std::string& IP2,
				     const std::string& IP3) :
								     
  ExBase(0,Place),Name(N),method(M),input({IP1,IP2,IP3})
  /*!
    Constructor 
    \param N :: Name of object/thing that failed
    \param Place :: Description string
    \param M :: Method
    \param IP1 :: Input 1
    \param IP2 :: Input 2
    \param IP3 :: Input 3
   */
{
  setOutLine();
}

  
ConstructionError::ConstructionError(const ConstructionError& A) :
  ExBase(A),Name(A.Name),method(A.method),input(A.input)
  /*!
    Copy constructor
    \param A :: Object to copy
  */
{}

ConstructionError&
ConstructionError::operator=(const ConstructionError& A) 
  /*!
    Assignment operator
    \param A :: Object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      ExBase::operator=(A);
      Name=A.Name;
      method=A.method;
      input=A.input;
    }
  return *this;
}

void
ConstructionError::setOutLine()
  /*!
    Writes out the range and limits
    to outline
  */
{
  std::stringstream cx;
  cx<<"ConstructionError:"<<getErr()<<" ::\n"
    <<"Name:"<<Name<<"\n"
    <<"Method :"<<method<<"\n";
  if (input.empty())
    cx<<"Input Void";
  else
    {
      cx<<"Input :::";
      for(const std::string& F : input)
	cx<<F<<":";
      cx<<"::";
    }
  OutLine=cx.str();
  return;
}


// ----------------------------------------------
//           Abstract object function
// ----------------------------------------------

AbsObjMethod::AbsObjMethod(const std::string& Place) :
  ExBase(0,Place)
  /*!
    Constructor 
    \param Place :: Description string
  */
{
  setOutLine();
}

AbsObjMethod::AbsObjMethod(const AbsObjMethod& A) :
  ExBase(A)
  /*!
    Copy constructor
    \param A :: Object to copy
  */
{}

AbsObjMethod&
AbsObjMethod::operator=(const AbsObjMethod& A) 
  /*!
    Assignment operator
    \param A :: Object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      ExBase::operator=(A);
    }
  return *this;
}

void
AbsObjMethod::setOutLine()
  /*!
    Writes out the range and limits
    to outLine
  */
{
  std::stringstream cx;
  cx<<"AbsObjMethod:"<<getErr();
  OutLine=cx.str();
  return;
}

// ----------------------------------------------
//           Numercial Abort
// ----------------------------------------------

NumericalAbort::NumericalAbort(const std::string& Place) :
  ExBase(0,Place)
  /*!
    Constructor 
    \param Place :: Description string
  */
{
  setOutLine();
}

NumericalAbort::NumericalAbort(const NumericalAbort& A) :
  ExBase(A)
  /*!
    Copy constructor
    \param A :: Object to copy
  */
{}

NumericalAbort&
NumericalAbort::operator=(const NumericalAbort& A) 
  /*!
    Assignment operator
    \param A :: Object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      ExBase::operator=(A);
    }
  return *this;
}

void
NumericalAbort::setOutLine()
  /*!
    Writes out the range and limits
    to outLine
  */
{
  std::stringstream cx;
  cx<<"NumericalAbort:"<<getErr();
  OutLine=cx.str();
  return;
}

ExitAbort::ExitAbort(const std::string& Err,const int flag) :
  std::exception(),fullPath(flag),OutLine(Err),
  CodeLocation(ELog::RegMethod::getFull())
  /*!
    Constructor
    \param Err :: Class:method string
    \param flag :: Extract a full calling path info 
  */
{ }

ExitAbort::ExitAbort(const ExitAbort& A) :
  std::exception(A),fullPath(A.fullPath),
  OutLine(A.OutLine),CodeLocation(A.CodeLocation)
  /*!
    Copy constructor
    \param A :: ExitAbort object to copy
  */
{}

ExitAbort&
ExitAbort::operator=(const ExitAbort& A)
  /*!
    Assignment operator
    \param A :: ExitAbort object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      std::exception::operator=(A);
      fullPath=A.fullPath;
      OutLine=A.OutLine;
      CodeLocation=A.CodeLocation;
    }
  return *this;
}

const char*
ExitAbort::what() const throw()
   /*!
     Write out the code position using the RegMethod system
     \return char* to the string values
   */
{
  static std::string Item;

  Item=OutLine+
    "\nExit Stack:\n"+CodeLocation;
  return Item.c_str();
}

}   // NAMESPACE ColErr

///\cond TEMPLATE

namespace Geometry { 
  class Surface;
  class Face;
  class Edge;
  class Vertex;
  class Vec3D;
}
 
namespace SDef { class SrcBase; }
namespace TimeData { class WorkSpace; }
namespace mainSystem { class IItemBase; }
namespace ModelSupport { class ObjSurfMap; }
namespace MonteCarlo
{ 
  class Material;
  class Object; 
}


template class ColErr::EmptyValue<MonteCarlo::Material*>;
template class ColErr::EmptyValue<MonteCarlo::Object*>;
template class ColErr::EmptyValue<Geometry::Surface*>;
template class ColErr::EmptyValue<Geometry::Face*>;
template class ColErr::EmptyValue<Geometry::Edge*>;
template class ColErr::EmptyValue<void>;
template class ColErr::EmptyValue<int>;
template class ColErr::EmptyValue<long int>;
template class ColErr::EmptyValue<size_t>;
template class ColErr::EmptyValue<unsigned int>;
template class ColErr::EmptyValue<unsigned int*>;
template class ColErr::EmptyValue<int*>;
template class ColErr::EmptyValue<long int*>;
template class ColErr::EmptyValue<size_t*>;
template class ColErr::EmptyValue<double*>;
template class ColErr::EmptyValue<std::string*>;
template class ColErr::EmptyValue<std::string>;
template class ColErr::EmptyValue<Geometry::Vec3D*>;
template class ColErr::EmptyValue<ModelSupport::ObjSurfMap*>;
template class ColErr::IndexError<std::string>;
template class ColErr::IndexError<double>;
template class ColErr::IndexError<int>;
template class ColErr::IndexError<unsigned int>;
template class ColErr::IndexError<long int>;
template class ColErr::IndexError<size_t>;
template class ColErr::SizeError<size_t>;
template class ColErr::SizeError<double>;
template class ColErr::InContainerError<Geometry::Vec3D>;
template class ColErr::InContainerError<std::string>;
template class ColErr::InContainerError<int>;
template class ColErr::InContainerError<long int>;
template class ColErr::InContainerError<unsigned long int>;
template class ColErr::InContainerError<char>;
template class ColErr::InContainerError<double>;
template class ColErr::MisMatch<double>;
template class ColErr::MisMatch<int>;
template class ColErr::MisMatch<unsigned int>;
template class ColErr::MisMatch<long int>;
template class ColErr::MisMatch<unsigned long int>;
template class ColErr::ArrayError<2>;
template class ColErr::DimensionError<4,long int>;
template class ColErr::CastError<mainSystem::IItemBase>;
template class ColErr::CastError<TimeData::WorkSpace>;
template class ColErr::CastError<SDef::SrcBase>;
template class ColErr::CastError<Geometry::Surface>;
template class ColErr::CastError<void>;
template class ColErr::OrderError<double>;
template class ColErr::RangeError<double>;
template class ColErr::RangeError<int>;
template class ColErr::RangeError<long int>;
template class ColErr::RangeError<size_t>;
template class ColErr::TypeConvError<Geometry::Vec3D,double>;
template class ColErr::TypeConvError<double,Geometry::Vec3D>;

///\endcond TEMPLATE
 
