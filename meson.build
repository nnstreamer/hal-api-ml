project('hal-api-ml', 'c', 'cpp',
  version: '1.0.0',
  license: ['Apache-2.0'],
  meson_version: '>=0.63.0',
  default_options: [
    'werror=true',
    'warning_level=1',
    'c_std=gnu89',
    'cpp_std=c++17'
  ]
)

# compilers
cc = meson.get_compiler('c')
cxx = meson.get_compiler('cpp')

# deps
glib_dep = dependency('glib-2.0')
compiler_flags_dep = declare_dependency(
    compile_args: ['-Wno-unused-parameter', '-Wno-unused-variable']
)

hal_api_nns_lib = shared_library('hal-api-ml',
  'hal-ml.cc',
  dependencies: [compiler_flags_dep, glib_dep],
  install: true,
  install_dir: join_paths(meson.current_source_dir(), 'to-be-installed', 'lib64')
)