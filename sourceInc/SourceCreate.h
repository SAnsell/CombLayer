/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   sourceInc/SourceCreate.h
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
#ifndef SourceCreate_h
#define SourceCreate_h

namespace SDef
{
  class Source;
  void createGaussianSource(Source&,const double,const double,const double);  

  void createSimpleSource(Source&,const double,const double);
  void createBilbaoSource(const FuncDataBase&,Source&);
  void createESSSource(const FuncDataBase&,Source&);
  void createTS1Source(const FuncDataBase&,Source&);
  void createTS1GaussianSource(const FuncDataBase&,Source&);
  void createTS1GaussianNewSource(const FuncDataBase&,Source&);   // Goran
  void createTS1MuonSource(const FuncDataBase&,Source&); // Goran
  void createTS1EpbCollSource(const FuncDataBase&,Source&);   // Goran    
  void createSinbadSource(const FuncDataBase&,Source&);
  void createTS2Source(Source&);
}


#endif
 
