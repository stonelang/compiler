add_clang_tool(clangf
  main.cpp
)

clang_target_link_libraries(clangf
  PRIVATE
  clangBasic
  )