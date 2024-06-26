set(LLVM_LINK_COMPONENTS
  Support
  TargetParser
  FrontendOpenMP
)

add_clang_library(clangBasic
DiagnosticEngine.cpp    
FileManager.cpp         
SrcMgr.cpp
CharInfo.cpp            
FileEntry.cpp          
FileSystemStatCache.cpp 
SrcMgrAdapter.cpp

)

