set( proj Elastix )

ExternalProject_Add( ${proj}
  DOWNLOAD_COMMAND ""
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..
  BINARY_DIR ${proj}-build
  CMAKE_ARGS
    --no-warn-unused-cli
    -DELASTIX_BUILD_TESTING:BOOL=${ELASTIX_BUILD_TESTING}
    -DELASTIX_BUILD_BENCHMARKING:BOOL=${ELASTIX_BUILD_BENCHMARKING}
    -DELASTIX_BUILD_DASHBOARD:BOOL=${ELASTIX_BUILD_DASHBOARD}
    -DITK_DIR:PATH=${ITK_DIR}
  DEPENDS ${ELASTIX_DEPENDENCIES}
  INSTALL_COMMAND ""
)