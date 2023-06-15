#pragma once

struct GLFWwindow;

namespace evo
{
	class Window {
	public:
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		Window(int width, int height, const char* title);
		~Window();

		Window(Window&& w) noexcept : m_window(w.m_window) {
			w.m_window = nullptr;
		}

		void initialize_imgui();
		void framebuffer_size(int& width, int& height);
		void swap_buffers();
		void make_context_current();
		void close();
		bool closing();
		void set_size(int width, int height);
		void set_title(const char* title);
		void set_key_callback(void (*)(GLFWwindow*, int, int, int, int));
		void set_mouse_callback(void (*)(GLFWwindow*, int, int, int));
		void set_scroll_callback(void (*)(GLFWwindow*, double, double));
		void set_input_mode(int mode, int value);
		void cursor_position(double& x, double& y);
		void size(int& width, int& height);

	private:
		GLFWwindow* m_window;
	};
}