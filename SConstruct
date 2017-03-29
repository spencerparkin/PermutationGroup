# SConstruct for PermutationGroup library

import os

obj_env = Environment()
obj_env.Append( CCFLAGS = '--std=c++11' )
obj_env.Append( CCFLAGS = '-DLINUX' )
obj_env.Append( CCFLAGS = '-I../rapidjson/include' )
obj_env.Append( CCFLAGS = '-I../../rapidjson/include' )
#obj_env.Append( CCFLAGS = '-ggdb' )

cpp_source_list = Glob( 'Code/*.cpp' )
h_source_list = Glob( 'Code/*.h' )
source_list = cpp_source_list + h_source_list

object_list = []
for source_file in cpp_source_list:
  object_file = obj_env.StaticObject( source_file )
  object_list.append( object_file )

lib_env = Environment()
lib = lib_env.StaticLibrary( 'PermutationGroup', object_list )

dest_dir = '/usr'
if 'DESTDIR' in os.environ:
  dest_dir = os.environ[ 'DESTDIR' ]

install_env = Environment(
	INCL = dest_dir + '/include',
  LIB = dest_dir + '/lib',
  BIN = dest_dir + '/bin',
  SHARE = dest_dir + '/share' )

install_env.Install( '$INCL', h_source_list )
install_env.Install( '$LIB', lib )
install_env.Alias( 'install', [ '$LIB', '$INCL' ] )

