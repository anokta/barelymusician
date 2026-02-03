/**
 * Message types to communicate with `barelymusician-processor`.
 * @enum {string}
 */
export const MessageType = {
  INIT_SUCCESS: 'init-success',

  ENGINE_SET_TEMPO: 'engine-set-tempo',
  ENGINE_SET_CONTROL: 'engine-set-control',
  ENGINE_UPDATE: 'engine-update',

  INSTRUMENT_CREATE: 'instrument-create',
  INSTRUMENT_CREATE_SUCCESS: 'instrument-create-success',
  INSTRUMENT_DESTROY: 'instrument-destroy',
  INSTRUMENT_DESTROY_SUCCESS: 'instrument-destroy-success',
  INSTRUMENT_ON_NOTE_ON: 'instrument-on-note-on',
  INSTRUMENT_ON_NOTE_OFF: 'instrument-on-note-off',
  INSTRUMENT_SET_ALL_NOTES_OFF: 'instrument-set-all-notes-off',
  INSTRUMENT_SET_CONTROL: 'instrument-set-control',
  INSTRUMENT_SET_NOTE_CONTROL: 'instrument-set-note-control',
  INSTRUMENT_SET_NOTE_ON: 'instrument-set-note-on',
  INSTRUMENT_SET_NOTE_OFF: 'instrument-set-note-off',
  INSTRUMENT_SET_SAMPLE_DATA: 'instrument-set-sample-data',

  PERFORMER_CREATE: 'performer-create',
  PERFORMER_CREATE_SUCCESS: 'performer-create-success',
  PERFORMER_DESTROY: 'performer-destroy',
  PERFORMER_DESTROY_SUCCESS: 'performer-destroy-success',
  PERFORMER_GET_PROPERTIES_SUCCESS: 'performer-get-properties-success',
  PERFORMER_SET_LOOPING: 'performer-set-looping',
  PERFORMER_SET_LOOP_BEGIN_POSITION: 'performer-set-loop-begin-position',
  PERFORMER_SET_LOOP_LENGTH: 'performer-set-loop-length',
  PERFORMER_SET_POSITION: 'performer-set-position',
  PERFORMER_START: 'performer-start',
  PERFORMER_STOP: 'performer-stop',

  TASK_CREATE: 'task-create',
  TASK_CREATE_SUCCESS: 'task-create-success',
  TASK_DESTROY: 'task-destroy',
  TASK_DESTROY_SUCCESS: 'task-destroy-success',
  TASK_GET_PROPERTIES_SUCCESS: 'task-get-properties-success',
  TASK_ON_EVENT: 'task-on-event',
  TASK_SET_DURATION: 'task-set-duration',
  TASK_SET_POSITION: 'task-set-position',
  TASK_SET_PRIORITY: 'task-set-priority',
};
