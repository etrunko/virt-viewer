pod2man = find_program('pod2man')
manuals = ['remote-viewer', 'virt-viewer']

if pod2man.found()
  foreach m : manuals
      man = '@0@.1'.format(m)
      pod = '@0@.pod'.format(m)
      custom_target(man,
                    output : man,
                    input : pod,
                    install : true,
                    install_dir : join_paths(virt_viewer_datadir, 'man', 'man1'),
                    build_by_default : true,
                    command : [pod2man, '-c', 'Virtualization Support', '@INPUT@', '@OUTPUT@'])
  endforeach
endif
