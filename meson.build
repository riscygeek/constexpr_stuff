project(
    'constexpr_stuff',
    'cpp',
    version: '0.1',
    default_options: [
        'cpp_std=c++20',
        'warning_level=3',
    ]
)

includes = include_directories('include')

executable(
    'test-cexpr',
    'src/test.cpp',
    include_directories: includes,
    install: true,
)