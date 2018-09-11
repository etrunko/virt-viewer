intltool = find_program('intltool-merge')
podir = join_paths(meson.source_root(), 'po')

targets = {'remote-viewer.desktop' : 'applications',
           'remote-viewer.appdata.xml' : 'appdata',
           'virt-viewer-mime.xml' : 'mime/packages'}

foreach target, dir : targets
  style = target.split('.')[-1]
  custom_target(target,
                input : '@0@.in'.format(target),
                output : target,
                command : [intltool, '--@0@-style'.format(style), podir, '@INPUT@', '@OUTPUT@'],
                install : true,
                install_dir : join_paths(virt_viewer_datadir, dir))
endforeach
