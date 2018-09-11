dirs = [
  '16x16',
  '22x22',
  '24x24',
  '32x32',
  '48x48',
  '256x256'
]

foreach d : dirs
  file = files('@0@/virt-viewer.png'.format(d))
  dir = join_paths(virt_viewer_datadir, 'icons', 'hicolor', d, 'apps')
  install_data(file, install_dir : dir)
endforeach
