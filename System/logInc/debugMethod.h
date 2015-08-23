/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   logInc/debugMethod.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#ifndef ELog_debugMethod_h
#define ELog_debugMethod_h

namespace ELog
{
  /*!
    \class debugMethod 
    \brief Counts number of debug unit to clear debug flag
    \author S. Ansell
    \date Februayr 2014
    \version 1.0
    
    This is based on scope BUT only activated scope!!
    i.e. it is at the top of the class BUT is activeated 
    and then unactivated.
  */

class debugMethod
{
 private:

  static int debugCnt;            ///< Current sum of ABase
  static std::vector<int> ABase;  ///< Singleton actives

  /// \cond NOWRITTEN
  debugMethod(const debugMethod&);
  debugMethod& operator=(const debugMethod&);
  /// \endcond NOWRITTEN

 public:

  debugMethod();
  void activate();
  void deactivate();
  ~debugMethod();

};

}

#endif
