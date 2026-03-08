@tool
extends EditorPlugin

func _enter_tree() -> void:
	print("BarelyMusician loaded")

func _exit_tree() -> void:
	print("BarelyMusician unloaded")
