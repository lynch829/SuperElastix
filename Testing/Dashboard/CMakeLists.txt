# ---------------------------------------------------------------------
# Dashboard configuration

mark_as_advanced( ELASTIX_DASHBOARD_SOURCE_DIR )
set( ELASTIX_DASHBOARD_SOURCE_DIR "${CMAKE_SOURCE_DIR}/Testing/Dashboard" )

mark_as_advanced( ELASTIX_DASHBOARD_BINARY_DIR )
set( ELASTIX_DASHBOARD_BINARY_DIR "${CMAKE_BINARY_DIR}/Testing/Dashboard" )

mark_as_advanced( ELASTIX_DASHBOARD_LOGSTASH_CERTIFICATE )
set( ELASTIX_DASHBOARD_LOGSTASH_CERTIFICATE "elxLogstash.crt" )

mark_as_advanced( ELASTIX_DASHBOARD_LOGSTASH_RSA_PRIVATE_KEY )
set( ELASTIX_DASHBOARD_LOGSTASH_RSA_PRIVATE_KEY "elxLogstash.key" )

mark_as_advanced( ELASTIX_DASHBOARD_VOLUME )
if( NOT EXISTS "${ELASTIX_DASHBOARD_VOLUME}" )
  message( STATUS
    "ELASTIX_DASHBOARD_VOLUME points to the folder ${CMAKE_BINARY_DIR}/Testing/Dashboard/Data in your build directory. In production you should point this variable to a safe directory outside the build tree."
  )
  
  set( ELASTIX_DASHBOARD_VOLUME "${ELASTIX_DASHBOARD_BINARY_DIR}/Data" )
  file( MAKE_DIRECTORY "${ELASTIX_DASHBOARD_VOLUME}" )
endif()

# Test/Benchmark results will be forwarded to this IP. You will probably
# have change this in order to match docker settings on your own machine. 
mark_as_advanced( ELASTIX_DASHBOARD_IP )
set( ELASTIX_DASHBOARD_IP "192.168.59.103" )

configure_file(
  ${ELASTIX_DASHBOARD_SOURCE_DIR}/elxDashboard.yml.in
  ${ELASTIX_DASHBOARD_BINARY_DIR}/elxDashboard.yml
)

configure_file(
  ${ELASTIX_DASHBOARD_SOURCE_DIR}/elxLogstashForwarder.yml.in
  ${ELASTIX_DASHBOARD_BINARY_DIR}/elxLogstashForwarder.yml
)

configure_file(
  ${ELASTIX_DASHBOARD_SOURCE_DIR}/elxLogstash.conf.in
  ${ELASTIX_DASHBOARD_BINARY_DIR}/Logstash/elxLogstash.conf
)

configure_file(
  ${ELASTIX_DASHBOARD_SOURCE_DIR}/elxLogstashForwarder.json.in
  ${ELASTIX_DASHBOARD_BINARY_DIR}/elxLogstashForwarder.json
)