extends Control

@export var instrument: BarelyInstrument
@export var metronome: BarelyPerformer
@export var label: Label

@export var lookahead : float = 0.2
@export_range(30.0, 960.0, 1) var bpm: float = 120.0
@export_range(1.0, 16.0, 1) var beat_count: float = 4

var _beat = -1


func _ready() -> void:
	BarelyEngine.lookahead = lookahead

	var metronome_task = metronome.tasks[0]
	metronome_task.connect("task_begin", Callable(self, "_on_task_begin"))
	metronome_task.connect("task_end", Callable(self, "_on_task_end"))
	metronome.start()


func _process(_delta):
	BarelyEngine.speed = bpm / 60.0


func _input(event):
	if event is InputEventKey and event.pressed:
		if event.keycode == KEY_ESCAPE:
			get_tree().quit()
			return
		if event.keycode == KEY_SPACE:
			if metronome.is_playing():
				metronome.stop()
				print("Metronome paused")
			else:
				metronome.start()
				print("Metronome resumed")


func _on_task_begin():
	_beat = (_beat + 1) % int(beat_count)
	instrument.set_note_on(1.0 if _beat == 0 else 0.0)
	label.text = str(_beat + 1)


func _on_task_end():
	instrument.set_note_off(1.0 if _beat == 0 else 0.0)
