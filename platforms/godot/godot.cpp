#include <barelymusician.h>

#include <godot_cpp/classes/audio_server.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

class BarelyMusician : public Node {
  GDCLASS(BarelyMusician, Node)

 public:
  BarelyMusician() {
    if (const AudioServer* audio_server = AudioServer::get_singleton()) {
      BarelyEngine_Create(audio_server->get_mix_rate(), &engine);
    }
  }

  ~BarelyMusician() { BarelyEngine_Destroy(engine); }

  double get_tempo() const {
    double tempo = 0.0;
    BarelyEngine_GetTempo(engine, &tempo);
    return tempo;
  }

  void set_tempo(double tempo) { BarelyEngine_SetTempo(engine, tempo); }

 protected:
  static void _bind_methods();

 private:
  BarelyEngine* engine = nullptr;
};

void BarelyMusician::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_tempo"), &BarelyMusician::get_tempo);
  ClassDB::bind_method(D_METHOD("set_tempo", "tempo"), &BarelyMusician::set_tempo);
}

static void initialize_barelymusiciangodot_module(ModuleInitializationLevel p_level) {
  if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
    return;
  }
  ClassDB::register_class<BarelyMusician>();
}

extern "C" {

GDExtensionBool GDE_EXPORT barelymusiciangodot_library_init(
    GDExtensionInterfaceGetProcAddress p_get_proc_address,
    const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization* r_initialization) {
  godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);
  init_obj.register_initializer(initialize_barelymusiciangodot_module);
  return init_obj.init();
}

}  // extern "C"
