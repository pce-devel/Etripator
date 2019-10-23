add_library(argparse STATIC argparse/argparse.c argparse/argparse.h)
target_compile_options(argparse PUBLIC "-Wextra")