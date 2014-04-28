/* -------------------------------------------
 * opipe - opipe.hpp
 * -------------------------------------------
 * Declares the interface for the 'opipe' class, which manages unix pipe output
 * via a stream interface.
 * -------------------------------------------
 * Author:  James Payor (github.com/amoebius)
 * -------------------------------------------
 */

// See definition for available functionality, and "iopipe.hpp" for example usage.

#ifndef __OPIPE_HPP
#define __OPIPE_HPP


#include <ostream>
#include <unistd.h>
#include <ext/stdio_filebuf.h> // NB: Non-standard, see note in "iopipe.hpp".
typedef __gnu_cxx::stdio_filebuf<char> fdbuffer;  // Buffer that can wrap a POSIX file descriptor.


namespace cpipe {

	/* opipe:
	 * Wraps the output end of a pipe.
	 ***********************************/
	class opipe {

	private:
		// An internally reference-counted stream attached to the file.
		class opipe_ref {
		public:
			// Constructor from POSIX file descriptor:
			explicit opipe_ref(int fd);
			// Destructor:
			~opipe_ref();

			// Increment reference count:
			void inc();
			// Decrement reference count:
			void dec();

			// Return the file descriptor managed by this object:
			const int file() const;

			// Provide stream insertion operator:
			template <typename T>
			inline std::ostream& operator << (const T& rhs) {
				return stream << rhs; // NB:  Because reference counting, this *should* always be valid.
			}

			// Support std::endl and other manipulators:
			inline std::ostream& operator << (std::ostream&(*manipulator)(std::ostream&)) {
				return stream << manipulator;
			}


			// Cast to an ostream:
			operator std::ostream& ();

		private:
			// Reference counter and file descriptor:
			int instances, fd;
			// Internal buffer that can be constructed from a file descriptor:
			fdbuffer buffer;
			// ostream to wrap the buffer:
			std::ostream stream;
		};

	public:
		// Constructs an opipe with no opened pipe:
		opipe();
		// Constructs an opipe to manage the given file descriptor:
		explicit opipe(int fd);
		// Copy constructor:
		opipe(const opipe& other);
		// Assignment:
		opipe& operator = (const opipe& other);
		// Destructor:
		~opipe();

		// Attaches to a file descriptor:
		void open(int fd);
		// Detaches from the file descriptor:
		void close();
		// Binds the file descriptor (standard output by default) to the pipe:
		void bind(int fd = STDOUT_FILENO);
		// Binds to stderr:
		void bind_err();
		// Returns the file descriptor managed by the object:
		const int file() const;
		// Returns whether the object is currently managing a file:
		const bool is_open() const;

		// Cast to an ostream:
		std::ostream& get_ostream() const;
		operator std::ostream& () const;

		// Provide stream insertion operator:
		template <typename T>
		inline opipe& operator << (const T& rhs) {
			if(!isOpen || !write_success) {
				write_success = false;
			} else {
				write_success = (*pipe << rhs);
			}
			return *this;
		}

		// Support std::endl and other manipulators:
		inline opipe& operator << (std::ostream&(*manipulator)(std::ostream&)) {
			if(!isOpen || !write_success) {
				write_success = false;
			} else {
				write_success = (*pipe << manipulator);
			}
			return *this;
		}


		inline operator bool () const {
			return write_success;
		}

	private:
		// Reference to the internal stream:
		opipe_ref *pipe;
		// Open state:
		bool isOpen;
		// 'Was last operation successful' boolean:
		bool write_success;
	};


} // namespace cpipe


#endif // __OPIPE_HPP
