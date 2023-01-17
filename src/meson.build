climber_sources = [
  'main.c',
  'climber-application.c',
  'climber-window.c',
  'climber-preferences-dialog.c',
  'style.c',
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
