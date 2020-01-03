# glpp

This is a thin wrapper library for modern OpenGL in C++. All of its functions and classes can be used in conjunction with native
OpenGL calls. For a quick dive into the library you can check out the examples or read the documentation provided below.

[![The first triangle](doc/01.first_triangle.png)](example/01.first_triangle)  | [![Key and Mouse Actions](doc/02.input.png)](example/02.input)   | [![Loading Textures](doc/03.texture.png)](example/03.texture)
:---:|:---:|:---:
[![A simple version of the breakout game](doc/04.breakout.png)](example/04.breakout) | [![The first 3D Object](doc/05.cube.png )](example/05.cube) | [![Using framebuffers to implement a depth of field effect](doc/06.framebuffer.png)](example/06.framebuffer)

# Dependencies

* OpenGl 4.5 Core Profile
* GLEW
* GLFW3

# Installing

```bash
cd <glpp project dir>
mkdir build
cd build
cmake ..
make install
```

# Building
In you install the library a glppConfig.txt is created. The easiest way to link  against glpp is using cmake. You can use ```find_package(glpp)``` to find the library. It will export two targets. ```glpp::core``` which contains the OpenGl wrappers and ```glpp::system``` which contains a wrapper for window creation.

```cmake
find_package(glpp REQUIRED)
target_link_libraries(your_target PRIVATE glpp::core glpp::system)
```

# Error Handling

Native OpenGl lacks support for proper error handling. Faulty calls into the API result in silent failover or program termination.
In order enable proper error handling in debug mode ```glpp::call()``` is introduced. This method takes a handle to a API call and
the necessary arguments. If DEBUG mode is enabled ```glpp::call()``` will throw an exception containing an error description.

```c++
#include <glpp/glpp.hpp>

int main() {
//set up context
  glpp::call(glBindTexture,42,1337); // This will throw a std::runtime_error with the message:
  // "GL_INVALID_OPERATION: The specified operation is not allowed in the current state.
  // The offending command is ignored and has no other side effect than to set the error flag."
  return 0;
}
```
The precondition for ```glpp::call()``` is, that there are no OpenGl errors present on the error stack. I recommend to wrap all calls into the
OpenGl API with this wrapper to prevent any unnoticed errors.

# Higher level abstractions

## System

The system module is not included in the glpp::core library and contains the window class. This class removes all the boilerplate code
to create a window with a OpenGl context. If you need a more fine grained controll over the context creation, i recommend to fall back
to GLFW or whatever library you want. You dont need to link against the system module in this case.

```c++
#include <glpp/system/window.hpp>

int main(int argc,	char **argv) {
	glpp::system::window_t window(800, 600, "example", glpp::system::window_t::vsync_t::off);
	window.enter_main_loop([&]() {
  	glpp::call(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Whatever you want to render
  });
  return 0;
}
```

The ```window_t``` class comes with an ```input_handler_t``` attribute, which can be used to register callbacks to mouse and keyboard events. A reference to this attribute can be aquired with the ```input_handler()``` method. The input_handler can be set as a whole or single callbacks can be registered. Multithreading is not supported by the window class. Changes to the input_handler shall only be done from the main thread (thread that holds the window object) or from the inside of input callback functions.

```c++
#include <glpp/system/window.hpp>
#include <iostream>

int main(int argc, char **argv) {
	glpp::system::window_t window(800, 600, "example", glpp::system::window_t::vsync_t::off);
	window.input_handler().set_keyboard_action(key_t::escape, action_t::press, [&](int) {
		std::cout << "The escape key has been pressed." << std::endl;
		// Close App on Esc key press
		window.close();
	});

	window.input_handler().set_mouse_action(
		mouse_button_t::left, action_t::press,
		[&](double x, double y, int) {
			std::cout << "The left mouse button was clicked on pixel "
					  << x ", " << y << "." << std::endl;
		}
	);

	window.input_handler().set_mouse_action(
		mouse_button_t::left,
		action_t::release, [&](double x, double y, int) {
			std::cout << "The cursor moved to " << x ", " << y << "." << std::endl;
		}
	);

	return 0;
}
```

## Buffer
The ```glpp::object::buffer_t``` is a wrapper for OpenGl buffers. The class constructor allocates video memory and copys transfers the data. The initialisation of a static buffer for vertex data is shown in the example.
```c++
std::array<glm::vec3, 3> data;
buffer_t<glm::vec3> buffer(
	buffer_target_t::array_buffer,
	data.data(), data.size(),
	buffer_usage_t::static_draw
);
buffer.bind();
// Use the buffer
```
## Vertex Array Object
To be able to use buffers for rendering vertex array objects should be used. The OpenGl vao's are wrapped by ```glpp::object::vertex_array_t```. Because buffer_t has a template parameter, that encodes the data layout of the buffer, the vao can deduce the parameters for stride, elements per vertex and type in trivial cases. This can also
be overwritten if the buffer data shall be used in a different way.

```c++
std::array<glm::vec3, 3> data;
buffer_t<glm::vec3> buffer(
	buffer_target_t::array_buffer,
	data.data(), data.size(),
	buffer_usage_t::static_draw
);
vertex_array_t vao;
vao.attach(buffer, 0);
vao.bind();
// Call glDrawArrays or other render function
```

## Shader

This library comes with a wrapper for shader objects and shader programs. The shader objects ```glpp::object:shader_t``` takes a shader type and source code on construction and deals with the allocation and compilation of the shader and its code. The ```glpp::object:shader_program_t``` takes ```shader_t``` and linkes them together. The used shader objects can be deleted after linking. The shader programm can be used to set uniforms of shaders. The ```set_uniform()``` method is strongly typed and takes trivial types and vectors or matrices from the glm library as arguments.

```c++

shader_program_t get_shader_program() {
	shader_t vertex(
		shader_type_t::vertex,
		"#version 330 core\n\
		layout (location = 0) in vec3 aPos;\n\
		\n\
		void main()\n\
		{\n\
			gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n\
		}"
	);

	shader_t fragment(
		glpp::object::shader_type_t::fragment,
		"#version 330 core\n\
		uniform vec4 color;\n\
		out vec4 FragColor;\n\
		\n\
		void main()\n\
		{\n\
			FragColor = color;\n\
		}"
	);
	return shader_program_t(vertex, fragment);
	// vertex and fragment shader objects are freed after the linking.
}

int main() {
//[...]
	auto shader = get_shader_program();
	shader.set_uniform("color", glm::vec4(1.0, 0.0, 0.0, 1.0));
//[...]
}
```

## Model, View and Renderer

To render primitives there glpp implements an additional layer of abstraction. The ```glpp::render::renderer_t``` can render ```glpp::render::view_t```. The ```view_t``` encapsulates the buffer and vertex array objects and can be crated from ```glpp::render::model_t```. The implemented ```model_t``` does only provide a method to add verticies to the buffer. To get a more useful model class for you application you can simply inherit from the modal and add new features.

Both ```model_t``` and and ```view_t``` have a template argument ```vertex_description_t```. This argument is a POD struct, that is a representation of the datalayout in the buffer. The different attributes can be activated in the view by passing pointer-to-member of the attributes to the constructor of the view. The order of these arguments has to match the order in the vertex shader in order to bind them to the correct attributes.


```C++
#include <glpp/object/shader.hpp>
#include <glpp/render/renderer.hpp>
#include <glpp/render/model.hpp>
#include <glpp/render/view.hpp>
#include <glpp/system/window.hpp>

#include <glm/glm.hpp>

struct uniform_description_t {
	glm::vec4 color;
};

struct vertex_shader_t : public glpp::object::shader_t {
	vertex_shader_t() :
		glpp::object::shader_t(
			glpp::object::shader_type_t::vertex,
			"#version 330 core\n\
			//the vertex shader takes a vec3 for attribute location 0 \
			layout (location = 0) in vec3 aPos;\n\
			\n\
			void main()\n\
			{\n\
				gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n\
			}"
		)
	{}
};

struct fragment_shader_t : public glpp::object::shader_t {
	fragment_shader_t() :
		glpp::object::shader_t(
			glpp::object::shader_type_t::fragment,
			"#version 330 core\n\
			uniform vec4 color;\n\
			out vec4 FragColor;\n\
			\n\
			void main()\n\
			{\n\
				FragColor = color;\n\
			}"
		)
	{}
};

struct vertex_description_t {
   // the vertex description has to have at least one vec3,
   // because of the attributes in the vertex shader
	glm::vec3 position;
};

class model_t : public glpp::render::model_t<vertex_description_t> {
public:
	// We inherit from model_t to add our simple add_triangle helper method
	void add_triangle(
		const glm::vec3& first,
		const glm::vec3& second,
		const glm::vec3& third
	) {
		m_verticies.reserve(m_verticies.size()+3);
		m_verticies.emplace_back(vertex_description_t{first});
		m_verticies.emplace_back(vertex_description_t{second});
		m_verticies.emplace_back(vertex_description_t{third});
	}
};


int main(
	int argc,
	char **argv
) {

	glpp::system::window_t window(800, 600, "example", glpp::system::window_t::vsync_t::off);
	glpp::render::renderer_t<uniform_description_t> renderer{
		vertex_shader_t(),
		fragment_shader_t()
	};

	// before we can set a uniform for the first time, we have to register its name
	renderer.set_uniform_name( &uniform_description_t::color, "color" );
	renderer.set_uniform( &uniform_description_t::color, glm::vec4(1.0f, 0.5f, 0.2f, 1.0f));

	// Create a model
	model_t model;
	// Add our geometry
	model.add_triangle(
		glm::vec3{-1.0, -1.0, 0.0},
		glm::vec3{1.0, -1.0, 0.0},
		glm::vec3{0.0, 1.0, 0.0}
	);

	// Initilise the view using our custom model
	glpp::render::view_t<vertex_description_t> view(
		model,
		// use the glm::vec3 for the attribute on location 0,
		// which is "layout (location = 0) in vec3 aPos;"
		&vertex_description_t::position
	);

	glpp::call(glClearColor, 0.5,0.0,0.0,1.0);
	window.enter_main_loop([&]() {
		glpp::call(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// use the renderer to render our view
		renderer.render(view);
	});

	return 0;
}
```

![screenshot of the rendered triangle from the example](doc/01.first_triangle.png)

## Textures

Nearly all types of 2D textures are supported by glpp at the moment. Support for 1D and 3D textures, is planned but not yet implemented. Images can be loaded from files if the glpp::image is linked to the binary.

The initialisation and use of textures happens in four steps. The first step is to load the image data into memory. The datatype to hold the pixeldata in memory is ```glpp::object::image_t``` and is templated with one type. This type encodes the layout of the pixeldata. Currently ```float```, ```double```, ```char```, ```int``` and the vector types of the glm library are supported. There are C++17 template deduction guides implemented for most constructors of ```image_t```.

```C++
glpp::object::image_t<glm::vec3> pixel_data("smiley.png");
```

The second step is the set up of the texture object on the GPU. This is done by the constructor of  ```glpp::object::texture_t```. ```texture_t``` takes a const-reference to a ```image_t``` and a couple of optional parameters to describe the layout on the GPU, behaviour of the filters and mipmaps.

```C++
glpp::object::texture_t texture_one(
	pixel_data, // You may use a tempory image_t here to release the memory after the data has been loaded to the gpu
	glpp::object::image_format_t::prefered, // this format will give you 8-bit resolution for each color channel.
	glpp::object::clamp_mode_t::clamp_to_edge,
	glpp::object::filter_mode_t::linear
);
```

To be able to reuse textures for different shaders without reloading them for each frame, OpenGl exposes texture units/texture slots. The textures are bound to the texture units, which in turn can be bound to shader programms. If your device supports OpenGl 4.5, there are at least 80 of them available. The binding of a texture to a texture unit is represented by the ```texture_slot_t```. The binding happens by either calling the constructor ```texture_slot_t``` with the reference to the texture you want to bind. The more readable alternative is the call to ```texture_t::bind_to_texture_slot()```, which will return a ```texture_slot_t```. As long as the ```texture_slot_t``` object is in scope, the binding of the texture to the texture unit will persist. All texture units are managed by ```texture_slot_t``` under the assumtion, that there are no texture units bound without the call into ```texture_slot_t```.

```C++
auto texture_unit_one = texture_one.bind_to_texture_slot();
```

The final step is to tell the shader programm, which texture units it has to use to fill its samplers. This is done by ```shader_program_t::set_texture()```. If the ```renderer_t``` class is used, the ```set_texture()``` can be called on the renderer. After this step you can use the texture in your shader.

```C++
renderer.set_texture("texture_one" , texture_unit_one);
```

The final result of this example will look like this if projected to a quad:
![screenshot of the rendered triangle from the example](doc/03.texture.png)

Because bindeless state-access is used to set up the texture, newly created textures are not bound to any texture unit. Therefore you have to bind every ```texture_t``` to a texture unit and the shader in use. Ommiting one of the steps will cause undefined behaviour, which will crash the programm in the best case.

## Framebuffer

Custom framebuffers can be created and bound with the ```framebuffer_t``` type. The use is straight forward. Create a instance of the class with either the dimensions or a list of textures, that shall be attatched to the framebuffer. Once the instance is created, additional textures can be attatched. To render to this frambuffer, simply bind it via the ```bind()``` method. To use the default framebuffer again a static method ```bind_default_framebuffer()``` is provided.
