extends Node

@onready var instrument: BarelyInstrument = $Instrument
@onready var metronome: BarelyPerformer = $Metronome
@onready var label: Label = $Label

@export_range(30.0, 960.0, 1) var tempo: float = 120.0
@export_range(1.0, 16.0, 1) var beat_count: float = 4

var _beat = -1

func _ready() -> void:
	var metronome_task = metronome.tasks[0]
	metronome_task.connect("task_begin", Callable(self, "_on_task_begin"))
	metronome_task.connect("task_end", Callable(self, "_on_task_end"))
	metronome.start()

func _input(event):
	if event is InputEventKey and event.pressed:
		if event.keycode == KEY_SPACE:
			if metronome.is_playing():
				metronome.stop()
			else:
				metronome.start()

func _process(_delta):
	BarelyEngine.tempo = tempo

func _on_task_begin():
	_beat = (_beat + 1) % int(beat_count)
	instrument.set_note_on(1.0 if _beat == 0 else 0.0)
	label.text = str(_beat + 1)

func _on_task_end():
	instrument.set_note_off(1.0 if _beat == 0 else 0.0)
