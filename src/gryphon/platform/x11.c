#include "gryphon/platform/x11.h"
#include "glad.h"
#include "GL/glx.h"
#include "arena.h"
#include "profile.h"
#include "string8.h"
#include "gryphon/gryphon.h"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <unistd.h>

struct gry_window {
	Display *dpy;
	Window x_window;
	XEvent event;
	GLXFBConfig fb_cfg;
	GLXContext opengl_ctx;
	XVisualInfo *vi;
	i32 scren_num;
	u32 width;
	u32 height;
	b8 should_close;
};

static void x11_err(string8 err) {
	static const char prefix[] = "[x11] error: ";
	write(2, prefix, sizeof(prefix) - 1);
	write(2, err.data, err.length);
	write(2, "\n", 1);
}

static GLXContext x11_create_core_ctx(Display *dpy, GLXFBConfig fb_cfg) {
	PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB =
			(PFNGLXCREATECONTEXTATTRIBSARBPROC)glXGetProcAddressARB(
					(const GLubyte *)"glXCreateContextAttribsARB");
	assert(glXCreateContextAttribsARB && "GLX_ARB_create_context not supported");

	int context_attribs[] = {GLX_CONTEXT_MAJOR_VERSION_ARB,
							 4,
							 GLX_CONTEXT_MINOR_VERSION_ARB,
							 6,
							 GLX_CONTEXT_PROFILE_MASK_ARB,
							 GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
#ifndef NDEBUG
							 GLX_CONTEXT_FLAGS_ARB,
							 GLX_CONTEXT_DEBUG_BIT_ARB,
#endif
							 None};

	GLXContext ctx = glXCreateContextAttribsARB(dpy, fb_cfg, NULL, True, context_attribs);
	if(!ctx) {
		x11_err(string8_lit("[x11] Failed to create OpenGL 4.6 core context\n"));
		return NULL;
	}
	return ctx;
}

gry_window *platform_create_window(arena *a, u32 w, u32 h, string8 title) {
	begin_time_function;
	Display *dpy = XOpenDisplay(NULL);
	if(dpy == NULL) {
		x11_err(string8_lit("X Display could not be created\n"));
		return NULL;
	}

	gry_window *win = arena_push_struct_zero(a, gry_window);
	u32 screen = DefaultScreen(dpy);
	Window root = DefaultRootWindow(dpy);

	// Query the  GLX version
	i32 major;
	i32 minor;
	b8 ok = glXQueryVersion(dpy, &major, &minor);
	if(!ok) {
		x11_err(string8_lit("Unable to query the GLX version.\n"));
	}

	i32 attributes[] = {
			GLX_X_RENDERABLE,
			True,
			GLX_DRAWABLE_TYPE,
			GLX_WINDOW_BIT,
			GLX_RENDER_TYPE,
			GLX_RGBA_BIT,
			GLX_DOUBLEBUFFER,
			True,
			GLX_RED_SIZE,
			8,
			GLX_GREEN_SIZE,
			8,
			GLX_BLUE_SIZE,
			8,
			GLX_ALPHA_SIZE,
			8,
			GLX_DEPTH_SIZE,
			24,
			GLX_STENCIL_SIZE,
			8,
			None,
	};

	i32 config_count = 0;
	GLXFBConfig *configs = glXChooseFBConfig(dpy, screen, attributes, &config_count);
	if(config_count <= 0) {
		x11_err(string8_lit("Unable to find any framebuffer configurations"));
	}
	GLXFBConfig cfg = configs[0];

	if(cfg == NULL) {
		x11_err(string8_lit("No suitible framebuffer config found.\n"));
		return NULL;
	}

	// Get XVisual
	XVisualInfo *vi = glXGetVisualFromFBConfig(dpy, cfg);
	XSetWindowAttributes attr = {0};
	i32 x = 0;
	i32 y = 0;
	i32 border_width = 0;
	attr.colormap = XCreateColormap(dpy, root, vi->visual, AllocNone);
	attr.event_mask = ExposureMask | KeyPressMask;
	Window x_window =
			XCreateWindow(dpy, root, x, y, w, h, border_width, vi->depth, InputOutput,
						  vi->visual, CWEventMask | CWColormap, &attr);
	GLXContext ctx = x11_create_core_ctx(dpy, cfg);
	glXMakeCurrent(dpy, x_window, ctx);

	// Initialize glad
	if(!gladLoadGL((GLADloadfunc)glXGetProcAddressARB)) {
		x11_err(string8_lit("Failed to initialize glad"));
		return NULL;
	}

	win->opengl_ctx = ctx;
	win->dpy = dpy;
	win->width = w;
	win->height = h;

	int fb_width = win->width;
	int fb_height = win->height;
	glViewport(0, 0, fb_width, fb_height);

	win->vi = vi;
	win->fb_cfg = cfg;
	win->x_window = x_window;
	win->scren_num = screen;
	win->should_close = false;
	XMapWindow(win->dpy, win->x_window);
	XStoreName(win->dpy, win->x_window, (char *)title.data);
	end_time_function;
	return win;
}

void platform_poll_events(gry_window *win) {
	while(XPending(win->dpy)) {
		XNextEvent(win->dpy, &win->event);
		switch(win->event.type) {
		case KeyPress:
			if(XLookupKeysym(&win->event.xkey, 0) == XK_Escape) {
				win->should_close = true;
			}
		}
	}
}

b8 platform_window_should_close(gry_window *win) {
	return win->should_close;
}

void platform_swap_buffers(gry_window *win) {
	glXSwapBuffers(win->dpy, win->x_window);
}

void platform_close_window(gry_window *win) {
	XUnmapWindow(win->dpy, win->x_window);
	XDestroyWindow(win->dpy, win->x_window);
}
