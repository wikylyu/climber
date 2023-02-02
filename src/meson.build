climber_sources = [
  'main.c',
  'style.c',
  'message.c',
  'climber-application.c',
  'climber-window.c',
  'climber-preferences-dialog.c',
  'climber-service-switch.c',
  'climber-log-view.c',
  'climber-window-statusbar.c',
  'io/climber-service.c',
  'io/climber-tunnel.c',
  'io/address.c',
  'io/http/request.c',
  'io/http/response.c',
  'io/http/common.c',
  'io/mtop/request.c',
]

climber_deps = [
  dependency('gtk4'),
  dependency('libadwaita-1'),
  dependency('libsoup-3.0')
]

climber_sources += gnome.compile_resources('climber-resources',
  'climber.gresource.xml',
  c_name: 'climber'
)

executable('climber', climber_sources,
  dependencies: climber_deps,
       install: true,
)
