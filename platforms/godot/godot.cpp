#include <barelymusician.h>

#include <memory>

#include "gdextension_interface.h"
#include "godot_cpp/classes/audio_server.hpp"
#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/object.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/error_macros.hpp"
#include "godot_cpp/core/method_bind.hpp"

using ::godot::AudioServer;
using ::godot::ClassDB;
using ::godot::D_METHOD;
using ::godot::GDExtensionBinding;
using ::godot::ModuleInitializationLevel;
using ::godot::Object;

class BarelyMusician : public Object {
 public:
  static void create() {
    if (const AudioServer* audio_server = AudioServer::get_singleton()) {
      BarelyEngine_Create(audio_server->get_mix_rate(), &engine_);
    }
  }

  static void destroy() {
    BarelyEngine_Destroy(engine_);
    engine_ = nullptr;
  }

  static BarelyEngine* get() {
    if (engine_ == nullptr) {
      create();
    }
    return engine_;
  }

  static double get_tempo() {
    double tempo = 0.0;
    BarelyEngine_GetTempo(get(), &tempo);
    return tempo;
  }

  static double get_timestamp() {
    double timestamp = 0.0;
    BarelyEngine_GetTimestamp(get(), &timestamp);
    return timestamp;
  }

  static void set_tempo(double tempo) { BarelyEngine_SetTempo(get(), tempo); }

  static void update(double timestamp) { BarelyEngine_Update(get(), timestamp); }

 private:
  GDCLASS(BarelyMusician, Object);

  static void _bind_methods() {
    ClassDB::bind_static_method(kClassName, D_METHOD("get_tempo"), &BarelyMusician::get_tempo);
    ClassDB::bind_static_method(kClassName, D_METHOD("get_timestamp"),
                                &BarelyMusician::get_timestamp);
    ClassDB::bind_static_method(kClassName, D_METHOD("set_tempo", "tempo"),
                                &BarelyMusician::set_tempo);
    ClassDB::bind_static_method(kClassName, D_METHOD("update", "timestamp"),
                                &BarelyMusician::update);
  }

  static constexpr const char* kClassName = "BarelyMusician";
  static inline BarelyEngine* engine_ = nullptr;  // singleton
};

void initialize_barelymusiciangodot(ModuleInitializationLevel p_level) {
  if (p_level != ModuleInitializationLevel::MODULE_INITIALIZATION_LEVEL_SCENE) {
    return;
  }
  ClassDB::register_class<BarelyMusician>();
  BarelyMusician::create();
}

void terminate_barelymusiciangodot(ModuleInitializationLevel p_level) {
  if (p_level != ModuleInitializationLevel::MODULE_INITIALIZATION_LEVEL_SCENE) {
    return;
  }
  BarelyMusician::destroy();
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
