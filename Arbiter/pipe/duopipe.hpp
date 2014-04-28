/* -------------------------------------------
 * duopipe - duopipe.cpp
 * -------------------------------------------
 * Implements the 'duopipe' class, which wraps two unix pipes into an object
 * providing a two interlinked iopipes.
 * -------------------------------------------
 * Author:  James Payor (github.com/amoebius)
 * -------------------------------------------
 */

#ifndef __DUOPIPE_HPP
#define __DUOPIPE_HPP


#include "iopipe.hpp"


namespace cpipe {

	/* duopipe:
	 * Wraps two pipes into a 'front' and 'back' end.
	 **************************************************/

	class duopipe {
	public:
		// Instantiates two new pipes and builds a duopipe from them:
		duopipe();
		// Construct a duopipe NOT bound to any new pipes:
		explicit duopipe(int);
		
		// Reuses the pipes of another duopipe (copy constructor):
		duopipe(const duopipe& other);
		// Builds a duopipe from the front and back ends of the two specified pipes:
		duopipe(iopipe first, iopipe second);

		// Reuses the pipes of another duopipe for assignment operations:
		duopipe& operator = (const duopipe& other);

		// Returns the front end, represented as an iopipe:
		const iopipe& front() const;
		// Returns the back end, represented as an iopipe:
		const iopipe& back() const;

		// Binds stdin and stdout to the front end of the duopipe:
		void bind_front();
		// Binds stdin and stdout to the back end of the duopipe:
		void bind_back();

		// Indicates whether the duopipe is currently open:
		const bool is_open() const;
		// Closes both ends of the pipe:
		void close();

		// (NB:  The 'front'/'back' distinction is arbitrary, but the different ends
		//       need to be referred to somehow...)

	private:
		// The internal front and back iopipes:
		iopipe _front, _back;
		bool isOpen;
	};

}

#endif // __DUOPIPE_HPP
