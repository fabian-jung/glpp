#include "glpp/glpp.hpp"

#include <iostream>

namespace glpp {

std::string error_description(GLenum err) {
	switch(err) {
		case GL_NO_ERROR:
			return "GL_NO_ERROR: No error has been recorded. The value of this symbolic constant is guaranteed to be 0. ";
		case GL_INVALID_ENUM:
			return "GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag.  ";
		case GL_INVALID_VALUE:
			return "GL_INVALID_VALUE: A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag.  ";
		case GL_INVALID_OPERATION:
			return "GL_INVALID_OPERATION: The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag.  ";
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			return
			"GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete."
			"The offending command is ignored and has no other side effect than to set the error flag.";
		case GL_OUT_OF_MEMORY:
			return "GL_OUT_OF_MEMORY: There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded. . ";
		case GL_STACK_UNDERFLOW:
			return "GL_STACK_UNDERFLOW: An attempt has been made to perform an operation that would cause an internal stack to underflow. ";
		case GL_STACK_OVERFLOW:
			return "GL_STACK_OVERFLOW: An attempt has been made to perform an operation that would cause an internal stack to overflow. ";
		default:
			return "No Description";
	}
}

namespace detail {
	void check() {
		const auto err = glGetError();
		if(err != GL_NO_ERROR) {
			throw std::runtime_error(error_description(err));
		}
	}

}

}
