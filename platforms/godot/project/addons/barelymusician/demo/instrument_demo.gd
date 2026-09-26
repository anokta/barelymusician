extends Control

const OCTAVE_KEYS = ["A", "W", "S", "E", "D", "F", "T", "G", "Y", "H", "U", "J", "K"]
const MAX_OCTAVE_SHIFT = 4

const GRID_COUNT = 4

const MOUSE_ID = "mouse"

@export var instrument: BarelyInstrument
@export var musical_scale: BarelyScaleResource

@export var note_on_color: Color = Color.WHITE
@export var color_lerp_speed: float = 8.0

var _gain = 1.0
var _octave_shift = 0

var _active_notes = {}
var _active_note_counts = {}
var _active_pointers = {}

var _alphas = []
var _target_alphas = {}
var _active_grids = {}


func _ready():
  BarelyEngine.lookahead = 0.0
  BarelyEngine.delay_time = 0.5
  BarelyEngine.delay_feedback = 0.2
  BarelyEngine.delay_ping_pong = 0.5
  BarelyEngine.reverb_room_size = 0.4

  instrument.note_on.connect(_on_note_on)
  instrument.note_off.connect(_on_note_off)

  for y in range(GRID_COUNT):
    _alphas.append([])
    for x in range(GRID_COUNT):
      _alphas[y].append(0.0)
      _target_alphas[Vector2i(x, y)] = 0.0


func _process(delta):
  var should_redraw = false
  for y in range(GRID_COUNT):
    for x in range(GRID_COUNT):
      var prev_alpha = _alphas[y][x]
      _alphas[y][x] = lerpf(prev_alpha, _target_alphas[Vector2i(x, y)], color_lerp_speed * delta)
      should_redraw = should_redraw || (_alphas[y][x] != prev_alpha)
  if should_redraw:
    queue_redraw()


func _draw():
  var grid_width = size.x / GRID_COUNT
  var grid_height = size.y / GRID_COUNT
  var color = note_on_color
  for y in range(GRID_COUNT):
    for x in range(GRID_COUNT):
      color.a = _alphas[y][x]
      draw_rect(Rect2(x * grid_width, y * grid_height, grid_width, grid_height), color)


func _input(event):
  if event is InputEventKey and event.pressed:
    if event.keycode == KEY_ESCAPE:
      if not OS.has_feature("web"):
        get_tree().quit()
      return

    var key_string = OS.get_keycode_string(event.keycode).to_upper()
    if key_string == "Z":
      instrument.set_all_notes_off()
      _active_notes.clear()
      _active_note_counts.clear()
      _active_pointers.clear()
      _octave_shift = max(_octave_shift - 1, -MAX_OCTAVE_SHIFT)
    elif key_string == "X":
      instrument.set_all_notes_off()
      _active_notes.clear()
      _active_note_counts.clear()
      _active_pointers.clear()
      _octave_shift = min(_octave_shift + 1, MAX_OCTAVE_SHIFT)
    elif key_string == "C":
      _gain = max(_gain - 0.25, 0.0)
    elif key_string == "V":
      _gain = min(_gain + 0.25, 1.0)
    elif key_string == "1":
      instrument.osc_mix = 1.0
    elif key_string == "2":
      instrument.osc_mix = 0.0
    elif key_string == "3":
      instrument.osc_mix = 0.5
    else:
      var key_index = OCTAVE_KEYS.find(key_string)
      if key_index != -1:
        var pitch = musical_scale.get_pitch(
          musical_scale.get_pitch_count() * _octave_shift + key_index
        )
        if not _active_notes.has(key_string):
          _start_note(key_string, pitch)
  elif event is InputEventScreenTouch:
    var touch_id = "touch:%d:%d" % [event.device, event.index]
    if event.pressed:
      var pitch = _screen_position_to_pitch(event.position)
      if pitch != null:
        _active_pointers[touch_id] = true
        _update_pointer_note(touch_id, pitch)
    elif _active_pointers.has(touch_id):
      _active_pointers.erase(touch_id)
      _stop_note(touch_id)
  elif event is InputEventScreenDrag:
    var touch_id = "touch:%d:%d" % [event.device, event.index]
    if _active_pointers.has(touch_id):
      _update_pointer_note(touch_id, _screen_position_to_pitch(event.position))
  elif event is InputEventMouseButton and event.button_index == MOUSE_BUTTON_LEFT:
    if event.pressed:
      var pitch = _screen_position_to_pitch(event.position)
      if pitch != null:
        _active_pointers[MOUSE_ID] = true
        _update_pointer_note(MOUSE_ID, pitch)
    elif _active_pointers.has(MOUSE_ID):
      _active_pointers.erase(MOUSE_ID)
      _stop_note(MOUSE_ID)
  elif event is InputEventMouseMotion and _active_pointers.has(MOUSE_ID):
    if (event.button_mask & MOUSE_BUTTON_MASK_LEFT) != 0:
      _update_pointer_note(MOUSE_ID, _screen_position_to_pitch(event.position))


func _unhandled_input(event):
  if event is InputEventKey and not event.pressed:
    var key_string = OS.get_keycode_string(event.keycode).to_upper()
    if _active_notes.has(key_string):
      _stop_note(key_string)


func _screen_position_to_pitch(screen_position):
  var local_position = get_global_transform_with_canvas().affine_inverse() * screen_position
  if not Rect2(Vector2.ZERO, size).has_point(local_position):
    return null

  var grid_width = size.x / GRID_COUNT
  var grid_height = size.y / GRID_COUNT
  var grid_x = int(local_position.x / grid_width)
  var grid_y_from_bottom = GRID_COUNT - 1 - int(local_position.y / grid_height)
  var note_index = grid_y_from_bottom * GRID_COUNT + grid_x
  return musical_scale.get_pitch(musical_scale.get_pitch_count() * _octave_shift + note_index)


func _update_pointer_note(pointer_id, pitch):
  if _active_notes.has(pointer_id):
    if _active_notes[pointer_id] == pitch:
      return
    _stop_note(pointer_id)
  if pitch != null:
    _start_note(pointer_id, pitch)


func _start_note(note_id, pitch):
  if _active_notes.has(note_id):
    return
  var note_count = _active_note_counts.get(pitch, 0)
  if note_count == 0:
    instrument.set_note_on(pitch, _gain)
  _active_note_counts[pitch] = note_count + 1
  _active_notes[note_id] = pitch


func _stop_note(note_id):
  if not _active_notes.has(note_id):
    return
  var pitch = _active_notes[note_id]
  var note_count = _active_note_counts[pitch]
  if note_count > 1:
    _active_note_counts[pitch] = note_count - 1
  else:
    instrument.set_note_off(pitch)
    _active_note_counts.erase(pitch)
  _active_notes.erase(note_id)


func _on_note_on(pitch):
  var i = int(round((pitch - _octave_shift) * musical_scale.get_pitch_count()))
  var y = GRID_COUNT - i / GRID_COUNT - 1
  var x = i % GRID_COUNT
  if x < 0 or x >= GRID_COUNT or y < 0 or y >= GRID_COUNT:
    return
  var grid = Vector2i(x, y)
  _target_alphas[grid] = 1.0
  _active_grids[pitch] = grid


func _on_note_off(pitch):
  if not _active_grids.has(pitch):
    return
  _target_alphas[_active_grids[pitch]] = 0.0
  _active_grids.erase(pitch)
