set(stone_include_files
	${CMAKE_CURRENT_SOURCE_DIR}/include/clang/Compile/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/clang/Core/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/clang/CodeGeneration/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/clang/Frontend/Frontend.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/clang/Syntax/*.h
	
)
set(stone_lib_files
	${CMAKE_CURRENT_SOURCE_DIR}/lib/Compile/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/lib/Core/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/lib/CodeGeneration/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/lib/Frontend/Frontend.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/lib/Syntax/*.cpp
)
set(stone_tools_files
	${CMAKE_CURRENT_SOURCE_DIR}/tools/driver/*.cpp  
)

find_program(CLANG_FORMAT clang-format)
	if(CLANG_FORMAT)
	add_custom_target(
		clang-format
		clang-format
		-i
		-style=llvm
		${stone_lib_files}
		${stone_include_files}
		${stone_tools_files}
	)
endif()