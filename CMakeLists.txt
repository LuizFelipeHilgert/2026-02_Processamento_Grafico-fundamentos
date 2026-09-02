cmake_minimum_required(VERSION 3.11)
project(PG2026-2)

# Define o padrão do C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Ativa o FetchContent
include(FetchContent)

# Configura o FetchContent para baixar e compilar GLFW automaticamente
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG master
)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG master
)
FetchContent_MakeAvailable(glm)

# Configura o FetchContent para baixar a stb_image automaticamente
FetchContent_Declare(
  stb_image
  GIT_REPOSITORY https://github.com/nothings/stb.git
  GIT_TAG master
)
FetchContent_MakeAvailable(stb_image)

# Adiciona as pastas de cabeçalhos
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/common)
include_directories(${CMAKE_SOURCE_DIR}/include/glad)
include_directories(${glm_SOURCE_DIR})
include_directories(${stb_image_SOURCE_DIR})

add_compile_options(-Wno-pragmas)

# Define as bibliotecas para cada sistema operacional
if(WIN32)
    set(OPENGL_LIBS opengl32)

elseif(APPLE)
    find_library(OpenGL_LIBRARY OpenGL)
    set(OPENGL_LIBS ${OpenGL_LIBRARY})

else()
    find_package(OpenGL REQUIRED)
    set(OPENGL_LIBS ${OPENGL_gl_LIBRARY})
endif()

# Caminho esperado para a GLAD
set(GLAD_C_FILE "${CMAKE_SOURCE_DIR}/common/glad.c")

# Verifica se o arquivo da GLAD existe
if(NOT EXISTS ${GLAD_C_FILE})
    message(FATAL_ERROR
        "Arquivo glad.c não encontrado!
        Coloque glad.h em include/glad/
        e glad.c em common/"
    )
endif()

# Cria um executável para cada arquivo .cpp que possui uma função main().
# Isso permite manter várias soluções na mesma pasta sem juntar vários main() no mesmo programa.
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/Exemplos/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/Exercicios/*.cpp"
)

foreach(SOURCE_FILE ${PROJECT_SOURCES})
    file(READ "${SOURCE_FILE}" SOURCE_CONTENT)
    string(FIND "${SOURCE_CONTENT}" "int main(" MAIN_POSITION)

    # Arquivos vazios ou auxiliares sem main() não geram executável.
    if(MAIN_POSITION EQUAL -1)
        continue()
    endif()

    # Exemplo: src/Exercicios/Lista1/Ex1/a.cpp -> Lista1_Ex1_a
    file(RELATIVE_PATH RELATIVE_SOURCE "${CMAKE_SOURCE_DIR}/src" "${SOURCE_FILE}")
    string(REPLACE "/" "_" TARGET_NAME "${RELATIVE_SOURCE}")
    string(REPLACE ".cpp" "" TARGET_NAME "${TARGET_NAME}")

    add_executable(${TARGET_NAME} "${SOURCE_FILE}" ${GLAD_C_FILE})

    target_include_directories(${TARGET_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/include/glad
        ${glm_SOURCE_DIR}
        ${stb_image_SOURCE_DIR}
    )

    target_link_libraries(${TARGET_NAME} PRIVATE
        glfw
        ${OPENGL_LIBS}
        glm::glm
    )
endforeach()
