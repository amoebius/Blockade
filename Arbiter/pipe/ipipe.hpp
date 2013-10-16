/* -------------------------------------------
 * ipipe - ipipe.hpp
 * -------------------------------------------
 * Declares the interface for the 'ipipe' class, which manages unix pipe input
 * via a stream interface.
 * -------------------------------------------
 * Author:  James Payor (github.com/amoebius)
 * -------------------------------------------
 */

// See definition for available functionality, and "iopipe.hpp" for example usage.

#ifndef __IPIPE_HPP
#define __IPIPE_HPP


#include <istream>
#include <ext/stdio_filebuf.h> // NB: Non-standard, see note in "iopipe.hpp".
typedef __gnu_cxx::stdio_filebuf<char> fdbuffer;  // Buffer that can wrap a POSIX file descriptor.


namespace cpipe {

	/* ipipe:
	 * Wraps the input end of a pipe.
	 *********************************/
	class ipipe {

	private:
		// An internally reference-counted stream attached to the file.
		class ipipe_ref {
		public:
			// Constructor from POSIX file descriptor:
			explicit ipipe_ref(int fd);
			// Destructor:
			~ipipe_ref();

			// Increment reference count:
			void inc();
			// Decrement reference count:
			void dec();

			// Return the file descriptor managed by this object:
			const int file() const;

			// Provide stream extraction operator:
			template <typename T>
			inline std::istream& operator >> (T& rhs) {
				return stream >> rhs; // NB: Because reference counting, this *should* always be valid.
			}

			// Cast to an istream:
			operator std::istream& ();

		private:
			// Reference counter and file descriptor:
			int instances, fd;
			// Internal buffer that can be constructed from a file descriptor:
			fdbuffer buffer;
			// istream to wrap the buffer:
			std::istream stream;
		};

	public:
		// Constructs an ipipe with no opened pipe:
		ipipe();
		// Constructs an ipipe to manage the given file descriptor:
		explicit ipipe(int fd);
		// Copy constructor:
		ipipe(const ipipe& other);
		// Assignment:
		ipipe& operator = (const ipipe& other);
		// Desctructor:
		~ipipe();

		// Attaches to a file descriptor:
		void open(int fd);
		// Detaches from the file descriptor:
		void close();
		// Binds the given file descriptor (standard input by default) to the pipe:
		void bind(int fd = STDIN_FILENO);
		// Returns the file descriptor managed by the object:
		const int file() const;
		// Returns whether the object is currently managing a file:
		const bool is_open() const;

		// Cast to an istream:
		operator std::istream& () const;

		// Provide stream extraction operator:
		template <typename T>
		inline std::istream& operator >> (T& rhs) const {
			if(!isOpen) throw std::ios_base::failure("Input pipe not open.");
			return *pipe >> rhs;  // NB: Invalid if in a closed state.
		}

	private:
		// Reference to the internal stream:
		ipipe_ref *pipe;
		// Open state:
		bool isOpen;
	};


} // namespace pipe


#endif // __IPIPE_HPP
