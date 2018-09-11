virt_viewer_include += [include_directories('.')]
gnome = import('gnome')

virt_viewer_gresource = files('resources/virt-viewer.gresource.xml')
virt_viewer_resources = gnome.compile_resources('virt-viewer-resources',
                                                virt_viewer_gresource,
                                                source_dir : 'resources',
                                                c_name : 'virt_viewer')
virt_viewer_enums = gnome.mkenums_simple('virt-viewer-enums', sources : ['virt-viewer-display.h'])

# libvirt-viewer-util
libvirt_viewer_util_sources = [
  'virt-viewer-util.h',
  'virt-viewer-util.c',
]

libvirt_viewer_util_lib = static_library('virt-viewer-util', libvirt_viewer_util_sources,
                                         include_directories : virt_viewer_include,
                                         dependencies : virt_viewer_deps,
                                         install : false)

# libvirt-viewer
libvirt_viewer_sources = [
  virt_viewer_resources,
  virt_viewer_enums,
  'glib-compat.h',
  'glib-compat.c',
  'virt-viewer-auth.h',
  'virt-viewer-auth.c',
  'virt-viewer-app.h',
  'virt-viewer-app.c',
  'virt-viewer-file.h',
  'virt-viewer-file.c',
  'virt-viewer-session.h',
  'virt-viewer-session.c',
  'virt-viewer-display.h',
  'virt-viewer-display.c',
  'virt-viewer-notebook.h',
  'virt-viewer-notebook.c',
  'virt-viewer-window.h',
  'virt-viewer-window.c',
  'virt-viewer-vm-connection.h',
  'virt-viewer-vm-connection.c',
  'virt-viewer-timed-revealer.c',
  'virt-viewer-timed-revealer.h',
]

if virt_viewer_has_gtk_vnc
  libvirt_viewer_sources += ['virt-viewer-session-vnc.h',
                             'virt-viewer-session-vnc.c',
                             'virt-viewer-display-vnc.h',
                             'virt-viewer-display-vnc.c']
endif

if virt_viewer_has_spice_gtk
  libvirt_viewer_sources += ['virt-viewer-session-spice.h',
                             'virt-viewer-session-spice.c',
                             'virt-viewer-display-spice.h',
                             'virt-viewer-display-spice.c',
                             'virt-viewer-file-transfer-dialog.h',
                             'virt-viewer-file-transfer-dialog.c']
endif

if virt_viewer_has_ovirt
  libvirt_viewer_sources += ['ovirt-foreign-menu.h',
                             'ovirt-foreign-menu.c',
                             'remote-viewer-iso-list-dialog.c',
                             'remote-viewer-iso-list-dialog.h']
endif

libvirt_viewer_lib = static_library('virt-viewer', libvirt_viewer_sources,
                                    include_directories : virt_viewer_include,
                                    dependencies : virt_viewer_deps,
                                    install: false)

# remote-viewer
remote_viewer_sources = [
  virt_viewer_enums[1],
  'remote-viewer.h',
  'remote-viewer.c',
  'remote-viewer-connect.c',
  'remote-viewer-connect.h',
  'remote-viewer-main.c',
]

executable('remote-viewer',
           remote_viewer_sources,
           include_directories : virt_viewer_include,
           dependencies : virt_viewer_deps,
           link_with : [libvirt_viewer_util_lib, libvirt_viewer_lib],
           install : false)

# virt-viewer
virt_viewer_sources = [
  virt_viewer_enums[1],
  'virt-viewer.h',
  'virt-viewer.c',
  'virt-viewer-main.c',
]

if virt_viewer_has_libvirt
  executable('virt-viewer',
             virt_viewer_sources,
             include_directories : virt_viewer_include,
             dependencies : virt_viewer_deps,
             link_with : [libvirt_viewer_util_lib, libvirt_viewer_lib],
             install : true)
endif

# virt-viewer.rc
virt_viewer_rc = configuration_data()
virt_viewer_rc.set('VERSION', virt_viewer_version)
virt_viewer_rc.set('PACKAGE_STRING', '@0@ @1@'.format(virt_viewer_package_name, virt_viewer_version))
configure_file(input : 'virt-viewer.rc.in',
               output : 'virt-viewer.rc',
               configuration : virt_viewer_rc)
