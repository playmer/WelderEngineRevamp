add_executable(ShaderCompiler main.cpp)


set_target_properties(ShaderCompiler PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/Data)


target_link_libraries(ShaderCompiler
PUBLIC
  Common
  ZilchShaders
  Geometry 
  Zilch
  Platform
  SpirvCross
  SpirvHeaders
  SpirvTools
)


target_compile_features(ShaderCompiler PUBLIC cxx_std_17)
target_compile_features(Common PUBLIC cxx_std_17)
target_compile_features(ZilchShaders PUBLIC cxx_std_17)
target_compile_features(Geometry PUBLIC cxx_std_17)
target_compile_features(Zilch PUBLIC cxx_std_17)
target_compile_features(Platform PUBLIC cxx_std_17)


target_compile_options(ShaderCompiler PUBLIC -permissive-)
target_compile_options(Common PUBLIC -permissive-)
target_compile_options(ZilchShaders PUBLIC -permissive-)
target_compile_options(Geometry PUBLIC -permissive-)
target_compile_options(Zilch PUBLIC -permissive-)
target_compile_options(Platform PUBLIC -permissive-)
