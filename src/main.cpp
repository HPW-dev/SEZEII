#include "main-templ.hpp"

SDL_MAIN {
  parse_args(argc, argv);
  auto [plugin, pligin_info] {load_plugin()};
  auto [reader, reader_ctx] {init_render(pligin_info)};
  auto [vp_framebuffer, framebuffer_info] {init_thread_buffers(*reader)};
  seze::total_frames = get_total_frames(reader_ctx);
  fix_output_resolution(reader_ctx);
  shared_p<seze::Writer> writer {
    seze::nout
    ? nullptr
    : make_shared_p<seze::Writer>(seze::oname,
      framebuffer_info, seze::width, seze::height,
      reader_ctx, seze::simple_encoder)
  };
  auto [window, surface, event] {init_sdl()};

  while ( !seze::is_end) {
    proc_sdl(event);

    if (seze::is_pause)
      continue;

    // SEZE proc:
    read_to_bufs(reader, vp_framebuffer);
    proc_bufs(plugin, vp_framebuffer);
    if ( !seze::nrend)
      draw_bufs(window, surface, vp_framebuffer);
    if ( !seze::nout)
      write_to_file(*writer, vp_framebuffer);

    print_progress();
  }

  plugin->finalize();
  free_sdl(surface, window);
  return EXIT_SUCCESS;
}
