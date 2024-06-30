set(stone_include_files
	${CMAKE_CURRENT_SOURCE_DIR}/include/clang/Basic/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/clang/Compile/*.h
	
	
)
set(stone_lib_files
	${CMAKE_CURRENT_SOURCE_DIR}/lib/Basic/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/lib/Compile/*.cpp
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
	)
endif()