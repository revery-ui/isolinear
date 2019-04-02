let create =
    (~initialState: 'state, ~updater: Updater.t('actions, 'state), ()) => {
  let state = ref(initialState);

  let (stream, dispatch) = Stream.create();

  let dispatch = action => {
    let currentState = state^;
    let (newState, effect) = updater(currentState, action);
    state := newState;
    dispatch((newState, action));

    (newState, effect);
  };

  (dispatch, stream);
};
