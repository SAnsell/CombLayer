/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   teaSetInc/makeTeaSet.h
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
#ifndef teaSetSystem_makeTeaSet_h
#define teaSetSystem_makeTeaSet_h


/*!
  \namespace teaSetSystem
  \brief simple teaSetmodl
  \version 1.0
  \date May 2015
  \author S. Ansell
*/

namespace teaSetSystem
{
  class Mug;
  /*!
    \class makeTeaSet
    \version 1.0
    \author S. Ansell
    \date May 2015
    \brief General teaSet building system
  */

class makeTeaSet
{
 private:


  std::shared_ptr<teaSetSystem::Mug> ATube;   ///< pre-tube


 public:
  
  makeTeaSet();
  makeTeaSet(const makeTeaSet&);
  makeTeaSet& operator=(const makeTeaSet&);
  ~makeTeaSet();
  
  void build(Simulation*,const mainSystem::inputParam&);

};

}

#endif
