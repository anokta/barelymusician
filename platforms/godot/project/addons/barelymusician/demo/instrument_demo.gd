extends Control

@export var instrument: BarelyInstrument
@export var musical_scale: BarelyScaleResource

@export var note_on_color: Color = Color.WHITE
@export var color_lerp_speed: float = 8.0

const OCTAVE_KEYS = ["A", "W", "S", "E", "D", "F", "T", "G", "Y", "H", "U", "J", "K"]
const MAX_OCTAVE_SHIFT = 4

const GRID_COUNT = 4

var gain = 1.0
var octave_shift = 0
var active_notes = {}

var alphas = []
var target_alphas = {}
var active_grids = {}


func _ready():
	BarelyEngine.lookahead = 0.0
	BarelyEngine.delay_time = 0.5
	BarelyEngine.delay_feedback = 0.2
	BarelyEngine.delay_ping_pong = 0.5
	BarelyEngine.reverb_room_size = 0.4

	instrument.note_on.connect(_on_note_on)
	instrument.note_off.connect(_on_note_off)

	for y in range(GRID_COUNT):
		alphas.append([])
		for x in range(GRID_COUNT):
			alphas[y].append(0.0)
			target_alphas[Vector2i(x, y)] = 0.0


func _process(delta):
	var should_redraw = false
	for y in range(GRID_COUNT):
		for x in range(GRID_COUNT):
			var prev_alpha = alphas[y][x]
			alphas[y][x] = lerpf(prev_alpha, target_alphas[Vector2i(x, y)], color_lerp_speed * delta)
			should_redraw = should_redraw || (alphas[y][x] != prev_alpha);
	if should_redraw:
		queue_redraw()


func _draw():
	var grid_width = size.x / GRID_COUNT
	var grid_height = size.y / GRID_COUNT
	var color = note_on_color
	for y in range(GRID_COUNT):
		for x in range(GRID_COUNT):
			color.a = alphas[y][x]
			draw_rect(Rect2(x * grid_width, y * grid_height, grid_width, grid_height), color)


func _input(event):
	if event is InputEventKey and event.pressed:
		if event.keycode == KEY_ESCAPE:
			get_tree().quit()
			return

		var key_string = OS.get_keycode_string(event.keycode).to_upper()
		if key_string == "Z":
			instrument.set_all_notes_off()
			octave_shift = max(octave_shift - 1, -MAX_OCTAVE_SHIFT)
			return
		elif key_string == "X":
			instrument.set_all_notes_off()
			octave_shift = min(octave_shift + 1, MAX_OCTAVE_SHIFT)
			return
		elif key_string == "C":
			gain = max(gain - 0.25, 0.0)
			return
		elif key_string == "V":
			gain = min(gain + 0.25, 1.0)
			return
		elif key_string == "1":
			instrument.osc_mix = 1.0
			return
		elif key_string == "2":
			instrument.osc_mix = 0.0
			return
		elif key_string == "3":
			instrument.osc_mix = 0.5
			return

		var key_index = OCTAVE_KEYS.find(key_string)
		if key_index != -1:
			var pitch = musical_scale.get_pitch(
				musical_scale.get_pitch_count() * octave_shift + key_index
			)

			if not active_notes.has(key_string):
				instrument.set_note_on(pitch, gain)
				active_notes[key_string] = pitch


func _unhandled_input(event):
	if event is InputEventKey and not event.pressed:
		var key_string = OS.get_keycode_string(event.keycode).to_upper()
		if active_notes.has(key_string):
			instrument.set_note_off(active_notes[key_string])
			active_notes.erase(key_string)


func _on_note_on(pitch):
	var i = int(round((pitch - octave_shift) * musical_scale.get_pitch_count()))
	var y = GRID_COUNT - i / GRID_COUNT - 1
	var x = i % GRID_COUNT
	if x < 0 or x >= GRID_COUNT or y < 0 or y >= GRID_COUNT:
		return

	var grid = Vector2i(x, y)
	target_alphas[grid] = 1.0
	active_grids[pitch] = grid


func _on_note_off(pitch):
	if not active_grids.has(pitch):
		return

	target_alphas[active_grids[pitch]] = 0.0
	active_grids.erase(pitch)
