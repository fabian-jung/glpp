#pragma once

#include <GL/glew.h>
#include <string>
#include <stdexcept>

namespace glpp {

template <class result_t, class... gl_args_t, class... args_t>
result_t call(result_t (*fun)(gl_args_t...), args_t... args);

namespace detail {
	void check();
}

template <class result_t, class... gl_args_t, class... args_t>
result_t call(result_t (*fun)(gl_args_t...), args_t... args) {
	if constexpr(!std::is_same_v<result_t, void>) {
		auto result = fun(std::forward<args_t>(args)...);
		#ifndef NDEBUG
			detail::check();
		#endif
		return result;
	} else {
		fun(std::forward<args_t>(args)...);
		#ifndef NDEBUG
			detail::check();
		#endif
	}
}

}
