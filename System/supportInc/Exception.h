/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   supportInc/Exception.h
 *
 * Copyright (c) 2004-2024 by Stuart Ansell
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
#ifndef ColErr_Exception_h
#define ColErr_Exception_h

/*!
  \namespace ColErr
  \brief All the exceptions that make sense to trap
  \author S. Ansell
  \version 1.0
  \date February 2006
  
*/

namespace ColErr
{

/*!
  \class ExBase
  \brief Exception Base class
  \author Stuart Ansell
  \date Sept 2005

  Base class of all exceptions. The main
  virtual function is the getErrorStr 
  which returns the reporting information.
*/
 
class ExBase : public std::exception
{
 private:

  int state;             ///< Type of error
  std::string ErrLn;     ///< String causing error

 protected:

  std::string OutLine;       ///< Output string (so it is persistant)
  std::string CodeLocation;  ///< Code location

 public:

  ExBase(const int,const std::string&);
  ExBase(const std::string&);
  ExBase(const ExBase&);
  ExBase& operator=(const ExBase&);
  ~ExBase() throw() override {}  ///< Destructor 

  /// Main reporting method
  const char* what() const throw() override;
  
  /// Return the error number 
  int getErrorNum() const { return state; }    
  /// Get the error range
  const std::string& getErr() const { return ErrLn; }
};

 
/*!
  \class EmptyContainer
  \brief Exception for a container in an empty state
  \author Stuart Ansell
  \date May 2016
  \version 1.0
*/

class EmptyContainer : public ExBase
{
 private:

  void setOutLine();

 public:

  EmptyContainer(const std::string&);
  EmptyContainer(const EmptyContainer&);
  EmptyContainer& operator=(const EmptyContainer&);
  ~EmptyContainer() throw() override {}   ///< Destructor 

};

/*!
  \class IndexError
  \brief Exception for index errors
  \author Stuart Ansell
  \date Sept 2005
  \version 1.0

  Called when an index falls out of range
*/
template<typename T>
class IndexError : public ExBase
{
 private:

  const T Val;     ///< Actual value called 
  const T maxVal;  ///< Maximum value

  void setOutLine();

 public:

  IndexError(T ,T ,const std::string&);
  IndexError(const IndexError& A);
  IndexError& operator=(const IndexError&);
  ~IndexError() throw() override {}   ///< Destructor 

};


/*!
  \class SizeError
  \brief Exception for index errors
  \author Stuart Ansell
  \date November 2022
  \version 1.0

  Called when an index is not linearly
  ordered.
*/
template<typename T>
class LinearError : public ExBase
{
 private:

  std::vector<T> Val;     ///< Actual value called 
  void setOutLine();

 public:

  LinearError(std::vector<T>,const std::string&);
  LinearError(const LinearError& A);
  LinearError& operator=(const LinearError&);
  ~LinearError() throw() override {}   ///< Destructor 

};

/*!
  \class SizeError
  \brief Exception for index errors
  \author Stuart Ansell
  \date November 2015
  \version 1.0

  Called when an index falls out of range
*/

template<typename T>
class SizeError : public ExBase
{
 private:

  const T Val;     ///< Actual value called 
  const T minVal;  ///< Minimum value

  void setOutLine();

 public:

  SizeError(const T&,const T&,const std::string&);
  SizeError(const SizeError& A);
  SizeError& operator=(const SizeError&);
  ~SizeError() throw() override {}   ///< Destructor 

};


/*!
  \class FileError
  \brief Exception for file problems
  \author Stuart Ansell
  \date Sept 2005
  \version 1.0

  Records the filename and the point of failure.

*/
class FileError : public ExBase
{
 private:

  const std::string fileName;   ///< File causing the problem

  void setOutLine();

 public:

  FileError(const int,std::string ,const std::string&);
  FileError(const FileError&);
  FileError& operator=(const FileError&);
  ~FileError() throw() override {}   ///< Destructor 

};

/*!
  \class UninitializedValue
  \brief Exception for an object in a void state
  \author Stuart Ansell
  \date April 2025
  \version 1.0
*/

class UninitializedValue : public ExBase
{
 private:

  void setOutLine(const std::string&);

 public:

  UninitializedValue(const std::string&,const std::string&);
  UninitializedValue(const UninitializedValue&);
  UninitializedValue& operator=(const UninitializedValue&);
  ~UninitializedValue() throw() override {}   ///< Destructor 

};

/*!
  \class EmptyValue
  \brief Exception for an object in a void state
  \author Stuart Ansell
  \date August 2008
  \version 1.0
*/

template<typename T>
class EmptyValue : public ExBase
{
 private:

  void setOutLine();

 public:

  EmptyValue(const std::string&);
  EmptyValue(const EmptyValue&);
  EmptyValue<T>& operator=(const EmptyValue<T>&);
  ~EmptyValue() throw() override {}   ///< Destructor 

};

/*!
  \class InContainerError
  \brief Exception for an object not in a container
  \author Stuart Ansell
  \date Sept 2005
  \version 1.0

  Records the object being looked for

*/
template<typename T>
class InContainerError : public ExBase
{
 private:

  const T SearchObj;           ///< Search string

  void setOutLine();

 public:

  InContainerError(T ,const std::string&);
  InContainerError(const InContainerError&);
  InContainerError<T>& operator=(const InContainerError<T>&);
  ~InContainerError() throw() override {}   ///< Destructor 

  /// Accessor to search item
  const T& getItem() const { return SearchObj; }
};

/*!
  \class RangeError
  \brief Error Range in an array/list etc
  \author Stuart Ansell
  \date October 2005
  \version 1.0

  Records the object being looked for
  and the range required.
*/
template<typename T>
class RangeError : public ExBase
{
 private:

  const T Index;         ///< Current value
  const T minV;          ///< Min Value
  const T maxV;          ///< Max Value

  void setOutLine();

 public:

  RangeError(const T&,const T&,const T&,const std::string&);
  RangeError(const RangeError<T>& A);
  RangeError<T>& operator=(const RangeError<T>&);
  ~RangeError() throw() override {}   ///< Destructor 
 
};

/*!
  \class OrderError
  \brief Error in order of number
  \author Stuart Ansell
  \date November 2016
  \version 1.0

  Records the two items A should be <= B
*/
 
template<typename T>
class OrderError : public ExBase
{
 private:

  const T lowValue;         ///< Current value
  const T highValue;          ///< Min Value

  void setOutLine();

 public:

  OrderError(const T&,const T&,const std::string&);
  OrderError(const OrderError<T>& A);
  OrderError<T>& operator=(const OrderError<T>&);
  ~OrderError() throw() override {}   ///< Destructor 
 
};

/*!
  \class DimensionError
  \brief Error in the sized for a multi-dimension array
  \author Stuart Ansell
  \date October 2015
  \version 1.0

  Records the sizes and accessed values for the array 
*/
template<typename T>
class DimensionError : public ExBase
{
 private:

  std::vector<T> indexSize;         ///< Index search components
  std::vector<T> reqSize;           ///< required size

  void setOutLine();

 public:

  DimensionError(std::vector<T>,std::vector<T>,const std::string&);  
  DimensionError(const DimensionError<T>&);
  DimensionError<T>& operator=(const DimensionError<T>&);
  ~DimensionError() throw() override {}  ///< Destructor

};

/*!
  \class ArrayError
  \brief Error Range in an array/list etc
  \author Stuart Ansell
  \date October 2024
  \version 1.0

  Given an array , dimension requested in error
*/

class ArrayError : public ExBase
{
 private:

  size_t axis;                 ///< Axis looked for 
  size_t index;                ///< index aimed for
  std::vector<size_t> indexSize;         ///< Index search components

  void setOutLine();

 public:

  ArrayError(const size_t,const size_t,
	     std::vector<size_t>,
	     const std::string&);
  ArrayError(const ArrayError&) =default;
  ArrayError& operator=(const ArrayError&) =default;
  virtual ~ArrayError() throw() {}  ///< Destructor
};

/*!
  \class MisMatch
  \brief Error when two numbers should be identical (or close)
  \author Stuart Ansell
  \date October 2005
  \version 1.0

  Records the object being looked for
  and the range required.
*/
template<typename T>
class MisMatch : public ExBase
{
 private:

  const T Aval;        ///< Number A 
  const T Bval;        ///< container size

  void setOutLine();
 
 public:

  MisMatch(const T&,const T&,const std::string&);


  MisMatch(const MisMatch<T>& A);
  MisMatch<T>& operator=(const MisMatch<T>&);
  ~MisMatch() throw() override {}   ///< Destructor

};

/*!
  \class TypeMatch
  \brief Error if types do not match
  \author Stuart Ansell
  \date February 2012
  \version 1.0
*/

class TypeMatch : public ExBase
{
 private:

  const std::string AName;   ///< First type
  const std::string BName;   ///< Second type

  void setOutLine();
 
 public:

  TypeMatch(std::string ,std::string ,
	    const std::string&);


  TypeMatch(const TypeMatch& A);
  TypeMatch& operator=(const TypeMatch&);
  ~TypeMatch() throw() override {}   ///< Destructor
};


/*!
  \class InvalidLine
  \brief For a parser error on a line
  \author Stuart Ansell
  \date October 2005
  \version 1.0

  Stores the position on the line that the error occured
  as well as the line
*/

class InvalidLine : public ExBase
{
 private:
  
  size_t pos;        ///< Position of error
  std::string Line;    ///< Error Line

  void setOutLine();

 public:

  InvalidLine(const std::string&,std::string ,const size_t =0);
  InvalidLine(const InvalidLine&);
  InvalidLine& operator=(const InvalidLine&);
  ~InvalidLine() throw() override {}   ///< Destructor

};

/*!
  \class CastError
  \brief Dynamic Cast problems
  \author Stuart Ansell
  \date October 2006
  \version 1.0

  When a pointer cant by case
*/

template<typename Ptr>
class CastError : public ExBase
{
 private:
  
  Ptr const* Base;     ///< Storage to base pointer

  void setOutLine();

 public:

  CastError(const Ptr*,const std::string&);


  CastError(const CastError<Ptr>&);
  CastError<Ptr>& operator=(const CastError<Ptr>&);
  ~CastError() throw() override {}    ///< Destructor

};

/*!
  \class TypeConvError
  \brief Dynamic Cast problems
  \author Stuart Ansell
  \date October 2006
  \version 1.0

  When a pointer cant by case
*/

template<typename T,typename U>
class TypeConvError : public ExBase
{
 private:

  T ABase;                 ///< Base Item or Ptr
  void setOutLine();

 public:

  TypeConvError(T ,const std::string&);

  TypeConvError(const TypeConvError<T,U>&);
  TypeConvError<T,U>& operator=(const TypeConvError<T,U>&);
  ~TypeConvError() throw() override {}    ///< Destructor

};

/*!
  \class DynamicConv
  \brief Dynamic Cast problems
  \author Stuart Ansell
  \date August 2015
  \version 1.0

  When failing to convert from type A to type B
*/

class DynamicConv : public ExBase
{
 private:

  std::string Base;          ///< Base clase 
  std::string Derived;       ///< New derived class
  void setOutLine();

 public:

  DynamicConv(std::string ,std::string ,const std::string&);

  DynamicConv(const DynamicConv&);
  DynamicConv& operator=(const DynamicConv&);
  ~DynamicConv() throw() override {}    ///< Destructor

};

/*!
  \class CommandError
  \brief Exception a command breaking in Command
  \author Stuart Ansell
  \date December 2006
  \version 1.0

  Called when a command cannot be executed

*/
class CommandError : public ExBase
{
 private:

  std::string Cmd;    ///< Command that failed to be intepreted

  void setOutLine();

 public:

  CommandError(std::string ,const std::string&);
  CommandError(const CommandError&);
  CommandError& operator=(const CommandError&);
  ~CommandError() throw() override {}    ///< Destructor

};

/*!
  \class ConstructionError
  \brief Exception for construction of object
  \author Stuart Ansell
  \date January 2015
  \version 1.0

  Called when a construction cannot be executed,
  e.g. initializing an object from string/input

*/
class ConstructionError : public ExBase
{
 private:

  std::string Name;                  ///< Name of object
  std::string method;                ///< Name of object method
  std::vector<std::string> input;    ///< Tokenized inputs

  void setOutLine();

 public:

  ConstructionError(std::string ,const std::string&);
  ConstructionError(std::string ,const std::string&,
		    std::string );
  ConstructionError(std::string ,const std::string&,
		    std::string ,const std::string&);
  ConstructionError(std::string ,const std::string&,
		    std::string ,const std::string&,
		    const std::string&);
  ConstructionError(std::string ,const std::string&,
		    std::string ,const std::string&,
		    const std::string&,const std::string&);

  ConstructionError(const ConstructionError&);
  ConstructionError& operator=(const ConstructionError&);
  ~ConstructionError() throw() override {}    ///< Destructor

};

/*!
  \class AbsObjMethod
  \brief Abstract base class method 
  \author Stuart Ansell
  \date April 2008
  \version 1.0
  Called when an abstract null object is called
*/
class AbsObjMethod : public ExBase
{
 private:

  void setOutLine();

 public:

  AbsObjMethod(const std::string&);
  AbsObjMethod(const AbsObjMethod&);
  AbsObjMethod& operator=(const AbsObjMethod&);
  ~AbsObjMethod() throw() override {}    ///< Destructor

};

/*!
  \class NumericalAbort
  \brief Abort do to uncontroled numerical position
  \author Stuart Ansell
  \date June 2010
  \version 1.0
  Called when a numerical problem develops
*/
class NumericalAbort : public ExBase
{
 private:

  void setOutLine();

 public:

  NumericalAbort(const std::string&);
  NumericalAbort(const NumericalAbort&);
  NumericalAbort& operator=(const NumericalAbort&);
  ~NumericalAbort() throw() override {}    ///< Destructor

};

/*!
 \class ExitAbort 
 \version 1.0
 \author S. Ansell
 \date January 2011
 \brief Allow an exit error all the way to the top level 
*/				
		
class ExitAbort : public std::exception
{
 private:

  int fullPath;        ///< Full path written
  std::string OutLine;       ///< Output string (so it is persistant)
  std::string CodeLocation;  ///< Code location

 public:

  ExitAbort(std::string ,const int =0);
  ExitAbort(const ExitAbort&);
  ExitAbort& operator=(const ExitAbort&);
  ~ExitAbort() throw() override {}    ///< Destructor

  /// Access to path flag [true if written]
  int pathFlag() const { return fullPath; }
  /// Main reporting method
  const char* what() const throw() override;

};


}  // NAMESPACE  ColErr

#endif
