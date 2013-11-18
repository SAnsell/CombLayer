/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   include/RefControl.h
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
#ifndef RefControl_h
#define RefControl_h

/*!
  \class RefControl
  \brief Impliments a reference counted data template 
  \version 1.0
  \date February 2006
  \author S.Ansell
  
  This version works only on data that is created via new().
  It is thread safe and works in the Standard template 
  libraries (but appropiate functionals are needed for 
  sorting etc.).

  The underlying data can be accessed via the normal pointer
  semantics but call the access function if the data is required
  to be modified.
*/

template<typename dataTYPE>
class RefControl
{
 private:

  /// typedef for a point to data
  typedef boost::shared_ptr<dataTYPE> ptrTYPE;   
  ptrTYPE Data;                                  ///< Real object Ptr

 public:

  RefControl();
  explicit RefControl(const dataTYPE&);
  RefControl(const RefControl<dataTYPE>&);      
  RefControl<dataTYPE>& operator=(const RefControl<dataTYPE>&);

  ~RefControl();


  /// Pointer dereference access
  const dataTYPE& operator*() const { return *Data; }  
  /// indirectrion dereference access	
  const dataTYPE* operator->() const { return Data.get(); } 
  /// Equality (via ptr)
  bool operator==(const RefControl<dataTYPE>& A) { return Data==A.Data; }
  /// Inequality (via ptr) 
  bool operator!=(const RefControl<dataTYPE>& A) { return Data!=A.Data; }
  dataTYPE& access();
  /// Unique access
  bool unique() const { return Data.unique(); }
};

template<typename dataTYPE>
RefControl<dataTYPE>::RefControl() :
  Data(new dataTYPE())
  /*!
    Constructor : creates new data() object
  */
{ }

template<typename dataTYPE>
RefControl<dataTYPE>::RefControl(const dataTYPE& A) :
  Data(new dataTYPE(A))
  /*!
    Constructor : creates new data() object
    \param A :: Object to COPY : it does not assume the
    ownership of this object
  */
{ }


template<typename dataTYPE>
RefControl<dataTYPE>::RefControl(const RefControl<dataTYPE>& A) :
  Data(A.Data)
  /*!
    Copy constructor : double references the data object
    \param A :: object to copy
  */
{ }

template<typename dataTYPE>
RefControl<dataTYPE>&
RefControl<dataTYPE>::operator=(const RefControl<dataTYPE>& A) 
  /*!
    Assignment operator : double references the data object
    maybe drops the old reference.
    \param A :: object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Data=A.Data;
    }
  return *this;
}

template<typename dataTYPE>
RefControl<dataTYPE>::~RefControl()
  /*!
    Destructor : No work is required since Data is
    a shared_ptr.
  */
{}

template<typename dataTYPE>
dataTYPE&
RefControl<dataTYPE>::access()
  /*!
    Access function 
    Creates a copy of Data so that it can be modified.
    Believed to be thread safe sicne
    creates an extra reference before deleteing.
    \return new copy of *this
  */
{
  if (Data.unique())
    return *Data;

  ptrTYPE oldData=Data; 
  Data.reset();
  Data=ptrTYPE(new dataTYPE(*oldData));
  return *Data;
}


#endif
