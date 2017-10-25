/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   include/version.h
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
#ifndef version_h
#define version_h

/*!
  \class version 
  \author S. Ansell
  \date May 2009
  \version 1.0
  \brief Maintains an external file for a version number
 */

class version
{
 private:

  std::string VFile;                     ///< File Name
  std::string buildTag;                  ///< github tag if found
  int vNum;                              ///< Number 

  version();
  version(const version&);
  version& operator=(const version&);

 public:

  ~version() {}      ///< Destructor
  
  static version& Instance();

  int setFile(const std::string&);
  int getVersion() const { return vNum; } ///< Access Vnum
  std::string getBuildTag() const;
  int getIncrement();

  void write() const;
};

#endif
