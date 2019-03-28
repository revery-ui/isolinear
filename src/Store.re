let create =
    (~initialState: 'state, ~updater: Updater.t('actions, 'state), ()) => {
  let state = ref(initialState);

  let dispatch = action => {
    let currentState = state^;
    let (newState, effect) = updater(currentState, action);
    state := newState;
    (newState, effect);
  };

  let getState = () => {
    state^;
  };

  (dispatch, getState);
};
