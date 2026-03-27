extends Node

@onready var instrument: BarelyInstrument = $Instrument

const OCTAVE_KEYS = ['A', 'W', 'S', 'E', 'D', 'F', 'T', 'G', 'Y', 'H', 'U', 'J', 'K']
const ROOT_PITCH = 0.0
const MAX_OCTAVE_SHIFT = 4

var gain = 1.0
var octave_shift = 0
var active_notes = {}

func _ready():
	BarelyEngine.delay_time = 0.5
	BarelyEngine.delay_feedback = 0.2
	BarelyEngine.delay_ping_pong = 0.5
	BarelyEngine.reverb_room_size = 0.4

	instrument.note_on.connect(_on_note_on)
	instrument.note_off.connect(_on_note_off)

func _input(event):
	if event is InputEventKey and event.pressed:
		if event.keycode == KEY_ESCAPE:
			get_tree().quit()
			return

		var key_string = OS.get_keycode_string(event.keycode).to_upper()
		if key_string == 'Z':
			instrument.set_all_notes_off()
			octave_shift = max(octave_shift - 1, -MAX_OCTAVE_SHIFT)
			return
		elif key_string == 'X':
			instrument.set_all_notes_off()
			octave_shift = min(octave_shift + 1, MAX_OCTAVE_SHIFT)
			return
		elif key_string == 'C':
			gain = max(gain - 0.25, 0.0)
			return
		elif key_string == 'V':
			gain = min(gain + 0.25, 1.0)
			return
		elif key_string == '1':
			instrument.osc_mix = 1.0
			return
		elif key_string == '2':
			instrument.osc_mix = 0.0
			return
		elif key_string == '3':
			instrument.osc_mix = 0.5
			return
		elif key_string == '0':
			instrument.arp_mode = instrument.ARP_MODE_UP if instrument.arp_mode == instrument.ARP_MODE_NONE else instrument.ARP_MODE_NONE
			return

		var key_index = OCTAVE_KEYS.find(key_string)
		if key_index != -1:
			var pitch = ROOT_PITCH + octave_shift + (key_index / 12.0)
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
	print("NoteOn(%.2f)\n" % pitch)

func _on_note_off(pitch):
	print("NoteOff(%.2f)\n" % pitch)
