find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 CONFIG REQUIRED)

message("Using Python: " ${Python_EXECUTABLE})

# pybind11 method:
pybind11_add_module(_bergamot SHARED bergamot.cpp)
target_link_libraries(_bergamot PUBLIC pybind11::module pybind11::headers bergamot-translator)
target_include_directories(_bergamot PUBLIC ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/src)
