/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/UTest.cxx
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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>


std::string
getSlice(const int N,const int F,const int A,const int B,const int S)
{
  std::ostringstream cx;
  if (N<1)
    {
      cx<<" "<<A<<" "<<B;
      return cx.str();
    }

  int first;

  cx<<" "<<((!F) ? A : S);
  int i;
  for(i=F;i<N-1;i+=2)
    cx<<" ( "<<-(S+i)<<" : "<<S+i+1<<" )";
  if (i==N)
    cx<<" "<<B;
  else
    cx<<" "<<-(S+i);

  return  cx.str();
}

int
main()
{
  std::cout<<getSlice(4,0,100,-200,31)<<std::endl;
  std::cout<<getSlice(4,1,100,-200,31)<<std::endl;
  std::cout<<getSlice(5,0,100,-200,31)<<std::endl;
  std::cout<<getSlice(5,1,100,-200,31)<<std::endl;
  return 0;
}
