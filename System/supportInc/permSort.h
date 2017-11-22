/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   supportInc/permSort.h
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
#ifndef mathSupport_permSort_h
#define mathSupport_permSort_h

namespace mathSupport
{

template<typename T,typename Compare>
std::vector<size_t>
sortPermutation(const std::vector<T>& mainVec,
		const Compare& compare)
  /*!
    Grouped permuation system 
    [Taken from stackoverflow: 17074324 - Timothy Shields]
    Uses the trick of sorting the index vector and leaving
    the main vector untouched(!)
    \param mainVec :: main vector of components to sort
    \param compare :: comparison operator
    \return index list
  */
{
  std::vector<size_t> index(mainVec.size());
  for(size_t i=0;i<mainVec.size();i++)
    index[i]=i;
  std::sort(index.begin(),index.end(),
            [&](const size_t i, const size_t j)
	    { 
	      return compare(mainVec[i],mainVec[j]);
	    });
  return index;
}

template <typename T>
void
applyPermutation(std::vector<T>& mainVec,
                 const std::vector<std::size_t>& index)
  /*!
    Grouped permuation system 
    [Taken from stackoverflow: 17074324 - Timothy Shields =>
    Modifed to process mainVec in place.
    Process the vector file
    \param mainVec :: main vector of components to return sorted
    \param index :: list of positions
  */
{
  std::vector<T> outVec;
  outVec.reserve(mainVec.size());

  for(size_t i=0;i<index.size();i++)
    outVec[i]=std::move(mainVec[index[i]]);

  // move back to original container [ care as need to set size]
  for(size_t i=0;i<index.size();i++)
    mainVec[i]=std::move(outVec[i]);
  
  return;
}


}  // NAMESPACE mathSupport


#endif
