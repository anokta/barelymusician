#include "gdextension_interface.h"
#include "godot/engine.h"
#include "godot/instrument.h"
#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/core/memory.hpp"

using ::godot::ClassDB;
using ::godot::GDExtensionBinding;
using ::godot::ModuleInitializationLevel;

void initialize_barelymusiciangodot(ModuleInitializationLevel level) {
  if (level != ModuleInitializationLevel::MODULE_INITIALIZATION_LEVEL_SCENE) {
    return;
  }
  ClassDB::register_internal_class<barely::godot::BarelyAudioStreamPlayback>();
  ClassDB::register_internal_class<barely::godot::BarelyEngineNode>();
  ClassDB::register_class<barely::godot::BarelyAudioStream>();
  ClassDB::register_class<barely::godot::BarelyEngine>();
  ClassDB::register_class<barely::godot::BarelyInstrument>();
  ::godot::Engine::get_singleton()->register_singleton("BarelyEngine",
                                                       memnew(barely::godot::BarelyEngine));
}

void terminate_barelymusiciangodot(ModuleInitializationLevel level) {
  if (level != ModuleInitializationLevel::MODULE_INITIALIZATION_LEVEL_SCENE) {
    return;
  }
}

extern "C" {

GDExtensionBool GDE_EXPORT barelymusiciangodot_init(
    GDExtensionInterfaceGetProcAddress p_get_proc_address,
    const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization* r_initialization) {
  GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);
  init_obj.register_initializer(initialize_barelymusiciangodot);
  init_obj.register_terminator(terminate_barelymusiciangodot);
  init_obj.set_minimum_library_initialization_level(
      ModuleInitializationLevel::MODULE_INITIALIZATION_LEVEL_SCENE);
  return init_obj.init();
}

}  // extern "C"
