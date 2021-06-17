#include <iostream>

#include "spec.hpp"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

void write_types(const std::string_view filename) {
    std::ofstream file(filename.cbegin());
    file <<
R"(
#pragma once

#include <cstddef>
#include <cstdint>

#ifndef WITH_GLEW

)";

    for(const auto& type : gl_types) {
        file << "using " << type.alias << " = " << type.native_type << ";\n";
    }

    file << 
R"(
#else
#include <GL/glew.h>
#endif
)";  
}

void write_constants(const std::string_view filename) {
    std::ofstream file(filename.cbegin());
    file <<
R"(
#pragma once  
#include "types.hpp"

#ifndef WITH_GLEW
)";
    for(const auto& constant : gl_constants) {
        file << "constexpr " << constant.type << " " << constant.name << " = " << constant.value << ";\n";
    }
    file <<
R"(
#else
#include <GL/glew.h>
#endif
)";
}

void write_arguments(std::ostream& stream, const std::vector<parameter_definition_t>& params) {
    if(params.size() > 0) {
        std::for_each(params.begin(), params.end()-1, [&](const auto& param){
            stream << param.type << " " << param.name << ", ";
        }); 
        stream << params.back().type << " " << params.back().name;
    }        
}

void write_apply_arguments(std::ostream& stream, const std::vector<parameter_definition_t>& params) {
    if(params.size() > 0) {
        std::for_each(params.begin(), params.end()-1, [&](const auto& param){
            stream <<param.name << ", ";
        }); 
        stream << params.back().name;
    }        
}
void write_functions(const std::string_view filename) {
    std::ofstream file(filename.cbegin());
    file <<
R"(
#pragma once

#include "types.hpp"

#ifndef WITH_GLEW
)";

    for(const auto& function : gl_functions) {
        file << function.result << " " << function.name << "(";
        write_arguments(file, function.arguments);
        file << ");\n";
    }
    file <<
R"(
#else
#include <GL/glew.h>
#endif
)";
}

void write_functions_impl(const std::string_view filename) {
    std::ofstream file(filename.cbegin());

    file <<
R"(
#include <glpp/gl/functions.hpp>
#include <glpp/gl/context.hpp>

#ifndef WITH_GLEW
)";

    for(const auto& function : gl_functions) {
        file << function.result << " " << function.name << "(";
        write_arguments(file, function.arguments);
        file << ") {\n";
        file << "   ";
        if(function.result != "void") {
            file << "return ";
        }
        file << "glpp::gl::context." << function.name << "(";
        write_apply_arguments(file, function.arguments);
        file <<");\n";
        file << "}\n";
    }
    file <<
R"(
#else
#include <GL/glew.h>
#endif
)";
}    
void write_context(const std::string_view filename) {
    std::ofstream file(filename.cbegin());
    file <<
R"(
#pragma once
#include "types.hpp"
#include <functional>
#include <ostream>

#ifndef WITH_GLEW

namespace gl {
template <class GlGetProcAddress>
void init(GlGetProcAddress&& glGetProcAddress);
}

namespace glpp::gl {
struct gl_context_t {
    template <class signature_t>
    using dynamic_function_t = std::function<signature_t>;

    gl_context_t() = default;

    void enable_throw();
    void enable_logging(std::ostream& out);

    template <class Function>
    void for_each(Function&& fun);

)";

     for(const auto& function : gl_functions) {
        file << "   dynamic_function_t<" << function.result << "(";
        write_arguments(file, function.arguments);
        file << ")> " << function.name << ";\n";
    }

    file <<
R"(    
};
inline thread_local gl_context_t context;

struct mock_context_t : public gl_context_t {
    mock_context_t();
};

template <class Function>
void gl_context_t::for_each(Function&& function) {
)";
    auto i = 0;
    for(const auto& function : gl_functions) {
        file << "   function(" << function.name << ", \"" << function.name << "\", " << i++ << ");\n";
    }
    file <<
R"(
}

}


namespace glpp {

template <class T>
struct signature_helper_t {};

template <class T>
struct signature_helper_t<std::function<T>> {
    using type = T;
};

template <class T>
using signature_t = typename signature_helper_t<T>::type;

template <class GlGetProcAddress>
void init(GlGetProcAddress&& glGetProcAddress) {
#ifdef WITH_GLEW
    glewExperimental = GL_TRUE;
	if(glewInit() != GLEW_OK) {
		throw std::runtime_error("OpenGL functions could not be loaded.");
	};
#else
    glpp::gl::context.for_each([glGetProcAddress](auto& fn, const char* name, int){
        using fn_t = signature_t<std::remove_reference_t<decltype(fn)>>;
        fn = reinterpret_cast<fn_t*>(glGetProcAddress(name));
    });
#endif
}

}

#endif
)";
}

void write_context_impl(const std::string_view filename) {
    std::ofstream file(filename.cbegin());
    file <<
R"(
#include <glpp/gl/types.hpp>
#include <glpp/gl/constants.hpp>
#include <glpp/gl/functions.hpp>
#include <glpp/gl/context.hpp>
#include <stdexcept>
namespace glpp::gl {

void gl_context_t::enable_throw() {

    for_each([](auto& fn, const char* name, int) {
        if(!fn) {
            fn = [name, fn](auto... values) -> decltype(fn(values...)) {
                throw std::runtime_error(std::string(name)+" is not loaded into runtime.");
            };
        }
    });

}

mock_context_t::mock_context_t() {
    for_each([](auto& fn, const char*, int) {
        fn = [](auto... args) -> decltype(fn(args...)) {
            using result_t = decltype(fn(args...));
            if constexpr(!std::is_same_v<result_t, void>) {
                return result_t{};
            }
        };
    });
}

template <class... Args>
void print(std::ostream& out, Args... args);

template <>
void print(std::ostream&) {}

template <class T>
void print(std::ostream& out, T v) {
    out << v << ":" << typeid(T).name();
}

template <class T, class U, class... Args>
void print(std::ostream& out, T v, U w, Args... args) {
    print(out,v);
    out << ", ";
    print(out, w, args...);
}

void gl_context_t::enable_logging(std::ostream& out) {

    for_each([&out](auto& fn, const char* name, int) {
        fn = [&out, f = std::move(fn), name](auto... args) -> decltype(fn(args...)) {
            out << name << "(";
            print(out, args...);
            out << ")\n";
            if constexpr(std::is_same_v<decltype(fn(args...)), void>) {
                f(args...);
            } else {
                return f(args...);
            }
        };
    });
}

}
)";
}


void create_base_header(const std::string_view filename) {
    std::ofstream file(filename.cbegin());
    file << 
R"(
#pragma once

#ifdef WITH_GLEW

#include <GL/glew.h>
#include <stdexcept>
#include <functional>

#else

#include "gl/types.hpp"
#include "gl/constants.hpp"
#include "gl/functions.hpp"

#endif
)";
}

void create_base_header_impl(const std::string_view filename) {
    std::ofstream file(filename.cbegin());
    file << 
R"(
#include "glpp/gl.hpp"

namespace glpp {
}

)";
}	

void create_directory(const std::string_view dir) {
    namespace fs = std::filesystem;
    if (!fs::is_directory(dir) || !fs::exists(dir)) {
        fs::create_directory(dir);
    }
}

int main(int, char*[]) {

    create_directory("src");
    create_directory("include");
    create_directory("include/glpp");
    create_directory("include/glpp/gl");
   
    write_types("include/glpp/gl/types.hpp");
    write_constants("include/glpp/gl/constants.hpp");
    write_functions("include/glpp/gl/functions.hpp");   
    write_functions_impl("src/functions.cpp");   
    write_context("include/glpp/gl/context.hpp"); 
    write_context_impl("src/context.cpp"); 
    create_base_header("include/glpp/gl.hpp");
    create_base_header_impl("src/gl.cpp");

    return 0;
}