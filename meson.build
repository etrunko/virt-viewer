project('virt-viewer', 'c',
        version : '8.0',
        license : 'GPLv2',
        meson_version : '>= 0.47.0')

#
# globals
#
virt_viewer_deps = []
virt_viewer_include = [include_directories('.')]
virt_viewer_config_data = configuration_data()
virt_viewer_package_name = meson.project_name()
virt_viewer_version = meson.project_version()
virt_viewer_datadir = join_paths(get_option('prefix'), get_option('datadir'))
virt_viewer_localedir = join_paths(virt_viewer_datadir, 'locale')

virt_viewer_cflags = ['-DG_LOG_DOMAIN="@0@"'.format(virt_viewer_package_name),
                      '-Wall',
                      '-Wmissing-prototypes',
                      '-std=c99',
                      '-Wnested-externs',
                      '-Wpointer-arith',
                      '-Wextra',
                      '-Wshadow',
                      '-Wcast-align',
                      '-Wwrite-strings',
                      '-Waggregate-return',
                      '-Winline',
                      '-Wredundant-decls',
                      '-Wdeprecated-declarations',
                      '-Wno-sign-compare',
                      '-Wno-cast-function-type',
                      '-Werror']

compiler = meson.get_compiler('c')

# dependency versions
glib2_version = '2.38'
glib2_required = '>= @0@'.format(glib2_version)
gtk_version = '3.18'
gtk_required = '>= @0@'.format(gtk_version)
libxml2_required = '>= 2.6.0'
libvirt_required = '>= 0.10.0'
libvirt_glib_required = '>= 0.1.8'
gtk_vnc_required = '>= 0.4.0'
spice_gtk_required = '>= 0.35'
spice_protocol_required = '>= 0.12.7'
govirt_required = '>= 0.3.3'
rest_required = '>= 0.8'

#
# check system headers
#
headers = ['sys/socket.h', 'sys/un.h', 'windows.h']
foreach header : headers
  if compiler.has_header(header)
    virt_viewer_config_data.set('HAVE_@0@'.format(header.underscorify().to_upper()), '1')
  endif
endforeach

#
# check system functions
#
functions = ['fork', 'socketpair']
foreach function : functions
  if compiler.has_function(function)
    virt_viewer_config_data.set('HAVE_@0@'.format(function.underscorify().to_upper()), '1')
  endif
endforeach

#
# Check for dependencies
#
virt_viewer_deps = [compiler.find_library('m')]

# mandatory deps
deps = {'glib-2.0' : glib2_required,
        'gio-2.0' : glib2_required,
        'gthread-2.0' : glib2_required,
        'gmodule-export-2.0': glib2_required,
        'gtk+-3.0' : gtk_required,
        'libxml-2.0' : libxml2_required}

foreach dep, version : deps
  virt_viewer_deps += dependency(dep, version : version)
endforeach

glib2_encoded_version = 'GLIB_VERSION_@0@'.format(glib2_version.underscorify())
gtk_encoded_version = 'GDK_VERSION_@0@'.format(gtk_version.underscorify())

virt_viewer_cflags += ['-DGLIB_VERSION_MIN_REQUIRED=@0@'.format(glib2_encoded_version),
                       '-DGLIB_VERSION_MAX_ALLOWED=@0@'.format(glib2_encoded_version),
                       '-DGDK_VERSION_MIN_REQUIRED=@0@'.format(gtk_encoded_version),
                       '-DGDK_VERSION_MAX_ALLOWED=@0@'.format(gtk_encoded_version)]

# optional deps

# libvirt
virt_viewer_has_libvirt = false

libvirt_dep = dependency('libvirt', version : libvirt_required, required : get_option('libvirt'))
libvirt_glib_dep = dependency('libvirt-glib-1.0', version : libvirt_glib_required, required : get_option('libvirt'))

if libvirt_dep.found() and libvirt_glib_dep.found()
  virt_viewer_deps += [libvirt_dep, libvirt_glib_dep]

  if compiler.has_function('virDomainOpenGraphicsFD', dependencies : libvirt_dep)
    virt_viewer_config_data.set('HAVE_VIR_DOMAIN_OPEN_GRAPHICS_FD', '1')
  endif

  virt_viewer_config_data.set('HAVE_LIBVIRT', '1')
  virt_viewer_has_libvirt = true
endif

# gtk-vnc
virt_viewer_has_gtk_vnc = false
gtk_vnc_dep = dependency('gtk-vnc-2.0', version : gtk_vnc_required, required : get_option('gtk-vnc'))

if gtk_vnc_dep.found()
  virt_viewer_deps += gtk_vnc_dep
  virt_viewer_config_data.set('HAVE_GTK_VNC', '1')
  virt_viewer_has_gtk_vnc = true
endif

# spice-gtk
virt_viewer_has_spice_gtk = false
spice_protocol_dep = dependency('#spice-protocol', version : spice_protocol_required, required : get_option('spice-gtk'), fallback : ['spice-single', 'spice_protocol_dep'])
spice_glib_dep = dependency('#spice-client-glib-2.0', version : spice_gtk_required, required : get_option('spice-gtk'), fallback : ['spice-single', 'spice_client_glib_dep'])
spice_gtk_dep = dependency('#spice-client-gtk-3.0', version : spice_gtk_required, required : get_option('spice-gtk'), fallback : ['spice-single', 'spice_client_gtk_dep'])

if spice_protocol_dep.found() and spice_glib_dep.found() and spice_gtk_dep.found()
  virt_viewer_deps += [spice_protocol_dep, spice_glib_dep, spice_gtk_dep]
  virt_viewer_config_data.set('HAVE_SPICE_GTK', '1')
  virt_viewer_has_spice_gtk = true
endif

# ovirt
virt_viewer_has_ovirt = false
rest_dep = dependency('rest-0.7', version : rest_required, required : get_option('ovirt'))
ovirt_dep = dependency('govirt-1.0', version : govirt_required, required: get_option('ovirt'))

if rest_dep.found() and ovirt_dep.found()
  virt_viewer_deps += [rest_dep, ovirt_dep]

  if (compiler.has_function('ovirt_api_search_vms', dependencies : ovirt_dep) and
      compiler.has_function('ovirt_vm_get_host', dependencies : ovirt_dep) and
      compiler.has_function('ovirt_host_get_cluster', dependencies : ovirt_dep) and
      compiler.has_function('ovirt_cluster_get_data_center', dependencies : ovirt_dep))
    virt_viewer_config_data.set('HAVE_OVIRT_DATA_CENTER', '1')
  endif

  virt_viewer_config_data.set('HAVE_OVIRT', '1')
  virt_viewer_has_ovirt = true
endif

osid = get_option('osid')
if osid.strip() != ''
  virt_viewer_config_data.set_quoted('REMOTE_VIEWER_OS_ID', osid)
endif

buildid = get_option('buildid')
if buildid.strip() == '0'
  buildid = ''
else
  buildid = '-@0@'.format(buildid)
endif

virt_viewer_config_data.set_quoted('BUILDID', '@0@'.format(buildid))

#
# global C defines
#
add_project_arguments(compiler.get_supported_arguments(virt_viewer_cflags),
                      language : 'c')

#
# Subdirectories
#
subdir('src')
subdir('tests')
subdir('icons')
subdir('po')
subdir('data')
subdir('man')

#
# Write config.h
#
config_data = {'PACKAGE' : virt_viewer_package_name,
               'GETTEXT_PACKAGE' : virt_viewer_package_name,
               'VERSION' : virt_viewer_version,
               'PACKAGE_VERSION' : virt_viewer_version,
               'LOCALE_DIR' : virt_viewer_localedir}

foreach key, value : config_data
  virt_viewer_config_data.set_quoted(key, value)
endforeach

virt_viewer_config_data.set('_GNU_SOURCE', '')

configure_file(output : 'config.h',
               install : false,
               configuration : virt_viewer_config_data)
