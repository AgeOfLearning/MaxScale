add_library(mysqlmon SHARED mysql_mon.c monitor_common.c)
target_link_libraries(mysqlmon log_manager utils)
install(TARGETS mysqlmon DESTINATION ${MAXSCALE_LIBDIR})

add_library(galeramon SHARED galeramon.c  monitor_common.c)
target_link_libraries(galeramon log_manager utils)
install(TARGETS galeramon DESTINATION ${MAXSCALE_LIBDIR})

add_library(ndbclustermon SHARED ndbclustermon.c  monitor_common.c)
target_link_libraries(ndbclustermon log_manager utils)
install(TARGETS ndbclustermon DESTINATION ${MAXSCALE_LIBDIR})
if(BUILD_MMMON)
  add_library(mmmon SHARED mmmon.c  monitor_common.c)
  target_link_libraries(mmmon log_manager utils)
  install(TARGETS mmmon DESTINATION ${MAXSCALE_LIBDIR})
endif()
