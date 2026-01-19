set(CMAKE_FOLDER platforms)

set(BARELY_EXPORTED_FUNCTIONS "['_malloc','_free',
  '_BarelyEngine_Create',
  '_BarelyEngine_CreateInstrument',
  '_BarelyEngine_CreateTask',
  '_BarelyEngine_CreatePerformer',
  '_BarelyEngine_Destroy',
  '_BarelyEngine_DestroyInstrument',
  '_BarelyEngine_DestroyPerformer',
  '_BarelyEngine_DestroyTask',
  '_BarelyEngine_Process',
  '_BarelyEngine_SetControl',
  '_BarelyEngine_SetTempo',
  '_BarelyEngine_Update',
  '_BarelyInstrument_SetAllNotesOff',
  '_BarelyInstrument_SetControl',
  '_BarelyInstrument_SetNoteControl',
  '_BarelyInstrument_SetNoteEventCallback',
  '_BarelyInstrument_SetNoteOff',
  '_BarelyInstrument_SetNoteOn',
  '_BarelyInstrument_SetSampleData',
  '_BarelyPerformer_GetPosition',
  '_BarelyPerformer_IsPlaying',
  '_BarelyPerformer_Start',
  '_BarelyPerformer_Stop',
  '_BarelyPerformer_SetLoopBeginPosition',
  '_BarelyPerformer_SetLoopLength',
  '_BarelyPerformer_SetLooping',
  '_BarelyPerformer_SetPosition',
  '_BarelyTask_IsActive',
  '_BarelyTask_SetDuration',
  '_BarelyTask_SetEventCallback',
  '_BarelyTask_SetPosition'
  ]"
)

add_executable(
  barelymusicianwasm
  wasm.cpp
)
set_target_properties(
  barelymusicianwasm PROPERTIES
  LINK_FLAGS " \
    -s ALLOW_TABLE_GROWTH=1 \
    -s ASSERTIONS=0 \
    -s ENVIRONMENT=web,worker \
    -s EXPORTED_FUNCTIONS=${BARELY_EXPORTED_FUNCTIONS} \
    -s EXPORTED_RUNTIME_METHODS=['HEAP32','HEAPF32','addFunction','getValue'] \
    -s EXPORT_ES6=1 \
    -s EXPORT_NAME=barelymusician \
    -s MODULARIZE=1 \
    -s NO_FILESYSTEM=1 \
    -s SINGLE_FILE=1 \
    -s WASM=1 \
    "
  OUTPUT_NAME "barelymusician"
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
target_compile_definitions(
  barelymusicianwasm PUBLIC
  BARELY_EXPORT
)
target_link_libraries(
  barelymusicianwasm PRIVATE
  barelymusician
)

function(package_barelymusicianwasm)
  add_custom_command(
    TARGET barelymusicianwasm POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:barelymusicianwasm> ${CMAKE_CURRENT_SOURCE_DIR}
  )
endfunction()
package_barelymusicianwasm()
