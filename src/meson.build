climber_sources = [
  'main.c',
  'style.c',
  'message.c',
  'climber-application.c',
  'climber-window.c',
  'climber-preferences-dialog.c',
  'climber-service-switch.c',
  'io/climber-service.c',
]

climber_deps = [
  dependency('gtk4'),
  dependency('libadwaita-1'),
]

climber_sources += gnome.compile_resources('climber-resources',
  'climber.gresource.xml',
  c_name: 'climber'
)

executable('climber', climber_sources,
  dependencies: climber_deps,
       install: true,
)
