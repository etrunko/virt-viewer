tests = {
  'test-version-compare' : [],
  'test-monitor-mapping' : [],
  'test-hotkeys' : [libvirt_viewer_lib],
  'test-monitor-alignment' : [],
}

foreach t, libs : tests
  test(t,
       executable(t,
                  ['@0@.c'.format(t), virt_viewer_enums[1]],
                  include_directories : virt_viewer_include,
                  dependencies : virt_viewer_deps,
                  link_with : [libvirt_viewer_util_lib, libs],
                  install : false))
endforeach
