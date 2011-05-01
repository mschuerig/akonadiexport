project(akonadiexport)

find_package (KdepimLibs REQUIRED)
include_directories( ${KDE4_INCLUDES} ${KDEPIMLIBS_INCLUDE_DIRS} ${QT_INCLUDES} )

set(akonadiexport_SRCS
  akonadiexport.cpp
  main.cpp
)

kde4_add_executable(akonadiexport ${akonadiexport_SRCS})

target_link_libraries(akonadiexport
  ${KDE4_KDEUI_LIBS}
  ${KDEPIMLIBS_AKONADI_LIBS}
)

install(TARGETS akonadiexport ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( PROGRAMS akonadiexport.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR} )
