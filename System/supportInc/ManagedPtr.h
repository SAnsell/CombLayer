/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   supportInc/ManagedPtr.h
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
#ifndef ManagedPtr_h
#define ManagedPtr_h

/*!
  \class ManagedPtr
  \brief Holds a point as a scopable object
  \version 1.0
  \author S. Ansell
  \date November 2006

  The principle is that we manage a pointer,
  when becomes free then it becomes is drops out of
  scope. It is then deleted. The advantage of this construction
  is that the ptr is able to be unmanaged/passed to another
  responsible.

  \todo Check if this can be done with traits? 
*/

template<typename T>
class ManagedPtr
{
 private:
  
  int managed;            ///< Do we delete this
  T* Ptr;                 ///< Pointer

  /// Private Copy Constructors:
  ManagedPtr(const ManagedPtr&);
  /// Private Assignment operator
  ManagedPtr& operator=(const ManagedPtr&);   

 public:

  typedef T element_type;   ///< Stored type

  /// Constructor
  explicit ManagedPtr(T* APtr) : managed(1),Ptr(APtr)
    {}

  /// Assignment operator
  ManagedPtr& operator=(T* APtr) 
    {
      if (APtr!=Ptr)
	{
	  if (managed) delete Ptr;
	  managed=1;
	  Ptr=APtr;
	}
      return *this;
    }

  /// Deletion
  ~ManagedPtr() { if (managed) delete Ptr; }

  operator bool () const { return (Ptr); }         ///< Existance     
  T& operator*() const { return *Ptr; }            ///< Dereference   
  T* operator->() const { return Ptr; }            ///< De-reference  
  bool operator!() const { return (Ptr) ? 0 : 1; } ///< ! existing    
  T* get() const { return Ptr; }                   ///< accessor      
  void unManage() { managed=0; }                   ///< Dont delete   
  void Manage() { managed=1; }                     ///< Allow delete  
  void setManaged(const int I) { managed = I; }    ///< Set Manged state
  void reset() { if (managed) delete Ptr; Ptr=0; } ///< Clear points  
  
};


/*!
  \class ManagedPtrBlock
  \brief Holds a block of points as a scope-able object
  \version 1.0
  \author S. Ansell
  \date November 2006

  This holds a new assigned block. When the ManagePtrBlock
  drops out of scope, the ptr is deleted. However, the ptr,
  ownership can be passed to another object when required.
*/

template<typename T>
class ManagedPtrBlock
{
 private:
 
  int managed;       ///< Do we delete this
  T* Ptr;            ///< Pointer
  /// Private Copy constructor
  ManagedPtrBlock(const ManagedPtrBlock&);
  /// Private Assignment opertor
  ManagedPtrBlock& operator=(const ManagedPtrBlock&);

 public:

  typedef T element_type;              ///< storage type

  /// Constructor
  explicit ManagedPtrBlock(T* APtr) : managed(1),Ptr(APtr) {}
  /// Assignment operator
  ManagedPtrBlock<T>& operator=(T* APtr) 
    {
      if (APtr!=Ptr)
	{
	  if (managed) delete [] Ptr;
	  managed=1;
	  Ptr=APtr;
	}
      return *this;
    }
  /// Destructor
  ~ManagedPtrBlock() { if (managed) delete [] Ptr; }   

  operator bool () const { return (Ptr); }              ///< Existance     
  T& operator*() const { return *Ptr; }                 ///< Dereference
  T* operator->() const { return Ptr; }                 ///< De-reference
  T* get() const { return Ptr; }                        ///< accessor 
  bool operator!() const { return (Ptr) ? 0 : 1; }      ///< ! existing    
  void unMangage() { managed=0; }                       ///< Dont delete   
  void Mangage() { managed=1; }                         ///< Allow delete  
  void setManaged(const int I) { managed = I; }         ///< Set Manged st 
  void reset() { if (managed) delete [] Ptr; Ptr=0; }   ///< Clear points  
                                                        
};

#endif
